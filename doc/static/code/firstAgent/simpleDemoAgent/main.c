
#include <stdio.h>
#include <mastic/mastic.h>

void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData){
    int r = mtic_readInputAsInt(name);
    printf("%s changed to %d\n", name, r);
    mtic_writeOutputAsInt("output1", 2*r);
}


int main(int argc, const char * argv[]) {
    char **devices = NULL;
    int nb = 0;
    mtic_getNetdevicesList(&devices, &nb);
    for (int i = 0; i < nb; i++){
        printf("device: %s\n", devices[i]);
    }
    mtic_freeNetdevicesList(devices, nb);
    
    if (argc == 2){
        mtic_setAgentName(argv[1]);
    }else{
        mtic_setAgentName("simpleDemoAgent");
    }
    
    mtic_createInput("input1", INTEGER_T, 0, 0);
    mtic_createOutput("output1", INTEGER_T, 0, 0);
    
    mtic_setDefinitionName("myDef");
    mtic_setDefinitionVersion("1.0");
    mtic_setDefinitionDescription("Definition for our first agent.");
    
    mtic_observeInput("input1", myIOPCallback, NULL);
    
    mtic_startWithDevice("en0", 5669);
    
    getchar();
    
    mtic_stop();
    
    return 0;
}
