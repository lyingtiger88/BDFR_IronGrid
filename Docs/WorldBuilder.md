# IRON GRID — World Builder

## Current implementation

The World Builder now has a working deterministic biome planner, manual/hybrid override zones, and real Paper2D environment spawning.

### Main classes

- `AIronGridWorldBuilder`
- `UIronGridEnvironmentProfile`
- `UIronGridEnvironmentPalette`
- `AIronGridWorldZone`
- `AIronGridGeneratedSpriteActor`

## Biomes

The hidden planning grid classifies cells into:

- Dense Forest
- Light Forest
- Clearing
- Road
- Water

Forest shapes use layered Perlin noise. Road and river paths are deterministic meandering bands. Clearings are seeded areas. The visible result is jittered and does not need to look grid-based.

## World Builder buttons

- Generate Preview
- Build Environment
- Clear Generated Environment
- Clear Preview
- Randomize Seed
- Validate Builder

## Environment Profile

Create a Data Asset of class `IronGridEnvironmentProfile`.

Recommended forest prototype:

- ForestDensity = 0.65
- ClearingRatio = 0.20
- RoadDensity = 0.35
- WaterAmount = 0.20
- ForestNoiseScale = 0.00125
- DenseForestThreshold = 0.62
- LightForestThreshold = 0.36
- BaseRoadWidth = 420
- BaseRiverWidth = 520
- MinimumTankCorridorWidth = 260
- MinimumClearingRadius = 500
- ClearingCount = 5

## Environment Palette

Create a Data Asset of class `IronGridEnvironmentPalette`.

Assign the imported PaperSprites to:

- DenseForestTrees
- LightForestTrees
- Bushes
- Rocks
- ClearingGround
- RoadGround
- WaterGround

The palette also controls tree counts, bush/rock probabilities, sprite scale and sprite rotation.

## Manual / Hybrid editing

Place `IronGridWorldZone` actors in the level.

Resize the zone box and set:

- BiomeOverride
- Priority
- bExcludeGeneration

Modes:

- Auto: ignores manual zones.
- Hybrid: generates automatically, then zones override selected regions.
- Manual: starts as Clearing and only zones define special biome areas.

Higher Priority wins when zones overlap.

## First practical test

1. Place `IronGridWorldBuilder`.
2. Assign EnvironmentProfile and EnvironmentPalette.
3. Use MapSize = 10000 x 10000 and PreviewCellSize = 500.
4. Press Generate Preview.
5. Confirm biome colors:
   - dark green = dense forest
   - light green = light forest
   - tan = clearing
   - brown = road
   - blue = water
6. Press Build Environment.
7. The assigned PaperSprites will be spawned deterministically into the level.
8. Use Clear Generated Environment before rebuilding.

The next milestone should replace the prototype road/water cell art with spline-aware connected road/river pieces and add tank traversability validation.
