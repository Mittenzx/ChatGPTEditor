// Copyright Epic Games, Inc. All Rights Reserved.

#include "SceneEditingManager.h"
#include "AuditLogger.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Light.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/DirectionalLight.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/TriggerVolume.h"
#include "EngineUtils.h"
#include "Misc/MessageDialog.h"

FSceneEditingManager& FSceneEditingManager::Get()
{
	static FSceneEditingManager Instance;
	return Instance;
}

TArray<FSceneEditAction> FSceneEditingManager::ParseCommand(const FString& Command)
{
	TArray<FSceneEditAction> Actions;
	FString LowerCommand = Command.ToLower();

	// Parse spawn commands
	if (LowerCommand.Contains(TEXT("add")) || LowerCommand.Contains(TEXT("spawn")) || LowerCommand.Contains(TEXT("place")))
	{
		FSceneEditAction Action;
		Action.Operation = ESceneEditOperation::SpawnActor;
		Action.ActorClass = ParseActorType(Command);
		Action.Count = ParseCount(Command);
		Action.Description = Command;
		
		// Check for location-based placement
		if (LowerCommand.Contains(TEXT("at playerstart")) || LowerCommand.Contains(TEXT("at player start")))
		{
			Action.Location = FVector::ZeroVector; // Will be resolved to PlayerStart location
			Action.PropertyName = TEXT("AtPlayerStart");
		}
		
		Actions.Add(Action);
	}
	// Parse delete commands
	else if (LowerCommand.Contains(TEXT("delete")) || LowerCommand.Contains(TEXT("remove")))
	{
		FSceneEditAction Action;
		Action.Operation = ESceneEditOperation::DeleteActor;
		Action.SearchPattern = ParseActorType(Command);
		Action.Description = Command;
		Actions.Add(Action);
	}
	// Parse move commands
	else if (LowerCommand.Contains(TEXT("move")))
	{
		FSceneEditAction Action;
		Action.Operation = ESceneEditOperation::MoveActor;
		Action.SearchPattern = ParseActorType(Command);
		Action.Location = ParseMovementOffset(Command);
		Action.Description = Command;
		Actions.Add(Action);
	}
	// Parse property modification commands
	else if (LowerCommand.Contains(TEXT("change")) || LowerCommand.Contains(TEXT("set")) || LowerCommand.Contains(TEXT("modify")))
	{
		FSceneEditAction Action;
		Action.Operation = ESceneEditOperation::ModifyProperty;
		Action.SearchPattern = ParseActorType(Command);
		Action.Description = Command;
		
		// Parse property changes (e.g., "change light color to red")
		if (LowerCommand.Contains(TEXT("color")))
		{
			Action.PropertyName = TEXT("Color");
			if (LowerCommand.Contains(TEXT("red")))
				Action.PropertyValue = TEXT("Red");
			else if (LowerCommand.Contains(TEXT("blue")))
				Action.PropertyValue = TEXT("Blue");
			else if (LowerCommand.Contains(TEXT("green")))
				Action.PropertyValue = TEXT("Green");
		}
		
		Actions.Add(Action);
	}

	return Actions;
}

bool FSceneEditingManager::ExecuteActions(const TArray<FSceneEditAction>& Actions, UWorld* World, bool bPreviewOnly)
{
	if (!World)
	{
		FAuditLogger::Get().LogOperation(TEXT("Execute Actions"), TEXT("Error"), TEXT("None"), false, TEXT("Invalid world"));
		return false;
	}

	// For each action, execute it
	for (const FSceneEditAction& Action : Actions)
	{
		FString AffectedActors;
		bool bSuccess = false;

		switch (Action.Operation)
		{
			case ESceneEditOperation::SpawnActor:
			{
				TArray<AActor*> SpawnedActors = SpawnActors(Action, World);
				bSuccess = SpawnedActors.Num() > 0;
				AffectedActors = FString::Printf(TEXT("%d actors spawned"), SpawnedActors.Num());
				FAuditLogger::Get().LogOperation(Action.Description, TEXT("Spawn"), AffectedActors, bSuccess);
				break;
			}
			
			case ESceneEditOperation::DeleteActor:
			{
				TArray<FString> DeletedActors = DeleteActors(Action, World);
				bSuccess = DeletedActors.Num() > 0;
				AffectedActors = FString::Join(DeletedActors, TEXT(", "));
				FAuditLogger::Get().LogOperation(Action.Description, TEXT("Delete"), AffectedActors, bSuccess);
				break;
			}
			
			case ESceneEditOperation::MoveActor:
			{
				TArray<FString> MovedActors = MoveActors(Action, World);
				bSuccess = MovedActors.Num() > 0;
				AffectedActors = FString::Join(MovedActors, TEXT(", "));
				FAuditLogger::Get().LogOperation(Action.Description, TEXT("Move"), AffectedActors, bSuccess);
				break;
			}
			
			case ESceneEditOperation::ModifyProperty:
			{
				TArray<FString> ModifiedActors = ModifyActorProperties(Action, World);
				bSuccess = ModifiedActors.Num() > 0;
				AffectedActors = FString::Join(ModifiedActors, TEXT(", "));
				FAuditLogger::Get().LogOperation(Action.Description, TEXT("Modify"), AffectedActors, bSuccess);
				break;
			}
		}
	}

	return true;
}

TArray<AActor*> FSceneEditingManager::SpawnActors(const FSceneEditAction& Action, UWorld* World)
{
	TArray<AActor*> SpawnedActors;

	if (!World)
		return SpawnedActors;

	// Determine actor class to spawn
	UClass* ActorClass = nullptr;
	
	if (Action.ActorClass.Contains(TEXT("light")))
	{
		if (Action.ActorClass.Contains(TEXT("point")))
			ActorClass = APointLight::StaticClass();
		else if (Action.ActorClass.Contains(TEXT("spot")))
			ActorClass = ASpotLight::StaticClass();
		else if (Action.ActorClass.Contains(TEXT("directional")))
			ActorClass = ADirectionalLight::StaticClass();
		else
			ActorClass = APointLight::StaticClass(); // Default to point light
	}
	else if (Action.ActorClass.Contains(TEXT("camera")))
	{
		ActorClass = ACameraActor::StaticClass();
	}
	else
	{
		// Default to static mesh actor
		ActorClass = AStaticMeshActor::StaticClass();
	}

	// Determine spawn location
	FVector SpawnLocation = Action.Location;
	if (Action.PropertyName == TEXT("AtPlayerStart"))
	{
		SpawnLocation = FindPlayerStartLocation(World);
	}

	// Spawn the requested number of actors with spacing between them
	static const float ActorSpacingDistance = 100.0f; // Units between spawned actors
	for (int32 i = 0; i < Action.Count; ++i)
	{
		FVector OffsetLocation = SpawnLocation + FVector(i * ActorSpacingDistance, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, OffsetLocation, Action.Rotation, SpawnParams);
		if (SpawnedActor)
		{
			SpawnedActors.Add(SpawnedActor);
		}
	}

	return SpawnedActors;
}

TArray<FString> FSceneEditingManager::DeleteActors(const FSceneEditAction& Action, UWorld* World)
{
	TArray<FString> DeletedActorNames;

	if (!World)
		return DeletedActorNames;

	TArray<AActor*> ActorsToDelete = FindActorsByPattern(Action.SearchPattern, World);

	for (AActor* Actor : ActorsToDelete)
	{
		if (Actor && !Actor->IsPendingKill())
		{
			FString ActorName = Actor->GetName();
			World->DestroyActor(Actor);
			DeletedActorNames.Add(ActorName);
		}
	}

	return DeletedActorNames;
}

TArray<FString> FSceneEditingManager::MoveActors(const FSceneEditAction& Action, UWorld* World)
{
	TArray<FString> MovedActorNames;

	if (!World)
		return MovedActorNames;

	TArray<AActor*> ActorsToMove = FindActorsByPattern(Action.SearchPattern, World);
	FVector Offset = Action.Location;

	for (AActor* Actor : ActorsToMove)
	{
		if (Actor)
		{
			FVector NewLocation = Actor->GetActorLocation() + Offset;
			Actor->SetActorLocation(NewLocation);
			MovedActorNames.Add(Actor->GetName());
		}
	}

	return MovedActorNames;
}

TArray<FString> FSceneEditingManager::ModifyActorProperties(const FSceneEditAction& Action, UWorld* World)
{
	TArray<FString> ModifiedActorNames;

	if (!World)
		return ModifiedActorNames;

	TArray<AActor*> ActorsToModify = FindActorsByPattern(Action.SearchPattern, World);

	for (AActor* Actor : ActorsToModify)
	{
		if (Actor)
		{
			// Handle color changes for lights
			if (Action.PropertyName == TEXT("Color"))
			{
				if (ALight* Light = Cast<ALight>(Actor))
				{
					FLinearColor NewColor = FLinearColor::White;
					
					if (Action.PropertyValue == TEXT("Red"))
						NewColor = FLinearColor::Red;
					else if (Action.PropertyValue == TEXT("Blue"))
						NewColor = FLinearColor::Blue;
					else if (Action.PropertyValue == TEXT("Green"))
						NewColor = FLinearColor::Green;
					
					Light->SetLightColor(NewColor);
					ModifiedActorNames.Add(Actor->GetName());
				}
			}
		}
	}

	return ModifiedActorNames;
}

TArray<AActor*> FSceneEditingManager::FindActorsByPattern(const FString& Pattern, UWorld* World)
{
	TArray<AActor*> MatchingActors;

	if (!World)
		return MatchingActors;

	FString LowerPattern = Pattern.ToLower();

	// Search by actor type
	if (LowerPattern.Contains(TEXT("light")))
	{
		for (TActorIterator<ALight> It(World); It; ++It)
		{
			MatchingActors.Add(*It);
		}
	}
	else if (LowerPattern.Contains(TEXT("camera")))
	{
		for (TActorIterator<ACameraActor> It(World); It; ++It)
		{
			MatchingActors.Add(*It);
		}
	}
	else if (LowerPattern.Contains(TEXT("trigger")))
	{
		for (TActorIterator<ATriggerVolume> It(World); It; ++It)
		{
			MatchingActors.Add(*It);
		}
	}
	else if (LowerPattern.Contains(TEXT("prop")) || LowerPattern.Contains(TEXT("static mesh")))
	{
		for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
		{
			MatchingActors.Add(*It);
		}
	}
	else if (LowerPattern.Contains(TEXT("all")))
	{
		// Return all actors except critical ones
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			// Skip PlayerStart and other critical actors
			if (!Actor->IsA<APlayerStart>() && !Actor->GetName().Contains(TEXT("Brush")))
			{
				MatchingActors.Add(Actor);
			}
		}
	}

	return MatchingActors;
}

FVector FSceneEditingManager::FindPlayerStartLocation(UWorld* World)
{
	if (!World)
		return FVector::ZeroVector;

	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		return It->GetActorLocation();
	}

	// If no PlayerStart found, return origin
	return FVector::ZeroVector;
}

FString FSceneEditingManager::ParseActorType(const FString& Command)
{
	FString LowerCommand = Command.ToLower();

	if (LowerCommand.Contains(TEXT("point light")))
		return TEXT("point light");
	else if (LowerCommand.Contains(TEXT("spot light")))
		return TEXT("spot light");
	else if (LowerCommand.Contains(TEXT("directional light")))
		return TEXT("directional light");
	else if (LowerCommand.Contains(TEXT("light")))
		return TEXT("light");
	else if (LowerCommand.Contains(TEXT("camera")))
		return TEXT("camera");
	else if (LowerCommand.Contains(TEXT("trigger")))
		return TEXT("trigger");
	else if (LowerCommand.Contains(TEXT("prop")))
		return TEXT("prop");
	else if (LowerCommand.Contains(TEXT("static mesh")))
		return TEXT("static mesh");

	return TEXT("actor");
}

int32 FSceneEditingManager::ParseCount(const FString& Command)
{
	// Look for numbers in the command
	static const int32 MaxParsableCount = 100; // Maximum count we'll try to parse
	
	for (int32 i = 1; i <= MaxParsableCount; ++i)
	{
		if (Command.Contains(FString::FromInt(i)))
			return i;
	}

	return 1; // Default to 1 if no count specified
}

FVector FSceneEditingManager::ParseLocation(const FString& Command, UWorld* World)
{
	// For now, return zero vector - could be extended to parse coordinates
	return FVector::ZeroVector;
}

FVector FSceneEditingManager::ParseMovementOffset(const FString& Command)
{
	FString LowerCommand = Command.ToLower();
	FVector Offset = FVector::ZeroVector;

	// Maximum units we'll try to parse from natural language
	static const int32 MaxParsableUnits = 1000;
	static const float DefaultMovementOffset = 100.0f; // Default units when no number specified

	// Parse movement directions
	if (LowerCommand.Contains(TEXT("up")))
	{
		// Look for units
		for (int32 i = 1; i <= MaxParsableUnits; ++i)
		{
			if (Command.Contains(FString::FromInt(i)))
			{
				Offset.Z = i;
				break;
			}
		}
		if (Offset.Z == 0)
			Offset.Z = DefaultMovementOffset;
	}
	else if (LowerCommand.Contains(TEXT("down")))
	{
		for (int32 i = 1; i <= MaxParsableUnits; ++i)
		{
			if (Command.Contains(FString::FromInt(i)))
			{
				Offset.Z = -i;
				break;
			}
		}
		if (Offset.Z == 0)
			Offset.Z = -DefaultMovementOffset;
	}

	return Offset;
}
