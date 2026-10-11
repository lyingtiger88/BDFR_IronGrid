#pragma once

#include "CoreMinimal.h"
#include "IronGridWorldTypes.generated.h"

UENUM(BlueprintType)
enum class EIronGridBiomeType : uint8
{
    DenseForest UMETA(DisplayName="Dense Forest"),
    LightForest UMETA(DisplayName="Light Forest"),
    Clearing UMETA(DisplayName="Clearing"),
    Road UMETA(DisplayName="Road"),
    Water UMETA(DisplayName="Water")
};

USTRUCT(BlueprintType)
struct BDFR_IRONGRID_API FIronGridBiomeCell
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    int32 GridX = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    int32 GridY = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    FVector WorldLocation = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    EIronGridBiomeType Biome = EIronGridBiomeType::Clearing;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    float Density = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    bool bManualOverride = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World")
    bool bExcluded = false;
};
