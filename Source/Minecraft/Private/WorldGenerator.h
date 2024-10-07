// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomeAttributes.h"
#include "BlockType.h"
#include "Chunk.h"
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
	void GenerateChunk(FVector2d Position);

	TArray<TArray<AChunk*>> Chunks;
	TArray<ChunkCoord> ActiveChunk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	int Seed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	UBiomeAttributes* Biome;

	ChunkCoord PlayerChunkCoord;
	ChunkCoord PlayerLastChunkCoord;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetVoxel(FVector Pos);
	bool IsChunkInWorld(ChunkCoord Coord);
	bool IsVoxelInWorld(FVector Pos);
	void CheckViewDistance();
	ChunkCoord GetChunkCoordFromVector(FVector Pos);
	
	FVector SpawnPosition;
	
	AActor* Player;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	UMaterialInterface* Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generating")
	TArray<FBlockType> BlockType;

};

