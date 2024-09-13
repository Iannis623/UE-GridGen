// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTileDeleteTool.h"
#include "InteractiveToolManager.h"
#include "Engine/World.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "Kismet/GameplayStatics.h"

// for raycast into World
#include "CollisionQueryParams.h"


// localization namespace
#define LOCTEXT_NAMESPACE "GridTileDeleteTool"

/*
 * ToolBuilder implementation
*/

UInteractiveTool* UGridTileDeleteToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UGridTileDeleteTool* NewTool = NewObject<UGridTileDeleteTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * ToolProperties implementation
*/

UGridTileDeleteToolProperties::UGridTileDeleteToolProperties()
{
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass()));
	Pattern = EGridPattern::None;
	Range = FIntPoint(0, 0);
}


/*
 * Tool implementation
*/

FInputRayHit UGridTileDeleteTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
	return Result;
}

void UGridTileDeleteTool::OnClickPress(const FInputDeviceRay& PressPos)
{
	OnClickDrag(PressPos);
}


void UGridTileDeleteTool::OnClickDrag(const FInputDeviceRay& DragPos)
{
	if (!Properties->GridManager)
		return;
	
	FVector HitPos;
	FindRayHit(DragPos.WorldRay, HitPos);

	
	// Selecting multiple tiles?
	if (Properties->Pattern != EGridPattern::None && (Properties->Range.X > 0 || Properties->Range.Y > 0))
	{
		const TArray<FIntVector> Indexes = Properties->GridManager->GridActor->GetIndexesFromPatternAndRange(Properties->GridManager->GridActor->GetTileIndexFromWorldLocation(HitPos), Properties->Pattern, Properties->Range);
		
		// Shift down?
		if (!bShiftModifierDown)
		{
			Properties->GridManager->GridActor->RemoveGridTiles(Indexes);
		}
		else
		{
			Properties->GridManager->GridActor->AddGridTiles(Indexes);
		}
		
		return;
	}

	// Single tile
	// Shift down?
	if (!bShiftModifierDown)
	{
		Properties->GridManager->GridActor->RemoveGridTile(Properties->GridManager->GridActor->GetTileIndexFromWorldLocation(HitPos));
	}
	else
	{
		Properties->GridManager->GridActor->AddGridTile(
			FGridTileData(
				Properties->GridManager->GridActor->GetTileIndexFromWorldLocation(HitPos),
				ETileType::Normal,
				FTransform(
		FRotator(0,0,0),
		Properties->GridManager->GridActor->GetTileLocationFromGridIndex(Properties->GridManager->GridActor->GetTileIndexFromWorldLocation(HitPos)),
					Properties->GridManager->GridActor->GetTileScale()
					)));
	}
}

FInputRayHit UGridTileDeleteTool::FindRayHit(const FRay& WorldRay, FVector& HitPos)
{
	 // trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	 FHitResult Result;

	// @TODO: Iannis - Implement world trace by changing it to a LineTraceSingleByChannel
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);
	if (bHitWorld)
	{
		HitPos = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}

	if (Properties->GridManager)
	{
		// If nothing hit, we simulate a plane and make the system think it hit something
		const FPlane Plane = FPlane(Properties->GridManager->GridActor->GridCenterLocation, FVector(0.0f, 0.0f,1.0f));
		HitPos = FMath::LinePlaneIntersection(WorldRay.Origin, WorldRay.PointAt(999999), Plane);
		return FInputRayHit(0);
	}
	
	return FInputRayHit();
}


void UGridTileDeleteTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void UGridTileDeleteTool::Setup()
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
	Properties = NewObject<UGridTileDeleteToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UGridTileDeleteTool::OnUpdateModifierState(int ModifierID, bool bIsOn)
{
	// keep track of the "second point" modifier (shift key for mouse input)
	if (ModifierID == ShiftModifierID)
	{
		bShiftModifierDown = bIsOn;
	}
}

#undef LOCTEXT_NAMESPACE