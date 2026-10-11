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

    // Organic decoration is intentionally independent from the hidden planning
    // grid so the final forest does not reveal square cells.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="80.0", ClampMax="1000.0"))
    float OrganicTreeSpacing = 190.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OrganicDenseForestAcceptance = 0.92f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OrganicLightForestAcceptance = 0.52f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="1.0", ClampMax="30.0"))
    float OrganicAttemptMultiplier = 7.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OrganicBushChanceNearTree = 0.42f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OrganicRockChanceNearTree = 0.10f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic", meta=(ClampMin="20.0", ClampMax="1000.0"))
    float OrganicCompanionRadius = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Clusters", meta=(ClampMin="1", ClampMax="32"))
    int32 OrganicClusterCount = 8;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Clusters", meta=(ClampMin="300.0", ClampMax="5000.0"))
    float OrganicClusterMinRadius = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Clusters", meta=(ClampMin="300.0", ClampMax="5000.0"))
    float OrganicClusterMaxRadius = 2200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Clusters", meta=(ClampMin="0.0", ClampMax="1.0"))
    float OrganicClusterEdgeFalloff = 0.60f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Scale", meta=(ClampMin="0.1", ClampMax="4.0"))
    float OrganicTreeScaleMultiplier = 1.45f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Scale", meta=(ClampMin="0.1", ClampMax="4.0"))
    float OrganicBushScaleMultiplier = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Organic|Scale", meta=(ClampMin="0.1", ClampMax="4.0"))
    float OrganicRockScaleMultiplier = 0.80f;
};
