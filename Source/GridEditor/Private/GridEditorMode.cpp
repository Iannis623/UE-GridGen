// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridEditorMode.h"
#include "GridEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "GridEditorModeCommands.h"
#include "GridEditorModeStyle.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"

#include "Tools/GridGenerationTool.h"
#include "Tools/GridSelectTool.h"
#include "Tools/GridTileAddTool.h"
#include "Tools/GridTileDeleteTool.h"
#include "Tools/GridTileMoveTool.h"


// step 2: register a ToolBuilder in FGridEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "GridEditorMode"

const FEditorModeID UGridEditorMode::EM_GridEditorModeId = TEXT("EM_GridEditorMode");

FString UGridEditorMode::GenerationToolName = TEXT("Grid_GenerationTool");
FString UGridEditorMode::SelectToolName = TEXT("Grid_SelectTool");
FString UGridEditorMode::TileAddToolName = TEXT("Grid_TileAddTool");
FString UGridEditorMode::TileDeleteToolName = TEXT("Grid_TileDeleteTool");
FString UGridEditorMode::TileMoveToolName = TEXT("Grid_TileMoveTool");


UGridEditorMode::UGridEditorMode()
{
	//FModuleManager::Get().LoadModule("EditorStyle");
	
	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UGridEditorMode::EM_GridEditorModeId,
		LOCTEXT("ModeName", "Grid"),
		FSlateIcon(FGridEditorModeStyle::GetStyleSetName(), "GridEditorMode.GridEdMode"),
		true);
}

UGridEditorMode::~UGridEditorMode()
{
}


void UGridEditorMode::ActorSelectionChangeNotify()
{
}

void UGridEditorMode::Enter()
{
	UEdMode::Enter();
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FGridEditorModeCommands& SampleToolCommands = FGridEditorModeCommands::Get();
	
	RegisterTool(SampleToolCommands.GenerationTool, GenerationToolName, NewObject<UGridGenerationToolBuilder>(this));
	RegisterTool(SampleToolCommands.SelectTool, SelectToolName, NewObject<UGridSelectToolBuilder>(this));
	RegisterTool(SampleToolCommands.TileAddTool, TileAddToolName, NewObject<UGridTileAddToolBuilder>(this));
	RegisterTool(SampleToolCommands.TileDeleteTool, TileDeleteToolName, NewObject<UGridTileDeleteToolBuilder>(this));
	RegisterTool(SampleToolCommands.TileMoveTool, TileMoveToolName, NewObject<UGridTileMoveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, GenerationToolName);
	GetToolManager()->ActivateTool(EToolSide::Left);
	
}

void UGridEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FGridEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UGridEditorMode::GetModeCommands() const
{
	return FGridEditorModeCommands::Get().GetCommands();
}

void UGridEditorMode::OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
	SelectedToolName = Manager->GetActiveToolName(EToolSide::Left);
}

void UGridEditorMode::OnToolEnded(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
	if (false)
	{
		Manager->SelectActiveToolType(EToolSide::Left, SelectedToolName);
		Manager->ActivateTool(EToolSide::Left);
	}
}


#undef LOCTEXT_NAMESPACE
