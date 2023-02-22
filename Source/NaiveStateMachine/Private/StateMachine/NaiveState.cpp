// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveState.h"

UNaiveStateNode::UNaiveStateNode(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

void UNaiveStateNode::OnEnter(UObject* Owner, uint8* NodeMemory)
{
	//Blueprint_OnEnter(Owner);
}

void UNaiveStateNode::OnExit(UObject* Owner, uint8* NodeMemory)
{
	//Blueprint_OnExit(Owner);
}

void UNaiveStateNode::OnUpdate(UObject* Owner, uint8* NodeMemory, float DeltaSeconds)
{
	//Blueprint_OnUpdate(Owner, DeltaSeconds);
}

void UNaiveStateNode::SetParentState(UNaiveStateNode* InParent)
{
	ParentState = InParent;
}

UNaiveStateNode* UNaiveStateNode::GetParentState() const
{
	return ParentState;
}
