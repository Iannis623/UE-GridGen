// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridRunnable.h"
#include "GridTilesData.h"


class GRID_API FGridGenerateInstancesWorker : public FGridRunnable
{
public:
	FGridGenerateInstancesWorker(
		UObject* InWorldContext,
		FVector CenterLocation,
		FVector TileSize,
		FIntPoint TileCount,
		FVector BottomLeftCorner,
		bool bFastGen
	);

	virtual ~FGridGenerateInstancesWorker() override;

	bool Init() override;
	void RefreshResult();
	uint32 Run() override;

	void FillData(FGridTileData Data);
	TMap<FIntVector, FGridTileData> GridTiles;
	TArray<FIntVector> InstanceIndexes;
	TMap<FIntPoint, FTileHeightTranslator> TileHeightTranslator;
	TArray<FTransform> Transforms;

	FVector GetTileLocationFromGridIndex(const FIntVector Index) const;
	FVector GetTileScale() const;

	TObjectPtr<UObject> WorldContext = nullptr;

	FVector GridCenterLocation;
	FVector GridTileSize;
	FIntPoint GridTileCount;
	FVector GridBottomLeftCorner;
	bool bGridFastGen;

	void Stop() override;
};
