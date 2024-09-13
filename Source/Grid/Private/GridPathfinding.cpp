// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPathfinding.h"
#include "GridActor.h"
#include "Algo/Reverse.h"


// Sets default values
AGridPathfinding::AGridPathfinding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AGridPathfinding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridPathfinding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<FIntVector> AGridPathfinding::FindPath(const FIntVector Start, const FIntVector Target, const bool Diagonals, const TArray<ETileType> TileTypes, const bool ReturnReachableTiles, const int32 PathLength)
{
	StartIndex = Start;
	TargetIndex = Target;
	bIncludeDiagonals = Diagonals;
	ValidTileTypes = TileTypes;
	bReturnReachableTiles = ReturnReachableTiles;
	MaxPathLength = PathLength;

	ClearGeneratedData();

	if (!IsInputDataValid())
	{
		OnPathfindingCompleted.Broadcast(TArray<FIntVector>());
		return TArray<FIntVector>();
	}

	DiscoverTile(
		FPathfindingData(
			StartIndex,
			1,
			0,
			GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bIncludeDiagonals)));

	TArray<FIntVector> Path;
	while (DiscoveredTilesIndexes.Num() > 0)
	{
		// Path found
		if (AnalyzeNextDiscoveredTile())
		{
			Path = GeneratePath();
			
			OnPathfindingCompleted.Broadcast(Path);
			return Path;
		}
	}

	// No Path found
	Path = bReturnReachableTiles ? AnalyzedTileIndexes : TArray<FIntVector>();
	
	OnPathfindingCompleted.Broadcast(Path);
	return Path;
}


TArray<FIntVector> AGridPathfinding::GeneratePath()
{
	FIntVector CurrentIndex = TargetIndex;
	TArray<FIntVector> InvertedPath;

	while (CurrentIndex != StartIndex)
	{
		InvertedPath.Emplace(CurrentIndex);
		CurrentIndex = PathfindingData.Find(CurrentIndex)->PreviousIndex;
	}

	Algo::Reverse(InvertedPath);

	return InvertedPath;
}

void AGridPathfinding::ClearGeneratedData()
{
	PathfindingData.Empty();
	DiscoveredTileSortingCosts.Empty();
	DiscoveredTilesIndexes.Empty();
	AnalyzedTileIndexes.Empty();

	OnPathfindingDataCleared.Broadcast();
}

void AGridPathfinding::InsertTileInDiscoveredArray(FPathfindingData TileData)
{
	int32 SortingCost = GetTileSortingCost(TileData);

	if (DiscoveredTileSortingCosts.Num() == 0)
	{
		DiscoveredTileSortingCosts.Emplace(SortingCost);
		DiscoveredTilesIndexes.Emplace(TileData.Index);
	}
	else
	{
		if (SortingCost >= DiscoveredTileSortingCosts.Last())
		{
			DiscoveredTileSortingCosts.Emplace(SortingCost);
			DiscoveredTilesIndexes.Emplace(TileData.Index);
		}
		else
		{
			for (int32 i = 0; i < DiscoveredTileSortingCosts.Num(); ++i)
			{
				if (DiscoveredTileSortingCosts[i] >= SortingCost)
				{
					DiscoveredTileSortingCosts.Insert(SortingCost, i);
					DiscoveredTilesIndexes.Insert(TileData.Index, i);

					return;
				}
			}
		}
	}
}

void AGridPathfinding::DiscoverTile(FPathfindingData TilePathData)
{
	PathfindingData.Emplace(TilePathData.Index, TilePathData);
	
	InsertTileInDiscoveredArray(TilePathData);

	OnPathfindingDataUpdated.Broadcast(TilePathData.Index);
}

bool AGridPathfinding::DiscoverNextNeighbour()
{
	CurrentNeighbour = CurrentNeighbours[0];
	CurrentNeighbours.RemoveAt(0);

	if (AnalyzedTileIndexes.Contains(CurrentNeighbour.Index))
	{
		return false;
	}

	const int32 CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbour.CostToEnterTile;
	if (CostFromStart > MaxPathLength)
	{
		return false;
	}

	const int32 IndexInDiscovered = DiscoveredTilesIndexes.Find(CurrentNeighbour.Index);
	// not new neighbour?
	if (IndexInDiscovered != -1)
	{
		CurrentNeighbour = PathfindingData.FindRef(CurrentNeighbour.Index);
		if (CostFromStart >= CurrentNeighbour.CostFromStart)
		{
			return false;
		}

		DiscoveredTilesIndexes.RemoveAt(IndexInDiscovered);
		DiscoveredTileSortingCosts.RemoveAt(IndexInDiscovered);
	}

	DiscoverTile(
		FPathfindingData(
			CurrentNeighbour.Index,
			CurrentNeighbour.CostToEnterTile,
			CostFromStart,
			GetMinimumCostBetweenTwoTiles(CurrentNeighbour.Index, TargetIndex, bIncludeDiagonals),
			CurrentDiscoveredTile.Index));

	return CurrentNeighbour.Index == TargetIndex;
}

bool AGridPathfinding::AnalyzeNextDiscoveredTile()
{
	CurrentDiscoveredTile = GetCheapestTileFromDiscoveredList();

	OnPathfindingDataUpdated.Broadcast(CurrentDiscoveredTile.Index);

	CurrentNeighbours = GetValidTileNeighbours(CurrentDiscoveredTile.Index, bIncludeDiagonals, ValidTileTypes);

	while (CurrentNeighbours.Num() > 0)
	{
		if (DiscoverNextNeighbour())
		{
			// this is the target!
			return true;
		}	
	}

	return false;
}

FPathfindingData AGridPathfinding::GetCheapestTileFromDiscoveredList()
{
	const FIntVector TileIndex = DiscoveredTilesIndexes[0];

	DiscoveredTileSortingCosts.RemoveAt(0);
	DiscoveredTilesIndexes.RemoveAt(0);

	return PathfindingData.FindRef(TileIndex);
}

TArray<FPathfindingData> AGridPathfinding::GetValidTileNeighbours(const FIntVector Index, const bool IncludeDiagonals, const TArray<ETileType> ValidTypes)
{
	const FGridTileData* InputData = Grid->GetGridTiles().Find(Index);
	TArray<FPathfindingData> ValidTileNeighbours;

	TArray<FIntVector> Neighbours = GetNeighbourIndexes(Index, IncludeDiagonals);
	for (FIntVector Neighbour : Neighbours)
	{
		FGridTileData* Data = Grid->GetGridTiles().Find(Neighbour);

		// if tile is a valid type and there's no unit on the tile and if the height is within height reach
		if (ValidTypes.Contains(Data->Type) && !Data->UnitOnTile && abs(Data->Transform.GetLocation().Z - InputData->Transform.GetLocation().Z) <= Grid->GridTileSize.Z * HeightReachMult )
		{
			ValidTileNeighbours.Add(
				FPathfindingData(
						Data->Index,
						UGridTilesData::GetTileTypeCost(Data->Type),
						999999,
						999999,
						Index));
		}
	}

	return ValidTileNeighbours;
}

TArray<FIntVector> AGridPathfinding::GetNeighbourIndexes(const FIntVector Index, const bool IncludeDiagonals)
{
	FIntVector Up{1, 0, 0};
	FIntVector Right{0, 1, 0};
	FIntVector Down{-1, 0, 0};
	FIntVector Left{0, -1, 0};

	Up += Index;
	Right += Index;
	Down += Index;
	Left += Index;

	if (!IncludeDiagonals)
	{
		return {Up, Right, Down, Left};
	}
	
	FIntVector UpRight{1, 1, 0};
	FIntVector DownRight{-1, 1, 0};
	FIntVector DownLeft{-1, -1, 0};
	FIntVector UpLeft{1, -1, 0};

	UpRight += Index;
	DownRight += Index;
	DownLeft += Index;
	UpLeft += Index;

	return {Up, Right, Down, Left, UpRight, DownRight, DownLeft, UpLeft};
}

int32 AGridPathfinding::GetMinimumCostBetweenTwoTiles(const FIntVector Index1, const FIntVector Index2, const bool IncludeDiagonals)
{
	const FIntVector LocalIndex = Index1 - Index2;
	
	return IncludeDiagonals ? FMath::Max(abs(LocalIndex.X), abs(LocalIndex.Y)) : abs(LocalIndex.X) + abs(LocalIndex.Y);
}

bool AGridPathfinding::IsInputDataValid()
{
	// @TODO: Condense down eventually
	
	if (StartIndex == TargetIndex)
	{
		return false;
	}

	if (!Grid->IsTileWalkable(StartIndex))
	{
		return false;
	}

	if (bReturnReachableTiles)
	{
		return true;
	}

	if (!Grid->IsTileWalkable(TargetIndex))
	{
		return false;
	}

	if (GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bIncludeDiagonals) > MaxPathLength)
	{
		return false;
	}

	if (!ValidTileTypes.Contains(Grid->GetGridTiles().Find(TargetIndex)->Type))
	{
		return false;
	}

	return IsValid(Grid->GetGridTiles().Find(TargetIndex)->UnitOnTile);
}

bool AGridPathfinding::IsDiagonal(const FIntVector Index1, const FIntVector Index2)
{
	return !GetNeighbourIndexes(Index1, false).Contains(Index2);
}

int32 AGridPathfinding::GetTileSortingCost(const FPathfindingData TileData)
{
	return 2 * (TileData.CostFromStart + TileData.MinimumCostToTarget) + IsDiagonal(TileData.Index, TileData.PreviousIndex);
}

int32 AGridPathfinding::GetPathCost(TArray<FIntVector> Path)
{
	int32 PathCost = 0;

	for (FIntVector Tile : Path)
	{
		PathCost += UGridTilesData::GetTileTypeCost(Grid->GetGridTiles().Find(Tile)->Type);
	}

	return PathCost;
}
