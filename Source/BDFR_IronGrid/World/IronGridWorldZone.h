#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/IronGridWorldTypes.h"
#include "IronGridWorldZone.generated.h"

class UBoxComponent;

UCLASS(BlueprintType)
class BDFR_IRONGRID_API AIronGridWorldZone : public AActor
{
    GENERATED_BODY()

public:
    AIronGridWorldZone();

    UFUNCTION(BlueprintPure, Category="IronGrid|World Zone")
    bool ContainsWorldPoint(const FVector& WorldPoint) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Zone")
    bool bEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Zone")
    bool bExcludeGeneration = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Zone")
    EIronGridBiomeType BiomeOverride = EIronGridBiomeType::Clearing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Zone")
    int32 Priority = 0;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World Zone")
    TObjectPtr<UBoxComponent> ZoneBounds;
};
