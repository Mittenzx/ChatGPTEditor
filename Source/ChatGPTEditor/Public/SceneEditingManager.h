// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneEditingTypes.h"

class UWorld;

/**
 * Scene editing manager - handles all level design and actor manipulation operations
 * Provides safe, audited scene editing with preview and confirmation
 */
class FSceneEditingManager
{
public:
	/** Get singleton instance */
	static FSceneEditingManager& Get();

	/** Parse natural language command into scene editing actions */
	TArray<FSceneEditAction> ParseCommand(const FString& Command);

	/** Execute scene editing actions (with preview and confirmation) */
	bool ExecuteActions(const TArray<FSceneEditAction>& Actions, UWorld* World, bool bPreviewOnly = false);

	/** Spawn actors based on action */
	TArray<AActor*> SpawnActors(const FSceneEditAction& Action, UWorld* World);

	/** Delete actors based on action */
	TArray<FString> DeleteActors(const FSceneEditAction& Action, UWorld* World);

	/** Move actors based on action */
	TArray<FString> MoveActors(const FSceneEditAction& Action, UWorld* World);

	/** Modify actor properties based on action */
	TArray<FString> ModifyActorProperties(const FSceneEditAction& Action, UWorld* World);

	/** Get actors matching a search pattern */
	TArray<AActor*> FindActorsByPattern(const FString& Pattern, UWorld* World);

	/** Find PlayerStart location in the world */
	FVector FindPlayerStartLocation(UWorld* World);

private:
	FSceneEditingManager() = default;

	/** Helper to parse actor type from command */
	FString ParseActorType(const FString& Command);

	/** Helper to parse count from command */
	int32 ParseCount(const FString& Command);

	/** Helper to parse location from command */
	FVector ParseLocation(const FString& Command, UWorld* World);

	/** Helper to parse movement offset from command */
	FVector ParseMovementOffset(const FString& Command);
};
