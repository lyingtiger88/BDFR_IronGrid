#include "PowerUp/IronGridPowerUp.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "PaperSpriteComponent.h"
#include "Tank/IronGridTankPawn.h"
#include "UObject/ConstructorHelpers.h"

AIronGridPowerUp::AIronGridPowerUp()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(false);

    PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
    SetRootComponent(PickupCollision);
    PickupCollision->InitSphereRadius(60.0f);
    PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
    PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    PickupCollision->OnComponentBeginOverlap.AddDynamic(
        this,
        &AIronGridPowerUp::HandlePickupOverlap);

    VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
    VisualRoot->SetupAttachment(PickupCollision);

    PowerUpSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PowerUpSprite"));
    PowerUpSprite->SetupAttachment(VisualRoot);
    PowerUpSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PowerUpSprite->SetCastShadow(false);

    DebugVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugVisual"));
    DebugVisual->SetupAttachment(VisualRoot);
    DebugVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DebugVisual->SetCastShadow(false);
    DebugVisual->SetRelativeScale3D(FVector(0.35f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    if (SphereMesh.Succeeded())
    {
        DebugVisual->SetStaticMesh(SphereMesh.Object);
    }
}

void AIronGridPowerUp::BeginPlay()
{
    Super::BeginPlay();

    if (bHideDebugVisualWhenSpriteAssigned &&
        PowerUpSprite &&
        PowerUpSprite->GetSprite() != nullptr &&
        DebugVisual)
    {
        DebugVisual->SetVisibility(false, true);
    }

    if (SpawnSound && GetNetMode() != NM_DedicatedServer)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            SpawnSound,
            GetActorLocation(),
            SpawnSoundVolume);
    }

    OnPowerUpSpawned();
}

void AIronGridPowerUp::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bAnimateVisual || !VisualRoot)
    {
        return;
    }

    AnimationTime += DeltaSeconds;

    const float BobZ =
        FMath::Sin(AnimationTime * BobSpeed) *
        BobAmplitude;

    VisualRoot->SetRelativeLocation(
        FVector(0.0f, 0.0f, BobZ));

    VisualRoot->AddLocalRotation(
        FRotator(
            0.0f,
            RotationSpeed * DeltaSeconds,
            0.0f));
}

void AIronGridPowerUp::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!HasAuthority())
    {
        return;
    }

    AIronGridTankPawn* Tank =
        Cast<AIronGridTankPawn>(OtherActor);

    if (!Tank)
    {
        return;
    }

    ApplyPowerUp(Tank);

    PickupCollision->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    MulticastPickupFeedback();
    OnPowerUpPickedUp(Tank);

    SetActorHiddenInGame(true);
    SetLifeSpan(0.25f);
}

void AIronGridPowerUp::ApplyPowerUp(
    AIronGridTankPawn* Tank)
{
    if (!Tank)
    {
        return;
    }

    switch (PowerUpType)
    {
        case EIronGridPowerUpType::Ammo:
            Tank->AddReserveAmmo(AmmoAmount);
            break;

        case EIronGridPowerUpType::SpeedBoost:
            Tank->ApplySpeedBoost(
                SpeedMultiplier,
                EffectDuration);
            break;

        case EIronGridPowerUpType::ReloadBoost:
            Tank->ApplyReloadBoost(
                ReloadTimeMultiplier,
                EffectDuration);
            break;

        case EIronGridPowerUpType::Repair:
            Tank->ApplyRepairPowerUp(
                RepairAmount);
            break;

        default:
            break;
    }
}

void AIronGridPowerUp::MulticastPickupFeedback_Implementation()
{
    if (PickupSound && GetNetMode() != NM_DedicatedServer)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            PickupSound,
            GetActorLocation(),
            PickupSoundVolume);
    }
}
