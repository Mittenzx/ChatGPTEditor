// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetAutomation.h"
#include "Misc/MessageDialog.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/TextureFactory.h"
#include "Factories/BlueprintFactory.h"
#include "Materials/Material.h"
#include "Engine/Texture2D.h"
#include "Engine/Blueprint.h"
#include "UObject/SavePackage.h"
#include "PackageTools.h"
#include "ObjectTools.h"

#define LOCTEXT_NAMESPACE "AssetAutomation"

TArray<FAssetOperation> FAssetAutomation::ParseResponse(const FString& Response)
{
	TArray<FAssetOperation> Operations;
	
	// Split response into lines
	TArray<FString> Lines;
	Response.ParseIntoArrayLines(Lines);
	
	for (const FString& Line : Lines)
	{
		FAssetOperation Op = ParseLine(Line);
		if (Op.IsValid())
		{
			Operations.Add(Op);
		}
	}
	
	return Operations;
}

FAssetOperation FAssetAutomation::ParseLine(const FString& Line)
{
	FAssetOperation Op;
	Op.OriginalCommand = Line;
	
	FString LowerLine = Line.ToLower();
	
	// Parse: "Create material [MaterialName]"
	if (LowerLine.Contains(TEXT("create")) && LowerLine.Contains(TEXT("material")))
	{
		Op.Type = FAssetOperation::EType::CreateMaterial;
		Op.AssetName = ExtractAssetName(Line, TEXT("material"));
		Op.TargetPath = TEXT("/Game/Materials");
	}
	// Parse: "Create texture [TextureName]"
	else if (LowerLine.Contains(TEXT("create")) && LowerLine.Contains(TEXT("texture")))
	{
		Op.Type = FAssetOperation::EType::CreateTexture;
		Op.AssetName = ExtractAssetName(Line, TEXT("texture"));
		Op.TargetPath = TEXT("/Game/Textures");
	}
	// Parse: "Create blueprint [BlueprintName]"
	else if (LowerLine.Contains(TEXT("create")) && LowerLine.Contains(TEXT("blueprint")))
	{
		Op.Type = FAssetOperation::EType::CreateBlueprint;
		Op.AssetName = ExtractAssetName(Line, TEXT("blueprint"));
		Op.TargetPath = TEXT("/Game/Blueprints");
	}
	// Parse: "Rename [OldName] to [NewName]"
	else if (LowerLine.Contains(TEXT("rename")) && LowerLine.Contains(TEXT(" to ")))
	{
		Op.Type = FAssetOperation::EType::RenameAsset;
		
		int32 ToIndex = LowerLine.Find(TEXT(" to "));
		if (ToIndex != INDEX_NONE)
		{
			FString BeforeTo = Line.Left(ToIndex);
			FString AfterTo = Line.Mid(ToIndex + 4);
			
			// Extract old name (after "rename")
			int32 RenameIndex = BeforeTo.ToLower().Find(TEXT("rename"));
			if (RenameIndex != INDEX_NONE)
			{
				Op.AssetName = BeforeTo.Mid(RenameIndex + 7).TrimStartAndEnd();
			}
			
			// Extract new name
			Op.NewName = AfterTo.TrimStartAndEnd();
		}
	}
	// Parse: "Delete [AssetName]"
	else if (LowerLine.Contains(TEXT("delete")) && (LowerLine.Contains(TEXT("asset")) || LowerLine.Contains(TEXT("material")) || LowerLine.Contains(TEXT("texture")) || LowerLine.Contains(TEXT("blueprint"))))
	{
		Op.Type = FAssetOperation::EType::DeleteAsset;
		
		// Try to extract asset name after "delete"
		int32 DeleteIndex = LowerLine.Find(TEXT("delete"));
		if (DeleteIndex != INDEX_NONE)
		{
			FString AfterDelete = Line.Mid(DeleteIndex + 7).TrimStartAndEnd();
			// Remove common words
			AfterDelete.ReplaceInline(TEXT("asset"), TEXT(""));
			AfterDelete.ReplaceInline(TEXT("Asset"), TEXT(""));
			AfterDelete.ReplaceInline(TEXT("the"), TEXT(""));
			AfterDelete.ReplaceInline(TEXT("The"), TEXT(""));
			Op.AssetName = AfterDelete.TrimStartAndEnd();
		}
	}
	
	return Op;
}

FString FAssetAutomation::ExtractAssetName(const FString& Command, const FString& Keyword)
{
	FString LowerCommand = Command.ToLower();
	FString LowerKeyword = Keyword.ToLower();
	
	int32 KeywordIndex = LowerCommand.Find(LowerKeyword);
	if (KeywordIndex == INDEX_NONE)
	{
		return TEXT("");
	}
	
	// Get everything after the keyword
	FString AfterKeyword = Command.Mid(KeywordIndex + Keyword.Len()).TrimStartAndEnd();
	
	// Extract the first word/identifier (could be quoted or not)
	if (AfterKeyword.StartsWith(TEXT("\"")))
	{
		// Extract quoted name
		int32 EndQuoteIndex = AfterKeyword.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, 1);
		if (EndQuoteIndex != INDEX_NONE)
		{
			return AfterKeyword.Mid(1, EndQuoteIndex - 1);
		}
	}
	else
	{
		// Extract first word
		int32 SpaceIndex = AfterKeyword.Find(TEXT(" "));
		if (SpaceIndex != INDEX_NONE)
		{
			return AfterKeyword.Left(SpaceIndex);
		}
		return AfterKeyword;
	}
	
	return TEXT("");
}

bool FAssetAutomation::ExecuteOperation(const FAssetOperation& Operation, bool bAllowAssetWrite)
{
	// Check if asset write is allowed
	if (!bAllowAssetWrite)
	{
		FAuditLogEntry Entry;
		Entry.Operation = Operation.GetTypeAsString();
		Entry.AssetName = Operation.AssetName;
		Entry.User = FPlatformProcess::UserName();
		Entry.bSucceeded = false;
		Entry.Details = TEXT("Asset write permission not enabled");
		WriteAuditLog(Entry);
		
		FMessageDialog::Open(EAppMsgType::Ok,
			LOCTEXT("PermissionDenied", "Asset write operations are disabled. Please enable 'Allow Asset Write Operations' to proceed."));
		return false;
	}
	
	// Show confirmation dialog
	if (!ShowConfirmationDialog(Operation))
	{
		FAuditLogEntry Entry;
		Entry.Operation = Operation.GetTypeAsString();
		Entry.AssetName = Operation.AssetName;
		Entry.User = FPlatformProcess::UserName();
		Entry.bSucceeded = false;
		Entry.Details = TEXT("User cancelled operation");
		WriteAuditLog(Entry);
		return false;
	}
	
	// Execute the operation
	bool bSuccess = false;
	FString ErrorDetails;
	
	switch (Operation.Type)
	{
	case FAssetOperation::EType::CreateMaterial:
		bSuccess = CreateMaterial(Operation);
		break;
	case FAssetOperation::EType::CreateTexture:
		bSuccess = CreateTexture(Operation);
		break;
	case FAssetOperation::EType::CreateBlueprint:
		bSuccess = CreateBlueprint(Operation);
		break;
	case FAssetOperation::EType::RenameAsset:
		bSuccess = RenameAsset(Operation);
		break;
	case FAssetOperation::EType::MoveAsset:
		bSuccess = MoveAsset(Operation);
		break;
	case FAssetOperation::EType::DeleteAsset:
		bSuccess = DeleteAsset(Operation);
		break;
	default:
		ErrorDetails = TEXT("Unknown operation type");
		break;
	}
	
	// Log the result
	FAuditLogEntry Entry;
	Entry.Operation = Operation.GetTypeAsString();
	Entry.AssetName = Operation.AssetName;
	Entry.User = FPlatformProcess::UserName();
	Entry.bSucceeded = bSuccess;
	Entry.Details = bSuccess ? TEXT("Operation completed successfully") : ErrorDetails;
	WriteAuditLog(Entry);
	
	return bSuccess;
}

bool FAssetAutomation::ShowConfirmationDialog(const FAssetOperation& Operation)
{
	FText Title = FText::Format(
		LOCTEXT("ConfirmOperationTitle", "Confirm: {0}"),
		FText::FromString(Operation.GetTypeAsString())
	);
	
	FText Message;
	if (Operation.Type == FAssetOperation::EType::RenameAsset)
	{
		Message = FText::Format(
			LOCTEXT("ConfirmRename", "Asset Operation Preview:\n\nOperation: {0}\nAsset: {1}\nNew Name: {2}\n\nCommand: {3}\n\nDo you want to proceed?"),
			FText::FromString(Operation.GetTypeAsString()),
			FText::FromString(Operation.AssetName),
			FText::FromString(Operation.NewName),
			FText::FromString(Operation.OriginalCommand)
		);
	}
	else
	{
		Message = FText::Format(
			LOCTEXT("ConfirmOperation", "Asset Operation Preview:\n\nOperation: {0}\nAsset: {1}\nPath: {2}\n\nCommand: {3}\n\n{4}\n\nDo you want to proceed?"),
			FText::FromString(Operation.GetTypeAsString()),
			FText::FromString(Operation.AssetName),
			FText::FromString(Operation.TargetPath),
			FText::FromString(Operation.OriginalCommand),
			Operation.IsDestructive() ? 
				FText::FromString(TEXT("⚠️ WARNING: This is a destructive operation!")) :
				FText::GetEmpty()
		);
	}
	
	EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message, &Title);
	return Result == EAppReturnType::Yes;
}

void FAssetAutomation::WriteAuditLog(const FAuditLogEntry& Entry)
{
	FString LogPath = GetAuditLogPath();
	
	// Create directory if it doesn't exist
	FString LogDir = FPaths::GetPath(LogPath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*LogDir))
	{
		PlatformFile.CreateDirectoryTree(*LogDir);
	}
	
	// Format log entry
	FString LogEntry = FString::Printf(
		TEXT("[%s] User: %s | Operation: %s | Asset: %s | Success: %s | Details: %s\n"),
		*Entry.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S")),
		*Entry.User,
		*Entry.Operation,
		*Entry.AssetName,
		Entry.bSucceeded ? TEXT("YES") : TEXT("NO"),
		*Entry.Details
	);
	
	// Append to log file
	FFileHelper::SaveStringToFile(LogEntry, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

FString FAssetAutomation::GetAuditLogPath()
{
	return FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
}

bool FAssetAutomation::CreateMaterial(const FAssetOperation& Operation)
{
	if (Operation.AssetName.IsEmpty())
	{
		return false;
	}
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();
	
	FString PackageName = Operation.TargetPath / Operation.AssetName;
	FString AssetName = Operation.AssetName;
	
	// Create the factory
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
	
	// Create the asset
	UObject* NewAsset = AssetTools.CreateAsset(AssetName, Operation.TargetPath, UMaterial::StaticClass(), Factory);
	
	return NewAsset != nullptr;
}

bool FAssetAutomation::CreateTexture(const FAssetOperation& Operation)
{
	if (Operation.AssetName.IsEmpty())
	{
		return false;
	}
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();
	
	FString PackageName = Operation.TargetPath / Operation.AssetName;
	FString AssetName = Operation.AssetName;
	
	// Create the factory
	UTextureFactory* Factory = NewObject<UTextureFactory>();
	
	// Create the asset
	UObject* NewAsset = AssetTools.CreateAsset(AssetName, Operation.TargetPath, UTexture2D::StaticClass(), Factory);
	
	return NewAsset != nullptr;
}

bool FAssetAutomation::CreateBlueprint(const FAssetOperation& Operation)
{
	if (Operation.AssetName.IsEmpty())
	{
		return false;
	}
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();
	
	FString PackageName = Operation.TargetPath / Operation.AssetName;
	FString AssetName = Operation.AssetName;
	
	// Create the factory
	UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
	Factory->ParentClass = AActor::StaticClass();
	
	// Create the asset
	UObject* NewAsset = AssetTools.CreateAsset(AssetName, Operation.TargetPath, UBlueprint::StaticClass(), Factory);
	
	return NewAsset != nullptr;
}

bool FAssetAutomation::RenameAsset(const FAssetOperation& Operation)
{
	if (Operation.AssetName.IsEmpty() || Operation.NewName.IsEmpty())
	{
		return false;
	}
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	// Find the asset
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssetsByPackageName(FName(*Operation.AssetName), AssetDataList);
	
	if (AssetDataList.Num() == 0)
	{
		// Try to find by asset name alone
		AssetRegistry.GetAssetsByClass(UObject::StaticClass()->GetFName(), AssetDataList);
		AssetDataList = AssetDataList.FilterByPredicate([&](const FAssetData& Data) {
			return Data.AssetName.ToString() == Operation.AssetName;
		});
	}
	
	if (AssetDataList.Num() == 0)
	{
		return false;
	}
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();
	
	// Rename the asset
	TArray<FAssetRenameData> RenameData;
	FAssetRenameData NewRenameData(AssetDataList[0].GetAsset(), AssetDataList[0].PackagePath.ToString(), Operation.NewName);
	RenameData.Add(NewRenameData);
	
	return AssetTools.RenameAssets(RenameData);
}

bool FAssetAutomation::MoveAsset(const FAssetOperation& Operation)
{
	if (Operation.AssetName.IsEmpty() || Operation.TargetPath.IsEmpty())
	{
		return false;
	}
	
	// Note: Move operation would be similar to rename but with different path
	// For simplicity, this is a placeholder
	return false;
}

bool FAssetAutomation::DeleteAsset(const FAssetOperation& Operation)
{
	if (Operation.AssetName.IsEmpty())
	{
		return false;
	}
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	// Find the asset
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssetsByPackageName(FName(*Operation.AssetName), AssetDataList);
	
	if (AssetDataList.Num() == 0)
	{
		// Try to find by asset name alone
		AssetRegistry.GetAssetsByClass(UObject::StaticClass()->GetFName(), AssetDataList);
		AssetDataList = AssetDataList.FilterByPredicate([&](const FAssetData& Data) {
			return Data.AssetName.ToString() == Operation.AssetName;
		});
	}
	
	if (AssetDataList.Num() == 0)
	{
		return false;
	}
	
	// Delete the asset
	TArray<UObject*> ObjectsToDelete;
	ObjectsToDelete.Add(AssetDataList[0].GetAsset());
	
	return ObjectTools::DeleteObjects(ObjectsToDelete, true) > 0;
}

#undef LOCTEXT_NAMESPACE
