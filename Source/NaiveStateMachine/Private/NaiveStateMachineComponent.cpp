// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineComponent.h"
#include "StateMachine/NaiveStateMachine.h"
#include "NaiveStateMachineFunctionLibrary.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UNaiveStateMachineComponent::UNaiveStateMachineComponent()
{
}


// Called when the game starts
void UNaiveStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultStateMachine)
	{
		RunStateMachine(DefaultStateMachine);
	}
}


void UNaiveStateMachineComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopStateMachine();

	Super::EndPlay(EndPlayReason);
}

void UNaiveStateMachineComponent::SendEvent(FName InEvent)
{
	if (StateMachineHandle.IsValid())
	{
		UNaiveStateMachineFunctionLibrary::SendEventToStateMachine(this, StateMachineHandle, InEvent);
	}
}


void UNaiveStateMachineComponent::RunStateMachine(UNaiveStateMachine* InStateMachine)
{
	FNaiveRunStateMachineRequest Request;
	Request.Owner = GetOuter();
	Request.World = GetWorld();
	Request.Template = InStateMachine;
	Request.OnStateChanged.BindUObject(this, &UNaiveStateMachineComponent::OnStateChanged);

	StateMachineHandle = UNaiveStateMachineFunctionLibrary::RunStateMachine(this, Request);
}


void UNaiveStateMachineComponent::StopStateMachine()
{
	if (StateMachineHandle.IsValid())
	{
		UNaiveStateMachineFunctionLibrary::StopStateMachine(this, StateMachineHandle);
		StateMachineHandle.Invalidate();
	}
}

void UNaiveStateMachineComponent::OnStateChanged(FName OldState, FName NewState)
{
	OnStateChangeEvent.Broadcast(OldState, NewState);
}

