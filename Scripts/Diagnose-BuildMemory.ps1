param()

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "IRON GRID - BUILD MEMORY DIAGNOSTICS" -ForegroundColor Cyan
Write-Host ""

$OS = Get-CimInstance Win32_OperatingSystem
$PageFiles = Get-CimInstance Win32_PageFileUsage
$Computer = Get-CimInstance Win32_ComputerSystem

function ToGB([double]$KB) {
    return [math]::Round($KB / 1MB, 2)
}

$PhysicalTotalGB = [math]::Round($Computer.TotalPhysicalMemory / 1GB, 2)
$PhysicalFreeGB = ToGB $OS.FreePhysicalMemory
$VirtualTotalGB = ToGB $OS.TotalVirtualMemorySize
$VirtualFreeGB = ToGB $OS.FreeVirtualMemory

Write-Host "Physical RAM total : $PhysicalTotalGB GB"
Write-Host "Physical RAM free  : $PhysicalFreeGB GB"
Write-Host "Virtual total      : $VirtualTotalGB GB"
Write-Host "Virtual free       : $VirtualFreeGB GB"
Write-Host ""

if ($PageFiles) {
    Write-Host "Page file usage:" -ForegroundColor Yellow
    foreach ($PF in $PageFiles) {
        $AllocatedGB = [math]::Round($PF.AllocatedBaseSize / 1024, 2)
        $CurrentGB = [math]::Round($PF.CurrentUsage / 1024, 2)
        $PeakGB = [math]::Round($PF.PeakUsage / 1024, 2)
        Write-Host "  $($PF.Name)"
        Write-Host "    Allocated: $AllocatedGB GB"
        Write-Host "    Current  : $CurrentGB GB"
        Write-Host "    Peak     : $PeakGB GB"
    }
} else {
    Write-Host "No active Windows page file was reported." -ForegroundColor Red
}

Write-Host ""
Write-Host "Fixed drives:" -ForegroundColor Yellow
Get-CimInstance Win32_LogicalDisk -Filter "DriveType=3" | ForEach-Object {
    $FreeGB = [math]::Round($_.FreeSpace / 1GB, 1)
    $SizeGB = [math]::Round($_.Size / 1GB, 1)
    Write-Host "  $($_.DeviceID)  Free: $FreeGB GB / $SizeGB GB"
}

Write-Host ""
Write-Host "If Virtual total is low or the page file is missing/small, increase Windows virtual memory and reboot."