// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGenerateInstancesWorker.h"
#include "Async/Async.h"
#include "GridActor.h"

#pragma region Main Thread Code
FGridGenerateInstancesWorker::FGridGenerateInstancesWorker(UObject* InWorldContext, FVector CenterLocation, FVector TileSize, FIntPoint TileCount, FVector BottomLeftCorner, bool bFastGen)
{
	WorldContext = InWorldContext;
	GridCenterLocation = CenterLocation;
	GridTileSize = TileSize;
	GridTileCount = TileCount;
	GridBottomLeftCorner = BottomLeftCorner;
	bGridFastGen = bFastGen;
	
}

FGridGenerateInstancesWorker::~FGridGenerateInstancesWorker()
{
	Cleanup();
}
#pragma endregion

bool FGridGenerateInstancesWorker::Init()
{
	return FGridRunnable::Init();
}

uint32 FGridGenerateInstancesWorker::Run()
{
	if (!bGridFastGen)
	{
		for (int32 x = 0; x < GridTileCount.X; ++x)
		{
			for (int32 y = 0; y < GridTileCount.Y; ++y)
			{
				FillData(
					FGridTileData(FIntVector(x,y,0),
					ETileType::Normal,
					FTransform(FRotator(0.0f, 0.0f, 0.0f),
								GetTileLocationFromGridIndex(FIntVector(x,y,0)),
								GetTileScale())));
			}
		}
	}
	// Only generate borders
	else
	{
		for (int32 x = 0; x <= GridTileCount.X; ++x)
		{
			FillData(
				FGridTileData(FIntVector(x,0,0),
				ETileType::Normal,
				FTransform(FRotator(0.0f, 0.0f, 0.0f),
							GetTileLocationFromGridIndex(FIntVector(x,0,0)),
							GetTileScale())));
			FillData(
				FGridTileData(FIntVector(x,GridTileCount.Y,0),
				ETileType::Normal,
				FTransform(FRotator(0.0f, 0.0f, 0.0f),
							GetTileLocationFromGridIndex(FIntVector(x,GridTileCount.Y,0)),
							GetTileScale())));
		}
		for (int32 y = 1; y < GridTileCount.Y; ++y)
		{
			FillData(
				FGridTileData(FIntVector(0,y,0),
				ETileType::Normal,
				FTransform(FRotator(0.0f, 0.0f, 0.0f),
							GetTileLocationFromGridIndex(FIntVector(0,y,0)),
							GetTileScale())));
			FillData(
				FGridTileData(FIntVector(GridTileCount.X,y,0),
				ETileType::Normal,
				FTransform(FRotator(0.0f, 0.0f, 0.0f),
							GetTileLocationFromGridIndex(FIntVector(GridTileCount.X,y,0)),
							GetTileScale())));
		}
	}

	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		AGridActor* Grid = Cast<AGridActor>(WorldContext);
		Grid->GenerateGridTiles(Transforms, InstanceIndexes, GridTiles, TileHeightTranslator);
	});
	
	bDone = true;

	// ? I hear that it's good to let the thread sleep a bit, so the OS can utilise it better or something.
	// ^- https://unrealcommunity.wiki/multithreading-with-frunnable-2a4xuf68
	FPlatformProcess::Sleep(0.01f);
	
	return 0;
}

void FGridGenerateInstancesWorker::FillData(FGridTileData Data)
{
	GridTiles.Emplace(Data.Index, Data);
	InstanceIndexes.Emplace(Data.Index);
	Transforms.Emplace(Data.Transform);

	FTileHeightTranslator Translator = TileHeightTranslator.FindRef(FIntPoint(Data.Index.X, Data.Index.Y));
	Translator.Translator.Emplace(Data.Index);
	TileHeightTranslator.Emplace(FIntPoint(Data.Index.X, Data.Index.Y), Translator);
}

FVector FGridGenerateInstancesWorker::GetTileLocationFromGridIndex(const FIntVector Index) const
{
	return GridBottomLeftCorner + (GridTileSize * FVector(Index));
}

FVector FGridGenerateInstancesWorker::GetTileScale() const
{
	return GridTileSize / 100.0f;
}
	

void FGridGenerateInstancesWorker::Stop()
{
	FGridRunnable::Stop();
}




