# IRON GRID — World Builder

## Phase 1 — Deterministic foundation

Native classes:

- `AIronGridWorldBuilder`
- `UIronGridEnvironmentProfile`

The first phase intentionally does not spawn environment art yet. It establishes the deterministic map-generation foundation that later forest, road, river and POI generators will use.

### World Builder actor

Place `IronGridWorldBuilder` in the level.

Main properties:

- BuilderMode: Auto / Manual / Hybrid
- Seed
- MapSize
- PreviewCellSize
- EnvironmentProfile

Editor buttons:

- Generate Preview
- Clear Preview
- Randomize Seed
- Validate Builder

Using the same Seed and settings always generates the same preview points.

### Environment Profile

Create a Data Asset using `IronGridEnvironmentProfile`.

Suggested first forest profile:

- ForestDensity = 0.65
- ClearingRatio = 0.20
- RoadDensity = 0.35
- WaterAmount = 0.20
- CoverDensity = 0.55
- DecorationDensity = 0.65
- MinimumTankCorridorWidth = 260
- MinimumClearingRadius = 500

### What Phase 2 will add

Phase 2 converts these deterministic sample cells into biome data:

- Dense Forest
- Light Forest
- Clearing
- Road reservation
- Water reservation

No visible square/grid pattern will remain in the final environment. The grid is only a hidden planning structure.
