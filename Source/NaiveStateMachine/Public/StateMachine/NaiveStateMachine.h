// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Blueprint.h"
#include "Templates/SubclassOf.h"
#include "NaiveStateMachineTypes.h"
#include "NaiveStateMachine.generated.h"


class UNaiveStateNode;
class UNaiveTransitionNode;

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveStateMachine : public UDataAsset
{
	GENERATED_BODY()

public:
	UNaiveStateMachine();
	virtual  void PostInitProperties() override;

	TMap<FName, FNaiveState>& GetStateContextsMutable() { return RegisteredStateMachineContext; }
	const TMap<FName, FNaiveState>& GetStateContexts() const { return RegisteredStateMachineContext; }

	void SetDefaultState(const FName& InState);
	const FName& GetDefaultState() const;

	TArray<FNaiveTransition>& GetEntryTransitionsMutable() { return SubEntryTransitions; }
	const TArray<FNaiveTransition>& GetEntryTransitions() const { return SubEntryTransitions; }
public:
#if WITH_EDITORONLY_DATA
	/** Graph for state machine */
	UPROPERTY()
	UEdGraph* EdGraph;

	/** Info about the graphs we last edited */
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif

	static const TCHAR StateNameJointMark;

protected:
	UPROPERTY()
	TMap<FName, FNaiveState> RegisteredStateMachineContext;

	UPROPERTY()
	TArray<FNaiveTransition> SubEntryTransitions;

	UPROPERTY()
	FName DefaultState = NAIVE_INVALID_STATE;

	UPROPERTY()
	FName StateMachineName;
};
