// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveSMNodeBase.h"
#include "NaiveStateMachineFunctionLibrary.h"
#include "NaiveStateMachineTypes.h"

UNaiveSMNodeBase::UNaiveSMNodeBase(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = false;
}

void UNaiveSMNodeBase::Initialize(UObject* InOwnerObject, UWorld* InWorld)
{
	FastWorld = InWorld;
	OnInitialized(InOwnerObject);
}

void UNaiveSMNodeBase::SendEventToMachine(int32 InHandler, FName InMessage)
{
	UNaiveStateMachineFunctionLibrary::SendEventToStateMachine(GetWorld(), FNaiveStateMachineHandle(InHandler), InMessage);
}

UWorld* UNaiveSMNodeBase::GetWorld() const
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

void UNaiveSMNodeBase::BeginDestroy()
{
	Super::BeginDestroy();
}

void UNaiveSMNodeBase::ForceInstancing(bool bEnable)
{
	bCreateNodeInstance = bEnable;
}

bool UNaiveSMNodeBase::HasInstance() const
{
	return bCreateNodeInstance;
}

uint16 UNaiveSMNodeBase::GetInstanceMemorySize() const
{
	return 0;
}

void UNaiveSMNodeBase::OnInitialized_Implementation(UObject* InOwner)
{
	
}


