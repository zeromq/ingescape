$Generator = "Visual Studio 15 2017 Win64"

cmake -S ..\.. -B dependencies -G $Generator -DDEPS_ONLY=ON

echo $PSScriptRoot\dependencies\ingescape.sln

Remove-Item -path $PSScriptRoot\dependencies\ingescape.sln 
