#include "World/IronGridWorldBuilder.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Math/RandomStream.h"
#include "PaperSprite.h"
#include "UObject/SoftObjectPath.h"
#include "World/IronGridEnvironmentPalette.h"
#include "World/IronGridEnvironmentProfile.h"
#include "World/IronGridGeneratedSpriteActor.h"
#include "World/IronGridWorldZone.h"

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

    MapBounds->SetBoxExtent(
        FVector(
            FMath::Max(MapSize.X * 0.5f, 50.0f),
            FMath::Max(MapSize.Y * 0.5f, 50.0f),
            100.0f));
}

void AIronGridWorldBuilder::GeneratePreview()
{
    ResolveDefaultDataAssetsIfNeeded();
    ClearPreview();
    UpdateBounds();
    BuildBiomeMap();
    DrawPreview();

#if WITH_EDITOR
    if (GEngine)
    {
        int32 DenseCount = 0;
        int32 LightCount = 0;
        int32 ClearingCount = 0;
        int32 RoadCount = 0;
        int32 WaterCount = 0;

        for (const FIronGridBiomeCell& Cell : GeneratedCells)
        {
            switch (Cell.Biome)
            {
                case EIronGridBiomeType::DenseForest: ++DenseCount; break;
                case EIronGridBiomeType::LightForest: ++LightCount; break;
                case EIronGridBiomeType::Clearing: ++ClearingCount; break;
                case EIronGridBiomeType::Road: ++RoadCount; break;
                case EIronGridBiomeType::Water: ++WaterCount; break;
                default: break;
            }
        }

        GEngine->AddOnScreenDebugMessage(
            -1,
            6.0f,
            FColor::Green,
            FString::Printf(
                TEXT("WORLD PREVIEW | Seed:%d | Dense:%d Light:%d Clear:%d Road:%d Water:%d"),
                Seed,
                DenseCount,
                LightCount,
                ClearingCount,
                RoadCount,
                WaterCount));
    }
#endif
}

void AIronGridWorldBuilder::BuildBiomeMap()
{
    GeneratedCells.Reset();

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float SafeCellSize = FMath::Max(PreviewCellSize, 100.0f);
    const int32 CellsX = FMath::Max(1, FMath::FloorToInt(MapSize.X / SafeCellSize));
    const int32 CellsY = FMath::Max(1, FMath::FloorToInt(MapSize.Y / SafeCellSize));

    const FVector Origin = GetActorLocation();
    const FVector2D HalfSize = MapSize * 0.5f;

    FRandomStream RandomStream(Seed);

    TArray<FVector2D> ClearingCenters;
    const int32 ClearingTargetCount =
        EnvironmentProfile
            ? EnvironmentProfile->ClearingCount
            : 5;

    for (int32 Index = 0; Index < ClearingTargetCount; ++Index)
    {
        ClearingCenters.Add(
            FVector2D(
                RandomStream.FRandRange(-HalfSize.X * 0.80f, HalfSize.X * 0.80f),
                RandomStream.FRandRange(-HalfSize.Y * 0.80f, HalfSize.Y * 0.80f)));
    }

    GeneratedCells.Reserve(CellsX * CellsY);

    for (int32 X = 0; X < CellsX; ++X)
    {
        for (int32 Y = 0; Y < CellsY; ++Y)
        {
            const float CenterX =
                -HalfSize.X +
                (static_cast<float>(X) + 0.5f) * SafeCellSize;

            const float CenterY =
                -HalfSize.Y +
                (static_cast<float>(Y) + 0.5f) * SafeCellSize;

            const float JitterRange = SafeCellSize * 0.18f;
            const float JitterX = RandomStream.FRandRange(-JitterRange, JitterRange);
            const float JitterY = RandomStream.FRandRange(-JitterRange, JitterRange);

            const FVector2D LocalLocation(
                CenterX + JitterX,
                CenterY + JitterY);

            FIronGridBiomeCell Cell;
            Cell.GridX = X;
            Cell.GridY = Y;
            Cell.WorldLocation =
                Origin +
                FVector(LocalLocation.X, LocalLocation.Y, 20.0f);

            if (BuilderMode == EIronGridWorldBuilderMode::Manual)
            {
                Cell.Biome = EIronGridBiomeType::Clearing;
                Cell.Density = 0.0f;
            }
            else
            {
                Cell.Biome =
                    ClassifyAutomaticBiome(
                        Cell.WorldLocation,
                        LocalLocation,
                        Cell.Density,
                        ClearingCenters);
            }

            if (BuilderMode != EIronGridWorldBuilderMode::Auto)
            {
                ApplyZoneOverride(Cell);
            }

            GeneratedCells.Add(Cell);
        }
    }
}

EIronGridBiomeType AIronGridWorldBuilder::ClassifyAutomaticBiome(
    const FVector& WorldLocation,
    const FVector2D& LocalLocation,
    float& OutDensity,
    const TArray<FVector2D>& ClearingCenters) const
{
    const float ForestDensity =
        EnvironmentProfile
            ? EnvironmentProfile->ForestDensity
            : 0.65f;

    const float ClearingRatio =
        EnvironmentProfile
            ? EnvironmentProfile->ClearingRatio
            : 0.20f;

    const float RoadDensity =
        EnvironmentProfile
            ? EnvironmentProfile->RoadDensity
            : 0.35f;

    const float WaterAmount =
        EnvironmentProfile
            ? EnvironmentProfile->WaterAmount
            : 0.20f;

    const float RoadWidth =
        (EnvironmentProfile ? EnvironmentProfile->BaseRoadWidth : 420.0f) *
        FMath::Lerp(0.70f, 1.45f, RoadDensity);

    const float RiverWidth =
        (EnvironmentProfile ? EnvironmentProfile->BaseRiverWidth : 520.0f) *
        FMath::Lerp(0.65f, 1.45f, WaterAmount);

    // A deterministic meandering road running primarily east/west.
    const float RoadPhase = static_cast<float>(Seed % 997) * 0.013f;
    const float RoadY =
        FMath::Sin(LocalLocation.X * 0.00072f + RoadPhase) * MapSize.Y * 0.10f +
        FMath::Sin(LocalLocation.X * 0.00155f - RoadPhase * 0.45f) * MapSize.Y * 0.035f;

    // A deterministic river running primarily north/south.
    const float RiverPhase = static_cast<float>(Seed % 617) * 0.017f;
    const float RiverX =
        -MapSize.X * 0.18f +
        FMath::Sin(LocalLocation.Y * 0.00058f + RiverPhase) * MapSize.X * 0.10f +
        FMath::Sin(LocalLocation.Y * 0.00131f + RiverPhase * 0.31f) * MapSize.X * 0.035f;

    if (WaterAmount > 0.02f &&
        FMath::Abs(LocalLocation.X - RiverX) <= RiverWidth * 0.5f)
    {
        OutDensity = 0.0f;
        return EIronGridBiomeType::Water;
    }

    if (RoadDensity > 0.02f &&
        FMath::Abs(LocalLocation.Y - RoadY) <= RoadWidth * 0.5f)
    {
        OutDensity = 0.0f;
        return EIronGridBiomeType::Road;
    }

    const float BaseClearingRadius =
        EnvironmentProfile
            ? EnvironmentProfile->MinimumClearingRadius
            : 500.0f;

    const float ClearingRadius =
        BaseClearingRadius *
        FMath::Lerp(0.75f, 1.65f, ClearingRatio);

    for (const FVector2D& Center : ClearingCenters)
    {
        if (FVector2D::Distance(LocalLocation, Center) <= ClearingRadius)
        {
            OutDensity = 0.0f;
            return EIronGridBiomeType::Clearing;
        }
    }

    OutDensity = SampleForestNoise(LocalLocation);

    const float BaseDenseThreshold =
        EnvironmentProfile
            ? EnvironmentProfile->DenseForestThreshold
            : 0.62f;

    const float BaseLightThreshold =
        EnvironmentProfile
            ? EnvironmentProfile->LightForestThreshold
            : 0.36f;

    const float DensityShift = (0.65f - ForestDensity) * 0.42f;

    const float DenseThreshold =
        FMath::Clamp(BaseDenseThreshold + DensityShift, 0.38f, 0.86f);

    const float LightThreshold =
        FMath::Clamp(BaseLightThreshold + DensityShift, 0.18f, DenseThreshold - 0.08f);

    if (OutDensity >= DenseThreshold)
    {
        return EIronGridBiomeType::DenseForest;
    }

    if (OutDensity >= LightThreshold)
    {
        return EIronGridBiomeType::LightForest;
    }

    return EIronGridBiomeType::Clearing;
}

float AIronGridWorldBuilder::SampleForestNoise(
    const FVector2D& LocalLocation) const
{
    const float NoiseScale =
        EnvironmentProfile
            ? EnvironmentProfile->ForestNoiseScale
            : 0.00125f;

    const FVector2D SeedOffset(
        static_cast<float>((Seed * 37) % 10007) * 0.071f,
        static_cast<float>((Seed * 83) % 12007) * 0.059f);

    const FVector2D P =
        (LocalLocation + SeedOffset) * NoiseScale;

    const float N1 = FMath::PerlinNoise2D(P);
    const float N2 =
        FMath::PerlinNoise2D(
            P * 2.17f +
            FVector2D(31.7f, -19.4f));

    const float N3 =
        FMath::PerlinNoise2D(
            P * 4.73f +
            FVector2D(-11.2f, 47.1f));

    const float Combined =
        (N1 + N2 * 0.42f + N3 * 0.18f) / 1.60f;

    return FMath::Clamp(Combined * 0.5f + 0.5f, 0.0f, 1.0f);
}

void AIronGridWorldBuilder::ApplyZoneOverride(
    FIronGridBiomeCell& Cell) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const AIronGridWorldZone* BestZone = nullptr;
    int32 BestPriority = MIN_int32;

    for (TActorIterator<AIronGridWorldZone> It(World); It; ++It)
    {
        const AIronGridWorldZone* Zone = *It;

        if (!Zone ||
            !Zone->bEnabled ||
            Zone->Priority < BestPriority ||
            !Zone->ContainsWorldPoint(Cell.WorldLocation))
        {
            continue;
        }

        BestZone = Zone;
        BestPriority = Zone->Priority;
    }

    if (!BestZone)
    {
        return;
    }

    Cell.bManualOverride = true;

    if (BestZone->bExcludeGeneration)
    {
        Cell.bExcluded = true;
        Cell.Density = 0.0f;
        return;
    }

    Cell.Biome = BestZone->BiomeOverride;

    switch (Cell.Biome)
    {
        case EIronGridBiomeType::DenseForest:
            Cell.Density = 1.0f;
            break;

        case EIronGridBiomeType::LightForest:
            Cell.Density = 0.55f;
            break;

        default:
            Cell.Density = 0.0f;
            break;
    }
}

FColor AIronGridWorldBuilder::GetBiomeDebugColor(
    EIronGridBiomeType Biome) const
{
    switch (Biome)
    {
        case EIronGridBiomeType::DenseForest:
            return FColor(18, 100, 38);

        case EIronGridBiomeType::LightForest:
            return FColor(70, 180, 75);

        case EIronGridBiomeType::Clearing:
            return FColor(210, 190, 90);

        case EIronGridBiomeType::Road:
            return FColor(155, 105, 55);

        case EIronGridBiomeType::Water:
            return FColor(35, 130, 220);

        default:
            return FColor::White;
    }
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
                FColor(55, 55, 55),
                true,
                -1.0f,
                0,
                0.75f);
        }

        for (int32 Y = 0; Y <= CellsY; ++Y)
        {
            const float LocalY = -HalfSize.Y + Y * SafeCellSize;

            DrawDebugLine(
                World,
                Origin + FVector(-HalfSize.X, LocalY, 10.0f),
                Origin + FVector(HalfSize.X, LocalY, 10.0f),
                FColor(55, 55, 55),
                true,
                -1.0f,
                0,
                0.75f);
        }
    }

    if (!bShowBiomePoints)
    {
        return;
    }

    for (const FIronGridBiomeCell& Cell : GeneratedCells)
    {
        FColor Color =
            Cell.bExcluded
                ? FColor::Red
                : GetBiomeDebugColor(Cell.Biome);

        if (Cell.bManualOverride && !Cell.bExcluded)
        {
            Color = Color.ReinterpretAsLinear().Desaturate(0.15f).ToFColor(true);
        }

        DrawDebugPoint(
            World,
            Cell.WorldLocation,
            SamplePointSize,
            Color,
            true,
            -1.0f,
            0);
    }
}

void AIronGridWorldBuilder::ResolveDefaultDataAssetsIfNeeded()
{
    if (!bAutoResolveDefaultDataAssets)
    {
        return;
    }

    if (!EnvironmentProfile && DefaultEnvironmentProfilePath.IsValid())
    {
        EnvironmentProfile =
            Cast<UIronGridEnvironmentProfile>(
                DefaultEnvironmentProfilePath.TryLoad());
    }

    if (!EnvironmentPalette && DefaultEnvironmentPalettePath.IsValid())
    {
        EnvironmentPalette =
            Cast<UIronGridEnvironmentPalette>(
                DefaultEnvironmentPalettePath.TryLoad());
    }
}

int32 AIronGridWorldBuilder::CountValidSprites(
    const TArray<TObjectPtr<UPaperSprite>>& Sprites) const
{
    int32 Count = 0;

    for (UPaperSprite* Sprite : Sprites)
    {
        if (IsValid(Sprite))
        {
            ++Count;
        }
    }

    return Count;
}

UPaperSprite* AIronGridWorldBuilder::PickSprite(
    const TArray<TObjectPtr<UPaperSprite>>& Sprites,
    FRandomStream& RandomStream) const
{
    if (Sprites.IsEmpty())
    {
        return nullptr;
    }

    const int32 StartIndex =
        RandomStream.RandRange(0, Sprites.Num() - 1);

    // Arrays edited in the Data Asset can contain empty slots. Walk from the
    // deterministic random start until a valid sprite is found instead of
    // silently failing the spawn.
    for (int32 Offset = 0; Offset < Sprites.Num(); ++Offset)
    {
        const int32 Index =
            (StartIndex + Offset) % Sprites.Num();

        if (IsValid(Sprites[Index]))
        {
            return Sprites[Index].Get();
        }
    }

    return nullptr;
}

UPaperSprite* AIronGridWorldBuilder::PickSpriteByNameTokens(
    const TArray<TObjectPtr<UPaperSprite>>& Sprites,
    const TArray<FString>& PreferredTokens,
    FRandomStream& RandomStream) const
{
    TArray<UPaperSprite*> Candidates;

    for (UPaperSprite* Sprite : Sprites)
    {
        if (!IsValid(Sprite))
        {
            continue;
        }

        const FString Name = Sprite->GetName();

        for (const FString& Token : PreferredTokens)
        {
            if (Name.Contains(Token, ESearchCase::IgnoreCase))
            {
                Candidates.Add(Sprite);
                break;
            }
        }
    }

    if (Candidates.IsEmpty())
    {
        return PickSprite(Sprites, RandomStream);
    }

    return Candidates[
        RandomStream.RandRange(0, Candidates.Num() - 1)];
}

void AIronGridWorldBuilder::SpawnSprite(
    UPaperSprite* Sprite,
    const FVector& Location,
    FRandomStream& RandomStream,
    FName LayerTag)
{
    if (!Sprite || !GetWorld() || !EnvironmentPalette)
    {
        return;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = this;
    SpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AIronGridGeneratedSpriteActor* GeneratedActor =
        GetWorld()->SpawnActor<AIronGridGeneratedSpriteActor>(
            Location +
            FVector(0.0f, 0.0f, EnvironmentPalette->SpriteZOffset),
            FRotator::ZeroRotator,
            SpawnParameters);

    if (!GeneratedActor)
    {
        return;
    }

    const float MinScale =
        FMath::Min(
            EnvironmentPalette->MinSpriteScale,
            EnvironmentPalette->MaxSpriteScale);

    const float MaxScale =
        FMath::Max(
            EnvironmentPalette->MinSpriteScale,
            EnvironmentPalette->MaxSpriteScale);

    const float Scale =
        RandomStream.FRandRange(MinScale, MaxScale) *
        EnvironmentPalette->GeneratedSpriteScaleMultiplier;

    FRotator Rotation = EnvironmentPalette->SpriteRotation;

    if (EnvironmentPalette->bRandomizeYaw)
    {
        Rotation.Yaw += RandomStream.FRandRange(0.0f, 360.0f);
    }

    GeneratedActor->Configure(
        Sprite,
        Rotation,
        Scale,
        LayerTag);
}

void AIronGridWorldBuilder::BuildEnvironment()
{
    ClearGeneratedEnvironment();
    ResolveDefaultDataAssetsIfNeeded();

    if (!EnvironmentPalette)
    {
#if WITH_EDITOR
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                5.0f,
                FColor::Red,
                FString::Printf(
                    TEXT("BUILD FAILED: EnvironmentPalette is NULL. Assign DA_ForestPalette or place it at %s"),
                    *DefaultEnvironmentPalettePath.ToString()));
        }
#endif
        return;
    }

    GeneratePreview();

    if (GeneratedCells.IsEmpty())
    {
#if WITH_EDITOR
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                8.0f,
                FColor::Red,
                TEXT("BUILD FAILED: biome generation produced 0 cells."));
        }
#endif
        UE_LOG(LogTemp, Error, TEXT("IronGrid WorldBuilder: Build failed because GeneratedCells is empty."));
        return;
    }

    const int32 DenseAssets = CountValidSprites(EnvironmentPalette->DenseForestTrees);
    const int32 LightAssets = CountValidSprites(EnvironmentPalette->LightForestTrees);
    const int32 BushAssets = CountValidSprites(EnvironmentPalette->Bushes);
    const int32 RockAssets = CountValidSprites(EnvironmentPalette->Rocks);
    const int32 ClearingAssets = CountValidSprites(EnvironmentPalette->ClearingGround);
    const int32 RoadAssets = CountValidSprites(EnvironmentPalette->RoadGround);
    const int32 WaterAssets = CountValidSprites(EnvironmentPalette->WaterGround);

    const int32 TotalValidAssets =
        DenseAssets + LightAssets + BushAssets + RockAssets +
        ClearingAssets + RoadAssets + WaterAssets;

    if (TotalValidAssets == 0)
    {
#if WITH_EDITOR
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                10.0f,
                FColor::Red,
                TEXT("BUILD FAILED: DA_ForestPalette is assigned but all sprite arrays are empty."));
        }
#endif
        UE_LOG(
            LogTemp,
            Error,
            TEXT("IronGrid WorldBuilder: EnvironmentPalette '%s' has 0 valid PaperSprites."),
            *GetNameSafe(EnvironmentPalette));
        return;
    }

#if WITH_EDITOR
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            10.0f,
            FColor::Cyan,
            FString::Printf(
                TEXT("PALETTE OK | Dense:%d Light:%d Bush:%d Rock:%d Ground:%d Road:%d Water:%d"),
                DenseAssets,
                LightAssets,
                BushAssets,
                RockAssets,
                ClearingAssets,
                RoadAssets,
                WaterAssets));
    }
#endif

    UE_LOG(
        LogTemp,
        Display,
        TEXT("IronGrid WorldBuilder palette '%s': Dense=%d Light=%d Bush=%d Rock=%d Ground=%d Road=%d Water=%d"),
        *GetNameSafe(EnvironmentPalette),
        DenseAssets,
        LightAssets,
        BushAssets,
        RockAssets,
        ClearingAssets,
        RoadAssets,
        WaterAssets);

    int32 SpawnedCount = 0;
    int32 SpawnedGroundCount = 0;
    int32 SpawnedTreeCount = 0;
    int32 SpawnedBushCount = 0;
    int32 SpawnedRockCount = 0;
    const float SafeCellSize = FMath::Max(PreviewCellSize, 100.0f);

    for (const FIronGridBiomeCell& Cell : GeneratedCells)
    {
        if (Cell.bExcluded)
        {
            continue;
        }

        const int32 CellSeed =
            Seed ^
            (Cell.GridX * 73856093) ^
            (Cell.GridY * 19349663);

        FRandomStream RandomStream(CellSeed);

        // Biome classification/decoration may use jitter, but ground tiles must
        // remain locked to the exact planning-cell center or visible seams appear.
        const FVector2D HalfSize = MapSize * 0.5f;
        const FVector ExactCellCenter =
            GetActorLocation() +
            FVector(
                -HalfSize.X + (static_cast<float>(Cell.GridX) + 0.5f) * SafeCellSize,
                -HalfSize.Y + (static_cast<float>(Cell.GridY) + 0.5f) * SafeCellSize,
                20.0f);

        auto SpawnGround = [&](
            const TArray<TObjectPtr<UPaperSprite>>& Sprites,
            FName Tag,
            const TArray<FString>& PreferredTokens = TArray<FString>())
        {
            UPaperSprite* GroundSprite =
                PreferredTokens.IsEmpty()
                    ? PickSprite(Sprites, RandomStream)
                    : PickSpriteByNameTokens(Sprites, PreferredTokens, RandomStream);

            if (GroundSprite)
            {
                FActorSpawnParameters Params;
                Params.Owner = this;
                Params.SpawnCollisionHandlingOverride =
                    ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                AIronGridGeneratedSpriteActor* GroundActor =
                    GetWorld()->SpawnActor<AIronGridGeneratedSpriteActor>(
                        ExactCellCenter +
                        FVector(0.0f, 0.0f, EnvironmentPalette->GroundZOffset),
                        FRotator::ZeroRotator,
                        Params);

                if (GroundActor)
                {
                    float GroundScale =
                        EnvironmentPalette->GroundSpriteScale;

                    if (EnvironmentPalette->bAutoScaleGroundToCell)
                    {
                        const float ReferenceSize =
                            FMath::Max(
                                EnvironmentPalette->GroundReferenceWorldSize,
                                1.0f);

                        GroundScale *=
                            SafeCellSize / ReferenceSize;
                    }

                    GroundScale *=
                        EnvironmentPalette->GroundTileOverlapScale;

                    GroundActor->Configure(
                        GroundSprite,
                        EnvironmentPalette->GroundSpriteRotation,
                        GroundScale,
                        Tag);

                    ++SpawnedCount;
                    ++SpawnedGroundCount;
                }
            }
        };

        if (Cell.Biome == EIronGridBiomeType::Road)
        {
            SpawnGround(EnvironmentPalette->RoadGround, TEXT("IG_Road"));
            continue;
        }

        if (Cell.Biome == EIronGridBiomeType::Water)
        {
            SpawnGround(EnvironmentPalette->WaterGround, TEXT("IG_Water"));
            continue;
        }

        // Every non-road/non-water cell receives a continuous land base.
        // Forest dressing is then layered above this ground tile.
        const TArray<FString> ForestGroundTokens =
        {
            TEXT("Grass"),
            TEXT("ForestFloor")
        };

        const TArray<FString> ClearingGroundTokens =
        {
            TEXT("Clearing"),
            TEXT("Dirt")
        };

        if (Cell.Biome == EIronGridBiomeType::Clearing)
        {
            SpawnGround(
                EnvironmentPalette->ClearingGround,
                TEXT("IG_Clearing"),
                ClearingGroundTokens);

            if (RandomStream.FRand() <= EnvironmentPalette->RockChancePerCell)
            {
                if (UPaperSprite* Rock = PickSprite(EnvironmentPalette->Rocks, RandomStream))
                {
                    const FVector Offset(
                        RandomStream.FRandRange(-SafeCellSize * 0.32f, SafeCellSize * 0.32f),
                        RandomStream.FRandRange(-SafeCellSize * 0.32f, SafeCellSize * 0.32f),
                        0.0f);

                    SpawnSprite(
                        Rock,
                        ExactCellCenter + Offset,
                        RandomStream,
                        TEXT("IG_Rock"));

                    ++SpawnedCount;
                    ++SpawnedRockCount;
                }
            }

            continue;
        }

        SpawnGround(
            EnvironmentPalette->ClearingGround,
            TEXT("IG_ForestBase"),
            ForestGroundTokens);

        const bool bDense =
            Cell.Biome == EIronGridBiomeType::DenseForest;

        const int32 MinTrees =
            bDense
                ? EnvironmentPalette->DenseForestMinTreesPerCell
                : EnvironmentPalette->LightForestMinTreesPerCell;

        const int32 MaxTrees =
            bDense
                ? EnvironmentPalette->DenseForestMaxTreesPerCell
                : EnvironmentPalette->LightForestMaxTreesPerCell;

        const int32 TreeCount =
            RandomStream.RandRange(
                FMath::Min(MinTrees, MaxTrees),
                FMath::Max(MinTrees, MaxTrees));

        const TArray<TObjectPtr<UPaperSprite>>& TreePool =
            bDense
                ? EnvironmentPalette->DenseForestTrees
                : EnvironmentPalette->LightForestTrees;

        for (int32 TreeIndex = 0; TreeIndex < TreeCount; ++TreeIndex)
        {
            UPaperSprite* Tree =
                PickSprite(TreePool, RandomStream);

            if (!Tree)
            {
                continue;
            }

            const FVector Offset(
                RandomStream.FRandRange(-SafeCellSize * 0.42f, SafeCellSize * 0.42f),
                RandomStream.FRandRange(-SafeCellSize * 0.42f, SafeCellSize * 0.42f),
                0.0f);

            SpawnSprite(
                Tree,
                ExactCellCenter + Offset,
                RandomStream,
                bDense ? TEXT("IG_DenseTree") : TEXT("IG_LightTree"));

            ++SpawnedCount;
            ++SpawnedTreeCount;
        }

        if (RandomStream.FRand() <= EnvironmentPalette->BushChancePerForestCell)
        {
            if (UPaperSprite* Bush = PickSprite(EnvironmentPalette->Bushes, RandomStream))
            {
                const FVector Offset(
                    RandomStream.FRandRange(-SafeCellSize * 0.40f, SafeCellSize * 0.40f),
                    RandomStream.FRandRange(-SafeCellSize * 0.40f, SafeCellSize * 0.40f),
                    0.0f);

                SpawnSprite(
                    Bush,
                    ExactCellCenter + Offset,
                    RandomStream,
                    TEXT("IG_Bush"));

                ++SpawnedCount;
                ++SpawnedBushCount;
            }
        }

        if (RandomStream.FRand() <= EnvironmentPalette->RockChancePerCell)
        {
            if (UPaperSprite* Rock = PickSprite(EnvironmentPalette->Rocks, RandomStream))
            {
                const FVector Offset(
                    RandomStream.FRandRange(-SafeCellSize * 0.36f, SafeCellSize * 0.36f),
                    RandomStream.FRandRange(-SafeCellSize * 0.36f, SafeCellSize * 0.36f),
                    0.0f);

                SpawnSprite(
                    Rock,
                    ExactCellCenter + Offset,
                    RandomStream,
                    TEXT("IG_Rock"));

                ++SpawnedCount;
                ++SpawnedRockCount;
            }
        }
    }

#if WITH_EDITOR
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            6.0f,
            FColor::Green,
            FString::Printf(
                TEXT("BUILD OK | Total:%d Ground:%d Trees:%d Bushes:%d Rocks:%d"),
                SpawnedCount,
                SpawnedGroundCount,
                SpawnedTreeCount,
                SpawnedBushCount,
                SpawnedRockCount));
    }
#endif

    UE_LOG(
        LogTemp,
        Display,
        TEXT("IronGrid WorldBuilder BUILD OK: Total=%d Ground=%d Trees=%d Bushes=%d Rocks=%d"),
        SpawnedCount,
        SpawnedGroundCount,
        SpawnedTreeCount,
        SpawnedBushCount,
        SpawnedRockCount);

    if (bHidePreviewAfterBuild)
    {
        if (UWorld* World = GetWorld())
        {
            FlushPersistentDebugLines(World);
        }
    }
}

void AIronGridWorldBuilder::ClearGeneratedEnvironment()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TArray<AIronGridGeneratedSpriteActor*> ActorsToDestroy;

    for (TActorIterator<AIronGridGeneratedSpriteActor> It(World); It; ++It)
    {
        AIronGridGeneratedSpriteActor* Actor = *It;

        if (Actor &&
            Actor->ActorHasTag(TEXT("IronGridGeneratedEnvironment")) &&
            Actor->GetOwner() == this)
        {
            ActorsToDestroy.Add(Actor);
        }
    }

    for (AIronGridGeneratedSpriteActor* Actor : ActorsToDestroy)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
}

void AIronGridWorldBuilder::ClearPreview()
{
    GeneratedCells.Reset();

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
        Message = TEXT("WORLD BUILDER: EnvironmentProfile is not assigned.");
        MessageColor = FColor::Yellow;
    }
    else if (!EnvironmentPalette)
    {
        Message = TEXT("WORLD BUILDER: Biome generation ready. EnvironmentPalette not assigned.");
        MessageColor = FColor::Yellow;
    }

    GEngine->AddOnScreenDebugMessage(
        -1,
        6.0f,
        MessageColor,
        Message);
#endif
}


void AIronGridWorldBuilder::ValidateGameplay()
{
    if (GeneratedCells.IsEmpty())
    {
        BuildBiomeMap();
    }

    if (GeneratedCells.IsEmpty())
    {
#if WITH_EDITOR
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                5.0f,
                FColor::Red,
                TEXT("WORLD VALIDATION FAILED: no biome cells generated."));
        }
#endif
        return;
    }

    const float SafeCellSize = FMath::Max(PreviewCellSize, 100.0f);
    const int32 CellsX = FMath::Max(1, FMath::FloorToInt(MapSize.X / SafeCellSize));
    const int32 CellsY = FMath::Max(1, FMath::FloorToInt(MapSize.Y / SafeCellSize));

    auto IsTraversable = [](const FIronGridBiomeCell& Cell)
    {
        return
            !Cell.bExcluded &&
            Cell.Biome != EIronGridBiomeType::Water;
    };

    int32 StartIndex = INDEX_NONE;
    int32 TraversableCount = 0;

    for (int32 Index = 0; Index < GeneratedCells.Num(); ++Index)
    {
        if (IsTraversable(GeneratedCells[Index]))
        {
            ++TraversableCount;

            if (StartIndex == INDEX_NONE)
            {
                StartIndex = Index;
            }
        }
    }

    if (StartIndex == INDEX_NONE || TraversableCount == 0)
    {
#if WITH_EDITOR
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                6.0f,
                FColor::Red,
                TEXT("WORLD VALIDATION FAILED: no traversable cells."));
        }
#endif
        return;
    }

    TArray<bool> Visited;
    Visited.Init(false, GeneratedCells.Num());

    TArray<int32> Queue;
    Queue.Reserve(GeneratedCells.Num());
    Queue.Add(StartIndex);
    Visited[StartIndex] = true;

    int32 ReadIndex = 0;
    int32 ReachableCount = 0;

    const int32 DX[4] = { 1, -1, 0, 0 };
    const int32 DY[4] = { 0, 0, 1, -1 };

    while (ReadIndex < Queue.Num())
    {
        const int32 CurrentIndex = Queue[ReadIndex++];
        ++ReachableCount;

        const FIronGridBiomeCell& CurrentCell =
            GeneratedCells[CurrentIndex];

        for (int32 Direction = 0; Direction < 4; ++Direction)
        {
            const int32 NX = CurrentCell.GridX + DX[Direction];
            const int32 NY = CurrentCell.GridY + DY[Direction];

            if (NX < 0 || NX >= CellsX || NY < 0 || NY >= CellsY)
            {
                continue;
            }

            const int32 NeighborIndex = NX * CellsY + NY;

            if (!GeneratedCells.IsValidIndex(NeighborIndex) ||
                Visited[NeighborIndex] ||
                !IsTraversable(GeneratedCells[NeighborIndex]))
            {
                continue;
            }

            Visited[NeighborIndex] = true;
            Queue.Add(NeighborIndex);
        }
    }

    const float ReachableRatio =
        TraversableCount > 0
            ? static_cast<float>(ReachableCount) /
              static_cast<float>(TraversableCount)
            : 0.0f;

    int32 RoadCells = 0;
    int32 WaterCells = 0;
    int32 ExcludedCells = 0;

    for (const FIronGridBiomeCell& Cell : GeneratedCells)
    {
        if (Cell.Biome == EIronGridBiomeType::Road)
        {
            ++RoadCells;
        }

        if (Cell.Biome == EIronGridBiomeType::Water)
        {
            ++WaterCells;
        }

        if (Cell.bExcluded)
        {
            ++ExcludedCells;
        }
    }

#if WITH_EDITOR
    if (GEngine)
    {
        const bool bHealthyConnectivity =
            ReachableRatio >= 0.95f;

        const FColor ResultColor =
            bHealthyConnectivity
                ? FColor::Green
                : FColor::Yellow;

        GEngine->AddOnScreenDebugMessage(
            -1,
            8.0f,
            ResultColor,
            FString::Printf(
                TEXT("WORLD VALIDATION | Reachable: %.1f%% | Road:%d Water:%d Excluded:%d | %s"),
                ReachableRatio * 100.0f,
                RoadCells,
                WaterCells,
                ExcludedCells,
                bHealthyConnectivity
                    ? TEXT("PASS")
                    : TEXT("WARNING: disconnected traversable regions")));
    }
#endif
}
