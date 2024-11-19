// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Smash Camera Settings"))
class SMASHUE_API UCameraSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = Camera)
	FName CameraMainTag;

	UPROPERTY(Config, EditAnywhere, Category = Camera)
	FName CameraDistanceMinTag;

	UPROPERTY(Config, EditAnywhere, Category = Camera)
	FName CameraDistanceMaxTag;

	UPROPERTY(Config, EditAnywhere, Category = Camera)
	FName CameraBoundsTag;

	UPROPERTY(Config, EditAnywhere, Category = Camera)
	float DistanceBetweenTargetsMin;

	UPROPERTY(Config, EditAnywhere, Category = Camera)
	float DistanceBetweenTargetsMax;
};
