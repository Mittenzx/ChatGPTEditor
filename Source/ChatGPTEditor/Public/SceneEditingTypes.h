// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneEditingTypes.generated.h"

/**
 * Types of scene editing operations
 */
UENUM(BlueprintType)
enum class ESceneEditOperation : uint8
{
	SpawnActor UMETA(DisplayName = "Spawn Actor"),
	DeleteActor UMETA(DisplayName = "Delete Actor"),
	MoveActor UMETA(DisplayName = "Move Actor"),
	ModifyProperty UMETA(DisplayName = "Modify Property")
};

/**
 * Represents a single scene editing action
 */
USTRUCT(BlueprintType)
struct FSceneEditAction
{
	GENERATED_BODY()

	UPROPERTY()
	ESceneEditOperation Operation = ESceneEditOperation::SpawnActor;

	UPROPERTY()
	FString ActorClass;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY()
	FString PropertyName;

	UPROPERTY()
	FString PropertyValue;

	UPROPERTY()
	int32 Count = 1;

	UPROPERTY()
	FString SearchPattern;

	UPROPERTY()
	FString Description;
};

/**
 * Represents an audit log entry for scene editing operations
 */
USTRUCT(BlueprintType)
struct FAuditLogEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FDateTime Timestamp;

	UPROPERTY()
	FString UserCommand;

	UPROPERTY()
	FString OperationType;

	UPROPERTY()
	FString AffectedActors;

	UPROPERTY()
	bool bWasSuccessful = false;

	UPROPERTY()
	FString ErrorMessage;
};
