#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IronGridPlayerController.generated.h"

class AIronGridTankPawn;
class SWidget;

UCLASS()
class BDFR_IRONGRID_API AIronGridPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIronGridPlayerController();

    virtual void PlayerTick(float DeltaTime) override;
    virtual void SetupInputComponent() override;

    UFUNCTION(BlueprintCallable, Category="IronGrid|Pause")
    void TogglePauseMenu();

    UFUNCTION(BlueprintCallable, Category="IronGrid|Pause")
    void OpenPauseMenu();

    UFUNCTION(BlueprintCallable, Category="IronGrid|Pause")
    void ClosePauseMenu();

    UFUNCTION(BlueprintPure, Category="IronGrid|Pause")
    bool IsPauseMenuOpen() const { return bPauseMenuOpen; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Tank")
    AIronGridTankPawn* GetIronGridTank() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Aim")
    float AimPlaneZ = 0.0f;

private:
    bool bPauseMenuOpen = false;
    TSharedPtr<SWidget> PauseMenuWidget;
};
