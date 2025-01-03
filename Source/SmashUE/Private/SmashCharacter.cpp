// Fill out your copyright notice in the Description page of Project Settings.


#include "SmashCharacter.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Characters/SmashCharacterStateID.h"


#include "Kismet/GameplayStatics.h"

// Sets default values
ASmashCharacter::ASmashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASmashCharacter::BeginPlay()
{
	Super::BeginPlay();
	CreateStateMachine();
	InitStateMachine();

	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);
}

// Called every frame
void ASmashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickStateMachine(DeltaTime);
	RotateMeshUsingOrientX();
	MoveCharacter(CharacterMovementVector, DeltaTime);
}

// Called to bind functionality to input
void ASmashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupMappingContextIntoController();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	BindInputMoveXAxisAndActions(EnhancedInputComponent);
}

float ASmashCharacter::GetOrientX() const
{
	return OrientX;
}

void ASmashCharacter::SetOrientX(float NewOrientX)
{
	OrientX = NewOrientX;
}

void ASmashCharacter::TickStateMachine(float DeltaTime) const
{
	if (StateMachine == nullptr) return;
	StateMachine->Tick(DeltaTime);
}

void ASmashCharacter::ChangeAnimation(UAnimMontage* AnimMontage)
{
	if (AnimMontage == nullptr) return;
	this->PlayAnimMontage(AnimMontage);
}

void ASmashCharacter::RotateMeshUsingOrientX() const
{
	FRotator Rotation = GetMesh()->GetRelativeRotation();
	Rotation.Yaw = -90.f * OrientX;
	GetMesh()->SetRelativeRotation(Rotation);
}

void ASmashCharacter::MoveCharacter(const FVector& InMovementVector, const float DeltaTime)
{
	//AddMovementInput(FVector::ForwardVector, OrientX);
}

void ASmashCharacter::CreateStateMachine()
{
	StateMachine = NewObject<USmashCharacterStateMachine>(this);
}

void ASmashCharacter::
InitStateMachine()
{
	if (StateMachine == nullptr) return;
	StateMachine->Init(this);
}

void ASmashCharacter::SetupMappingContextIntoController() const
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr) return;

	ULocalPlayer* Player = PlayerController->GetLocalPlayer();
	if (Player == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSystem == nullptr) return;
}

float ASmashCharacter::GetInputMoveX() const
{
	return InputMoveX;
}


void ASmashCharacter::BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if(InputData == nullptr) return;

	if(InputData->InputActionMoveX)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionMoveX, ETriggerEvent::Started, this, &ASmashCharacter::OnInputMoveX);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveX, ETriggerEvent::Triggered, this, &ASmashCharacter::OnInputMoveX);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveX, ETriggerEvent::Completed, this, &ASmashCharacter::OnInputMoveX);
	}

	if (InputData->InputActionMoveXFast)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXFast, ETriggerEvent::Triggered, this, &ASmashCharacter::OnInputMoveXFast);
	}

	if (InputData->InputActionJump)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionJump, ETriggerEvent::Started, this, &ASmashCharacter::OnInputJump);
		EnhancedInputComponent->BindAction(InputData->InputActionJump, ETriggerEvent::Completed, this, &ASmashCharacter::OnInputJumpReleased);
	}
}

void ASmashCharacter::OnInputMoveX(const FInputActionValue& InputActionValue)
{
	InputMoveX = InputActionValue.Get<float>();
}

void ASmashCharacter::OnInputJump(const FInputActionValue& InputActionValue)
{
	if (!HasJumped)
	{
	CharacterJump();
	HasJumped = true;
	}
}

void ASmashCharacter::OnInputJumpReleased(const FInputActionValue& InputActionValue)
{
	HasJumped = false;
}

FVector ASmashCharacter::GetFollowPosition()
{
	return GetActorLocation();
}

bool ASmashCharacter::IsFollowable()
{
	return true;
}

void ASmashCharacter::CharacterJump()
{
	if (NbOfJumps > 0)
	{
		NbOfJumps--;
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
		
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
	}
}

void ASmashCharacter::OnInputMoveXFast(const FInputActionValue& InputActionValue)
{
	InputMoveX = InputActionValue.Get<float>();
	InputMoveXFastEvent.Broadcast(InputMoveX);
}


