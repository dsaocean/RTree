<#
.SYNOPSIS
Packs the RTree header-only NuGet package.

.DESCRIPTION
RTree is header-only: the package is the header plus a props file that puts it
on the include path. This script exists because the package was previously
assembled by hand, which is not repeatable and leaves no record of what went
into a given version.

.PARAMETER Version
The package version. Must not reuse a version already present on any feed -
check D:\Nuget\DSA-Stage and the DSA-PreRelease share before picking one.

.PARAMETER PushSource
Feed to push to. Omit to pack without pushing.

.EXAMPLE
.\Build.ps1 -Version 2022.3.5
.\Build.ps1 -Version 2022.3.5 -PushSource DSA-Stage
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Version,
    [string]$PushSource
)

$ErrorActionPreference = 'Stop'
$here = $PSScriptRoot
$out  = Join-Path $here 'bin'

New-Item -ItemType Directory -Force -Path $out | Out-Null

nuget pack (Join-Path $here 'RTree.nuspec') -Version $Version -OutputDirectory $out -NoDefaultExcludes
if ($LASTEXITCODE -ne 0) { throw "nuget pack failed" }

$pkg = Join-Path $out "RTree.$Version.nupkg"
if (-not (Test-Path $pkg)) { throw "expected package not produced: $pkg" }
Write-Host "Packed $pkg"

if ($PushSource)
{
    nuget push $pkg -Source $PushSource
    if ($LASTEXITCODE -ne 0) { throw "nuget push to $PushSource failed" }
    Write-Host "Pushed to $PushSource"
}
