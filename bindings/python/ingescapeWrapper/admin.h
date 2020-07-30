//
//  admin.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef admin_h
#define admin_h

#include <Python.h>
#include <frameobject.h>

//wrapper for igs_setCommandLine
PyObject * setCommandLine_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setVerbose
PyObject * setVerbose_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_getVerbose
PyObject * getVerbose_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setLogStream
PyObject * setLogStream_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_getLogStream
PyObject * getLogStream_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setLogInFile
PyObject * setLogInFile_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_getLogInFile
PyObject * getLogInFile_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setUseColorVerbose
PyObject * setUseColorVerbose_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_getUseColorVerbose
PyObject * getUseColorVerbose_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setLogPath
PyObject * setLogPath_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_getLogPath
PyObject * getLogPath_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_interrupted
PyObject * isStarted_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setDefinitionPath
PyObject * setDefinitionPath_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_setMappingPath
PyObject * setMappingPath_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_writeDefinitionToPath
PyObject * writeDefinitionToPath_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_writeMappingToPath
PyObject * writeMappingToPath_wrapper(PyObject * self, PyObject * args);

//wrapper for igs_version
PyObject * version_wrapper(PyObject * self, PyObject * args);

PyObject * trace_wrapper(PyObject * self, PyObject * args);

PyObject * debug_wrapper(PyObject * self, PyObject * args);

PyObject * info_wrapper(PyObject * self, PyObject * args);

PyObject * warn_wrapper(PyObject * self, PyObject * args);

PyObject * error_wrapper(PyObject * self, PyObject * args);

PyObject * fatal_wrapper(PyObject * self, PyObject * args);

PyObject * setLogLevel_wrapper(PyObject * self, PyObject * args);

PyObject * getLogLevel_wrapper(PyObject * self, PyObject * args);

PyObject * setLicensePath_wrapper(PyObject * self, PyObject * args);

PyObject * igs_getNetdevicesList_wrapper(PyObject * self, PyObject * args);

PyObject * igs_getNetadressesList_wrapper(PyObject * self, PyObject * args);

PyDoc_STRVAR(
            setDefinitionPathDoc,
             "igs_setDefinitionPath()\n"
             "--\n"
             "\n"
             "Set the path to write the current definition with igs_writeDefinitionToPath .");

PyDoc_STRVAR(
             setMappingPathDoc,
             "igs_setMappingPath()\n"
             "--\n"
             "\n"
             "Set the path to write the current mapping with igs_writeMappingToPath.");

PyDoc_STRVAR(
             writeDefinitionToPathDoc,
             "igs_writeDefinitionToPath()\n"
             "--\n"
             "\n"
             "Write the current definition to th file setted in igs_setDefinitionPath.");

PyDoc_STRVAR(
             writeMappingToPathDoc,
             "igs_writeMappingToPath()\n"
             "--\n"
             "\n"
             "Write the current mapping to th file setted in igs_setMappingPath.");

PyDoc_STRVAR(
             versionDoc,
             "igs_version()\n"
             "--\n"
             "\n"
             "Return the version of IngeScape.");

PyDoc_STRVAR(
             interruptedDoc,
             "igs_interrupted()\n"
             "--\n"
             "\n"
             "Return True if agent is stopped.");

PyDoc_STRVAR(
             setCommandLineDoc,
             "igs_setCommandLine(commandLine)\n"
             "--\n"
             "\n"
             "Command line for the agent can be passed here for inclusion in the agent's headers.\n"
             "If not set, header is initialized with exec path.\n");

PyDoc_STRVAR(
             setVerboseDoc,
             "igs_setVerbose(verbose)\n"
             "--\n"
             "\n"
             "enable log in console (enabled by default).\n Param verbose is a boolean");

PyDoc_STRVAR(
             isVerboseDoc,
             "igs_isVerbose()\n"
             "--\n"
             "\n"
             "Return True if log in console is enabled (enabled by default).");

PyDoc_STRVAR(
             setLogStreamDoc,
             "igs_setLogStream(log)\n"
             "--\n"
             "\n"
             "enable log in socket.\n Param verbose is a boolean");

PyDoc_STRVAR(
             getLogStreamDoc,
             "igs_getLogStream()\n"
             "--\n"
             "\n"
             "Return True if log in socket is enabled.");

PyDoc_STRVAR(
             setLogInFileDoc,
             "igs_setLogInFile(log)\n"
             "--\n"
             "\n"
             "enable log in file.\n Param verbose is a boolean");

PyDoc_STRVAR(
             getLogInFileDoc,
             "igs_getLogInFile()\n"
             "--\n"
             "\n"
             "Return True if log in file is enabled.");

PyDoc_STRVAR(
             setLogPathDoc,
             "igs_setLogPath(path)\n"
             "--\n"
             "\n"
             "Set the path of the file were log will be saved.\n Param is a string containing the path to the file");

PyDoc_STRVAR(
             getLogPathDoc,
             "igs_getLogPath()\n"
             "--\n"
             "\n"
             "Get the path of the file were log will be saved.\n Return a string containing the path to the file");

PyDoc_STRVAR(
             setUseColorVerboseDoc,
             "igs_setUseColorVerbose(verbose)\n"
             "--\n"
             "\n"
             "Enable use colors in console.\n Param is boolean.");

PyDoc_STRVAR(
             getUseColorVerboseDoc,
             "igs_getUseColorVerbose()\n"
             "--\n"
             "\n"
             "Return true if colors are used in console.");






#endif /* admin_h */
