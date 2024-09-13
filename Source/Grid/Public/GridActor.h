// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTilesData.h"
#include "GridActor.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class GRID_API AGridActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	class FGridGenerateInstancesWorker* GridGenerateInstancesWorker;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ***
	// Grid Properties
	// ***

	UPROPERTY(Category="Grid", VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInstancedStaticMeshComponent> GridComponent;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> GridMesh;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> GridMaterial;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	FVector GridCenterLocation;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	FVector GridTileSize;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	FIntPoint GridTileCount;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	FVector GridBottomLeftCorner;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGridTilesData> GridTilesData;

	// ***
	// Grid Generation
	// ***

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void SpawnGrid(const FVector CenterLocation, const FVector TileSize, const FIntPoint TileCount, const bool bFastGen = false);
	
	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void GenerateGridTiles(const TArray<FTransform> Transforms, const TArray<FIntVector> Indexes, const TMap<FIntVector, FGridTileData> GridTiles, const TMap<FIntPoint, FTileHeightTranslator> TileHeightTranslator) const;

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void AddGridTiles(const TArray<FIntVector> Indexes) const;

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void RemoveGridTiles(const TArray<FIntVector> Indexes);

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void AddGridTile(FGridTileData Data) const;

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void RemoveGridTile(const FIntVector Index) const;

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void ClearGridTiles() const;

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void MoveGridTile(const FIntVector Index, const int32 MoveAmount);

	UFUNCTION(Category="Grid|Generation", BlueprintCallable)
	void DestroyGrid();

	UFUNCTION(Category="Grid|Generation", BlueprintCallable, BlueprintPure)
	void CalculateCenterAndBottomLeft(FVector& CenterLocation, FVector& BottomLeftCornerLocation) const;

	// ***
	// Grid Utilities
	// ***

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	FVector GetTileLocationFromGridIndex(const FIntVector Index) const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	FVector GetTileScale() const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	FIntVector GetTileIndexFromWorldLocation(const FVector Location) const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	TMap<FIntVector, FGridTileData>& GetGridTiles() const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	TArray<FIntVector>& GetInstanceIndexes() const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	TArray<FIntVector> FindGridTilesAtIndex(const FIntPoint Index) const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	bool IsIndexValid(const FIntVector Index) const;
	
	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	bool IsWithinBounds(const FIntVector Index) const;

	UFUNCTION(Category="Grid|Utilities", BlueprintCallable, BlueprintPure)
	bool IsTileWalkable(const FIntVector Index) const;

	UFUNCTION(Category="Instances", BlueprintCallable)
	void InitializeInstances(UStaticMesh* Mesh, UMaterialInstance* Material);

	// ***
	// Grid Patterns
	// ***
	
	UFUNCTION(Category="Grid|Patterns", BlueprintCallable, BlueprintPure)
	static TArray<FIntVector> GetIndexesFromPatternAndRange(FIntVector OriginIndex, const EGridPattern Pattern, const FIntPoint Range);
	

private:

	// ***
	// Instances
	// ***
	
	void AddInstance(const FGridTileData& Data) const;
	
	void RemoveInstance(const FIntVector Index) const;
	
	void ClearInstances() const;

	// ***
	// Grid Patterns
	// ***
	
	static TArray<FIntVector> OffsetIndexArray(TArray<FIntVector> Array, const FIntVector Offset);
	
	static TArray<FIntVector> GetPatternLine(const FIntPoint Range);
	
	static TArray<FIntVector> GetPatternDiagonal(const FIntPoint Range);
	
	static TArray<FIntVector> GetPatternHalfDiagonal(const FIntPoint Range);
	
	static TArray<FIntVector> GetPatternStar(const FIntPoint Range);
	
	static TArray<FIntVector> GetPatternDiamond(const FIntPoint Range);
	
	static TArray<FIntVector> GetPatternSquare(const FIntPoint Range);

	// ***
	// Utilities
	// ***

	TMap<FIntPoint, FTileHeightTranslator>& GetTileHeightTranslator() const;

	void AddTileToTranslator(FIntVector Index) const;

	void RemoveTileFromTranslator(FIntVector Index) const;
};
