param(
    [switch]$Deep,
    [switch]$RegenerateProjectFiles,
    [string]$EngineRoot = "D:\\GameDev\\UE_5.8"
)

[CmdletBinding(SupportsShouldProcess = $true)]
param()

$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $RepoRoot "BDFR_IronGrid.uproject"

if (-not (Test-Path $ProjectFile)) {
    throw "BDFR_IronGrid.uproject was not found at: $ProjectFile"
}

$BlockingProcesses = @(
    "UnrealEditor",
    "UnrealEditor-Win64-DebugGame",
    "devenv",
    "MSBuild"
)

$Running = Get-Process -ErrorAction SilentlyContinue |
    Where-Object { $BlockingProcesses -contains $_.ProcessName }

if ($Running) {
    Write-Host ""
    Write-Host "Cleanup stopped because build/editor processes are still running:" -ForegroundColor Yellow
    $Running | Select-Object ProcessName, Id | Format-Table -AutoSize
    Write-Host "Close Unreal Editor / Visual Studio and run the script again."
    exit 2
}

$SafeTargets = @(
    (Join-Path $RepoRoot "Intermediate"),
    (Join-Path $RepoRoot "Binaries"),
    (Join-Path $RepoRoot ".vs"),
    (Join-Path $RepoRoot "BDFR_IronGrid.sln")
)

$DeepTargets = @(
    (Join-Path $RepoRoot "DerivedDataCache")
)

if ($Deep) {
    $SafeTargets += $DeepTargets
}

Write-Host ""
Write-Host "IRON GRID - Project Cleanup" -ForegroundColor Cyan
Write-Host "Repository: $RepoRoot"
Write-Host "Deep clean: $Deep"
Write-Host ""

foreach ($Target in $SafeTargets) {
    if (Test-Path $Target) {
        if ($PSCmdlet.ShouldProcess($Target, "Delete generated project data")) {
            Write-Host "Removing: $Target"
            Remove-Item -LiteralPath $Target -Recurse -Force
        }
    }
    else {
        Write-Host "Skip (not found): $Target" -ForegroundColor DarkGray
    }
}

Write-Host ""
Write-Host "Cleanup complete." -ForegroundColor Green
Write-Host "Preserved: Source, Content, Config, Plugins, Docs, Scripts, .uproject"

if ($RegenerateProjectFiles) {
    $GenerateBat = Join-Path $EngineRoot "Engine\\Build\\BatchFiles\\GenerateProjectFiles.bat"

    if (-not (Test-Path $GenerateBat)) {
        throw "GenerateProjectFiles.bat not found: $GenerateBat"
    }

    Write-Host ""
    Write-Host "Regenerating Visual Studio project files..."
    & $GenerateBat "-project=$ProjectFile" -game -engine
    exit $LASTEXITCODE
}
