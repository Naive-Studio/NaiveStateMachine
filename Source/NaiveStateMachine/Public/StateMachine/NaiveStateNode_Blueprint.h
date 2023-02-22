// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/NaiveState.h"
#include "NaiveStateNode_Blueprint.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NAIVESTATEMACHINE_API UNaiveStateNode_Blueprint : public UNaiveStateNode
{
	GENERATED_BODY()

public:
	UNaiveStateNode_Blueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnEnter(UObject* Owner, uint8* NodeMemory) override;
	virtual void OnExit(UObject* Owner, uint8* NodeMemory) override;
	virtual void OnUpdate(UObject* Owner, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "NaiveStateMachine", meta = (DisplayName = "OnEnter"))
	void Blueprint_OnEnter(UObject* Owner);

	UFUNCTION(BlueprintImplementableEvent, Category = "NaiveStateMachine", meta = (DisplayName = "OnExit"))
	void Blueprint_OnExit(UObject* Owner);

	UFUNCTION(BlueprintImplementableEvent, Category = "NaiveStateMachine", meta = (DisplayName = "OnUpdate"))
	void Blueprint_OnUpdate(UObject* Owner, float DeltaSeconds);
};
