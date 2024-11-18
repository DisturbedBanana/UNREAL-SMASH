// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SMASHUE_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Subsystem Overrides
	
public:
	virtual void PostInitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override {return TStatId();};
#pragma endregion

#pragma region Main Camera
	
protected:
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;

	void TickUpdateCameraPosition(float DeltaTime);

	FVector CalculateAveragePositionBetweenTargets();

	UCameraComponent* FindCameraByTag(const FName& Tag);
	
#pragma endregion

#pragma region Follow Targets
	
public:
	
	void AddFollowTarget(UObject* FollowTarget);

	void RemoveFollowTarget(UObject* FollowTarget);

protected:
	
	UPROPERTY()
	TArray<UObject*> FollowTargets;
	
#pragma endregion

#pragma region Misc
	
	float CameraYPosition;

#pragma endregion

#pragma region Bounds
protected:
	UPROPERTY()
	FVector CameraBoundsMin;

	UPROPERTY()
	FVector CameraBoundsMax;

	UPROPERTY()
	float CameraBoundsYProjectionCenter;

	AActor* FindCameraBoundsActor();

	void InitCameraBounds(AActor* CameraBoundsActor);

	void ClampPositionIntoCameraBounds(FVector& Position);

	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);

	FVector CalculateWorldPositionFromViewportPosition(FVector2D& ViewportPosition);
	
#pragma endregion
};
