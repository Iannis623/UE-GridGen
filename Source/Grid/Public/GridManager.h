// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridActor.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class GRID_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category="Grid", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AGridActor> GridActor;

	UFUNCTION()
	void InitializeGridManager();
};
