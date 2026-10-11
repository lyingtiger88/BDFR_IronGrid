#include "World/IronGridGeneratedSpriteActor.h"

#include "Components/SceneComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

AIronGridGeneratedSpriteActor::AIronGridGeneratedSpriteActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
    Sprite->SetupAttachment(Root);
    Sprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Sprite->SetCastShadow(false);

    Tags.Add(TEXT("IronGridGeneratedEnvironment"));
}

void AIronGridGeneratedSpriteActor::Configure(
    UPaperSprite* InSprite,
    const FRotator& InRotation,
    float UniformScale,
    FName InLayerTag)
{
    if (!Sprite)
    {
        return;
    }

    Sprite->SetSprite(InSprite);

    // Paper2D sprites are authored in the XZ plane by default.
    // IRON GRID is a true top-down game, so rotate generated environment art
    // into the XY ground plane. Palette Yaw remains an in-plane rotation.
    const FRotator TopDownRotation(
        InRotation.Pitch,
        InRotation.Yaw,
        InRotation.Roll + 90.0f);

    Sprite->SetRelativeRotation(TopDownRotation);
    SetActorScale3D(FVector(UniformScale));

    if (!InLayerTag.IsNone())
    {
        Tags.AddUnique(InLayerTag);
    }
}
