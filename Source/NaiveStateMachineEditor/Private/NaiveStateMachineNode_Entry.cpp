// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineNode_Entry.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "NaiveStateMachineGraphSchema.h"
#include "NaiveStateMachineEditorColors.h"

#define LOCTEXT_NAMESPACE "NaivwStateMachineEntryNode"


UNaiveStateMachineNode_Entry::UNaiveStateMachineNode_Entry()
{
	bIsReadOnly = true;
}

void UNaiveStateMachineNode_Entry::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, UNaiveStateMachineGraphSchema::PC_Entry, TEXT("Entry"));
}

FText UNaiveStateMachineNode_Entry::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::GetEmpty();
}

FText UNaiveStateMachineNode_Entry::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UNaiveStateMachineNode_Entry::GetOutputNode() const
{
	if (Pins.Num() > 0 && Pins[0] != NULL)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if (Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != NULL)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}
	return NULL;
}


#undef LOCTEXT_NAMESPACE

void SNaiveStateMachineNode_Entry::Construct(const FArguments& InArgs, UNaiveStateMachineNode_Entry* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SNaiveStateMachineNode_Entry::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);

	ContentScale.Bind(this, &SGraphNode::GetContentScale);
	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(0)
		.BorderBackgroundColor(NaiveStateMachineEditorColors::NodeBorder::Default)
		[
			SNew(SOverlay)

			// PIN AREA
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(10.0f)
		[
			SAssignNew(RightNodeBox, SVerticalBox)
		]
		]
		];

	CreatePinWidgets();
}