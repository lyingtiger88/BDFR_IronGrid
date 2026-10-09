#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IronGridGameMode.generated.h"

UCLASS()
class BDFR_IRONGRID_API AIronGridGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIronGridGameMode();

    virtual void InitGame(
        const FString& MapName,
        const FString& Options,
        FString& ErrorMessage) override;

protected:
    // Optional Blueprint pawn class. Set this to the tank Blueprint that contains
    // project-specific sprites, sounds, FX and tuning. Native C++ remains fallback.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IronGrid|Classes")
    TSoftClassPtr<APawn> TankPawnClassOverride;
};
