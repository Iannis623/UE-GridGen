// Fill out your copyright notice in the Description page of Project Settings.

#include "GridEditorModeStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FGridEditorModeStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )

// This is to fix the issue that SlateStyleMacros like IMAGE_BRUSH look for RootToContentDir but StyleSet->RootToContentDir is how this style is set up
#define RootToContentDir StyleSet->RootToContentDir

FString FGridEditorModeStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("Grid"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FGridEditorModeStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FGridEditorModeStyle::Get() { return StyleSet; }

FName FGridEditorModeStyle::GetStyleSetName()
{
	static FName GridStyleName(TEXT("GridStyle"));
	return GridStyleName;
}

const FSlateBrush* FGridEditorModeStyle::GetBrush(FName PropertyName, const ANSICHAR* Specifier)
{
	return Get()->GetBrush(PropertyName, Specifier);
}

void FGridEditorModeStyle::Initialize()
{
	// Texture size definitions
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

	// If we get asked for something that we don't set, we should default to editor style
	StyleSet->SetParentStyleName("EditorStyle");

	// Icons path
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("Grid"))->GetContentDir());
	StyleSet->SetCoreContentRoot(IPluginManager::Get().FindPlugin(TEXT("Grid"))->GetContentDir());
	
	
	// Editor Mode icon
	StyleSet->Set("GridEditorMode.GridEdMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_16px", Icon16x16));

	// Tool Manager icons
	StyleSet->Set("GridEditorMode.GenerationTool", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_New_40px", Icon20x20));
	StyleSet->Set("GridEditorMode.TileAddTool", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_Add_40px", Icon20x20));
	StyleSet->Set("GridEditorMode.TileDeleteTool", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_Delete_40px", Icon20x20));
	StyleSet->Set("GridEditorMode.TileMoveTool", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_Move_40px", Icon20x20));
	StyleSet->Set("GridEditorMode.ImportTool", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_Import_40px", Icon20x20));
	StyleSet->Set("GridEditorMode.SelectTool", new IMAGE_PLUGIN_BRUSH("Icons/icon_Grid_Select_40px", Icon20x20));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}
#undef IMAGE_PLUGIN_BRUSH

void FGridEditorModeStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}