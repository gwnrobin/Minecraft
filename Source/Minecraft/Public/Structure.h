// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FVoxelMod.h"

/**
 * 
 */
class MINECRAFT_API FStructure
{
public:
	FStructure();
	~FStructure();

	static void MakeTree(FVector Position, TQueue<FVoxelMod> *Queue, int MinTrunkHeight, int MaxTrunkHeight);
};
