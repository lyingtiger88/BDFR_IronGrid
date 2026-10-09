#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IronGridGameMode.generated.h"

UCLASS(Config=Game, DefaultConfig)
class BDFR_IRONGRID_API AIronGridGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIronGridGameMode();

protected:
    // Optional Blueprint pawn class. Set this to the tank Blueprint that contains
    // project-specific sprites, sounds, FX and tuning. Native C++ remains fallback.
    UPROPERTY(Config, EditDefaultsOnly, Category="IronGrid|Classes")
    TSoftClassPtr<APawn> TankPawnClassOverride;
};
