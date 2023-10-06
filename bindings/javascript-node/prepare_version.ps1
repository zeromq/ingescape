
$IHM_VERSION_MAJOR = Select-String "#define INGESCAPE_VERSION_MAJOR" ..\..\include\ingescape.h | ForEach-Object { ([string]$_).Split(" ")[2] }
$IHM_VERSION_MINOR = Select-String "#define INGESCAPE_VERSION_MINOR" ..\..\include\ingescape.h | ForEach-Object { ([string]$_).Split(" ")[2] }
$IHM_VERSION_PATCH = Select-String "#define INGESCAPE_VERSION_PATCH" ..\..\include\ingescape.h | ForEach-Object { ([string]$_).Split(" ")[2] }
$NEW_CIRCLE_VER = "$IHM_VERSION_MAJOR.$IHM_VERSION_MINOR.$IHM_VERSION_PATCH"

(Get-Content ./package.json) -Replace '"version": ".*"', "`"version`": `"$NEW_CIRCLE_VER`"" | Set-Content ./package.json
