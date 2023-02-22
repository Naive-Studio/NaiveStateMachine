// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveSMNodeBase.h"
#include "StateMachine/NaiveTransition.h"
#include "NaiveState.generated.h"

USTRUCT()
struct FNaiveStateConfig
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNaiveStateNode> StateTemplate = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<FNaiveTransitionConfig> Transitions;

	UPROPERTY(EditAnywhere)
	class UNaiveStateMachine* SubStateMachineAsset = nullptr;
};

USTRUCT()
struct FNaiveStateContext : public FNaiveNodeInstanceContext
{
	GENERATED_USTRUCT_BODY()
	
	FName StateName = NAME_None;
	
	UPROPERTY(Transient)
	UNaiveStateNode* StateNodeInstance = nullptr;
	
	TArray<FNaiveTransitionContext> ActiveTransitions;
	
	UPROPERTY(Transient)
	class UNaiveStateMachine* OwnerStateMachine = nullptr;
};

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveStateNode : public UNaiveSMNodeBase
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
