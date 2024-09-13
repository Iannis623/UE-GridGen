// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridEditorModeCommands.h"
#include "GridEditorMode.h"
#include "EditorStyleSet.h"
#include "GridEditorModeStyle.h"
#include "Styling/SlateStyleMacros.h"

#define LOCTEXT_NAMESPACE "GridEditorModeCommands"

FGridEditorModeCommands::FGridEditorModeCommands()
	: TCommands<FGridEditorModeCommands>("GridEditorMode",
		NSLOCTEXT("GridEditorMode", "GridEditorModeCommands", "Grid Editor Mode"),
		NAME_None,
		FGridEditorModeStyle::GetStyleSetName())
{
}

void FGridEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(GenerationTool, "New", "Generates Grid based on provided values", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(GenerationTool);

	UI_COMMAND(SelectTool, "Select", "Select Tool", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(SelectTool);

	UI_COMMAND(TileAddTool, "Add", "Add Tiles to the Grid", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(TileAddTool);

	UI_COMMAND(TileDeleteTool, "Delete", "Delete Grid Tiles", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(TileDeleteTool);

	UI_COMMAND(TileMoveTool, "Move", "Move Grid Tiles", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(TileMoveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FGridEditorModeCommands::GetCommands()
{
	return FGridEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
