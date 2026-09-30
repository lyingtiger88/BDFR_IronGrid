# IRON GRID — Power-Up System v1

Native class:

`AIronGridPowerUp`

Source:

`Source/BDFR_IronGrid/PowerUp/`

The actor is replicated and pickup effects are applied authoritatively on the server.

## Power-up types

### Ammo

Adds ammunition to the tank reserve.

Default:

`AmmoAmount = 8`

### Speed Boost

Temporarily increases maximum forward/reverse track speed.

Default:

```
SpeedMultiplier = 1.35
EffectDuration = 10 s
```

### Reload Boost

Temporarily reduces reload time.

Default:

```
ReloadTimeMultiplier = 0.55
EffectDuration = 10 s
```

A lower reload multiplier means faster reload.

### Repair

Calls the tank Blueprint event:

`OnRepairPowerUp(RepairAmount)`

The hook is ready now. The actual repair/health-module logic will be connected when Armor & Damage is implemented.

Default:

`RepairAmount = 40`

## Spawn and pickup sounds

Every power-up has two independent audio slots:

- `SpawnSound`
- `PickupSound`

When the actor enters the world, SpawnSound is played at the power-up position.

When a tank overlaps the pickup:

1. the server applies the effect,
2. collision is disabled,
3. PickupSound is multicast to players,
4. the item is hidden,
5. the actor is destroyed shortly after.

Volumes are configurable through:

- `SpawnSoundVolume`
- `PickupSoundVolume`

## Visuals

Hierarchy:

```
PickupCollision
└── VisualRoot
    ├── PowerUpSprite
    └── DebugVisual
```

If `PowerUpSprite` has a PaperSprite assigned and `bHideDebugVisualWhenSpriteAssigned` is enabled, the temporary debug sphere is hidden automatically.

The visual can bob and rotate using:

- `bAnimateVisual`
- `BobAmplitude`
- `BobSpeed`
- `RotationSpeed`

## Creating Blueprint variants

Create Blueprint children of `IronGridPowerUp`, for example:

```
BP_PowerUp_Ammo
BP_PowerUp_Speed
BP_PowerUp_Reload
BP_PowerUp_Repair
```

For each Blueprint:

1. set `PowerUpType`,
2. assign its PaperSprite,
3. assign SpawnSound,
4. assign PickupSound,
5. tune amount/multiplier/duration.

These Blueprints can be placed directly in a map or spawned later by a battle-royale loot/power-up spawner.
