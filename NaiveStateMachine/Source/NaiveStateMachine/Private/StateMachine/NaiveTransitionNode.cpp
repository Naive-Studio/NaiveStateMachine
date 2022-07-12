// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveTransitionNode.h"

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
