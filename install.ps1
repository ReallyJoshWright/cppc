$installDir = Join-Path $HOME ".config\.cppc"

Write-Host "Creating install directory: $installDir"
try {
    New-Item -ItemType Directory -Path $installDir -Force | Out-Null
    Write-Host "Install directory created/ensured"
} catch {
    Write-Error "Error creating install directory: $($_.Exception.Message)"
    exit 1
}

Write-Host "Adding cppc.exe binary and builder.h to $installDir"
try {
    Copy-Item -Path "cppc.exe" -Destination $installDir -Force -ErrorAction Stop
    Copy-Item -Path "builder.h" -Destination $installDir -Force -ErrorAction Stop
} catch {
    Write-Error "Error copying files: $($_.Exception.Message)"
    exit 1
}

$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
$pathComponents = $currentPath.Split(';', [System.StringSplitOptions]::RemoveEmptyEntries)
$resolvedInstallDir = (Get-Item -Path $installDir).FullName

if (-not ($pathComponents -contains $resolvedInstallDir)) {
    $newPath = "$currentPath;$resolvedInstallDir"

    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    Write-Host "Added '$resolvedInstallDir' to the User PATH environment variable."
    Write-Host "You may need to restart applications (including your terminal/PowerShell window) for the new PATH to take effect."
} else {
    Write-Host "'$resolvedInstallDir' already exists in the User PATH environment variable."
}

Write-Host "Installation Complete!"

# Maybe run this
# Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
