# Clone if the repository does not exist. Update it if it exists.
# Update process stashes any changes before updating (--rebase)
# then pops the stash back into the repository
Function CloneOrUpdate {
    Param ($LocalPath, $CloneUrl)
    if (test-path "$LocalPath") {
        Write-Host "$LocalPath already exists. Repository will be updated."
        Set-Location -Path "$LocalPath"
        git stash
        git pull --rebase
        git stash pop
    } else {
        Write-Host "$LocalPath does not exist. Cloning from Gitlab."
        Write-Host "You may be asked for credentials to access the repository."
        if ($LocalPath -Match "libsodium" ) {
            # Clone 'stable' branch for the libsodium repository
            git clone --branch stable $CloneUrl $LocalPath
        } else {
            git clone $CloneUrl $LocalPath
        }
    }
}

$RunPath = (Get-Item -Path ".\").FullName
$ScriptDir = Split-Path -parent $PSCommandPath

CloneOrUpdate "$ScriptDir\..\..\..\libsodium" ssh://git@gitlab.ingescape.com:22222/third-party/libsodium.git
CloneOrUpdate "$ScriptDir\..\..\..\libzmq" ssh://git@gitlab.ingescape.com:22222/third-party/libzmq
CloneOrUpdate "$ScriptDir\..\..\..\czmq" ssh://git@gitlab.ingescape.com:22222/third-party/czmq.git
CloneOrUpdate "$ScriptDir\..\..\..\zyre" ssh://git@gitlab.ingescape.com:22222/third-party/zyre.git
CloneOrUpdate "$ScriptDir\..\..\..\ingescape" ssh://git@gitlab.ingescape.com:22222/I2/ingescape.git

Write-Host ""
Write-Host "Debug env is up to date."
Write-Host "You'll find the full ingescape solution right next to this script."
Write-Host "Happy debugging!"

# Back to square one
Set-Location -Path $RunPath