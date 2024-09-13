// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridManager.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "GridGenerationTool.generated.h"

/**
 *  Builder for UGridGenerationTool
 */
UCLASS()
class GRIDEDITOR_API UGridGenerationToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/**
 * Settings UObject for UGridGenerationTool. This UClass inherits from UInteractiveToolPropertySet,
 * which provides an OnModified delegate that the Tool will listen to for changes in property values.
 */
UCLASS(Transient)
class GRIDEDITOR_API UGridGenerationToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UGridGenerationToolProperties();

	/** Grid reference, assign if you want to modify an existing Grid */
	UPROPERTY(EditAnywhere, Category = "Grid")
	TObjectPtr<AGridManager> GridManager;

	/** Grid Mesh */
	UPROPERTY(EditAnywhere, Category = "Grid")
	TObjectPtr<UStaticMesh> Mesh;
	
	/** Grid Material */
	UPROPERTY(EditAnywhere, Category = "Grid")
	TObjectPtr<UMaterialInstance> Material;

	/** Center of the Grid */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector CenterLocation;

	/** Size of each Tile */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector TileSize;

	/** Number of Tiles */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FIntPoint TileCount;

	/** Generate the Grid fully */
	UFUNCTION(CallInEditor, Category = "Grid")
	void Generate() const;

	/** Generate the Grid with only borders filled, way faster */
	UFUNCTION(CallInEditor, Category = "Grid")
	void FastBorderGen();
};

/**
 * UGridGenerationTool
 */
UCLASS()
class GRIDEDITOR_API UGridGenerationTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

protected:
	UPROPERTY()
	TObjectPtr<UGridGenerationToolProperties> Properties;

	/** target World */
	TObjectPtr<UWorld> TargetWorld;
};