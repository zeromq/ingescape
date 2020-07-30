//
//  init.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef init_h
#define init_h
#include <Python.h>



// wrapper for igs_setAgentname
 PyObject * setAgentName_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_getAgentname
 PyObject * getAgentName_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_setAgentState
 PyObject * setAgentState_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_getAgentState
 PyObject * getAgentState_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_mute
 PyObject * mute_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_unmute
 PyObject * unmute_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_ismute
 PyObject * ismuted_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_freeze
 PyObject * freeze_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_unfreeze
 PyObject * unfreeze_wrapper(PyObject * self, PyObject * args);

// wrapper for igs_isFrozen
 PyObject * isFrozen_wrapper(PyObject * self, PyObject * args);

PyDoc_STRVAR(
             setAgentNameDoc,
             "igs_setAgentName(agentName)\n"
             "--\n"
             "\n"
             "set the name of the agent");

PyDoc_STRVAR(
             getAgentNameDoc,
             "igs_getAgentName()\n"
             "--\n"
             "\n"
             "get the name of the agent");

PyDoc_STRVAR(
             setAgentStateDoc,
             "igs_setAgentState(agentState)\n"
             "--\n"
             "\n"
             "set the state of the agent");

PyDoc_STRVAR(
             getAgentStateDoc,
             "igs_getAgentState()\n"
             "--\n"
             "\n"
             "get the state of the agent");

PyDoc_STRVAR(
             muteDoc,
             "igs_mute()\n"
             "--\n"
             "\n"
             "mute the agent");

PyDoc_STRVAR(
             unmuteDoc,
             "igs_unmute()\n"
             "--\n"
             "\n"
             "unmute the agent");

PyDoc_STRVAR(
             ismutedDoc,
             "igs_isMuted()\n"
             "--\n"
             "\n"
             "return True if the agent is muted");

PyDoc_STRVAR(
             freezeDoc,
             "igs_freeze()\n"
             "--\n"
             "\n"
             "freeze the agent");

PyDoc_STRVAR(
             unfreezeDoc,
             "igs_unfreeze()\n"
             "--\n"
             "\n"
             "unfreeze the agent");

PyDoc_STRVAR(
             isfrozenDoc,
             "igs_isFrozen()\n"
             "--\n"
             "\n"
             "return True if the agent is frozen");

#endif /* init_h */
