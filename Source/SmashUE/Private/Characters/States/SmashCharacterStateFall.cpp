﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateFall.h"

#include "SmashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
USmashCharacterStateFall::USmashCharacterStateFall()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USmashCharacterStateFall::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

ESmashCharacterStateID USmashCharacterStateFall::GetStateID()
{
	return ESmashCharacterStateID::Fall;
}


// Called every frame
void USmashCharacterStateFall::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USmashCharacterStateFall::StateInit(USmashCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
}

void USmashCharacterStateFall::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = FallHorizontalMoveSpeed;
	Character->GetCharacterMovement()->AirControl = FallAirControl;
}

void USmashCharacterStateFall::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	if (NextStateID == ESmashCharacterStateID::Idle)
	{
	}
		Character->NbOfJumps = 2;
}

void USmashCharacterStateFall::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
	
	if (Character->GetCharacterMovement()->Velocity.Z == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RESET JUMPS"));
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
	}

	Character->SetOrientX(Character->GetInputMoveX());
	Character->AddMovementInput(FVector::ForwardVector * FallAirControl, Character->GetOrientX());
}

