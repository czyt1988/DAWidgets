#Requires -Version 5.1
<#
.SYNOPSIS
    重新生成 DAWidgets 单文件合并版本（src-amalgamate/DAWidgets.h + DAWidgets.cpp）。

.DESCRIPTION
    流程与参考项目 qwt 的 tools/Amalgamate.sh 一致：

      1. 用 Qt 的 uic 把 src/*.ui 生成 ui_*.h；
      2. 用 Qt 的 rcc 把 src/resources/dawidgetsicons.qrc 生成 qrc_dawidgetsicons.cpp；
      3. 用 Amalgamate.exe 按 tools/amalgamate/ 下的模板，把全部公开头文件
         合并为 src-amalgamate/DAWidgets.h、全部实现合并为 src-amalgamate/DAWidgets.cpp。

    这两个输出文件是自动生成的，不要手工编辑。增删底层 .h/.cpp 时先更新
    tools/amalgamate/DAWidgetsAmalgamTemplatePublicHeaders.h 与
    tools/amalgamate/DAWidgetsAmalgamTemplate.cpp，再运行本脚本。

.PARAMETER QtBin
    包含 uic.exe / rcc.exe 的 Qt bin 目录。缺省时依次尝试：
    已配置构建目录 CMakeCache.txt 里的 Qt6_DIR / Qt5_DIR（build、build-tests、
    build-qt5、build-*），最后尝试 PATH 上的 qmake。

.PARAMETER BuildDir
    作为 Qt 路径来源的构建目录（相对仓库根）。缺省自动探测。

.EXAMPLE
    pwsh -File tools/Amalgamate.ps1

.EXAMPLE
    pwsh -File tools/Amalgamate.ps1 -QtBin "D:\Qt\6.7.3\msvc2019_64\bin"
#>
[CmdletBinding()]
param(
    [string]$QtBin,
    [string]$BuildDir
)

$ErrorActionPreference = 'Stop'

$RepoRoot = Split-Path -Parent $PSScriptRoot
$SrcDir = Join-Path $RepoRoot 'src'
$TplDir = Join-Path $PSScriptRoot 'amalgamate'
$GenDir = Join-Path $TplDir 'generated'
$OutDir = Join-Path $RepoRoot 'src-amalgamate'
$AmalgamateExe = Join-Path $PSScriptRoot 'Amalgamate.exe'

function Write-Step([string]$Text) {
    Write-Host ''
    Write-Host "==> $Text" -ForegroundColor Cyan
}

# 生成的 .cpp 中，rcc 会把资源文件的绝对路径写进注释。这里把本机仓库路径去掉，
# 让输出只剩 src/resources/... 之类的相对路径；同时统一成 CRLF + UTF-8 BOM，
# 与仓库既有源码一致（MSVC 无 /utf-8 时也能正确识别中文注释）。
function Finalize-AmalgamFile([string]$Path) {
    $text = [System.IO.File]::ReadAllText($Path, [System.Text.Encoding]::UTF8)
    $repoPosix = ($RepoRoot -replace '\\', '/') + '/'
    $text = $text.Replace($repoPosix, '')
    $text = ($text -replace "`r`n", "`n") -replace "`n", "`r`n"
    $encoding = New-Object System.Text.UTF8Encoding($true)
    [System.IO.File]::WriteAllText($Path, $text, $encoding)
}

function Resolve-QtBinFromCache([string]$CachePath) {
    if (-not (Test-Path $CachePath)) { return $null }
    $lines = Get-Content $CachePath
    foreach ($key in @('Qt6_DIR', 'Qt5_DIR', 'QT_DIR')) {
        $pattern = '^' + [regex]::Escape($key) + ':PATH=(.+)$'
        $line = $lines | Where-Object { $_ -match $pattern } | Select-Object -First 1
        if (-not $line) { continue }
        $cmakeDir = ($line -replace $pattern, '$1').Trim()
        # <prefix>/lib/cmake/Qt6 -> <prefix>/bin
        $prefix = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $cmakeDir))
        $bin = Join-Path $prefix 'bin'
        if ((Test-Path (Join-Path $bin 'uic.exe')) -or (Test-Path (Join-Path $bin 'uic'))) {
            return $bin
        }
    }
    return $null
}

function Resolve-QtBin([string]$Explicit, [string]$PreferredBuildDir) {
    if ($Explicit) {
        if (-not (Test-Path $Explicit)) { throw "指定的 QtBin 不存在: $Explicit" }
        return (Resolve-Path $Explicit).Path
    }

    $candidates = @()
    if ($PreferredBuildDir) { $candidates += $PreferredBuildDir }
    foreach ($name in @('build', 'build-tests', 'build-qt5', 'build-qt6')) {
        $candidates += $name
    }
    $candidates += (Get-ChildItem -Path $RepoRoot -Directory -Filter 'build*' -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -notin $candidates } | ForEach-Object { $_.Name })

    foreach ($name in $candidates) {
        $cache = Join-Path (Join-Path $RepoRoot $name) 'CMakeCache.txt'
        $bin = Resolve-QtBinFromCache $cache
        if ($bin) {
            Write-Host "使用构建目录 $name 中的 Qt: $bin"
            return $bin
        }
    }

    $qmake = Get-Command qmake -ErrorAction SilentlyContinue
    if ($qmake) {
        $bin = (& $qmake.Source -query QT_INSTALL_BINS 2>$null | Select-Object -First 1)
        if ($bin -and ((Test-Path (Join-Path $bin 'uic.exe')) -or (Test-Path (Join-Path $bin 'uic')))) {
            Write-Host "使用 PATH 上 qmake 报告的 Qt: $bin"
            return $bin
        }
    }

    return $null
}

function Resolve-Tool([string]$BinDir, [string]$BaseName) {
    foreach ($ext in @('.exe', '')) {
        $path = Join-Path $BinDir ($BaseName + $ext)
        if (Test-Path $path) { return $path }
    }
    return $null
}

$resolvedQtBin = Resolve-QtBin $QtBin $BuildDir
if (-not $resolvedQtBin) {
    throw @'
找不到 Qt 的 uic/rcc。请用 -QtBin 指定 Qt 的 bin 目录，例如：
  pwsh -File tools/Amalgamate.ps1 -QtBin "D:\Qt\6.7.3\msvc2019_64\bin"
或者先在本仓库任意构建目录成功配置一次 CMake（脚本会从 CMakeCache.txt 读取 Qt 路径）。
'@
}
$uic = Resolve-Tool $resolvedQtBin 'uic'
$rcc = Resolve-Tool $resolvedQtBin 'rcc'
if (-not $uic -or -not $rcc) {
    throw "Qt bin 目录中缺少 uic/rcc: $resolvedQtBin"
}
if (-not (Test-Path $AmalgamateExe)) {
    throw "缺少合并工具: $AmalgamateExe"
}

Write-Step "清理生成目录"
if (Test-Path $GenDir) { Remove-Item -Recurse -Force $GenDir }
New-Item -ItemType Directory -Force -Path $GenDir, $OutDir | Out-Null

Write-Step "生成 .ui 的 ui_*.h（uic）"
$uiFiles = Get-ChildItem -Path $SrcDir -Filter '*.ui' | Sort-Object Name
if ($uiFiles.Count -eq 0) { throw "src/ 下没有找到 .ui 文件" }
foreach ($ui in $uiFiles) {
    $out = Join-Path $GenDir ('ui_' + $ui.BaseName + '.h')
    & $uic $ui.FullName -o $out
    if ($LASTEXITCODE -ne 0) { throw "uic 失败: $($ui.Name)" }
    Write-Host "    ui_$($ui.BaseName).h"
}

Write-Step "生成资源代码 qrc_dawidgetsicons.cpp（rcc）"
$qrcFile = Join-Path $SrcDir 'resources/dawidgetsicons.qrc'
if (-not (Test-Path $qrcFile)) { throw "缺少资源文件: $qrcFile" }
& $rcc --name dawidgetsicons $qrcFile -o (Join-Path $GenDir 'qrc_dawidgetsicons.cpp')
if ($LASTEXITCODE -ne 0) { throw 'rcc 失败' }

Write-Step "合并头文件 -> src-amalgamate/DAWidgets.h"
$includeArgs = @(
    '-i', $SrcDir,
    '-i', $GenDir,
    '-i', $TplDir,
    '-w', '*.cpp;*.h;*.hpp;*.c'
)
# Amalgamate 在“输出与已有文件完全相同”时返回 1（不算错误），因此先删掉旧文件，
# 让它每次都真正重写，再用退出码判断成败。
$headerOut = Join-Path $OutDir 'DAWidgets.h'
$cppOut = Join-Path $OutDir 'DAWidgets.cpp'
Remove-Item -Force $headerOut, $cppOut -ErrorAction SilentlyContinue
& $AmalgamateExe @includeArgs (Join-Path $TplDir 'DAWidgetsAmalgamTemplate.h') $headerOut
if ($LASTEXITCODE -ne 0) { throw 'Amalgamate 合并头文件失败' }

Write-Step "合并实现 -> src-amalgamate/DAWidgets.cpp"
& $AmalgamateExe @includeArgs (Join-Path $TplDir 'DAWidgetsAmalgamTemplate.cpp') $cppOut
if ($LASTEXITCODE -ne 0) { throw 'Amalgamate 合并实现失败' }

Write-Step "规范化输出（相对路径、CRLF、UTF-8 BOM）"
Finalize-AmalgamFile $headerOut
Finalize-AmalgamFile $cppOut

Write-Step "结果"
foreach ($file in @('DAWidgets.h', 'DAWidgets.cpp')) {
    $path = Join-Path $OutDir $file
    if (-not (Test-Path $path)) { throw "缺少输出文件: $path" }
    $size = [math]::Round((Get-Item $path).Length / 1KB)
    Write-Host ("    src-amalgamate/{0}  ({1} KB)" -f $file, $size)
}
Write-Host ''
Write-Host '完成。这两个文件由工具生成，请勿手工编辑。' -ForegroundColor Green
Write-Host '建议随后编译验证：' -ForegroundColor Yellow
Write-Host '    cmake -S . -B build-amalgamate -DCMAKE_PREFIX_PATH=<Qt> -DDAWIDGETS_BUILD_AMALGAMATE_DEMO=ON -DDAWIDGETS_BUILD_DEMO=OFF'
Write-Host '    cmake --build build-amalgamate --config Release --target dawidgets_amalgamate_demo'
