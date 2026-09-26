#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IronGridTankPawn.generated.h"

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

    UFUNCTION(BlueprintCallable, Category="IronGrid|Aim")
    void SetDesiredAimPoint(const FVector& WorldPoint);

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    FVector GetDesiredAimPoint() const { return DesiredAimWorldPoint; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    FVector GetActualGunAimPoint() const;

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    float GetAimErrorDegrees() const;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement", meta=(ClampMin="0.0"))
    float MaxMoveSpeed = 700.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Movement", meta=(ClampMin="0.0"))
    float HullTurnSpeed = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Aim", meta=(ClampMin="0.0"))
    float TurretTraverseSpeed = 55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Aim", meta=(ClampMin="100.0"))
    float AimTraceDistance = 100000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Visual", meta=(ClampMin="-180.0", ClampMax="180.0"))
    float HullArtYawOffset = -90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IronGrid|Visual", meta=(ClampMin="-180.0", ClampMax="180.0"))
    float TurretArtYawOffset = -90.0f;

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
    float CameraZoomInterpSpeed = 3.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Camera")
    float CurrentGroundSpeed = 0.0f;

private:
    void MoveForward(float Value);
    void TurnHull(float Value);
    void UpdateTurret(float DeltaSeconds);
    void UpdateCamera(float DeltaSeconds);
    void ApplyCameraModeImmediate();

    float MoveInput = 0.0f;
    float TurnInput = 0.0f;
    FVector DesiredAimWorldPoint = FVector::ZeroVector;
    FVector PreviousActorLocation = FVector::ZeroVector;
};
