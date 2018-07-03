//
//  freezecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "freezecallback.h"
#include <stdio.h>
#include <ingescape/ingescape.h>

// chained list to stock all freezeCallbacks
typedef struct Element Element;
struct Element
{
    PyObject *call;     //freeze callback
    PyObject *arglist;  //arguments for the callback
    Element *suivant;
};

typedef struct Liste Liste;
struct Liste
{
    Element *premier;
};

Liste *initialisationCallback()
{
    Liste *liste = malloc(sizeof(*liste));
    Element *element = malloc(sizeof(*element));
    
    if (liste == NULL || element == NULL)
    {
        exit(EXIT_FAILURE);
    }
    
    element->call = NULL;
    element->arglist = NULL;
    liste->premier = NULL;
    
    return liste;
}


void insertion(Liste *liste, PyObject *nvcall, PyObject *nvarglist)
{
    /* Création du nouvel élément */
    Element *nouveau = malloc(sizeof(*nouveau));
    if (liste == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }
    nouveau->call = nvcall;
    nouveau->arglist = nvarglist;
    
    /* Insertion de l'élément au début de la liste */
    nouveau->suivant = liste->premier;
    liste->premier = nouveau;
}


Liste *callbackFreezeList;

// freezeCallback : execute the callback stacked with 
void freeze(bool isPaused, void *myData){
    Element * actuel = callbackFreezeList -> premier;
    while (actuel != NULL){
        // Lock the GIL in order to execute the callback saffely
        PyGILState_STATE d_gstate;
        d_gstate = PyGILState_Ensure();
        //Call the python callback actuel->call
        PyObject_CallObject(actuel->call, actuel->arglist);
        
        // Release the GIL
        PyGILState_Release(d_gstate);
        actuel = actuel -> suivant;
    }
    
}
 PyObject * igs_observeFreeze_wrapper(PyObject *self, PyObject *args)
{
    
    PyObject *temp;
    PyObject *temparglist;
    PyObject arg;
    
    //parse python argument into a callable object and some arguments
    if (PyArg_ParseTuple(args, "OO", &temp, &arg)) {
        if (!PyCallable_Check(temp)) { // check if the callback is a callable object
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    if (callbackFreezeList == NULL){
        callbackFreezeList = initialisationCallback();
    }
    
    Py_XINCREF(temp);         // Add a reference to new callback
    
    temparglist = Py_BuildValue("(O)", arg);

    Py_XINCREF(temparglist);         // Add a reference to new arglist
    
    insertion(callbackFreezeList, temp, temparglist);

    PyObject *result;
    int ret = igs_observeFreeze(freeze, NULL);

    result = PyLong_FromLong(ret);
    
    return result;
    
}

