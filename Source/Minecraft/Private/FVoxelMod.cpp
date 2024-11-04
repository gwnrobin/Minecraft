// Fill out your copyright notice in the Description page of Project Settings.


#include "FVoxelMod.h"

FVoxelMod::FVoxelMod()
{
	this->Position = FVector(0,0,0);
	this->ID = 0;
}

FVoxelMod::FVoxelMod(FVector Position, int ID)
{
	this->Position = Position*100;
	this->ID = ID;
}

FVoxelMod::~FVoxelMod()
{
}
