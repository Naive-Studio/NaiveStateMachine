// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineFunctionLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "NaiveStateMachineManager.h"

FNaiveStateMachineHandle UNaiveStateMachineFunctionLibrary::RunStateMachine(UObject* ContextObject, const FNaiveRunStateMachineRequest& InRequest)
{
	if (UNaiveStateMachineManager* SMManager = UNaiveStateMachineFunctionLibrary::GetStateMachineManager(ContextObject))
	{
		return SMManager->StartStateMachine(InRequest);
	}

	return FNaiveStateMachineHandle::HANDLE_NONE;
}

void UNaiveStateMachineFunctionLibrary::StopStateMachine(UObject* ContextObject, const FNaiveStateMachineHandle& InHandle)
{
	if (!InHandle.IsValid())
	{
		return;
	}

	if (UNaiveStateMachineManager* SMManager = UNaiveStateMachineFunctionLibrary::GetStateMachineManager(ContextObject))
	{
		SMManager->StopStateMachine(InHandle);
	}
}


FName UNaiveStateMachineFunctionLibrary::GetActiveState(UObject* ContextObject, const FNaiveStateMachineHandle& InHandle)
{
	if (!InHandle.IsValid())
	{
		return NAME_None;
	}

	if (UNaiveStateMachineManager* SMManager = UNaiveStateMachineFunctionLibrary::GetStateMachineManager(ContextObject))
	{
		return SMManager->GetActiveStateName(InHandle);
	}

	return NAME_None;
}

class UNaiveStateMachineManager* UNaiveStateMachineFunctionLibrary::GetStateMachineManager(UObject* ContextObject)
{
	return Cast<UNaiveStateMachineManager>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(ContextObject,UNaiveStateMachineManager::StaticClass()));
}

void UNaiveStateMachineFunctionLibrary::SendEventToStateMachine(UObject* ContextObject, const FNaiveStateMachineHandle& InHandle, const FName& EventName)
{
	if (!InHandle.IsValid())
	{
		return;
	}

	if (UNaiveStateMachineManager* SMManager = UNaiveStateMachineFunctionLibrary::GetStateMachineManager(ContextObject))
	{
		SMManager->SendEventToTransition(InHandle, EventName);
	}
}

