// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"


// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridManager::InitializeGridManager()
{
	UWorld* World = GEditor->GetEditorWorldContext().World();
    GridActor = World->SpawnActor<AGridActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	GridActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}


