#include "NaiveStateMachineEditorApplicationMode.h"
#include "NaiveStateMachineEditor.h"
#include "NaiveStateMachineEditorFactories.h"

#include "Framework/Docking/TabManager.h"

#define LOCTEXT_NAMESPACE "StateMachineApplicationMode"

FNaiveStateMachineEditorApplicationMode::FNaiveStateMachineEditorApplicationMode(TSharedPtr<class FNaiveStateMachineEditor> InEditor)
	:FApplicationMode(FNaiveStateMachineEditor::StateMachineMode, FNaiveStateMachineEditor::GetLocalizedMode)
{
	StateMachineEditor = InEditor;

	StateMachineEditorTabFactories.RegisterFactory(MakeShareable(new FStateMachineDetailsSummoner(InEditor)));

	TabLayout = FTabManager::NewLayout("Standalone_StateMachine_Layout")
		->AddArea(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(TEXT("AssetEditorToolkit_Toolbar"), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FNaiveStateMachineEditor::StateMachineGraphTabId, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab(FNaiveStateMachineEditor::StateMachineDetailsTabId, ETabState::OpenedTab)
				)
			)
		);

	StateMachineEditor.Pin()->ExtendToolbar(ToolbarExtender);
}

void FNaiveStateMachineEditorApplicationMode::RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager)
{
	check(StateMachineEditor.IsValid());
	TSharedPtr<FNaiveStateMachineEditor> StateMachineEditorPtr = StateMachineEditor.Pin();

	StateMachineEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	// Mode-specific setup
	StateMachineEditorPtr->PushTabFactories(StateMachineEditorTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FNaiveStateMachineEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
// 
// 	check(BehaviorTreeEditor.IsValid());
// 	TSharedPtr<FBehaviorTreeEditor> BehaviorTreeEditorPtr = BehaviorTreeEditor.Pin();
// 
// 	BehaviorTreeEditorPtr->SaveEditedObjectState();
}

void FNaiveStateMachineEditorApplicationMode::PostActivateMode()
{
	// Reopen any documents that were open when the blueprint was last saved
	check(StateMachineEditor.IsValid());
	TSharedPtr<FNaiveStateMachineEditor> StateMachineEditorPtr = StateMachineEditor.Pin();
	StateMachineEditorPtr->RestoreStateMachine();

	FApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE