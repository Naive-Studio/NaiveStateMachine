// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveSubStateMachineGraphSchema.h"
#include "NaiveStateMachineNode_Entry.h"
#include "NaiveStateMachineNode_State.h"
#include "NaiveStateMachineNode_SubMachine.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveStateMachineNode_SubEntry.h"
#include "NaiveStateMachineConnectionDrawingPolicy.h"

#include "EdGraph/EdGraph.h"
#include "EdGraphNode_Comment.h"
#include "Layout/SlateRect.h"
#include "GraphEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"

#define LOCTEXT_NAMESPACE "NaiveStateMachineSchema"

//////////////////////////////////////////////////////////////////////////

EGraphType UNaiveSubStateMachineGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UNaiveSubStateMachineGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create the entry/exit tunnels
	FGraphNodeCreator<UNaiveStateMachineNode_Entry> NodeCreator(Graph);
	UNaiveStateMachineNode_Entry* EntryNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);

	FGraphNodeCreator<UNaiveStateMachineNode_SubEntry> SunEntryNodeCreator(Graph);
	UNaiveStateMachineNode_SubEntry* SubEntryNode = SunEntryNodeCreator.CreateNode();
	SunEntryNodeCreator.Finalize();
 	SetNodeMetaData(SubEntryNode, "SubEntry");

	TryCreateConnection(EntryNode->GetOutputPin(), SubEntryNode->GetInputPin());
}

class FConnectionDrawingPolicy* UNaiveSubStateMachineGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FNaiveStateMachineConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

const FPinConnectionResponse UNaiveSubStateMachineGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	if (PinA && PinB)
	{
		const bool bPinAIsSubEntry = PinA->GetOwningNode()->IsA(UNaiveStateMachineNode_SubEntry::StaticClass());
		const bool bPinBIsSubEntry = PinB->GetOwningNode()->IsA(UNaiveStateMachineNode_SubEntry::StaticClass());
		const bool bPinAIsState = PinA->GetOwningNode()->IsA(UNaiveStateMachineNode_State::StaticClass());
		const bool bPinBIsState = PinB->GetOwningNode()->IsA(UNaiveStateMachineNode_State::StaticClass());

		if ((bPinAIsSubEntry && bPinBIsState && PinA->Direction == EGPD_Input) ||
			(bPinBIsSubEntry && bPinAIsState && PinB->Direction == EGPD_Input))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("SubEntry Can not have normal input pin"));
		}

	}
	
	return Super::CanCreateConnection(PinA, PinB);
}

bool UNaiveSubStateMachineGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	const bool bModified = UNaiveStateMachineGraphSchema::TryCreateConnection(PinA, PinB);
	
	return bModified;
}

void UNaiveSubStateMachineGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	//const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UNaiveSubStateMachineGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	//const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

#undef LOCTEXT_NAMESPACE