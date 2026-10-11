#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/IronGridWorldTypes.h"
#include "IronGridWorldBuilder.generated.h"

class UBoxComponent;
class UIronGridEnvironmentProfile;
class UIronGridEnvironmentPalette;
class UPaperSprite;

UENUM(BlueprintType)
enum class EIronGridWorldBuilderMode : uint8
{
    Auto UMETA(DisplayName="Auto"),
    Manual UMETA(DisplayName="Manual"),
    Hybrid UMETA(DisplayName="Hybrid")
};

UCLASS(BlueprintType)
class BDFR_IRONGRID_API AIronGridWorldBuilder : public AActor
{
    GENERATED_BODY()

public:
    AIronGridWorldBuilder();

    virtual void OnConstruction(const FTransform& Transform) override;

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void GeneratePreview();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void BuildEnvironment();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void ClearGeneratedEnvironment();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void ClearPreview();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void RandomizeSeed();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void ValidateBuilder();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void ValidateGameplay();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder")
    EIronGridWorldBuilderMode BuilderMode = EIronGridWorldBuilderMode::Hybrid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder")
    int32 Seed = 482193;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder", meta=(ClampMin="1000.0"))
    FVector2D MapSize = FVector2D(10000.0f, 10000.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder", meta=(ClampMin="100.0"))
    float PreviewCellSize = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder")
    TObjectPtr<UIronGridEnvironmentProfile> EnvironmentProfile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder")
    TObjectPtr<UIronGridEnvironmentPalette> EnvironmentPalette;

    // Production default: keep the planning grid hidden and use it only for
    // classification/validation. The visible environment is scattered in
    // continuous world space over the existing Landscape.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Organic")
    bool bUseOrganicEnvironment = true;

    // Legacy tile-per-cell ground remains available only for debugging.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Organic")
    bool bSpawnLegacyGroundTiles = false;

    // Road/water cell art is intentionally disabled in organic mode until the
    // spline path renderer is active. This prevents square road/river islands.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Organic")
    bool bSpawnLegacyRoadWaterTiles = false;

    // Convenience fallback for the standard project assets. Explicit actor
    // assignments still take priority.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Auto Setup")
    bool bAutoResolveDefaultDataAssets = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Auto Setup")
    FSoftObjectPath DefaultEnvironmentProfilePath =
        FSoftObjectPath(TEXT("/Game/DataAssets/DA_Environment_Forest.DA_Environment_Forest"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Auto Setup")
    FSoftObjectPath DefaultEnvironmentPalettePath =
        FSoftObjectPath(TEXT("/Game/DataAssets/DA_ForestPalette.DA_ForestPalette"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview")
    bool bShowPreviewGrid = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview")
    bool bShowBiomePoints = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview")
    bool bHidePreviewAfterBuild = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview", meta=(ClampMin="1.0", ClampMax="100.0"))
    float SamplePointSize = 18.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="IronGrid|World Builder|Runtime")
    TArray<FIronGridBiomeCell> GeneratedCells;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="IronGrid|World Builder|Runtime")
    TArray<FVector2D> GeneratedClearingCenters;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World Builder")
    TObjectPtr<UBoxComponent> MapBounds;

private:
    void UpdateBounds();
    void DrawPreview();
    void BuildBiomeMap();
    void BuildOrganicEnvironment();
    bool IsOrganicPointFarEnough(
        const FVector2D& Candidate,
        const TArray<FVector2D>& ExistingPoints,
        float MinimumSpacing) const;
    EIronGridBiomeType ClassifyOrganicPoint(
        const FVector2D& LocalLocation,
        float& OutDensity) const;

    EIronGridBiomeType ClassifyAutomaticBiome(
        const FVector& WorldLocation,
        const FVector2D& LocalLocation,
        float& OutDensity,
        const TArray<FVector2D>& ClearingCenters) const;

    void ApplyZoneOverride(FIronGridBiomeCell& Cell) const;
    float SampleForestNoise(const FVector2D& LocalLocation) const;
    FColor GetBiomeDebugColor(EIronGridBiomeType Biome) const;

    void ResolveDefaultDataAssetsIfNeeded();

    int32 CountValidSprites(
        const TArray<TObjectPtr<UPaperSprite>>& Sprites) const;

    UPaperSprite* PickSprite(
        const TArray<TObjectPtr<UPaperSprite>>& Sprites,
        FRandomStream& RandomStream) const;

    UPaperSprite* PickSpriteByNameTokens(
        const TArray<TObjectPtr<UPaperSprite>>& Sprites,
        const TArray<FString>& PreferredTokens,
        FRandomStream& RandomStream) const;

    void SpawnSprite(
        UPaperSprite* Sprite,
        const FVector& Location,
        FRandomStream& RandomStream,
        FName LayerTag,
        float AdditionalScaleMultiplier = 1.0f);
};
