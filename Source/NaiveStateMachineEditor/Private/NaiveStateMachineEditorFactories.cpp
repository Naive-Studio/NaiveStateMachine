// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineEditorFactories.h"
#include "StateMachine/NaiveStateMachine.h"
#include "NaiveStateMachineGraphNode.h"
#include "NaiveStateMachineNode_Entry.h"
#include "NaiveStateMachineNode_State.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineNode_SubMachine.h"
#include "NaiveStateMachineNode_SubEntry.h"
#include "NaiveStateMachineGraphSchema.h"
#include "NaiveStateMachineGraphPin.h"
#include "NaiveStateMachineConnectionDrawingPolicy.h"
#include "NaiveStateMachineEditor.h"

#include "KismetPins/SGraphPinExec.h"
#include "GraphEditor.h"
#include "Textures/SlateIcon.h"

#define LOCTEXT_NAMESPACE "StateMachineFactories"

UNaiveStateMachineEditorFactory::UNaiveStateMachineEditorFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNaiveStateMachine::StaticClass();
}

UObject* UNaiveStateMachineEditorFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UNaiveStateMachine* StateMachineAsset = NewObject<UNaiveStateMachine>(InParent, InClass, InName, Flags);
	return StateMachineAsset;
}

TSharedPtr<class SGraphNode> FStateMachineGraphPanelNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UNaiveStateMachineNode_Entry* EntryNode = Cast<UNaiveStateMachineNode_Entry>(Node))
	{
		return SNew(SNaiveStateMachineNode_Entry, EntryNode);
	}

	if (UNaiveStateMachineNode_SubMachine* TransitionNode = Cast<UNaiveStateMachineNode_SubMachine>(Node))
	{
		return SNew(SNaiveStateMachineNode_SubMachine, TransitionNode);
	}

	if (UNaiveStateMachineNode_SubEntry* TransitionNode = Cast<UNaiveStateMachineNode_SubEntry>(Node))
	{
		return SNew(SNaiveStateMachineNode_SubEntry, TransitionNode);
	}

	if (UNaiveStateMachineNode_State* StateNode = Cast<UNaiveStateMachineNode_State>(Node))
	{
		return SNew(SNaiveStateMachineNode_State, StateNode);
	}

	if (UNaiveStateMachineNode_Transition* TransitionNode = Cast<UNaiveStateMachineNode_Transition>(Node))
	{
		return SNew(SNaiveStateMachineNode_Transition, TransitionNode);
	}

	return NULL;
}

TSharedPtr<class SGraphPin> FStateMachineGraphPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UNaiveStateMachineGraphSchema::PC_State)
	{
		return SNew(SNaiveStateMachineGraphPin, InPin);
	}

	if (InPin->PinType.PinCategory == UNaiveStateMachineGraphSchema::PC_Entry)
	{
		return SNew(SGraphPinExec, InPin);
	}

	return nullptr;
}

class FConnectionDrawingPolicy* FStateMachinePinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	if (Schema->IsA(UNaiveStateMachineGraphSchema::StaticClass()))
	{
		return new FNaiveStateMachineConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

FStateMachineGraphEditorSummoner::FStateMachineGraphEditorSummoner(TSharedPtr<class FNaiveStateMachineEditor> InEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	:FDocumentTabFactoryForObjects<UEdGraph>(FNaiveStateMachineEditor::StateMachineGraphTabId, InEditorPtr)
	, StateMachineEditorPtr(InEditorPtr)
	, OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{

}

void FStateMachineGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	StateMachineEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FStateMachineGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FStateMachineGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FStateMachineGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	auto UpdateGraphEdPtr = OnCreateGraphEditorWidget.Execute(DocumentID);
	return UpdateGraphEdPtr;
}

const FSlateBrush* FStateMachineGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FAppStyle::GetBrush("NoBrush");
}

void FStateMachineGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	StateMachineEditorPtr.Pin()->GetStateMachineAsset()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}



//////////////////////////////////////////////////////////////////////////

 FStateMachineDetailsSummoner::FStateMachineDetailsSummoner(TSharedPtr<class FNaiveStateMachineEditor> InEditorPtr)
	 :FWorkflowTabFactory(FNaiveStateMachineEditor::StateMachineDetailsTabId,InEditorPtr)
	 ,StateMachineEditorPtr(InEditorPtr)
 {
	 TabLabel = LOCTEXT("StateMachineDetailsLebal", "Details");
	 TabIcon = FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Kismet.Tabs.Components");
	
	 bIsSingleton = true;

	 ViewMenuDescription = LOCTEXT("StateMachineDetailsView", "Details");
	 ViewMenuTooltip = LOCTEXT("StateMachineDetailsView_ToolTip", "Show the details view");
 }
 
 TSharedRef<SWidget> FStateMachineDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
 {
	 check(StateMachineEditorPtr.IsValid());
	 return StateMachineEditorPtr.Pin()->SpawnProperties();
 }
 
 FText FStateMachineDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
 {
	 return LOCTEXT("StateMachineDetailsTabTooltip", "The state machine details tab allows editing of the properties of state nodes");
 }

#undef LOCTEXT_NAMESPACE