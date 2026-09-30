param(
    [string]$EngineRoot = "D:\\GameDev\\UE_5.8",
    [ValidateSet("Development","DebugGame","Shipping")]
    [string]$Configuration = "Development",
    [ValidateRange(1, 16)]
    [int]$MaxParallelActions = 1
)

$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $RepoRoot "BDFR_IronGrid.uproject"
$BuildBat = Join-Path $EngineRoot "Engine\\Build\\BatchFiles\\Build.bat"

if (-not (Test-Path $ProjectFile)) { throw "Project file not found: $ProjectFile" }
if (-not (Test-Path $BuildBat)) { throw "Unreal Build.bat not found: $BuildBat. Pass -EngineRoot with your UE 5.8 path." }

$ConfigDir = Join-Path $env:APPDATA "Unreal Engine\\UnrealBuildTool"
$ConfigFile = Join-Path $ConfigDir "BuildConfiguration.xml"
New-Item -ItemType Directory -Force -Path $ConfigDir | Out-Null

if (Test-Path $ConfigFile) {
    $Backup = "$ConfigFile.bak"
    Copy-Item $ConfigFile $Backup -Force
    Write-Host "Backed up existing UBT configuration to: $Backup"
}

$Xml = @"
<?xml version="1.0" encoding="utf-8"?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
  <BuildConfiguration>
    <bAllowXGE>false</bAllowXGE>
    <bAllowFASTBuild>false</bAllowFASTBuild>
  </BuildConfiguration>
  <ParallelExecutor>
    <MaxProcessorCount>$MaxParallelActions</MaxProcessorCount>
    <ProcessorCountMultiplier>1</ProcessorCountMultiplier>
    <MemoryPerActionBytes>0</MemoryPerActionBytes>
  </ParallelExecutor>
</Configuration>
"@
Set-Content -Path $ConfigFile -Value $Xml -Encoding UTF8

Write-Host ""
Write-Host "IRON GRID - LOW MEMORY BUILD" -ForegroundColor Cyan
Write-Host "Project: $ProjectFile"
Write-Host "Engine : $EngineRoot"
Write-Host "Parallel compiler actions: $MaxParallelActions"
Write-Host ""

& $BuildBat BDFR_IronGridEditor Win64 $Configuration "-Project=$ProjectFile" -WaitMutex -architecture=x64
$ExitCode = $LASTEXITCODE

Write-Host ""
if ($ExitCode -eq 0) {
    Write-Host "Build completed successfully." -ForegroundColor Green
} else {
    Write-Host "Build failed with exit code $ExitCode." -ForegroundColor Red
    Write-Host "Run .\\Scripts\\Diagnose-BuildMemory.ps1 and check the page-file/commit values."
}

exit $ExitCode