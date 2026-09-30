#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IronGridPowerUp.generated.h"

class AIronGridTankPawn;
class UPrimitiveComponent;
class USceneComponent;
class USoundBase;
class USphereComponent;
class UStaticMeshComponent;
class UPaperSpriteComponent;

UENUM(BlueprintType)
enum class EIronGridPowerUpType : uint8
{
    Ammo UMETA(DisplayName="Ammo"),
    SpeedBoost UMETA(DisplayName="Speed Boost"),
    ReloadBoost UMETA(DisplayName="Reload Boost"),
    Repair UMETA(DisplayName="Repair")
};

UCLASS()
class BDFR_IRONGRID_API AIronGridPowerUp : public AActor
{
    GENERATED_BODY()

public:
    AIronGridPowerUp();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintPure, Category="IronGrid|PowerUp")
    EIronGridPowerUpType GetPowerUpType() const { return PowerUpType; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|PowerUp")
    TObjectPtr<USphereComponent> PickupCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|PowerUp")
    TObjectPtr<USceneComponent> VisualRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|PowerUp|Visual")
    TObjectPtr<UPaperSpriteComponent> PowerUpSprite;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|PowerUp|Visual")
    TObjectPtr<UStaticMeshComponent> DebugVisual;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp")
    EIronGridPowerUpType PowerUpType = EIronGridPowerUpType::Ammo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Ammo", meta=(ClampMin="1"))
    int32 AmmoAmount = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Speed", meta=(ClampMin="1.0"))
    float SpeedMultiplier = 1.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Reload", meta=(ClampMin="0.2", ClampMax="1.0"))
    float ReloadTimeMultiplier = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Timed", meta=(ClampMin="0.1"))
    float EffectDuration = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Repair", meta=(ClampMin="0.0"))
    float RepairAmount = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Audio")
    TObjectPtr<USoundBase> SpawnSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Audio")
    TObjectPtr<USoundBase> PickupSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Audio", meta=(ClampMin="0.0", ClampMax="2.0"))
    float SpawnSoundVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Audio", meta=(ClampMin="0.0", ClampMax="2.0"))
    float PickupSoundVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Visual")
    bool bAnimateVisual = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Visual", meta=(ClampMin="0.0"))
    float BobAmplitude = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Visual", meta=(ClampMin="0.0"))
    float BobSpeed = 2.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Visual")
    float RotationSpeed = 55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|PowerUp|Visual")
    bool bHideDebugVisualWhenSpriteAssigned = true;

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|PowerUp|FX")
    void OnPowerUpSpawned();

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|PowerUp|FX")
    void OnPowerUpPickedUp(AIronGridTankPawn* Tank);

private:
    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPickupFeedback();

    void ApplyPowerUp(AIronGridTankPawn* Tank);

    float AnimationTime = 0.0f;
};
