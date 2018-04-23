//
//  main.c
//  firstAgent
//

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
        mtic_setAgentName("firstAgent");
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

