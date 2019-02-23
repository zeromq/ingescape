+++
title = "Your first complete agent"
date = "2018-04-21T16:36:41+02:00"
tags = ["agents"]
categories = ["learn"]
banner = "img/banners/first_full_agent.png"
genre = "article"
+++

***ingeScape*** is designed to support both industrial and experimental environments. [Your first ingeScape agent](/blog/your_first_agent) illustrates the basics for both worlds. In this article, we provide a more advanced example that can serve as a boiler plate for any real-world agent to be used with command lines in a terminal on Windows, Linux or macos and developed in C with or without any other dependency.

This agent can receive parameters when started and commands through the command line when running. It is composed of a single  *main.c* file.

The full code for the agent is provided and downloadable below. Before giving the full picture, we will describe the most significant parts of the code.


### Handle the various types of IOPs 

ingeScape supports different types for the inputs, outputs and parameters of the agents. Dedicated functions are available to read these types and even convert them. These dedicated functions are common to inputs, outputs and parameters. A same name can be used independently in each of them.

To simply print an IOP value, the easiest way is to read it as a string and to use the ***igs_read{Input,Output,Parameter}AsString*** functions. In an ***observe*** callback as demonstrated in [your first ingeScape agent](/blog/your_first_agent), this looks like that:

{{< highlight c "linenos=table,linenostart=1" >}}
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType,
                  void* value, size_t valueSize, void* myData){
    
    //NB: value is a pointer to the new IOP value. If you are sure of
    //what you are doing, you can use it directly. If not, use the rest
    //of the code in this function.
    //valueSize is the actual size of the new IOP value.
    
    if (valueType == IGS_IMPULSION_T){
        printf("%s changed (impulsion)\n", name);
    } else {
        char *convertedValue = NULL;
        switch (iopType) {
            case IGS_INPUT_T:
                convertedValue = igs_readInputAsString(name);
                break;
            case IGS_OUTPUT_T:
                convertedValue = igs_readOutputAsString(name);
                break;
            case IGS_PARAMETER_T:
                convertedValue = igs_readParameterAsString(name);
                break;
            default:
                break;
        }
        printf("%s changed to %s", name, convertedValue);
        if (convertedValue != NULL){
            free(convertedValue);
        }
    }
{{< / highlight >}}
<br>
Functions for each specific type can be used. They  support implicit conversions when permitted by the C language. Here is the corresponding code:

{{< highlight c "linenos=table,linenostart=1" >}}
    if (iopType == IGS_INPUT_T){
        printf("input %s changed", name);
        switch (valueType) {
            case IGS_IMPULSION_T:
                printf(" (impulsion)\n");
                break;
            case IGS_BOOL_T:
                printf(" to %d\n", igs_readInputAsBool(name));
                break;
            case IGS_INTEGER_T:
                printf(" to %d\n", igs_readInputAsInt(name));
                break;
            case IGS_DOUBLE_T:
                printf(" to %lf\n", igs_readInputAsDouble(name));
                break;
            case IGS_STRING_T:
            {
                char *stringValue = igs_readInputAsString(name);
                printf(" to %s\n", (char *)stringValue);
                free(stringValue);
                break;
            }
            case IGS_DATA_T:
                //NB: for IGS_DATA_T, value and valueSize are already provided
                printf(" with size %zu\n", valueSize);
                break;
            default:
                break;
        }
    }
    //NB: exactly the same could be done for outputs and parameters
{{< / highlight >}}

### Create command line arguments

There is nothing specific to ingeScape here but it is important for the community to share good common practices about creating agents. The command line arguments are an important part of the public API of your agent. Well-chosen arguments will make agents more configurable and adaptable.

Here is what a function describing the arguments looks like:

{{< highlight c "linenos=table,linenostart=1" >}}
void print_usage() {
	printf("Usage example: firstFullAgent --verbose --port %d --device device_name --name firstFullAgent\n", port);
	printf("\nthese parameters have default value (indicated here above):\n");
	printf("--definition : path to the definition file (default: %s)\n", DEFAULTDEFINITIONPATH);
	printf("--mapping : path to the mapping file (default: %s)\n", DEFAULTMAPPINGPATH);
	printf("--verbose : enable verbose mode in the application (default is disabled)\n");
	printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
	printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
	printf("--name agent_name : published name for this agent (default: %s)\n", agentName);
	printf("--interactiveloop : enables interactive loop to pass commands in CLI (default: false)\n");
}
{{< / highlight >}}
<br>
The arguments here above should be the minimal ones provided by each real agent, as they cover all the mandatory aspects of an agent from its name to the network parameters it will use. 

### Set the network device

ingeScape makes network simple and requires only two parameters to configure an agent:

- the port to be used to connect on a platform (all agents running on the same port will automatically see each other),
- the network device (or IP address) to be used.

It is not possible to provide a default network device name that would fit with all operating systems supported by ingeScape. However, we provide a small code snippet enabling to list available devices on your computer. Available devices are ones with a proper IP address and UDP broadcast capabilities. Note that ingeScape also supports advanced network configurations  for secured or specifically constrained networks.

The code snippet below not only lists compatible devices but also implements the following behavior:

- If only one compatible network device exists, the agent uses it as default.
- If several network devices are available, the agent stops and devices are listed in the console so that one can be selected and passed as ***--device*** parameter in the command line running the agent.
- If no device is found, agent notifies an error and stops.

{{< highlight c "linenos=table,linenostart=1" >}}
    if (networkDevice == NULL){
        //we have no device to start with: try to find one
        char **devices = NULL;
        int nb = 0;
        igs_getNetdevicesList(&devices, &nb);
        if (nb == 1){
            //we have exactly one compliant network device available: we use it
            networkDevice = strdup(devices[0]);
            igs_info("using %s as default network device (this is the only one available)", networkDevice);
        }else{
            if (nb == 0){
                igs_error("No network device found: aborting.");
                exit(1);
            }else{
                igs_error("No network device passed as command line parameter and several are available.");
                printf("Please use one of these network devices:\n");
                for (int i = 0; i < nb; i++){
                    printf("\t%s\n", devices[i]);
                }
                printf("\n");
                print_usage();
                exit(1);
            }
        }
        igs_freeNetdevicesList(devices, nb);
    }
{{< / highlight >}}

### Handle definition and mapping

The definition and mapping of an agent, as illustrated in the [editor tour](/blog/editor_tour), are managed with three complementary methods:

- They can be hardcoded in an agent, using the ingeScape API.
- They can be loaded from external files.
- They can be loaded from the ingeScape Editor.

This example is designed to use external files. You can  create such files, based on the examples in the [editor tour](/blog/editor_tour), as ***definition.json*** and ***mapping.json***, in your *Documents/IngeScape/examples* folder, set as the default locations using this code:

{{< highlight c "linenos=table,linenostart=1" >}}
//definition and mapping as external resources
#define BUFFER_SIZE 1024
#define DEFAULTDEFINITIONPATH "~/Documents/IngeScape/examples/definition.json"
static char definitionFile[BUFFER_SIZE];
#define DEFAULTMAPPINGPATH "~/Documents/IngeScape/examples/mapping.json"
static char mappingFile[BUFFER_SIZE];
{{< / highlight >}}

If you want to hardcode your agent definition and/or mapping, you can comment this code and use the ingeScape API instead (check the *Definitions* section of the ingeScape header file):

{{< highlight c "linenos=table,linenostart=1" >}}
	//load definition
	igs_loadDefinitionFromPath(definitionFile);
	//    char *definition = igs_getDefinition();
	//    printf("%s\n", definition);
	//    free(definition);

	//load mapping
	igs_loadMappingFromPath(mappingFile);
	//    char *mapping = igs_getMapping();
	//    printf("%s\n", mapping);
	//    free(mapping);
{{< / highlight >}}

### Use introspection on an agent's IOPs
ingeScape provides introspection functions to list and display them. Here is the code to do so:

{{< highlight c "linenos=table,linenostart=1" >}}
    long numberOfEntries;
    
    char **myEntries = igs_getInputsList(&numberOfEntries);
    printf("Inputs :\n");
    for (int i = 0; i< numberOfEntries; i++){
        printf("\t%s\n", myEntries[i]);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
    
    myEntries = igs_getOutputsList(&numberOfEntries);
    printf("Outputs :\n");
    for (int i = 0; i< numberOfEntries; i++){
        printf("\t%s\n", myEntries[i]);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
    
    myEntries = igs_getParametersList(&numberOfEntries);
    printf("Parameters :\n");
    for (int i = 0; i< numberOfEntries; i++){
        printf("\t%s\n", myEntries[i]);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
{{< / highlight >}}

Introspection might be useful in the following situations:

- Agent needs to configure itself based on the loaded definition or requires to verify it.
- Agent's definition is dynamic depending on external events or internal behavior and needs to verify and/or adapt its current definition or mapping.

### About main loops

Agents are interactive software. As such, they need to continue their execution as long as something does not explicitly make them interrupt and stop. Interactive programs rely on at least one mainloop so that they won't stop by themselves. There are many different types of loops available in the operating systems and industrial frameworks, with whom ingeScape can be used. This example shows how to manually create an interactive loop with support for keyboard inputs in the command line, and another way, using the CZMQ library, which is used by ingeScape and thus always available to you.

Should you need more sophisticated or customized mainloops, we advise you to use the ***select*** function available in ***\<sys/select.h\>*** or the mainloop extension capabilities of the industrial framework you are using. Please note that ingeScape uses its own mainloop and threads that will not interfere with the one you are using. With some frameworks though, it will be necessary  to wrap the code in your callbacks to execute it into your frameworks's main thread or main queue. Check the ingeScape documentation for more details about this.

In this example, depending on the use of the *--interactiveloop* command line parameter, two different loops can be activated.

When *--interactiveloop* is passed as a command line argument, the interactive loop mode is activated, i.e. the one enabling the use of commands inside the terminal once the agent is running. This loop is based on ***fgets*** function from ***\<stdio.h\>***, which has the capability to wait until some text is input in the terminal. In the case where the user uses the *Ctrl + c* key combination, this function returns *false*, which we use to break the infinite loop we created. Here is the code for this loop:

{{< highlight c "linenos=table,linenostart=1" >}}
while (1) {
    char message [1024];
    if (!fgets (message, 1024, stdin))
        break;
    //handle the content of message here...
}
{{< / highlight >}}
<br>
The other mode uses the ***zloop*** object provided by the CZMQ library. Here is the code for this loop:

{{< highlight c "linenos=table,linenostart=1" >}}
//Run the main loop (non-interactive mode):
//we rely on CZMQ which is an ingeScape dependency and is thus
//always here.
zloop_t *loop = zloop_new();
zloop_start(loop); //this function is blocking until SIGINT is received
zloop_destroy(&loop);
{{< / highlight >}}

### How to properly stop an agent ?

When an agent has to stop, it shall call the *igs_stop* function in order for the ingeScape threads to terminate properly. This call also informs other agents on the platform that your agent is leaving. If an agent stops abruptly, other agents will detect it but it might take some time (up to 30 seconds).

The problem is how and when to call the *igs_stop* function, as there are many causes for an agent to stop :

- As a developer you may identify some situations when the agent shall stop (e.g. when typing a */quit* command in the terminal or when a certain value is received on a given input, or upon a user action in a graphic application),
- The ingeScape editor and some special agents have the ability to stop other agents remotely,
- An interruption signal is received by the agent from the operating system.

For each of these cases, there are good practices to follow in order to avoid any dead lock between your application's threads and the ingeScape threads.

<br>
#### Which thread am i in ?

When using ingeScape, you are in your application's threads except when executing code in an ingeScape callback, whether it is an IOP observation callback or any other. When executing code in an ingeScape callback, you need to keep in mind that in some situations, it is necessary to wrap some or all of the code in a callback so that it may be executed in your application's threads. Most industrial frameworks such as Microsoft .Net, Apple's Cocoa, Qt, etc. provide methods to do that easily.

When using a simple C program as it is the case here, your application has only one main thread, plus the ones in ingeScape that you enter in your callbacks code.

As all threads in an application share the same memory, most of the time, it is transparent for the developer to be executing code in an ingeScape callback or not. **But when stopping the agent, one needs to pay extra attention because calling the *igs_stop* function from an ingeScape thread will result in a deadlock. The agent will stop anyway but the stop will not be perfectly clean.**

<br>
#### Stop from any application's thread

If you are certain that you are not in an ingeScape callback, and thus not in an ingeScape thread, a simple call to the *igs_stop* function is sufficient to cleanly stop your agent. This is an easy case.

<br>
#### Handle system interruptions

The most common way to interrupt a program in a terminal is to press Ctrl + C in the terminal. This will send a SIGINT signal to your application. The *zloop* provided by the CZMQ library automatically handles interruptions and does not require additional code. When an ingeScape editor forces your agent to stop, SIGINT is also triggered. This is another easy case.

In case you **really need to** handle this signal by yourself, you have to register to SIGINT and call a function when the signal is emitted. In short, this requires the following code:

{{< highlight c "linenos=table,linenostart=1" >}}
//SIGINT handling
void interruptionReceived(int val) {
    //your code here...
}

//...

//register to SIGINT
signal(SIGINT, interruptionReceived);
{{< / highlight >}}

<br>
#### Stop from an ingeScape callback

**It is important that the *igs_stop* function is never called from an ingeScape callback.** This can be avoided by using a global flag that is set to true in the callback and induces the stop of the main thread's loop, and/or a function call in the main thread that actually executes a call to the *igs_stop* function.

<br>
#### Register to "stop" requests from other agents

Some special agents can remotely stop any other agent. This has the effect to stop the ingeScape loop and threads and to trigger SIGINT to your application process. But sometimes additional code is required for your application to stop properly even when forced to stop.

It is possible to observe forced stop requests and attach callbacks to them. This is achieved by using the *igs_observeForcedStop* function. ***Be careful: such callbacks are executed in an ingeScape thread in which the igs_stop function shall not be used***.
<br>

### Pass commands through the terminal

In addition to command line parameters, it may be useful to pass commands to an agent when it is running into a terminal. Such commands can be helpful to get various informations from the agent or to configure it on the fly. Commands can be atomic or be composed of one or more parameters.

In this example, it is necessary to end any keyboard input by the entry key. And we will interpret commands only if they start by '/' and provide at least one additional character.

This example supports three different types of command patterns :

- command with zero parameter, 
- command with one parameter,
- command with one parameter, followed by a string of characters. 

Feel free to extend them according to your needs and improve their reliability if needed.

In the code, commands are checked from the most complex to the simplest one, as checking the simplest one first would never allow more complex ones to be matched. Here is the code checking and handling the three types of patterns:

{{< highlight c "linenos=table,linenostart=1" >}}
			if ((message[0] == '/') && (strlen(message) > 2)) {
				int matches = sscanf(message + 1, "%s %s%n%s", command, param1, &usedChar, param2);
				if (matches > 2) {
					// copy the remaining of the message in param 2
					strncpy(param2, message + usedChar + 2, BUFFER_SIZE);
					// remove '\n' at the end
					param2[strnlen(param2, BUFFER_SIZE) - 1] = '\0';
				}
				// Process command
				if (matches == -1) {
					//printf("Error: could not interpret message %s\n", message + 1);
				}else if (matches == 1) {
					if (strncmp(command, "quit", BUFFER_SIZE) == 0){
						break;
                    }else if(strncmp(command, "help", BUFFER_SIZE) == 0){
                        print_cli_usage();
                    }else {
						printf("Received command: %s\n", command);
					}
				}else if (matches == 2) {
					printf("Received command: %s + %s\n", command, param1);
				}else if (matches == 3) {
					printf("Received command: %s + %s + %s\n", command, param1, param2);
				}else{
					printf("Error: message returned %d matches (%s)\n", matches, message);
				}
			}
{{< / highlight >}}

<br>
*NB:* in this example we only have two actually implemented commands which are **/quit** and **/help**.

### ingeScape logging support

ingeScape supports a versatile logging mechanism that can display and store logs into the terminal console, log files and a log stream available in the ingeScape editor.

Logs contain priority levels, from *trace* to *fatal* that are usual to most logging systems. Logging functions work like the famous *printf* function with a variable number of parameters and the same matching syntax.

When displayed in the console, logs can use colors if *igs_setUseColorVerbose* is passed *true*.

Here is how to configure and use logs:

{{< highlight c "linenos=table,linenostart=1" >}}
    //NB: file log and stream log are enabled optionaly
    igs_setLogLevel(IGS_LOG_TRACE); //set log level to TRACE (default is INFO)
    igs_setVerbose(verbose);
    igs_setUseColorVerbose(verbose);
    igs_setLogInFile(verbose);
    igs_setLogStream(verbose);
    igs_setLogPath("./log.csv");
    
    char *example = "example log message";
    igs_trace("this is a trace %s", example);
    igs_debug("this is a debug %s", example);
    igs_info("this is an info %s", example);
    igs_warn("this is a warning %s", example);
    igs_error("this is an error %s", example);
    igs_fatal("this is a fatale %s", example);
{{< / highlight >}}

### Compile your agent
You can download the source code for this agent [here](/code/firstFullAgent.zip).

With this source code, you will be able to compile the agent for:

- Linux with
  - cmake
  - Qt
- Microsoft Windows with
  - Visual Studio 2017
  - Qt
  - cmake
- macos with
  - Xcode
  - cmake

In all cases, as a prerequisite, you need to have the ingeScape library installed on your computer. We provide installers and packages to do so.

With Microsoft Windows, depending on your system, you will be able to choose between 32-bit and 64-bit architectures.

#### cmake
In a terminal, once the source code has been extracted, just go inside the extracted folder and type the following commands:

{{< highlight shell "linenos=inline" >}}
cd src
mkdir build
cd build
cmake ..
make
./firstFullAgent --help
./firstFullAgent --verbose --interactiveloop
/quit
{{< / highlight >}}

Go to [https://cmake.org/download/](https://cmake.org/download/) to know hot to install cmake for your system.

#### Qt
Qt and Qt Creator need to be installed properly on your system. Then, once the source code has been extracted:

- go inside the builds/qt subfolder and open *firstFullAgent.pro* with Qt Creator
- configure the kit or kits for your environment
- build and run the agent

On macos, in some cases, if you are using Homebrew to install third-party libraries, there might be an incompatibility between the JPEG, TIFF, etc. libraries embedded in CoreImage and the ones installed in */usr/local/lib* by Homebrew. This has nothing to do with ingeScape but might block the execution of the example.

The following error message is then displayed at runtime:
{{< highlight shell "linenos=inline" >}}
dyld: Symbol not found: __cg_jpeg_resync_to_restart
  Referenced from: /System/Library/Frameworks/ImageIO.framework/Versions/A/ImageIO
  Expected in: /usr/local/lib/libJPEG.dylib
 in /System/Library/Frameworks/ImageIO.framework/Versions/A/ImageIO
{{< / highlight >}}

If this happens, you need to edit the Run properties for your Qt project by editing the DYLD_LIBRARY_PATH environment variable and adding */System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/* as the first path to be checked, generally followed by your local project path and */usr/local/lib/*.

#### Xcode
Xcode needs to be installed properly on your system. It can be downloaded from the App Store. Then, once the source code has been extracted:

- go inside the builds/macos subfolder and open *firstFullAgent.xcodeproj* with Xcode
- build and run the agent

#### Visual Studio  2017
Visual Studio 2017 needs to be installed properly on your system. It can be downloaded from [here](https://visualstudio.microsoft.com/fr/vs/features/cplusplus/). The Community version will be enough for this first agent.

 Then, once the source code has been extracted:
 
- go inside the builds/windows/project subfolder and open *firstFullAgent.sln* with Visual Studio
- pick your platform (32 or 64-bit) and mode (Debug or Release)
- build and run the agent

