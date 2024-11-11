// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Inputs/SmashCharacterInputData.h"
#include "MatchGameMode.generated.h"

class AArenaPlayerStart;
class ASmashCharacter;

/**
 * 
 */
UCLASS()
class SMASHUE_API AMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
private:
	void FindPlayerStartActorsInArena(TArray<AArenaPlayerStart*>& ResultActors);

	TSubclassOf<ASmashCharacter> GetSmashCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const;

	void SpawnCharacters(const TArray<AArenaPlayerStart*>& SpawnPoints);

	USmashCharacterInputData* LoadInputDataFromConfig();

	UInputMappingContext* LoadInputMappingContextFromConfig();
	
protected:
	UPROPERTY(VisibleAnywhere)
	TArray<ASmashCharacter*> CharactersInsideArena;
};
