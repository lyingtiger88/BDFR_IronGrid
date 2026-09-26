#include "Player/IronGridPlayerController.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Tank/IronGridTankPawn.h"
#include "UI/IronGridPauseMenu.h"

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

void AIronGridPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        FInputActionBinding& PauseBinding =
            InputComponent->BindAction(
                TEXT("TogglePauseMenu"),
                IE_Pressed,
                this,
                &AIronGridPlayerController::TogglePauseMenu);

        PauseBinding.bExecuteWhenPaused = true;
    }
}

AIronGridTankPawn* AIronGridPlayerController::GetIronGridTank() const
{
    return Cast<AIronGridTankPawn>(GetPawn());
}

void AIronGridPlayerController::TogglePauseMenu()
{
    if (bPauseMenuOpen)
    {
        ClosePauseMenu();
    }
    else
    {
        OpenPauseMenu();
    }
}

void AIronGridPlayerController::OpenPauseMenu()
{
    if (bPauseMenuOpen || !IsLocalController())
    {
        return;
    }

    if (!PauseMenuWidget.IsValid())
    {
        PauseMenuWidget =
            SNew(SIronGridPauseMenu)
            .OwnerController(this);
    }

    if (GEngine && GEngine->GameViewport && PauseMenuWidget.IsValid())
    {
        GEngine->GameViewport->AddViewportWidgetContent(PauseMenuWidget.ToSharedRef(), 1000);
    }

    bPauseMenuOpen = true;
    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);

    // A local prototype may pause the world.
    // In multiplayer, opening the menu must never pause the authoritative server.
    if (GetNetMode() == NM_Standalone)
    {
        SetPause(true);
    }
}

void AIronGridPlayerController::ClosePauseMenu()
{
    if (!bPauseMenuOpen)
    {
        return;
    }

    if (GEngine && GEngine->GameViewport && PauseMenuWidget.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(PauseMenuWidget.ToSharedRef());
    }

    if (GetNetMode() == NM_Standalone)
    {
        SetPause(false);
    }

    bPauseMenuOpen = false;
    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}

void AIronGridPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (bPauseMenuOpen)
    {
        return;
    }

    AIronGridTankPawn* Tank = GetIronGridTank();
    if (!Tank)
    {
        return;
    }

    FHitResult CursorHit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, CursorHit) && CursorHit.bBlockingHit)
    {
        Tank->SetDesiredAimPoint(CursorHit.ImpactPoint);
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
