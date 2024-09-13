// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGenerationTool.h"
#include "InteractiveToolManager.h"
#include "Engine/World.h"

// localization namespace
#define LOCTEXT_NAMESPACE "GridGenerationTool"

/*
 * ToolBuilder implementation
*/

UInteractiveTool* UGridGenerationToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UGridGenerationTool* NewTool = NewObject<UGridGenerationTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * ToolProperties implementation
*/

UGridGenerationToolProperties::UGridGenerationToolProperties()
{
	GridManager = nullptr;
	Mesh = nullptr;
	Material = nullptr;
	CenterLocation = FVector(0.0f, 0.0f, 0.0f);
	TileSize = FVector(100.0f, 100.0f, 50.0f);
	TileCount = FIntPoint(100, 100);
}

void UGridGenerationToolProperties::Generate() const
{
	if (GridManager)
	{
		GridManager->GridActor->InitializeInstances(Mesh, Material);
		GridManager->GridActor->SpawnGrid(CenterLocation, TileSize, TileCount);
	}
	else
	{
		AGridManager* TempGridManager = GetWorld()->SpawnActor<AGridManager>(CenterLocation, FRotator::ZeroRotator);
		TempGridManager->InitializeGridManager();
		TempGridManager->GridActor->InitializeInstances(Mesh, Material);
		TempGridManager->GridActor->SpawnGrid(CenterLocation, TileSize, TileCount);
	}
}

void UGridGenerationToolProperties::FastBorderGen()
{
	if (GridManager)
	{
		GridManager->GridActor->InitializeInstances(Mesh, Material);
		GridManager->GridActor->SpawnGrid(CenterLocation, TileSize, TileCount, true);
	}
	else
	{
		AGridManager* TempGridManager = GetWorld()->SpawnActor<AGridManager>(CenterLocation, FRotator::ZeroRotator);
		TempGridManager->InitializeGridManager();
		TempGridManager->GridActor->InitializeInstances(Mesh, Material);
		TempGridManager->GridActor->SpawnGrid(CenterLocation, TileSize, TileCount, true);
	}
}



/*
 * Tool implementation
*/

void UGridGenerationTool::SetWorld(UWorld* World)
{
	this->TargetWorld = World;
}

void UGridGenerationTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UGridGenerationToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UGridGenerationTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	// @TODO: Iannis - There must be a better way to check the Property
	if (Property->GetName() == "GridManager" && Properties->GridManager)
	{
		Properties->Material = Properties->GridManager->GridActor->GridMaterial;
		Properties->Mesh = Properties->GridManager->GridActor->GridMesh;
		Properties->CenterLocation = Properties->GridManager->GridActor->GridCenterLocation;
		Properties->TileCount = Properties->GridManager->GridActor->GridTileCount;
		Properties->TileSize = Properties->GridManager->GridActor->GridTileSize;
	}
}


#undef LOCTEXT_NAMESPACE