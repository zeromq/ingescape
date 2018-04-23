---
title: "Your first ingeScape agent"
date: 2018-04-23T11:36:41+02:00
draft: true
---

Our main goal when building the ingeScape API was to make it extremely simple for most common situations and very progressive when fine tuning and advanced features become necessary.

Below is a first example producing a fully functional agent, with a very simple behavior. But before jumping into the code, we need to explain the core concepts of an agent: its *name*, its *inputs*, its *outputs* and its *parameters*.

**image here**

The name of the agent is how it is addressed by other agents in your system and seen in the ingeScape editor. We advise it to be unique in a given system even ingeScape supports multiple agents with the same name, as it is sometimes useful and relevant.


### **I**nputs, **O**utputs and **P**arameters : the IOPs


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

The whole set of IOPs for an agent is called the agent's ***definition***. Definitions can be created programmatically but also but writing JSON files ou even by using the editor. Definitions can be enriched with a name (that can be different from the agent name), a version and a description. We'll come back to this in further articles.

Agents are connected to one another by ***mapping*** outputs from agents to inputs of other agents. The set of ***mappings*** in a system composes the actual system communication architecture. We'll come back to this in further articles. The concept of *mapping* is not mandatory to understand this first example.


### Jumping into the code
Here we are, this is all you need to know to design your first agent. The example below is developed in C. The code is filled with comments enabling to understand each function and each code block. Just after the code example are several resources to help you compile it on different operating systems dans developing environments.

We recommend that you immediately copy and paste the code below in your favorite editor.

{{< highlight c "linenos=table,linenostart=1" >}}
#include <stdio.h>
#include <mastic/mastic.h> //this is the only include required for ingeScape

/*
 This is a callback function that runs everytime the
 input "input1" is written. The callback is linked to
 the input by the use of mtic_observeInput below.
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
    int r = mtic_readInputAsInt(name);
    printf("%s changed to %d\n", name, r);
    mtic_writeOutputAsInt("output1", 2*r);
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
     mtic_startWithDevice call below for your agent to start.
     */
    char **devices = NULL;
    int nb = 0;
    mtic_getNetdevicesList(&devices, &nb);
    for (int i = 0; i < nb; i++){
        printf("device: %s\n", devices[i]);
    }
    mtic_freeNetdevicesList(devices, nb);
    
    //set the agent name
    if (argc == 2){
        //either by using the argument on the command line
        mtic_setAgentName(argv[1]);
    }else{
        //or by giving a default name if no argument was passed
        mtic_setAgentName("simpleDemoAgent");
    }
    
    //Create an input and an ouput: both are of integer type
    //with default value set to zero.
    mtic_createInput("input1", INTEGER_T, 0, 0);
    mtic_createOutput("output1", INTEGER_T, 0, 0);
    
    //Set the definition information (optional)
    mtic_setDefinitionName("myDef");
    mtic_setDefinitionVersion("1.0");
    mtic_setDefinitionDescription("Definition for our first agent.");
    
    //Attach the callback to our agent's input
    //NB: callbacks can be attached to any IOP
    mtic_observeInput("input1", myIOPCallback, NULL);
    
    //Actually and finally start the agent
    //First argument is the network device name to use
    //Second argument is the network port used by all the agents of your system
    mtic_startWithDevice("en0", 5669);
    
    getchar();
    
    //Stop the agent properly before terminating the program
    mtic_stop();
    
    return 0;
}
{{< / highlight >}}

### Compiling on Linux
As a prerequesite, you need to have the ingeScape library installed on your computer. We suppose the ingeScape header is installed in */usr/local/include/* and the library in */usr/local/lib/*, which is pretty standard on most Linux environments. 

If so, copy the code in a *main.c* file and type the following commands in a terminal where your *main.c* file is located:

{{< highlight shell "linenos=inline" >}}
gcc -W -Wall -g -I/usr/local/include/ -std=gnu99 -o main.o -c main.c
gcc -o myFirstAgent main.o -L/usr/local/lib -lmastic
{{< / highlight >}}

### Compiling on macOS (two methods)
The first compilation method is the same as the one for Linux, exactly with the same assumptions for header and library location.

The other method is the use of Apple Xcode with a dedicated project. To save your time, such a project, including the code above, can be found [here](/code/firstAgent.zip).

### Compiling on Microsoft Windows
*TODO*