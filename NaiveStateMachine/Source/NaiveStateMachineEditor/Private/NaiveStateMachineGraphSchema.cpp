// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineGraphSchema.h"
#include "NaiveStateMachineNode_Entry.h"
#include "NaiveStateMachineNode_State.h"
#include "NaiveStateMachineNode_SubMachine.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineNode_SubEntry.h"
#include "NaiveStateMachineGraph.h"
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


const FName UNaiveStateMachineGraphSchema::PC_State(TEXT("State"));
const FName UNaiveStateMachineGraphSchema::PC_Entry(TEXT("Entry"));

//////////////////////////////////////////////////////////////////////////


UEdGraphNode* FNaiveEdGraphSchemaAction_NewStateNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "K2_AddNode", "Add Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;
		//@TODO: ANIM: SNAP_GRID isn't centralized or exposed - NodeTemplate->SnapToGrid(SNAP_GRID);

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
		
		ParentGraph->NotifyGraphChanged();
	}

	return ResultNode;
}

void FNaiveEdGraphSchemaAction_NewStateNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(NodeTemplate);
}

TSharedPtr<FNaiveEdGraphSchemaAction_NewStateNode> AddNewStateNodeAction(FGraphContextMenuBuilder& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip, const int32 Grouping = 0)
{
	TSharedPtr<FNaiveEdGraphSchemaAction_NewStateNode> NewStateNode(new FNaiveEdGraphSchemaAction_NewStateNode(Category, MenuDesc, Tooltip, Grouping));
	ContextMenuBuilder.AddAction(NewStateNode);
	return NewStateNode;
}

//////////////////////////////////////////////////////////////////////////

UEdGraphNode* FNaiveEdGraphSchemaAction_NewStateComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;
	FSlateRect Bounds;

	TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(ParentGraph);
	if (GraphEditorPtr.IsValid() && GraphEditorPtr->GetBoundsForSelectedNodes(/*out*/ Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);

	return NewNode;
}

//////////////////////////////////////////////////////////////////////////


EGraphType UNaiveStateMachineGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UNaiveStateMachineGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create the entry/exit tunnels
	FGraphNodeCreator<UNaiveStateMachineNode_Entry> NodeCreator(Graph);
	UNaiveStateMachineNode_Entry* EntryNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);
}

void UNaiveStateMachineGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add state node
	{
		TSharedPtr<FNaiveEdGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddState", "Add State..."), LOCTEXT("AddStateTooltip", "A new state"));
		Action->NodeTemplate = NewObject<UNaiveStateMachineNode_State>(ContextMenuBuilder.OwnerOfTemporaries);
	}

	// Add state node
	{
		TSharedPtr<FNaiveEdGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddSunEntry", "Add Sub Entry..."), LOCTEXT("AddStateTooltip", "A new entry node of sub state machine"));
		Action->NodeTemplate = NewObject<UNaiveStateMachineNode_SubEntry>(ContextMenuBuilder.OwnerOfTemporaries);
	}

	// Add state node
	{
		TSharedPtr<FNaiveEdGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddSubMachine", "Add Sub State Machine..."), LOCTEXT("AddStateTooltip", "A new sub state machine"));
		Action->NodeTemplate = NewObject<UNaiveStateMachineNode_SubMachine>(ContextMenuBuilder.OwnerOfTemporaries);
	}


	// Entry point (only if doesn't already exist)
	{
		bool bHasEntry = false;
		for (auto NodeIt = ContextMenuBuilder.CurrentGraph->Nodes.CreateConstIterator(); NodeIt; ++NodeIt)
		{
			UEdGraphNode* Node = *NodeIt;
			if (const UNaiveStateMachineNode_Entry* StateNode = Cast<UNaiveStateMachineNode_Entry>(Node))
			{
				bHasEntry = true;
				break;
			}
		}

		if (!bHasEntry)
		{
			TSharedPtr<FNaiveEdGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddEntryPoint", "Add Entry Point..."), LOCTEXT("AddEntryPointTooltip", "Define State Machine's Entry Point"));
			Action->NodeTemplate = NewObject<UNaiveStateMachineNode_Entry>(ContextMenuBuilder.OwnerOfTemporaries);
		}
	}

	// Add Comment
	if (!ContextMenuBuilder.FromPin)
	{
		auto CurrentGraph = ContextMenuBuilder.CurrentGraph;
		const bool bIsManyNodesSelected = CurrentGraph ? GetNodeSelectionCount(CurrentGraph) > 0 : false;
		const FText MenuDescription = bIsManyNodesSelected ? LOCTEXT("CreateCommentSelection", "Create Comment from Selection") : LOCTEXT("AddComment", "Add Comment...");
		const FText ToolTip = LOCTEXT("CreateCommentSelectionTooltip", "Create a resizeable comment box around selected nodes.");

		TSharedPtr<FNaiveEdGraphSchemaAction_NewStateComment> NewComment(new FNaiveEdGraphSchemaAction_NewStateComment(FText::GetEmpty(), MenuDescription, ToolTip, 0));
		ContextMenuBuilder.AddAction(NewComment);
	}
}

void UNaiveStateMachineGraphSchema::GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	check(Context && Context->Graph);

	if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("NaiveStateMachineNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		if (!Context->bIsDebugging)
		{
			// Node contextual actions
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
			Section.AddMenuEntry(FGraphEditorCommands::Get().ReconstructNodes);
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
			if (Context->Node->bCanRenameNode)
			{
				Section.AddMenuEntry(FGenericCommands::Get().Rename);
			}
		}
	}

	UEdGraphSchema::GetContextMenuActions(Menu, Context);
}

class FConnectionDrawingPolicy* UNaiveStateMachineGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FNaiveStateMachineConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

const FPinConnectionResponse UNaiveStateMachineGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA == nullptr || PinB == nullptr || PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	// Connect entry node to a state is OK
	const bool bPinAIsEntry = PinA->GetOwningNode()->IsA(UNaiveStateMachineNode_Entry::StaticClass());
	const bool bPinBIsEntry = PinB->GetOwningNode()->IsA(UNaiveStateMachineNode_Entry::StaticClass());
	const bool bPinAIsStateNode = PinA->GetOwningNode()->IsA(UNaiveStateMachineNode_State::StaticClass());
	const bool bPinBIsStateNode = PinB->GetOwningNode()->IsA(UNaiveStateMachineNode_State::StaticClass());

	const bool bPinAIsSubEntry = PinA->GetOwningNode()->IsA(UNaiveStateMachineNode_SubEntry::StaticClass());
	const bool bPinBIsSubEntry = PinB->GetOwningNode()->IsA(UNaiveStateMachineNode_SubEntry::StaticClass());

	if ((bPinAIsSubEntry && bPinBIsStateNode && PinA->Direction == EGPD_Input) ||
		(bPinBIsSubEntry && bPinAIsStateNode && PinB->Direction == EGPD_Input))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("SubEntry Can not have normal input pin"));
	}


	if (bPinAIsEntry || bPinBIsEntry)
	{
		if (bPinAIsEntry && bPinBIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
		}

		if (bPinBIsEntry && bPinAIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Entry must connect to a state node"));
	}


	const bool bPinAIsTransition = PinA->GetOwningNode()->IsA(UNaiveStateMachineNode_Transition::StaticClass());
	const bool bPinBIsTransition = PinB->GetOwningNode()->IsA(UNaiveStateMachineNode_Transition::StaticClass());

	if (bPinAIsTransition && bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot wire a transition to a transition"));
	}

	// Compare the directions
	bool bDirectionsOK = false;

	if ((PinA->Direction == EGPD_Input) && (PinB->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}
	else if ((PinB->Direction == EGPD_Input) && (PinA->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}

	/*
	if (!bDirectionsOK)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Directions are not compatible"));
	}
	*/

	// Transitions are exclusive (both input and output), but states are not
	if (bPinAIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
	}
	else if (bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
	}
	else if (!bPinAIsTransition && !bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Create a transition"));
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
	}

}

bool UNaiveStateMachineGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	if (PinB->Direction == PinA->Direction)
	{
		if (UNaiveStateMachineGraphNode* Node = Cast<UNaiveStateMachineGraphNode>(PinB->GetOwningNode()))
		{
			if (PinA->Direction == EGPD_Input)
			{
				PinB = Node->GetOutputPin();
			}
			else
			{
				PinB = Node->GetInputPin();
			}
		}
	}

	const bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);
	
	return bModified;
}

void UNaiveStateMachineGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	//const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UNaiveStateMachineGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	//const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

bool UNaiveStateMachineGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	//Reference: UAnimationStateMachineSchema::CreateAutomaticConversionNodeAndConnections
	UNaiveStateMachineNode_State* NodeA = Cast<UNaiveStateMachineNode_State>(PinA->GetOwningNode());
	UNaiveStateMachineNode_State* NodeB = Cast<UNaiveStateMachineNode_State>(PinB->GetOwningNode());

	if ((NodeA != NULL) && (NodeB != NULL)
		&& (NodeA->GetInputPin() != NULL) && (NodeA->GetOutputPin() != NULL)
		&& (NodeB->GetInputPin() != NULL) && (NodeB->GetOutputPin() != NULL))
	{
		UNaiveStateMachineNode_Transition* TransitionNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UNaiveStateMachineNode_Transition>(NodeA->GetGraph(), NewObject<UNaiveStateMachineNode_Transition>(), FVector2D(0.0f, 0.0f), false);

		if (PinA->Direction == EGPD_Output)
		{
			TransitionNode->CreateConnections(NodeA, NodeB);
		}
		else
		{
			TransitionNode->CreateConnections(NodeB, NodeA);
		}

		return true;
		/*
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(TransitionNode->GetBoundGraph());
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		*/
	}
	return false;
}

#undef LOCTEXT_NAMESPACE