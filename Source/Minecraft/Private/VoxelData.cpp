#include "VoxelData.h"

// Initialize static constants
const int FVoxelData::ChunkWidth = 16;
const int FVoxelData::ChunkHeight = 128;
const int FVoxelData::TextureAtlasSizeInBlocks = 32;
const int FVoxelData::WorldSizeInChunks = 10;
const int FVoxelData::ViewDistanceInChunks = 5;

const int FVoxelData::WorldSizeInVoxels = WorldSizeInChunks * ChunkWidth;

// Initialize voxel vertices
const FVector FVoxelData::VoxelVerts[8] =
{
	FVector(0, 0, 0), // 0: Bottom Left Back
	FVector(1, 0, 0), // 1: Bottom Right Back
	FVector(1, 0, 1), // 2: Top Right Back
	FVector(0, 0, 1), // 3: Top Left Back
	FVector(0, 1, 0), // 4: Bottom Left Front
	FVector(1, 1, 0), // 5: Bottom Right Front
	FVector(1, 1, 1), // 6: Top Right Front
	FVector(0, 1, 1)  // 7: Top Left Front
};


// Initialize face checks
const FVector FVoxelData::FaceChecks[6] =
{
    FVector(0.0f, -1.0f, 0.0f),  // Back
    FVector(0.0f, 1.0f, 0.0f),   // Front
    FVector(0.0f, 0.0f, 1.0f),   // Top
    FVector(0.0f, 0.0f, -1.0f),  // Bottom
    FVector(-1.0f, 0.0f, 0.0f),  // Left
    FVector(1.0f, 0.0f, 0.0f)    // Right
};


// Initialize voxel triangles
const int32 FVoxelData::VoxelTris[6][4] =
{
	{0, 3, 1, 2}, // Back Face
	{5, 6, 4, 7}, // Front Face
	{3, 7, 2, 6}, // Top Face
	{1, 5, 0, 4}, // Bottom Face
	{4, 7, 0, 3}, // Left Face
	{1, 2, 5, 6} // Right Face
};

// Initialize voxel UVs
const FVector2D FVoxelData::VoxelUVs[4] =
{
	FVector2D(0.0f, 0.0f), // Bottom Left
	FVector2D(1.0f, 0.0f), // Bottom Right
	FVector2D(0.0f, 1.0f), // Top Left
	FVector2D(1.0f, 1.0f)  // Top Right
};

