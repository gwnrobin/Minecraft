// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MINECRAFT_API FVoxelMod
{
public:
	FVoxelMod();
	FVoxelMod(FVector Position, int ID);
	~FVoxelMod();

	FVector Position;
	int ID;
	
};
