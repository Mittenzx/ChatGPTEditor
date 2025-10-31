// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChatGPTEditor : ModuleRules
{
	public ChatGPTEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"HTTP",
				"Json",
				"JsonUtilities",
				"BlueprintGraph",
				"Kismet",
				"KismetCompiler",
				"GraphEditor",
				"DesktopPlatform",
				"AssetTools",
				"AssetRegistry"
			}
		);
		
		// Optional Python support
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("PythonScriptPlugin");
		}
	}
}
