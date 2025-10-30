// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpawnActorTool.h"
#include "Editor.h"
#include "Engine/World.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/DirectionalLight.h"
#include "Camera/CameraActor.h"

FSpawnActorTool::FSpawnActorTool()
	: FMCPToolBase(TEXT("spawn_actor"), TEXT("Spawn one or more actors in the active Unreal Engine level"))
{
}

TSharedPtr<FJsonObject> FSpawnActorTool::GetInputSchema() const
{
	TSharedPtr<FJsonObject> Schema = MakeShared<FJsonObject>();
	Schema->SetStringField(TEXT("type"), TEXT("object"));
	
	TSharedPtr<FJsonObject> Properties = MakeShared<FJsonObject>();
	
	// actorClass property
	TSharedPtr<FJsonObject> ActorClassProp = MakeShared<FJsonObject>();
	ActorClassProp->SetStringField(TEXT("type"), TEXT("string"));
	ActorClassProp->SetStringField(TEXT("description"), TEXT("Type of actor to spawn (PointLight, SpotLight, Camera, etc.)"));
	Properties->SetObjectField(TEXT("actorClass"), ActorClassProp);
	
	// count property
	TSharedPtr<FJsonObject> CountProp = MakeShared<FJsonObject>();
	CountProp->SetStringField(TEXT("type"), TEXT("integer"));
	CountProp->SetNumberField(TEXT("minimum"), 1);
	CountProp->SetNumberField(TEXT("maximum"), 100);
	CountProp->SetNumberField(TEXT("default"), 1);
	CountProp->SetStringField(TEXT("description"), TEXT("Number of actors to spawn"));
	Properties->SetObjectField(TEXT("count"), CountProp);
	
	// location property (optional)
	TSharedPtr<FJsonObject> LocationProp = MakeShared<FJsonObject>();
	LocationProp->SetStringField(TEXT("type"), TEXT("object"));
	LocationProp->SetStringField(TEXT("description"), TEXT("Spawn location (optional, defaults to origin)"));
	
	TSharedPtr<FJsonObject> LocationProperties = MakeShared<FJsonObject>();
	
	TSharedPtr<FJsonObject> XProp = MakeShared<FJsonObject>();
	XProp->SetStringField(TEXT("type"), TEXT("number"));
	LocationProperties->SetObjectField(TEXT("x"), XProp);
	
	TSharedPtr<FJsonObject> YProp = MakeShared<FJsonObject>();
	YProp->SetStringField(TEXT("type"), TEXT("number"));
	LocationProperties->SetObjectField(TEXT("y"), YProp);
	
	TSharedPtr<FJsonObject> ZProp = MakeShared<FJsonObject>();
	ZProp->SetStringField(TEXT("type"), TEXT("number"));
	LocationProperties->SetObjectField(TEXT("z"), ZProp);
	
	LocationProp->SetObjectField(TEXT("properties"), LocationProperties);
	Properties->SetObjectField(TEXT("location"), LocationProp);
	
	Schema->SetObjectField(TEXT("properties"), Properties);
	
	// Required fields
	TArray<TSharedPtr<FJsonValue>> Required;
	Required.Add(MakeShared<FJsonValueString>(TEXT("actorClass")));
	Required.Add(MakeShared<FJsonValueString>(TEXT("count")));
	Schema->SetArrayField(TEXT("required"), Required);
	
	return Schema;
}

TSharedPtr<FJsonObject> FSpawnActorTool::Execute(const TSharedPtr<FJsonObject>& Arguments)
{
	// Extract parameters
	if (!Arguments.IsValid() || !Arguments->HasField(TEXT("actorClass")) || !Arguments->HasField(TEXT("count")))
	{
		return CreateErrorResponse(TEXT("Missing required parameters: actorClass and count"));
	}
	
	FString ActorClass = Arguments->GetStringField(TEXT("actorClass"));
	int32 Count = static_cast<int32>(Arguments->GetNumberField(TEXT("count")));
	
	// Validate count
	if (Count < 1 || Count > 100)
	{
		return CreateErrorResponse(TEXT("Count must be between 1 and 100"));
	}
	
	// Get location if provided
	FVector SpawnLocation = FVector::ZeroVector;
	if (Arguments->HasField(TEXT("location")))
	{
		const TSharedPtr<FJsonObject>& LocObj = Arguments->GetObjectField(TEXT("location"));
		SpawnLocation.X = LocObj->GetNumberField(TEXT("x"));
		SpawnLocation.Y = LocObj->GetNumberField(TEXT("y"));
		SpawnLocation.Z = LocObj->GetNumberField(TEXT("z"));
	}
	
	// Get world
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		return CreateErrorResponse(TEXT("No active world found. Please open a level."));
	}
	
	// Determine actor class to spawn
	UClass* ClassToSpawn = nullptr;
	
	if (ActorClass.Contains(TEXT("PointLight"), ESearchCase::IgnoreCase))
	{
		ClassToSpawn = APointLight::StaticClass();
	}
	else if (ActorClass.Contains(TEXT("SpotLight"), ESearchCase::IgnoreCase))
	{
		ClassToSpawn = ASpotLight::StaticClass();
	}
	else if (ActorClass.Contains(TEXT("DirectionalLight"), ESearchCase::IgnoreCase))
	{
		ClassToSpawn = ADirectionalLight::StaticClass();
	}
	else if (ActorClass.Contains(TEXT("Camera"), ESearchCase::IgnoreCase))
	{
		ClassToSpawn = ACameraActor::StaticClass();
	}
	else
	{
		return CreateErrorResponse(FString::Printf(TEXT("Unsupported actor class: %s"), *ActorClass));
	}
	
	// Spawn actors
	TArray<FString> SpawnedActorNames;
	for (int32 i = 0; i < Count; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = MakeUniqueObjectName(World, ClassToSpawn);
		
		FVector ActualLocation = SpawnLocation + FVector(i * 100.0f, 0, 0);
		AActor* NewActor = World->SpawnActor<AActor>(ClassToSpawn, ActualLocation, FRotator::ZeroRotator, SpawnParams);
		
		if (NewActor)
		{
			SpawnedActorNames.Add(NewActor->GetName());
		}
	}
	
	// Create response
	FString ResponseMessage = FString::Printf(TEXT("Successfully spawned %d %s actor(s): %s"),
		SpawnedActorNames.Num(), *ActorClass, *FString::Join(SpawnedActorNames, TEXT(", ")));
	
	return CreateSuccessResponse(ResponseMessage);
}

TArray<FString> FSpawnActorTool::GetRequiredPermissions() const
{
	TArray<FString> Permissions;
	Permissions.Add(TEXT("scene_editing"));
	return Permissions;
}
