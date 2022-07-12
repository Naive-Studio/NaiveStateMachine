// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/NaiveTransitionNode.h"
#include "NaiveTransitionNode_Blueprint.generated.h"

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveTransitionNode_Blueprint : public UNaiveTransitionNode
{
	GENERATED_BODY()
public:
	UNaiveTransitionNode_Blueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool CheckCondition(UObject* OwnerObject, uint8* NodeMemory, float DeltaTime) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "NaiveStateMachine", meta = (DisplayName = "CheckCondition"))
	bool Blueprint_CheckCondition(UObject* OwnerObject, float DeltaTime);
};
