#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IronGridHUD.generated.h"

UCLASS()
class BDFR_IRONGRID_API AIronGridHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    UPROPERTY(EditAnywhere, Category="IronGrid|HUD")
    float DesiredReticleSize = 12.0f;

    UPROPERTY(EditAnywhere, Category="IronGrid|HUD")
    float ActualReticleSize = 9.0f;

    UPROPERTY(EditAnywhere, Category="IronGrid|HUD")
    float BallisticReticleSize = 7.0f;

private:
    void DrawCross(const FVector2D& Position, float Size, const FLinearColor& Color, float Thickness);
};
