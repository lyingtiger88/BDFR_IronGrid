#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IronGridProjectile.generated.h"

class UPaperSpriteComponent;
class USphereComponent;
class UStaticMeshComponent;
class USoundBase;
class UProjectileMovementComponent;

UCLASS()
class BDFR_IRONGRID_API AIronGridProjectile : public AActor
{
    GENERATED_BODY()

public:
    AIronGridProjectile();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="IronGrid|Projectile")
    void InitializeProjectile(float InSpeed, float InDamage, float InGravityScale);

    UFUNCTION(BlueprintPure, Category="IronGrid|Projectile")
    float GetProjectileDamage() const { return Damage; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Projectile")
    float GetProjectileSpeed() const { return LaunchSpeed; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Projectile")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Projectile")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    // Production-facing sprite visual. Assign a PaperSprite in a Blueprint child.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Projectile|Visual")
    TObjectPtr<UPaperSpriteComponent> ProjectileSprite;

    // Temporary visible shell used until final sprite/tracer art is assigned.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Projectile|Debug")
    TObjectPtr<UStaticMeshComponent> DebugVisual;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="0.0"))
    float Damage = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="1.0"))
    float LaunchSpeed = 30000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="0.0"))
    float GravityScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="0.1"))
    float ProjectileLifeSeconds = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Audio")
    TObjectPtr<USoundBase> ImpactSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Audio", meta=(ClampMin="0.0", ClampMax="2.0"))
    float ImpactSoundVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Debug")
    bool bShowDebugTrail = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Debug")
    bool bShowDebugImpactFX = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Debug")
    bool bHideDebugVisualWhenSpriteAssigned = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Debug", meta=(ClampMin="0.01"))
    float DebugTrailDuration = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile|Debug", meta=(ClampMin="1.0"))
    float DebugMarkerRadius = 14.0f;

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|Projectile|FX")
    void OnProjectileLaunched();

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|Projectile|FX")
    void OnImpactFX(FVector ImpactPoint, FVector ImpactNormal);

private:
    UFUNCTION()
    void HandleProjectileHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastImpactFX(FVector ImpactPoint, FVector ImpactNormal);

    FVector PreviousDebugLocation = FVector::ZeroVector;
};
