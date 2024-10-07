// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockType.h"

int32 FBlockType::GetTextureID(int32 faceIndex) const
{
	switch (faceIndex)
	{
	case 0:
		return BackFaceTexture;
	case 1:
		return FrontFaceTexture;
	case 2:
		return TopFaceTexture;
	case 3:
		return BottomFaceTexture;
	case 4:
		return RightFaceTexture;
	case 5:
		return LeftFaceTexture;
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid face index"));
		return 0;
	}
}

