// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineGraphNode.h"
#include "SGraphNodeAI.h"
#include "NaiveStateMachineNode_Transition.generated.h"

class UNaiveStateMachineGraphNode;
/**
 * 
 */
UCLASS()
class UNaiveStateMachineNode_Transition : public UNaiveStateMachineGraphNode
{
	GENERATED_BODY()
public:
	UNaiveStateMachineNode_Transition();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void DestroyNode() override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UEdGraphNode Interface

	class UNaiveStateMachineNode_State* GetPreviousState() const;
	class UNaiveStateMachineNode_State* GetNextState() const;
	void CreateConnections(UNaiveStateMachineGraphNode* PreviousState, UNaiveStateMachineGraphNode* NextState);
	virtual class UClass* GetTemplateClass() const;

	UPROPERTY(EditAnywhere, Category = "Transition")
	TSubclassOf<class UNaiveTransitionNode> TransitionTemplate;

	/*
	* If true, the transition template will never be instanced.
	* If you want the transition to check the condition automatically(in game thread tick loop), please set it to false.
	*/
	UPROPERTY(EditAnywhere, Category = "Transition")
	bool bEventDriven = false;

	UPROPERTY(EditAnywhere, Category = "Transition", meta = (EditCondition = "bEventDriven"))
	TArray<FName> ObservingEvents;
};

class SToolTip;
class UAnimStateTransitionNode;

class SNaiveStateMachineNode_Transition : public SGraphNodeAI
{
public:
	SLATE_BEGIN_ARGS(SNaiveStateMachineNode_Transition) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UNaiveStateMachineNode_Transition* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;
	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface

	// SWidget interface
	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

	// Calculate position for multiple nodes to be placed between a start and end point, by providing this nodes index and max expected nodes 
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	static FLinearColor StaticGetTransitionColor(UNaiveStateMachineNode_Transition* TransNode, bool bIsHovered);

private:
	TSharedPtr<STextEntryPopup> TextEntryWidget;

private:
	virtual FText GetPreviewCornerText() const override;
	FSlateColor GetTransitionColor() const;
	const FSlateBrush* GetTransitionIconImage() const;

	TSharedRef<SWidget> GenerateRichTooltip();
};