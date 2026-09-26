#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class AIronGridPlayerController;
class AIronGridTankPawn;

class SIronGridPauseMenu : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SIronGridPauseMenu) {}
        SLATE_ARGUMENT(TWeakObjectPtr<AIronGridPlayerController>, OwnerController)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    AIronGridTankPawn* GetTank() const;

    FReply OnResumeClicked();
    FReply OnCameraModeClicked();

    void OnCameraResponseChanged(float Value);
    float GetCameraResponseValue() const;
    FText GetCameraResponseText() const;

    void OnFixedZoomChanged(float Value);
    float GetFixedZoomValue() const;
    FText GetFixedZoomText() const;

    void OnDynamicFarZoomChanged(float Value);
    float GetDynamicFarZoomValue() const;
    FText GetDynamicFarZoomText() const;

    FText GetCameraModeText() const;

    TWeakObjectPtr<AIronGridPlayerController> OwnerController;
};
