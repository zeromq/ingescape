#To build

- Open the vs solution to build the c# library : .\IngescapeCSharpWrapper\IngescapeCSharpWrapper.sln
- Check and modify the path of the library : C:\\ingescape\\libs\\debug\\ingescape.dll
- Build
- You will find the 3 files composing the c# library : .\IngescapeCSharpWrapper\IngescapeCSharp\bin\Debug\netstandard2.0
	-IngescapeCSharp.deps.json
	- IngescapeCSharp.dll
	- IngescapeCSharp.pdb
- You will find the sample into the sample project hiearchy 	.\IngescapeCSharpWrapper\CSharpSampleAgent\bin\Debug or Release depend of the build type
- If you want to make your own project from the sample be carefull of the reference of the c# dll library 'ingescape'

