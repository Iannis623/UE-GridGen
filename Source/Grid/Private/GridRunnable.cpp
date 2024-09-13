// Fill out your copyright notice in the Description page of Project Settings.


#include "GridRunnable.h"

// This code will be run on the thread that invoked this thread (i.e. game thread)
#pragma region Main Thread Code

FGridRunnable::FGridRunnable()
{
}

void FGridRunnable::CreateThread( FString ThreadName )
{
	// Constructs the actual thread object. It will begin execution immediately
	// If you've passed in any inputs, set them up before calling this.
	Thread = FRunnableThread::Create(this, *ThreadName);
}

bool FGridRunnable::IsRunning()
{
	return bRunThread && !bDone;
}

void FGridRunnable::Cancel( bool bMarkAsFailed )
{
	bRunThread = false;
	bFailed = bMarkAsFailed;
}

bool FGridRunnable::Failed()
{
	return bFailed;
}

FGridRunnable::~FGridRunnable()
{
}

void FGridRunnable::Cleanup()
{
	OnCleanup();
	
	if (Thread)
	{
		// Kill() is a blocking call, it waits for the thread to finish.
		// Hopefully that doesn't take too long
		Thread->Kill();
		delete Thread;
		Thread = nullptr;
	}
}

void FGridRunnable::OnCleanup()
{
}

// The code below will run on the new thread.
#pragma endregion

bool FGridRunnable::Init()
{
	return FRunnable::Init();
}

uint32 FGridRunnable::Run()
{
	return 0;
}

// Note that this function is NOT run on the new thread!
void FGridRunnable::Stop()
{
	// Clean up memory usage here, and make sure the Run() function stops soon
	// The main thread will be stopped until this finishes!

	bRunThread = false;
}

