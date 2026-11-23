#Requires -Version 5.1
<#
.SYNOPSIS
    DAWidgets 多工作树助手：为每个编码任务创建独立的 git worktree（.worktree/）。

.DESCRIPTION
    本仓库的编码任务一律在 <仓库根>/.worktree/<分支名> 下进行，主工作树只保持 master
    并用来集成结果。裸 `git worktree add` 只能建目录；本脚本额外做三件事：

      1. 定位主工作树：无论从主工作树还是从任意工作树里调用，新工作树都落在主工作树的
         .worktree/ 下，不会在任务工作树里再嵌套 .worktree/。
      2. 预配置构建目录：读取主工作树已有 build*/CMakeCache.txt 的生成器、Qt 路径与
         DAWIDGETS_BUILD_* 选项，在新工作树里预配置 build/。默认优先挑选开启了测试
         （DAWIDGETS_BUILD_TESTS=ON）的缓存，保证新工作树能直接跑 ctest。
      3. 安全删除：Windows 上构建产物常被占用，`git worktree remove` 失败后目录会变成
         “git 已注销但删不掉”的空壳。-Remove 会先确认 git 侧已注销，再重试删除；并且
         拒绝删除主工作树与未注册的普通目录。

    子模块检出保护：DAWidgets 同时是 data-workbench 的子模块。在子模块检出里建工作树会
    把 .worktree/ 塞进 data-workbench 的源码树，且与 data-workbench 的“主工作树改子模块
    + cmake --install”流程冲突，因此脚本直接拒绝。

    文件为 UTF-8 with BOM：Windows PowerShell 5.1 不会把无 BOM 的 UTF-8 当 UTF-8 解析，
    中文输出会变乱码。

.PARAMETER Branch
    分支名。创建时约定 feat/<域>-<主题>、fix/<主题>、docs/<主题>、chore/<主题>；
    分支已存在则直接检出，否则从 -Base 切出。使用 -Remove 时表示工作树目录名
    （如 feat-table-column-picker，即分支名把 / 换成 -）。

.PARAMETER Base
    创建新分支时的基线，默认 master（本仓库唯一集成分支）。

.PARAMETER FromBase
    作为配置模板的构建目录（相对仓库根，如 build、build-tests、build-qt5）。
    缺省自动挑选：优先 DAWIDGETS_BUILD_TESTS=ON 的缓存，其次 build-tests，再次 build。

.PARAMETER NoConfigure
    只创建工作树，不预配置 build/。

.PARAMETER List
    列出所有工作树。

.PARAMETER Remove
    删除工作树（保留分支，合并后用 git branch -d 删除分支）。

.EXAMPLE
    powershell -NoProfile -ExecutionPolicy Bypass -File scripts\worktree-add.ps1 feat/table-column-picker

.EXAMPLE
    pwsh -File scripts\worktree-add.ps1 fix/collapse-jitter master

.EXAMPLE
    pwsh -File scripts\worktree-add.ps1 -List

.EXAMPLE
    pwsh -File scripts\worktree-add.ps1 -Remove feat-table-column-picker
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$Branch,

    [Parameter(Position = 1)]
    [string]$Base = 'master',

    [string]$FromBase,

    [switch]$NoConfigure,
    [switch]$List,
    [switch]$Remove,
    [switch]$Help
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

if ([System.IO.Path]::DirectorySeparatorChar -eq '\') {
    $script:PathComparison = [System.StringComparison]::OrdinalIgnoreCase
} else {
    $script:PathComparison = [System.StringComparison]::Ordinal
}

function Write-Step {
    param([string]$Message)
    Write-Host ''
    Write-Host "=== $Message ==="
}

function Write-Note {
    param([string]$Message)
    Write-Host "  $Message"
}

function Fail {
    param([string]$Message)
    Write-Host "错误: $Message" -ForegroundColor Red
    exit 1
}

function Show-Usage {
    @'
用法:
  scripts\worktree-add.ps1 <分支> [基线分支]     创建工作树（基线缺省 master）
  scripts\worktree-add.ps1 -List                 列出所有工作树
  scripts\worktree-add.ps1 -Remove <目录名>      删除工作树（保留分支）

示例:
  pwsh -File scripts\worktree-add.ps1 feat/table-column-picker
  pwsh -File scripts\worktree-add.ps1 fix/collapse-jitter master
  pwsh -File scripts\worktree-add.ps1 -List
  pwsh -File scripts\worktree-add.ps1 -Remove feat-table-column-picker

工作树位置: <主工作树>\.worktree\<分支名把 / 换成 ->
分支命名:   feat/<域>-<主题> | fix/<主题> | docs/<主题> | chore/<主题>
'@ | Write-Host
}

# ---------------------------------------------------------------------------
# 路径工具
# ---------------------------------------------------------------------------
function Get-NormalizedPath {
    param([string]$Path)
    $full = [System.IO.Path]::GetFullPath($Path)
    while ($full.Length -gt 3 -and ($full.EndsWith('\') -or $full.EndsWith('/'))) {
        $full = $full.Substring(0, $full.Length - 1)
    }
    return $full
}

function Test-SamePath {
    param([string]$Left, [string]$Right)
    return [string]::Equals((Get-NormalizedPath $Left), (Get-NormalizedPath $Right), $script:PathComparison)
}

# ---------------------------------------------------------------------------
# git 查询
# ---------------------------------------------------------------------------
function Get-Worktrees {
    $lines = @(& git -C $repoRoot worktree list --porcelain 2>$null)
    if ($LASTEXITCODE -ne 0) {
        Fail 'git worktree list 失败'
    }

    $result = New-Object System.Collections.ArrayList
    $entry = $null
    foreach ($line in $lines) {
        if ($line.StartsWith('worktree ')) {
            if ($null -ne $entry) { [void]$result.Add($entry) }
            $entry = [pscustomobject]@{
                Path   = $line.Substring('worktree '.Length)
                Head   = ''
                Branch = ''
            }
        } elseif ($null -ne $entry -and $line.StartsWith('HEAD ')) {
            $entry.Head = $line.Substring('HEAD '.Length)
        } elseif ($null -ne $entry -and $line.StartsWith('branch ')) {
            $entry.Branch = $line.Substring('branch '.Length) -replace '^refs/heads/', ''
        }
    }
    if ($null -ne $entry) { [void]$result.Add($entry) }
    return $result.ToArray()
}

function Test-BranchExists {
    param([string]$Name)
    & git -C $mainRoot rev-parse --verify --quiet "refs/heads/$Name" > $null 2>&1
    return ($LASTEXITCODE -eq 0)
}

# ---------------------------------------------------------------------------
# CMakeCache 读取（只取需要的键）
# ---------------------------------------------------------------------------
function Get-CacheValue {
    param([string]$CacheFile, [string]$Key)
    $pattern = '^' + [System.Text.RegularExpressions.Regex]::Escape($Key) + ':[^=]*=(.*)$'
    foreach ($line in (Get-Content -LiteralPath $CacheFile -Encoding UTF8)) {
        if ($line -match $pattern) {
            return $Matches[1].Trim()
        }
    }
    return ''
}

function Find-CacheTemplate {
    param([string]$Explicit)
    if (-not [string]::IsNullOrWhiteSpace($Explicit)) {
        $dir = if ([System.IO.Path]::IsPathRooted($Explicit)) { $Explicit } else { Join-Path $mainRoot $Explicit }
        $cache = Join-Path $dir 'CMakeCache.txt'
        if (-not (Test-Path -LiteralPath $cache)) {
            Fail "找不到配置模板: $cache（-FromBase 需要指向一个已配置的构建目录）"
        }
        return [pscustomobject]@{
            Dir   = (Get-NormalizedPath $dir)
            Name  = (Split-Path -Leaf $dir)
            Cache = $cache
            Tests = $false
        }
    }

    $candidates = @(Get-ChildItem -LiteralPath $mainRoot -Directory -Filter 'build*' -ErrorAction SilentlyContinue |
        Where-Object { Test-Path -LiteralPath (Join-Path $_.FullName 'CMakeCache.txt') })
    if ($candidates.Count -eq 0) { return $null }

    $infos = @()
    foreach ($candidate in $candidates) {
        $cache = Join-Path $candidate.FullName 'CMakeCache.txt'
        $infos += [pscustomobject]@{
            Dir   = (Get-NormalizedPath $candidate.FullName)
            Name  = $candidate.Name
            Cache = $cache
            Tests = ((Get-CacheValue $cache 'DAWIDGETS_BUILD_TESTS') -eq 'ON')
        }
    }

    # 测试是每个任务的交付门槛，默认优先挑开启测试的缓存
    $testsOn = @($infos | Where-Object { $_.Tests })
    $pool = if ($testsOn.Count -gt 0) { $testsOn } else { $infos }

    $pick = @($pool | Where-Object { $_.Name -eq 'build-tests' } | Select-Object -First 1)
    if ($pick.Count -eq 0) {
        $pick = @($pool | Where-Object { $_.Name -eq 'build' } | Select-Object -First 1)
    }
    if ($pick.Count -eq 0) {
        $pick = @($pool | Select-Object -First 1)
    }
    return $pick[0]
}

# ---------------------------------------------------------------------------
# 预配置 build/：沿用主工作树模板的生成器 / Qt 路径 / 选项
# ---------------------------------------------------------------------------
function Configure-Build {
    param([string]$WorktreePath, $Template)

    $template = $Template
    if ($null -eq $template) {
        Write-Host ''
        Write-Warning '主工作树里没有可用的 build*/CMakeCache.txt，跳过预配置。'
        Write-Note '请按 AGENTS.md 手动配置（必须给 -DCMAKE_PREFIX_PATH=<Qt 路径>；跑测试需 -DDAWIDGETS_BUILD_TESTS=ON）'
        return
    }

    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if ($null -eq $cmake) {
        Write-Host ''
        Write-Warning "PATH 里找不到 cmake，跳过预配置（模板: $($template.Name)）"
        return
    }

    $generator = Get-CacheValue $template.Cache 'CMAKE_GENERATOR'
    $platform = Get-CacheValue $template.Cache 'CMAKE_GENERATOR_PLATFORM'
    $instance = Get-CacheValue $template.Cache 'CMAKE_GENERATOR_INSTANCE'
    $prefix = Get-CacheValue $template.Cache 'CMAKE_PREFIX_PATH'
    $buildType = Get-CacheValue $template.Cache 'CMAKE_BUILD_TYPE'
    $shared = Get-CacheValue $template.Cache 'DAWIDGETS_BUILD_SHARED'
    $tests = Get-CacheValue $template.Cache 'DAWIDGETS_BUILD_TESTS'
    $demo = Get-CacheValue $template.Cache 'DAWIDGETS_BUILD_DEMO'

    $buildDir = Join-Path $WorktreePath 'build'
    $cmakeArgs = @('-S', $WorktreePath, '-B', $buildDir)
    if (-not [string]::IsNullOrWhiteSpace($generator)) { $cmakeArgs += @('-G', $generator) }
    if (-not [string]::IsNullOrWhiteSpace($platform)) { $cmakeArgs += @('-A', $platform) }
    if (-not [string]::IsNullOrWhiteSpace($instance)) { $cmakeArgs += "-DCMAKE_GENERATOR_INSTANCE=$instance" }
    if (-not [string]::IsNullOrWhiteSpace($prefix)) { $cmakeArgs += "-DCMAKE_PREFIX_PATH=$prefix" }
    if (-not [string]::IsNullOrWhiteSpace($buildType)) { $cmakeArgs += "-DCMAKE_BUILD_TYPE=$buildType" }
    if (-not [string]::IsNullOrWhiteSpace($shared)) { $cmakeArgs += "-DDAWIDGETS_BUILD_SHARED=$shared" }
    if (-not [string]::IsNullOrWhiteSpace($tests)) { $cmakeArgs += "-DDAWIDGETS_BUILD_TESTS=$tests" }
    if (-not [string]::IsNullOrWhiteSpace($demo)) { $cmakeArgs += "-DDAWIDGETS_BUILD_DEMO=$demo" }

    Write-Step "预配置构建目录（模板: $($template.Name)）"
    Write-Note "生成器: $generator $platform"
    if (-not [string]::IsNullOrWhiteSpace($prefix)) { Write-Note "Qt:     $prefix" }
    if ($tests -eq 'ON') {
        Write-Note '测试:   已开启（DAWIDGETS_BUILD_TESTS=ON）'
    } else {
        Write-Note '测试:   模板未开启；需要测试请改用 -FromBase <开启测试的构建目录>'
    }
    $printedArgs = @($cmakeArgs | ForEach-Object { if ($_ -match '\s') { '"' + $_ + '"' } else { $_ } })
    Write-Note "命令:   cmake $($printedArgs -join ' ')"

    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Host ''
        Write-Warning '预配置失败（工作树本身可用），可按上面的命令手动重试'
        return
    }
    Write-Note "预配置完成: $buildDir"
}

# ---------------------------------------------------------------------------
# 创建
# ---------------------------------------------------------------------------
function New-Worktree {
    param([string]$Name, [string]$BaseBranch)

    $null = & git -C $repoRoot check-ref-format --branch $Name 2>$null
    if ($LASTEXITCODE -ne 0) {
        Fail "非法分支名: $Name（见 git check-ref-format）"
    }

    $existing = @($worktrees | Where-Object { $_.Branch -eq $Name })
    if ($existing.Count -gt 0) {
        Fail "分支 $Name 已在工作树中检出: $($existing[0].Path)。请在该工作树里继续，或换一个分支名"
    }

    $dirName = $Name -replace '[/\\]', '-'
    $wtPath = Join-Path $wtRoot $dirName
    if (Test-Path -LiteralPath $wtPath) {
        Fail "工作树目录已存在: $wtPath（删除用: -Remove $dirName）"
    }
    if ($wtPath.Length -gt 180) {
        Write-Warning "工作树路径较长（$($wtPath.Length) 字符），Windows 下构建产物可能触碰 260 字符上限"
    }

    New-Item -ItemType Directory -Force -Path $wtRoot | Out-Null

    # 先解析配置模板：-FromBase 写错时在建工作树之前就失败，不留下一个空工作树
    $template = $null
    if (-not $NoConfigure) {
        $template = Find-CacheTemplate $FromBase
    }

    Write-Step '创建工作树'
    $branchExisted = Test-BranchExists $Name
    if ($branchExisted) {
        Write-Note "分支 $Name 已存在，直接检出"
        & git -C $mainRoot worktree add $wtPath $Name
    } else {
        if (-not (Test-BranchExists $BaseBranch)) {
            Fail "基线分支不存在: $BaseBranch"
        }
        Write-Note "从 $BaseBranch 切出新分支 $Name"
        & git -C $mainRoot worktree add -b $Name $wtPath $BaseBranch
    }
    if ($LASTEXITCODE -ne 0) {
        Fail "创建工作树失败: $wtPath"
    }

    if (-not $NoConfigure) {
        Configure-Build -WorktreePath $wtPath -Template $template
    }

    $origin = if ($branchExisted) { '已存在的分支' } else { "基线 $BaseBranch" }
    Write-Step '完成'
    Write-Note "工作树: $wtPath"
    Write-Note "分支:   $Name（$origin）"
    if ($NoConfigure) {
        Write-Note '构建:   未预配置 build/，请按 AGENTS.md 自行配置后再构建'
    } else {
        Write-Note "构建:   cmake --build `"$(Join-Path $wtPath 'build')`" --config Release --parallel"
        Write-Note "测试:   ctest --test-dir `"$(Join-Path $wtPath 'build')`" -C Release --output-on-failure"
        Write-Note "示例:   $(Join-Path $wtPath 'build\examples\all_widgets_demo\Release\dawidgets_all_widgets_demo.exe')（Windows + VS 生成器）"
    }
    Write-Note "合并:   在 master 工作树 git merge $Name，然后 -Remove $dirName"
    Write-Host ''
    Write-Note '提醒: 1) 工作树里不要执行 cmake --install（安装前缀不属于本工作树），需要安装请回主工作树'
    Write-Note '      2) 不要手工 rm -rf 工作树目录，用 -Remove（Windows 上常被占用，脚本会重试并给出残留说明）'
}

# ---------------------------------------------------------------------------
# 删除
# ---------------------------------------------------------------------------
function Remove-Worktree {
    param([string]$Name)

    if ([string]::IsNullOrWhiteSpace($Name)) {
        Fail '缺少工作树目录名（用法: -Remove <目录名>）'
    }

    $target = if ([System.IO.Path]::IsPathRooted($Name)) {
        $Name
    } elseif ($Name -match '[\\/]') {
        [System.IO.Path]::GetFullPath($Name)
    } else {
        Join-Path $wtRoot $Name
    }
    $target = Get-NormalizedPath $target

    if (Test-SamePath $target $mainRoot) {
        Fail "拒绝删除主工作树: $target"
    }
    if (-not (Test-Path -LiteralPath $target)) {
        Fail "工作树目录不存在: $target"
    }

    $current = @(Get-Worktrees)
    $entry = @($current | Where-Object { Test-SamePath $_.Path $target })
    if ($entry.Count -eq 0) {
        # 上次删除时目录被占用（Windows 常见）：git 已摘除注册、只剩空壳目录。
        # 只清理空目录，避免误删普通目录。
        if (@(Get-ChildItem -LiteralPath $target -Force -ErrorAction SilentlyContinue).Count -eq 0) {
            Remove-Item -LiteralPath $target -Recurse -Force
            Write-Note "已清理上次遗留的空壳目录: $target"
            return
        }
        Fail "$target 不是已注册的工作树（只删除已注册的工作树，避免误删普通目录）"
    }

    Write-Step '删除工作树'
    & git -C $mainRoot worktree remove --force $target
    if ($LASTEXITCODE -ne 0) {
        $still = @(Get-Worktrees | Where-Object { Test-SamePath $_.Path $target })
        if ($still.Count -gt 0) {
            Fail "工作树仍处于注册状态，未能删除。请关闭占用该目录的进程（编辑器/终端/MSBuild）后重试"
        }
        for ($attempt = 1; $attempt -le 3; $attempt++) {
            Remove-Item -LiteralPath $target -Recurse -Force -ErrorAction SilentlyContinue
            if (-not (Test-Path -LiteralPath $target)) { break }
            Start-Sleep -Seconds 1
        }
        if (Test-Path -LiteralPath $target) {
            Write-Warning "git 侧已注销，但目录仍被占用未能删净: $target"
            Write-Note '退出占用该目录的进程后手工删除即可，不影响其它工作树与分支'
        } else {
            Write-Note 'git worktree remove 未删净，已清理残留目录'
        }
    }

    & git -C $mainRoot worktree prune
    Write-Note "已删除: $target"
    Write-Note '分支保留；合并到 master 后可用 git branch -d <分支> 删除'
}

function Show-Worktrees {
    Write-Host '工作树列表（* = 主工作树）:'
    $width = 0
    foreach ($wt in $worktrees) {
        if ($wt.Path.Length -gt $width) { $width = $wt.Path.Length }
    }
    foreach ($wt in $worktrees) {
        $mark = if (Test-SamePath $wt.Path $mainRoot) { '*' } else { ' ' }
        $branch = if ([string]::IsNullOrWhiteSpace($wt.Branch)) { '(detached)' } else { $wt.Branch }
        Write-Host ("  {0} {1}  {2}" -f $mark, $wt.Path.PadRight($width), $branch)
    }
    Write-Host ''
    Write-Host "工作树目录: $wtRoot"
}

# ---------------------------------------------------------------------------
# 入口
# ---------------------------------------------------------------------------
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$candidateRoot = [System.IO.Path]::GetFullPath((Join-Path $scriptDir '..'))

$top = & git -C $candidateRoot rev-parse --show-toplevel 2>$null
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($top)) {
    Fail "当前目录不在 git 仓库中: $candidateRoot"
}
$repoRoot = Get-NormalizedPath $top

$superProject = & git -C $repoRoot rev-parse --show-superproject-working-tree 2>$null
if ($LASTEXITCODE -eq 0 -and -not [string]::IsNullOrWhiteSpace($superProject)) {
    Fail @"
检测到当前目录是子模块检出（父仓库: $superProject）。
DAWidgets 作为 data-workbench 的子模块时，请在 data-workbench 主工作树里修改并 cmake --install；
DAWidgets 自身的工作树请在独立克隆（仓库根不是子模块的检出）里创建。
"@
}

$worktrees = @(Get-Worktrees)
if ($worktrees.Count -eq 0) {
    Fail '读取工作树列表失败'
}
$mainRoot = Get-NormalizedPath $worktrees[0].Path
$wtRoot = Join-Path $mainRoot '.worktree'

if ($Help -or ([string]::IsNullOrWhiteSpace($Branch) -and -not $List -and -not $Remove)) {
    Show-Usage
    exit 0
}
if ($List) {
    Show-Worktrees
    exit 0
}
if ($Remove) {
    Remove-Worktree $Branch
    exit 0
}

New-Worktree -Name $Branch.Trim() -BaseBranch $Base
