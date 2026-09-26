#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IronGridPlayerController.generated.h"

UCLASS()
class BDFR_IRONGRID_API AIronGridPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIronGridPlayerController();

    virtual void PlayerTick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Aim")
    float AimPlaneZ = 0.0f;
};
