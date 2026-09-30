#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IronGridProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class BDFR_IRONGRID_API AIronGridProjectile : public AActor
{
    GENERATED_BODY()

public:
    AIronGridProjectile();

    virtual void BeginPlay() override;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="0.0"))
    float Damage = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="1.0"))
    float LaunchSpeed = 30000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="0.0"))
    float GravityScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Projectile", meta=(ClampMin="0.1"))
    float ProjectileLifeSeconds = 8.0f;

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

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastImpactFX(FVector ImpactPoint, FVector ImpactNormal);
};
