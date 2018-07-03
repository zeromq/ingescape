//
//  stopcallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "stopcallback.h"
#include <stdio.h>
#include <ingescape/ingescape.h>

// chained list to stock all stopCallbacks
typedef struct stopElement stopElement;
struct stopElement
{
    PyObject *call;         //stopCallback
    PyObject *argstopList;  // argument for stopCallback
    stopElement *suivant;
};

typedef struct stopListe stopListe;
struct stopListe
{
    stopElement *premier;
};

stopListe *initialisationStopCallback()
{
    stopListe *stopListe = malloc(sizeof(*stopListe));
    stopElement *stopElement = malloc(sizeof(*stopElement));
    
    if (stopListe == NULL || stopElement == NULL)
    {
        exit(EXIT_FAILURE);
    }
    
    stopElement->call = NULL;
    stopElement->argstopList = NULL;
    stopListe->premier = NULL;
    
    return stopListe;
}


void insertionStop(stopListe *stopListe, PyObject *nvcall, PyObject *nvargstopList)
{
    /* Création du nouvel élément */
    stopElement *nouveau = malloc(sizeof(*nouveau));
    if (stopListe == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }
    nouveau->call = nvcall;
    nouveau->argstopList = nvargstopList;
    
    /* insertionStop de l'élément au début de la stopListe */
    nouveau->suivant = stopListe->premier;
    stopListe->premier = nouveau;
}


stopListe *callbackStopstopList;

//stopCallback : execute the callback stacked with the igs_observeForcedStop_wrapper
void stop(void *myData){
    //run through all callbacks to execute them
    stopElement * actuel = callbackStopstopList -> premier;
    while (actuel != NULL){
        // Lock the GIL to execute the callback safely
        PyGILState_STATE d_gstate;
        d_gstate = PyGILState_Ensure();
        
        //execute the callback
        PyObject_CallObject(actuel->call, actuel->argstopList);
        
        //release the GIL
        PyGILState_Release(d_gstate);
        actuel = actuel -> suivant;
    }
    
}

// wrapper for igs_observeForcedStop
PyObject * igs_observeForcedStop_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject arg;
    PyObject *tempargstopList;

    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "OO", &temp, &arg)) {
        if (!PyCallable_Check(temp)) {  // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    if (callbackStopstopList == NULL){
        callbackStopstopList = initialisationStopCallback();
    }
    
    Py_XINCREF(temp);               // Add a reference to new callback
    
    tempargstopList = Py_BuildValue("(O)", arg); //cast arglist into a tuple
    
    Py_XINCREF(tempargstopList);    // Add a reference to arglist

    // add the callback to the list of stopCallback
    insertionStop(callbackStopstopList, temp, tempargstopList);

    igs_observeForcedStop(stop, NULL);
    
    //return 1 if ok
    PyObject *result;
    result = PyLong_FromLong(1);
    return result;
    
}

