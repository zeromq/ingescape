//
//  observecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//
#include "observecallback.h"
#include <stdio.h>
#include <ingescape/ingescape.h>

typedef struct observeElement observeElement;
struct observeElement
{
    char *nameArg;      // name of the iop
    PyObject *call;     //observeCallback
    PyObject *arglist;  //argument of the callback
    observeElement *suivant;
};

typedef struct observeListe observeListe;
struct observeListe
{
    observeElement *premier;
};

observeListe *initialisationCallbackObserve()
{
    observeListe *observeListe = malloc(sizeof(*observeListe));
    observeElement *observeElement = malloc(sizeof(*observeElement));
    
    if (observeListe == NULL || observeElement == NULL){
        exit(EXIT_FAILURE);
    }
    
    observeElement->call = NULL;
    observeElement->nameArg = NULL;
    observeElement->arglist = NULL;
    observeListe->premier = NULL;

    return observeListe;
}


void insertionObserve(observeListe *observeListe, PyObject *nvcall, PyObject *nvarglist, char *nvname)
{
    /* Création du nouvel élément */
    observeElement *nouveau = malloc(sizeof(*nouveau));
    if (observeListe == NULL || nouveau == NULL){
        exit(EXIT_FAILURE);
    }
    Py_INCREF(nvcall);
    Py_INCREF(nvarglist);
    nouveau->call = nvcall;
    nouveau->nameArg = nvname;
    nouveau->arglist = nvarglist;
    Py_INCREF(nouveau->call);
    Py_INCREF(nouveau->arglist);
    
    /* insertionObserve de l'élément au début de la observeListe */
    nouveau->suivant = observeListe->premier;
    observeListe->premier = nouveau;
}


observeListe *callbackObserveList;

//observeCallback that execute the callback for the iop that has benn changed
void observe(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData){
    char* received = ((char *)myData);
    observeElement * actuel = callbackObserveList -> premier;
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();
    //run through all callbacks to execute them
    while (actuel != NULL){
        if (actuel->nameArg == received){
            
            // call python code
            PyObject_CallObject(actuel->call, actuel->arglist);
        }
        actuel = actuel -> suivant;
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

 PyObject * igs_observeInput_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *input;
    
    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &input, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    if (callbackObserveList == NULL){
        callbackObserveList = initialisationCallbackObserve();
    }
    
    Py_XINCREF(temp);       // Add a reference to new callback
    
    temparglist = Py_BuildValue("(O)", arg);    //cast arglist into a tuple

    Py_INCREF(temparglist);     // Add a reference to arglist

    // add the callback to the list of Callback
    insertionObserve(callbackObserveList, temp, temparglist, input);

    int ret = igs_observeInput(input, observe, input);
    
    //return 1 if ok
    PyObject *result;
    result = PyLong_FromLong(ret);
    return result;
    
}


 PyObject * igs_observeOutput_wrapper(PyObject *self, PyObject *args)
{
    
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *output;
    
    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &output, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    if (callbackObserveList == NULL){
        callbackObserveList = initialisationCallbackObserve();
    }
    
    Py_XINCREF(temp);         // Add a reference to new callback
    
    temparglist = Py_BuildValue("(O)", arg);    //cast arglist into a tuple
    
    Py_XINCREF(temparglist);         // Add a reference to arglist
    
    // add the callback to the list of Callback
    insertionObserve(callbackObserveList, temp, temparglist, output);

    int ret = igs_observeOutput(output, observe, output);
    
    //return 1 if ok
    PyObject *result;
    result = PyLong_FromLong(ret);
    return result;
    
}

 PyObject * igs_observeParameter_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *param;
    
    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &param, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    if (callbackObserveList == NULL){
        callbackObserveList = initialisationCallbackObserve();
    }
    
    Py_XINCREF(temp);         // Add a reference to new callback
    
    
    temparglist = Py_BuildValue("(O)", arg);        //cast arglist into a tuple
    
    Py_XINCREF(temparglist);         // Add a reference to arglist
    
    // add the callback to the list of Callback
    insertionObserve(callbackObserveList, temp, temparglist, param);

    int ret = igs_observeParameter(param, observe, param);
    
    //return 1 if ok
    PyObject *result;
    result = PyLong_FromLong(ret);
    return result;
    
}

