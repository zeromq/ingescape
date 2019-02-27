Due to the specificities of IOS and the requirements for cross compilation, we gathered all the dependencies inside a dedicated folder.

The ZMQ/Zyre suite is compile using cross compilation scripts doing all the job from the git checkout to the assembling of include files and static libraries. The ingescape iOS Xcode project expects all the corresponding files to be present but there is no need to push them into git.
To compile the zyre suite for iOS, just run ./crosscompile_zyre_suite.sh