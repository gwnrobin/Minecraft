#pragma once

#include "CoreMinimal.h"
#include "Lode.h"
#include "Engine/DataAsset.h"
#include "BiomeAttributes.generated.h"

UCLASS(BlueprintType)
class MINECRAFT_API UBiomeAttributes : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	FString BiomeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	int SolidGroundHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	int TerrainHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	float TerrainScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Data")
	TArray<FLode> Lodes;
};
