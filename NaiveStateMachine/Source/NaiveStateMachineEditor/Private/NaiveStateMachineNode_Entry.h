// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineGraphNode.h"
#include "SGraphNodeAI.h"
#include "NaiveStateMachineNode_Entry.generated.h"

/**
 * 
 */
UCLASS()
class UNaiveStateMachineNode_Entry : public UNaiveStateMachineGraphNode
{
	GENERATED_BODY()
public:

	UNaiveStateMachineNode_Entry();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	//~ End UEdGraphNode Interface

	NAIVESTATEMACHINEEDITOR_API UEdGraphNode* GetOutputNode() const;
};


class SNaiveStateMachineNode_Entry : public SGraphNodeAI
{
public:
	SLATE_BEGIN_ARGS(SNaiveStateMachineNode_Entry) {}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs, UNaiveStateMachineNode_Entry* InNode);

	virtual void UpdateGraphNode() override;
};