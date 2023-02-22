// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineNode_SubEntry.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineGraphPin.h"
#include "Kismet2/Kismet2NameValidators.h"

#include "StateMachine/NaiveState.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SGraphPreviewer.h"

#define LOCTEXT_NAMESPACE "SLStateNode"

UNaiveStateMachineNode_SubEntry::UNaiveStateMachineNode_SubEntry()
{
	//bIsReadOnly = true;
	NodeName = TEXT("SubEntry");
}

//////////////////////////////////////////////////////////////////////////
//
// SNaiveStateMachineNode_SubEntry
//
//////////////////////////////////////////////////////////////////////////

void SNaiveStateMachineNode_SubEntry::Construct(const FArguments& InArgs, UNaiveStateMachineGraphNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

TSharedPtr<SToolTip> SNaiveStateMachineNode_SubEntry::GetComplexTooltip()
{
	UNaiveStateMachineNode_SubEntry* StateNode = CastChecked<UNaiveStateMachineNode_SubEntry>(GraphNode);

	FText Result = LOCTEXT("SunEntry", "Sub Entry");

	return SNew(SToolTip)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), "NormalText.Important")
		.Text(Result)
		]
		];
}

#undef LOCTEXT_NAMESPACE
