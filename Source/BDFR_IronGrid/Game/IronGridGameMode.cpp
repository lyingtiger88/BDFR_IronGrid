#include "Game/IronGridGameMode.h"

#include "Engine/Engine.h"
#include "Player/IronGridPlayerController.h"
#include "Tank/IronGridTankPawn.h"
#include "UI/IronGridHUD.h"

AIronGridGameMode::AIronGridGameMode()
{
    // Safe native fallback. Blueprint defaults are not available yet in the
    // native constructor, so TankPawnClassOverride is applied later in InitGame.
    DefaultPawnClass = AIronGridTankPawn::StaticClass();
    PlayerControllerClass = AIronGridPlayerController::StaticClass();
    HUDClass = AIronGridHUD::StaticClass();
}

void AIronGridGameMode::InitGame(
    const FString& MapName,
    const FString& Options,
    FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    bool bAppliedBlueprintPawn = false;

    if (!TankPawnClassOverride.IsNull())
    {
        if (UClass* LoadedPawnClass = TankPawnClassOverride.LoadSynchronous())
        {
            if (LoadedPawnClass->IsChildOf(AIronGridTankPawn::StaticClass()))
            {
                DefaultPawnClass = LoadedPawnClass;
                bAppliedBlueprintPawn = true;
            }
        }
    }

#if !(UE_BUILD_SHIPPING)
    if (GEngine)
    {
        const FString PawnName =
            DefaultPawnClass
                ? DefaultPawnClass->GetName()
                : TEXT("NONE");

        GEngine->AddOnScreenDebugMessage(
            -1,
            8.0f,
            bAppliedBlueprintPawn ? FColor::Green : FColor::Yellow,
            FString::Printf(
                TEXT("IRON GRID GAME MODE PAWN: %s%s"),
                *PawnName,
                bAppliedBlueprintPawn
                    ? TEXT("  [Blueprint override applied]")
                    : TEXT("  [native fallback]")));
    }
#endif
}
