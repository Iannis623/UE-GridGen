// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActor.h"

#include "GridUtilities.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridGenerateInstancesWorker.h"
#include "Async/Async.h"

// Sets default values
AGridActor::AGridActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GridComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GridComponent"));
	RootComponent = GridComponent;

	GridTilesData = CreateDefaultSubobject<UGridTilesData>(TEXT("GridTiles"));

	InitializeInstances(GridMesh, GridMaterial);

	GridGenerateInstancesWorker = nullptr;
}

// Called when the game starts or when spawned
void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGridActor::BeginDestroy()
{
	if (GridGenerateInstancesWorker)
	{
		delete GridGenerateInstancesWorker;
		GridGenerateInstancesWorker = nullptr;
	}
	Super::BeginDestroy();
}


// Called every frame
void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ***
// Grid Generation
// ***

void AGridActor::SpawnGrid(const FVector CenterLocation, const FVector TileSize, const FIntPoint TileCount, const bool bFastGen)
{
	GridCenterLocation = CenterLocation;
	GridTileSize = TileSize;
	GridTileCount = TileCount;

	DestroyGrid();

	CalculateCenterAndBottomLeft(GridCenterLocation, GridBottomLeftCorner);
	
	GridGenerateInstancesWorker = new FGridGenerateInstancesWorker(this, GridCenterLocation, GridTileSize, GridTileCount, GridBottomLeftCorner, bFastGen);
	GridGenerateInstancesWorker->CreateThread("GridThread");
}


void AGridActor::GenerateGridTiles(const TArray<FTransform> Transforms, const TArray<FIntVector> Indexes, const TMap<FIntVector, FGridTileData> GridTiles, const TMap<FIntPoint, FTileHeightTranslator> TileHeightTranslator) const
{
	GridComponent->AddInstances(Transforms, false, false, false);
	GetGridTiles() = GridTiles;
	GetInstanceIndexes() = Indexes;
	GetTileHeightTranslator() = TileHeightTranslator;
}


void AGridActor::AddGridTiles(const TArray<FIntVector> Indexes) const
{
	if (Indexes.IsEmpty())
		return;

	TArray<FTransform> Transforms;

	for (FIntVector Index : Indexes)
	{
		// If present already, we skip adding, preserving the uniqueness
		if (!IsIndexValid(Index))
		{
			FTransform Transform = FTransform(
				FRotator(0,0,0),
				GetTileLocationFromGridIndex(Index),
				GetTileScale());
					
			Transforms.Emplace(Transform);
			
			GetInstanceIndexes().Emplace(Index);
			GetGridTiles().Emplace(Index, FGridTileData(Index, ETileType::Normal, Transform));
			AddTileToTranslator(Index);
		}
	}
	
	GridComponent->AddInstances(Transforms, false, false, false);
}

void AGridActor::RemoveGridTiles(const TArray<FIntVector> Indexes)
{
	if (Indexes.IsEmpty())
		return;
	
	TArray<int32> InstanceIndexes;
	TArray<FIntVector> InstanceIndexesToRemove;

	for (FIntVector Index : Indexes)
	{
		if (IsIndexValid(Index))
		{
			InstanceIndexes.Emplace(GetInstanceIndexes().Find(Index));
			InstanceIndexesToRemove.Emplace(Index);
			GetGridTiles().Remove(Index);
			RemoveTileFromTranslator(Index);
		}
	}
	
	GridComponent->RemoveInstances(InstanceIndexes);

	for (FIntVector InstanceIndexToRemove : InstanceIndexesToRemove)
	{
		GetInstanceIndexes().Remove(InstanceIndexToRemove);
	}
}


void AGridActor::AddGridTile(FGridTileData Data) const
{
	if (Data.Type != ETileType::None && IsWithinBounds(Data.Index))
	{
		GetGridTiles().Emplace(Data.Index, Data);
		RemoveTileFromTranslator(Data.Index);
		AddTileToTranslator(Data.Index);
		AddInstance(Data);
	}
}

void AGridActor::RemoveGridTile(const FIntVector Index) const
{
	if (GetGridTiles().Remove(Index) > 0)
	{
		RemoveTileFromTranslator(Index);
		RemoveInstance(Index);
	}
}

void AGridActor::ClearGridTiles() const
{
	GetGridTiles().Empty();
}


void AGridActor::DestroyGrid()
{
	ClearGridTiles();
	ClearInstances();
}


void AGridActor::CalculateCenterAndBottomLeft(FVector& CenterLocation, FVector& BottomLeftCornerLocation) const
{
	const FVector LocalCenterLocation = UGridUtilities::SnapVectorToVector(GridCenterLocation, GridTileSize);

	CenterLocation = LocalCenterLocation;
	BottomLeftCornerLocation = LocalCenterLocation - (GridTileSize * FVector(((GridTileCount - FIntPoint(GridTileCount.X % 2 == 0 ? 0 : 1, GridTileCount.Y % 2 == 0 ? 0 : 1)) / 2.0f),0.0f));
	
}


// ***
// Grid Utilities
// ***

FVector AGridActor::GetTileLocationFromGridIndex(const FIntVector Index) const
{
	return GridBottomLeftCorner + (GridTileSize * FVector(Index));
}

FVector AGridActor::GetTileScale() const
{
	return GridTileSize / 100.f;
}

FIntVector AGridActor::GetTileIndexFromWorldLocation(const FVector Location) const
{
	const FVector LocationOnGrid = Location - GridBottomLeftCorner;

	return FIntVector(UGridUtilities::SnapVectorToVector(LocationOnGrid, GridTileSize) / GridTileSize);
}

TMap<FIntVector, FGridTileData>& AGridActor::GetGridTiles() const
{
	return  GridTilesData->GridTiles;
}

TArray<FIntVector>& AGridActor::GetInstanceIndexes() const
{
	return GridTilesData->InstanceIndexes;
}

TMap<FIntPoint, FTileHeightTranslator>& AGridActor::GetTileHeightTranslator() const
{
	return GridTilesData->TileHeightTranslator;
}

TArray<FIntVector> AGridActor::FindGridTilesAtIndex(const FIntPoint Index) const
{
	 return GetTileHeightTranslator().FindRef(Index).Translator;
}

bool AGridActor::IsIndexValid(const FIntVector Index) const
{
	return GetGridTiles().Contains(Index);
}

bool AGridActor::IsWithinBounds(const FIntVector Index) const
{
	return (Index.X >= 0 && Index.X <= GridTileCount.X) && (Index.Y >= 0 && Index.Y <= GridTileCount.Y);
}

bool AGridActor::IsTileWalkable(const FIntVector Index) const
{
	return UGridTilesData::IsTileTypeWalkable(GetGridTiles().Find(Index)->Type);
}


void AGridActor::AddTileToTranslator(FIntVector Index) const
{
	FTileHeightTranslator Translator = GetTileHeightTranslator().FindRef(FIntPoint(Index.X, Index.Y));
	Translator.Translator.Emplace(Index);
	GetTileHeightTranslator().Emplace(FIntPoint(Index.X, Index.Y), Translator);
}

void AGridActor::RemoveTileFromTranslator(FIntVector Index) const
{
	FTileHeightTranslator Translator = GetTileHeightTranslator().FindRef(FIntPoint(Index.X, Index.Y));
	Translator.Translator.Remove(Index);
	
	if (Translator.Translator.IsEmpty())
	{
		GetTileHeightTranslator().Remove(FIntPoint(Index.X, Index.Y));
	}
	else
	{
		GetTileHeightTranslator().Emplace(FIntPoint(Index.X, Index.Y), Translator);
	}

}



//	***
// Instance code
//	***

void AGridActor::AddInstance(const FGridTileData& Data) const
{
	RemoveInstance(Data.Index);

	GridComponent->AddInstance(Data.Transform);

	GetInstanceIndexes().Emplace(Data.Index);
}

void AGridActor::RemoveInstance(const FIntVector Index) const
{
	if (GetInstanceIndexes().Contains(Index))
	{
		GridComponent->RemoveInstance(GetInstanceIndexes().Find(Index));
		GetInstanceIndexes().Remove(Index);
	}
}

void AGridActor::ClearInstances() const
{
	GridComponent->ClearInstances();
	GetInstanceIndexes().Empty();
}

void AGridActor::MoveGridTile(const FIntVector Index, const int32 MoveAmount)
{
	/*if (IsIndexValid(FIntVector(Index.X, Index.Y, Index.Z + 1 * MoveAmount)))
	{
		MoveGridTile(Index, MoveAmount + MoveAmount);
	}*/
	
	const FGridTileData Data = GetGridTiles().FindRef(Index);
	RemoveGridTile(Index);

	const FTransform Transform = FTransform(
	 Data.Transform.GetRotation(),
	 FVector(Data.Transform.GetLocation().X, Data.Transform.GetLocation().Y, Data.Transform.GetLocation().Z + GridTileSize.Z * MoveAmount),
	 Data.Transform.GetScale3D()
	 );
            
	AddGridTile(
		FGridTileData(
			 FIntVector(Data.Index.X, Data.Index.Y, Data.Index.Z + 1 * MoveAmount),
			 Data.Type,
			Transform,
			Data.States
			));
}


void AGridActor::InitializeInstances(UStaticMesh* Mesh, UMaterialInstance* Material)
{
	GridMesh = Mesh;
	GridMaterial = Material;
	
	GridComponent->SetStaticMesh(Mesh);
	GridComponent->SetMaterial(0, Material);
}

// ***
// Grid Patterns
// ***

TArray<FIntVector> AGridActor::GetIndexesFromPatternAndRange(FIntVector OriginIndex, const EGridPattern Pattern, const FIntPoint Range)
{
	switch (Pattern)
	{
	case EGridPattern::None:
		{
			TArray<FIntVector> Result;
			Result.Emplace(OriginIndex);
			return Result;
		}
		
	case EGridPattern::Line:
		return OffsetIndexArray(GetPatternLine(Range), OriginIndex);
		
	case EGridPattern::Diagonal:
		return OffsetIndexArray(GetPatternDiagonal(Range), OriginIndex);
		
	case EGridPattern::HalfDiagonal:
		return OffsetIndexArray(GetPatternHalfDiagonal(Range), OriginIndex);
		
	case EGridPattern::Star:
		return OffsetIndexArray(GetPatternStar(Range), OriginIndex);
		
	case EGridPattern::Diamond:
		return OffsetIndexArray(GetPatternDiamond(Range), OriginIndex);
		
	case EGridPattern::Square:
		return OffsetIndexArray(GetPatternSquare(Range), OriginIndex);
	}

	return TArray<FIntVector>();
}

TArray<FIntVector> AGridActor::OffsetIndexArray(TArray<FIntVector> Array, const FIntVector Offset)
{
	TArray<FIntVector> Result;
	
	for (FIntVector Element : Array)
	{
		Result.Emplace(Element + Offset);
	}

	return Result;
}

TArray<FIntVector> AGridActor::GetPatternLine(const FIntPoint Range)
{
	TArray<FIntVector> Result;
	
	for (int32 i = Range.X; i <= Range.Y; ++i)
	{
		// Y = Right
		Result.AddUnique(FIntVector(0, i, 0));
		// Y = Left
		Result.AddUnique(FIntVector(0, -i, 0));
		// X = Up
		Result.AddUnique(FIntVector(i, 0, 0));
		// X = Down
		Result.AddUnique(FIntVector(-i, 0, 0));
	}

	return Result;
}

TArray<FIntVector> AGridActor::GetPatternDiagonal(const FIntPoint Range)
{
	TArray<FIntVector> Result;
	
	for (int32 i = Range.X; i <= Range.Y; ++i)
	{
		// X = Up	Y = Right
		Result.AddUnique(FIntVector(i, i, 0));
		// X = Down	Y = Left
		Result.AddUnique(FIntVector(-i, -i, 0));
		// X = Up	Y = Left
		Result.AddUnique(FIntVector(i, -i, 0));
		// X = Down	Y = Right
		Result.AddUnique(FIntVector(-i, i, 0));
	}

	return Result;
}

TArray<FIntVector> AGridActor::GetPatternHalfDiagonal(const FIntPoint Range)
{
	return GetPatternDiagonal(FIntPoint(Range.X, Range.Y / 2));
}

TArray<FIntVector> AGridActor::GetPatternStar(const FIntPoint Range)
{
	TArray<FIntVector> Result = GetPatternLine(Range);
	TArray<FIntVector> Diagonal = GetPatternDiagonal(Range);

	Result.Append(Diagonal);

	return  Result;
}

TArray<FIntVector> AGridActor::GetPatternDiamond(const FIntPoint Range)
{
	TArray<FIntVector> Result;
	
	for (int32 i = Range.X; i <= Range.Y; ++i)
	{
		for (int32 j = 0; j <= i; ++j)
		{
			// X = Up	Y = Left to Right
			Result.AddUnique(FIntVector(j, -(i - j), 0));
			// X = Up to Down	Y = Right
			Result.AddUnique(FIntVector(i - j, j, 0));
			// X = Down	Y = Right to Left
			Result.AddUnique(FIntVector(-j, i - j, 0));
			// X = Down to Up	Y = Left
			Result.AddUnique(FIntVector(-(i - j), -j, 0));
		}
	}

	return Result;
}

TArray<FIntVector> AGridActor::GetPatternSquare(const FIntPoint Range)
{
	TArray<FIntVector> Result;
	
	for (int32 i = Range.X; i <= Range.Y; ++i)
	{
		for (int32 j = -i; j <= i; ++j)
		{
			// X = Down to Up	Y = Left
			Result.AddUnique(FIntVector(j, -i, 0));
			// X = Up	Y = Left to Right
			Result.AddUnique(FIntVector(i, j, 0));
			// X = Up to Down	Y = Right
			Result.AddUnique(FIntVector(-j, i, 0));
			// X = Down	Y = Right to Left
			Result.AddUnique(FIntVector(-i, -j, 0));
		}
	}

	return Result;
}





