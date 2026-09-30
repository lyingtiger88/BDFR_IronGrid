# Automatic project cleanup

The repository includes:

`Scripts/Clean-Project.ps1`

It removes only generated Unreal/Visual Studio files and does not touch project source or assets.

## Default cleanup

Run from the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Clean-Project.ps1
```

Default targets:

- `Intermediate/`
- `Binaries/`
- `.vs/`
- `BDFR_IronGrid.sln`

The following are always preserved:

- `Source/`
- `Content/`
- `Config/`
- `Plugins/`
- `Docs/`
- `Scripts/`
- `BDFR_IronGrid.uproject`

## Deep cleanup

To also remove the project's local Derived Data Cache:

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Clean-Project.ps1 -Deep
```

`Saved/` is intentionally not deleted by this script because it may contain logs, screenshots, autosaves, and useful debugging data.

## Clean and regenerate Visual Studio project files

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Clean-Project.ps1 -RegenerateProjectFiles
```

Custom Unreal Engine location:

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Clean-Project.ps1 -RegenerateProjectFiles -EngineRoot "D:\GameDev\UE_5.8"
```

## Preview without deleting

The script supports PowerShell's `-WhatIf`:

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Clean-Project.ps1 -WhatIf
```

The script refuses to clean while Unreal Editor, Visual Studio, or MSBuild is running.
