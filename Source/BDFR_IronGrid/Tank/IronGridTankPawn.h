#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IronGridTankPawn.generated.h"

class UCameraComponent;
class UPaperSpriteComponent;
class USceneComponent;
class USpringArmComponent;

UCLASS()
class BDFR_IRONGRID_API AIronGridTankPawn : public APawn
{
    GENERATED_BODY()

public:
    AIronGridTankPawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category="IronGrid|Aim")
    void SetDesiredAimPoint(const FVector& WorldPoint);

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    FVector GetDesiredAimPoint() const { return DesiredAimWorldPoint; }

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    FVector GetActualGunAimPoint() const;

    UFUNCTION(BlueprintPure, Category="IronGrid|Aim")
    float GetAimErrorDegrees() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<USceneComponent> TankRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<UPaperSpriteComponent> HullSprite;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Tank")
    TObjectPtr<USceneComponent> TurretPivot;

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

private:
    void MoveForward(float Value);
    void TurnHull(float Value);
    void UpdateTurret(float DeltaSeconds);

    float MoveInput = 0.0f;
    float TurnInput = 0.0f;
    FVector DesiredAimWorldPoint = FVector::ZeroVector;
};
