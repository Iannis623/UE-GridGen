// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Templates/SharedPointer.h"

struct FSlateBrush;

class GRIDEDITOR_API FGridEditorModeStyle
{
public:
	static void Initialize();

	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

	static FName GetStyleSetName();

	// use to access icons defined by the style set by name, eg GetBrush("BrushFalloffIcons.Smooth")
	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = NULL);

private:
	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

private:
	static TSharedPtr< class FSlateStyleSet > StyleSet;
};