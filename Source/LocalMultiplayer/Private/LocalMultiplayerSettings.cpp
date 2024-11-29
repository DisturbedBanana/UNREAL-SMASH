// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSettings.h"

#include "../../../../../../../../Program Files/Epic Games/UE_5.3/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"

struct FEnhancedActionKeyMapping;

bool FLocalMultiplayerProfileData::ContainsKey(const FKey& Key,
                                               ELocalMultiplayerInputMappingType MappingType) const
{
	for (FEnhancedActionKeyMapping Element : GetIMCFromType(MappingType)->GetMappings())
	{
		if (Element.Key == Key) return true;
	}
	return false;
}

UInputMappingContext* FLocalMultiplayerProfileData::GetIMCFromType(
	ELocalMultiplayerInputMappingType MappingType) const
{
	switch (MappingType)
	{
	case ELocalMultiplayerInputMappingType::Menu:
		return IMCMenu;
	case ELocalMultiplayerInputMappingType::InGame:
		return IMCInGame;
	default:
		return nullptr;
	}
}

int ULocalMultiplayerSettings::FindKeyBoardProfileIndexFromKey(const FKey& Key,
	ELocalMultiplayerInputMappingType MappingType) const
{
	for (int i = 0; i < KeyboardProfilesData.Num(); i++)
	{
		if (KeyboardProfilesData[i].ContainsKey(Key, MappingType))
		{
			return i;
		}
	}
	return -1;
}
