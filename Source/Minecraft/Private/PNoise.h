#pragma once

#include "CoreMinimal.h"

class FPNoise
{
public:
	FPNoise();
	~FPNoise();

	static float GetPerlinNoise(FVector2D Position, float Offset, float Scale);
	
	static bool GetPerlinNoise3D(FVector Position, float Offset, float Scale, float Threshold);
private:
	void Test();
};
