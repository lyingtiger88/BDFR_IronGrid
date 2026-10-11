#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PaperSprite.h"
#include "IronGridEnvironmentPalette.generated.h"

UCLASS(BlueprintType)
class BDFR_IRONGRID_API UIronGridEnvironmentPalette : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Forest")
    TArray<TObjectPtr<UPaperSprite>> DenseForestTrees;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Forest")
    TArray<TObjectPtr<UPaperSprite>> LightForestTrees;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Forest")
    TArray<TObjectPtr<UPaperSprite>> Bushes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Forest")
    TArray<TObjectPtr<UPaperSprite>> Rocks;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    TArray<TObjectPtr<UPaperSprite>> ClearingGround;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    TArray<TObjectPtr<UPaperSprite>> RoadGround;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    TArray<TObjectPtr<UPaperSprite>> WaterGround;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0"))
    int32 DenseForestMinTreesPerCell = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0"))
    int32 DenseForestMaxTreesPerCell = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0"))
    int32 LightForestMinTreesPerCell = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0"))
    int32 LightForestMaxTreesPerCell = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BushChancePerForestCell = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RockChancePerCell = 0.22f;

    // Global normalization for the generated 256/512px IRON GRID environment pack.
    // Keeps trees/rocks/bushes in gameplay scale instead of texture-pixel scale.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0.05", ClampMax="4.0"))
    float GeneratedSpriteScaleMultiplier = 0.50f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0.1"))
    float MinSpriteScale = 0.90f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement", meta=(ClampMin="0.1"))
    float MaxSpriteScale = 1.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement")
    FRotator SpriteRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement")
    bool bRandomizeYaw = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Placement")
    float SpriteZOffset = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    bool bAutoScaleGroundToCell = true;

    // Our sliced ground tiles are authored as 256x256 Paper2D sources at the
    // default 1 pixel = 1 Unreal unit import scale.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground", meta=(ClampMin="1.0"))
    float GroundReferenceWorldSize = 256.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    float GroundSpriteScale = 1.0f;

    // Slight overlap hides sub-pixel seams between otherwise edge-matched tiles.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground", meta=(ClampMin="1.0", ClampMax="1.10"))
    float GroundTileOverlapScale = 1.02f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    FRotator GroundSpriteRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IronGrid|Ground")
    float GroundZOffset = 0.0f;
};
