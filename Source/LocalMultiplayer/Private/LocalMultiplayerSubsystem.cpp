#include "LocalMultiplayerSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
    UE_LOG(LogTemp, Warning, TEXT("MultiplayerSettings->GetNbKeyboardProfiles() -> %d"), MultiplayerSettings->GetNbKeyboardProfiles());
    // Keyboards
    int CurrentPlayerIndex = 0;
    
    for (int i = 0; i < MultiplayerSettings->GetNbKeyboardProfiles(); ++i)
    {
        UGameplayStatics::CreatePlayer(this, CurrentPlayerIndex, true);
        CurrentPlayerIndex++;
    }

    //Gamepads
    for (int DeviceID = 0; DeviceID < MultiplayerSettings->NbMaxGamepads; ++DeviceID)
    {
        UGameplayStatics::CreatePlayer(this, CurrentPlayerIndex, true);
        CurrentPlayerIndex++;
    }

    UE_LOG(LogTemp, Warning, TEXT("GetGameInstance()->GetNumLocalPlayers -> %d"), GetGameInstance()->GetNumLocalPlayers());
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex)
{
    const int* PlayerIndex = PlayerIndexFromKeyboardProfileIndex.Find(KeyboardProfileIndex);
    return PlayerIndex ? *PlayerIndex : -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex)
{
    int PlayerIndex = LastAssignedPlayerIndex++;
    AssignKeyboardMapping(PlayerIndex, KeyboardProfileIndex, ELocalMultiplayerInputMappingType::InGame);
    PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, PlayerIndex);
    return PlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex,
    ELocalMultiplayerInputMappingType MappingType)
{
    ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem) return;

    const FLocalMultiplayerProfileData& Profile = MultiplayerSettings->KeyboardProfilesData[KeyboardProfileIndex];
    UInputMappingContext* MappingContext = Profile.GetIMCFromType(MappingType);
    UE_LOG(LogTemp, Warning, TEXT("PlayerIndex -> %s"), *Profile.ProfileName.ToString());
    if (MappingContext != nullptr)
    {
        InputSubsystem->AddMappingContext(MappingContext, 1);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NO MAPPING CONTEXT"));
    }
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamePadDeviceID(int DeviceID)
{
    const int* PlayerIndex = PlayerIndexFromGamepadProfileIndex.Find(DeviceID);
    return PlayerIndex ? *PlayerIndex : -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamePadDeviceID(int DeviceID)
{
    if(PlayerIndexFromGamepadProfileIndex.Contains(DeviceID))
        return PlayerIndexFromGamepadProfileIndex[DeviceID];
    
    const int PlayerIndex = LastAssignedPlayerIndex++;
    PlayerIndexFromGamepadProfileIndex.Add(DeviceID, PlayerIndex);
    return PlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex,
    ELocalMultiplayerInputMappingType MappingType) const
{
    ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem) return;

    const FLocalMultiplayerProfileData& GamepadProfile = MultiplayerSettings->GamepadProfileData;
    UInputMappingContext* MappingContext = GamepadProfile.GetIMCFromType(MappingType);
    if (MappingContext)
    {
        InputSubsystem->AddMappingContext(MappingContext, 1);
    }
}

void ULocalMultiplayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    MultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
}
