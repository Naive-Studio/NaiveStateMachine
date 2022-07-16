// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UNaiveStateMachine;

class INaiveStateMachineEditor : public FWorkflowCentricApplication
{
public:
	virtual UNaiveStateMachine* GetStateMachineAsset() const = 0;

	virtual void InitStateMachineAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UNaiveStateMachine* InStateMachine) = 0;
};
 