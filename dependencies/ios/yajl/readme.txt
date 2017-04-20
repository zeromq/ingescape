What to to to the source code to achieve compilation :
- extract .h and .c files from the yawl git repo
- rename the api folder to yajl
- compile yajl in its original repo using cmake standard procedure and copy yajl_version.h inside our yajl subfolder containing all the headers
- add the new files (if any) in the Xcode project
- configure the header search path in the Xcode project to include the yajl subfolder
- adjust the the includes in the header files to switch from "api/xxx.h" to <yajl/xxx.h>
