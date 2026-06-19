<#
.SYNOPSIS
  HangulInputSlate를 설치된 모든 대상 플랫폼으로 빌드 검증한다 (RunUAT BuildPlugin).

  Win64는 항상 빌드. Linux는 크로스컴파일 툴체인(LINUX_MULTIARCH_ROOT)이 설정돼 있으면
  자동 포함. Mac은 Windows 호스트에서 빌드 불가라 건너뛴다.

.PARAMETER EnginePath
  UE 설치 경로 (예: C:\Program Files\Epic Games\UE_5.7). 미지정 시 런처 매니페스트에서 UE_5.7 자동 탐지.

.PARAMETER OutputDir
  패키지 출력 폴더. 기본: %TEMP%\HangulInputSlate_Build.

.PARAMETER Platforms
  대상 플랫폼 직접 지정 (예: Win64,Linux). 미지정 시 자동 감지.

.EXAMPLE
  ./Scripts/build-plugin.ps1
  ./Scripts/build-plugin.ps1 -EnginePath "C:\Program Files\Epic Games\UE_5.7"
#>
param(
    [string]$EnginePath = $env:UE_ENGINE_PATH,
    [string]$OutputDir = (Join-Path $env:TEMP "HangulInputSlate_Build"),
    [string[]]$Platforms
)
$ErrorActionPreference = "Stop"

$PluginRoot = Split-Path -Parent $PSScriptRoot
$UPlugin = Join-Path $PluginRoot "HangulInputSlate.uplugin"
if (-not (Test-Path $UPlugin)) { throw "uplugin을 찾을 수 없음: $UPlugin" }

if (-not $EnginePath) {
    $manifest = "C:\ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat"
    if (Test-Path $manifest) {
        $ue = (Get-Content $manifest -Raw | ConvertFrom-Json).InstallationList |
            Where-Object { $_.AppName -eq "UE_5.7" } | Select-Object -First 1
        if ($ue) { $EnginePath = $ue.InstallLocation }
    }
}
if (-not $EnginePath -or -not (Test-Path $EnginePath)) {
    throw "UE 엔진 경로를 찾을 수 없음. -EnginePath 로 지정하세요 (예: C:\Program Files\Epic Games\UE_5.7)."
}

$RunUAT = Join-Path $EnginePath "Engine\Build\BatchFiles\RunUAT.bat"
if (-not (Test-Path $RunUAT)) { throw "RunUAT.bat 없음: $RunUAT" }

if (-not $Platforms) {
    $Platforms = @("Win64")
    if ($env:LINUX_MULTIARCH_ROOT -and (Test-Path $env:LINUX_MULTIARCH_ROOT)) {
        $Platforms += "Linux"
    } else {
        Write-Host "[i] Linux 툴체인(LINUX_MULTIARCH_ROOT) 미설정 — Linux 건너뜀." -ForegroundColor Yellow
    }
    Write-Host "[i] Mac은 Windows 호스트에서 빌드 불가 — 건너뜀 (Mac에서 별도 검증 필요)." -ForegroundColor Yellow
}
$TargetPlatforms = $Platforms -join "+"
New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
$logFile = Join-Path $OutputDir "_build.log"

Write-Host "==> 플러그인: $UPlugin"
Write-Host "==> 엔진:     $EnginePath"
Write-Host "==> 요청 플랫폼: $TargetPlatforms"
Write-Host "==> 출력:     $OutputDir`n"

& $RunUAT BuildPlugin -Plugin="$UPlugin" -Package="$OutputDir" -TargetPlatforms="$TargetPlatforms" -Rocket 2>&1 | Tee-Object -FilePath $logFile
$code = $LASTEXITCODE

# Installed 엔진은 미설치 타깃을 조용히 제외하므로, 요청한 플랫폼이 실제로 빌드됐는지 확인한다.
$built = ""
$m = Select-String -Path $logFile -Pattern "Building plugin for target platforms:\s*(.+)" | Select-Object -Last 1
if ($m) { $built = $m.Matches[0].Groups[1].Value.Trim() }
$dropped = $Platforms | Where-Object { $built -notmatch [regex]::Escape($_) }

if ($code -ne 0) {
    Write-Host "`nBUILD FAILED (ExitCode=$code)" -ForegroundColor Red
} elseif ($dropped) {
    Write-Host "`n[!] 요청했으나 빌드되지 않은 플랫폼: $($dropped -join ', ')" -ForegroundColor Yellow
    Write-Host "    이 엔진에 해당 타깃 플랫폼이 설치돼 있지 않을 수 있습니다 (Installed 엔진 한계)." -ForegroundColor Yellow
    Write-Host "BUILD OK but INCOMPLETE — 실제 빌드 타깃: $built" -ForegroundColor Yellow
} else {
    Write-Host "`nBUILD SUCCESSFUL — 실제 빌드 타깃: $built" -ForegroundColor Green
}
exit $code
