# IRON GRID: BATTLEGROUND

A 64-player open-world tank battle royale built in Unreal Engine with a modernized SNES-inspired HD sprite / 2.5D art direction.

> **Current milestone:** Playable Tank Prototype  
> **Engine:** Unreal Engine 5.8.x  
> **Repository:** `BDFR_IronGrid`

## Core pillars

- 64-player online battle royale
- Large open-world battlefield
- Server-authoritative multiplayer
- Tank movement inspired by realistic tracked-vehicle handling
- Independent hull and turret rotation
- Mouse-driven aiming where the desired aim marker moves instantly while the turret/gun follows at traverse speed
- Ballistic projectiles and modular vehicle damage
- HD retro / SNES-inspired art with separate hull and turret sprites

## Current playable prototype

The repository currently includes:

- Unreal Engine 5.8 C++ project foundation
- Game, Editor, and Dedicated Server targets
- Separate hull and turret sprite architecture
- Corrected visual rotation pipeline for 2D tank art
- Smooth 360-degree turret tracking
- Desired aim vs actual gun aim reticles
- Tracked Movement v2 with acceleration, braking, independent track speeds, differential steering, and pivot turns
- Fixed and Speed Reactive camera modes
- Runtime camera settings
- Pause menu on **P**
- Camera controls inside the pause menu
- Multiplayer-safe pause-menu behavior
- Server-authoritative cannon firing with replicated ballistic projectiles
- Reload / reserve-ammo system
- Gravity-based shell trajectory and predicted ballistic impact reticle

The current milestone is **Weapons & Ballistics v1**. Next comes **Armor, Penetration & Module Damage**, followed by deeper **Network Replication** work.

---

# Development Roadmap

### Legend

- [x] Implemented
- [~] Prototype / partially implemented
- [ ] Planned

## Progress overview

- [x] Phase 0 — Project Foundation
- [~] Phase 1 — Core Tank Prototype
- [~] Phase 2 — War Thunder-style Aiming
- [~] Phase 3 — Camera System
- [~] Phase 4 — Pause Menu & Initial Settings
- [~] Phase 5 — Weapons & Ballistics
- [ ] Phase 6 — Armor, Damage & Vehicle Modules
- [~] Phase 7 — Multiplayer Foundation
- [ ] Phase 8 — Battle Royale Game Mode
- [ ] Phase 9 — Open World
- [~] Phase 10 — SNES-HD Art Pipeline
- [ ] Phase 11 — Audio & Feedback
- [ ] Phase 12 — Optimization & Release

<details open>
<summary><strong>Phase 0 — Project Foundation</strong> — COMPLETE</summary>

- [x] Unreal Engine 5.8 C++ project descriptor
- [x] Game target
- [x] Editor target
- [x] Dedicated Server target
- [x] Runtime game module
- [x] Paper2D enabled
- [x] Slate / SlateCore UI dependencies
- [x] Unreal-specific `.gitignore`
- [x] Initial project documentation
- [x] Sprite / art pipeline documentation
- [x] Aiming-system documentation

</details>

<details open>
<summary><strong>Phase 1 — Core Tank Prototype</strong></summary>

### Tank representation

- [x] Separate hull and turret architecture
- [x] Hull sprite component
- [x] Independent turret sprite component
- [x] Logical `TurretPivot` separated from sprite art orientation
- [x] `HullVisualRoot` for art-direction correction
- [x] `TurretVisualRoot` for art-direction correction
- [x] Configurable hull art yaw offset
- [x] Configurable turret art yaw offset
- [x] Current turret artwork corrected to match mouse direction
- [x] Muzzle component attached to logical turret axis

### Basic movement

- [x] W / S forward and reverse prototype movement
- [x] A / D hull turning
- [~] Collision-aware actor movement
- [x] Acceleration and deceleration
- [x] Braking model
- [x] Independent left/right track speeds
- [x] Differential steering
- [x] Neutral steering / pivot turn
- [ ] Engine torque
- [ ] Transmission / gears
- [ ] Surface-dependent traction
- [ ] Track damage affecting mobility

**Status:** Tracked Movement v2 functional; torque, transmission, terrain traction, and damaged-track behavior remain planned.

</details>

<details open>
<summary><strong>Phase 2 — War Thunder-style Aiming</strong></summary>

### Mouse aiming

- [x] Mouse cursor projected into the game world
- [x] Surface hit under cursor used as desired aim point
- [x] Fallback flat-plane aiming
- [x] Desired aim point independent of hull direction

### Turret tracking

- [x] Full 360-degree logical turret rotation
- [x] Turret smoothly follows mouse target
- [x] Vehicle-specific turret traverse speed
- [x] Hull rotation correctly accounted for in turret local yaw
- [x] Actual gun direction derived from turret orientation
- [x] Aim error calculated in degrees
- [x] Visual sprite rotation separated from gameplay rotation

### Multi-reticle system

- [x] Desired aim reticle
- [x] Actual gun / turret reticle
- [x] Alignment state changes actual-reticle color
- [~] Reticles currently drawn as native HUD crosses
- [ ] Replace debug crosses with final Iron Grid sprite reticles
- [ ] Ballistic impact reticle
- [ ] Obstructed-shot indication
- [ ] Reload indicator
- [ ] Target lock / target information state

**Status:** Core aiming system functional.

</details>

<details open>
<summary><strong>Phase 3 — Camera System</strong></summary>

### Top-down camera

- [x] Orthographic top-down camera
- [x] Spring-arm hierarchy
- [x] Fixed camera mode
- [x] Speed Reactive camera mode
- [x] Camera zooms out as speed increases
- [x] Camera moves closer as speed decreases
- [x] Orthographic zoom controlled through `OrthoWidth`
- [x] Perspective-compatible spring-arm distance handling
- [x] Smooth interpolated transitions
- [x] Runtime camera-response setting
- [x] Slower default camera response for smoother motion
- [x] C toggles Fixed / Speed Reactive modes

### Camera tuning

- [x] Fixed zoom setting
- [x] Dynamic maximum zoom setting
- [x] Camera response-speed setting
- [ ] Manual mouse-wheel zoom
- [ ] Movement-direction look-ahead
- [ ] Optional aim-direction look-ahead
- [ ] Camera shake / recoil response
- [ ] Spectator camera

**Status:** Two camera modes functional.

</details>

<details open>
<summary><strong>Phase 4 — Pause Menu & Initial Settings</strong></summary>

- [x] P opens pause menu
- [x] P closes pause menu
- [x] Resume button
- [x] Camera mode setting
- [x] Camera response slider
- [x] Fixed zoom slider
- [x] Dynamic maximum zoom slider
- [x] Basic controls reference
- [x] Standalone mode pauses the local world
- [x] Multiplayer-aware behavior prevents pausing the authoritative server
- [ ] Final SNES-HD menu skin
- [ ] Audio settings
- [ ] Graphics settings
- [ ] Resolution / fullscreen settings
- [ ] Control rebinding
- [ ] Mouse sensitivity
- [ ] Persistent settings save/load
- [ ] Main menu
- [ ] Accessibility settings

**Status:** Functional prototype menu.

</details>

<details>
<summary><strong>Phase 5 — Weapons & Ballistics</strong></summary>

- [x] Fire input
- [x] Projectile actor
- [x] Server-authoritative firing
- [~] Muzzle flash hook
- [ ] Recoil
- [x] Reload timing
- [x] Basic ammunition inventory / reserve ammo
- [ ] AP ammunition
- [ ] HE ammunition
- [ ] HEAT ammunition
- [ ] Smoke ammunition
- [x] Projectile velocity
- [x] Gravity / shell drop
- [x] Range-dependent ballistic trajectory
- [ ] Ricochet logic
- [~] Impact FX hook
- [x] Ballistic impact reticle
- [ ] Rangefinder
- [ ] Optional stabilizer system

**Status:** Weapons & Ballistics v1 functional; ammo types, ricochet, recoil, final FX, and penetration remain planned.

</details>

<details>
<summary><strong>Phase 6 — Armor, Damage & Vehicle Modules</strong></summary>

- [ ] Armor thickness / armor zones
- [ ] Impact angle calculation
- [ ] Penetration calculation
- [ ] Hull damage
- [ ] Engine module
- [ ] Transmission module
- [ ] Left track module
- [ ] Right track module
- [ ] Turret drive module
- [ ] Gun breech / weapon module
- [ ] Ammo rack
- [ ] Fire system
- [ ] Repair system
- [ ] Destroyed tank state
- [ ] Persistent physical wreck / battlefield cover
- [ ] Burning wreck effects

</details>

<details>
<summary><strong>Phase 7 — Multiplayer Foundation</strong></summary>

- [x] Dedicated Server build target
- [~] Multiplayer-first architecture considered in current systems
- [ ] Tank movement replication
- [ ] Turret rotation replication
- [ ] Aim-state replication
- [x] Projectile replication
- [ ] Damage replication
- [ ] Server authority validation
- [ ] Client prediction
- [ ] Reconciliation
- [ ] Network relevancy / interest management
- [ ] 64-player load test
- [ ] Join / leave handling
- [ ] Session system
- [ ] Matchmaking
- [ ] Reconnect handling
- [ ] Anti-cheat strategy

**Status:** Server target ready; gameplay networking not yet implemented.

</details>

<details>
<summary><strong>Phase 8 — Battle Royale Game Mode</strong></summary>

- [ ] 64-player match flow
- [ ] Solo mode
- [ ] Duo mode
- [ ] Squad mode
- [ ] Spawn / insertion system
- [ ] Safe zone
- [ ] Shrinking combat area
- [ ] Zone pressure / damage mechanic
- [ ] Supply drops
- [ ] Tank equipment loot
- [ ] Ammunition loot
- [ ] Repair items
- [ ] Vehicle upgrades
- [ ] Player elimination
- [ ] Spectating
- [ ] Last-player / last-team victory
- [ ] Match result screen

</details>

<details>
<summary><strong>Phase 9 — Open World</strong></summary>

- [ ] World Partition map
- [ ] Large prototype battlefield
- [ ] City biome
- [ ] Village biome
- [ ] Forest biome
- [ ] Rivers / water areas
- [ ] Bridges
- [ ] Military bases
- [ ] Industrial areas
- [ ] Hills / elevation
- [ ] Roads
- [ ] Battle City-inspired brick districts
- [ ] Steel fortifications
- [ ] Destructible brick walls
- [ ] Destructible structures
- [ ] Tank wreck persistence
- [ ] HLOD / streaming optimization
- [ ] PCG-assisted environment generation

</details>

<details>
<summary><strong>Phase 10 — SNES-HD Art Pipeline</strong></summary>

- [x] SNES-inspired HD visual direction selected
- [x] 2.5D / sprite-based presentation selected
- [x] Hull and turret assets defined as separate layers
- [x] Runtime 360-degree turret rotation chosen instead of directional turret sprites
- [x] Sprite import workflow documented
- [x] Reticle concept sheet created outside the repository
- [x] Tank / environment concept sheets created outside the repository
- [ ] Final production hull PNG set
- [ ] Final production turret PNG set
- [ ] Final reticle PNG set
- [ ] Final projectile / FX sprite set
- [ ] Environment tile set
- [ ] Structure sprite set
- [ ] Destruction sprite set
- [ ] UI sprite set
- [ ] Assets imported and committed to Unreal Content
- [ ] Final palette / style bible

**Status:** Art direction defined; production assets pending.

</details>

<details>
<summary><strong>Phase 11 — Audio & Feedback</strong></summary>

- [ ] Tank engine loops
- [ ] Track sounds
- [ ] Turret motor sound
- [ ] Cannon firing
- [ ] Shell impacts
- [ ] Ricochets
- [ ] Explosions
- [ ] Destruction
- [ ] UI sounds
- [ ] Low-health / fire warnings
- [ ] Dynamic battle ambience

</details>

<details>
<summary><strong>Phase 12 — Optimization & Release</strong></summary>

- [ ] 64-player CPU profiling
- [ ] 64-player server profiling
- [ ] Network bandwidth profiling
- [ ] Sprite draw-call optimization
- [ ] Texture atlas strategy
- [ ] World Partition tuning
- [ ] HLOD tuning
- [ ] Dedicated server deployment
- [ ] Crash reporting
- [ ] Logging / telemetry
- [ ] Packaging
- [ ] Closed multiplayer test
- [ ] Balance pass
- [ ] Release candidate

</details>

---

## Recommended next development order

1. **Armor / Penetration Prototype**
   - armor zones
   - impact angle
   - penetration
   - module damage
   - ricochet foundation

2. **Weapon Polish**
   - AP / HE / HEAT ammo types
   - recoil
   - production muzzle / impact FX
   - rangefinder

3. **Replication Prototype**
   - two-player dedicated-server test
   - movement
   - turret
   - firing
   - damage

4. **Battle Royale Vertical Slice**
   - small test map
   - 8–16 players first
   - safe zone
   - elimination
   - winner state

5. **Scale toward 64 players**
   - relevancy
   - optimization
   - larger world
   - matchmaking

---

## Visual architecture

Tank rendering is split into independent layers:

1. **Hull sprite** — rotates with vehicle movement.
2. **Turret sprite** — rotates independently through 360 degrees in-engine.
3. **Weapon / muzzle FX** — anchored to the turret muzzle socket / pivot.
4. **Reticle system** — separates desired mouse aim from actual gun / turret aim.

## Repository structure

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

See `Docs/AimingSystem.md` for the detailed aiming and multi-reticle design.  
See `Docs/MovementSystem.md` for the Tracked Movement v2 model and tuning parameters.  
See `Docs/WeaponSystem.md` for cannon firing, reload, projectile, and ballistic prediction details.
