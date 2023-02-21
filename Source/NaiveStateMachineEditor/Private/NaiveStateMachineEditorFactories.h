// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EdGraphUtilities.h"

#include "EdGraph/EdGraph.h"
#include "GraphEditor.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"


#include "NaiveStateMachineEditorFactories.generated.h"

/**
 * 
 */
UCLASS()
class UNaiveStateMachineEditorFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UNaiveStateMachineEditorFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FString GetDefaultNewAssetName() const override;
};

class NAIVESTATEMACHINEEDITOR_API FStateMachineGraphPinFactory : public FGraphPanelPinFactory
{
public:
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};

class NAIVESTATEMACHINEEDITOR_API FStateMachinePinConnectionFactory : public FGraphPanelPinConnectionFactory
{
public:
	virtual class FConnectionDrawingPolicy* CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
};

class NAIVESTATEMACHINEEDITOR_API FStateMachineGraphPanelNodeFactory : public FGraphPanelNodeFactory
{
protected:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};


struct FStateMachineGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);
public:
	FStateMachineGraphEditorSummoner(TSharedPtr<class FNaiveStateMachineEditor> InEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);

	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;

protected:
	TWeakPtr<class FNaiveStateMachineEditor> StateMachineEditorPtr;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};

struct FStateMachineDetailsSummoner : public FWorkflowTabFactory
{
public:
	FStateMachineDetailsSummoner(TSharedPtr<class FNaiveStateMachineEditor> InEditorPtr);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class FNaiveStateMachineEditor> StateMachineEditorPtr;
};