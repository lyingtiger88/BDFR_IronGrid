# Aiming System

## Goal
IRON GRID uses a two-reticle aiming model inspired by modern tank games while preserving the project's top-down presentation.

## Reticles
### Desired Aim Reticle
- Controlled directly by the mouse.
- Represents where the player wants the weapon to point.
- Moves immediately with player input.
- Does **not** guarantee the gun is currently aligned.

### Gun / Turret Reticle
- Represents the actual current firing direction.
- Follows the desired aim reticle at the turret traverse speed.
- Uses vehicle-specific traverse limits and acceleration.
- The projectile leaves along this actual gun direction.

### Optional Ballistic Impact Reticle
- Computed from muzzle position, muzzle velocity, gravity and collision prediction.
- Shows estimated world impact point.
- Can be hidden for arcade presets.

## Runtime flow
```
Mouse Position
    ↓
Desired World Aim Point
    ↓
Desired Aim Reticle
    ↓
Desired Turret Yaw
    ↓
Traverse Motor / Rotation Interpolation
    ↓
Actual Turret Yaw
    ↓
Gun Reticle
    ↓
Ballistic Shot Direction
```

## Sprite architecture
The hull and turret are separate sprites.

- Hull pivot: vehicle center.
- Turret pivot: turret ring center.
- Barrel direction: local +X/up axis defined consistently for every turret sprite.
- Rotation is performed continuously in Unreal Engine; no directional turret animation sheet is required.
- Muzzle socket/offset is stored per turret type.

This allows smooth 360-degree turret traversal without swapping directional sprites.

## Gameplay variables
```
DesiredAimWorldPosition
CurrentTurretYaw
TargetTurretYaw
TurretTraverseSpeed
TurretTraverseAcceleration
GunElevationSpeed
AimToleranceDegrees
MuzzleWorldPosition
ProjectileVelocity
```

## Firing rule
A shot may always be fired in arcade mode, but it travels along the **actual gun direction**. A stricter mode may require the angular error between desired aim and current gun aim to be below a configurable threshold.
