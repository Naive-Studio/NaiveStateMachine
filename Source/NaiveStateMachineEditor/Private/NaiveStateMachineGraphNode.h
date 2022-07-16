// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraphNode.h"
#include "NaiveStateMachineGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class UNaiveStateMachineGraphNode : public UAIGraphNode
{
	GENERATED_BODY()
	
public:
	virtual class UNaiveStateMachineGraph* GetStateMachineGraph();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetDescription() const override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;

	// @return the name of this state
	virtual FString GetNodeName() const { return "DefaultNodeName"; }

	virtual class UClass* GetTemplateClass() const { return nullptr; }

	// Populates the OutTransitions array with a list of transition nodes connected to this state
	NAIVESTATEMACHINEEDITOR_API virtual void GetTransitionList(TArray<class UNaiveStateMachineNode_Transition*>& OutTransitions, bool bWantSortedList = false) { }

};
