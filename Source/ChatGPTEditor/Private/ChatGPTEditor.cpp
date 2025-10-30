// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChatGPTEditor.h"
#include "SChatGPTWindow.h"
#include "MCP/SMCPTestWindow.h"
#include "AuditLogger.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"

static const FName ChatGPTEditorTabName("ChatGPTEditor");
static const FName MCPTestWindowTabName("MCPTestWindow");

// Define log category
DEFINE_LOG_CATEGORY(LogChatGPTEditor);

#define LOCTEXT_NAMESPACE "FChatGPTEditorModule"

void FChatGPTEditorModule::StartupModule()
{
	// Initialize audit logger
	FAuditLogger::Get().Initialize();
	FAuditLogger::Get().LogEvent(TEXT("MODULE_STARTUP"), TEXT("ChatGPT Editor module started"));
	
	// Register legacy ChatGPT tab spawner - Make it visible in the Window menu
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ChatGPTEditorTabName, FOnSpawnTab::CreateRaw(this, &FChatGPTEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FChatGPTEditorTabTitle", "ChatGPT (Legacy)"))
		.SetMenuType(ETabSpawnerMenuType::Enabled)
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
	
	// Register MCP Test Window
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MCPTestWindowTabName, FOnSpawnTab::CreateRaw(this, &FChatGPTEditorModule::OnSpawnMCPTestTab))
		.SetDisplayName(LOCTEXT("FMCPTestWindowTabTitle", "MCP Test Window"))
		.SetMenuType(ETabSpawnerMenuType::Enabled)
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

void FChatGPTEditorModule::ShutdownModule()
{
	// Log shutdown
	FAuditLogger::Get().LogEvent(TEXT("MODULE_SHUTDOWN"), TEXT("ChatGPT Editor module shutting down"));
	FAuditLogger::Get().Shutdown();
	
	// Unregister tab spawners
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ChatGPTEditorTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MCPTestWindowTabName);
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

TSharedRef<SDockTab> FChatGPTEditorModule::OnSpawnMCPTestTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Create the MCP Test Window widget
			SNew(SMCPTestWindow)
		];
}

void FChatGPTEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ChatGPTEditorTabName);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FChatGPTEditorModule, ChatGPTEditor)
