// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTilesData.h"

#include "GridPathfinding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathfindingDataUpdatedSignature, FIntVector, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathfindingDataClearedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathfindingCompletedSignature, TArray<FIntVector>, Path);

USTRUCT(BlueprintType)
struct FPathfindingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	FIntVector Index{-1, -1, 0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	int32 CostToEnterTile = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	int32 CostFromStart = 999999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	int32 MinimumCostToTarget = 999999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	FIntVector PreviousIndex{-1, -1, 0};
};

class AGridActor;

UCLASS()
class GRID_API AGridPathfinding : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridPathfinding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ***
	// Delegates
	// ***

	UPROPERTY(BlueprintAssignable)
	FOnPathfindingDataUpdatedSignature OnPathfindingDataUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnPathfindingDataClearedSignature OnPathfindingDataCleared;

	UPROPERTY(BlueprintAssignable)
	FOnPathfindingCompletedSignature OnPathfindingCompleted;

	// ***
	// Pathfinding
	// ***

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AGridActor> Grid;

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	FIntVector StartIndex{0, 0, 0};

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	FIntVector TargetIndex{0, 0, 0};

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	bool bIncludeDiagonals = false;

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	TArray<ETileType> ValidTileTypes;

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	bool bReturnReachableTiles = false;

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	int32 MaxPathLength = 1;

	UPROPERTY(Category="Pathfinding", EditAnywhere, BlueprintReadWrite)
	float HeightReachMult = 4.0f;

	UFUNCTION(Category="Pathfinding|Generation", BlueprintCallable)
	TArray<FIntVector> FindPath(const FIntVector Start, const FIntVector Target, const bool Diagonals,
	                            const TArray<ETileType> TileTypes, const bool ReturnReachableTiles,
	                            const int32 PathLength);

	UFUNCTION(Category="Pathfinding|Generation", BlueprintCallable)
	TArray<FIntVector> GeneratePath();

	UFUNCTION(Category="Pathfinding|Generation", BlueprintCallable)
	void ClearGeneratedData();

	UFUNCTION(Category="Pathfinding|Discovery", BlueprintCallable)
	void InsertTileInDiscoveredArray(FPathfindingData TileData);

	UFUNCTION(Category="Pathfinding|Discovery", BlueprintCallable)
	void DiscoverTile(FPathfindingData TilePathData);

	UFUNCTION(Category="Pathfinding|Discovery", BlueprintCallable)
	bool DiscoverNextNeighbour();

	UFUNCTION(Category="Pathfinding|Discovery", BlueprintCallable)
	bool AnalyzeNextDiscoveredTile();

	UFUNCTION(Category="Pathfinding|Discovery", BlueprintCallable, BlueprintPure)
	FPathfindingData GetCheapestTileFromDiscoveredList();

	UFUNCTION(Category="Pathfinding|Neighbours", BlueprintCallable, BlueprintPure)
	TArray<FPathfindingData> GetValidTileNeighbours(const FIntVector Index, const bool IncludeDiagonals,
	                                                const TArray<ETileType> ValidTypes);

	UFUNCTION(Category="Pathfinding|Neighbours", BlueprintCallable, BlueprintPure)
	TArray<FIntVector> GetNeighbourIndexes(const FIntVector Index, const bool IncludeDiagonals);

	UFUNCTION(Category="Pathfinding|Utilities", BlueprintCallable, BlueprintPure)
	int32 GetMinimumCostBetweenTwoTiles(const FIntVector Index1, const FIntVector Index2, const bool IncludeDiagonals);

	UFUNCTION(Category="Pathfinding|Utilities", BlueprintCallable, BlueprintPure)
	bool IsInputDataValid();

	UFUNCTION(Category="Pathfinding|Utilities", BlueprintCallable, BlueprintPure)
	bool IsDiagonal(const FIntVector Index1, const FIntVector Index2);

	UFUNCTION(Category="Pathfinding|Utilities", BlueprintCallable, BlueprintPure)
	int32 GetTileSortingCost(const FPathfindingData TileData);

	UFUNCTION(Category="Pathfinding|Utilities", BlueprintCallable, BlueprintPure)
	int32 GetPathCost(TArray<FIntVector> Path);

private:
	TArray<FIntVector> DiscoveredTilesIndexes;

	TArray<int32> DiscoveredTileSortingCosts;

	TArray<FIntVector> AnalyzedTileIndexes;

	FPathfindingData CurrentDiscoveredTile;

	TArray<FPathfindingData> CurrentNeighbours;

	TMap<FIntVector, FPathfindingData> PathfindingData;

	FPathfindingData CurrentNeighbour;
};
