// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/USmashCharacterIdleState.h"

#include "SmashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ESmashCharacterStateID UUSmashCharacterIdleState::GetStateID()
{
	return ESmashCharacterStateID::Idle;
}


void UUSmashCharacterIdleState::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	Character->GetCharacterMovement()->MaxWalkSpeed = 0;
	//UE_LOG(LogTemp, Warning, TEXT("IDLE"));
	Character->InputMoveXFastEvent.AddDynamic(this, &UUSmashCharacterIdleState::OnInputMoveXFast);
}

void UUSmashCharacterIdleState::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	
	Character->InputMoveXFastEvent.RemoveDynamic(this, &UUSmashCharacterIdleState::OnInputMoveXFast);
}

void UUSmashCharacterIdleState::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Walk);
	}
}

void UUSmashCharacterIdleState::OnInputMoveXFast(float InputMoveX)
{
	StateMachine->ChangeState(ESmashCharacterStateID::Run);
}
