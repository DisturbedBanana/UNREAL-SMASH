// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "Inputs/SmashCharacterInputData.h"
#include "GameFramework/Character.h"
#include "SmashCharacter.generated.h"

UCLASS()
class SMASHUE_API ASmashCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Default
public:
	// Sets default values for this character's properties
	ASmashCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion

#pragma region Orient
public:
	float GetOrientX() const;
	void SetOrientX(float NewOrientX);
	void TickStateMachine(float DeltaTime) const;

	void ChangeAnimation(UAnimMontage* AnimMontage);

protected:
	UPROPERTY(BlueprintReadOnly)
	float OrientX = 1.f;

	void RotateMeshUsingOrientX() const;
#pragma endregion

#pragma region Movement
public:
	FVector CharacterMovementVector = FVector::Zero();

	UPROPERTY(EditAnywhere)
	float MoveSpeedMax = 250.f;
	
protected:
	UFUNCTION()
	void MoveCharacter(const FVector& InMovementVector,const float DeltaTime);
#pragma endregion

#pragma region State Machine
public:
	void CreateStateMachine();
	void InitStateMachine();

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USmashCharacterStateMachine> StateMachine;
	
#pragma endregion

#pragma region Input Data / Mapping Context

public:
	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY()
	TObjectPtr<USmashCharacterInputData> InputData;

protected:
	void SetupMappingContextIntoController() const;

#pragma endregion

#pragma region Input Move X

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputMoveXEvent, float, InputMoveX);

public:
	float GetInputMoveX() const;

	UPROPERTY()
	FInputMoveXEvent InputMoveXFastEvent;
	bool IsJumping = false;
	
protected:
	UPROPERTY()
	float InputMoveX = 0.0f;


private:
	void OnInputMoveXFast(const FInputActionValue& InputActionValue);
	void BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputMoveX(const FInputActionValue& InputActionValue);
	void OnInputJump(const FInputActionValue& InputActionValue);

#pragma endregion
};
