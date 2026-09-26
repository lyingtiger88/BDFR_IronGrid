# IRON GRID: BATTLEGROUND — Development Roadmap

> **Current milestone:** Playable Tank Prototype  
> **Engine:** Unreal Engine 5.8.x  
> **Repository:** `BDFR_IronGrid`

This roadmap tracks what is already implemented in the repository and what remains for the full 64-player open-world battle royale vision.

## Legend

- [x] Implemented in repository
- [~] Partially implemented / prototype quality
- [ ] Planned

---

# Phase 0 — Project Foundation

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

**Status: COMPLETE**

---

# Phase 1 — Core Tank Prototype

## Tank representation

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

## Basic movement

- [x] W / S forward and reverse prototype movement
- [x] A / D hull turning
- [~] Collision-aware actor movement
- [ ] Acceleration and deceleration
- [ ] Braking model
- [ ] Independent left/right tracks
- [ ] Differential steering
- [ ] Neutral steering / pivot turn
- [ ] Engine torque
- [ ] Transmission / gears
- [ ] Surface-dependent traction
- [ ] Track damage affecting mobility

**Status: BASIC MOVEMENT PLAYABLE — ADVANCED TRACK PHYSICS NOT STARTED**

---

# Phase 2 — War Thunder-style Aiming

## Mouse aiming

- [x] Mouse cursor projected into the game world
- [x] Surface hit under cursor used as desired aim point
- [x] Fallback flat-plane aiming
- [x] Desired aim point independent of hull direction

## Turret tracking

- [x] Full 360-degree logical turret rotation
- [x] Turret smoothly follows mouse target
- [x] Vehicle-specific turret traverse speed variable
- [x] Hull rotation correctly accounted for in turret local yaw
- [x] Actual gun direction derived from turret orientation
- [x] Aim error calculated in degrees
- [x] Visual sprite rotation separated from gameplay rotation

## Multi-reticle system

- [x] Desired aim reticle
- [x] Actual gun/turret reticle
- [x] Alignment state changes actual reticle color
- [~] Reticles currently drawn as native HUD crosses
- [ ] Replace debug/native crosses with final Iron Grid sprite reticles
- [ ] Ballistic impact reticle
- [ ] Obstructed-shot indication
- [ ] Reload indicator
- [ ] Target lock / target information state

**Status: CORE AIMING SYSTEM FUNCTIONAL**

---

# Phase 3 — Camera System

## Top-down camera

- [x] Orthographic top-down camera
- [x] Spring arm camera hierarchy
- [x] Fixed camera mode
- [x] Speed-reactive camera mode
- [x] Camera zooms out as tank speed increases
- [x] Camera moves back in as speed decreases
- [x] Orthographic zoom handled with `OrthoWidth`
- [x] Perspective-compatible spring-arm distance handling
- [x] Smooth interpolated camera transitions
- [x] Camera response speed exposed to Blueprint/runtime settings
- [x] Reduced default camera response speed for smoother motion
- [x] C key toggles Fixed / Speed Reactive modes

## Camera tuning

- [x] Fixed zoom setting
- [x] Dynamic maximum zoom setting
- [x] Camera response-speed setting
- [ ] Manual mouse-wheel zoom
- [ ] Camera look-ahead based on movement direction
- [ ] Optional look-ahead toward mouse aim
- [ ] Camera shake / recoil response
- [ ] Spectator camera

**Status: TWO CAMERA MODES FUNCTIONAL**

---

# Phase 4 — Pause Menu & Initial Settings

- [x] P key opens pause menu
- [x] P key closes pause menu
- [x] Resume button
- [x] Camera mode setting
- [x] Camera response slider
- [x] Fixed zoom slider
- [x] Dynamic maximum zoom slider
- [x] Basic controls reference
- [x] Standalone mode pauses the local world
- [x] Multiplayer-aware behavior: opening the menu will not pause the authoritative server
- [ ] SNES-HD final menu skin
- [ ] Audio settings
- [ ] Graphics settings
- [ ] Resolution / fullscreen settings
- [ ] Control rebinding
- [ ] Mouse sensitivity
- [ ] Save/load settings between sessions
- [ ] Main menu
- [ ] Accessibility settings

**Status: FUNCTIONAL PROTOTYPE MENU**

---

# Phase 5 — Weapons & Ballistics

- [ ] Fire input
- [ ] Projectile actor
- [ ] Server-authoritative firing
- [ ] Muzzle flash
- [ ] Recoil
- [ ] Reload timing
- [ ] Ammunition inventory
- [ ] AP ammunition
- [ ] HE ammunition
- [ ] HEAT ammunition
- [ ] Smoke ammunition
- [ ] Projectile velocity
- [ ] Gravity / shell drop
- [ ] Range-dependent trajectory
- [ ] Ricochet logic
- [ ] Impact effects
- [ ] Ballistic impact reticle
- [ ] Rangefinder
- [ ] Optional stabilizer system

**Status: NOT STARTED**

---

# Phase 6 — Armor, Damage & Vehicle Modules

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

**Status: NOT STARTED**

---

# Phase 7 — Multiplayer Foundation

- [x] Dedicated Server build target
- [~] Multiplayer-first project architecture considered in current systems
- [ ] Tank movement replication
- [ ] Turret rotation replication
- [ ] Aim-state replication
- [ ] Projectile replication
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

**Status: SERVER TARGET READY — GAMEPLAY NETWORKING NOT IMPLEMENTED**

---

# Phase 8 — Battle Royale Game Mode

- [ ] 64-player match flow
- [ ] Solo mode
- [ ] Duo mode
- [ ] Squad mode
- [ ] Spawn / insertion system
- [ ] Safe zone
- [ ] Shrinking combat area
- [ ] Zone damage / pressure mechanic
- [ ] Supply drops
- [ ] Tank equipment loot
- [ ] Ammunition loot
- [ ] Repair items
- [ ] Vehicle upgrades
- [ ] Player elimination
- [ ] Spectating
- [ ] Last-player / last-team victory
- [ ] Match result screen

**Status: PLANNED**

---

# Phase 9 — Open World

- [ ] World Partition map
- [ ] Large prototype battlefield
- [ ] City biome
- [ ] Village biome
- [ ] Forest biome
- [ ] River / water areas
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

**Status: PLANNED**

---

# Phase 10 — SNES-HD Art Pipeline

- [x] SNES-inspired HD visual direction selected
- [x] 2.5D / sprite-based presentation selected
- [x] Hull and turret assets defined as separate layers
- [x] Runtime 360-degree turret rotation chosen instead of directional turret sprites
- [x] Sprite import workflow documented
- [x] Reticle concept sheet created outside the repository
- [x] Tank / environment sprite concept sheets created outside the repository
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

**Status: ART DIRECTION DEFINED — PRODUCTION ASSETS PENDING**

---

# Phase 11 — Audio & Feedback

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

**Status: PLANNED**

---

# Phase 12 — Optimization & Release

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

**Status: FUTURE**

---

# Current Completed Milestone Summary

At this stage, **IRON GRID: BATTLEGROUND has a functional C++ tank-control prototype** with:

- Unreal Engine 5.8 project foundation
- Dedicated Server target
- Independent hull and turret visual architecture
- Corrected sprite/art rotation pipeline
- 360-degree mouse-following turret
- Desired-vs-actual aim system
- Basic tank movement
- Fixed and speed-reactive top-down camera modes
- Runtime camera settings
- Pause menu with camera controls
- Multiplayer-safe pause-menu behavior

The next major milestone is **Tank Movement v2**, followed by **Weapons & Ballistics**, then **network replication**.

---

# Recommended Next Development Order

1. **Tank Movement v2**
   - acceleration
   - braking
   - differential tracks
   - neutral steering

2. **Weapon Prototype**
   - fire
   - projectile
   - reload
   - muzzle FX
   - shell impact

3. **Ballistics & Damage**
   - shell drop
   - penetration
   - armor
   - modules

4. **Replication Prototype**
   - two-player dedicated-server test
   - movement
   - turret
   - firing
   - damage

5. **Battle Royale Vertical Slice**
   - small test map
   - 8–16 players first
   - safe zone
   - elimination
   - winner state

6. **Scale toward 64 players**
   - relevancy
   - optimization
   - larger world
   - matchmaking
