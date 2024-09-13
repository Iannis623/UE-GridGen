// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "GridSelectTool.generated.h"

/**
 *  Builder for UGridSelectTool
 */
UCLASS()
class GRIDEDITOR_API UGridSelectToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/**
 * Settings UObject for UGridSelectTool. This UClass inherits from UInteractiveToolPropertySet,
 * which provides an OnModified delegate that the Tool will listen to for changes in property values.
 */
UCLASS(Transient)
class GRIDEDITOR_API UGridSelectToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UGridSelectToolProperties();
};

/**
 * UGridSelectTool
 */
UCLASS()
class GRIDEDITOR_API UGridSelectTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

protected:
	UPROPERTY()
	TObjectPtr<UGridSelectToolProperties> Properties;

	/** target World */
	TObjectPtr<UWorld> TargetWorld;
};