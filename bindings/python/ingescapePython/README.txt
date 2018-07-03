--ingescapePython -- python library for INGESCAPE--
==============================================

Multi Agent Supervision Transport Integration
and Control

Created by vaugien.
Copyright © 2018 ingenuity. All rights reserved.

This library is a python extension of the C
INGESCAPE library pythonising the ingeScape 
library 

-----------------INSTALLATION-----------------
==============================================

In a terminal :
    cd ingescapePython/ingescapepython/
    python3 setup.py install

-----------------DEPENDENCIES-----------------
==============================================

Dependencies :
	-Python.frameworks 3.6.4
	-libingescape.dylib
	-libzyre.dylib
	-libyajl.dylib
    	-libzmq.5.dylib
    	-libczmq.4.dylib
    
-------------------EXAMPLES-------------------
==============================================

In a python3 script:

from ingescapePython import *
import time
import struct


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def norme(self):
        return (self.x**2 + self.y**2)**0.5
    def __repr__(self):
        return "Point : ({}, {})".format(self.x, self.y)
    def __mul__(self, value):
        return Point(self.x*value, self.y*value)


def calldata(msg):
    received = igs_readOutputAsData("outputData")[0]
    print(msg, received)


def callint(msg):
    received = ingescape.sendAgent.outputs["outputInt"].read_iop()
    print(msg, received)


def calldouble(msg):
    received = ingescape.sendAgent.outputs["outputDouble"].read_iop()
    print(msg, received)


def callstr(msg):
    received = ingescape.sendAgent.outputs["outputString"].read_iop()
    print(msg, received)


def callbol(msg):
    received = ingescape.sendAgent.outputs["outputBol"].read_iop()
    print(msg, received)


def callimp(msg):
    print(msg)


point = Point(1, 2)

if __name__ == '__main__' :
    ingescape = IngeScape("ingescape")
    ingescape.sendAgent = Agent("sendAgent")
    ingescape.sendAgent.load_definition_from_path("./python_definition.json")
    ingescape.sendAgent.load_mapping_from_path("./python_mapping.json")
    ingescape.sendAgent.outputs["outputData"].observe_iop(calldata, "message sent DATA")
    ingescape.sendAgent.outputs["outputInt"].observe_iop(callint, "message sent INTEGER : ")
    ingescape.sendAgent.outputs["outputDouble"].observe_iop(calldouble, "message sent DOUBLE :")
    ingescape.sendAgent.outputs["outputString"].observe_iop(callstr, "message sent STRING : ")
    ingescape.sendAgent.outputs["outputImp"].observe_iop(callimp, "message sent IMP ")
    ingescape.sendAgent.outputs["outputBol"].observe_iop(callbol, "message sent BOOL : ")
    ingescape.sendAgent.start_with_device("en0", 5679)
    i = 2
    while True:
        time.sleep(0.5)
        if i % 5 == 0:
            ingescape.sendAgent.outputs["outputData"].write_iop(struct.pack("ii", 1, 1))
            ingescape.sendAgent.outputs["outputInt"].write_iop(i)
            ingescape.sendAgent.outputs["outputString"].write_iop(str(i) + " str")
            ingescape.sendAgent.outputs["outputBol"].write_iop(True if i % 2 == 0 else False)
            ingescape.sendAgent.outputs["outputDouble"].write_iop(float(i*2))
            ingescape.sendAgent.outputs["outputImp"].write_iop(1)
        elif i > 500:
            break
        i += 1
    ingescape.sendAgent.stop()