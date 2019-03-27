# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from enum import Enum
from ingescape import *
import sys
import pickle


class IopType(Enum):
    input = 1
    output = 2
    parameter = 3


class ValueType(Enum):
    INTEGER = 1
    DOUBLE = 2
    STRING = 3
    BOOL = 4
    IMPULSION = 5
    DATA = 6


"""Read Input per Type"""


def read_input_int(iop):
    received = igs_readInputAsInt(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_input_bool(iop):
    received = igs_readInputAsBool(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_input_double(iop):
    received = igs_readInputAsDouble(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_input_string(iop):
    received = igs_readInputAsString(iop.name).decode('utf-8')
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_input_data(iop):
    received = igs_readInputAsData(iop.name)
    iop.value = pickle.loads(received[0])
    iop.size = received[1]
    return iop.value


"""Read Output Per Type"""


def read_output_int(iop):
    received = igs_readOutputAsInt(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_output_bool(iop):
    received = igs_readOutputAsBool(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_output_double(iop):
    received = igs_readOutputAsDouble(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_output_string(iop):
    received = igs_readOutputAsString(iop.name).decode('utf-8')
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_output_data(iop):
    received = igs_readOutputAsData(iop.name)

    iop.value = received[0]
    iop.size = received[1]
    return iop.value


"""Read Parameter Per type"""


def read_parameter_int(iop):
    received = igs_readParameterAsInt(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_parameter_bool(iop):
    received = igs_readParameterAsBool(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_parameter_double(iop):
    received = igs_readParameterAsDouble(iop.name)
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_parameter_string(iop):
    received = igs_readParameterAsString(iop.name).decode('utf-8')
    iop.value = received
    iop.size = sys.getsizeof(received)
    return received


def read_parameter_data(iop):
    received = igs_readParameterAsData(iop.name)
    iop.value = pickle.loads(received[0])
    iop.size = received[1]
    return iop.value


dict_read_switch_input = {(IopType.input.name, ValueType.INTEGER.name): read_input_int,
                          (IopType.input.name, ValueType.BOOL.name): read_input_bool,
                          (IopType.input.name, ValueType.DOUBLE.name): read_input_double,
                          (IopType.input.name, ValueType.STRING.name): read_input_string,
                          (IopType.input.name, ValueType.DATA.name): read_input_data}
dict_read_switch_output = {(IopType.output.name, ValueType.INTEGER.name): read_output_int,
                           (IopType.output.name, ValueType.BOOL.name): read_output_bool,
                           (IopType.output.name, ValueType.DOUBLE.name): read_output_double,
                           (IopType.output.name, ValueType.STRING.name): read_output_string,
                           (IopType.output.name, ValueType.DATA.name): read_output_data}
dict_read_switch_parameter = {(IopType.parameter.name, ValueType.INTEGER.name): read_parameter_int,
                              (IopType.parameter.name, ValueType.BOOL.name): read_parameter_bool,
                              (IopType.parameter.name, ValueType.DOUBLE.name): read_parameter_double,
                              (IopType.parameter.name, ValueType.STRING.name): read_parameter_string,
                              (IopType.parameter.name, ValueType.DATA.name): read_parameter_data}

dict_read_switch = {}
dict_read_switch.update(dict_read_switch_input)
dict_read_switch.update(dict_read_switch_output)
dict_read_switch.update(dict_read_switch_parameter)


"""Write Input Per Type"""


def write_input_int(iop, value):
    iop.value = value
    return igs_writeInputAsInt(iop.name, value)
    
    
def write_input_bool(iop, value):
    iop.value = value
    return igs_writeInputAsBool(iop.name, value)
    
    
def write_input_double(iop, value):
    iop.value = value
    return igs_writeInputAsDouble(iop.name, value)

    
def write_input_string(iop, value):
    iop.value = value
    return igs_writeInputAsString(iop.name, value)


def write_input_impulsion(iop, value):
    return igs_writeInputAsImpulsion(iop.name)


def write_input_data(iop, value):
    if isinstance(value, bytes):
        size = sys.getsizeof(value) - 33
        iop.value = value
        iop.size = size
        return igs_writeInputAsData(iop.name, value, size)
    else:
        iop.value = value
        iop.size = sys.getsizeof(iop.value)
        message = pickle.dumps(value)
        size = sys.getsizeof(message) - 33
        return igs_writeInputAsData(iop.name, message, size)


"""Write Output Per Type"""


def write_output_int(iop, value):
    iop.value = value
    return igs_writeOutputAsInt(iop.name, value)


def write_output_bool(iop, value):
    iop.value = value
    return igs_writeOutputAsBool(iop.name, value)


def write_output_double(iop, value):
    iop.value = value
    return igs_writeOutputAsDouble(iop.name, value)


def write_output_string(iop, value):
    iop.value = value
    return igs_writeOutputAsString(iop.name, value)


def write_output_impulsion(iop, value):
    return igs_writeOutputAsImpulsion(iop.name)


def write_output_data(iop, value):
    if isinstance(value, bytes):
        size = sys.getsizeof(value) - 33
        iop.value = value
        iop.size = size
        return igs_writeOutputAsData(iop.name, value, size)
    else:
        iop.value = value
        iop.size = sys.getsizeof(iop.value)
        message = pickle.dumps(value)
        size = sys.getsizeof(message) - 33
        return igs_writeOutputAsData(iop.name, message, size)
    
    
"""Write Parameter Per Type"""


def write_parameter_int(iop, value):
    iop.value = value
    return igs_writeParameterAsInt(iop.name, value)


def write_parameter_bool(iop, value):
    iop.value = value
    return igs_writeParameterAsBool(iop.name, value)


def write_parameter_double(iop, value):
    iop.value = value
    return igs_writeParameterAsDouble(iop.name, value)


def write_parameter_string(iop, value):
    iop.value = value
    return igs_writeParameterAsString(iop.name, value)


def write_parameter_data(iop, value):
    if isinstance(value, bytes):
        size = sys.getsizeof(value) - 33
        iop.value = value
        iop.size = size
        return igs_writeInputAsData(iop.name, value, size)
    else:
        iop.value = value
        iop.size = sys.getsizeof(iop.value)
        message = pickle.dumps(value)
        size = sys.getsizeof(message) - 33
        return igs_writeInputAsData(iop.name, message, size)
    
    
dict_write_switch_input = {(IopType.input.name, ValueType.INTEGER.name): write_input_int,
                           (IopType.input.name, ValueType.BOOL.name): write_input_bool,
                           (IopType.input.name, ValueType.DOUBLE.name): write_input_double,
                           (IopType.input.name, ValueType.STRING.name): write_input_string,
                           (IopType.input.name, ValueType.DATA.name): write_input_data,
                           (IopType.input.name, ValueType.IMPULSION.name): write_input_impulsion}
dict_write_switch_output = {(IopType.output.name, ValueType.INTEGER.name): write_output_int,
                            (IopType.output.name, ValueType.BOOL.name): write_output_bool,
                            (IopType.output.name, ValueType.DOUBLE.name): write_output_double,
                            (IopType.output.name, ValueType.STRING.name): write_output_string,
                            (IopType.output.name, ValueType.DATA.name): write_output_data,
                            (IopType.output.name, ValueType.IMPULSION.name): write_output_impulsion}
dict_write_switch_parameter = {(IopType.parameter.name, ValueType.INTEGER.name): write_parameter_int,
                               (IopType.parameter.name, ValueType.BOOL.name): write_parameter_bool,
                               (IopType.parameter.name, ValueType.DOUBLE.name): write_parameter_double,
                               (IopType.parameter.name, ValueType.STRING.name): write_parameter_string,
                               (IopType.parameter.name, ValueType.DATA.name): write_parameter_data}


dict_write_switch = {}
dict_write_switch.update(dict_write_switch_input)
dict_write_switch.update(dict_write_switch_output)
dict_write_switch.update(dict_write_switch_parameter)
