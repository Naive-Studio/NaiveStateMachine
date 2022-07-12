// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineGraphNode.h"
#include "NaiveStateMachineGraphSchema.h"
#include "NaiveStateMachineGraph.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/KismetEditorUtilities.h"

/////////////////////////////////////////////////////
// FNaiveStateNodeNameValidator

class FNaiveStateMachineGraphNodeNameValidator : public FStringSetNameValidator
{
public:
	FNaiveStateMachineGraphNodeNameValidator(const UNaiveStateMachineGraphNode* InStateNode)
		: FStringSetNameValidator(FString())
	{
		TArray<UNaiveStateMachineGraphNode*> Nodes;
		UNaiveStateMachineGraph* StateMachine = CastChecked<UNaiveStateMachineGraph>(InStateNode->GetGraph());

		StateMachine->GetNodesOfClass<UNaiveStateMachineGraphNode>(Nodes);
		for (auto NodeIt = Nodes.CreateIterator(); NodeIt; ++NodeIt)
		{
			auto Node = *NodeIt;
			if (Node != InStateNode)
			{
				Names.Add(Node->GetNodeName());
			}
		}
	}
};

/////////////////////////////////////////////////////
// UNaiveStateMachineGraphNode

class UNaiveStateMachineGraph* UNaiveStateMachineGraphNode::GetStateMachineGraph()
{
	return CastChecked<UNaiveStateMachineGraph>(GetGraph());
}

FText UNaiveStateMachineGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::GetEmpty();
}

FText UNaiveStateMachineGraphNode::GetDescription() const
{
	return FText::GetEmpty();
}

UObject* UNaiveStateMachineGraphNode::GetJumpTargetForDoubleClick() const
{
	return GetTemplateClass();
}

bool UNaiveStateMachineGraphNode::CanJumpToDefinition() const
{
	return GetJumpTargetForDoubleClick() != nullptr;
}

void UNaiveStateMachineGraphNode::JumpToDefinition() const
{
	if (UObject* HyperlinkTarget = GetJumpTargetForDoubleClick())
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HyperlinkTarget);
	}
}

void UNaiveStateMachineGraphNode::OnRenameNode(const FString& NewName)
{
	Super::OnRenameNode(NewName);
}

TSharedPtr<class INameValidatorInterface> UNaiveStateMachineGraphNode::MakeNameValidator() const
{
	return MakeShareable(new FNaiveStateMachineGraphNodeNameValidator(this));
}

bool UNaiveStateMachineGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const
{
	return DesiredSchema->IsA(UNaiveStateMachineGraphSchema::StaticClass());
}
