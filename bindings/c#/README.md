#To build

- Open the vs solution to build the c# library : .\IngescapeCSharpWrapper\IngescapeCSharpWrapper.sln
- Check and modify the path of the library : C:\\ingescape\\libs\\debug\\ingescape.dll
- Build
- You will find the 3 files composing the c# library : .\IngescapeCSharpWrapper\IngescapeCSharp\bin\Debug\netstandard2.0
- IngescapeCSharp.deps.json
	- IngescapeCSharp.dll
	- IngescapeCSharp.pdb

# To test it
- Open the sample vs solution to build a c# sample agent : .\sample\CSharpSampleAgent\CSharpSampleAgent.sln
- Check and modify the path of the reference to the Librairie IngescapeCSharp 
- Build & run it