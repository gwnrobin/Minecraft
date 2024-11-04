// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGenerator.h"
#include "Components/ActorComponent.h"
#include "BlockPlacer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UBlockPlacer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlockPlacer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AWorldGenerator* World;
	int32 SelectedBlockIndex = 1;
	FVector PlacePosition;
	FVector HighlightPosition;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void PlaceBlocks();
	UFUNCTION(BlueprintCallable)
	void ChangeBlock(int32 NewID);

	float CheckIncrements = 0.1f;
	float Reach = 8;
		
};
