// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GridTilesData.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	None		UMETA(DisplayName="None"),
	Normal		UMETA(DisplayName="Normal"),
	Obstacle	UMETA(DisplayName="Obstacle"),
	FlyingOnly	UMETA(DisplayName="Flying Only")
};

UENUM(BlueprintType)
enum class ETileState : uint8
{
	None			UMETA(DisplayName="None"),
	Hovered			UMETA(DisplayName="Hovered"),
	Selected		UMETA(DisplayName="Selected"),
	Neighbour		UMETA(DisplayName="Neighbour"),
	Path			UMETA(DisplayName="Path"),
	Discovered		UMETA(DisplayName="Discovered"),
	Analyzed		UMETA(DisplayName="Analyzed"),
	Reachable		UMETA(DisplayName="Reachable"),
	SpellRange		UMETA(DisplayName="Spell Range"),
	SpellRangeAoE	UMETA(DisplayName="Spell Range AoE")
	
};

UENUM(BlueprintType)
enum class EGridPattern : uint8
{
	None			UMETA(DisplayName="None"),
	Line			UMETA(DisplayName="Line"),
	Diagonal		UMETA(DisplayName="Diagonal"),
	HalfDiagonal	UMETA(DisplayName="Half Diagonal"),
	Star			UMETA(DisplayName="Star"),
	Diamond			UMETA(DisplayName="Diamond"),
	Square			UMETA(DisplayName="Square")
};

USTRUCT(BlueprintType)
struct FGridTileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Tile")
	FIntVector Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Tile")
	ETileType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Tile")
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Tile")
	TArray<ETileState> States;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Tile")
	TObjectPtr<AActor> UnitOnTile;
};

USTRUCT(BlueprintType)
struct FTileHeightTranslator
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Translator")
	TArray<FIntVector> Translator;
};


UCLASS()
class GRID_API UGridTilesData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntVector, FGridTileData> GridTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntVector> InstanceIndexes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntPoint, FTileHeightTranslator> TileHeightTranslator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETileState, FIntVector> TileStateToIndexes;

	UFUNCTION(BlueprintCallable)
	static int32 GetTileTypeCost(const ETileType TileType)
	{
		switch (TileType)
		{
		case ETileType::None:
			return 0;
		case ETileType::Normal:
			return 1;
		case ETileType::Obstacle:
			return 0;
		case ETileType::FlyingOnly:
			return 1;
		}

		return 0;
	};

	UFUNCTION(BlueprintCallable)
	static bool IsTileTypeWalkable(const ETileType TileType)
	{
		return TileType != ETileType::None && TileType != ETileType::Obstacle;
	}
};
