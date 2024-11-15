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
	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Blue,FString::SanitizeFloat(Character->GetCharacterMovement()->GravityScale));
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
	CalculateJumpVariables();
	Character->Jump();
	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red,"Enter StateJump");
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	Character->CharacterMovementVector = Character->GetActorForwardVector();
	Character->GetCharacterMovement()->MaxWalkSpeed = 125;

	if (Character->GetCharacterMovement()->IsFalling())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
	}
}