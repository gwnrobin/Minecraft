// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure.h"

#include "PNoise.h"
#include "VoxelData.h"

FStructure::FStructure()
{
}

FStructure::~FStructure()
{
}

void FStructure::MakeTree(FVector Position, TQueue<FVoxelMod> *Queue, int MinTrunkHeight, int MaxTrunkHeight)
{
	int Height = MaxTrunkHeight * FPNoise::GetPerlinNoise(FVector2D(Position.X, Position.Y), 250, 3);

	if(Height < MinTrunkHeight)
	{
		Height = MinTrunkHeight;
	}

	for (int i = 1; i < Height; i++)
	{
		Queue->Enqueue(FVoxelMod(FVector(Position.X, Position.Y, Position.Z + i), 6));
	}

	for (int x = -3; x < 4; x++)
	{
		for (int y = -3; y < 4; y++)
		{
			for (int z = 0; z < 7; z++)
			{
				Queue->Enqueue(FVoxelMod(FVector(Position.X + x, Position.Y + y, Position.Z + Height + z), 7));
			}
		}
	}
}
