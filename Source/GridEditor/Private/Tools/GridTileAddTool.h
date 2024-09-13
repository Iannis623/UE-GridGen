// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridManager.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "GridTileAddTool.generated.h"

/**
 *  Builder for UGridGenerationTool
 */
UCLASS()
class GRIDEDITOR_API UGridTileAddToolBuilder : public UInteractiveToolBuilder
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
class GRIDEDITOR_API UGridTileAddToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UGridTileAddToolProperties();

	/** Grid reference */
	UPROPERTY(EditAnywhere, Category = "Grid")
	TObjectPtr<AGridManager> GridManager;

	/** Add pattern */
	UPROPERTY(EditAnywhere, Category = "Grid")
	EGridPattern Pattern;

	/** Add range */
	UPROPERTY(EditAnywhere, Category = "Grid")
	FIntPoint Range;
};

/**
 * UGridGenerationTool
 */
UCLASS()
class GRIDEDITOR_API UGridTileAddTool : public UInteractiveTool, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	/** IClickDragBehaviorTarget implementation */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	virtual void OnTerminateDragSequence() override {}

	virtual void OnUpdateModifierState(int ModifierID, bool bIsOn) override;
	

protected:
	UPROPERTY()
	TObjectPtr<UGridTileAddToolProperties> Properties;

	/** target World */
	TObjectPtr<UWorld> TargetWorld;

	static const int ShiftModifierID = 1;		// identifier we associate with the shift key
	bool bShiftModifierDown = false;					// flag we use to keep track of modifier state

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World
};