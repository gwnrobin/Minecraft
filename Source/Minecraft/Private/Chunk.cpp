#include "Chunk.h"
#include "VoxelData.h"
#include "WorldGenerator.h"

AChunk::AChunk()
{
	//PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("chunk"));
	SetRootComponent(MeshComponent);
	vertexIndex = 0;

	VoxelMap.SetNum(FVoxelData::ChunkWidth); // Set outer dimension (x)
	for (int32 x = 0; x < FVoxelData::ChunkWidth; x++)
	{
		VoxelMap[x].SetNum(FVoxelData::ChunkWidth); // Set second dimension (y)
		for (int32 y = 0; y < FVoxelData::ChunkWidth; y++)
		{
			VoxelMap[x][y].SetNum(FVoxelData::ChunkHeight); // Set third dimension (z)
		}
	}
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::Init()
{
	if(IsVoxelMapPopulated)
		return;
	
	AttachToActor(World, FAttachmentTransformRules::KeepWorldTransform);
	SetActorScale3D(FVector(100, 100, 100));
	SetActorLocation(FVector(ChunkCoord->X * FVoxelData::ChunkWidth * 100, ChunkCoord->Y * FVoxelData::ChunkWidth * 100, 0.0f));
	
	PopulateVoxelMap();
	UpdateChunk();
}

void AChunk::PopulateVoxelMap()
{
	for (int z = 0; z < FVoxelData::ChunkHeight; z++)
	{
		for (int x = 0; x < FVoxelData::ChunkWidth; x++)
		{
			for (int y = 0; y < FVoxelData::ChunkWidth; y++)
			{
				VoxelMap[x][y][z] = World->GetVoxel(FVector(x, y, z) + MeshComponent->GetRelativeLocation());
			}
		}
	}
	IsVoxelMapPopulated = true;
}

void AChunk::UpdateChunk()
{
	while (!Modifications.IsEmpty())
	{
		FVoxelMod V;
		Modifications.Dequeue(V);

		FVector Pos = V.Position/100 -= MeshComponent->GetRelativeLocation();
		VoxelMap[Pos.X][Pos.Y][Pos.Z] = V.ID;
	}
	
	ClearMeshData();
	
	for (int z = 0; z < FVoxelData::ChunkHeight; z++)
	{
		for (int x = 0; x < FVoxelData::ChunkWidth; x++)
		{
			for (int y = 0; y < FVoxelData::ChunkWidth; y++)
			{
				if(World->BlockType[VoxelMap[x][y][z]].IsSolid)
				{
					UpdateMeshData(FVector(x,y,z));
				}
			}
		}
	}

	CreateMesh();
}

void AChunk::EditVoxel(FVector Pos, int32 NewId)
{
	int32 XCheck = FMath::FloorToInt(Pos.X);
	int32 YCheck = FMath::FloorToInt(Pos.Y);
	int32 ZCheck = FMath::FloorToInt(Pos.Z);

	XCheck -= FMath::FloorToInt(MeshComponent->GetRelativeLocation().X);
	YCheck -= FMath::FloorToInt(MeshComponent->GetRelativeLocation().Y);

	VoxelMap[XCheck][YCheck][ZCheck] = NewId;

	UpdateChunk();
}

void AChunk::UpdateSurroundingChunks(int X, int Y, int Z)
{
	FVector	ThisVoxel = FVector(X, Y, Z);

	for (int p = 0; p < 6; p++)
	{
		FVector CurrentVoxel = ThisVoxel + FVoxelData::FaceChecks[p];

		if(!IsVoxelInChunk(CurrentVoxel.X, CurrentVoxel.Y, CurrentVoxel.Z))
		{
			World->GetChunkFromVector3(CurrentVoxel + MeshComponent->GetRelativeLocation())->UpdateChunk();
		}
	}
}

bool AChunk::CheckVoxel(FVector Pos)
{
	int32 x = FMath::FloorToInt(Pos.X);
	int32 y = FMath::FloorToInt(Pos.Y);
	int32 z = FMath::FloorToInt(Pos.Z);

	if (!IsVoxelInChunk(x, y, z))
	{
		return World->CheckForVoxel(Pos + MeshComponent->GetRelativeLocation(), false);
	}

	// Check if the voxel is solid within the current chunk
	return World->BlockType[VoxelMap[x][y][z]].IsSolid;
}



bool AChunk::IsVoxelInChunk(int X, int Y, int Z)
{
	if (X < 0 || X >= FVoxelData::ChunkWidth || 
		Y < 0 || Y >= FVoxelData::ChunkWidth || 
		Z < 0 || Z >= FVoxelData::ChunkHeight)
	{
		return false;
	}
	return true;
}

int32 AChunk::GetVoxelFromGlobalVector(FVector Pos)
{
	int32 XCheck = FMath::FloorToInt(Pos.X);
	int32 YCheck = FMath::FloorToInt(Pos.Y);
	int32 ZCheck = FMath::FloorToInt(Pos.Z);

	XCheck -= FMath::FloorToInt(MeshComponent->GetRelativeLocation().X);
	YCheck -= FMath::FloorToInt(MeshComponent->GetRelativeLocation().Y);

	return VoxelMap[XCheck][YCheck][ZCheck];
}

bool AChunk::IsActive()
{
	return IsHidden();
}

void AChunk::SetActiveState(bool state)
{
	SetActorHiddenInGame(state);
}

void AChunk::UpdateMeshData(FVector Pos)
{
	for (int p = 0; p < 6; p++)
	{ 
		if (!CheckVoxel(Pos + FVoxelData::FaceChecks[p])) 
		{
			uint8 BlockID = VoxelMap[(int32)Pos.X][(int32)Pos.Y][(int32)Pos.Z];
			
			for (int32 i = 0; i < 4; i++)
			{
				FVector vertex = Pos + FVoxelData::VoxelVerts[FVoxelData::VoxelTris[p][i]];
				Vertices.Add(vertex);  // Add distinct vertices
			}

			AddTexture(World->BlockType[BlockID].GetTextureID(p));

			// Ensure correct triangle index order
			Triangles.Add(vertexIndex + 0);
			Triangles.Add(vertexIndex + 1);
			Triangles.Add(vertexIndex + 2);
			Triangles.Add(vertexIndex + 2);
			Triangles.Add(vertexIndex + 1);
			Triangles.Add(vertexIndex + 3);
			
			vertexIndex += 4;
		}
	}
}

void AChunk::CreateMesh()
{
	MeshComponent->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), Uvs, TArray<FColor>(),TArray<FProcMeshTangent>(), true);
	MeshComponent->SetMaterial(0, World->Material);
}

void AChunk::ClearMeshData()
{
	vertexIndex = 0;
	Vertices.Empty();
	Triangles.Empty();
	Uvs.Empty();
}

void AChunk::AddTexture(int TextureID)
{
	// Calculate the row and column based on the TextureID
	int32 Column = TextureID % FVoxelData::TextureAtlasSizeInBlocks;
	int32 Row = TextureID / FVoxelData::TextureAtlasSizeInBlocks;
    
	// Calculate the UV coordinates 
	float x = Column * FVoxelData::GetNormalizedBlockTextureSize();
	float y = Row * FVoxelData::GetNormalizedBlockTextureSize();
	
	Uvs.Add(FVector2D(x + FVoxelData::GetNormalizedBlockTextureSize(), y + FVoxelData::GetNormalizedBlockTextureSize())); // Top-right becomes Bottom-right
	Uvs.Add(FVector2D(x + FVoxelData::GetNormalizedBlockTextureSize(), y)); // Top-left becomes Bottom-right
	Uvs.Add(FVector2D(x, y + FVoxelData::GetNormalizedBlockTextureSize())); // Bottom-right becomes Top-left
	Uvs.Add(FVector2D(x, y)); // Bottom-left becomes Top-left
}
