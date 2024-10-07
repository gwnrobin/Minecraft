#include "WorldGenerator.h"
#include "Chunk.h"
#include "MathUtil.h"
#include "PNoise.h"
#include "VoxelData.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWorldGenerator::AWorldGenerator()
{
	Chunks.SetNum(FVoxelData::WorldSizeInChunks);

	// Resize each inner array and initialize
	for (int32 i = 0; i < FVoxelData::WorldSizeInChunks; ++i)
	{
		Chunks[i].SetNum(FVoxelData::WorldSizeInChunks);
	}
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	FRandomStream RandomStream;
	RandomStream.Initialize(Seed);
	
	GenerateWorld();
	
	Player = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// Spawning the player in the center of the map, ensuring that X, Y, Z components are separated
	SpawnPosition = FVector(FVoxelData::WorldSizeInVoxels / 2 * 100, // X
							FVoxelData::WorldSizeInVoxels / 2 * 100, // Y
							(FVoxelData::ChunkHeight + 3) * 100);   // Z

	
	Player->SetActorLocation(SpawnPosition);

	PlayerLastChunkCoord = GetChunkCoordFromVector(Player->GetActorLocation());
}


void AWorldGenerator::GenerateWorld()
{
	for (int x = FVoxelData::WorldSizeInChunks / 2 - FVoxelData::ViewDistanceInChunks / 2; x < FVoxelData::WorldSizeInChunks / 2 + FVoxelData::ViewDistanceInChunks / 2; x++)
	{
		for (int y = FVoxelData::WorldSizeInChunks / 2 - FVoxelData::ViewDistanceInChunks / 2; y < FVoxelData::WorldSizeInChunks / 2 + FVoxelData::ViewDistanceInChunks / 2; y++)
		{
			GenerateChunk(FVector2d(x,y));
		}
	}
}

int AWorldGenerator::GetVoxel(FVector Pos)
{
	int ZPos = FMath::FloorToInt(Pos.Z);
	
	if(!IsVoxelInWorld(Pos))
		return 0;
	
	if(ZPos == 0)
		return 1; //bedrock;

	int TerrainHeight = FMath::FloorToInt(Biome->TerrainHeight * FPNoise::GetPerlinNoise(FVector2D(Pos.X, Pos.Y), 0, Biome->TerrainScale)) + Biome->SolidGroundHeight;
	int VoxelValue = 0;
	
	if(ZPos == TerrainHeight)
		VoxelValue = 3;
	else if(ZPos < TerrainHeight && ZPos > TerrainHeight - 4)
		VoxelValue = 5;
	else if(ZPos > TerrainHeight)
		return 0;
	else
		VoxelValue = 2;

	if(VoxelValue == 2)
	{
		for (auto Lode : Biome->Lodes)
		{
			if(ZPos > Lode.MinHeight && ZPos < Lode.MaxHeight)
			{
				if(FPNoise::GetPerlinNoise3D(Pos, Lode.noiseOffset, Lode.Scale, Lode.threshold))
				{
					VoxelValue = Lode.blockID;
				}
			}
		}
	}

	return VoxelValue;
	
	
}

void AWorldGenerator::GenerateChunk(FVector2d Position)
{
	AChunk* NewChunk = GetWorld()->SpawnActorDeferred<AChunk>(AChunk::StaticClass(), FTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        
	if (NewChunk)
	{
		// Set properties before the actor is fully spawned
		NewChunk->World = this;
		NewChunk->ChunkCoord = new ChunkCoord(Position.X, Position.Y); 
		
		// Complete the spawning process
		UGameplayStatics::FinishSpawningActor(NewChunk, FTransform());
	}

	Chunks[Position.X][Position.Y] = NewChunk;
	ActiveChunk.Add(ChunkCoord(Position.X, Position.Y));
}

bool AWorldGenerator::IsChunkInWorld(ChunkCoord Coord)
{
	return (Coord.X >= 0 && Coord.X < FVoxelData::WorldSizeInChunks && Coord.Y >= 0 && Coord.Y < FVoxelData::WorldSizeInChunks);
}

bool AWorldGenerator::IsVoxelInWorld(FVector Pos)
{
	// If you are sure Pos represents voxel positions (and not world coordinates):
	return FMath::FloorToInt(Pos.X) >= 0 && FMath::FloorToInt(Pos.X) < FVoxelData::WorldSizeInVoxels &&
		   FMath::FloorToInt(Pos.Y) >= 0 && FMath::FloorToInt(Pos.Y) < FVoxelData::WorldSizeInVoxels &&
		   FMath::FloorToInt(Pos.Z) >= 0 && FMath::FloorToInt(Pos.Z) < FVoxelData::ChunkHeight;
}

void AWorldGenerator::CheckViewDistance()
{
	ChunkCoord LocalPlayerChunkCoord = GetChunkCoordFromVector(Player->GetActorLocation());

	TArray<ChunkCoord> previousActiveChunks = ActiveChunk;

	for (int x = LocalPlayerChunkCoord.X - FVoxelData::ViewDistanceInChunks; x < LocalPlayerChunkCoord.X + FVoxelData::ViewDistanceInChunks; x++)
	{
		for (int y = LocalPlayerChunkCoord.Y - FVoxelData::ViewDistanceInChunks; y < LocalPlayerChunkCoord.Y + FVoxelData::ViewDistanceInChunks; y++)
		{
			if(IsChunkInWorld(ChunkCoord(x, y)))
			{
				if(Chunks[x][y] == nullptr)
				{
					GenerateChunk(FVector2D(x, y));
				}
				else if(Chunks[x][y]->IsHidden())
				{
					Chunks[x][y]->SetActorHiddenInGame(false);
					ActiveChunk.Add(ChunkCoord(x, y));
				}
			}
			for (int i = 0; i < previousActiveChunks.Num(); i++)
			{
				if(previousActiveChunks[i].Equals(ChunkCoord(x, y)))
				{
					previousActiveChunks.RemoveAt(i);
				}
			}
		}
	}

	for (auto PreviousActiveChunk : previousActiveChunks)
	{
		Chunks[PreviousActiveChunk.X][PreviousActiveChunk.Y]->SetActorHiddenInGame(true);
		//ActiveChunk.Remove(ChunkCoord(PreviousActiveChunk.X, PreviousActiveChunk.Y));
	}
}

ChunkCoord AWorldGenerator::GetChunkCoordFromVector(FVector Pos)
{
	int x = FMath::FloorToInt(Pos.X / (FVoxelData::ChunkWidth * 100));
	int y = FMath::FloorToInt(Pos.Y / (FVoxelData::ChunkWidth * 100));

	return ChunkCoord(x, y);
}


// Called every frame
void AWorldGenerator::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerChunkCoord = GetChunkCoordFromVector(Player->GetActorLocation());

	if(!GetChunkCoordFromVector(Player->GetActorLocation()).Equals(PlayerLastChunkCoord))
	{
		CheckViewDistance();
	}
}

