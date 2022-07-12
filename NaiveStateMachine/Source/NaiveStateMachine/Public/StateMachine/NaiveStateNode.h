// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineObjectBase.h"
#include "NaiveStateNode.generated.h"

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveStateNode : public UNaiveStateMachineObjectBase
{
	GENERATED_BODY()

public:
	UNaiveStateNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnEnter(UObject* Owner, uint8* NodeMemory);
	virtual void OnExit(UObject* Owner, uint8* NodeMemory);
	virtual void OnUpdate(UObject* Owner, uint8* NodeMemory, float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "FSM")
	void SetParentState(UNaiveStateNode* InParent);

	UFUNCTION(BlueprintCallable, Category = "FSM")
	UNaiveStateNode* GetParentState() const;
	
protected:
	UPROPERTY()
	UNaiveStateNode* ParentState = nullptr;
};
