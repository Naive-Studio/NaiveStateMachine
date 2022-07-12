// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeAI.h"
#include "NaiveStateMachineGraphNode.h"
#include "NaiveStateMachineNode_State.generated.h"

class UNaiveStateNode;
/**
 * 
 */
UCLASS()
class UNaiveStateMachineNode_State : public UNaiveStateMachineGraphNode
{
	GENERATED_BODY()
public:
	UNaiveStateMachineNode_State();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void DestroyNode() override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UEdGraphNode Interface

	//~ Begin UNaiveStateMachineGraphNode Interface
	virtual FString GetNodeName() const override;
	virtual class UClass* GetTemplateClass() const override;
	virtual void GetTransitionList(TArray<class UNaiveStateMachineNode_Transition*>& OutTransitions, bool bWantSortedList = false) override;
	//~ End UNaiveStateMachineGraphNode Interface

protected:
	UPROPERTY(EditAnywhere, Category = "State")
	TSubclassOf<class UNaiveStateNode> StateTemplate;

	UPROPERTY(EditAnywhere, Category = GraphNode)
	FString NodeName = TEXT("New State");

	UPROPERTY()
	class UEdGraph* BoundGraph;
};

class SNaiveStateMachineNode_State : public SGraphNodeAI
{
public:
	SLATE_BEGIN_ARGS(SNaiveStateMachineNode_State) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UNaiveStateMachineGraphNode* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface

protected:
	FSlateColor GetBorderBackgroundColor() const;

	virtual FText GetPreviewCornerText() const;
	virtual const FSlateBrush* GetNameIcon() const;
};
