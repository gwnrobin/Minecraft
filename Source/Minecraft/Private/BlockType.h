// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockType.generated.h"

USTRUCT(BlueprintType)
struct FBlockType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FString BlockName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	bool IsSolid;

	// Texture Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	int32 BackFaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	int32 FrontFaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	int32 TopFaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	int32 BottomFaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	int32 RightFaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	int32 LeftFaceTexture;

	// Function to get texture based on face index
	int32 GetTextureID(int32 faceIndex) const;

	FBlockType()
	{
		BlockName = "";
		IsSolid = true;
		BackFaceTexture = 0;
		FrontFaceTexture = 0;
		TopFaceTexture = 0;
		BottomFaceTexture = 0;
		RightFaceTexture = 0;
		LeftFaceTexture = 0;
	}
};
