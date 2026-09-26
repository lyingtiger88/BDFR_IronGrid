#include "UI/IronGridHUD.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "Tank/IronGridTankPawn.h"

void AIronGridHUD::DrawHUD()
{
    Super::DrawHUD();

    APlayerController* PC = GetOwningPlayerController();
    AIronGridTankPawn* Tank = PC ? Cast<AIronGridTankPawn>(PC->GetPawn()) : nullptr;
    if (!PC || !Tank)
    {
        return;
    }

    FVector2D DesiredScreen;
    if (PC->ProjectWorldLocationToScreen(Tank->GetDesiredAimPoint(), DesiredScreen, true))
    {
        DrawCross(DesiredScreen, DesiredReticleSize, FLinearColor(1.0f, 0.75f, 0.05f, 1.0f), 2.0f);
    }

    FVector2D ActualScreen;
    if (PC->ProjectWorldLocationToScreen(Tank->GetActualGunAimPoint(), ActualScreen, true))
    {
        const float Error = Tank->GetAimErrorDegrees();
        const FLinearColor ActualColor = Error <= 1.5f
            ? FLinearColor(0.1f, 1.0f, 0.2f, 1.0f)
            : FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);

        DrawCross(ActualScreen, ActualReticleSize, ActualColor, 2.0f);
    }
}

void AIronGridHUD::DrawCross(const FVector2D& Position, float Size, const FLinearColor& Color, float Thickness)
{
    DrawLine(Position.X - Size, Position.Y, Position.X + Size, Position.Y, Color, Thickness);
    DrawLine(Position.X, Position.Y - Size, Position.X, Position.Y + Size, Color, Thickness);
}
