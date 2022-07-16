// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraphEditor.h"
#include "Misc/NotifyHook.h"
#include "Layout/Visibility.h"
#include "GraphEditor.h"
#include "INaiveStateMachineEditor.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

/**
 * 
 */
class FNaiveStateMachineEditor : public INaiveStateMachineEditor, public FAIGraphEditor, public FNotifyHook
{
public:
	FNaiveStateMachineEditor();
	virtual ~FNaiveStateMachineEditor();

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	/** Begin INaiveStateMachineEditor Interface*/
	virtual void InitStateMachineAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UNaiveStateMachine* InStateMachine) override;
	virtual UNaiveStateMachine* GetStateMachineAsset() const override;
	/** End INaiveStateMachineEditor Interface*/

	bool IsPropertyEditable() const;
	void OnPackageSaved(const FString& PackageFileName, UObject* Outer);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	void OnGraphNodeDoubleClicked(class UEdGraphNode* Node);
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);
	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);

	/**
 * Get the localized text to display for the specified mode
 * @param	InMode	The mode to display
 * @return the localized text representation of the mode
 */
	static FText GetLocalizedMode(FName InMode);

	virtual void SaveAsset_Execute() override;

	void ExtendToolbar(TSharedPtr<class FExtender> Extender);

	void BindCommands();

	void CreateNewState();
	//void CreateNewSubmachine();
	void CreateNewTraisntion();

	void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager);
	
	void RestoreStateMachine();

	static const FName StateMachineGraphTabId;
	static const FName StateMachineDetailsTabId;
	static const FName StateMachineMode;

	/** Spawns the tab with the update graph inside */
	TSharedRef<SWidget> SpawnProperties();
private:
	/** Create widget for graph editing */
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);

	/** Called when the selection changes in the GraphEditor */
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection) override;

	/** Handler for when a node class is picked */
	void HandleNewNodeClassPicked(UClass* InClass) const;

	void UpdateGraphAsset();

private:

	TSharedPtr<class SGraphEditor> GraphEditorView;

	/* The State machine asset being edited */
	class UNaiveStateMachine* StateMachine = nullptr;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;

	/** Property View */
	TSharedPtr<class IDetailsView> DetailsView;

	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;

	TWeakObjectPtr<class UNaiveStateMachineNode_SubMachine> FocusedGraphOwner;
};
