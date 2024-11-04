#include "WorldGenerator.h"
#include "Chunk.h"
#include "MathUtil.h"
#include "PNoise.h"
#include "Structure.h"
#include "VoxelData.h"
#include "GameFramework/Character.h"
#include "Async/Async.h"
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
			Chunks[x][y] = GenerateChunk(FVector2D(x, y), true);
			ActiveChunks.Add(ChunkCoord(x,y));
		}
	}

	while (!Modifications.IsEmpty())
	{
		FVoxelMod V;
		Modifications.Dequeue(V);

		ChunkCoord C = GetChunkCoordFromVector(V.Position);

		if(Chunks[C.X][C.Y] == nullptr)
		{
			Chunks[C.X][C.Y] = GenerateChunk(FVector2D(C.X, C.Y), true);
			ActiveChunks.Add(C);
		}

		Chunks[C.X][C.Y]->Modifications.Enqueue(V);

		if(!ChunksToUpdate.Contains(Chunks[C.X][C.Y]))
		{
			ChunksToUpdate.Add(Chunks[C.X][C.Y]);
		}
	}

	for (int i = 0; i < ChunksToUpdate.Num(); i++)
	{
		ChunksToUpdate[0]->UpdateChunk();
		ChunksToUpdate.RemoveAt(0);
	}
}

void AWorldGenerator::CreateChunk()
{
	ChunkCoord C;
	ChunksToCreate.Dequeue(C);
		
	ActiveChunks.Add(C);
	Chunks[C.X][C.Y]->Init();
}

void AWorldGenerator::UpdateChunks()
{
	bool Updated = false;
	int Index = 0;

	while (!Updated && Index < ChunksToUpdate.Num() - 1)
	{
		if(ChunksToUpdate[Index]->IsVoxelMapPopulated)
		{
			ChunksToUpdate[Index]->UpdateChunk();
			ChunksToUpdate.RemoveAt(Index);
			Updated = true;
		}
		else
		{
			Index++;
		}
	}
}

void AWorldGenerator::ApplyModifications()
{
	IsApplyingModifications = true;
	int Count = 0;

	while (!Modifications.IsEmpty())
	{
		FVoxelMod V;
		Modifications.Dequeue(V);

		ChunkCoord C = GetChunkCoordFromVector(V.Position);

		if(Chunks[C.X][C.Y] == nullptr)
		{
			Chunks[C.X][C.Y] = GenerateChunk(FVector2D(C.X, C.Y), true);
			ActiveChunks.Add(C);
		}

		Chunks[C.X][C.Y]->Modifications.Enqueue(V);

		if(!ChunksToUpdate.Contains(Chunks[C.X][C.Y]))
		{
			ChunksToUpdate.Add(Chunks[C.X][C.Y]);
		}

		Count++;
		if(Count > 200)
		{
			Count = 0;
			return;
		}
		
	}
	IsApplyingModifications = false;
}

int AWorldGenerator::GetVoxel(FVector Pos)
{
	int ZPos = FMath::FloorToInt(Pos.Z);
	int VoxelValue;

	if (!IsVoxelInWorld(Pos))
		VoxelValue = 0;

	if (ZPos == 0)
		VoxelValue = 1; // bedrock

	// Base terrain height calculation
	float BaseTerrainNoise = FPNoise::GetPerlinNoise(FVector2D(Pos.X, Pos.Y), 0, Biome->TerrainScale);

	// Adjust noise to control how much it influences the terrain
	BaseTerrainNoise = FMath::Clamp(BaseTerrainNoise, 0.3f, 0.7f); // Focus on middle range for flatter terrain

	// Base terrain height to be between 32 and 80 (prevent going too low)
	int BaseTerrainHeight = FMath::FloorToInt(32 + (BaseTerrainNoise * 48)); // 32 to 80 for normal terrain

	// Add mountain noise on top of base terrain for mountain peaks
	float MountainNoise = FPNoise::GetPerlinNoise(FVector2D(Pos.X, Pos.Y), 0, 0.05); // Keep mountain noise smaller scale
	int MountainHeight = FMath::FloorToInt(MountainNoise * 20); // Mountain variation between 0 and 20 blocks

	// Final terrain height: base height plus mountain height
	int FinalTerrainHeight = FMath::Clamp(BaseTerrainHeight + MountainHeight, 32, 120); // Ensure total stays between 32 and 120

	// Return voxel type based on Z position and final terrain height
	if (ZPos > FinalTerrainHeight)
		VoxelValue = 0; // air above terrain
	else
		VoxelValue = 2; // stone for solid terrain

	if(ZPos == FinalTerrainHeight)
		VoxelValue = 3;
	else if(ZPos < FinalTerrainHeight && ZPos > FinalTerrainHeight - 4)
		VoxelValue = 5;


	if(ZPos == FinalTerrainHeight)
	{
		if(FPNoise::GetPerlinNoise(FVector2D(Pos.X, Pos.Y), 0, Biome->TreeZoneScale) > Biome->TreeZoneThreshold)
		{
			VoxelValue = 1;
			if(FPNoise::GetPerlinNoise(FVector2D(Pos.X, Pos.Y), 0, Biome->TreePlacementScale) > Biome->TreePlacementThreshold)
			{
				VoxelValue = 6;
				//Modifications.Enqueue( FVoxelMod(FVector(Pos.X, Pos.Y, Pos.Z + 1)*100, 6));
				FStructure::MakeTree(Pos, &Modifications, Biome->MinTreeSize, Biome->MaxTreeSize);
			}
		}
	}

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

AChunk* AWorldGenerator::GenerateChunk(FVector2d Position, bool InstantGenerate)
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

	if(InstantGenerate)
	{
		NewChunk->Init();
		NewChunk->CreateMesh();
	}

	return NewChunk;
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

AChunk* AWorldGenerator::GetChunkFromVector3(FVector Pos)
{
	int x = FMath::FloorToInt(Pos.X / FVoxelData::ChunkWidth);
	int y = FMath::FloorToInt(Pos.Y / FVoxelData::ChunkWidth);

	return Chunks[x][y];
}


void AWorldGenerator::CheckViewDistance()
{
	ChunkCoord LocalPlayerChunkCoord = GetChunkCoordFromVector(Player->GetActorLocation());

	PlayerLastChunkCoord = PlayerChunkCoord;

	TArray<ChunkCoord> previousActiveChunks = ActiveChunks;

	for (int x = LocalPlayerChunkCoord.X - FVoxelData::ViewDistanceInChunks; x < LocalPlayerChunkCoord.X + FVoxelData::ViewDistanceInChunks; x++)
	{
		for (int y = LocalPlayerChunkCoord.Y - FVoxelData::ViewDistanceInChunks; y < LocalPlayerChunkCoord.Y + FVoxelData::ViewDistanceInChunks; y++)
		{
			if(IsChunkInWorld(ChunkCoord(x, y)))
			{
				if(Chunks[x][y] == nullptr)
				{
					Chunks[x][y] = GenerateChunk(FVector2D(x, y), false);
					ChunksToCreate.Enqueue(ChunkCoord(x, y));
				}
				else if(Chunks[x][y]->IsHidden())
				{
					Chunks[x][y]->SetActorHiddenInGame(false);
				}
				ActiveChunks.Add(ChunkCoord(x, y));
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

	if(!Modifications.IsEmpty() && !IsApplyingModifications)
	{
		ApplyModifications();
	}

	if(!ChunksToCreate.IsEmpty())
	{
		CreateChunk();
	}

	if(!ChunksToUpdate.IsEmpty())
	{
		UpdateChunks();
	}
}

bool AWorldGenerator::CheckForVoxel(FVector Pos, bool Debug = false)
{
	// Get the chunk coordinates from the world position
	ChunkCoord ThisChunk = GetChunkCoordFromVector(Pos);

	// Check if the chunk is inside the world boundaries
	if (!IsChunkInWorld(ThisChunk) || Pos.Z < 0 || Pos.Z > FVoxelData::ChunkHeight)
		return false;

	// Check if the chunk exists and if its voxel map is populated
	if (Chunks[ThisChunk.X][ThisChunk.Y] != nullptr && Chunks[ThisChunk.X][ThisChunk.Y]->IsVoxelMapPopulated)
	{
		// Retrieve the voxel from the chunk and check if it's solid
		int32 VoxelType = Chunks[ThisChunk.X][ThisChunk.Y]->GetVoxelFromGlobalVector(Pos);
		return BlockType[VoxelType].IsSolid;
	}

	// Fallback to using GetVoxel if the chunk's voxel map is not populated
	int32 VoxelValue = GetVoxel(Pos);

	if(Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Checking voxel at position (%f, %f, %f) in chunk (%d, %d)"), Pos.X, Pos.Y, Pos.Z, ThisChunk.X, ThisChunk.Y);

		if (Chunks[ThisChunk.X][ThisChunk.Y] == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Chunk at (%d, %d) is null!"), ThisChunk.X, ThisChunk.Y);
		}
		else if (!Chunks[ThisChunk.X][ThisChunk.Y]->IsVoxelMapPopulated)
		{
			UE_LOG(LogTemp, Warning, TEXT("Chunk at (%d, %d) has no voxel map populated!"), ThisChunk.X, ThisChunk.Y);
		}
	}
    
	// If GetVoxel returns a valid value, check if it's solid
	return BlockType[VoxelValue].IsSolid;
}


