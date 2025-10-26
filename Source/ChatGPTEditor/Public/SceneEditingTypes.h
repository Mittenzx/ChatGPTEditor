// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Types of scene editing operations
 * Note: Using plain C++ enum instead of UENUM as these are editor-only types
 * not exposed to Blueprint. If Blueprint integration is needed in the future,
 * convert to UENUM and add .generated.h support.
 */
enum class ESceneEditOperation : uint8
{
	SpawnActor,
	DeleteActor,
	MoveActor,
	ModifyProperty
};

/**
 * Represents a single scene editing action
 * Note: Using plain C++ struct as this is editor-only and not serialized to disk.
 * If persistence is needed, convert to USTRUCT with UPROPERTY macros.
 */
struct FSceneEditAction
{
	ESceneEditOperation Operation = ESceneEditOperation::SpawnActor;
	FString ActorClass;
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	FString PropertyName;
	FString PropertyValue;
	int32 Count = 1;
	FString SearchPattern;
	FString Description;
};

/**
 * Represents an audit log entry for scene editing operations
 * Note: Currently in-memory only. If persistent storage is needed,
 * convert to USTRUCT with UPROPERTY macros or implement custom serialization.
 */
struct FAuditLogEntry
{
	FDateTime Timestamp;
	FString UserCommand;
	FString OperationType;
	FString AffectedActors;
	bool bWasSuccessful = false;
	FString ErrorMessage;
};
