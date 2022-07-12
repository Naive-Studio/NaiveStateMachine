// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineAssetTypeActions.h"
#include "StateMachine/NaiveStateMachine.h"
#include "NaiveStateMachineEditor.h"
#include "NaiveStateMachineEditorModule.h"
#include "Subsystems/AssetEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "StateMachineAssetTypeActions"

FNaiveStateMachineAssetTypeActions::FNaiveStateMachineAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
{
	AssetCategory = InAssetCategory;
}

FText FNaiveStateMachineAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions_StateMachine", "AssetTypeActions_StateMachine", "State Machine");
}

FColor FNaiveStateMachineAssetTypeActions::GetTypeColor() const
{
	return FColor::Silver;
}

UClass* FNaiveStateMachineAssetTypeActions::GetSupportedClass() const
{
	return UNaiveStateMachine::StaticClass();
}

void FNaiveStateMachineAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto& Object : InObjects)
	{
		UNaiveStateMachine* StateMachineAsset = Cast<UNaiveStateMachine>(Object);
		if (StateMachineAsset != nullptr)
		{
			bool bFoundExisting = false;
			const bool bFocusIfOpen = false;

			FNaiveStateMachineEditor* ExistingInstance = static_cast<FNaiveStateMachineEditor*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(StateMachineAsset,bFocusIfOpen));
			if (ExistingInstance != nullptr && ExistingInstance->GetStateMachineAsset() == nullptr)
			{
				ExistingInstance->InitStateMachineAssetEditor(Mode, EditWithinLevelEditor, StateMachineAsset);
				bFoundExisting = true;
			}

			if (!bFoundExisting)
			{
				FNaiveStateMachineEditorModule& StateMachineEditorModule = FModuleManager::GetModuleChecked<FNaiveStateMachineEditorModule>("NaiveStateMachineEditor");
				StateMachineEditorModule.CreateStateMachineEditor(Mode, EditWithinLevelEditor, StateMachineAsset);
			}
		}
	}
}

uint32 FNaiveStateMachineAssetTypeActions::GetCategories()
{
	return AssetCategory;
}

#undef LOCTEXT_NAMESPACE