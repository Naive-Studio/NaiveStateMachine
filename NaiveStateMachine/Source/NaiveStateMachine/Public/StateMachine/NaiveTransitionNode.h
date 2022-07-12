// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineObjectBase.h"
#include "NaiveTransitionNode.generated.h"

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveTransitionNode : public UNaiveStateMachineObjectBase
{
	GENERATED_BODY()

public:
	UNaiveTransitionNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Initialize(UObject* InOwnerObject, UWorld* InWorld) override;
	
	virtual bool CheckCondition(UObject* OwnerObject, uint8* NodeMemory, float DeltaTime);
};
