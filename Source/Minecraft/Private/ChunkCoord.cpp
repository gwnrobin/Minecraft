#include "Minecraft/Public/ChunkCoord.h"

ChunkCoord::ChunkCoord(int X, int Y)
{
	this->X = X;
	this->Y = Y;
}

ChunkCoord::~ChunkCoord()
{
}

bool ChunkCoord::Equals(const ChunkCoord& Other)
{
	return (Other.X == X && Other.Y == Y);
}
