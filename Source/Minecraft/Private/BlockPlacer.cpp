// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockPlacer.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBlockPlacer::UBlockPlacer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBlockPlacer::BeginPlay()
{
	Super::BeginPlay();

	AWorldGenerator* WorldGeneratorActor = Cast<AWorldGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldGenerator::StaticClass()));
	if (WorldGeneratorActor)
	{
		World = WorldGeneratorActor;
	}
}


// Called every frame
void UBlockPlacer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PlaceBlocks();
}

void UBlockPlacer::PlaceBlocks() {

	float Step = CheckIncrements;
	FVector LastPos = FVector::ZeroVector;
	APlayerCameraManager* Camera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

	while (Step < Reach) {

		FVector Pos = Camera->GetCameraLocation()/100 + (Camera->GetCameraRotation().Vector() * Step);

		if(World->CheckForVoxel(Pos, false))
		{
			HighlightPosition = FVector(FMath::FloorToInt(Pos.X), FMath::FloorToInt(Pos.Y), FMath::FloorToInt(Pos.Z));
			PlacePosition = LastPos;
			
			return;
		}
		
		LastPos = FVector(FMath::FloorToInt(Pos.X), FMath::FloorToInt(Pos.Y), FMath::FloorToInt(Pos.Z));

		Step += CheckIncrements;
	}
}

void UBlockPlacer::ChangeBlock(int32 NewID)
{
	if (World)
	{
		if (AChunk* Chunk = World->GetChunkFromVector3(HighlightPosition))
		{
			Chunk->EditVoxel(HighlightPosition, NewID);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Chunk is null for HighlightPosition in UBlockPlacer::ChangeBlock %s"), *HighlightPosition.ToString());
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("World is null in UBlockPlacer::ChangeBlock"));
	}
}


