// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSelectTool.h"
#include "InteractiveToolManager.h"
#include "Engine/World.h"

// localization namespace
#define LOCTEXT_NAMESPACE "GridSelectTool"

/*
 * ToolBuilder implementation
*/

UInteractiveTool* UGridSelectToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UGridSelectTool* NewTool = NewObject<UGridSelectTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * ToolProperties implementation
*/

UGridSelectToolProperties::UGridSelectToolProperties()
{
}


/*
 * Tool implementation
*/

void UGridSelectTool::SetWorld(UWorld* World)
{
	this->TargetWorld = World;
}

void UGridSelectTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UGridSelectToolProperties>(this);
	AddToolPropertySource(Properties);
}

#undef LOCTEXT_NAMESPACE