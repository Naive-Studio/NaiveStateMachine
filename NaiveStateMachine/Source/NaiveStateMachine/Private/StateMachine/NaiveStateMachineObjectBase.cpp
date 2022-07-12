// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveStateMachineObjectBase.h"
#include "NaiveStateMachineFunctionLibrary.h"
#include "NaiveStateMachineTypes.h"

UNaiveStateMachineObjectBase::UNaiveStateMachineObjectBase(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = false;
}

void UNaiveStateMachineObjectBase::Initialize(UObject* InOwnerObject, UWorld* InWorld)
{
	FastWorld = InWorld;
	OnInitialized(InOwnerObject);
}

void UNaiveStateMachineObjectBase::SendEventToMachine(int32 InHandler, FName InMessage)
{
	UNaiveStateMachineFunctionLibrary::SendEventToStateMachine(GetWorld(), FNaiveStateMachineHandle(InHandler), InMessage);
}

UWorld* UNaiveStateMachineObjectBase::GetWorld() const
{
	if(!GetOuter())
	{
		return nullptr;
	}

	if(FastWorld)
	{
		return FastWorld;
	}

	return GetOuter()->GetWorld();
}

void UNaiveStateMachineObjectBase::BeginDestroy()
{
	Super::BeginDestroy();
}

void UNaiveStateMachineObjectBase::ForceInstancing(bool bEnable)
{
	bCreateNodeInstance = bEnable;
}

bool UNaiveStateMachineObjectBase::HasInstance() const
{
	return bCreateNodeInstance;
}

uint16 UNaiveStateMachineObjectBase::GetInstanceMemorySize() const
{
	return 0;
}

void UNaiveStateMachineObjectBase::OnInitialized_Implementation(UObject* InOwner)
{
	
}


