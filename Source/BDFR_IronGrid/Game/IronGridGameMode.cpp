#include "Game/IronGridGameMode.h"

#include "Player/IronGridPlayerController.h"
#include "Tank/IronGridTankPawn.h"
#include "UI/IronGridHUD.h"

AIronGridGameMode::AIronGridGameMode()
{
    DefaultPawnClass = AIronGridTankPawn::StaticClass();

    if (!TankPawnClassOverride.IsNull())
    {
        if (UClass* LoadedPawnClass = TankPawnClassOverride.LoadSynchronous())
        {
            if (LoadedPawnClass->IsChildOf(AIronGridTankPawn::StaticClass()))
            {
                DefaultPawnClass = LoadedPawnClass;
            }
        }
    }

    PlayerControllerClass = AIronGridPlayerController::StaticClass();
    HUDClass = AIronGridHUD::StaticClass();
}
