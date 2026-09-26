# Unreal Project Setup

Target engine: **Unreal Engine 5.8.x**.

## First launch
1. Clone the repository.
2. Right-click `BDFR_IronGrid.uproject`.
3. Choose **Generate Visual Studio project files**.
4. Open `BDFR_IronGrid.sln`.
5. Build the **Development Editor** target.
6. Launch `BDFR_IronGrid.uproject`.

## Current playable prototype
The C++ foundation already includes:
- Tank pawn with independent hull and turret layers.
- W/S forward and reverse movement.
- A/D hull turning.
- Mouse-to-world desired aim position.
- Smooth 360-degree turret traverse performed by the engine.
- Actual gun direction calculated from the muzzle.
- Dual reticle HUD:
  - **Yellow**: player desired aim.
  - **White/green**: actual turret/gun aim.
- Green actual reticle indicates the turret is nearly aligned with the desired aim.

## Sprite setup
Assign a hull sprite to `HullSprite` and a separate transparent turret sprite to `TurretSprite`.
The turret art should point consistently along its local forward direction. Unreal rotates the turret component continuously; no 4/8/16/32-direction turret sprite sheet is required.

## Next engineering milestones
- Acceleration/braking and tracked differential steering.
- Neutral steering / pivot turn.
- Server-authoritative replication.
- Ballistic projectile actor.
- Armor and module damage.
- Sprite-based reticle textures replacing debug cross rendering.
- Dedicated server target and 64-player relevancy strategy.
