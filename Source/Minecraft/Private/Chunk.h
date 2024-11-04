// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FVoxelMod.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Minecraft/Public/ChunkCoord.h"
#include "Chunk.generated.h"

// Forward declaration instead of including "WorldGenerator.h"
class AWorldGenerator;

UCLASS()
class AChunk : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AChunk();

		AWorldGenerator* World;
		ChunkCoord* ChunkCoord;

		TQueue<FVoxelMod> Modifications;

	bool IsVoxelMapPopulated = false;
		
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		void Init();
		int32 GetVoxelFromGlobalVector(FVector Pos);
		void CreateMesh();
		void ClearMeshData();
		void EditVoxel(FVector Pos, int32 NewId);
		void UpdateChunk();

	private:
		UProceduralMeshComponent* MeshComponent;

		TArray<TArray<TArray<uint8>>> VoxelMap;

		int vertexIndex;
		TArray<FVector> Vertices;
		TArray<int> Triangles;
		TArray<FVector2D> Uvs;
	
		void PopulateVoxelMap();
		void UpdateSurroundingChunks(int X, int Y, int z);
		bool CheckVoxel(FVector Pos);
		bool IsVoxelInChunk(int X, int Y, int Z);
		bool IsActive();
		void SetActiveState(bool state);
		void UpdateMeshData(FVector Pos);
		void AddTexture(int TextureID);
};
