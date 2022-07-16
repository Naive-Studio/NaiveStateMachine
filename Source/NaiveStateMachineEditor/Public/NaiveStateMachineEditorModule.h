// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "Toolkits/AssetEditorToolkit.h"

class INaiveStateMachineEditor;
class UNaiveStateMachine;

class FNaiveStateMachineEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Gets the extensibility managers for outside entities to extend this editor's menus and toolbars */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

	TSharedRef<INaiveStateMachineEditor> CreateStateMachineEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UNaiveStateMachine* InTargetAsset);

private:
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

public:
	static const FName StateMachineEditorAppIdentifier;

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

	/** All created asset type actions.  Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

	EAssetTypeCategories::Type NaiveStateMachineAssetCategoryBit;

	TSharedPtr<class FStateMachineGraphPanelNodeFactory> GraphPanelNodeFactory;
	TSharedPtr<class FStateMachineGraphPinFactory> GraphPinFactory;
	TSharedPtr<class FStateMachinePinConnectionFactory> PinConnectionFactory;
};
