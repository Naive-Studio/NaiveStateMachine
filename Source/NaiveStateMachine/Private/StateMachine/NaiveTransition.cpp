// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveTransition.h"

bool FNaiveTransitionContext::IsListeningEvents(const TArray<FName>& InEvents) const
{
	const TArray<FName>& ObservingEvents = TransitionConfig ? TransitionConfig->ObservingEvents : TArray<FName>(nullptr, 0);
	if (TransitionConfig && ObservingEvents.Num() > 0 && InEvents.Num() > 0)
	{
		for (auto& Event : InEvents)
		{
			if (ObservingEvents.Find(Event)!=INDEX_NONE)
			{
				return true;
			}
		}
	}
	return false;
}

void FNaiveTransitionContext::SetTransitionConfig(const FNaiveTransitionConfig* InTransitionConfig)
{
	TransitionConfig = InTransitionConfig;
}

void FNaiveTransitionContext::SetTransitionNodeInstance(UNaiveTransitionNode* NodeInstance)
{
	TransitionNodeInstance = NodeInstance;
}

UNaiveTransitionNode::UNaiveTransitionNode(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

bool UNaiveTransitionNode::CheckCondition(UObject* OwnerObject, uint8* NodeMemory, float DeltaTime)
{
	return false;
}

void UNaiveTransitionNode::Initialize(UObject* InOwnerObject, UWorld* InWorld)
{
	Super::Initialize(InOwnerObject, InWorld);
	
}
