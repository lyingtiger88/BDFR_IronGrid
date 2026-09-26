#include "UI/IronGridPauseMenu.h"

#include "Player/IronGridPlayerController.h"
#include "Tank/IronGridTankPawn.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void SIronGridPauseMenu::Construct(const FArguments& InArgs)
{
    OwnerController = InArgs._OwnerController;

    ChildSlot
    [
        SNew(SBorder)
        .Padding(FMargin(36.0f))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SBox)
            .WidthOverride(520.0f)
            [
                SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 20.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("IRON GRID: BATTLEGROUND")))
                    .Justification(ETextJustify::Center)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 12.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("PAUSED")))
                    .Justification(ETextJustify::Center)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 4.0f)
                [
                    SNew(SButton)
                    .Text(FText::FromString(TEXT("RESUME")))
                    .HAlign(HAlign_Center)
                    .OnClicked(this, &SIronGridPauseMenu::OnResumeClicked)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 18.0f, 0.0f, 6.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("CAMERA SETTINGS")))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 4.0f)
                [
                    SNew(SButton)
                    .Text(this, &SIronGridPauseMenu::GetCameraModeText)
                    .OnClicked(this, &SIronGridPauseMenu::OnCameraModeClicked)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 8.0f, 0.0f, 2.0f)
                [
                    SNew(STextBlock)
                    .Text(this, &SIronGridPauseMenu::GetCameraResponseText)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 8.0f)
                [
                    SNew(SSlider)
                    .Value(this, &SIronGridPauseMenu::GetCameraResponseValue)
                    .OnValueChanged(this, &SIronGridPauseMenu::OnCameraResponseChanged)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 8.0f, 0.0f, 2.0f)
                [
                    SNew(STextBlock)
                    .Text(this, &SIronGridPauseMenu::GetFixedZoomText)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 8.0f)
                [
                    SNew(SSlider)
                    .Value(this, &SIronGridPauseMenu::GetFixedZoomValue)
                    .OnValueChanged(this, &SIronGridPauseMenu::OnFixedZoomChanged)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 8.0f, 0.0f, 2.0f)
                [
                    SNew(STextBlock)
                    .Text(this, &SIronGridPauseMenu::GetDynamicFarZoomText)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 16.0f)
                [
                    SNew(SSlider)
                    .Value(this, &SIronGridPauseMenu::GetDynamicFarZoomValue)
                    .OnValueChanged(this, &SIronGridPauseMenu::OnDynamicFarZoomChanged)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 12.0f, 0.0f, 4.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("CONTROLS")))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .AutoWrapText(true)
                    .Text(FText::FromString(
                        TEXT("W/S  Move forward/reverse\n")
                        TEXT("A/D  Turn hull\n")
                        TEXT("Mouse  Desired turret aim\n")
                        TEXT("C  Toggle camera mode\n")
                        TEXT("P  Pause / Resume")
                    ))
                ]
            ]
        ]
    ];
}

AIronGridTankPawn* SIronGridPauseMenu::GetTank() const
{
    return OwnerController.IsValid() ? OwnerController->GetIronGridTank() : nullptr;
}

FReply SIronGridPauseMenu::OnResumeClicked()
{
    if (OwnerController.IsValid())
    {
        OwnerController->ClosePauseMenu();
    }
    return FReply::Handled();
}

FReply SIronGridPauseMenu::OnCameraModeClicked()
{
    if (AIronGridTankPawn* Tank = GetTank())
    {
        Tank->ToggleCameraMode();
    }
    return FReply::Handled();
}

FText SIronGridPauseMenu::GetCameraModeText() const
{
    const AIronGridTankPawn* Tank = GetTank();
    if (!Tank)
    {
        return FText::FromString(TEXT("Camera Mode: Unavailable"));
    }

    const TCHAR* ModeName =
        Tank->GetCameraMode() == EIronGridCameraMode::Fixed
            ? TEXT("Fixed")
            : TEXT("Speed Reactive");

    return FText::FromString(FString::Printf(TEXT("Camera Mode: %s"), ModeName));
}

void SIronGridPauseMenu::OnCameraResponseChanged(float Value)
{
    if (AIronGridTankPawn* Tank = GetTank())
    {
        Tank->SetCameraResponseSpeed(FMath::Lerp(0.35f, 3.0f, Value));
    }
}

float SIronGridPauseMenu::GetCameraResponseValue() const
{
    if (const AIronGridTankPawn* Tank = GetTank())
    {
        return FMath::GetMappedRangeValueClamped(
            FVector2D(0.35f, 3.0f),
            FVector2D(0.0f, 1.0f),
            Tank->GetCameraResponseSpeed());
    }
    return 0.0f;
}

FText SIronGridPauseMenu::GetCameraResponseText() const
{
    const AIronGridTankPawn* Tank = GetTank();
    const float Value = Tank ? Tank->GetCameraResponseSpeed() : 0.0f;
    return FText::FromString(FString::Printf(TEXT("Camera Response: %.2f"), Value));
}

void SIronGridPauseMenu::OnFixedZoomChanged(float Value)
{
    if (AIronGridTankPawn* Tank = GetTank())
    {
        Tank->SetFixedCameraZoom(FMath::Lerp(1800.0f, 4200.0f, Value));
    }
}

float SIronGridPauseMenu::GetFixedZoomValue() const
{
    if (const AIronGridTankPawn* Tank = GetTank())
    {
        return FMath::GetMappedRangeValueClamped(
            FVector2D(1800.0f, 4200.0f),
            FVector2D(0.0f, 1.0f),
            Tank->GetFixedCameraZoom());
    }
    return 0.0f;
}

FText SIronGridPauseMenu::GetFixedZoomText() const
{
    const AIronGridTankPawn* Tank = GetTank();
    const float Value = Tank ? Tank->GetFixedCameraZoom() : 0.0f;
    return FText::FromString(FString::Printf(TEXT("Fixed Zoom: %.0f"), Value));
}

void SIronGridPauseMenu::OnDynamicFarZoomChanged(float Value)
{
    if (AIronGridTankPawn* Tank = GetTank())
    {
        Tank->SetDynamicFarCameraZoom(FMath::Lerp(2800.0f, 5000.0f, Value));
    }
}

float SIronGridPauseMenu::GetDynamicFarZoomValue() const
{
    if (const AIronGridTankPawn* Tank = GetTank())
    {
        return FMath::GetMappedRangeValueClamped(
            FVector2D(2800.0f, 5000.0f),
            FVector2D(0.0f, 1.0f),
            Tank->GetDynamicFarCameraZoom());
    }
    return 0.0f;
}

FText SIronGridPauseMenu::GetDynamicFarZoomText() const
{
    const AIronGridTankPawn* Tank = GetTank();
    const float Value = Tank ? Tank->GetDynamicFarCameraZoom() : 0.0f;
    return FText::FromString(FString::Printf(TEXT("Dynamic Max Zoom: %.0f"), Value));
}
