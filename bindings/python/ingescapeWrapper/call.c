//
//  call.c
//  ingescapeWrapp
//
//  Created by vaugien on 24/03/2020.
//  Copyright © 2020 ingenuity. All rights reserved.
//

#include "call.h"
#include <Python.h>
#include <ingescape/ingescape_advanced.h>
#include "uthash/utlist.h"

#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif


/* igs_sendCall
 *Function in c that wrapp all the send call dynamic in python
 */
PyObject * sendCall_wrapper(PyObject * self, PyObject * args)
{
    igs_callArgument_t *argumentList = NULL;
    char* agentNameOrUUID;
    char *callName;
    PyObject *argTuple = NULL;
    
    if (PyTuple_Size(args) != 3){
        printf("Expect 3 arguments, %zu were given \n", PyTuple_Size(args));
        return PyLong_FromLong(-1);
    }
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "ssO",&agentNameOrUUID, &callName, &argTuple)) {
        if(!PyList_Check(argTuple)){
            return PyLong_FromLong(-1);
        }
    }
    
    //PyList parsing
    size_t tupleArgumentSize = PyTuple_Size(argTuple);
    for (size_t index = 0; index < tupleArgumentSize; index++){
        if(!PyTuple_Check(PyTuple_GetItem(argTuple, index))){
            return PyLong_FromLong(-1);
        }

        PyObject *newArgument = PyTuple_GetItem(argTuple, index);
        size_t sizeArgument = PyTuple_Size(newArgument);
        int typeValue;
        PyObject *argumentValue;
        if (sizeArgument != 2){
            printf("Argument should be (type, argument value) \n\n");
            return PyLong_FromLong(-1);
        }else{
            if (!PyArg_ParseTuple(newArgument, "iO", &typeValue, &argumentValue)){
                printf("Argument should be (type(int), argument value) \n\n");
                return PyLong_FromLong(-1);
            }
        }

        if(typeValue == 1){
            bool value;
            if (!PyArg_ParseTuple(newArgument, "ib", &typeValue, &value)){
                printf("type specifies bool and argument is not bool\n");
                return PyLong_FromLong(-1);
            }
            printf("Bool : %i \n", value);
            igs_addBoolToArgumentsList(&argumentList, value);
        }else if(typeValue == 2){
            int value;
            if (!PyArg_ParseTuple(newArgument, "ii", &typeValue, &value)){
                printf("type specifies int and argument is not int\n");
                return PyLong_FromLong(-1);
            }
            printf("Int : %i \n", value);
            igs_addIntToArgumentsList(&argumentList, value);
        }else if(typeValue == 3){
            double value;
            if (!PyArg_ParseTuple(newArgument, "id", &typeValue, &value)){
                printf("type specifies double and argument is not double\n");
                return PyLong_FromLong(-1);
            }
            printf("Double : %f \n", value);
            igs_addDoubleToArgumentsList(&argumentList, value);
        }else if(typeValue == 4){
            const char *value;
            if (!PyArg_ParseTuple(newArgument, "is", &typeValue, &value)){
                printf("type specifies string and argument is not string\n");
                return PyLong_FromLong(-1);
            }
            printf("String : %s \n", value);
            igs_addStringToArgumentsList(&argumentList, value);
        }else if(typeValue == 5){
            long size;
            Py_buffer buf;
            if (!PyArg_ParseTuple(newArgument, "iy*k", &typeValue, &buf, &size)){
                printf("type specifies data and argument is not data\n");
                return PyLong_FromLong(-1);
            }
            igs_addDataToArgumentsList(&argumentList, buf.buf, (size_t)size);
        }else{
            printf("Type should be 1:bool, 2:int, 3:double, 4:char*, 5:data\n");
            return PyLong_FromLong(-1);
        }
    }
    int result = igs_sendCall(agentNameOrUUID, callName, &argumentList);
    return PyLong_FromLong(result);
}


/* igs_removeCall
 *Function in c that wrapp the removeCall Function
 */
PyObject * removeCall_wrapper(PyObject * self, PyObject * args){
    char *name;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_removeCall(name);
    return PyLong_FromLong(result);
}

/* igs_addArgumentToCall
 *Function in c that wrapp the addArgumentToCall function
 */
PyObject * addArgumentToCall_wrapper(PyObject * self, PyObject * args){
    char *callName;
    char *argName;
    int type;
    int result;    
    // parse arguments
    if (!PyArg_ParseTuple(args, "ssi", &callName, &argName, &type)) {
        return PyLong_FromLong(-1);
    }
    result = igs_addArgumentToCall(callName, argName, type);
    return PyLong_FromLong(result);
}


/* igs_removeArgumentFromCall
 *Function in c that wrapp the removeArgumentFromCall function
 */
PyObject * removeArgumentFromCall_wrapper(PyObject * self, PyObject * args){
    char *callName;
    char *argName;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "ss", &callName, &argName)) {
        return PyLong_FromLong(-1);
    }
    result = igs_removeArgumentFromCall(callName, argName);
    return PyLong_FromLong(result);
}

//igs_getNumberOfCalls
PyObject * getNumberOfCalls_wrapper(PyObject * self, PyObject * args)
{
    size_t result;
    PyObject * ret;
    result = igs_getNumberOfCalls();
    return PyLong_FromLong(result);
}

//igs_checkCallExistence
PyObject * checkCallExistence_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    if (igs_checkCallExistence(name)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

//igs_getCallsList
PyObject * getCallsList_wrapper(PyObject * self, PyObject * args)
{
    PyObject * ret;

    size_t nbOfElements = igs_getNumberOfCalls();
    char **result = igs_getCallsList(&nbOfElements);
    
    // build the resulting list into a Python object.
    ret = PyTuple_New(nbOfElements);
    size_t i ;
    for (i = 0; i < nbOfElements; i++){
        //set items of the python list one by one
        PyTuple_SetItem(ret, i, PyBytes_FromString(result[i]));
    }
    igs_freeCallsList(result, nbOfElements);
    return ret;
}

//igs_getNumberOfArgumentForCall
PyObject * getNumberOfArgumentForCall_wrapper(PyObject * self, PyObject * args)
{
    char* callName;
    // parse the number of element
    if (!PyArg_ParseTuple(args, "s", &callName)) {
        return NULL;
    }
    size_t result = igs_getNumberOfArgumentsForCall(callName);
    return PyLong_FromLong(result);
}


//igs_getArgumentListForCall
PyObject * getArgumentListForCall_wrapper(PyObject * self, PyObject * args)
{
    char* callName;
    
    // parse the number of element
    if (!PyArg_ParseTuple(args, "s", &callName)) {
        printf("Error parsing in getArgumentListForCall");
        return NULL;
    }
    
    igs_callArgument_t *firstElement = igs_getFirstArgumentForCall(callName);
    size_t nbOfElements = igs_getNumberOfArgumentsForCall(callName);
    
    // build the resulting list into a Python object.
    PyObject *ret = PyTuple_New(nbOfElements);
    size_t index = 0;
    igs_callArgument_t *newArg = NULL;
    LL_FOREACH(firstElement, newArg){
        PyTuple_SetItem(ret, index, Py_BuildValue("(si)",newArg->name, newArg->type));
    }
    return ret;
}

//igs_checkCallArgumentExistence
PyObject * checkCallArgumentExistence_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    char * argName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &argName)) {
        return NULL;
    }
    if (igs_checkCallArgumentExistence(callName, argName)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

typedef struct callCallback {
    char *callName;      // name of the iop
    PyObject *call;     //observeCallback
    PyObject *arglist;  //argument of the callback
    struct callCallback *next;
    struct callCallback *prev;
}callCallback;
callCallback *callList = NULL;


void observeCall(const char *senderAgentName, const char *senderAgentUUID,
             const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
             void* myData){
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();
    //run through all callbacks to execute them
    callCallback *actuel = NULL;
    DL_FOREACH(callList, actuel){
        if (strcmp(actuel->callName, callName) == 0){
            PyObject *tupleArgs = PyTuple_New(nbArgs);
            igs_callArgument_t *currentArg = NULL;
            size_t index = 0;
            LL_FOREACH(firstArgument, currentArg){
                switch(currentArg->type){
                    case IGS_BOOL_T:
                        printf("Bool found : %i \n", currentArg->i);
                        if (currentArg->b){
                            PyTuple_SetItem(tupleArgs, index, Py_True);
                        }else{
                            PyTuple_SetItem(tupleArgs, index, Py_False);
                        }
                        break;
                    case IGS_INTEGER_T:
                        printf("Int found : %i \n", currentArg->i);
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("i", currentArg->i));
                        break;
                    case IGS_DOUBLE_T:
                        printf("Double found : %i \n", currentArg->i);
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("d", currentArg->d));
                        break;
                    case IGS_STRING_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("s", currentArg->c));
                        break;
                    case IGS_IMPULSION_T:
                        PyTuple_SetItem(tupleArgs, index, Py_None);
                        break;
                    case IGS_DATA_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("y#", currentArg->data, currentArg->size));
                        break;
                    case IGS_UNKNOWN_T:
                        break;
                }
                index ++;
            }
            // call python code
            PyObject *pyAgentName = Py_BuildValue("(sssOO)", senderAgentName, senderAgentUUID, callName, tupleArgs, actuel->arglist);
            PyObject *KWARGS = NULL;
            PyObject_Call(actuel->call, pyAgentName, KWARGS);
            Py_XDECREF(pyAgentName);
            Py_XDECREF(KWARGS);
            
            break;
        }
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject * initCall_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *callName;

    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &callName, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(temp);       // Add a reference to new callback

    temparglist = Py_BuildValue("(O)", arg);    //cast arglist into a tuple

    Py_INCREF(temparglist);     // Add a reference to arglist

    // add the callback to the list of Callback
    callCallback *newElt = calloc(1, sizeof(callCallback));
    newElt->callName = strndup(callName, strlen(callName));
    newElt->arglist = temparglist;
    newElt->call = temp;
    
    int ret = igs_initCall(callName, observeCall, NULL);
    if (ret == 1){
        DL_APPEND(callList, newElt);
    }
    return PyLong_FromLong(ret);

}
