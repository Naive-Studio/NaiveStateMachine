// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineNode_State.h"
#include "StateMachine/NaiveStateMachine.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineGraphPin.h"
#include "Kismet2/Kismet2NameValidators.h"

#include "StateMachine/NaiveStateNode.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SGraphPreviewer.h"

#define LOCTEXT_NAMESPACE "SLStateNode"

UNaiveStateMachineNode_State::UNaiveStateMachineNode_State()
{
	bCanRenameNode = true;
}

void UNaiveStateMachineNode_State::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
}

FText UNaiveStateMachineNode_State::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetNodeName());
}

FString UNaiveStateMachineNode_State::GetNodeName() const
{
	return NodeName;
}

class UClass* UNaiveStateMachineNode_State::GetTemplateClass() const
{
	return StateTemplate != nullptr ? StateTemplate : nullptr;
}

void UNaiveStateMachineNode_State::OnRenameNode(const FString& NewName)
{
	NodeName = NewName;

	int32 InvalidCharIndex = INDEX_NONE;
	while (NodeName.FindChar(UNaiveStateMachine::StateNameJointMark,InvalidCharIndex))
	{
		NodeName.RemoveAt(InvalidCharIndex);
	}

	TSharedPtr<INameValidatorInterface> NameValidator = MakeNameValidator();
	NameValidator->FindValidString(NodeName);

	Super::OnRenameNode(NodeName);
}

void UNaiveStateMachineNode_State::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnRenameNode(NodeName);
}

FText UNaiveStateMachineNode_State::GetTooltipText() const
{
	return LOCTEXT("SLStateNode_Tooltip", "This is a state");
}

void UNaiveStateMachineNode_State::PostPasteNode()
{
	Super::PostPasteNode();
}

void UNaiveStateMachineNode_State::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
}

void UNaiveStateMachineNode_State::DestroyNode()
{
	TArray<UNaiveStateMachineNode_Transition*> OutTransitions;
	GetTransitionList(OutTransitions);

	for (UNaiveStateMachineNode_Transition* Trandition : OutTransitions)
	{
		Trandition->Modify();
		Trandition->DestroyNode();
	}

	Super::DestroyNode();
}

void UNaiveStateMachineNode_State::GetTransitionList(TArray<UNaiveStateMachineNode_Transition*>& OutTransitions, bool bWantSortedList /*= false*/)
{
	// Normal transitions
	for (int32 LinkIndex = 0; LinkIndex < Pins[1]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[1]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UNaiveStateMachineNode_Transition* Transition = Cast<UNaiveStateMachineNode_Transition>(TargetNode))
		{
			OutTransitions.Add(Transition);
		}
	}

	// Bidirectional transitions where we are the 'backwards' link
	for (int32 LinkIndex = 0; LinkIndex < Pins[0]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[0]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UNaiveStateMachineNode_Transition* Transition = Cast<UNaiveStateMachineNode_Transition>(TargetNode))
		{
			//if (Transition->Bidirectional)
			{
				OutTransitions.Add(Transition);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void SNaiveStateMachineNode_State::Construct(const FArguments& InArgs, UNaiveStateMachineGraphNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

void SNaiveStateMachineNode_State::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	SGraphNodeAI::GetNodeInfoPopups(Context, Popups);
}

void SNaiveStateMachineNode_State::UpdateGraphNode()
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
			.BorderBackgroundColor(this, &SNaiveStateMachineNode_State::GetBorderBackgroundColor)
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
							.BackgroundColor(this, &SNaiveStateMachineNode_State::GetErrorColor)
						.ToolTipText(this, &SNaiveStateMachineNode_State::GetErrorMsgToolTip)
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
								.OnVerifyTextChanged(this, &SNaiveStateMachineNode_State::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SNaiveStateMachineNode_State::OnNameTextCommited)
								.IsReadOnly(this, &SNaiveStateMachineNode_State::IsNameReadOnly)
								.IsSelected(this, &SNaiveStateMachineNode_State::IsSelectedExclusively)
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

void SNaiveStateMachineNode_State::CreatePinWidgets()
{
	UNaiveStateMachineNode_State* StateNode = CastChecked<UNaiveStateMachineNode_State>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SNaiveStateMachineGraphPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SNaiveStateMachineNode_State::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

TSharedPtr<SToolTip> SNaiveStateMachineNode_State::GetComplexTooltip()
{
	UNaiveStateMachineNode_State* StateNode = CastChecked<UNaiveStateMachineNode_State>(GraphNode);

	FText Result = LOCTEXT("BadState", "Bad state (missing state template class)");

	if (StateNode->GetTemplateClass())
	{
		Result = FText::Format(LOCTEXT("StateTooltip", "State Object : {0}"),
			FText::FromName(StateNode->GetTemplateClass()->GetFName()));
	}

	return SNew(SToolTip)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FAppStyle::Get(),"NormalText.Important")
				.Text(Result)
			]
		];
}

FSlateColor SNaiveStateMachineNode_State::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

FText SNaiveStateMachineNode_State::GetPreviewCornerText() const
{
	UNaiveStateMachineNode_State* StateNode = CastChecked<UNaiveStateMachineNode_State>(GraphNode);

	return FText::Format(NSLOCTEXT("SNaiveStateMachineNode_State", "PreviewCornerStateText", "{0} state"), FText::FromString(StateNode->GetNodeName()));
}

const FSlateBrush* SNaiveStateMachineNode_State::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

#undef LOCTEXT_NAMESPACE

