#include "Minecraft/Public/ChunkCoord.h"

#include "VoxelData.h"

ChunkCoord::ChunkCoord(int X, int Y)
{
	this->X = X;
	this->Y = Y;
}

ChunkCoord::ChunkCoord(FVector Pos)
{
	 int XCheck = FMath::FloorToInt(Pos.X);
	 int YCheck = FMath::FloorToInt(Pos.Y);

	X = XCheck / FVoxelData::ChunkWidth;
	Y = YCheck / FVoxelData::ChunkWidth;
}

ChunkCoord::~ChunkCoord()
{
}

bool ChunkCoord::Equals(const ChunkCoord& Other)
{
	return (Other.X == X && Other.Y == Y);
}
