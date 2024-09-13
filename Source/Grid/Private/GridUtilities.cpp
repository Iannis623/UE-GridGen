// Fill out your copyright notice in the Description page of Project Settings.


#include "GridUtilities.h"

FVector UGridUtilities::SnapVectorToVector(const FVector& V1, const FVector& V2)
{
	return FVector(
		FMath::GridSnap(V1.X, V2.X),
		FMath::GridSnap(V1.Y, V2.Y),
		FMath::GridSnap(V1.Z, V2.Z));
}
