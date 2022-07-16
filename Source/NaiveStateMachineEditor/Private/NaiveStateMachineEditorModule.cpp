// Copyright Epic Games, Inc. All Rights Reserved.

#include "NaiveStateMachineEditorModule.h"
#include "NaiveStateMachineAssetTypeActions.h"
#include "NaiveStateMachineEditorFactories.h"
#include "NaiveStateMachineEditor.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "FNaiveStateMachineEditorModule"

const FName FNaiveStateMachineEditorModule::StateMachineEditorAppIdentifier(TEXT("NaiveStateMachineEditorApp"));


void FNaiveStateMachineEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	NaiveStateMachineAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Naive State Machine")), LOCTEXT("NaiveStateMachine", "Naive State Machine"));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FNaiveStateMachineAssetTypeActions(NaiveStateMachineAssetCategoryBit)));

	GraphPanelNodeFactory = MakeShareable(new FStateMachineGraphPanelNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory);

	GraphPinFactory = MakeShareable(new FStateMachineGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(GraphPinFactory);

	PinConnectionFactory = MakeShareable(new FStateMachinePinConnectionFactory());
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(PinConnectionFactory);
}

void FNaiveStateMachineEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Unregister our custom created assets from the AssetTools
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 i = 0; i < CreatedAssetTypeActions.Num(); ++i)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[i].ToSharedRef());
		}
	}

	CreatedAssetTypeActions.Empty();

	if (GraphPanelNodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory);
		GraphPanelNodeFactory.Reset();
	}
}

TSharedRef<INaiveStateMachineEditor> FNaiveStateMachineEditorModule::CreateStateMachineEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UNaiveStateMachine* InTargetAsset)
{
	TSharedRef<FNaiveStateMachineEditor> NewStateMachineEditor(new FNaiveStateMachineEditor());
	NewStateMachineEditor->InitStateMachineAssetEditor(Mode, InitToolkitHost, InTargetAsset);

	return NewStateMachineEditor;
}

void FNaiveStateMachineEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNaiveStateMachineEditorModule, NaiveStateMachineEditor)