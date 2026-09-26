#include "Tank/IronGridTankPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperSpriteComponent.h"

AIronGridTankPawn::AIronGridTankPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    TankRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TankRoot"));
    SetRootComponent(TankRoot);

    HullSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("HullSprite"));
    HullSprite->SetupAttachment(TankRoot);

    TurretPivot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretPivot"));
    TurretPivot->SetupAttachment(TankRoot);

    TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
    TurretSprite->SetupAttachment(TurretPivot);

    Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(TurretPivot);
    Muzzle->SetRelativeLocation(FVector(90.0f, 0.0f, 0.0f));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(TankRoot);
    CameraBoom->TargetArmLength = 1800.0f;
    CameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bUsePawnControlRotation = false;

    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
    TopDownCamera->OrthoWidth = 2600.0f;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AIronGridTankPawn::BeginPlay()
{
    Super::BeginPlay();

    DesiredAimWorldPoint = GetActorLocation() + GetActorForwardVector() * 1000.0f;
}

void AIronGridTankPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!FMath::IsNearlyZero(TurnInput))
    {
        AddActorLocalRotation(FRotator(0.0f, TurnInput * HullTurnSpeed * DeltaSeconds, 0.0f));
    }

    if (!FMath::IsNearlyZero(MoveInput))
    {
        const FVector Delta = GetActorForwardVector() * MoveInput * MaxMoveSpeed * DeltaSeconds;
        AddActorWorldOffset(Delta, true);
    }

    UpdateTurret(DeltaSeconds);
}

void AIronGridTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AIronGridTankPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("TurnHull"), this, &AIronGridTankPawn::TurnHull);
}

void AIronGridTankPawn::SetDesiredAimPoint(const FVector& WorldPoint)
{
    DesiredAimWorldPoint = WorldPoint;
}

FVector AIronGridTankPawn::GetActualGunAimPoint() const
{
    if (!Muzzle || !GetWorld())
    {
        return GetActorLocation();
    }

    const FVector Start = Muzzle->GetComponentLocation();
    const FVector End = Start + Muzzle->GetForwardVector() * AimTraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(IronGridGunAim), false, this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return Hit.ImpactPoint;
    }

    return End;
}

float AIronGridTankPawn::GetAimErrorDegrees() const
{
    const FVector ToDesired = (DesiredAimWorldPoint - TurretPivot->GetComponentLocation()).GetSafeNormal2D();
    const FVector Actual = TurretPivot->GetForwardVector().GetSafeNormal2D();

    const float Dot = FMath::Clamp(FVector::DotProduct(ToDesired, Actual), -1.0f, 1.0f);
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
    const float TargetLocalYaw = FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, DesiredWorldYaw);
    const float CurrentLocalYaw = FRotator::NormalizeAxis(TurretPivot->GetRelativeRotation().Yaw);
    const float MaxStep = TurretTraverseSpeed * DeltaSeconds;
    const float NewLocalYaw = FMath::FixedTurn(CurrentLocalYaw, TargetLocalYaw, MaxStep);

    TurretPivot->SetRelativeRotation(FRotator(0.0f, NewLocalYaw, 0.0f));
}
