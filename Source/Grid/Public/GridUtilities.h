// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GridUtilities.generated.h"

/**
 * 
 */
UCLASS()
class GRID_API UGridUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FVector SnapVectorToVector(const FVector& V1, const FVector& V2);
};
