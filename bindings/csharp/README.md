# Ingescape C#

## Installing
An installer of the wrapper exists in this repository to install the library in your system with the last version of ingescape: "TODO"


## Building
- Open the Visual Studio solution to build the C# library : .\IngescapeCSharpWrapper\IngescapeCSharpWrapper.sln
- Build
- You will find the library in the following folder: .\IngescapeCSharpWrapper\IngescapeCSharp\bin\(Debug ou Release)\(x64 ou x86)\netstandard2.0\IngescapeCSharp.dll


## Testing
The 'Tester' project is a C# version of igstester.c to test the C# library. Open the Test Explorer tab in Visual Studio to execute the test.
By default, this test project reference the IngescapeCSharp project and create a copy of the dll into the output folder. The IngescapeCSharp.dll only works with ingescape.dll and its dependencies in the same directory but we cannot reference the ingescape.dll in a Visual Studio project.
For that reason, we implemented a pre-build event to copy ingescape.dll and its dependencies, generated from the respective projects in the solution, into the output directory.

