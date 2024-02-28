
$IGS_MAJOR = Select-String "#define INGESCAPE_VERSION_MAJOR" ..\..\include\ingescape.h | ForEach-Object { ([string]$_).Split(" ")[2] }
$IGS_MINOR = Select-String "#define INGESCAPE_VERSION_MINOR" ..\..\include\ingescape.h | ForEach-Object { ([string]$_).Split(" ")[2] }
$IGS_PATCH = Select-String "#define INGESCAPE_VERSION_PATCH" ..\..\include\ingescape.h | ForEach-Object { ([string]$_).Split(" ")[2] }
$IGS_VERSION = "$IGS_MAJOR.$IGS_MINOR.$IGS_PATCH"

echo "This script uses Microsoft Visual Studio 2019"
echo "Some changes are needed to use a different version"

if ( -not (Test-Path -LiteralPath '..\..\build\ReleaseX64' -PathType Container) ) {
    echo "ingescape library must be built in <git_root>/build/ReleaseX64 before compiling the python binding"
    exit 1
}

echo "Prepare pyproject.toml from pyproject.toml.in"
(Get-Content ./pyproject.toml.in) -Replace '@IGS_VERSION@', "`"$IGS_VERSION`"" | Set-Content ./pyproject.toml
echo "Prepare setup.py from setup.py.in"
(Get-Content ./setup.py.in) -Replace '@IGS_VERSION@', "`"$IGS_VERSION`"" | Set-Content ./setup.py

echo "Copy library files"
mkdir -f ./dependencies/windows
mkdir -f ./dependencies/windows/x64
Copy-Item -Path ..\..\build\ReleaseX64\Release\libingescape.lib -Destination ./dependencies/windows/x64 -Force
Copy-Item -Path ..\..\build\ReleaseX64\dependencies\czmq\Release\libczmq.lib -Destination ./dependencies/windows/x64 -Force
Copy-Item -Path ..\..\build\ReleaseX64\dependencies\libzmq\lib\Release\libzmq-v142-mt-s-4_3_6.lib -Destination ./dependencies/windows/x64/libzmq.lib -Force
Copy-Item -Path ..\..\build\ReleaseX64\dependencies\zyre\Release\libzyre.lib -Destination ./dependencies/windows/x64 -Force
copy-Item -Path ..\..\build\ReleaseX64\dependencies\sodium\Release\libsodium.lib -Destination ./dependencies/windows/x64 -Force
copy-Item -Path ../../include/* -Destination ./dependencies/include -Recurse -Force
copy-Item -Path ../../dependencies/czmq/include/* -Destination ./dependencies/include -Recurse -Force
copy-Item -Path ../../dependencies/libzmq/include/* -Destination ./dependencies/include -Recurse -Force
copy-Item -Path ../../dependencies/sodium/src/libsodium/include/* -Destination ./dependencies/include -Recurse -Force
copy-Item -Path ../../dependencies/zyre/include/* -Destination ./dependencies/include -Recurse -Force
pushd 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build'
cmd /c "vcvarsall.bat x64 & set" |
foreach {
  if ($_ -match "=") {
    $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
  }
}
popd
