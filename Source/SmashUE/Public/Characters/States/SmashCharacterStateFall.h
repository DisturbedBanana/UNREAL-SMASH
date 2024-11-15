// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateFall.generated.h"


UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateFall : public USmashCharacterState
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USmashCharacterStateFall();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual ESmashCharacterStateID GetStateID() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StateInit(USmashCharacterStateMachine* InStateMachine) override;
	
	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;

	virtual void StateExit(ESmashCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Fall")
	float FallHorizontalMoveSpeed = 400;
	UPROPERTY(EditAnywhere, Category = "Fall")
	float FallAirControl = 1;
	UPROPERTY(EditAnywhere, Category = "Fall")
	float FallGravityScale = 3;
	UPROPERTY(EditAnywhere, Category = "Fall")
	float FallFaravityScale = 10;
};
