#include "LocalMultiplayerGameViewportClient.h"
#include "EnhancedInputSubsystems.h"
#include "LocalMultiplayerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "InputCoreTypes.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
    Super::PostInitProperties();
    
    MaxSplitscreenPlayers = 8;
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
    const ULocalMultiplayerSettings* MultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
    ULocalMultiplayerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();

    if (MultiplayerSettings == nullptr || Subsystem == nullptr) return false;

    int ProfileIndex = MultiplayerSettings->FindKeyBoardProfileIndexFromKey(EventArgs.Key, ELocalMultiplayerInputMappingType::InGame);
    int PlayerIndex = EventArgs.IsGamepad() ? Subsystem->GetAssignedPlayerIndexFromGamePadDeviceID(EventArgs.InputDevice.GetId())
                                            : Subsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(ProfileIndex);

    if (PlayerIndex < 0) return false;

    ULocalPlayer* Player = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);
    if (Player == nullptr) return false;

    APlayerController* PlayerController = Player->GetPlayerController(GetWorld());
    if (PlayerController == nullptr) return false;
    
    FInputKeyParams OutParams(EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, EventArgs.IsGamepad(), EventArgs.InputDevice);
    return PlayerController->InputKey(OutParams);
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta,
    float DeltaTime, int32 NumSamples, bool bGamePad)
{
    const ULocalMultiplayerSettings* MultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
    ULocalMultiplayerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
    if (MultiplayerSettings == nullptr || Subsystem == nullptr) return false;
    
    int PlayerIndex =Subsystem->GetAssignedPlayerIndexFromGamePadDeviceID(InputDevice.GetId());

    if (PlayerIndex < 0) return false;

    ULocalPlayer* Player = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);
    if (Player == nullptr) return false;

    APlayerController* PlayerController = Player->GetPlayerController(GetWorld());
    if (PlayerController == nullptr) return false;

    FInputKeyParams OutParams(Key, Delta, DeltaTime, NumSamples, bGamePad, InputDevice);
    return PlayerController->InputKey(OutParams);
}
