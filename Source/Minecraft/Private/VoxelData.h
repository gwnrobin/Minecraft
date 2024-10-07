#pragma once

#include "CoreMinimal.h"

class FVoxelData
{

public:
	// Static constant integers
	static const int ChunkWidth;
	static const int ChunkHeight;
	static const int WorldSizeInChunks;
	static const int ViewDistanceInChunks;

	static const int WorldSizeInVoxels;
	
	static const int TextureAtlasSizeInBlocks;

	// Static method for normalized block texture size
	static float GetNormalizedBlockTextureSize()
	{
		return 1.0f / TextureAtlasSizeInBlocks;
	}

	// Static arrays for voxel vertices
	static const FVector VoxelVerts[8];
    
	// Static arrays for face checks
	static const FVector FaceChecks[6];

	// Static 2D array for voxel triangles
	static const int32 VoxelTris[6][4];

	// Static arrays for voxel UVs
	static const FVector2D VoxelUVs[4];
};
