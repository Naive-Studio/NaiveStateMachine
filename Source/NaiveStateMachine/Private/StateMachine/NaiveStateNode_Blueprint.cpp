// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveStateNode_Blueprint.h"

UNaiveStateNode_Blueprint::UNaiveStateNode_Blueprint(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
}

void UNaiveStateNode_Blueprint::OnEnter(UObject* Owner, uint8* NodeMemory)
{
	Blueprint_OnEnter(Owner);
}

void UNaiveStateNode_Blueprint::OnExit(UObject* Owner, uint8* NodeMemory)
{
	Blueprint_OnExit(Owner);
}

void UNaiveStateNode_Blueprint::OnUpdate(UObject* Owner, uint8* NodeMemory, float DeltaSeconds)
{
	Blueprint_OnUpdate(Owner,DeltaSeconds);
}
