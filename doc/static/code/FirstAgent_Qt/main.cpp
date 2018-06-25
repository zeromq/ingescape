#include <QGuiApplication>
#include <QQmlApplicationEngine>

extern "C"{
#include <ingescape/ingescape.h>
}

/*
 This is a callback function that runs everytime the
 input "input1" is written. The callback is linked to
 the input by the use of igs_observeInput below.
 The behavior of this agent is very simple:
 - reading the changed input
 - printing the new value
 - multiplying the value by 2 and writing the result to the output
 */
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData){
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
int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    /*
     Use ingeScape function to find compatible network devices
     on your computer.
     You need to use one of the returned results in the
     igs_startWithDevice call below for your agent to start.
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
    igs_startWithIP("10.0.0.151", 5669);

    return app.exec();
}

