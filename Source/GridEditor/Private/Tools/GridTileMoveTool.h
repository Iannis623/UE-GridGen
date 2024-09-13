// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridManager.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "GridTileMoveTool.generated.h"

UENUM()
enum class ETileMoveMode : uint8
{
	Flatten		UMETA(DisplayName="Flatten"),
	Add			UMETA(DisplayName="Add"),
	Preserve	UMETA(DisplayName="Preserve")
};

/**
 *  Builder for UGridGenerationTool
 */
UCLASS()
class GRIDEDITOR_API UGridTileMoveToolBuilder : public UInteractiveToolBuilder
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
class GRIDEDITOR_API UGridTileMoveToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UGridTileMoveToolProperties();

	/** Grid reference */
	UPROPERTY(EditAnywhere, Category = "Grid")
	TObjectPtr<AGridManager> GridManager;

	/** Move Mode */
	UPROPERTY(EditAnywhere, Category = "Grid")
	ETileMoveMode MoveMode;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 MoveAmount;

	/** Move pattern */
	UPROPERTY(EditAnywhere, Category = "Grid")
	EGridPattern Pattern;

	/** Move range */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FIntPoint Range;
};

/**
 * UGridGenerationTool
 */
UCLASS()
class GRIDEDITOR_API UGridTileMoveTool : public UInteractiveTool, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	/** IClickDragBehaviorTarget implementation */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
	virtual void OnTerminateDragSequence() override {}

	virtual void OnUpdateModifierState(int ModifierID, bool bIsOn) override;
	

protected:
	UPROPERTY()
	TObjectPtr<UGridTileMoveToolProperties> Properties;

	/** target World */
	TObjectPtr<UWorld> TargetWorld;

	static const int ShiftModifierID = 1;								// identifier we associate with the shift key
	bool bShiftModifierDown = false;									// flag we use to keep track of modifier state
	TArray<FIntPoint> PreviousIndexes;									// used to track previous index so we don't keep modifying height of same tile, don't consider Z
	bool bFirstClick = true;											// used to track if it's the first click for filtering reasons, we can always enforce the first move
	int32 StartingZ = 0;												// used to track the starting Z when clicking, useful for out of bounds situations

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos, bool& HitWorld);		// raycasts into World
};