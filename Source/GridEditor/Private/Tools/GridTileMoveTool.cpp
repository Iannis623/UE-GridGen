// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTileMoveTool.h"
#include "InteractiveToolManager.h"
#include "Engine/World.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "Kismet/GameplayStatics.h"

// for raycast into World
#include "CollisionQueryParams.h"


// localization namespace
#define LOCTEXT_NAMESPACE "GridTileMoveTool"

/*
 * ToolBuilder implementation
*/

UInteractiveTool* UGridTileMoveToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UGridTileMoveTool* NewTool = NewObject<UGridTileMoveTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * ToolProperties implementation
*/

UGridTileMoveToolProperties::UGridTileMoveToolProperties()
{
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass()));
	MoveMode = ETileMoveMode::Flatten;
	MoveAmount = 1;
	Pattern = EGridPattern::None;
	Range = FIntPoint(0, 0);
}


/*
 * Tool implementation
*/

FInputRayHit UGridTileMoveTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	bool TempBool;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp, TempBool);
	return Result;
}

void UGridTileMoveTool::OnClickPress(const FInputDeviceRay& PressPos)
{
	bFirstClick = true;
	OnClickDrag(PressPos);
}

void UGridTileMoveTool::OnClickDrag(const FInputDeviceRay& DragPos)
{
	if (Properties->GridManager)
	{
		FVector HitPos;
		bool bHitWorld;
		FindRayHit(DragPos.WorldRay, HitPos, bHitWorld);

		if (!bHitWorld)
			return;

		const FIntVector TileIndex = Properties->GridManager->GridActor->GetTileIndexFromWorldLocation(HitPos);

		// If it's the same Index, excluding Z, then we can continue
		//if (PreviousIndex == FIntPoint(TileIndex.X, TileIndex.Y))
		//	return;
		//PreviousIndex = FIntPoint(TileIndex.X, TileIndex.Y);

		

		// Shift down?
		const int32 Modifier = bShiftModifierDown ? -1 : 1;

		// Selecting multiple tiles?
		if (Properties->Pattern != EGridPattern::None && (Properties->Range.X > 0 || Properties->Range.Y > 0))
		{
			const TArray<FIntVector> Indexes = Properties->GridManager->GridActor->GetIndexesFromPatternAndRange(Properties->GridManager->GridActor->GetTileIndexFromWorldLocation(HitPos), Properties->Pattern, Properties->Range);
			
			for (FIntVector Index : Indexes)
			{
				if (!PreviousIndexes.Contains(FIntPoint(Index.X, Index.Y)))
				{
					TArray<FIntVector> Tiles = Properties->GridManager->GridActor->FindGridTilesAtIndex(FIntPoint(Index.X, Index.Y));

					for (FIntVector Tile : Tiles)
					{
						switch (Properties->MoveMode)
						{
						case ETileMoveMode::Flatten:
							if (StartingZ != Tile.Z && !bFirstClick)
							{
								Properties->GridManager->GridActor->MoveGridTile(Tile, (StartingZ - Tile.Z));
								PreviousIndexes.Emplace(FIntPoint(Tile.X, Tile.Y));
							}
							else if (TileIndex.Z == Tile.Z && bFirstClick)
							{
								StartingZ = (TileIndex.Z + Properties->MoveAmount) * Modifier;
								Properties->GridManager->GridActor->MoveGridTile(Tile, Properties->MoveAmount * Modifier);
								PreviousIndexes.Emplace(FIntPoint(Tile.X, Tile.Y));
							}
						break;

						case ETileMoveMode::Add:
							Properties->GridManager->GridActor->MoveGridTile(Tile, Properties->MoveAmount * Modifier);
							PreviousIndexes.Emplace(FIntPoint(Tile.X, Tile.Y));
						break;

						case ETileMoveMode::Preserve:
							if (StartingZ == Tile.Z && !bFirstClick)
							{
								Properties->GridManager->GridActor->MoveGridTile(Tile, Properties->MoveAmount * Modifier);
								PreviousIndexes.Emplace(FIntPoint(Tile.X, Tile.Y));
							}
							else if (TileIndex.Z == Tile.Z && bFirstClick)
							{
								StartingZ = (TileIndex.Z + Properties->MoveAmount - 1) * Modifier;
								Properties->GridManager->GridActor->MoveGridTile(Tile, Properties->MoveAmount * Modifier);
								PreviousIndexes.Emplace(FIntPoint(Tile.X, Tile.Y));
							}
						break;
						}
					}
				}
			}

			bFirstClick = false;
			return;
		}

		if (!PreviousIndexes.Contains(FIntPoint(TileIndex.X, TileIndex.Y)))
		{
			switch (Properties->MoveMode)
			{
			case ETileMoveMode::Flatten:
				if (StartingZ != TileIndex.Z && !bFirstClick)
				{
					Properties->GridManager->GridActor->MoveGridTile(TileIndex, (StartingZ - TileIndex.Z));
					PreviousIndexes.Emplace(FIntPoint(TileIndex.X, TileIndex.Y));
				}
				else if (bFirstClick)
				{
					StartingZ = (TileIndex.Z + Properties->MoveAmount) * Modifier;
					Properties->GridManager->GridActor->MoveGridTile(TileIndex, Properties->MoveAmount * Modifier);
					PreviousIndexes.Emplace(FIntPoint(TileIndex.X, TileIndex.Y));
				}
			break;
				
			case ETileMoveMode::Add:
				Properties->GridManager->GridActor->MoveGridTile(TileIndex, Properties->MoveAmount * Modifier);
				PreviousIndexes.Emplace(FIntPoint(TileIndex.X, TileIndex.Y));
			break;

			case ETileMoveMode::Preserve:
				if (StartingZ == TileIndex.Z && !bFirstClick)
				{
					Properties->GridManager->GridActor->MoveGridTile(TileIndex, Properties->MoveAmount * Modifier);
					PreviousIndexes.Emplace(FIntPoint(TileIndex.X, TileIndex.Y));
				}
				else if (bFirstClick)
				{
					StartingZ = (TileIndex.Z + Properties->MoveAmount - 1) * Modifier;
					Properties->GridManager->GridActor->MoveGridTile(TileIndex, Properties->MoveAmount * Modifier);
					PreviousIndexes.Emplace(FIntPoint(TileIndex.X, TileIndex.Y));
				}
			break;
			}
			bFirstClick = false;
		}
	}
}

void UGridTileMoveTool::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
	PreviousIndexes.Empty();
}


FInputRayHit UGridTileMoveTool::FindRayHit(const FRay& WorldRay, FVector& HitPos, bool& HitWorld)
{
	 // trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	 FHitResult Result;

	// @TODO: Iannis - Implement world trace by changing it to a LineTraceSingleByChannel
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);
	HitWorld = bHitWorld;
	if (bHitWorld)
	{
		HitPos = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}
	return FInputRayHit();
}


void UGridTileMoveTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void UGridTileMoveTool::Setup()
{
	UInteractiveTool::Setup();

	// Add default mouse input behavior
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	// We will use the shift key to indicate that we should move the second point. 
	// This call tells the Behavior to call our OnUpdateModifierState() function on mouse-down and mouse-move
	MouseBehavior->Modifiers.RegisterModifier(ShiftModifierID, FInputDeviceState::IsShiftKeyDown);
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<UGridTileMoveToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UGridTileMoveTool::OnUpdateModifierState(int ModifierID, bool bIsOn)
{
	// keep track of the "second point" modifier (shift key for mouse input)
	if (ModifierID == ShiftModifierID)
	{
		bShiftModifierDown = bIsOn;
	}
}

#undef LOCTEXT_NAMESPACE