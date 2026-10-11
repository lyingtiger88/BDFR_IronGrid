#include "World/IronGridWorldBuilder.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Math/RandomStream.h"
#include "World/IronGridEnvironmentProfile.h"

AIronGridWorldBuilder::AIronGridWorldBuilder()
{
    PrimaryActorTick.bCanEverTick = false;

    MapBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("MapBounds"));
    SetRootComponent(MapBounds);

    MapBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MapBounds->SetGenerateOverlapEvents(false);
    MapBounds->SetHiddenInGame(true);
    MapBounds->ShapeColor = FColor(60, 200, 255);
}

void AIronGridWorldBuilder::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    UpdateBounds();
}

void AIronGridWorldBuilder::UpdateBounds()
{
    if (!MapBounds)
    {
        return;
    }

    const FVector Extent(
        FMath::Max(MapSize.X * 0.5f, 50.0f),
        FMath::Max(MapSize.Y * 0.5f, 50.0f),
        100.0f);

    MapBounds->SetBoxExtent(Extent);
}

void AIronGridWorldBuilder::GeneratePreview()
{
    ClearPreview();
    UpdateBounds();

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FRandomStream RandomStream(Seed);

    const float SafeCellSize = FMath::Max(PreviewCellSize, 100.0f);
    const int32 CellsX = FMath::Max(1, FMath::FloorToInt(MapSize.X / SafeCellSize));
    const int32 CellsY = FMath::Max(1, FMath::FloorToInt(MapSize.Y / SafeCellSize));

    const FVector Origin = GetActorLocation();
    const FVector2D HalfSize = MapSize * 0.5f;

    GeneratedSamplePoints.Reserve(CellsX * CellsY);

    for (int32 X = 0; X < CellsX; ++X)
    {
        for (int32 Y = 0; Y < CellsY; ++Y)
        {
            const float MinX = -HalfSize.X + X * SafeCellSize;
            const float MinY = -HalfSize.Y + Y * SafeCellSize;

            const float JitterX = RandomStream.FRandRange(0.15f, 0.85f) * SafeCellSize;
            const float JitterY = RandomStream.FRandRange(0.15f, 0.85f) * SafeCellSize;

            GeneratedSamplePoints.Add(
                Origin + FVector(MinX + JitterX, MinY + JitterY, 25.0f));
        }
    }

    DrawPreview();

#if WITH_EDITOR
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            4.0f,
            FColor::Green,
            FString::Printf(
                TEXT("IRON GRID WORLD PREVIEW | Seed:%d | Cells:%d x %d | Samples:%d"),
                Seed,
                CellsX,
                CellsY,
                GeneratedSamplePoints.Num()));
    }
#endif
}

void AIronGridWorldBuilder::ClearPreview()
{
    GeneratedSamplePoints.Reset();

    if (UWorld* World = GetWorld())
    {
        FlushPersistentDebugLines(World);
    }
}

void AIronGridWorldBuilder::RandomizeSeed()
{
    Seed = FMath::RandRange(1, MAX_int32);
    GeneratePreview();
}

void AIronGridWorldBuilder::ValidateBuilder()
{
#if WITH_EDITOR
    if (!GEngine)
    {
        return;
    }

    FString Message = TEXT("IRON GRID WORLD BUILDER: OK");
    FColor MessageColor = FColor::Green;

    if (MapSize.X < 1000.0f || MapSize.Y < 1000.0f)
    {
        Message = TEXT("WORLD BUILDER WARNING: MapSize is too small.");
        MessageColor = FColor::Yellow;
    }
    else if (PreviewCellSize <= 0.0f)
    {
        Message = TEXT("WORLD BUILDER ERROR: PreviewCellSize must be > 0.");
        MessageColor = FColor::Red;
    }
    else if (!EnvironmentProfile)
    {
        Message = TEXT("WORLD BUILDER: Core settings OK. EnvironmentProfile not assigned yet.");
        MessageColor = FColor::Yellow;
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, MessageColor, Message);
#endif
}

void AIronGridWorldBuilder::DrawPreview()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const FVector Origin = GetActorLocation();
    const FVector2D HalfSize = MapSize * 0.5f;
    const float SafeCellSize = FMath::Max(PreviewCellSize, 100.0f);

    if (bShowPreviewGrid)
    {
        const int32 CellsX = FMath::Max(1, FMath::FloorToInt(MapSize.X / SafeCellSize));
        const int32 CellsY = FMath::Max(1, FMath::FloorToInt(MapSize.Y / SafeCellSize));

        for (int32 X = 0; X <= CellsX; ++X)
        {
            const float LocalX = -HalfSize.X + X * SafeCellSize;
            DrawDebugLine(
                World,
                Origin + FVector(LocalX, -HalfSize.Y, 10.0f),
                Origin + FVector(LocalX, HalfSize.Y, 10.0f),
                FColor(80, 80, 80),
                true,
                -1.0f,
                0,
                1.0f);
        }

        for (int32 Y = 0; Y <= CellsY; ++Y)
        {
            const float LocalY = -HalfSize.Y + Y * SafeCellSize;
            DrawDebugLine(
                World,
                Origin + FVector(-HalfSize.X, LocalY, 10.0f),
                Origin + FVector(HalfSize.X, LocalY, 10.0f),
                FColor(80, 80, 80),
                true,
                -1.0f,
                0,
                1.0f);
        }
    }

    if (bShowSamplePoints)
    {
        for (const FVector& Point : GeneratedSamplePoints)
        {
            DrawDebugPoint(
                World,
                Point,
                SamplePointSize,
                FColor::Emerald,
                true,
                -1.0f,
                0);
        }
    }
}
