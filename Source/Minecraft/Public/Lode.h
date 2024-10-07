#pragma once

#include "CoreMinimal.h"
#include "Lode.generated.h"

USTRUCT(BlueprintType)
struct MINECRAFT_API FLode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	FString NodeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	int32 blockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	int MinHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	int MaxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	float threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	float noiseOffset;
};
