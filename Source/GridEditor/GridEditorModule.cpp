// Copyright Epic Games, Inc. All Rights Reserved.

#include "GridEditorModule.h"
#include "GridEditor/Public/GridEditorModeCommands.h"
#include "GridEditor/Public/GridEditorModeStyle.h"

#define LOCTEXT_NAMESPACE "GridModule"

void FGridEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGridEditorModule::OnPostEngineInit);
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FGridEditorModeCommands::Register();
}

void FGridEditorModule::ShutdownModule()
{
	FGridEditorModeStyle::Shutdown();
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FGridEditorModeCommands::Unregister();
}

void FGridEditorModule::OnPostEngineInit()
{
	FGridEditorModeStyle::Initialize();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGridEditorModule, GridEditorMode)