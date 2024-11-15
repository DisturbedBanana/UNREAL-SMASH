// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterState.h"
#include "SmashCharacterStateJump.generated.h"


UCLASS(ClassGroup=(SmashCharacterState), meta=(BlueprintSpawnableComponent))
class SMASHUE_API USmashCharacterStateJump : public USmashCharacterState
{
	GENERATED_BODY()

private:
	void CalculateJumpVariables();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Sets default values for this component's properties
	USmashCharacterStateJump();
	
	// Called every frame
	virtual ESmashCharacterStateID GetStateID() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StateInit(USmashCharacterStateMachine* InStateMachine) override;
	
	virtual void StateEnter(ESmashCharacterStateID PreviousStateID) override;

	virtual void StateExit(ESmashCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Jump")
	float JumpWalkSpeed = 400;
	UPROPERTY(EditAnywhere, Category = "Jump")
	float JumpMaxHeight = 280;
	UPROPERTY(EditAnywhere, Category = "Jump")
	float JumpDuration = 1;
	UPROPERTY(EditAnywhere, Category = "Jump")
	float JumpAirControl = 1;
};
