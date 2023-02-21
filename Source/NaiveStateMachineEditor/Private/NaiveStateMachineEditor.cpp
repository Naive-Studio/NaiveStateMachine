// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineEditor.h"
#include "StateMachine/NaiveStateMachine.h"
#include "NaiveStateMachineGraph.h"
#include "NaiveStateMachineGraphSchema.h"
#include "NaiveStateMachineEditorModule.h"
#include "NaiveStateMachineNode_SubMachine.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineEditorFactories.h"
#include "NaiveStateMachineEditorApplicationMode.h"

#include "GraphEditor.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "Framework/Docking/TabManager.h"
#include "UObject/Package.h"
#include "Modules/ModuleManager.h"
#include "IDetailsView.h"
#include "Widgets/Docking/SDockTab.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "StateMachine/NaiveStateNode_Blueprint.h"
#include "StateMachine/NaiveTransitionNode_Blueprint.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

#define LOCTEXT_NAMESPACE "NaiveStateMachineEditor"

const FName FNaiveStateMachineEditor::StateMachineGraphTabId = TEXT("Document");
const FName FNaiveStateMachineEditor::StateMachineDetailsTabId = TEXT("Details");
const FName FNaiveStateMachineEditor::StateMachineMode = TEXT("StateMachine");

class FNaiveStateMachineCommands : public TCommands<FNaiveStateMachineCommands>
{
public:
	FNaiveStateMachineCommands() : TCommands<FNaiveStateMachineCommands>("StateMachine.Nodes", LOCTEXT("Nodes", "Nodes"), NAME_None, FAppStyle::Get().GetStyleSetName())
	{
	}

	TSharedPtr<FUICommandInfo> NewState;
	TSharedPtr<FUICommandInfo> NewTransition;

	/** Initialize commands */
	virtual void RegisterCommands() override
	{
		UI_COMMAND(NewState, "New State", "Create new state blueprint object", EUserInterfaceActionType::Button, FInputChord());
		//UI_COMMAND(NewSubmachine, "New Submachine", "Create new sub-machine blueprint object", EUserInterfaceActionType::Button, FInputChord());
		UI_COMMAND(NewTransition, "New Transition", "Create new transition blueprint object", EUserInterfaceActionType::Button, FInputChord());
	}
};


FNaiveStateMachineEditor::FNaiveStateMachineEditor()
	:INaiveStateMachineEditor()
{
	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FNaiveStateMachineEditor::OnPackageSaved);

	bCheckDirtyOnAssetSave = true;

	DetailsView = nullptr;

	FNaiveStateMachineCommands::Register();
}

FNaiveStateMachineEditor::~FNaiveStateMachineEditor()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
	FNaiveStateMachineCommands::Unregister();
}

void FNaiveStateMachineEditor::InitStateMachineAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UNaiveStateMachine* InStateMachine)
{
	if (InStateMachine)
	{
		StateMachine = InStateMachine;
	}

	const TSharedPtr<FNaiveStateMachineEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);

		// Register the document factories
		{
			const TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FStateMachineGraphEditorSummoner(ThisPtr,
				FStateMachineGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FNaiveStateMachineEditor::CreateGraphEditorWidget)
			));

			// Also store off a reference to the graph editor factory so we can find all the tabs spawned by it later.
			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(StateMachine);
	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		FGraphEditorCommands::Register();

		constexpr bool bCreateDefaultStandaloneMenu = true;
		constexpr bool bCreateDefaultToolbar = true;
		const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());

		InitAssetEditor(
			Mode,
			InitToolkitHost,
			FNaiveStateMachineEditorModule::StateMachineEditorAppIdentifier,
			DummyLayout,
			bCreateDefaultStandaloneMenu,
			bCreateDefaultToolbar,
			ObjectsToEdit);

		FNaiveStateMachineEditorModule& StateMachineEditorModule = FModuleManager::LoadModuleChecked<FNaiveStateMachineEditorModule>("NaiveStateMachineEditor");
		AddMenuExtender(StateMachineEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

		AddApplicationMode(StateMachineMode, MakeShareable(new FNaiveStateMachineEditorApplicationMode(SharedThis(this))));
		SetCurrentMode(StateMachineMode);

		BindCommands();
		RegenerateMenusAndToolbars();
	}
}

void FNaiveStateMachineEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);

	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

TSharedRef<class SGraphEditor> FNaiveStateMachineEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph);

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = NSLOCTEXT("AppearanceCornerText_NaiveStateMachineEditor", "AppearanceCornerText", "FINITE STATE MACHINE");
	AppearanceInfo.InstructionText = LOCTEXT("AppearanceInstructionText_NaiveStateMachineEditor", "Right click to add new nodes.");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FNaiveStateMachineEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FNaiveStateMachineEditor::OnGraphNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FNaiveStateMachineEditor::OnNodeTitleCommitted);

	CreateCommandList();

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("NaiveStateMachineEditor", "StateMachineGraphLabel", "State Machine Graph"))
		.TextStyle(FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
		]
		];

	// Make full graph editor
	GraphEditorView = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);; // Keep pointer to editor

	return GraphEditorView.ToSharedRef();
}

void FNaiveStateMachineEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if (NewSelection.Num())
	{
		for (TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
			UNaiveStateMachineGraphNode* GraphNode = Cast<UNaiveStateMachineGraphNode>(*SetIt);
			if (GraphNode)
			{
				{
					Selection.Add(GraphNode);
				}
			}
		}
	}

	if (Selection.Num() == 1)
	{
		DetailsView->SetObjects(Selection);
	}
	else
	{
		DetailsView->SetObject(NULL);
	}
}

void FNaiveStateMachineEditor::OnGraphNodeDoubleClicked(class UEdGraphNode* Node) const
{
	const UNaiveStateMachineGraphNode* GraphNode = Cast<UNaiveStateMachineGraphNode>(Node);
	UObject* JumpTarget = nullptr;
	if (GraphNode->IsA(UNaiveStateMachineNode_SubMachine::StaticClass()))
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(GraphNode->GetJumpTargetForDoubleClick());
	}

	if(const UClass* TemplateClass = GraphNode->GetTemplateClass())
	{
		UPackage* Pkg = TemplateClass->GetOuterUPackage();
		const FString& ClassName = TemplateClass->GetName().LeftChop(2);

		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(FindObject<UBlueprint>(Pkg, *ClassName));
	}
}

void FNaiveStateMachineEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		static const FText TransactionTitle = FText::FromString(FString(TEXT("Rename Node")));
		const FScopedTransaction Transaction(TransactionTitle);
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

void FNaiveStateMachineEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	UpdateGraphEdPtr = InGraphEditor;
	//FocusedGraphOwner = Cast<UNaiveStateMachineNode_SubMachine>(InGraphEditor->GetCurrentGraph()->GetOuter());

	const FGraphPanelSelectionSet& CurrentSelection = InGraphEditor->GetSelectedNodes();
	if (CurrentSelection.Num() > 0)
	{
		OnSelectedNodesChanged(CurrentSelection);
	}
}

FText FNaiveStateMachineEditor::GetLocalizedMode(FName InMode)
{
	static TMap< FName, FText > LocModes;

	if (LocModes.Num() == 0)
	{
		LocModes.Add(StateMachineMode, LOCTEXT("StateMachineMode", "State Machine"));
	}

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

TSharedRef<SWidget> FNaiveStateMachineEditor::SpawnProperties()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ObjectsUseNameArea;
	DetailsViewArgs.bHideSelectionTip = false;
	
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(nullptr);
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FNaiveStateMachineEditor::OnFinishedChangingProperties);

	return DetailsView.ToSharedRef();
}

void FNaiveStateMachineEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::UnregisterTabSpawners(InTabManager);
}

FName FNaiveStateMachineEditor::GetToolkitFName() const
{
	return FName("NaiveStateMachineEditor");
}

FText FNaiveStateMachineEditor::GetBaseToolkitName() const
{
	return LOCTEXT("NaiveStateMachineBaseToolkitName", "NaiveStateMachineAssetEditor");
}

FString FNaiveStateMachineEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("NaiveGameplayCustomized");
}

FLinearColor FNaiveStateMachineEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

UNaiveStateMachine* FNaiveStateMachineEditor::GetStateMachineAsset() const
{
	return StateMachine;
}

bool FNaiveStateMachineEditor::IsPropertyEditable() const
{
	TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin();
	return FocusedGraphEd.IsValid() && FocusedGraphEd->GetCurrentGraph() && FocusedGraphEd->GetCurrentGraph()->bEditable;
}

void FNaiveStateMachineEditor::OnPackageSaved(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext PostSaveContext)
{
	SaveAsset_Execute();
}

void FNaiveStateMachineEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
// 	if (FocusedGraphOwner.IsValid())
// 	{
// 		FocusedGraphOwner->OnInnerGraphChanged();
// 	}
}

void FNaiveStateMachineEditor::SaveAsset_Execute()
{
	UpdateGraphAsset();

	// save it
	FAssetEditorToolkit::SaveAsset_Execute();
}

void FNaiveStateMachineEditor::UpdateGraphAsset()
{
	// modify BT asset
	TSharedPtr<SGraphEditor> UpdateGraphEditor = UpdateGraphEdPtr.Pin();
	if (UpdateGraphEditor.IsValid() && UpdateGraphEditor->GetCurrentGraph() != NULL)
	{
		//let's find root node
		UNaiveStateMachineGraph* EdGraph = Cast<UNaiveStateMachineGraph>(UpdateGraphEditor->GetCurrentGraph());
		EdGraph->UpdateAsset();
	}
}

void FNaiveStateMachineEditor::ExtendToolbar(TSharedPtr<FExtender> Extender)
{
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Nodes");
			{
				ToolbarBuilder.AddToolBarButton(FNaiveStateMachineCommands::Get().NewState, 
					NAME_None, TAttribute<FText>(), TAttribute<FText>(), 
					FSlateIcon(FAppStyle::Get().GetStyleSetName(), "BTEditor.Graph.NewTask","BTEditor.Graph.NewTask.Small"));
// 				ToolbarBuilder.AddToolBarButton(FNaiveStateMachineCommands::Get().NewSubmachine,
// 					NAME_None, TAttribute<FText>(), TAttribute<FText>(),
// 					FSlateIcon(FEditorStyle::GetStyleSetName(), "BTEditor.Graph.NewDecorator", "BTEditor.Graph.NewDecorator.Small"));
				ToolbarBuilder.AddToolBarButton(FNaiveStateMachineCommands::Get().NewTransition,
					NAME_None, TAttribute<FText>(), TAttribute<FText>(),
					FSlateIcon(FAppStyle::Get().GetStyleSetName(), "BTEditor.Graph.NewService", "BTEditor.Graph.NewService.Small"));
			}
			ToolbarBuilder.EndSection();
		}
	};

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		ToolkitCommands,
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar)
	);

	AddToolbarExtender(Extender);
}

void FNaiveStateMachineEditor::BindCommands()
{
	ToolkitCommands->MapAction(FNaiveStateMachineCommands::Get().NewState,
		FExecuteAction::CreateSP(this, &FNaiveStateMachineEditor::CreateNewState)
	);

// 	ToolkitCommands->MapAction(FNaiveStateMachineCommands::Get().NewSubmachine,
// 		FExecuteAction::CreateSP(this, &FNaiveStateMachineEditor::CreateNewSubmachine)
// 	);

	ToolkitCommands->MapAction(FNaiveStateMachineCommands::Get().NewTransition,
		FExecuteAction::CreateSP(this, &FNaiveStateMachineEditor::CreateNewTraisntion)
	);
}

void FNaiveStateMachineEditor::CreateNewState()
{
	HandleNewNodeClassPicked(UNaiveStateNode_Blueprint::StaticClass());
}

// void FStateMachineEditor::CreateNewSubmachine()
// {
// 	HandleNewNodeClassPicked(USLSubStateMachine::StaticClass());
// }

void FNaiveStateMachineEditor::CreateNewTraisntion()
{
	HandleNewNodeClassPicked(UNaiveTransitionNode_Blueprint::StaticClass());
}

void FNaiveStateMachineEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager)
{
	// 	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_StateMachineEditor", "State Machine Editor"));
// 	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
// 
// 
// 	InTabManager->RegisterTabSpawner(StateMachineGraphTabId, FOnSpawnTab::CreateSP(this, &FStateMachineEditor::SpawnTab_MainGraph))
// 		.SetDisplayName(NSLOCTEXT("NaiveStateMachineEditor", "Graph", "Graph"))
// 		.SetGroup(CurrentAppModePtr->GetWorkspaceMenuCategory())
// 		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.StateMachine_16x"));
 
// 	InTabManager->RegisterTabSpawner(StateMachinePropertiesTabId, FOnSpawnTab::CreateSP(this, &FStateMachineEditor::SpawnTab_Properties))
// 		.SetDisplayName(NSLOCTEXT("NaiveStateMachineEditor", "PropertiesTab", "Details"))
// 		.SetGroup(CurrentAppModePtr->GetWorkspaceMenuCategory())
// 		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FNaiveStateMachineEditor::RestoreStateMachine()
{
	UNaiveStateMachineGraph* MyGraph = Cast<UNaiveStateMachineGraph>(StateMachine->EdGraph);
	const bool bNewGraph = MyGraph == NULL;
	if (StateMachine->EdGraph == NULL)
	{
		StateMachine->EdGraph = FBlueprintEditorUtils::CreateNewGraph(StateMachine, TEXT("StateMachine"), UNaiveStateMachineGraph::StaticClass(), UNaiveStateMachineGraphSchema::StaticClass());
		MyGraph = Cast<UNaiveStateMachineGraph>(StateMachine->EdGraph);

		// let's read data from BT script and generate nodes
		const UEdGraphSchema* Schema = StateMachine->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*StateMachine->EdGraph);

		MyGraph->OnCreated();
	}
	else
	{
		MyGraph->OnLoaded();
	}

	MyGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(MyGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);
	
// 	if (StateMachine->LastEditedDocuments.Num() > 0)
// 	{
// 		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
// 		GraphEditor->SetViewLocation(BehaviorTree->LastEditedDocuments[0].SavedViewOffset, BehaviorTree->LastEditedDocuments[0].SavedZoomAmount);
// 	}
// 
// 	const bool bIncreaseVersionNum = false;
// 	if (bNewGraph)
// 	{
// 		MyGraph->UpdateAsset(UBehaviorTreeGraph::ClearDebuggerFlags | UBehaviorTreeGraph::KeepRebuildCounter);
// 		RefreshBlackboardViewsAssociatedObject();
// 	}
// 	else
// 	{
// 		MyGraph->UpdateAsset(UBehaviorTreeGraph::KeepRebuildCounter);
// 		//RefreshDebugger();
// 	}
}

void FNaiveStateMachineEditor::HandleNewNodeClassPicked(UClass* InClass) const
{
	if (StateMachine != nullptr)
	{
		FString ClassName = FBlueprintEditorUtils::GetClassNameWithoutSuffix(InClass);

		FString PathName = StateMachine->GetOutermost()->GetPathName();
		PathName = FPaths::GetPath(PathName);
		PathName /= ClassName;

		FString Name;
		FString PackageName;
		const FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(PathName, TEXT("_New"), PackageName, Name);
		
		UPackage* Package = CreatePackage(*PackageName);
		if (ensure(Package))
		{
			// Create and init a new Blueprint
			if (UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(InClass, Package, FName(*Name), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass()))
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(NewBP);

				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(NewBP);

				// Mark the package dirty...
				Package->MarkPackageDirty();
			}
		}
	}
}


#undef LOCTEXT_NAMESPACE