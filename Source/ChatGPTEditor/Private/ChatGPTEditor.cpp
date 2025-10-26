// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChatGPTEditor.h"
#include "SChatGPTWindow.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"

static const FName ChatGPTEditorTabName("ChatGPTEditor");

#define LOCTEXT_NAMESPACE "FChatGPTEditorModule"

void FChatGPTEditorModule::StartupModule()
{
	// Register tab spawner
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ChatGPTEditorTabName, FOnSpawnTab::CreateRaw(this, &FChatGPTEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FChatGPTEditorTabTitle", "ChatGPT"))
		.SetMenuType(ETabSpawnerMenuType::Hidden)
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

void FChatGPTEditorModule::ShutdownModule()
{
	// Unregister tab spawner
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ChatGPTEditorTabName);
}

TSharedRef<SDockTab> FChatGPTEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Create the ChatGPT window widget
			SNew(SChatGPTWindow)
		];
}

void FChatGPTEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ChatGPTEditorTabName);
}

void FChatGPTEditorModule::RegisterMenus()
{
	// Register menu extensions if needed
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FChatGPTEditorModule::PluginButtonClicked));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FChatGPTEditorModule, ChatGPTEditor)
