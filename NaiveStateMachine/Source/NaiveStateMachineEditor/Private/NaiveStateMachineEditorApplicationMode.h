#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "NaiveStateMachineEditor.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"

/** Application mode for main behavior tree editing mode */
class FNaiveStateMachineEditorApplicationMode : public FApplicationMode
{
public:
	FNaiveStateMachineEditorApplicationMode(TSharedPtr<class FNaiveStateMachineEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

protected:
	TWeakPtr<class FNaiveStateMachineEditor> StateMachineEditor;

	// Set of spawnable tabs in behavior tree editing mode
	FWorkflowAllowedTabSet StateMachineEditorTabFactories;
};