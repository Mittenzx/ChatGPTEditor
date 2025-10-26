// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Types of scene editing operations
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
