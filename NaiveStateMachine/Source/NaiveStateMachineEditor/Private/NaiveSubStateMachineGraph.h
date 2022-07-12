// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveSubStateMachineGraph.generated.h"

/**
 * 
 */
UCLASS()
class UNaiveSubStateMachineGraph : public UNaiveStateMachineGraph
{
	GENERATED_BODY()
public:
	UNaiveSubStateMachineGraph();

	virtual void UpdateAsset(int32 UpdateFlags = 0) override;

	// Entry node within the state machine
	UPROPERTY()
	class UNaiveStateMachineNode_SubEntry* EntryNode;

	// Parent instance node
	UPROPERTY()
	class UNaiveStateMachineNode_State* OwnerGraphNode;

	// Parent instance graph
	UPROPERTY()
	class UNaiveStateMachineGraph* ParentGraph;
};
