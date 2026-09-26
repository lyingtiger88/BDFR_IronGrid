#include "Player/IronGridPlayerController.h"

#include "Tank/IronGridTankPawn.h"

AIronGridPlayerController::AIronGridPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AIronGridPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}

void AIronGridPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    AIronGridTankPawn* Tank = Cast<AIronGridTankPawn>(GetPawn());
    if (!Tank)
    {
        return;
    }

    FVector WorldOrigin;
    FVector WorldDirection;
    if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
    {
        return;
    }

    if (FMath::IsNearlyZero(WorldDirection.Z))
    {
        return;
    }

    const float T = (AimPlaneZ - WorldOrigin.Z) / WorldDirection.Z;
    if (T <= 0.0f)
    {
        return;
    }

    Tank->SetDesiredAimPoint(WorldOrigin + WorldDirection * T);
}
