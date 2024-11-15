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

	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red,"Enter StateIdle");
	Character->GetCharacterMovement()->MaxWalkSpeed = 0;

	Character->InputMoveXFastEvent.AddDynamic(this, &UUSmashCharacterIdleState::OnInputMoveXFast);
}

void UUSmashCharacterIdleState::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red,"Exit StateIdle");
	Character->InputMoveXFastEvent.RemoveDynamic(this, &UUSmashCharacterIdleState::OnInputMoveXFast);
}

void UUSmashCharacterIdleState::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (FMath::Abs(Character->GetInputMoveX()) > CharacterSettings->InputMoveXThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Walk);
	}

	if (Character->IsJumping)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
	}
}

void UUSmashCharacterIdleState::OnInputMoveXFast(float InputMoveX)
{
	StateMachine->ChangeState(ESmashCharacterStateID::Run);
}
