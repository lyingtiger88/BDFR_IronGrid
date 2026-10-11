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
    Sprite->SetRelativeRotation(InRotation);
    SetActorScale3D(FVector(UniformScale));

    if (!InLayerTag.IsNone())
    {
        Tags.AddUnique(InLayerTag);
    }
}
