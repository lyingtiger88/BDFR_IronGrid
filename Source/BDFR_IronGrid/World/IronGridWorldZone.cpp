#include "World/IronGridWorldZone.h"

#include "Components/BoxComponent.h"

AIronGridWorldZone::AIronGridWorldZone()
{
    PrimaryActorTick.bCanEverTick = false;

    ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBounds"));
    SetRootComponent(ZoneBounds);

    ZoneBounds->SetBoxExtent(FVector(750.0f, 750.0f, 200.0f));
    ZoneBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ZoneBounds->SetGenerateOverlapEvents(false);
    ZoneBounds->SetHiddenInGame(true);
    ZoneBounds->ShapeColor = FColor(255, 185, 40);
}

bool AIronGridWorldZone::ContainsWorldPoint(const FVector& WorldPoint) const
{
    if (!bEnabled || !ZoneBounds)
    {
        return false;
    }

    const FVector LocalPoint =
        ZoneBounds->GetComponentTransform().InverseTransformPosition(WorldPoint);

    const FVector Extent =
        ZoneBounds->GetUnscaledBoxExtent();

    return
        FMath::Abs(LocalPoint.X) <= Extent.X &&
        FMath::Abs(LocalPoint.Y) <= Extent.Y;
}
