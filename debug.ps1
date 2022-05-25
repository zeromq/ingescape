 cmake -S . -B build\DebugX64 -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Debug -DWITH_DEPS=ON -DCI_PIPELINE_ID="$env:CI_PIPELINE_ID" -DWITH_CSHARP_WRAPPER=ON
 Powershell.exe -executionpolicy remotesigned bindings/csharp/cmake/FixCmakeSolution.ps1 -buildFolder build/DebugX64/bindings/csharp
 Powershell.exe -executionpolicy remotesigned bindings/csharp/cmake/FixCmakeSolution.ps1 -buildFolder build/DebugX64/bindings/csharp/test -projectName igstester
 cmake --build build\DebugX64 --target ALL_BUILD --config Debug