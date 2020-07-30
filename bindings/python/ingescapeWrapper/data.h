//
//  data.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef data_h
#define data_h

#include <Python.h>

PyDoc_STRVAR(
             readInputAsDataDoc,
             "igs_readInputAsData(nameOfInput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfInput as Data \n"
             "return a Tuple contaning the data serialized and the size of the data byte");

PyDoc_STRVAR(
             readOutputAsDataDoc,
             "igs_readOutputAsData(nameOfOutput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfOutput as Data \n"
             "return a Tuple contaning the data serialized and the size of the data byte");

PyDoc_STRVAR(
             readParameterAsDataDoc,
             "igs_readParameterAsData(nameOfParameter)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfParameter as Data \n"
             "return a Tuple contaning the data serialized and the size of the data byte");


PyDoc_STRVAR(
             writeInputAsDataDoc,
             "igs_writeInputAsData(nameOfInput, dataSerialized, sizeOfDataSerialized)\n"
             "--\n"
             "\n"
             "write a value as data into an agent's input.\n" 
             "Return 0 if ok\n");

PyDoc_STRVAR(
             writeOutputAsDataDoc,
             "igs_writeOutputAsData(nameOfOuput, dataSerialized, sizeOfDataSerialized)\n"
             "--\n"
             "\n"
             "write a value as data into an agent's Ouput.\n"
             "Return 0 if ok\n"); 

PyDoc_STRVAR(
             writeParameterAsDataDoc,
             "igs_writeParameterAsData(nameOfParameter, dataSerialized, sizeOfDataSerialized)\n"
             "--\n"
             "\n"
             "write a value as data into an agent's Parameter.\n"
             "Return 0 if ok\n");

//igs_writeOutputAsData
 PyObject * writeOutputAsData_wrapper(PyObject * self, PyObject * args);

//igs_readOutputAsData
 PyObject * readOutputAsData_wrapper(PyObject * self, PyObject * args);

//igs_readInputAsData
 PyObject * readInputAsData_wrapper(PyObject * self, PyObject * args);

//igs_writeInputAsData
 PyObject * writeInputAsData_wrapper(PyObject * self, PyObject * args);

//igs_writeParameterAsData
 PyObject * writeParameterAsData_wrapper(PyObject * self, PyObject * args);

//igs_readParameterAsData
 PyObject * readParameterAsData_wrapper(PyObject * self, PyObject * args);

#endif /* data_h */
