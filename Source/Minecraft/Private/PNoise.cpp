#include "PNoise.h"
#include "VoxelData.h"

float FPNoise::GetPerlinNoise(FVector2D Position, float Offset, float Scale)
{
	return FMath::PerlinNoise2D(FVector2D((Position.X + 0.1f) / FVoxelData::ChunkWidth * Scale + Offset, (Position.Y + 0.1f) / FVoxelData::ChunkWidth * Scale + Offset));
}

bool FPNoise::GetPerlinNoise3D(FVector Position, float Offset, float Scale, float Threshold)
{
	float X = (Position.X + Offset + 0.1) * Scale;
	float Y = (Position.Y + Offset + 0.1) * Scale;
	float Z = (Position.Z + Offset + 0.1) * Scale;

	float Perlin = FMath::PerlinNoise3D(FVector(X, Y, Z));

	return Perlin > Threshold;
}
