// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateJump.h"

#include "SmashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
USmashCharacterStateJump::USmashCharacterStateJump()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

ESmashCharacterStateID USmashCharacterStateJump::GetStateID()
{
	return ESmashCharacterStateID::Jump;
}


void USmashCharacterStateJump::CalculateJumpVariables()
{
	Character->GetCharacterMovement()->JumpZVelocity = (2*JumpMaxHeight) / (JumpDuration/2);
	Character->GetCharacterMovement()->AirControl = JumpAirControl;
	Character->GetCharacterMovement()->GravityScale = 2*JumpMaxHeight / (JumpDuration * JumpMaxHeight);
	
	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Blue,FString::SanitizeFloat(Character->GetCharacterMovement()->AirControl));
}

// Called when the game starts
void USmashCharacterStateJump::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USmashCharacterStateJump::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USmashCharacterStateJump::StateInit(USmashCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
}

void USmashCharacterStateJump::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	if (Character->NbOfJumps > 0)
	{
	CalculateJumpVariables();
	Character->Jump();
	Character->GetCharacterMovement()->MaxWalkSpeed = JumpWalkSpeed;
	}
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	Character->MoveSpeedMax = 0;
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	
	if (Character->GetCharacterMovement()->Velocity.Z < 0)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
	}

	Character->SetOrientX(Character->GetInputMoveX());
	Character->AddMovementInput(FVector::ForwardVector, Character->GetOrientX());
}