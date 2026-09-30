# IRON GRID — Weapons & Ballistics v1

The first cannon/ballistics layer is now implemented in C++ and is designed to remain usable when the project moves to dedicated-server multiplayer.

## Controls

- **Left Mouse Button** — fire cannon
- **R** — manual reload
- Mouse movement still controls desired turret aim

## Firing model

The owning client requests a shot, but the authoritative firing decision is made on the server.

Flow:

```
Local Fire Input
    ↓
FireWeapon()
    ↓
ServerFireWeapon RPC
    ↓
Server validates reload / ammo / cooldown
    ↓
Spawn AIronGridProjectile
    ↓
Replicated projectile movement
```

In standalone play, the same code path executes locally without requiring a network session.

## Projectile

Native projectile class:

`AIronGridProjectile`

Path:

`Source/BDFR_IronGrid/Weapon/`

The projectile uses:

- `USphereComponent` collision
- `UProjectileMovementComponent`
- replicated actor movement
- gravity
- point-damage delivery on server impact
- Blueprint impact-FX hook
- automatic lifetime cleanup

Default values are deliberately exposed for Blueprint tuning.

## Ballistic parameters

Tank weapon settings include:

- `MuzzleVelocity`
- `ProjectileGravityScale`
- `ProjectileDamage`
- `ProjectileSpawnHeight`
- `BallisticPredictionTime`
- `BallisticPredictionRadius`

Default muzzle velocity is currently:

`30000 uu/s`

The shell is launched from the logical muzzle direction, so firing always follows the **actual turret orientation**, not the desired mouse reticle.

## Reload / ammunition

Default tank setup behaves like a conventional single-round cannon:

- `MagazineSize = 1`
- `StartingReserveAmmo = 30`
- `ReloadDuration = 3.5 s`

After firing the loaded round, reload begins automatically if reserve ammunition remains.

The system also supports a larger magazine size for future autoloaders.

Runtime replicated state:

- `CurrentAmmoInMagazine`
- `ReserveAmmo`
- `bReloading`

## Ballistic impact reticle

The HUD now predicts the projectile path using Unreal projectile-path prediction.

Reticle roles:

- Yellow — desired mouse aim
- White / green — actual turret direction
- Red / orange — predicted ballistic impact point

The ballistic reticle accounts for:

- muzzle position
- actual turret direction
- muzzle velocity
- gravity
- projectile radius
- world collision

This means the predicted impact marker can separate from the direct gun reticle as range increases.

## FX hooks

No final production FX assets are committed yet.

Blueprint hooks are available for:

- `OnWeaponFired`
- `OnReloadStarted`
- `OnReloadFinished`
- projectile `OnImpactFX`

These can later drive:

- muzzle flash sprites
- smoke
- recoil animation
- cannon audio
- impact sparks
- dust / debris
- explosion effects

## Current limitations

Still planned:

- ammunition types (AP / HE / HEAT / smoke)
- armor penetration
- ricochet
- module damage
- true recoil
- rangefinder
- stabilizer
- final projectile sprite / tracer
- production muzzle and impact FX
- advanced client-side firing prediction



## Weapon Polish v1.1

The weapon prototype now has a production-facing visual layer in addition to the debug shell.

### Projectile visual

`AIronGridProjectile` now contains:

```
Collision
├── ProjectileSprite
└── DebugVisual
```

To use final shell art:

1. Create/open a Blueprint child of `IronGridProjectile`.
2. Select `ProjectileSprite`.
3. Assign the Paper2D shell/tracer sprite.
4. Adjust its relative rotation for the top-down plane.
5. Leave `bHideDebugVisualWhenSpriteAssigned` enabled.

When a sprite is assigned, the temporary sphere is hidden automatically.

The orange debug trajectory is controlled separately with:

`bShowDebugTrail`

Turn it off when the final tracer visual is ready.

### Muzzle feedback

Each successful shot now produces:

- a short debug muzzle flash/arrow in development builds
- `OnWeaponFired` Blueprint event
- whole-vehicle recoil opposite the cannon direction
- a small camera recoil pulse

Tuning:

- `bShowDebugMuzzleFX`
- `DebugMuzzleFXDuration`
- `TankRecoilSpeed`
- `TankRecoilDamping`
- `CameraRecoilKick`
- `CameraRecoilReturnSpeed`

The turret stays fixed on its mount. Recoil moves the whole tank opposite the cannon direction while preserving the logical muzzle direction and ballistic calculation.

### Impact feedback

Projectile impacts now provide:

- a short red/orange development impact marker
- `OnImpactFX(ImpactPoint, ImpactNormal)` Blueprint event

The debug impact marker is controlled by:

`bShowDebugImpactFX`

### Blueprint FX workflow

Use the existing Blueprint events to replace debug feedback with production assets:

- Tank: `OnWeaponFired`
- Tank: `OnReloadStarted`
- Tank: `OnReloadFinished`
- Projectile: `OnProjectileLaunched`
- Projectile: `OnImpactFX`

These events are intended for sprite flashes, smoke, audio, shell tracers, dust, debris, and later Niagara effects.
