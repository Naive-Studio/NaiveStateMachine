// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeAI.h"
#include "NaiveStateMachineNode_State.h"
#include "NaiveStateMachineNode_SubEntry.generated.h"

class UNaiveStateNode;
/**
 * 
 */
UCLASS(hideCategories = ("State", GraphNode))
class UNaiveStateMachineNode_SubEntry : public UNaiveStateMachineNode_State
{
	GENERATED_BODY()
public:
	UNaiveStateMachineNode_SubEntry();
};

class SNaiveStateMachineNode_SubEntry : public SNaiveStateMachineNode_State
{
public:
	SLATE_BEGIN_ARGS(SNaiveStateMachineNode_SubEntry) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UNaiveStateMachineGraphNode* InNode);

	// SGraphNode interface
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface
};
