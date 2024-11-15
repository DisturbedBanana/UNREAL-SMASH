// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "EngineUtils.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
	CameraYPosition = CameraMain->GetComponentTransform().GetLocation().Y;
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(AActor* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(AActor* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if (FollowTargets.Num() == 0 || !CameraMain) return;

	FVector TargetPosition = CalculateAveragePositionBetweenTargets();

	// Smoothly interpolate the camera position toward the target position
	FVector CurrentPosition = CameraMain->GetComponentLocation();
	FVector NewPosition = FMath::VInterpTo(CurrentPosition, TargetPosition, DeltaTime, 3);

	CameraMain->SetWorldLocation(NewPosition);
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	if (FollowTargets.Num() == 0) return FVector::ZeroVector;

	FVector AveragePosition = FVector::ZeroVector;
	for (AActor* Target : FollowTargets)
	{
		if (Target)
		{
			AveragePosition += Target->GetActorLocation();
		}
	}

	AveragePosition /= FollowTargets.Num();
	AveragePosition.Y = CameraYPosition;
	return AveragePosition;
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag)
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->ActorHasTag(Tag))
		{
			UCameraComponent* CameraComponent = Actor->FindComponentByClass<UCameraComponent>();
			if (CameraComponent)
			{
				return CameraComponent;
			}
		}
	}
	return nullptr;
}
