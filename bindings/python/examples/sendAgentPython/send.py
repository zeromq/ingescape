# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from ingescapePython import *
import time
import pickle


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
    received = ingescape.sendAgent.outputs["outputData"].read_iop()
    print(msg, pickle.loads(received))


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


if __name__ == '__main__':
    ingescape = IngeScape("ingescape")
    ingescape.sendAgent = Agent("sendAgent")

    ingescape.sendAgent.load_definition_from_path("./send_def.json")
    ingescape.sendAgent.load_mapping_from_path("./send_map.json")

    ingescape.sendAgent.outputs["outputData"].observe_iop(calldata, "message sent DATA :")
    ingescape.sendAgent.outputs["outputInt"].observe_iop(callint, "message sent INTEGER : ")
    ingescape.sendAgent.outputs["outputDouble"].observe_iop(calldouble, "message sent DOUBLE :")
    ingescape.sendAgent.outputs["outputString"].observe_iop(callstr, "message sent STRING : ")
    ingescape.sendAgent.outputs["outputImp"].observe_iop(callimp, "message sent IMP ")
    ingescape.sendAgent.outputs["outputBol"].observe_iop(callbol, "message sent BOOL : ")

    ingescape.sendAgent.start_with_device("en0", 5679)
    i = 2

    while True:
        try:
            time.sleep(1)
            if i % 5 == 0:
                ingescape.sendAgent.outputs["outputInt"].write_iop(i)
                ingescape.sendAgent.outputs["outputString"].write_iop(str(i) + " str")
                ingescape.sendAgent.outputs["outputBol"].write_iop(True if i % 2 == 0 else False)
                ingescape.sendAgent.outputs["outputDouble"].write_iop(float(i*2))
                ingescape.sendAgent.outputs["outputImp"].write_iop()
                ingescape.sendAgent.outputs["outputData"].write_iop(pickle.dumps(point*i))
                pass
            elif i > 1000:
                break
            i += 1
        except KeyboardInterrupt:
            break

    time.sleep(1)
    ingescape.sendAgent.stop()
    sys.exit()
