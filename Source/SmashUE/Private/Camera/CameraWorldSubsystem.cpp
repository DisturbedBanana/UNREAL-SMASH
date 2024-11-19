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
	CameraSettings = GetDefault<UCameraSettings>();
	CameraMain = FindCameraByTag();
	CameraYPosition = CameraMain->GetComponentTransform().GetLocation().Y;

	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if (CameraBoundsActor != nullptr)
	{
		InitCameraBounds(CameraBoundsActor);
	}

	InitCameraZoomParameters();
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraZoom(DeltaTime);
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
		if (Actor->ActorHasTag(CameraSettings->CameraBoundsTag))
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
	
	CameraBoundsMin = FVector(BoundsCenter.X - BoundsExtent.X, CameraYPosition, BoundsCenter.Z - BoundsExtent.Z);
	CameraBoundsMax = FVector(BoundsCenter.X + BoundsExtent.X, CameraYPosition, BoundsCenter.Z + BoundsExtent.Z);
	
	CameraBoundsYProjectionCenter = BoundsCenter.Y;
	
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

float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets()
{
	float GreatestDistance = 0.f;
	
	for (int32 i = 0; i < FollowTargets.Num(); ++i)
	{
		UObject* TargetA = FollowTargets[i];
		if (!TargetA || !TargetA->GetClass()->ImplementsInterface(UCameraFollowTarget::StaticClass())) continue;

		TScriptInterface<ICameraFollowTarget> CameraFollowTargetA;
		CameraFollowTargetA.SetObject(TargetA);
		CameraFollowTargetA.SetInterface(Cast<ICameraFollowTarget>(TargetA));

		if (!CameraFollowTargetA->IsFollowable()) continue;

		FVector PositionA = CameraFollowTargetA->GetFollowPosition();

		for (int32 j = i + 1; j < FollowTargets.Num(); ++j)
		{
			UObject* TargetB = FollowTargets[j];
			if (!TargetB || !TargetB->GetClass()->ImplementsInterface(UCameraFollowTarget::StaticClass())) continue;

			TScriptInterface<ICameraFollowTarget> CameraFollowTargetB;
			CameraFollowTargetB.SetObject(TargetB);
			CameraFollowTargetB.SetInterface(Cast<ICameraFollowTarget>(TargetB));

			if (!CameraFollowTargetB->IsFollowable()) continue;

			FVector PositionB = CameraFollowTargetB->GetFollowPosition();
			
			float Distance = FVector::Dist(PositionA, PositionB);
			
			if (Distance > GreatestDistance)
			{
				GreatestDistance = Distance;
			}
		}
	}

	return GreatestDistance;
}


void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	if (FollowTargets.Num() == 0 || !CameraMain) return;

	// Calculate the average position of the targets
	FVector TargetPosition = CalculateAveragePositionBetweenTargets();

	// Smoothly interpolate the camera position toward the target position using PositionDampingFactor
	FVector CurrentPosition = CameraMain->GetComponentLocation();
	float InterpolationFactor = FMath::Clamp(CameraSettings->PositionDampingFactor * DeltaTime, 0.f, 1.f);
	FVector NewPosition = FMath::Lerp(CurrentPosition, TargetPosition, InterpolationFactor);

	CameraMain->SetWorldLocation(NewPosition);
}



void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if (CameraMain == nullptr) return;

	// Calculate the greatest distance between targets
	float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

	// Calculate the zoom percent (inverse lerp logic)
	float ZoomPercent = 1.f - (GreatestDistanceBetweenTargets - CameraSettings->DistanceBetweenTargetsMin) /
						(CameraSettings->DistanceBetweenTargetsMax - CameraSettings->DistanceBetweenTargetsMin);
	ZoomPercent = FMath::Clamp(ZoomPercent, 0.f, 1.f);

	// Target Y position based on zoom percent
	float TargetYPosition = FMath::Lerp(CameraZoomYMin, CameraZoomYMax, ZoomPercent);

	// Smoothly interpolate the Y position of the camera using SizeDampingFactor
	FVector CurrentPosition = CameraMain->GetComponentLocation();
	float InterpolationFactor = FMath::Clamp(CameraSettings->SizeDampingFactor * DeltaTime, 0.f, 1.f);
	CurrentPosition.Y = FMath::Lerp(CurrentPosition.Y, TargetYPosition, InterpolationFactor);

	CameraMain->SetWorldLocation(CurrentPosition);
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

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->ActorHasTag(CameraSettings->CameraMainTag))
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

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		if (Actor->ActorHasTag(CameraSettings->CameraDistanceMinTag))
		{
			UCameraComponent* CameraComponent = Actor->FindComponentByClass<UCameraComponent>();
			if (CameraComponent)
			{
				FVector CameraLocation = CameraComponent->GetComponentLocation();
				CameraZoomYMin = CameraLocation.Y; 
			}
		}
		else if (Actor->ActorHasTag(CameraSettings->CameraDistanceMaxTag))
		{
			UCameraComponent* CameraComponent = Actor->FindComponentByClass<UCameraComponent>();
			if (CameraComponent)
			{
				FVector CameraLocation = CameraComponent->GetComponentLocation();
				CameraZoomYMax = CameraLocation.Y; 
			}
		}
	}
}

