#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IronGridEnvironmentProfile.generated.h"

UCLASS(BlueprintType)
class BDFR_IRONGRID_API UIronGridEnvironmentProfile : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Environment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ForestDensity = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Environment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ClearingRatio = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Environment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RoadDensity = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Environment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float WaterAmount = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Environment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float CoverDensity = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Environment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float DecorationDensity = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Gameplay", meta=(ClampMin="100.0"))
    float MinimumTankCorridorWidth = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Gameplay", meta=(ClampMin="100.0"))
    float MinimumClearingRadius = 500.0f;
};
