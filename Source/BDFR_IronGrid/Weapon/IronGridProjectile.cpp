#include "Weapon/IronGridProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundBase.h"

AIronGridProjectile::AIronGridProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);

    Collision->InitSphereRadius(8.0f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->OnComponentHit.AddDynamic(this, &AIronGridProjectile::HandleProjectileHit);

    ProjectileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ProjectileSprite"));
    ProjectileSprite->SetupAttachment(Collision);
    ProjectileSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectileSprite->SetCastShadow(false);

    DebugVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugVisual"));
    DebugVisual->SetupAttachment(Collision);
    DebugVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DebugVisual->SetCastShadow(false);
    // Deliberately oversized placeholder so the shell is easy to see
    // from the current orthographic prototype camera.
    DebugVisual->SetRelativeScale3D(FVector(0.20f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    if (SphereMesh.Succeeded())
    {
        DebugVisual->SetStaticMesh(SphereMesh.Object);
    }

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = Collision;
    ProjectileMovement->InitialSpeed = LaunchSpeed;
    ProjectileMovement->MaxSpeed = LaunchSpeed;
    ProjectileMovement->ProjectileGravityScale = GravityScale;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

void AIronGridProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(ProjectileLifeSeconds);
    PreviousDebugLocation = GetActorLocation();

    if (bHideDebugVisualWhenSpriteAssigned &&
        ProjectileSprite &&
        ProjectileSprite->GetSprite() != nullptr &&
        DebugVisual)
    {
        DebugVisual->SetVisibility(false, true);
    }

    OnProjectileLaunched();

    if (AActor* ProjectileOwner = GetOwner())
    {
        Collision->IgnoreActorWhenMoving(ProjectileOwner, true);
    }
}

void AIronGridProjectile::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

#if !(UE_BUILD_SHIPPING)
    if (bShowDebugTrail && GetWorld() && GetNetMode() != NM_DedicatedServer)
    {
        const FVector CurrentLocation = GetActorLocation();

        DrawDebugLine(
            GetWorld(),
            PreviousDebugLocation,
            CurrentLocation,
            FColor::Orange,
            false,
            DebugTrailDuration,
            0,
            4.0f);

        DrawDebugSphere(
            GetWorld(),
            CurrentLocation,
            DebugMarkerRadius,
            8,
            FColor::Yellow,
            false,
            DebugTrailDuration,
            0,
            1.5f);

        PreviousDebugLocation = CurrentLocation;
    }
#endif
}

void AIronGridProjectile::InitializeProjectile(
    float InSpeed,
    float InDamage,
    float InGravityScale)
{
    LaunchSpeed = FMath::Max(InSpeed, 1.0f);
    Damage = FMath::Max(InDamage, 0.0f);
    GravityScale = FMath::Max(InGravityScale, 0.0f);

    if (ProjectileMovement)
    {
        ProjectileMovement->InitialSpeed = LaunchSpeed;
        ProjectileMovement->MaxSpeed = LaunchSpeed;
        ProjectileMovement->ProjectileGravityScale = GravityScale;
        ProjectileMovement->Velocity = GetActorForwardVector() * LaunchSpeed;
    }
}

void AIronGridProjectile::HandleProjectileHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!HasAuthority())
    {
        return;
    }

    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        const FVector ShotDirection = GetVelocity().GetSafeNormal();

        UGameplayStatics::ApplyPointDamage(
            OtherActor,
            Damage,
            ShotDirection,
            Hit,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass());
    }

    MulticastImpactFX(Hit.ImpactPoint, Hit.ImpactNormal);
    Destroy();
}

void AIronGridProjectile::MulticastImpactFX_Implementation(
    FVector ImpactPoint,
    FVector ImpactNormal)
{
#if !(UE_BUILD_SHIPPING)
    if (bShowDebugImpactFX &&
        GetWorld() &&
        GetNetMode() != NM_DedicatedServer)
    {
        DrawDebugSphere(
            GetWorld(),
            ImpactPoint,
            28.0f,
            12,
            FColor::Red,
            false,
            0.35f,
            0,
            3.0f);

        DrawDebugDirectionalArrow(
            GetWorld(),
            ImpactPoint,
            ImpactPoint + ImpactNormal * 90.0f,
            24.0f,
            FColor::Orange,
            false,
            0.35f,
            0,
            3.0f);
    }
#endif

    if (ImpactSound && GetWorld())
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ImpactSound,
            ImpactPoint,
            ImpactSoundVolume);
    }

    OnImpactFX(ImpactPoint, ImpactNormal);
}
