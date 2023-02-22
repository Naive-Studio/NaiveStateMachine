// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineNode_SubMachine.h"
#include "NaiveSubStateMachineGraph.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveSubStateMachineGraphSchema.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineGraphPin.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "StateMachine/NaiveState.h"
#include "StateMachine/NaiveStateMachine.h"
#include "EdGraph/EdGraph.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SGraphPreviewer.h"

#define LOCTEXT_NAMESPACE "SLStateNode"

UNaiveStateMachineNode_SubMachine::UNaiveStateMachineNode_SubMachine()
{
	NodeName = TEXT("New Sub State Machine");
}

void UNaiveStateMachineNode_SubMachine::PostPlacedNewNode()
{
	// Create a new sub  graph
	if (!EditorStateMachineGraph)
	{
		EditorStateMachineGraph = CastChecked<UNaiveSubStateMachineGraph>(FBlueprintEditorUtils::CreateNewGraph(this, *NodeName, UNaiveSubStateMachineGraph::StaticClass(), UNaiveSubStateMachineGraphSchema::StaticClass()));
		check(EditorStateMachineGraph);
	
		const UEdGraphSchema* Schema = EditorStateMachineGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditorStateMachineGraph);
	}

	EditorStateMachineGraph->OwnerGraphNode = this;
	// Find an interesting name
	//TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	//FBlueprintEditorUtils::RenameGraph(EditorStateMachineGraph, NodeName);
	

	// Add the new graph as a child of our parent graph
	UNaiveStateMachineGraph* ParentGraph = Cast<UNaiveStateMachineGraph>(GetGraph());
	if (ParentGraph && ParentGraph->SubGraphs.Find(EditorStateMachineGraph) == INDEX_NONE)
	{
		EditorStateMachineGraph->ParentGraph = ParentGraph;
		ParentGraph->Modify();
		ParentGraph->SubGraphs.Add(EditorStateMachineGraph);
	}
}

UObject* UNaiveStateMachineNode_SubMachine::GetJumpTargetForDoubleClick() const
{
	return StateMachineAsset;
}

//////////////////////////////////////////////////////////////////////////
//SNaiveStateMachineNode_SubMachine
//////////////////////////////////////////////////////////////////////////

void SNaiveStateMachineNode_SubMachine::Construct(const FArguments& InArgs, UNaiveStateMachineGraphNode* InNode)
{
	//SNaiveStateMachineNode_State::Construct(InArgs, InNode);

	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

void SNaiveStateMachineNode_SubMachine::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	SGraphNodeAI::GetNodeInfoPopups(Context, Popups);
}

void SNaiveStateMachineNode_SubMachine::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(0)
		.BorderBackgroundColor(this, &SNaiveStateMachineNode_SubMachine::GetBorderBackgroundColor)
		[
			SNew(SOverlay)

			// PIN AREA
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(RightNodeBox, SVerticalBox)
		]

	// STATE NAME AREA
	+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
		.BorderBackgroundColor(TitleShadowColor)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::SelfHitTestInvisible)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			// POPUP ERROR MESSAGE
			SAssignNew(ErrorText, SErrorText)
			.BackgroundColor(this, &SNaiveStateMachineNode_SubMachine::GetErrorColor)
		.ToolTipText(this, &SNaiveStateMachineNode_SubMachine::GetErrorMsgToolTip)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(NodeTypeIcon)
		]
	+ SHorizontalBox::Slot()
		.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(InlineEditableText, SInlineEditableTextBlock)
			.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
		.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
		.OnVerifyTextChanged(this, &SNaiveStateMachineNode_SubMachine::OnVerifyNameTextChanged)
		.OnTextCommitted(this, &SNaiveStateMachineNode_SubMachine::OnNameTextCommited)
		.IsReadOnly(this, &SNaiveStateMachineNode_SubMachine::IsNameReadOnly)
		.IsSelected(this, &SNaiveStateMachineNode_SubMachine::IsSelectedExclusively)
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			NodeTitle.ToSharedRef()
		]
		]
		]
		]
		]
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SNaiveStateMachineNode_SubMachine::CreatePinWidgets()
{
	UNaiveStateMachineNode_SubMachine* StateNode = CastChecked<UNaiveStateMachineNode_SubMachine>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SNaiveStateMachineGraphPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SNaiveStateMachineNode_SubMachine::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

TSharedPtr<SToolTip> SNaiveStateMachineNode_SubMachine::GetComplexTooltip()
{
	UNaiveStateMachineNode_SubMachine* StateNode = CastChecked<UNaiveStateMachineNode_SubMachine>(GraphNode);

	FText Result = LOCTEXT("SubStateMachine", "Sub State Machine");

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

FSlateColor SNaiveStateMachineNode_SubMachine::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.2f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

FText SNaiveStateMachineNode_SubMachine::GetPreviewCornerText() const
{
	UNaiveStateMachineNode_State* StateNode = CastChecked<UNaiveStateMachineNode_State>(GraphNode);

	return FText::Format(NSLOCTEXT("SNaiveStateMachineNode_State", "PreviewCornerStateText", "{0} state"), FText::FromString(StateNode->GetNodeName()));
}

const FSlateBrush* SNaiveStateMachineNode_SubMachine::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}


#undef LOCTEXT_NAMESPACE