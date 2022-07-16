// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineNode_State.h"
#include "StateMachine/NaiveTransitionNode.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "EdGraphUtilities.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "ScopedTransaction.h"
#include "Animation/BlendProfile.h"
#include "SGraphPanel.h"
#include "ConnectionDrawingPolicy.h"

#define LOCTEXT_NAMESPACE "NaiveStateMachineTransitionNode"


UNaiveStateMachineNode_Transition::UNaiveStateMachineNode_Transition()
{

}

void UNaiveStateMachineNode_Transition::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UNaiveStateMachineNode_Transition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UNaiveStateMachineGraphNode* PrevState = GetPreviousState();
	UNaiveStateMachineGraphNode* NextState = GetNextState();

	FFormatNamedArguments Args;
	Args.Add(TEXT("PrevState"), FText::FromString(PrevState->GetNodeName()));
	Args.Add(TEXT("NextState"), FText::FromString(NextState->GetNodeName()));

	return FText::Format(LOCTEXT("PrevStateToNewState", "{PrevState} to {NextState}"), Args);
}

FText UNaiveStateMachineNode_Transition::GetTooltipText() const
{
	return LOCTEXT("StateTransitionTooltip", "This is a state transition");
}

FLinearColor UNaiveStateMachineNode_Transition::GetNodeTitleColor() const
{
	return FColorList::Red;
}

void UNaiveStateMachineNode_Transition::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UNaiveStateMachineNode_Transition::PrepareForCopying()
{
	Super::PrepareForCopying();
}

void UNaiveStateMachineNode_Transition::PostPasteNode()
{
	Super::PostPasteNode();
}

void UNaiveStateMachineNode_Transition::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
}

void UNaiveStateMachineNode_Transition::DestroyNode()
{
	Super::DestroyNode();
}

void UNaiveStateMachineNode_Transition::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
}


void UNaiveStateMachineNode_Transition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

UNaiveStateMachineNode_State* UNaiveStateMachineNode_Transition::GetPreviousState() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UNaiveStateMachineNode_State>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

UNaiveStateMachineNode_State* UNaiveStateMachineNode_Transition::GetNextState() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UNaiveStateMachineNode_State>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

void UNaiveStateMachineNode_Transition::CreateConnections(UNaiveStateMachineGraphNode* PreviousState, UNaiveStateMachineGraphNode* NextState)
{
	// Previous to this
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	PreviousState->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(PreviousState->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	NextState->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(NextState->GetInputPin());
}

class UClass* UNaiveStateMachineNode_Transition::GetTemplateClass() const
{
	return TransitionTemplate != nullptr ? TransitionTemplate : nullptr;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void SNaiveStateMachineNode_Transition::Construct(const FArguments& InArgs, UNaiveStateMachineNode_Transition* InNode)
{
	this->GraphNode = InNode;
	this->UpdateGraphNode();
}

void SNaiveStateMachineNode_Transition::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{

}

void SNaiveStateMachineNode_Transition::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	// Ignored; position is set by the location of the attached state nodes
}

bool SNaiveStateMachineNode_Transition::RequiresSecondPassLayout() const
{
	return true;
}

void SNaiveStateMachineNode_Transition::PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const
{
	UNaiveStateMachineNode_Transition* TransNode = CastChecked<UNaiveStateMachineNode_Transition>(GraphNode);

	// Find the geometry of the state nodes we're connecting
	FGeometry StartGeom;
	FGeometry EndGeom;

	int32 TransIndex = 0;
	int32 NumOfTrans = 1;

	UNaiveStateMachineGraphNode* PrevState = TransNode->GetPreviousState();
	UNaiveStateMachineGraphNode* NextState = TransNode->GetNextState();
	if ((PrevState != NULL) && (NextState != NULL))
	{
		const TSharedRef<SNode>* pPrevNodeWidget = NodeToWidgetLookup.Find(PrevState);
		const TSharedRef<SNode>* pNextNodeWidget = NodeToWidgetLookup.Find(NextState);
		if ((pPrevNodeWidget != NULL) && (pNextNodeWidget != NULL))
		{
			const TSharedRef<SNode>& PrevNodeWidget = *pPrevNodeWidget;
			const TSharedRef<SNode>& NextNodeWidget = *pNextNodeWidget;

			StartGeom = FGeometry(FVector2D(PrevState->NodePosX, PrevState->NodePosY), FVector2D::ZeroVector, PrevNodeWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(NextState->NodePosX, NextState->NodePosY), FVector2D::ZeroVector, NextNodeWidget->GetDesiredSize(), 1.0f);

			TArray<UNaiveStateMachineNode_Transition*> Transitions;
			PrevState->GetTransitionList(Transitions);

			Transitions = Transitions.FilterByPredicate([NextState](const UNaiveStateMachineNode_Transition* InTransition) -> bool
				{
					return InTransition->GetNextState() == NextState;
				});

			TransIndex = Transitions.IndexOfByKey(TransNode);
			NumOfTrans = Transitions.Num();
		}
	}

	//Position Node
	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, TransIndex, NumOfTrans);
}

void SNaiveStateMachineNode_Transition::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			SNew(SImage)
			.Image(FEditorStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
		.ColorAndOpacity(this, &SNaiveStateMachineNode_Transition::GetTransitionColor)
		]
	+ SOverlay::Slot()
		[
			SNew(SImage)
			.Image(this, &SNaiveStateMachineNode_Transition::GetTransitionIconImage)
		]
		];
}

TSharedPtr<SToolTip> SNaiveStateMachineNode_Transition::GetComplexTooltip()
{
	return SNew(SToolTip)
		[
			GenerateRichTooltip()
		];
}

void SNaiveStateMachineNode_Transition::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UNaiveStateMachineNode_Transition* TransNode = CastChecked<UNaiveStateMachineNode_Transition>(GraphNode);
	if (UEdGraphPin* Pin = TransNode->GetInputPin())
	{
		GetOwnerPanel()->AddPinToHoverSet(Pin);
	}

	SGraphNode::OnMouseEnter(MyGeometry, MouseEvent);
}

void SNaiveStateMachineNode_Transition::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	UNaiveStateMachineNode_Transition* TransNode = CastChecked<UNaiveStateMachineNode_Transition>(GraphNode);
	if (UEdGraphPin* Pin = TransNode->GetInputPin())
	{
		GetOwnerPanel()->RemovePinFromHoverSet(Pin);
	}

	SGraphNode::OnMouseLeave(MouseEvent);
}

void SNaiveStateMachineNode_Transition::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	const float MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const float MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}

FLinearColor SNaiveStateMachineNode_Transition::StaticGetTransitionColor(UNaiveStateMachineNode_Transition* TransNode, bool bIsHovered)
{
	const FLinearColor ActiveColor(1.0f, 0.4f, 0.3f, 1.0f);
	const FLinearColor HoverColor(0.724f, 0.256f, 0.0f, 1.0f);
	FLinearColor BaseColor(0.9f, 0.9f, 0.9f, 1.0f);

	return bIsHovered ? HoverColor : BaseColor;
}

FText SNaiveStateMachineNode_Transition::GetPreviewCornerText() const
{
	UNaiveStateMachineNode_Transition* TransNode = CastChecked<UNaiveStateMachineNode_Transition>(GraphNode);
	bool bReverse = false;
	UNaiveStateMachineGraphNode* PrevState = (bReverse ? TransNode->GetNextState() : TransNode->GetPreviousState());
	UNaiveStateMachineGraphNode* NextState = (bReverse ? TransNode->GetPreviousState() : TransNode->GetNextState());

	FText Result = LOCTEXT("BadTransition", "Bad transition (missing source or target)");

	// Show the priority if there is any ambiguity
	if (PrevState != NULL)
	{
		if (NextState != NULL)
		{
			TArray<UNaiveStateMachineNode_Transition*> TransitionFromSource;
			PrevState->GetTransitionList(/*out*/ TransitionFromSource);

			Result = FText::Format(LOCTEXT("TransitionXToY", "{0} to {1} : {2}"),
				FText::FromString(PrevState->GetNodeName()), 
				FText::FromString(NextState->GetNodeName()), 
				FText::FromName(TransNode->GetTemplateClass()->GetFName()));
		}
	}

	return Result;
}

FSlateColor SNaiveStateMachineNode_Transition::GetTransitionColor() const
{
	UNaiveStateMachineNode_Transition* TransNode = CastChecked<UNaiveStateMachineNode_Transition>(GraphNode);
	return StaticGetTransitionColor(TransNode, IsHovered());
}

const FSlateBrush* SNaiveStateMachineNode_Transition::GetTransitionIconImage() const
{
	return FEditorStyle::GetBrush("Graph.TransitionNode.Icon");
}

TSharedRef<SWidget> SNaiveStateMachineNode_Transition::GenerateRichTooltip()
{
	UNaiveStateMachineNode_Transition* TransNode = CastChecked<UNaiveStateMachineNode_Transition>(GraphNode);
	if (TransNode->GetTemplateClass() == nullptr)
	{
		return SNew(STextBlock).Text(LOCTEXT("NoTransitionClass","Bad transition (missing template class)"));
	}

	TSharedRef<SVerticalBox> Widget = SNew(SVerticalBox);
	const FText TooltipDesc = GetPreviewCornerText();

	Widget->AddSlot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.TextStyle(FEditorStyle::Get(), TEXT("NormalText.Important"))
			.Text(TooltipDesc)
		];

	return Widget;
}

#undef LOCTEXT_NAMESPACE