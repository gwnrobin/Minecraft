// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomeAttributes.h"
#include "BlockType.h"
#include "Chunk.h"
#include "FVoxelMod.h"
#include "GameFramework/Actor.h"
#include "WorldGenerator.generated.h"

UCLASS()
class AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GenerateWorld();
	void CreateChunk();
	void UpdateChunks();
	void ApplyModifications();
	AChunk* GenerateChunk(FVector2d Position, bool InstantGenerate);

	TArray<TArray<AChunk*>> Chunks;
	TArray<ChunkCoord> ActiveChunks;
	
	TQueue<ChunkCoord> ChunksToCreate;
	TArray<AChunk*> ChunksToUpdate;
	
	TQueue<FVoxelMod> Modifications;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	int Seed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	UBiomeAttributes* Biome;

	ChunkCoord PlayerChunkCoord;
	ChunkCoord PlayerLastChunkCoord;

	bool IsCreatingChunks;
	bool IsApplyingModifications;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitChunksASync();
	bool CheckForVoxel(FVector, bool);
	int GetVoxel(FVector Pos);
	bool IsChunkInWorld(ChunkCoord Coord);
	bool IsVoxelInWorld(FVector Pos);
	AChunk* GetChunkFromVector3(FVector Pos);
	void CheckViewDistance();
	ChunkCoord GetChunkCoordFromVector(FVector Pos);
	
	
	FVector SpawnPosition;
	
	AActor* Player;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	UMaterialInterface* Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	TArray<FBlockType> BlockType;

};

