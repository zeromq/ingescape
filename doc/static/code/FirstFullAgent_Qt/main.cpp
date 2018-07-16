//
//  main.c
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif

  #define NOMINMAX
  #include <windows.h>
  #include <winsock2.h>
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <maincontroller.h>
#include <QDebug>

extern "C"{
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <ingescape/ingescape.h>
#include "regexp.h" //regexp utilities
}


//default agent parameters to be overriden by command line paramters
int port = 5670;
QString agentName;
QString networkDevice;
bool verbose = false;


#ifdef Q_OS_WIN
 const char *homeVar= "HOMEPATH";
#else
 const char *homeVar = "HOME";
#endif

//definition and mapping as external resources
#define BUFFER_SIZE 1024
#define DEFAULTDEFINITIONPATH "~/Documents/IngeScape/data/definition.json"
char definitionFile[BUFFER_SIZE];
#define DEFAULTMAPPINGPATH "~/Documents/IngeScape/data/mapping.json"
char mappingFile[BUFFER_SIZE];

//SIGINT handling
static volatile bool interruptionFlag = false;
void interruptionReceived(int val) {
    interruptionFlag = true;
}

/*
 This callback is able to display the new value of any IOP it observes.
 In order to use the igs_read* functions, the IOP type must be checked
 first and then the value type.
 */
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType,
                  void* value, size_t valueSize, void* myData){

    if (valueType == IGS_IMPULSION_T){
        qDebug("%s changed (impulsion)\n", name);
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
        qDebug("%s changed to %s", name, convertedValue);
    }

    /*
     In the example of an Input, here is the code that could be used
     to get the actual value.
     Please not that igs_read*AsString require memory management.
    */
    if (iopType == IGS_INPUT_T){
        qDebug("input %s changed", name);
        switch (valueType) {
            case IGS_IMPULSION_T:
            qDebug(" (impulsion)\n");
            break;
            case IGS_BOOL_T:
            qDebug(" to %d\n", igs_readInputAsBool(name));
            break;
            case IGS_INTEGER_T:
            qDebug(" to %d\n", igs_readInputAsInt(name));
            break;
            case IGS_DOUBLE_T:
            qDebug(" to %lf\n", igs_readInputAsDouble(name));
            break;
            case IGS_STRING_T:
            {
                char *stringValue = igs_readInputAsString(name);
                qDebug(" to %s\n", (char *)stringValue);
                free(stringValue);
                break;
            }
            case IGS_DATA_T:
            //NB: for IGS_DATA_T, value and valueSize are already provided
            qDebug(" with size %zu\n", valueSize);
            break;
            default:
            break;
        }
    }
    //NB: exactly the same could be done for outputs and parameters
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(){
    qDebug("Usage example: firstFullAgent --verbose --port 5670 --name firstFullAgent\n");
    qDebug("\nthese parameters have default value (indicated here above):\n");
    qDebug("--definition : path to the definition file (default: %s)\n", DEFAULTDEFINITIONPATH);
    qDebug("--mapping : path to the mapping file (default: %s)\n", DEFAULTMAPPINGPATH);
    qDebug("--verbose : enable verbose mode in the application (default is disabled)\n");
    qDebug("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
    qDebug("--device device_name : name of the network device to be used (useful if several devices available)\n");
    qDebug("--name agent_name : published name of this agent (default: %s)\n", agentName);
    qDebug("--noninteractiveloop : non-interactive loop for use as a deamon (default is false)\n");
}

//helper to convert paths starting with ~ to absolute paths
void makeFilePath(char *from, char *to, size_t size_of_to){
    if (from[0] == '~'){
        from++;
        char *home = getenv(homeVar);
        strncpy(to, home, size_of_to);
        strncat(to, from, size_of_to);
    }else{
        strncpy(to, from, size_of_to);
    }
}


int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    // Register the MainController type so it can be accessed from qml
    qmlRegisterSingletonType<MainController>("FIRSTFULLAGENT", 1, 0, "MainC", &MainController::qmlSingleton);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // register to SIGINT
    signal(SIGINT, interruptionReceived);

    // Init our path variables
    char definitionPath[BUFFER_SIZE];
    strncpy(definitionPath, DEFAULTDEFINITIONPATH, BUFFER_SIZE*sizeof(char));
    char mappingPath[BUFFER_SIZE];
    strncpy(mappingPath, DEFAULTMAPPINGPATH, BUFFER_SIZE*sizeof(char));


    //
    // Command Line options
    //
    QCommandLineParser parser;
    parser.setApplicationDescription("First full Agent");

    // Option used to print agent usage
    QCommandLineOption helpOption("h", "Help");
    parser.addOption(helpOption);

    // Enable verbose mode in the application
    QCommandLineOption verboseOption("v", "Verbose");
    parser.addOption(verboseOption);

    // Path to the definition file
    QCommandLineOption definitionPathOption("f", "Definition path", "definitionpath");
    parser.addOption(definitionPathOption);

    // Path to the mapping file
    QCommandLineOption mappingPathOption("m", "Mapping path", "mappingpath");
    parser.addOption(mappingPathOption);

    // Port used for autodiscovery between agents
    QCommandLineOption portOption("p", "Port", "port");
    parser.addOption(portOption);

    // Name of the network device to be used (useful if several devices available)
    QCommandLineOption deviceOption("d", "Network device", "device", "Ethernet");
    parser.addOption(deviceOption);

    // Published name of this agent
    QCommandLineOption nameOption("n", "Agent name", "agentname", "firstFullAgent");
    parser.addOption(nameOption);

    // Process the actual command line arguments given by the user
    parser.process(app);

    //
    // Check option values
    //
    bool isSetHelpOption = parser.isSet(helpOption);
    if (isSetHelpOption) {
        print_usage();
    }

    bool isSetVerboseOption = parser.isSet(verboseOption);
    if (isSetVerboseOption) {
        verbose = true;
    }

    bool isSetDefinitionPathOption = parser.isSet(definitionPathOption);
    if (isSetDefinitionPathOption) {
        std::string strDefinition = parser.value(definitionPathOption).toStdString();
        strncpy(definitionPath, strDefinition.c_str(), BUFFER_SIZE);
    }

    bool isSetMappingPathOption = parser.isSet(mappingPathOption);
    if (isSetMappingPathOption) {
        std::string strMapping = parser.value(mappingPathOption).toStdString();
        strncpy(mappingPath, strMapping.c_str(), BUFFER_SIZE);
    }

    bool isSetPortOption = parser.isSet(portOption);
    if (isSetPortOption) {
        port = parser.value(portOption).toInt();
    }

    networkDevice = parser.value(deviceOption);
    agentName = parser.value(nameOption);

    makeFilePath(definitionPath, definitionFile, BUFFER_SIZE*sizeof(char));
    makeFilePath(mappingPath, mappingFile, BUFFER_SIZE*sizeof(char));

    igs_setLogLevel(IGS_LOG_TRACE);
    igs_setVerbose(verbose);
    igs_setUseColorVerbose(verbose);
    igs_setLogInFile(verbose);
    igs_setLogStream(verbose);
    igs_setLogPath("./log.csv");

    const char *example = "example log message";
    igs_trace("this is a trace %s", example);
    igs_debug("this is a debug %s", example);
    igs_info("this is an info %s", example);
    igs_warn("this is a warning %s", example);
    igs_error("this is an error %s", example);
    igs_fatal("this is a fatale %s", example);

    igs_setAgentName(agentName.toStdString().c_str());

    //load definition
    igs_loadDefinitionFromPath(definitionFile);

    //load mapping
    igs_loadMappingFromPath(mappingFile);

    //explore and print definition
    long numberOfEntries;
    char **myEntries = igs_getInputsList(&numberOfEntries);
    qDebug("Inputs :\n");
    for (int i = 0; i< numberOfEntries; i++){
        qDebug("\t%s\n", myEntries[i]);
        igs_observeInput(myEntries[i], myIOPCallback, NULL);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
    myEntries = igs_getOutputsList(&numberOfEntries);
    qDebug("Outputs :\n");
    for (int i = 0; i< numberOfEntries; i++){
        qDebug("\t%s\n", myEntries[i]);
        igs_observeOutput(myEntries[i], myIOPCallback, NULL);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
    myEntries = igs_getParametersList(&numberOfEntries);
    qDebug("Parameters :\n");
    for (int i = 0; i< numberOfEntries; i++){
        qDebug("\t%s\n", myEntries[i]);
        igs_observeParameter(myEntries[i], myIOPCallback, NULL);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);

    igs_startWithDevice(networkDevice.toStdString().c_str(), port);

    int execValue = app.exec();

    //Stop the agent properly before terminating the program
    igs_stop();

    return execValue;
}
