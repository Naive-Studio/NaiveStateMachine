// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "NaiveStateMachineNode_State.h"
#include "NaiveStateMachineNode_SubMachine.generated.h"

/**
 * 
 */
UCLASS()
class UNaiveStateMachineNode_SubMachine : public UNaiveStateMachineNode_State
{
	GENERATED_BODY()
public:
	UNaiveStateMachineNode_SubMachine();

	virtual void PostPlacedNewNode() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;

public:

	UPROPERTY(EditAnywhere, Category = "SubStateMachine")
	class UNaiveStateMachine* StateMachineAsset = nullptr;

protected:
	UPROPERTY()
	class UNaiveSubStateMachineGraph* EditorStateMachineGraph;

};

class SNaiveStateMachineNode_SubMachine : public SGraphNodeAI
{
public:
	SLATE_BEGIN_ARGS(SNaiveStateMachineNode_SubMachine) {}
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
