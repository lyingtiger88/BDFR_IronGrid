# IRON GRID: BATTLEGROUND

A 64-player open-world tank battle royale built in Unreal Engine with a modernized SNES-inspired HD sprite/2.5D art direction.

## Core pillars
- 64-player online battle royale
- Large open-world battlefield
- Server-authoritative multiplayer
- Tank movement inspired by realistic tracked-vehicle handling
- Independent hull and turret rotation
- Mouse-driven aiming: the desired aim marker moves instantly, while the turret/gun aim marker follows at the vehicle's traverse speed
- Ballistic projectiles and modular vehicle damage
- HD retro/SNES-inspired art with separate hull and turret sprites

## Visual architecture
Tank rendering is split into independent layers:
1. **Hull sprite** — rotates with vehicle movement.
2. **Turret sprite** — rotates independently through 360 degrees in-engine.
3. **Weapon / muzzle FX** — anchored to the turret muzzle socket/pivot.
4. **Reticle system** — separates desired mouse aim from actual gun/turret aim.

## Working repository structure
```
BDFR_IronGrid/
├── Config/
├── Content/
│   ├── Art/
│   │   ├── Sprites/
│   │   │   ├── Tanks/
│   │   │   │   ├── Hulls/
│   │   │   │   └── Turrets/
│   │   │   ├── Reticles/
│   │   │   ├── FX/
│   │   │   ├── Environment/
│   │   │   └── UI/
│   ├── Blueprints/
│   ├── Maps/
│   └── UI/
├── Docs/
└── Source/
```

## Project status
Pre-production / prototype foundation.

See `Docs/AimingSystem.md` for the aiming and multi-reticle design.
