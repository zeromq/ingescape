# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from ingescapePython.switch_iop import *
from enum import Enum


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


class Iop(object):
    """
    Class to create an Iop
    Ex : ingescape.agentPython.createInput add an Iop to ingescape.agentPython.inputs[]
    """
    def __init__(self, ioptype, name, typevalue, value, size):
        self.iopType = ioptype          # type of Iop Iop_Type.input/output/parameter
        self.name = name                # name of the Iop
        self.valueType = typevalue      # type of value ValueType.INTEGER/DOUBLE/.....
        self.value = value              # value of the Iop
        self.size = size                # size of the value of the Iop
        self.observed = False           # True if Iop is observed
        self.ismuted = False            # True if Iop is muted
        self.write_iop(self.value)      # initialize the Iop to value

    def read_iop(self):
        """
        Find the Agent's iop by name and return the value.
            return the data

            If iop.ValueType is Data:
                        You have to deserialize it.
                        From python to python : pickle.loads(myData)
                        From python to C : struct.unpack("type of data.args", data.args)

        ----------------------------------------------------------------------------------
            Example from C to Python :
            Send in C
            ------------
            typedef struct Point{
                int x;
                int y;
            };
            struct Point message;
            message.x = 0;
            message.y = 10;
            int size = sizeof(message);
            igs_writeOutputAsData("outputdata", message size);

            Receiv in Python
            ------------
            class Point:
                def__init__(self,x,y):
                    self.x, self.y = x, y

            received = ingescape.agent.inputs["inputData"].read_iop()
            unpacked = struct.unpack("ii", received)
            point = Point(unpacked[0], unpacked[1])

        """
        type_iop = (self.iopType.name, self.valueType.name)
        return dict_read_switch[type_iop](self)

    def write_iop(self, value=1):
        """
        Find the Agent's iop by name and write it.
                    return 1 if ok
                    0 else
                    If iop.ValueType is Data:
                        You have to serialize it.
                        From python to python : pickle.dumps(myData)
                        From python to C : struct.pack("type of data.args", data.args)
        ----------------------------------------------------------------------------------
            Example from python to C :
            Send in Python
            ------------

            class Point:
                def__init__(self,x,y):
                    self.x, self.y = x, y
            point = Point(0, 10)
            ingescape.agent.output["outputdata"].write_iop(struct.pack("ii", point.x, point.y)

            Receiv in C
            -------------
            typedef struct Point{
                int x;
                int y;
            };
            void *myData;
            int size;
            igs_readInputAsData("inputdata", myData, &size);
            struct Points received = *((struct Points *)myData);

        """
        type_iop = (self.iopType.name, self.valueType.name)
        return dict_write_switch[type_iop](self, value)

    def get_types(self):
        """
        this method returns the value type of the output (integer, bool, etc .)
                param name The string which contains the name of the output. Can't be NULL or empty.
                return The output value type.
                0 The parameter name cannot be NULL or empty.
                -1 The definition live is NULL.
                -2 The parameter named is not found
        """
        type_iop = None
        if self.iopType == IopType.input:
            type_iop = igs_getTypeForInput(self.name)
        elif self.iopType == IopType.output:
            type_iop = igs_getTypeForOutput(self.name)
        elif self.iopType == IopType.parameter:
            type_iop = igs_getTypeForParameter(self.name)
        self.valueType = type_iop
        return type_iop

    def mute_output(self):
        """
        mute an agent output
                param name is the name of the agent output
                return 1 if ok
                0 else
        """
        if self.iopType == IopType.output:
            self.ismuted = True
        return igs_muteOutput(self.name)

    def un_mute_output(self):
        """
        unmute an agent output
                param name is the name of the agent output
                return 1 if correct or 0
        """
        if self.iopType == IopType.output:
            self.ismuted = False
        return igs_unmuteOutput(self.name)

    def is_output_muted(self):
        """
        Give the state of an agent output (mute/unmute)
                param name is the name of the agent output
                return true if muted else false
        """
        return igs_isOutputMuted(self.name)

    def observe_iop(self, callback, args):
        """
        Observe a input and associate a callback to it.
                When the iop value will change the associated callback will be called.
                param callback is the callback function
                param args is a tuple containing the arg to callback
        """
        if self.iopType == IopType.input:
            recept = igs_observeInput(self.name, callback, args)
        elif self.iopType == IopType.output:
            recept = igs_observeOutput(self.name, callback, args)
        elif self.iopType == IopType.parameter:
            recept = igs_observeParameter(self.name, callback, args)
        else:
            recept = 0
        return recept


def get_type_value_for_int(typeiop):
    if typeiop == 1:
        return ValueType.INTEGER
    elif typeiop == 2:
        return ValueType.DOUBLE
    elif typeiop == 3:
        return ValueType.STRING
    elif typeiop == 4:
        return ValueType.BOOL
    elif typeiop == 5:
        return ValueType.IMPULSION
    elif typeiop == 6:
        return ValueType.DATA


type_value = {1: 0, 2: 0, 3: "", 4: False, 5: 0, 6: 0}
type_size = {1: sys.getsizeof(0), 2: sys.getsizeof(0), 3: sys.getsizeof(""), 4: sys.getsizeof(False), 5: sys.getsizeof(0), 6: sys.getsizeof(0)}
