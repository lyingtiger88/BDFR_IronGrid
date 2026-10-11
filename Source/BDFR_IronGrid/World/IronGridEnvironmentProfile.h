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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Shape", meta=(ClampMin="0.00001", ClampMax="0.02"))
    float ForestNoiseScale = 0.00125f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Shape", meta=(ClampMin="0.0", ClampMax="1.0"))
    float DenseForestThreshold = 0.62f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Shape", meta=(ClampMin="0.0", ClampMax="1.0"))
    float LightForestThreshold = 0.36f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Road", meta=(ClampMin="100.0"))
    float BaseRoadWidth = 420.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Water", meta=(ClampMin="100.0"))
    float BaseRiverWidth = 520.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Gameplay", meta=(ClampMin="100.0"))
    float MinimumTankCorridorWidth = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Gameplay", meta=(ClampMin="100.0"))
    float MinimumClearingRadius = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Gameplay", meta=(ClampMin="0", ClampMax="16"))
    int32 ClearingCount = 5;
};
