// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/NaiveTransitionNode_Blueprint.h"

UNaiveTransitionNode_Blueprint::UNaiveTransitionNode_Blueprint(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
}

bool UNaiveTransitionNode_Blueprint::CheckCondition(UObject* OwnerObject, uint8* NodeMemory, float DeltaTime)
{
	return Blueprint_CheckCondition(OwnerObject,DeltaTime);
}


