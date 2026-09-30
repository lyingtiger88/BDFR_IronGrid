#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IronGridTankPawn.generated.h"

class AIronGridProjectile;
class UCameraComponent;
class UPaperSpriteComponent;
class USceneComponent;
class USpringArmComponent;

UENUM(BlueprintType)
enum class EIronGridCameraMode : uint8
{
    Fixed UMETA(DisplayName="Fixed"),
    SpeedReactive UMETA(DisplayName="Speed Reactive")
};

UCLASS()
class BDFR_IRONGRID_API AIronGridTankPawn : public APawn
{
    GENERATED_BODY()

public:
    AIronGridTankPawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category="IronGrid|Aim")
    void SetDesiredAimPoint(const FVector& WorldPoint);

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    FVector GetDesiredAimPoint() const { return DesiredAimWorldPoint; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    FVector GetActualGunAimPoint() const;

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    float GetAimErrorDegrees() const;

    UFUNCTION(BlueprintPure, Category="IronGrid|Weapon")
    FVector GetPredictedBallisticImpactPoint() const;

    UFUNCTION(BlueprintCallable, Category="IronGrid|Weapon")
    void FireWeapon();

    UFUNCTION(BlueprintCallable, Category="IronGrid|Weapon")
    void ReloadWeapon();

    UFUNCTION(BlueprintPure, Category="IronGrid|Weapon")
    int32 GetAmmoInMagazine() const { return CurrentAmmoInMagazine; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Weapon")
    int32 GetMagazineSize() const { return MagazineSize; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Weapon")
    int32 GetReserveAmmo() const { return ReserveAmmo; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Weapon")
    bool IsWeaponReloading() const { return bReloading; }

    UFUNCTION(BlueprintCallable, Category="IronGrid|Visual")
    void ApplyVisualRotationOffsets();

    UFUNCTION(BlueprintCallable, Category="IronGrid|Camera")
    void SetCameraMode(EIronGridCameraMode NewMode);

    UFUNCTION(BlueprintCallable, Category="IronGrid|Camera")
    void ToggleCameraMode();

    UFUNCTION(BlueprintPure, Category="IronGrid|Camera")
    EIronGridCameraMode GetCameraMode() const { return CameraMode; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Camera")
    float GetCurrentGroundSpeed() const { return CurrentGroundSpeed; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Movement")
    float GetCurrentForwardSpeed() const { return CurrentForwardSpeed; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Movement")
    float GetLeftTrackSpeed() const { return CurrentLeftTrackSpeed; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Movement")
    float GetRightTrackSpeed() const { return CurrentRightTrackSpeed; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Movement")
    bool IsBraking() const { return bBrakeHeld; }

    UFUNCTION(BlueprintCallable, Category="IronGrid|Camera")
    void SetCameraResponseSpeed(float NewSpeed);

    UFUNCTION(BlueprintPure, Category="IronGrid|Camera")
    float GetCameraResponseSpeed() const { return CameraZoomInterpSpeed; }

    UFUNCTION(BlueprintCallable, Category="IronGrid|Camera")
    void SetFixedCameraZoom(float NewOrthoWidth);

    UFUNCTION(BlueprintPure, Category="IronGrid|Camera")
    float GetFixedCameraZoom() const { return FixedOrthoWidth; }

    UFUNCTION(BlueprintCallable, Category="IronGrid|Camera")
    void SetDynamicFarCameraZoom(float NewOrthoWidth);

    UFUNCTION(BlueprintPure, Category="IronGrid|Camera")
    float GetDynamicFarCameraZoom() const { return FarOrthoWidth; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<USceneComponent> TankRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<USceneComponent> HullVisualRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<UPaperSpriteComponent> HullSprite;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<USceneComponent> TurretPivot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<USceneComponent> TurretVisualRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<UPaperSpriteComponent> TurretSprite;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Weapon")
    TObjectPtr<USceneComponent> Muzzle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Camera")
    TObjectPtr<UCameraComponent> TopDownCamera;

    // Maximum forward track speed in Unreal units per second.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0"))
    float MaxMoveSpeed = 700.0f;

    // Reverse is intentionally slower than forward movement.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0"))
    float MaxReverseSpeed = 360.0f;

    // Maximum hull yaw rate. Differential-track math is clamped to this value.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0"))
    float HullTurnSpeed = 90.0f;

    // Distance between left and right track centerlines.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="10.0"))
    float TrackSeparation = 260.0f;

    // Track-speed change rates.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="1.0"))
    float TrackAcceleration = 420.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="1.0"))
    float TrackDeceleration = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="1.0"))
    float BrakeDeceleration = 1050.0f;

    // Differential steering authority while moving.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0", ClampMax="1.0"))
    float MovingSteeringStrength = 0.62f;

    // Steering authority retained at maximum speed.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0", ClampMax="1.0"))
    float HighSpeedSteeringScale = 0.48f;

    // Opposite track speed used for neutral / pivot steering.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0"))
    float PivotTrackSpeed = 245.0f;

    // Damp track speeds after hitting a blocking object.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement|Tracked", meta=(ClampMin="0.0", ClampMax="1.0"))
    float CollisionSpeedRetention = 0.18f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Movement|Runtime")
    float CurrentLeftTrackSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Movement|Runtime")
    float CurrentRightTrackSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Movement|Runtime")
    float CurrentForwardSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Movement|Runtime")
    bool bBrakeHeld = false;


    // --- Weapon / ballistics ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon")
    TSubclassOf<AIronGridProjectile> ProjectileClass;

    // Most tanks reload one shell at a time. Increase this for autoloaders/magazines.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ammo", meta=(ClampMin="1"))
    int32 MagazineSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ammo", meta=(ClampMin="0"))
    int32 StartingReserveAmmo = 30;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Weapon|Runtime")
    int32 CurrentAmmoInMagazine = 0;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Weapon|Runtime")
    int32 ReserveAmmo = 0;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Weapon|Runtime")
    bool bReloading = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ballistics", meta=(ClampMin="100.0"))
    float MuzzleVelocity = 30000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ballistics", meta=(ClampMin="0.0"))
    float ProjectileGravityScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ballistics", meta=(ClampMin="0.0"))
    float ProjectileDamage = 100.0f;

    // Keeps the projectile above the flat battlefield collision plane.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ballistics", meta=(ClampMin="0.0"))
    float ProjectileSpawnHeight = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ballistics", meta=(ClampMin="0.1"))
    float BallisticPredictionTime = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon|Ballistics", meta=(ClampMin="0.0"))
    float BallisticPredictionRadius = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon", meta=(ClampMin="0.0"))
    float FireInterval = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Weapon", meta=(ClampMin="0.05"))
    float ReloadDuration = 3.5f;

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|Weapon|FX")
    void OnWeaponFired();

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|Weapon|FX")
    void OnReloadStarted();

    UFUNCTION(BlueprintImplementableEvent, Category="IronGrid|Weapon|FX")
    void OnReloadFinished();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Aim", meta=(ClampMin="0.0"))
    float TurretTraverseSpeed = 55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Aim", meta=(ClampMin="100.0"))
    float AimTraceDistance = 100000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Visual", meta=(ClampMin="-180.0", ClampMax="180.0"))
    float HullArtYawOffset = -90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Visual", meta=(ClampMin="-180.0", ClampMax="180.0"))
    float TurretArtYawOffset = 90.0f;

    // --- Camera mode ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera")
    EIronGridCameraMode CameraMode = EIronGridCameraMode::SpeedReactive;

    // Fixed mode values.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Fixed", meta=(ClampMin="100.0"))
    float FixedCameraArmLength = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Fixed", meta=(ClampMin="100.0"))
    float FixedOrthoWidth = 2600.0f;

    // Speed-reactive near/far values.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Speed Reactive", meta=(ClampMin="100.0"))
    float NearCameraArmLength = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Speed Reactive", meta=(ClampMin="100.0"))
    float FarCameraArmLength = 2400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Speed Reactive", meta=(ClampMin="100.0"))
    float NearOrthoWidth = 2200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Speed Reactive", meta=(ClampMin="100.0"))
    float FarOrthoWidth = 3600.0f;

    // Ground speed at which the camera reaches the fully zoomed-out setting.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera|Speed Reactive", meta=(ClampMin="1.0"))
    float SpeedForMaxCameraZoom = 700.0f;

    // Higher values make the zoom react faster.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Camera", meta=(ClampMin="0.1"))
    float CameraZoomInterpSpeed = 1.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Camera")
    float CurrentGroundSpeed = 0.0f;

private:
    void MoveForward(float Value);
    void TurnHull(float Value);
    void BrakePressed();
    void BrakeReleased();

    UFUNCTION(Server, Reliable)
    void ServerFireWeapon();

    UFUNCTION(Server, Reliable)
    void ServerReloadWeapon();

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastMuzzleFX();

    void PerformFire();
    void StartReload();
    void CompleteReload();

    void UpdateTrackedMovement(float DeltaSeconds);
    float MoveTrackSpeedToward(float CurrentSpeed, float TargetSpeed, float DeltaSeconds) const;
    void UpdateTurret(float DeltaSeconds);
    void UpdateCamera(float DeltaSeconds);
    void ApplyCameraModeImmediate();

    float MoveInput = 0.0f;
    float TurnInput = 0.0f;
    FVector DesiredAimWorldPoint = FVector::ZeroVector;
    FVector PreviousActorLocation = FVector::ZeroVector;

    FTimerHandle ReloadTimerHandle;
    float LastFireTime = -1000.0f;
};
