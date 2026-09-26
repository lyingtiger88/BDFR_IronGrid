# Sprite & Art Pipeline

## Visual target
High-quality SNES-inspired HD pixel art presented inside a modern Unreal Engine 2.5D battlefield.

## Tank composition
Each tank is built from independent layers:
- Hull
- Turret + barrel
- Shadow
- Damage overlay
- Muzzle flash
- Track/dust FX
- Selection/team indicators

The hull and turret must never be baked together for gameplay assets.

## Export rules
Recommended source export:
- PNG
- Transparent background
- Lossless
- Consistent canvas size per category
- Pivot metadata documented separately
- No baked shadow on hull/turret
- Front direction standardized across every asset

## Suggested folders
```
Content/Art/Sprites/Tanks/Hulls
Content/Art/Sprites/Tanks/Turrets
Content/Art/Sprites/Reticles
Content/Art/Sprites/FX
Content/Art/Sprites/Environment
Content/Art/Sprites/UI
```

## Unreal import baseline
- Texture Group: 2D Pixels (or project equivalent)
- Mipmaps: disabled for UI/reticles; evaluated per world sprite
- Compression: UI / UserInterface2D for HUD where appropriate
- Filtering: nearest for hard pixel style, or controlled hybrid filtering for HD sprites
- Alpha preserved
- Pivot corrected before final Paper2D sprite creation

## Reticle asset set
At minimum:
- DesiredAim
- ActualGunAim
- BallisticImpact
- TargetLocked
- Reloading
- OutOfTraverse / Obstructed
- FriendlyTarget
- EnemyTarget
