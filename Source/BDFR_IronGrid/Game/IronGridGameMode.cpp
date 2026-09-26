#include "Game/IronGridGameMode.h"

#include "Player/IronGridPlayerController.h"
#include "Tank/IronGridTankPawn.h"
#include "UI/IronGridHUD.h"

AIronGridGameMode::AIronGridGameMode()
{
    DefaultPawnClass = AIronGridTankPawn::StaticClass();
    PlayerControllerClass = AIronGridPlayerController::StaticClass();
    HUDClass = AIronGridHUD::StaticClass();
}
