# IRON GRID — Tracked Movement v2

The current tank prototype uses a lightweight differential-track model rather than directly translating and rotating the pawn from input.

## Controls

- **W / S** — forward / reverse throttle
- **A / D** — differential steering
- **A / D with no throttle** — neutral / pivot steering
- **Space** — brake

## Core model

The tank maintains independent runtime speeds for:

- `CurrentLeftTrackSpeed`
- `CurrentRightTrackSpeed`

Forward velocity is the average of the two track speeds:

```
ForwardSpeed = (LeftTrackSpeed + RightTrackSpeed) / 2
```

Hull yaw is generated from their difference:

```
YawRateRadians = (LeftTrackSpeed - RightTrackSpeed) / TrackSeparation
```

This means steering emerges from the tracks instead of being applied as an unrelated actor rotation.

## Acceleration and braking

Each track approaches its target speed independently.

Tuning parameters:

- `TrackAcceleration`
- `TrackDeceleration`
- `BrakeDeceleration`
- `MaxMoveSpeed`
- `MaxReverseSpeed`

Changing directly from forward to reverse uses the stronger braking rate so the tank must shed momentum before accelerating in the opposite direction.

## Differential steering

While moving, steering applies a speed differential between the two tracks.

Relevant parameters:

- `MovingSteeringStrength`
- `HighSpeedSteeringScale`
- `HullTurnSpeed`
- `TrackSeparation`

Steering authority is intentionally reduced at high speed to prevent arcade-like instant rotation.

## Neutral / pivot steering

When throttle is near zero and steering input is present, the tracks run in opposite directions:

```
LeftTrack  =  Steering * PivotTrackSpeed
RightTrack = -Steering * PivotTrackSpeed
```

This allows the tank to rotate around its center while nearly stationary.

## Collision response

Movement uses swept actor translation. On a blocking hit, track speeds are damped using:

`CollisionSpeedRetention`

This is a prototype response and will later be replaced or extended by terrain traction, obstacle interaction, damaged-track behavior, and network-authoritative movement.

## Current limitations

Tracked Movement v2 is still a gameplay model, not a full rigid-body tank simulation.

Planned later:

- engine torque curve
- transmission / gears
- terrain-dependent traction
- slope response
- damaged-track asymmetry
- track animation / visual speed
- server-authoritative prediction and reconciliation
