+++
title = "Your first complete agent"
date = "2018-04-21T16:36:41+02:00"
tags = ["agents"]
categories = ["learn"]
banner = "img/banners/first_full_agent.png"
genre = "article"
+++

***ingeScape*** is designed to support both industrial and experimental environments. [Your first ingeScape agent](/blog/your_first_agent) illustrates the basics for both worlds. We provide here a more advanced example that can serve as a boiler plate for any real-world agent to be used with command lines in a terminal and developed in C without any other dependency.

This agent can receive parameters when started and commands when running. It is composed of three files:

- *main.c* contains the agent logic
- *regexp.h* is a utility header to handle commands entered when the agent is running
- *regexp.c* provides the implementations for the functions in *regex.h*

*regexp.h* and *regexp.c* are provided for your convenience.

The full code for the agent is provided and downloadable below. Before giving the full picture, we will describe the most significant parts of the code.


### Observe IOPs of various types

ingeScape supports different types for the inputs, outputs and parameters of the agents. Dedicated functions are available to read these types and even convert them. These dedicated functions are duplicated between inputs, outputs and parameters because a same name can be used independently in each of them.

To simply print an IOP value, the easiest way is to read it as a string and to use the ***igs_read{Input,Output,Parameter}AsString*** functions. In an ***observe*** callback, this looks like :

{{< highlight c "linenos=table,linenostart=1" >}}
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType,
                  void* value, size_t valueSize, void* myData){
    
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
    }
{{< / highlight >}}
<br>
If the IOP is an ***Input***, functions for each specific type can be used. They also support implicit conversions when it is possible. This gives the end of our callback:

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
}
{{< / highlight >}}

### Create command line arguments

There is nothing specific to ingeScape here but it is important for the community to share good practices about creating agents. The command line arguments are an important part of the public API of your agent. Well-chosen arguments will make agents more configurable and adaptable.

Here is what a function describing the arguments looks like:

{{< highlight c "linenos=table,linenostart=1" >}}
void print_usage(){
    printf("Usage example: firstFullAgent --verbose --port 5670 --name firstFullAgent\n");
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--definition : path to the definition file (default: %s)\n", DEFAULTDEFINITIONPATH);
    printf("--mapping : path to the mapping file (default: %s)\n", DEFAULTMAPPINGPATH);
    printf("--verbose : enable verbose mode in the application (default is disabled)\n");
    printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
    printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
    printf("--name agent_name : published name of this agent (default: %s)\n", agentName);
    printf("--noninteractiveloop : non-interactive loop for use as a deamon (default is false)\n");
}
{{< / highlight >}}
<br>
The arguments here above should be the minimal ones provided by each real agent, as they cover all the mandatory aspects of an agent from its name to the network parameters it will use. 

When relevant, agents can also load external ***definition*** and/or ***mapping***, as illustrated in this example. They are expressed in JSON format as illustrated in [The editor tour](/blog/editor_tour). As an exercise, you can copy them on your disk and try to load them into this agent.


### Introspection of agent's IOPs

When loading an external definition, an agent does not know its IOPs. That is why ingeScape provides introspection functions to list and display them.

Here is the code to do so:

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


### About mainloops

Agents are interactive software. As such, they need to continue their execution as long as something does not explicitly make them interrupt and stop. Interactive programs rely on at least one mainloop so that they won't stop by themselves. Because of the variety of loops in the various operating systems and industrial frameworks, we decided not to force the use of yet another mainloop mechanism in ingeScape. However, it is very easy to create your own infinite loop if you need to. This example shows two different ways of doing this.

Should you need more sophisticated mainloops, we advise you to use the ***select*** function available in ***\<sys/select.h\>*** or the mainloop extension capabilities of the framework you are using. Please note that ingeScape uses its own mainloop and threads that will not interfere with the one you are using. With some frameworks though, it will be necessary  to wrap the code in your callbacks to execute it into your frameworks's main thread or main queue. Check the ingeScape documentation for more details about this.

In this example, depending on the use of the *- - noninteractiveloop* command line parameter, two different loops can be activated. Both are based on a simple infinite *while* loop.

By default, the interactive loop mode is activated, i.e. the one enabling the use of commands inside the terminal once the agent is running. This loop is based on ***fgets*** function from ***\<stdio.h\>***, which has the capability to wait until some text is input in the terminal. In the case where the user uses the *Ctrl + c* key combination, this function returns *false*, which we use to break the infinite loop we have created. Here is the code for this loop:

{{< highlight c "linenos=table,linenostart=1" >}}
while (1) {
    char message [1024];
    if (!fgets (message, 1024, stdin))
        break;
    //handle the content of message here...
}
{{< / highlight >}}
<br>
The other mode consists in a simple infinite loop combined with a *sleep* command. The call to *sleep* is blocking for the passed duration (here 2 seconds). This is not ideal because your agent might take up to the *sleep* duration before actually leaving the loop upon an interruption. Here is the code for this loop:

{{< highlight c "linenos=table,linenostart=1" >}}
while (1) {
    if (interrupted){
        printf("Interruption signal received : stopping.\n");
        break;
    }
    sleep(2);
}
{{< / highlight >}}

### How to properly stop an agent

When an agent stops, it shall call the *igs_stop* function in order for the ingeScape thread to close properly. This also informs other agents on the platform that your agent is leaving. If an agent stops abruptly, other agents will detect it but it might take some time (up to 30 seconds).

The problem is how and when to call the *igs_stop* function, as there are many causes for an agent to stop :

- As a developer you may identify some situations when the agent shall stop (e.g. when typing a */quit* command in the terminal or when a certain value is received on a given input),
- The ingeScape editor and some special agents have the ability to stop other agents remotely,
- An interruption signal is received by the agent from the operating system.

For each of these cases, there a good practices to follow in order to avoid any dead lock between your application's threads and the ingeScape thread.

<br>
#### Which thread am i in ?

When using ingeScape, you are in your application's threads except when executing code in an ingeScape callback, whether it is an IOP observation callback or any other. When executing code in an ingeScape callback, you need to keep in mind that in some situations, it is necessary to wrap some or all of the code in a callback so that it may be executed in your application's threads. Most industrial frameworks such as Microsoft .Net, Apple's Cocoa, Qt, etc. provide methods to do that easily.

When using a simple C program as it is the case here, your application has only one main thread, plus the ones in ingeScape that you encounter in your callbacks code.

As all threads in an application share the same memory, most of the time, it is transparent for the developer to be executing code in an ingeScape callback or not. But when stopping the agent, one needs to pay extra attention because calling the *igs_stop* function from an ingeScape thread will result in a deadlock. The agent will stop anyway but the stop will not be perfectly clean.

<br>
#### Stop from any application's thread

If you are certain that you are not in an ingeScape callback, and thus not in an ingeScape thread, a simple call to the *igs_stop* function is sufficient to cleanly stop your agent. This is the easy case.

<br>
#### Handle system interruptions

The most common way to interrupt a program in a terminal is to press Ctrl + C in the terminal. This will send a SIGINT signal to your application. 

To handle this signal, you need to :

- register to SIGINT and call a function when the signal is emitted,
- create a global flag (or any other way to store an information) that is false by default and set to true when SIGINT is received,
- watch this flag in your main loop and stop when the flag is set to true.
interruption
In short, this requires the following code:

{{< highlight c "linenos=table,linenostart=1" >}}
//SIGINT handling
static volatile bool interruptionFlag = false;
void interruptionReceived(int val) {
    interruptionFlag = true;
}

//...

//register to SIGINT
signal(SIGINT, interruptionReceived);
//run the main loop (non-interactive mode)
while (1) {
    if (interruptionFlag){
        printf("Interruption received : stopping.\n");
        break;
    }
    sleep(2);
}
{{< / highlight >}}

<br>
#### Stop from an ingeScape callback

**It is important that the *igs_stop* function is never called from an ingeScape callback.** This can be avoided by using a global flag that is set to true in the callback and induces the stop of the main loop, and/or a function call in the main thread that actually executes a call to the *igs_stop* function.

<br>
#### Handle stop request from other agents

Some special agents can remotely stop any other agent. This has the effect to stop the ingeScape loop and threads. But some additional code is required for your application to stop properly as well.

Very simple, ingeScape brings the *igs_Interrupted* global variable that is false by default and becomes true when a stop request has been received or when the *igs_stop* function has been called. You can monitor this variable in your code to stop your application when a stop request or an actual stop is triggered.

It is possible to observe stop requests and attach callbacks to them. This is achieved by using the *igs_observeForcedStop* function. ***Be careful, such callbacks are executed in an ingeScape thread in which 
the igs_stop function shall not be used***.

### Pass commands through the terminal

In addition to command line parameters, it may be useful to pass commands to an agent when it is running into a terminal. Such commands can be helpful to get various informations from the agent or to configure it on the fly. Commands can be atomic or be composed of one or more parameters.

This is where we will use our *regexp.h* and *regexp.c* files to handle and parse what is typed into the terminal. In this example, it is necessary to end any keyboard input by the entry key. And we try to interpret commands only if they start by '/' and provide at least one additional character.

This example uses three different regular expressions, respectively capturing a command with :

- zero parameter, 
- one parameter,
- one parameter followed by a string of characters. 

Feel free to extend them according to your needs and improve their reliability if needed.

{{< highlight c "linenos=table,linenostart=1" >}}
//single command
const char *reg1 = "/([[:alnum:]]+)"; 
//command + parameter (single word)
const char *reg2 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)";
//command + parameter + string
const char *reg3 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)[[:space:]]{1}([[:print:]]+)"; 
{{< / highlight >}}
<br>
In the code, regular expressions are checked from the most complex to the simplest one, as checking the simplest one first would never allow more complex ones to be matched. Here is the code checking and handling the three regular expressions:

{{< highlight c "linenos=table,linenostart=1" >}}
                //command + parameter + string
                compile_regex(&r, reg3);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    char *parameter = my_matches.results[1];
                    char *message = my_matches.results[2];
                    printf("command: %s\nparameter: %s\nmessage: %s\n",command,parameter,message);
                    clean_matches(&my_matches);
                    continue;
                }
                //command + parameter (single word)
                compile_regex(&r, reg2);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    char *parameter = my_matches.results[1];
                    printf("command: %s\nparameter: %s\n",command,parameter);
                    clean_matches(&my_matches);
                    continue;
                }
                //single command
                compile_regex(&r, reg1);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    //printf("command: %s\n",command);
                    if (strcmp(command, "quit") == 0){
                        break; //simply stops the main loop
                    }
                    clean_matches(&my_matches);
                    continue;
                }
{{< / highlight >}}

<br>
*NB:* in this example we only have one actually implemented command which is **/quit**.

### ingeScape logging support

ingeScape supports a versatile logging mechanism that can display and store logs into the terminal console, log files and a log stream available in the ingeScape editor.

Logs contain priority levels, from *trace* to *fatal* that are common with most other logging systems. Logging functions work like the famous *printf* function with a variable number of parameters and the same matching syntax.

When displayed in the console, logs can use colors if *igs_setUseColorVerbose* is passed *true*.

Here is how to configure and use logs:

{{< highlight c "linenos=table,linenostart=1" >}}
    igs_setLogLevel(IGS_LOG_TRACE);
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

### Compile on Linux
As a prerequisite, you need to have the ingeScape library installed on your computer. We suppose the ingeScape header is installed in */usr/local/include/* and the library in */usr/local/lib/*, which is pretty standard on most Linux environments. 

The example can be downloaded [here](/code/firstFullAgent.zip). It provides a Makefile to compile the example and install it on your system if you want to.

Here is what to type in your terminal once your are in the same directory as the provided Makefile to run and stop the agent:

{{< highlight shell "linenos=inline" >}}
make
./firstFullAgent --help
./firstFullAgent --verbose
/quit
{{< / highlight >}}

### Compile on macOS (two methods)
The first compilation method is the same as the one for Linux, exactly with the same assumptions for header and library location.

The other method is the use of Apple Xcode with a dedicated project. To save your time, such a project, including the code above, is available [here](/code/firstFullAgent.zip).

### Compile on Microsoft Windows
*TODO*
