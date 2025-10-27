// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChatGPTEditor.h"
#include "SChatGPTWindow.h"
#include "AuditLogger.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"

static const FName ChatGPTEditorTabName("ChatGPTEditor");

// Define log category
DEFINE_LOG_CATEGORY(LogChatGPTEditor);

#define LOCTEXT_NAMESPACE "FChatGPTEditorModule"

void FChatGPTEditorModule::StartupModule()
{
	// Initialize audit logger
	FAuditLogger::Get().Initialize();
	FAuditLogger::Get().LogEvent(TEXT("MODULE_STARTUP"), TEXT("ChatGPT Editor module started"));
	
	// Register tab spawner - Make it visible in the Window menu
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ChatGPTEditorTabName, FOnSpawnTab::CreateRaw(this, &FChatGPTEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FChatGPTEditorTabTitle", "ChatGPT"))
		.SetMenuType(ETabSpawnerMenuType::Enabled)
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

void FChatGPTEditorModule::ShutdownModule()
{
	// Log shutdown
	FAuditLogger::Get().LogEvent(TEXT("MODULE_SHUTDOWN"), TEXT("ChatGPT Editor module shutting down"));
	FAuditLogger::Get().Shutdown();
	
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FChatGPTEditorModule, ChatGPTEditor)
