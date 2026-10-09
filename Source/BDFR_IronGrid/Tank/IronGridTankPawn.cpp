#include "Tank/IronGridTankPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PaperSpriteComponent.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "Weapon/IronGridProjectile.h"

AIronGridTankPawn::AIronGridTankPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    // Tank movement replication is intentionally deferred to the dedicated
    // movement-networking milestone. The pawn must replicate for weapon RPCs,
    // but its transform is still controlled by the current local prototype.
    SetReplicateMovement(false);

    ProjectileClass = AIronGridProjectile::StaticClass();

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

    EngineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
    EngineAudioComponent->SetupAttachment(TankRoot);
    EngineAudioComponent->bAutoActivate = false;

    TrackAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TrackAudio"));
    TrackAudioComponent->SetupAttachment(TankRoot);
    TrackAudioComponent->bAutoActivate = false;

    TurretAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TurretAudio"));
    TurretAudioComponent->SetupAttachment(TurretPivot);
    TurretAudioComponent->bAutoActivate = false;

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

    if (HasAuthority())
    {
        CurrentAmmoInMagazine = FMath::Max(MagazineSize, 1);
        ReserveAmmo = FMath::Max(StartingReserveAmmo, 0);
        bReloading = false;
    }

    InitializeAudioComponents();

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
        TurretVisualRoot->SetRelativeLocation(FVector::ZeroVector);
    }
}

void AIronGridTankPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateTrackedMovement(DeltaSeconds);
    UpdateTurret(DeltaSeconds);
    UpdateWeaponFeedback(DeltaSeconds);
    UpdateAudio(DeltaSeconds);
    UpdateCamera(DeltaSeconds);
}

void AIronGridTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AIronGridTankPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("TurnHull"), this, &AIronGridTankPawn::TurnHull);
    PlayerInputComponent->BindAction(TEXT("Brake"), IE_Pressed, this, &AIronGridTankPawn::BrakePressed);
    PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &AIronGridTankPawn::BrakeReleased);
    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AIronGridTankPawn::FireWeapon);
    PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AIronGridTankPawn::ReloadWeapon);
    PlayerInputComponent->BindAction(TEXT("TestAudio"), IE_Pressed, this, &AIronGridTankPawn::TestAssignedAudio);
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

void AIronGridTankPawn::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AIronGridTankPawn, CurrentAmmoInMagazine);
    DOREPLIFETIME(AIronGridTankPawn, ReserveAmmo);
    DOREPLIFETIME(AIronGridTankPawn, bReloading);
    DOREPLIFETIME(AIronGridTankPawn, ActiveSpeedMultiplier);
    DOREPLIFETIME(AIronGridTankPawn, ActiveReloadMultiplier);
}

FVector AIronGridTankPawn::GetPredictedBallisticImpactPoint() const
{
    if (!Muzzle || !GetWorld())
    {
        return GetActorLocation();
    }

    FPredictProjectilePathParams Params;
    Params.StartLocation =
        Muzzle->GetComponentLocation() +
        FVector(0.0f, 0.0f, ProjectileSpawnHeight);

    Params.LaunchVelocity =
        Muzzle->GetForwardVector() *
        MuzzleVelocity;

    Params.bTraceWithCollision = true;
    Params.ProjectileRadius = BallisticPredictionRadius;
    Params.MaxSimTime = BallisticPredictionTime;
    Params.SimFrequency = 20.0f;
    Params.TraceChannel = ECC_Visibility;
    Params.OverrideGravityZ =
        GetWorld()->GetGravityZ() *
        ProjectileGravityScale;

    Params.ActorsToIgnore.Add(
        const_cast<AIronGridTankPawn*>(this));

    FPredictProjectilePathResult Result;
    UGameplayStatics::PredictProjectilePath(
        this,
        Params,
        Result);

    if (Result.HitResult.bBlockingHit)
    {
        return Result.HitResult.ImpactPoint;
    }

    if (Result.PathData.Num() > 0)
    {
        return Result.PathData.Last().Location;
    }

    return Params.StartLocation;
}

void AIronGridTankPawn::FireWeapon()
{
    if (HasAuthority())
    {
        PerformFire();
    }
    else
    {
        ServerFireWeapon();
    }
}

void AIronGridTankPawn::ServerFireWeapon_Implementation()
{
    PerformFire();
}

void AIronGridTankPawn::ReloadWeapon()
{
    if (HasAuthority())
    {
        StartReload();
    }
    else
    {
        ServerReloadWeapon();
    }
}

void AIronGridTankPawn::ServerReloadWeapon_Implementation()
{
    StartReload();
}

void AIronGridTankPawn::PerformFire()
{
    if (!GetWorld() || !Muzzle || bReloading)
    {
        return;
    }

    const float CurrentTime = GetWorld()->GetTimeSeconds();

    if ((CurrentTime - LastFireTime) < FireInterval)
    {
        return;
    }

    if (CurrentAmmoInMagazine <= 0)
    {
        StartReload();
        return;
    }

    const FVector SpawnLocation =
        Muzzle->GetComponentLocation() +
        FVector(0.0f, 0.0f, ProjectileSpawnHeight);

    const FRotator SpawnRotation =
        Muzzle->GetComponentRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Blueprint may explicitly have ProjectileClass set to None.
    // Always fall back to the native test projectile so firing remains testable.
    TSubclassOf<AIronGridProjectile> ClassToSpawn = ProjectileClass;

    if (!ClassToSpawn)
    {
        ClassToSpawn = AIronGridProjectile::StaticClass();
    }

    AIronGridProjectile* Projectile =
        GetWorld()->SpawnActor<AIronGridProjectile>(
            ClassToSpawn,
            SpawnLocation,
            SpawnRotation,
            SpawnParams);

    if (!Projectile)
    {
#if !(UE_BUILD_SHIPPING)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Red,
                TEXT("IRON GRID: PROJECTILE SPAWN FAILED"));
        }
#endif
        return;
    }

#if !(UE_BUILD_SHIPPING)
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            1.0f,
            FColor::Green,
            TEXT("IRON GRID: SHELL FIRED"));
    }
#endif

    Projectile->InitializeProjectile(
        MuzzleVelocity,
        ProjectileDamage,
        ProjectileGravityScale);

    --CurrentAmmoInMagazine;
    LastFireTime = CurrentTime;

    MulticastMuzzleFX();

    if (CurrentAmmoInMagazine <= 0 && ReserveAmmo > 0)
    {
        StartReload();
    }
}

void AIronGridTankPawn::StartReload()
{
    if (!GetWorld() ||
        bReloading ||
        CurrentAmmoInMagazine >= MagazineSize ||
        ReserveAmmo <= 0)
    {
        return;
    }

    bReloading = true;
    MulticastReloadStartedAudio();
    OnReloadStarted();

    GetWorldTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &AIronGridTankPawn::CompleteReload,
        FMath::Max(ReloadDuration * ActiveReloadMultiplier, 0.15f),
        false);
}

void AIronGridTankPawn::CompleteReload()
{
    const int32 NeededAmmo =
        FMath::Max(
            MagazineSize - CurrentAmmoInMagazine,
            0);

    const int32 AmmoToLoad =
        FMath::Min(
            NeededAmmo,
            ReserveAmmo);

    CurrentAmmoInMagazine += AmmoToLoad;
    ReserveAmmo -= AmmoToLoad;
    bReloading = false;

    MulticastReloadFinishedAudio();
    OnReloadFinished();
}

void AIronGridTankPawn::MulticastMuzzleFX_Implementation()
{
    if (Muzzle)
    {
        FVector RecoilDirection = -Muzzle->GetForwardVector();
        RecoilDirection.Z = 0.0f;
        RecoilDirection.Normalize();

        CurrentRecoilVelocity =
            RecoilDirection * TankRecoilSpeed;
    }

    CurrentCameraRecoil = FMath::Max(CurrentCameraRecoil, CameraRecoilKick);

#if !(UE_BUILD_SHIPPING)
    if (bShowDebugMuzzleFX &&
        Muzzle &&
        GetWorld() &&
        GetNetMode() != NM_DedicatedServer)
    {
        const FVector MuzzleLocation = Muzzle->GetComponentLocation();
        const FVector MuzzleForward = Muzzle->GetForwardVector();

        DrawDebugSphere(
            GetWorld(),
            MuzzleLocation,
            30.0f,
            12,
            FColor::Yellow,
            false,
            DebugMuzzleFXDuration,
            0,
            3.0f);

        DrawDebugDirectionalArrow(
            GetWorld(),
            MuzzleLocation,
            MuzzleLocation + MuzzleForward * 140.0f,
            35.0f,
            FColor::Orange,
            false,
            DebugMuzzleFXDuration,
            0,
            5.0f);
    }
#endif

    PlayTankOneShot(
        CannonFireSound,
        Muzzle ? Muzzle->GetComponentLocation() : GetActorLocation(),
        TEXT("CannonFireSound"));

    OnWeaponFired();
}

void AIronGridTankPawn::UpdateWeaponFeedback(float DeltaSeconds)
{
    if (!CurrentRecoilVelocity.IsNearlyZero())
    {
        const FVector RecoilDelta =
            CurrentRecoilVelocity * DeltaSeconds;

        FHitResult RecoilHit;
        AddActorWorldOffset(
            RecoilDelta,
            true,
            &RecoilHit);

        if (RecoilHit.bBlockingHit)
        {
            CurrentRecoilVelocity *= 0.20f;
        }

        CurrentRecoilVelocity =
            FMath::VInterpTo(
                CurrentRecoilVelocity,
                FVector::ZeroVector,
                DeltaSeconds,
                TankRecoilDamping);
    }

    CurrentCameraRecoil =
        FMath::FInterpTo(
            CurrentCameraRecoil,
            0.0f,
            DeltaSeconds,
            CameraRecoilReturnSpeed);
}

void AIronGridTankPawn::AddReserveAmmo(int32 Amount)
{
    if (!HasAuthority() || Amount <= 0)
    {
        return;
    }

    ReserveAmmo += Amount;
}

void AIronGridTankPawn::ApplySpeedBoost(float Multiplier, float Duration)
{
    if (!HasAuthority())
    {
        return;
    }

    ActiveSpeedMultiplier = FMath::Max(Multiplier, 1.0f);

    GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);

    if (Duration > 0.0f)
    {
        GetWorldTimerManager().SetTimer(
            SpeedBoostTimerHandle,
            this,
            &AIronGridTankPawn::ClearSpeedBoost,
            Duration,
            false);
    }
}

void AIronGridTankPawn::ApplyReloadBoost(float ReloadTimeMultiplier, float Duration)
{
    if (!HasAuthority())
    {
        return;
    }

    ActiveReloadMultiplier =
        FMath::Clamp(ReloadTimeMultiplier, 0.20f, 1.0f);

    GetWorldTimerManager().ClearTimer(ReloadBoostTimerHandle);

    if (Duration > 0.0f)
    {
        GetWorldTimerManager().SetTimer(
            ReloadBoostTimerHandle,
            this,
            &AIronGridTankPawn::ClearReloadBoost,
            Duration,
            false);
    }
}

void AIronGridTankPawn::ApplyRepairPowerUp(float RepairAmount)
{
    if (!HasAuthority() || RepairAmount <= 0.0f)
    {
        return;
    }

    OnRepairPowerUp(RepairAmount);
}

void AIronGridTankPawn::ClearSpeedBoost()
{
    ActiveSpeedMultiplier = 1.0f;
}

void AIronGridTankPawn::ClearReloadBoost()
{
    ActiveReloadMultiplier = 1.0f;
}

void AIronGridTankPawn::MulticastReloadStartedAudio_Implementation()
{
    PlayTankOneShot(
        ReloadStartSound,
        GetActorLocation(),
        TEXT("ReloadStartSound"));
}

void AIronGridTankPawn::MulticastReloadFinishedAudio_Implementation()
{
    PlayTankOneShot(
        ReloadCompleteSound,
        GetActorLocation(),
        TEXT("ReloadCompleteSound"));
}

void AIronGridTankPawn::InitializeAudioComponents()
{
    // Blueprint authors can assign loop sounds either through the dedicated
    // IronGrid|Audio properties or directly on the inherited AudioComponents.
    // Never overwrite a component-assigned sound with nullptr.
    if (EngineAudioComponent)
    {
        if (!EngineLoopSound)
        {
            EngineLoopSound = EngineAudioComponent->Sound;
        }
        else
        {
            EngineAudioComponent->SetSound(EngineLoopSound);
        }

        EngineAudioComponent->SetVolumeMultiplier(EngineVolume);
        EngineAudioComponent->SetPitchMultiplier(EngineIdlePitch);

        if (EngineLoopSound)
        {
            EngineAudioComponent->Play();
        }
    }

    if (TrackAudioComponent)
    {
        if (!TrackLoopSound)
        {
            TrackLoopSound = TrackAudioComponent->Sound;
        }
        else
        {
            TrackAudioComponent->SetSound(TrackLoopSound);
        }

        TrackAudioComponent->SetVolumeMultiplier(0.0f);
        TrackAudioComponent->SetPitchMultiplier(TrackMinPitch);

        if (TrackLoopSound)
        {
            TrackAudioComponent->Play();
        }
    }

    if (TurretAudioComponent)
    {
        if (!TurretLoopSound)
        {
            TurretLoopSound = TurretAudioComponent->Sound;
        }
        else
        {
            TurretAudioComponent->SetSound(TurretLoopSound);
        }

        TurretAudioComponent->SetVolumeMultiplier(0.0f);

        if (TurretLoopSound)
        {
            TurretAudioComponent->Play();
        }
    }

#if !(UE_BUILD_SHIPPING)
    if (bShowAudioDebugMessages && IsLocallyControlled() && GEngine)
    {
        const FString AudioState = FString::Printf(
            TEXT("PAWN:%s | AUDIO Engine:%s Tracks:%s Turret:%s Cannon:%s Reload:%s/%s"),
            *GetClass()->GetPathName(),
            EngineLoopSound ? TEXT("OK") : TEXT("NONE"),
            TrackLoopSound ? TEXT("OK") : TEXT("NONE"),
            TurretLoopSound ? TEXT("OK") : TEXT("NONE"),
            CannonFireSound ? TEXT("OK") : TEXT("NONE"),
            ReloadStartSound ? TEXT("OK") : TEXT("NONE"),
            ReloadCompleteSound ? TEXT("OK") : TEXT("NONE"));

        GEngine->AddOnScreenDebugMessage(
            -1,
            10.0f,
            FColor::Cyan,
            AudioState);
    }
#endif
}

void AIronGridTankPawn::PlayTankOneShot(
    USoundBase* Sound,
    const FVector& WorldLocation,
    const TCHAR* DebugLabel)
{
    if (!Sound || !GetWorld() || GetNetMode() == NM_DedicatedServer)
    {
#if !(UE_BUILD_SHIPPING)
        if (bShowAudioDebugMessages && IsLocallyControlled() && GEngine && !Sound)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Red,
                FString::Printf(TEXT("AUDIO MISSING: %s"), DebugLabel));
        }
#endif
        return;
    }

    if (IsLocallyControlled() && bLocalTankOneShotsAs2D)
    {
        UGameplayStatics::PlaySound2D(this, Sound);
    }
    else
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            Sound,
            WorldLocation);
    }

#if !(UE_BUILD_SHIPPING)
    if (bShowAudioDebugMessages && IsLocallyControlled() && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            1.25f,
            FColor::Green,
            FString::Printf(TEXT("AUDIO PLAY: %s"), DebugLabel));
    }
#endif
}

void AIronGridTankPawn::TestAssignedAudio()
{
    struct FAudioTestEntry
    {
        USoundBase* Sound;
        const TCHAR* Label;
    };

    const FAudioTestEntry Tests[] =
    {
        { EngineLoopSound, TEXT("EngineLoopSound") },
        { TrackLoopSound, TEXT("TrackLoopSound") },
        { TurretLoopSound, TEXT("TurretLoopSound") },
        { CannonFireSound, TEXT("CannonFireSound") },
        { ReloadStartSound, TEXT("ReloadStartSound") },
        { ReloadCompleteSound, TEXT("ReloadCompleteSound") }
    };

    constexpr int32 TestCount = UE_ARRAY_COUNT(Tests);
    const int32 Index = AudioTestIndex % TestCount;
    AudioTestIndex = (AudioTestIndex + 1) % TestCount;

    PlayTankOneShot(
        Tests[Index].Sound,
        GetActorLocation(),
        Tests[Index].Label);
}

void AIronGridTankPawn::UpdateAudio(float DeltaSeconds)
{
    const float EffectiveMaxSpeed =
        FMath::Max(MaxMoveSpeed * ActiveSpeedMultiplier, 1.0f);

    const float TrackActivity =
        FMath::Clamp(
            FMath::Max(
                FMath::Abs(CurrentLeftTrackSpeed),
                FMath::Abs(CurrentRightTrackSpeed)) /
            EffectiveMaxSpeed,
            0.0f,
            1.0f);

    const float ForwardSpeedAlpha =
        FMath::Clamp(
            FMath::Abs(CurrentForwardSpeed) /
            EffectiveMaxSpeed,
            0.0f,
            1.0f);

    if (EngineAudioComponent && EngineLoopSound)
    {
        if (!EngineAudioComponent->IsPlaying())
        {
            EngineAudioComponent->SetSound(EngineLoopSound);
            EngineAudioComponent->Play();
        }

        EngineAudioComponent->SetPitchMultiplier(
            FMath::Lerp(
                EngineIdlePitch,
                EngineMaxPitch,
                FMath::Max(TrackActivity, ForwardSpeedAlpha)));

        EngineAudioComponent->SetVolumeMultiplier(EngineVolume);
    }

    if (TrackAudioComponent && TrackLoopSound)
    {
        if (!TrackAudioComponent->IsPlaying())
        {
            TrackAudioComponent->SetSound(TrackLoopSound);
            TrackAudioComponent->Play();
        }

        TrackAudioComponent->SetPitchMultiplier(
            FMath::Lerp(
                TrackMinPitch,
                TrackMaxPitch,
                TrackActivity));

        TrackAudioComponent->SetVolumeMultiplier(
            TrackVolume * TrackActivity);
    }

    if (TurretAudioComponent && TurretLoopSound)
    {
        if (!TurretAudioComponent->IsPlaying())
        {
            TurretAudioComponent->SetSound(TurretLoopSound);
            TurretAudioComponent->Play();
        }

        const float TurretError = GetAimErrorDegrees();
        const bool bTurretMoving =
            TurretError > TurretAudioErrorThreshold;

        TurretAudioComponent->SetVolumeMultiplier(
            bTurretMoving ? TurretVolume : 0.0f);
    }
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
        const float EffectiveMaxMoveSpeed =
            MaxMoveSpeed * ActiveSpeedMultiplier;

        const float EffectiveMaxReverseSpeed =
            MaxReverseSpeed * ActiveSpeedMultiplier;

        const float BaseTrackSpeed =
            Throttle >= 0.0f
                ? Throttle * EffectiveMaxMoveSpeed
                : Throttle * EffectiveMaxReverseSpeed;

        const float SpeedReference =
            FMath::Max(EffectiveMaxMoveSpeed, 1.0f);

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
            EffectiveMaxMoveSpeed *
            MovingSteeringStrength *
            SteeringScale;

        DesiredLeftTrackSpeed =
            BaseTrackSpeed + SteeringDifferential;

        DesiredRightTrackSpeed =
            BaseTrackSpeed - SteeringDifferential;

        DesiredLeftTrackSpeed =
            FMath::Clamp(
                DesiredLeftTrackSpeed,
                -EffectiveMaxReverseSpeed,
                EffectiveMaxMoveSpeed);

        DesiredRightTrackSpeed =
            FMath::Clamp(
                DesiredRightTrackSpeed,
                -EffectiveMaxReverseSpeed,
                EffectiveMaxMoveSpeed);
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

    // Add a small fire pulse. Perspective cameras physically move along the spring arm.
    TargetArmLength += CurrentCameraRecoil * 0.35f;
    TargetOrthoWidth += CurrentCameraRecoil;

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
