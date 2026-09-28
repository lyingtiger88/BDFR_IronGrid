#include "Tank/IronGridTankPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperSpriteComponent.h"

AIronGridTankPawn::AIronGridTankPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    TankRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TankRoot"));
    SetRootComponent(TankRoot);

    HullVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HullVisualRoot"));
    HullVisualRoot->SetupAttachment(TankRoot);

    HullSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("HullSprite"));
    HullSprite->SetupAttachment(HullVisualRoot);

    TurretPivot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretPivot"));
    TurretPivot->SetupAttachment(TankRoot);

    TurretVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretVisualRoot"));
    TurretVisualRoot->SetupAttachment(TurretPivot);

    TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
    TurretSprite->SetupAttachment(TurretVisualRoot);

    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(TurretPivot);
    Muzzle->SetRelativeLocation(FVector(90.0f, 0.0f, 0.0f));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(TankRoot);
    CameraBoom->TargetArmLength = FixedCameraArmLength;
    CameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bUsePawnControlRotation = false;

    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
    TopDownCamera->OrthoWidth = FixedOrthoWidth;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AIronGridTankPawn::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyVisualRotationOffsets();
}

void AIronGridTankPawn::BeginPlay()
{
    Super::BeginPlay();

    ApplyVisualRotationOffsets();

    DesiredAimWorldPoint = GetActorLocation() + GetActorForwardVector() * 1000.0f;
    PreviousActorLocation = GetActorLocation();

    ApplyCameraModeImmediate();
}

void AIronGridTankPawn::ApplyVisualRotationOffsets()
{
    if (HullVisualRoot)
    {
        HullVisualRoot->SetRelativeRotation(FRotator(0.0f, HullArtYawOffset, 0.0f));
    }

    if (TurretVisualRoot)
    {
        TurretVisualRoot->SetRelativeRotation(FRotator(0.0f, TurretArtYawOffset, 0.0f));
    }
}

void AIronGridTankPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateTrackedMovement(DeltaSeconds);
    UpdateTurret(DeltaSeconds);
    UpdateCamera(DeltaSeconds);
}

void AIronGridTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AIronGridTankPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("TurnHull"), this, &AIronGridTankPawn::TurnHull);
    PlayerInputComponent->BindAction(TEXT("Brake"), IE_Pressed, this, &AIronGridTankPawn::BrakePressed);
    PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &AIronGridTankPawn::BrakeReleased);
    PlayerInputComponent->BindAction(TEXT("ToggleCameraMode"), IE_Pressed, this, &AIronGridTankPawn::ToggleCameraMode);
}

void AIronGridTankPawn::SetDesiredAimPoint(const FVector& WorldPoint)
{
    DesiredAimWorldPoint = WorldPoint;
}

FVector AIronGridTankPawn::GetActualGunAimPoint() const
{
    if (!Muzzle || !TurretPivot)
    {
        return GetActorLocation();
    }

    const FVector Origin = TurretPivot->GetComponentLocation();
    const float DesiredDistance = FVector::Dist2D(Origin, DesiredAimWorldPoint);
    const float DisplayDistance = FMath::Clamp(DesiredDistance, 100.0f, AimTraceDistance);

    FVector ActualDirection = TurretPivot->GetForwardVector();
    ActualDirection.Z = 0.0f;
    ActualDirection.Normalize();

    FVector Point = Origin + ActualDirection * DisplayDistance;
    Point.Z = DesiredAimWorldPoint.Z;
    return Point;
}

float AIronGridTankPawn::GetAimErrorDegrees() const
{
    if (!TurretPivot)
    {
        return 180.0f;
    }

    const FVector ToDesired =
        (DesiredAimWorldPoint - TurretPivot->GetComponentLocation()).GetSafeNormal2D();

    const FVector Actual =
        TurretPivot->GetForwardVector().GetSafeNormal2D();

    const float Dot =
        FMath::Clamp(FVector::DotProduct(ToDesired, Actual), -1.0f, 1.0f);

    return FMath::RadiansToDegrees(FMath::Acos(Dot));
}

void AIronGridTankPawn::MoveForward(float Value)
{
    MoveInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AIronGridTankPawn::TurnHull(float Value)
{
    TurnInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AIronGridTankPawn::BrakePressed()
{
    bBrakeHeld = true;
}

void AIronGridTankPawn::BrakeReleased()
{
    bBrakeHeld = false;
}

float AIronGridTankPawn::MoveTrackSpeedToward(
    float CurrentSpeed,
    float TargetSpeed,
    float DeltaSeconds) const
{
    float ChangeRate = TrackDeceleration;

    if (bBrakeHeld)
    {
        TargetSpeed = 0.0f;
        ChangeRate = BrakeDeceleration;
    }
    else
    {
        const bool bSameDirection =
            FMath::IsNearlyZero(CurrentSpeed) ||
            FMath::Sign(CurrentSpeed) == FMath::Sign(TargetSpeed);

        const bool bAccelerating =
            bSameDirection &&
            FMath::Abs(TargetSpeed) > FMath::Abs(CurrentSpeed);

        if (bAccelerating)
        {
            ChangeRate = TrackAcceleration;
        }
        else if (!bSameDirection && !FMath::IsNearlyZero(TargetSpeed))
        {
            // Direction changes should first shed momentum quickly.
            ChangeRate = BrakeDeceleration;
        }
    }

    return FMath::FInterpConstantTo(
        CurrentSpeed,
        TargetSpeed,
        DeltaSeconds,
        ChangeRate);
}

void AIronGridTankPawn::UpdateTrackedMovement(float DeltaSeconds)
{
    if (DeltaSeconds <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    const float Throttle = FMath::Clamp(MoveInput, -1.0f, 1.0f);
    const float Steering = FMath::Clamp(TurnInput, -1.0f, 1.0f);

    float DesiredLeftTrackSpeed = 0.0f;
    float DesiredRightTrackSpeed = 0.0f;

    const bool bPivotTurn =
        FMath::Abs(Throttle) < 0.05f &&
        FMath::Abs(Steering) > 0.05f &&
        !bBrakeHeld;

    if (bPivotTurn)
    {
        // Neutral steering: tracks run in opposite directions.
        // Positive steering produces positive Unreal yaw.
        DesiredLeftTrackSpeed = Steering * PivotTrackSpeed;
        DesiredRightTrackSpeed = -Steering * PivotTrackSpeed;
    }
    else if (!bBrakeHeld)
    {
        const float BaseTrackSpeed =
            Throttle >= 0.0f
                ? Throttle * MaxMoveSpeed
                : Throttle * MaxReverseSpeed;

        const float SpeedReference =
            FMath::Max(MaxMoveSpeed, 1.0f);

        const float SpeedAlpha =
            FMath::Clamp(
                FMath::Abs(CurrentForwardSpeed) / SpeedReference,
                0.0f,
                1.0f);

        const float SteeringScale =
            FMath::Lerp(
                1.0f,
                HighSpeedSteeringScale,
                SpeedAlpha);

        const float SteeringDifferential =
            Steering *
            MaxMoveSpeed *
            MovingSteeringStrength *
            SteeringScale;

        DesiredLeftTrackSpeed =
            BaseTrackSpeed + SteeringDifferential;

        DesiredRightTrackSpeed =
            BaseTrackSpeed - SteeringDifferential;

        DesiredLeftTrackSpeed =
            FMath::Clamp(
                DesiredLeftTrackSpeed,
                -MaxReverseSpeed,
                MaxMoveSpeed);

        DesiredRightTrackSpeed =
            FMath::Clamp(
                DesiredRightTrackSpeed,
                -MaxReverseSpeed,
                MaxMoveSpeed);
    }

    CurrentLeftTrackSpeed =
        MoveTrackSpeedToward(
            CurrentLeftTrackSpeed,
            DesiredLeftTrackSpeed,
            DeltaSeconds);

    CurrentRightTrackSpeed =
        MoveTrackSpeedToward(
            CurrentRightTrackSpeed,
            DesiredRightTrackSpeed,
            DeltaSeconds);

    CurrentForwardSpeed =
        0.5f * (CurrentLeftTrackSpeed + CurrentRightTrackSpeed);

    // UE uses +X as forward and +Y as right.
    // With left track at -Y and right track at +Y, positive Unreal yaw
    // corresponds to the left track moving faster than the right track.
    const float SafeTrackSeparation =
        FMath::Max(TrackSeparation, 10.0f);

    const float YawRateRadians =
        (CurrentLeftTrackSpeed - CurrentRightTrackSpeed) /
        SafeTrackSeparation;

    const float UnclampedYawRateDegrees =
        FMath::RadiansToDegrees(YawRateRadians);

    const float YawRateDegrees =
        FMath::Clamp(
            UnclampedYawRateDegrees,
            -HullTurnSpeed,
            HullTurnSpeed);

    if (!FMath::IsNearlyZero(YawRateDegrees))
    {
        AddActorLocalRotation(
            FRotator(
                0.0f,
                YawRateDegrees * DeltaSeconds,
                0.0f));
    }

    if (!FMath::IsNearlyZero(CurrentForwardSpeed))
    {
        const FVector Delta =
            GetActorForwardVector() *
            CurrentForwardSpeed *
            DeltaSeconds;

        FHitResult Hit;
        AddActorWorldOffset(Delta, true, &Hit);

        if (Hit.bBlockingHit)
        {
            CurrentLeftTrackSpeed *= CollisionSpeedRetention;
            CurrentRightTrackSpeed *= CollisionSpeedRetention;
            CurrentForwardSpeed =
                0.5f * (CurrentLeftTrackSpeed + CurrentRightTrackSpeed);
        }
    }
}

void AIronGridTankPawn::UpdateTurret(float DeltaSeconds)
{
    if (!TurretPivot)
    {
        return;
    }

    FVector ToAim = DesiredAimWorldPoint - TurretPivot->GetComponentLocation();
    ToAim.Z = 0.0f;

    if (ToAim.IsNearlyZero())
    {
        return;
    }

    const float DesiredWorldYaw = ToAim.Rotation().Yaw;
    const float HullWorldYaw = GetActorRotation().Yaw;
    const float TargetLocalYaw =
        FMath::FindDeltaAngleDegrees(HullWorldYaw, DesiredWorldYaw);

    const float CurrentLocalYaw =
        FRotator::NormalizeAxis(TurretPivot->GetRelativeRotation().Yaw);

    const float MaxStep = TurretTraverseSpeed * DeltaSeconds;
    const float NewLocalYaw =
        FMath::FixedTurn(CurrentLocalYaw, TargetLocalYaw, MaxStep);

    TurretPivot->SetRelativeRotation(FRotator(0.0f, NewLocalYaw, 0.0f));
}

void AIronGridTankPawn::SetCameraResponseSpeed(float NewSpeed)
{
    CameraZoomInterpSpeed = FMath::Clamp(NewSpeed, 0.25f, 6.0f);
}

void AIronGridTankPawn::SetFixedCameraZoom(float NewOrthoWidth)
{
    FixedOrthoWidth = FMath::Clamp(NewOrthoWidth, 1200.0f, 6000.0f);

    if (CameraMode == EIronGridCameraMode::Fixed)
    {
        ApplyCameraModeImmediate();
    }
}

void AIronGridTankPawn::SetDynamicFarCameraZoom(float NewOrthoWidth)
{
    FarOrthoWidth = FMath::Clamp(NewOrthoWidth, NearOrthoWidth + 100.0f, 7000.0f);

    if (CameraMode == EIronGridCameraMode::SpeedReactive)
    {
        ApplyCameraModeImmediate();
    }
}

void AIronGridTankPawn::SetCameraMode(EIronGridCameraMode NewMode)
{
    CameraMode = NewMode;
    ApplyCameraModeImmediate();
}

void AIronGridTankPawn::ToggleCameraMode()
{
    SetCameraMode(
        CameraMode == EIronGridCameraMode::Fixed
            ? EIronGridCameraMode::SpeedReactive
            : EIronGridCameraMode::Fixed
    );
}

void AIronGridTankPawn::ApplyCameraModeImmediate()
{
    if (!CameraBoom || !TopDownCamera)
    {
        return;
    }

    float TargetArmLength = FixedCameraArmLength;
    float TargetOrthoWidth = FixedOrthoWidth;

    if (CameraMode == EIronGridCameraMode::SpeedReactive)
    {
        const float SpeedAlpha =
            FMath::Clamp(CurrentGroundSpeed / FMath::Max(SpeedForMaxCameraZoom, 1.0f), 0.0f, 1.0f);

        TargetArmLength = FMath::Lerp(NearCameraArmLength, FarCameraArmLength, SpeedAlpha);
        TargetOrthoWidth = FMath::Lerp(NearOrthoWidth, FarOrthoWidth, SpeedAlpha);
    }

    CameraBoom->TargetArmLength = TargetArmLength;

    if (TopDownCamera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        TopDownCamera->OrthoWidth = TargetOrthoWidth;
    }
}

void AIronGridTankPawn::UpdateCamera(float DeltaSeconds)
{
    if (!CameraBoom || !TopDownCamera)
    {
        return;
    }

    // Calculate real ground speed from actual displacement.
    // This remains valid even before we introduce a custom movement component.
    const FVector CurrentLocation = GetActorLocation();

    if (DeltaSeconds > KINDA_SMALL_NUMBER)
    {
        CurrentGroundSpeed =
            FVector::Dist2D(CurrentLocation, PreviousActorLocation) / DeltaSeconds;
    }
    else
    {
        CurrentGroundSpeed = 0.0f;
    }

    PreviousActorLocation = CurrentLocation;

    float TargetArmLength = FixedCameraArmLength;
    float TargetOrthoWidth = FixedOrthoWidth;

    if (CameraMode == EIronGridCameraMode::SpeedReactive)
    {
        const float SpeedAlpha =
            FMath::Clamp(CurrentGroundSpeed / FMath::Max(SpeedForMaxCameraZoom, 1.0f), 0.0f, 1.0f);

        TargetArmLength =
            FMath::Lerp(NearCameraArmLength, FarCameraArmLength, SpeedAlpha);

        TargetOrthoWidth =
            FMath::Lerp(NearOrthoWidth, FarOrthoWidth, SpeedAlpha);
    }

    // Perspective cameras physically move along the spring arm.
    CameraBoom->TargetArmLength =
        FMath::FInterpTo(
            CameraBoom->TargetArmLength,
            TargetArmLength,
            DeltaSeconds,
            CameraZoomInterpSpeed
        );

    // Orthographic cameras do not visually zoom when only the boom length changes,
    // so change OrthoWidth as well.
    if (TopDownCamera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        TopDownCamera->OrthoWidth =
            FMath::FInterpTo(
                TopDownCamera->OrthoWidth,
                TargetOrthoWidth,
                DeltaSeconds,
                CameraZoomInterpSpeed
            );
    }
}
