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

    // Prefer the actual world surface below the cursor.
    // This is more reliable than assuming the battlefield is always at Z = 0.
    FHitResult CursorHit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, CursorHit) && CursorHit.bBlockingHit)
    {
        Tank->SetDesiredAimPoint(CursorHit.ImpactPoint);
        return;
    }

    // Fallback for empty space: project the cursor ray onto a flat aiming plane.
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
