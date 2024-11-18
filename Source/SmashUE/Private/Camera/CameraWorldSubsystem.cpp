// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "EngineUtils.h"
#include "Camera/CameraFollowTarget.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
	CameraYPosition = CameraMain->GetComponentTransform().GetLocation().Y;

	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if (CameraBoundsActor != nullptr)
	{
		InitCameraBounds(CameraBoundsActor);
	}	
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->ActorHasTag("CameraBounds"))
		{
			return Actor;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString("No Camera Bounds Found"));
	return nullptr;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	FVector BoundsCenter;
	FVector BoundsExtent;
	CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtent);

	// Calculate CameraBoundsMin and CameraBoundsMax
	CameraBoundsMin = FVector(BoundsCenter.X - BoundsExtent.X, CameraYPosition, BoundsCenter.Z - BoundsExtent.Z);
	CameraBoundsMax = FVector(BoundsCenter.X + BoundsExtent.X, CameraYPosition, BoundsCenter.Z + BoundsExtent.Z);

	// Set CameraYProjectionCenter
	CameraBoundsYProjectionCenter = BoundsCenter.Y;

	// Debug output
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Camera Bounds Min: %s"), *CameraBoundsMin.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Camera Bounds Max: %s"), *CameraBoundsMax.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Camera Y Projection Center: %f"), CameraBoundsYProjectionCenter));
}


void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin, ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);

	FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	FVector WorldBoundsOffset = FVector(WorldBoundsMax - WorldBoundsMin);

	Position.X = FMath::Clamp(Position.X, CameraBoundsMin.X + FMath::Abs(WorldBoundsOffset.X/2) ,CameraBoundsMax.X - FMath::Abs(WorldBoundsOffset.X/2));
	Position.Y = CameraYPosition; 
	Position.Z = FMath::Clamp(Position.Z, CameraBoundsMin.Z + FMath::Abs(WorldBoundsOffset.Z/2),CameraBoundsMax.Z - FMath::Abs(WorldBoundsOffset.X/2));
}


void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient == nullptr) return;

	FViewport* Viewport = ViewportClient->Viewport;
	if (Viewport == nullptr) return;

	FIntRect ViewRect(Viewport->GetInitialPositionXY(), Viewport->GetInitialPositionXY() + Viewport->GetSizeXY());
	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;

	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(FVector2D& ViewportPosition)
{
	if(CameraMain == nullptr)	return FVector::ZeroVector;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController == nullptr) return FVector::ZeroVector;

	float YDistanceToCenter = CameraMain->GetOwner()->GetActorLocation().Y - CameraBoundsYProjectionCenter;

	FVector CameraWorldProjectDir;
	FVector WorldPosition;
	UGameplayStatics::DeprojectScreenToWorld(PlayerController,ViewportPosition, WorldPosition, CameraWorldProjectDir);

	WorldPosition += CameraWorldProjectDir * YDistanceToCenter;

	return WorldPosition;
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
	for (UObject* Target : FollowTargets)
	{
		if (Target->GetClass()->ImplementsInterface(UCameraFollowTarget::StaticClass()))
		{
			TScriptInterface<ICameraFollowTarget> CameraFollowTarget;
			CameraFollowTarget.SetObject(Target);
			CameraFollowTarget.SetInterface(Cast<ICameraFollowTarget>(Target));
			
			if (CameraFollowTarget->IsFollowable())
			{
				AveragePosition += CameraFollowTarget->GetFollowPosition();
			}
		}
	}

	AveragePosition /= FollowTargets.Num();
	AveragePosition.Y = CameraYPosition;
	ClampPositionIntoCameraBounds(AveragePosition);
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
