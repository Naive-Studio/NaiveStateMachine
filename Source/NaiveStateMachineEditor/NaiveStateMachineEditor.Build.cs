// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NaiveStateMachineEditor : ModuleRules
{
	public NaiveStateMachineEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				"NaiveStateMachineEditor/Private",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"BlueprintGraph",
				"NaiveStateMachine",
				"AIGraph",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "SlateCore",
				// ... add private dependencies that you statically link with here ...	
                "UnrealEd",
                "EditorStyle",
                "Projects", // IPluginManager
				"RenderCore",
                "InputCore",
                "EditorStyle",
                "MessageLog",
                "EditorWidgets",

				// Accessing the menu
				"WorkspaceMenuStructure",

				// Used for the Blueprint Nodes
				"BlueprintGraph",
                "Kismet",
                "KismetCompiler",
                "KismetWidgets",

				// graph stuff
				"GraphEditor",
                "ContentBrowser",

				// e.g. FPlatformApplicationMisc::ClipboardCopy
				"ApplicationCore",
                "PropertyEditor",
                "ToolMenus",
				"AssetTools",
				"AssetRegistry"

			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
