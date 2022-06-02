# Ingescape C#

## Installing
Ingescape C# comes with the main Ingescape library and is available on Windows only.


## Building

### Visual Studio
Simply use the Visual Studio solution in .\IngescapeCSharpWrapper\IngescapeCSharpWrapper.sln

### Cmake

Cmake supports multiple versions of Visual Studio. One shall be specified in the cmake command.

Compilation is possible for _Win32_ or _x64_. Change the '-A' option below according to your needs.
Build type may be _Debug_ or _Release_.

	cmake -S . -B .\build -G"Visual Studio 16 2019" -Ax64 -DCMAKE_BUILD_TYPE=Release -DWITH_TEST=OFF
	Powershell.exe -executionpolicy remotesigned bindings/csharp/builds/visual-studio/cmake/FixCmakeSolution.ps1 -builddirectory ./build/bindings/csharp -projectName IngescapeCSharp        
    cmake --build .\build --target IngescapeCSharp --config Release

## Testing
The test.cs file is a C# transcription of igstester.c for the C# binding. 

Open the Test Explorer tab in Visual Studio to execute the tests.

Tests can be built using CMAKE.

	cmake -S . -B.\build -G"Visual Studio 16 2019" -Ax64 -DCMAKE_BUILD_TYPE=Release
	Powershell.exe -executionpolicy remotesigned bindings/csharp/builds/visual-studio/cmake/FixCmakeSolution.ps1 -builddirectory ./build/bindings/csharp -projectName IngescapeCSharp
	Powershell.exe -executionpolicy remotesigned bindings/csharp/builds/visual-studio/cmake/FixCmakeSolution.ps1 -builddirectory ./build/bindings/csharp/test -projectName igstester
	cmake --build .\build --target igstester --config Release