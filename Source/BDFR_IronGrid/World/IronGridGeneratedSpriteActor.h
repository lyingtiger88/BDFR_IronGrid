#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IronGridGeneratedSpriteActor.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
class USceneComponent;

UCLASS()
class BDFR_IRONGRID_API AIronGridGeneratedSpriteActor : public AActor
{
    GENERATED_BODY()

public:
    AIronGridGeneratedSpriteActor();

    void Configure(
        UPaperSprite* InSprite,
        const FRotator& InRotation,
        float UniformScale,
        FName InLayerTag);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Generated Environment")
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IronGrid|Generated Environment")
    TObjectPtr<UPaperSpriteComponent> Sprite;
};
