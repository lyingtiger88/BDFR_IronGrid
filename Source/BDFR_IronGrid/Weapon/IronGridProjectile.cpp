#include "Weapon/IronGridProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AIronGridProjectile::AIronGridProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

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

    DebugVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugVisual"));
    DebugVisual->SetupAttachment(Collision);
    DebugVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DebugVisual->SetCastShadow(false);
    DebugVisual->SetRelativeScale3D(FVector(0.08f));

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

    if (AActor* ProjectileOwner = GetOwner())
    {
        Collision->IgnoreActorWhenMoving(ProjectileOwner, true);
    }
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
    OnImpactFX(ImpactPoint, ImpactNormal);
}
