// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

class GRID_API FGridRunnable : public FRunnable
{
public:
	FGridRunnable();
	virtual ~FGridRunnable() override;

	void Cleanup();
	virtual void OnCleanup();
	void CreateThread( FString ThreadName );
		
	TObjectPtr<UObject> WorldContext = nullptr;
	
	bool Init() override;
	uint32 Run() override;
	
	void Stop() override;
	
	virtual bool IsRunning();
	void Cancel( bool bMarkAsFailed );

	bool Failed();
	
protected:
	FRunnableThread* Thread = nullptr;

	bool bRunThread = true;
	bool bDone = false;
	bool bFailed = false;
};
