+++
title = "Your first ingeScape agent"
date = "2018-04-23T11:36:41+02:00"
tags = ["agents"]
categories = ["learn"]
banner = "img/banners/your_first_agent.png"
genre = "article"
+++

This article gives you an introduction to the ingeScape library, in order to create a first software agent to be integrated in any ingeScape environment.

Creating your own agents is the key step to building the services that will make your future systems based on ingeScape.

### Forewords

Our main goal when building the ingeScape API was to make it extremely simple for most usual situations, and very progressive when fine-tuning and advanced features become necessary.

Below is a first example producing a fully functional agent, with a very simple behavior. But before jumping into the code, we need to explain the core concepts of an agent: its *name*, its *inputs*, its *outputs* and its *parameters*.

The name of the agent is how it is addressed by other agents in your system and seen in the ingeScape editor. We advise it to be unique in a given system, even ingeScape supports multiple agents with the same name, as it may be useful and relevant in some contexts.


### Inputs, Outputs and Parameters : the IOPs
Inputs, Outputs and Parameters, often abbreviated as ***IOPs*** by the ingeScape community, are all managed in the same way. They all carry a *name*, a *type* and a *value*. Giving a value when creating an IOP is optional.

Here are the supported IOP types:

- ***Impulsion*** : the simplest of all, inspired from the electronic world,
- ***Boolean***, ***Integer*** and ***Double*** : the numeric types,
- ***String*** : the string of characters,
- ***Data*** : raw binary information agnostically handled by ingeScape.

Even though they are manipulated in the same way, the IOPs do not share the same semantics:

- ***Inputs*** enable the agents to receive information. When written, they shall trigger your agent's internal behavior and optionally write to outputs.
- ***Outputs*** enable agents to distribute information (i.e. output values) to other agents.
- ***Parameters*** enable users of your agent to change its internal behavior. Exposed parameters are optional and shall be chosen and handled carefully as they are your agent's configuration settings.

The whole set of IOPs for an agent is called the agent's ***definition***. Definitions can be created programmatically but also by writing JSON files or even by using the editor. Definitions can be enriched with a name (that can be different from the agent name), a version and a description. We'll come back to this in further articles.

Agents are connected to one another by ***mapping*** outputs from agents to inputs of other agents. The set of ***mappings*** in a system composes the actual system communication architecture. We'll come back to this in further articles. The concept of *mapping* is not mandatory to understand this first example.


### The code
Here we are, this is all you need to know to design your first agent. The example below is developed in C. The code is filled with comments enabling to understand each function and each code block. Just after the code example are several resources to help you compile it on different operating systems and developing environments.

We recommend that you immediately copy and paste this code in your favorite C code editor.

{{< highlight c "linenos=table,linenostart=1" >}}
#include <stdio.h>
#include <ingescape/ingescape.h> //this is the only include required for ingeScape

/*
 This is a callback function that runs everytime the
 input "input1" is written. The callback is linked to
 the input by the use of igs_observeInput below.
 The behavior of this agent is very simple:
 - reading the changed input
 - printing the new value
 - multiplying the value by 2 and writing the result to the output
 */
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData){
    /*
     This callback is fed with all the information necessary:
     - the IOP type : input, output or parameter
     - the IOP name
     - the IOP value type (impulsion, boolean, etc.)
     - a pointer to the value
     - the actual memory size of the value
     - optional data passed from the call to observeInput
     */
    int r = igs_readInputAsInt(name);
    printf("%s changed to %d\n", name, r);
    igs_writeOutputAsInt("output1", 2*r);
}

/*
 This is the main function. It initializes the agent
 and makes a call to getchar so that the agent does
 not terminate right away. Agent termination is achieved
 by entering any character on the keyboard and pressing enter.
 */
int main(int argc, const char * argv[]) {
    /*
     Use ingeScape function to find compatible network devices
     on your computer.
     You need to use one of the returned results in the
     igs_startWithDevice call below for your agent to start
     on the network.
     */
    char **devices = NULL;
    int nb = 0;
    igs_getNetdevicesList(&devices, &nb);
    for (int i = 0; i < nb; i++){
        printf("device: %s\n", devices[i]);
    }
    igs_freeNetdevicesList(devices, nb);

    //set the agent name
    if (argc == 2){
        //either by using the argument on the command line
        igs_setAgentName(argv[1]);
    }else{
        //or by giving a default name if no argument was passed
        igs_setAgentName("simpleDemoAgent");
    }

    //Create an input and an ouput: both are of integer type
    //with default value set to zero.
    igs_createInput("input1", IGS_INTEGER_T, 0, 0);
    igs_createOutput("output1", IGS_INTEGER_T, 0, 0);

    //Set the definition information (optional)
    igs_setDefinitionName("myDef");
    igs_setDefinitionVersion("1.0");
    igs_setDefinitionDescription("Definition for our first agent.");

    //Attach the callback to our agent's input
    //NB: callbacks can be attached to any IOP
    igs_observeInput("input1", myIOPCallback, NULL);

    //Actually and finally start the agent
    //First argument is the network device name to use
    //Second argument is the network port used by all the agents of your system
    igs_startWithDevice("en0", 5670);

    getchar();

    //Stop the agent properly before terminating the program
    igs_stop();

    return 0;
}
{{< / highlight >}}

### Compile on Linux
As a prerequisite, you need to have the ingeScape library and its dependencies installed on your computer. We suppose the ingeScape headers are installed in */usr/local/include/* and the libraries in */usr/local/lib/*, which is pretty standard on most Linux environments.

When ready, copy the code in a *main.c* file and type the following commands in a terminal where your *main.c* file is located:

{{< highlight shell "linenos=inline" >}}
gcc -W -Wall -g -I/usr/local/include/ -std=gnu99 -o main.o -c main.c
gcc -o myFirstAgent main.o -L/usr/local/lib -lingescape
{{< / highlight >}}

You can also use an example [Qt project](/code/firstAgent_Qt.zip) embedding ingeScape in a simple graphical Qt application with an empty UI.

### Compile on macOS (three methods)
#### GCC/Clang
The first compilation method is the same as the one for Linux, exactly with the same assumptions for headers and libraries location.

*NB: on macOS gcc is an alias for clang. Both can be used indifferently.*

#### Xcode
Another method is the use of Apple Xcode with a dedicated project. To save your time, such a project, including the code, is available [here](/code/firstAgent.zip).

#### Qt
You can also use an example [Qt project](/code/firstAgent_Qt.zip) embedding ingeScape in a simple graphical Qt application with an empty UI.

In some cases, if you are using Homebrew to install third-party libraries, there might be an incompatibility between the JPEG, TIFF, etc. libraries embedded in CoreImage and the ones installed in */usr/local/lib* by Homebrew. This has nothing to do with ingeScape but might block the execution of the example.

The following error message is then displayed at runtime:
{{< highlight shell "linenos=inline" >}}
dyld: Symbol not found: __cg_jpeg_resync_to_restart
  Referenced from: /System/Library/Frameworks/ImageIO.framework/Versions/A/ImageIO
  Expected in: /usr/local/lib/libJPEG.dylib
 in /System/Library/Frameworks/ImageIO.framework/Versions/A/ImageIO
{{< / highlight >}}

If this happens, you need to edit the Run properties for your Qt project by editing the DYLD_LIBRARY_PATH environment variable and adding */System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/* as the first path to be checked, generally followed by your local project path and */usr/local/lib/*.

### Compile on Microsoft Windows
The easiest way to compile and run your first agent on Microsoft Windows is to use one of the following projects:

- [Qt project](/code/firstAgent_Qt.zip)
- [Visual Studio 2015 project](/code/firstAgent_VS.zip)

For both projects, the ingeScape library and its dependencies have to be deployed next to the project file (.pro or .sln).
