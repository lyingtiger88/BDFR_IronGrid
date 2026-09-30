# IRON GRID — Audio System v1

The prototype now exposes an audio layer for the tank, weapon, projectile impacts, and power-up pickups.

## Tank audio slots

Open the tank Blueprint and use:

`Class Defaults > IronGrid > Audio`

Assign:

- `EngineLoopSound`
- `TrackLoopSound`
- `TurretLoopSound`
- `CannonFireSound`
- `ReloadStartSound`
- `ReloadCompleteSound`

## Loop requirements

Engine, track, and turret sounds should use a looping SoundWave, SoundCue, or MetaSound.

The code creates three Audio Components:

```
TankRoot
├── EngineAudio
├── TrackAudio
└── TurretPivot
    └── TurretAudio
```

### Engine

Engine pitch changes with track/vehicle activity.

Tuning:

- `EngineVolume`
- `EngineIdlePitch`
- `EngineMaxPitch`

### Tracks

Track volume and pitch increase with left/right track activity, including pivot turns.

Tuning:

- `TrackVolume`
- `TrackMinPitch`
- `TrackMaxPitch`

### Turret motor

The turret loop is audible only while the gun is still traversing toward the desired aim direction.

Tuning:

- `TurretVolume`
- `TurretAudioErrorThreshold`

## Weapon audio

The following one-shot sounds are played from the tank's world location:

- cannon fire
- reload start
- reload complete

Cannon audio is triggered from the same multicast used for muzzle feedback, so multiplayer clients can hear the shot.

Reload start/finish use multicast audio events as well.

## Projectile impact audio

`AIronGridProjectile` exposes:

- `ImpactSound`
- `ImpactSoundVolume`

Assign these on `BP_IronGridProjectile` or another projectile Blueprint child.

Impact audio is played at the hit location.

## Suggested Content folders

```
Content/Audio/
├── Tank/
│   ├── Engine/
│   ├── Tracks/
│   └── Turret/
├── Weapon/
│   ├── Cannon/
│   ├── Reload/
│   └── Impact/
└── PowerUps/
    ├── Spawn/
    └── Pickup/
```

Final production sounds are not stored in the repository yet; the runtime slots and playback logic are ready for imported WAV/SoundCue/MetaSound assets.
