#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IronGridWorldBuilder.generated.h"

class UBoxComponent;
class UIronGridEnvironmentProfile;

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
    void ClearPreview();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void RandomizeSeed();

    UFUNCTION(CallInEditor, BlueprintCallable, Category="IronGrid|World Builder")
    void ValidateBuilder();

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview")
    bool bShowPreviewGrid = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview")
    bool bShowSamplePoints = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|World Builder|Preview", meta=(ClampMin="1.0", ClampMax="100.0"))
    float SamplePointSize = 12.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="IronGrid|World Builder|Runtime")
    TArray<FVector> GeneratedSamplePoints;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|World Builder")
    TObjectPtr<UBoxComponent> MapBounds;

private:
    void UpdateBounds();
    void DrawPreview();
};
