// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridEditorModeToolkit.h"
#include "GridEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "GridEditorModeToolkit"

FGridEditorModeToolkit::FGridEditorModeToolkit()
{
}

void FGridEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FGridEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FGridEditorModeToolkit::GetToolkitFName() const
{
	return FName("GridEditorMode");
}

FText FGridEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "GridEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
