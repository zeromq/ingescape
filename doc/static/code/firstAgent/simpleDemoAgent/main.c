
#include <stdio.h>
#include <ingescape/ingescape.h>

void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData){
    int r = igs_readInputAsInt(name);
    printf("%s changed to %d\n", name, r);
    igs_writeOutputAsInt("output1", 2*r);
}


int main(int argc, const char * argv[]) {
    char **devices = NULL;
    int nb = 0;
    igs_getNetdevicesList(&devices, &nb);
    for (int i = 0; i < nb; i++){
        printf("device: %s\n", devices[i]);
    }
    igs_freeNetdevicesList(devices, nb);
    
    if (argc == 2){
        igs_setAgentName(argv[1]);
    }else{
        igs_setAgentName("simpleDemoAgent");
    }
    
    igs_createInput("input1", IGS_INTEGER_T, 0, 0);
    igs_createOutput("output1", IGS_INTEGER_T, 0, 0);
    
    igs_setDefinitionName("myDef");
    igs_setDefinitionVersion("1.0");
    igs_setDefinitionDescription("Definition for our first agent.");
    
    igs_observeInput("input1", myIOPCallback, NULL);
    
    igs_startWithDevice("en0", 5669);
    
    getchar();
    
    igs_stop();
    
    return 0;
}
