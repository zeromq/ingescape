# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from ingescapePython import *
import time


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def norme(self):
        return (self.x ** 2 + self.y ** 2) ** 0.5

    def __repr__(self):
        return "Point : ({}, {})".format(self.x, self.y)

    def __mul__(self, value):
        return Point(self.x * value, self.y * value)


def call_int(msg):
    received = ingescape.receivAgent.inputs["inputInt"].read_iop()
    print(msg, received)


def call_data(msg):
    try:
        received = ingescape.receivAgent.inputs["inputData"].read_iop()
        print(msg, received)
    except Exception as e:
        print("error", e)


def calldouble(msg):
    received = ingescape.receivAgent.inputs["inputDouble"].read_iop()
    print(msg, received)


def callstr(msg):
    try:
        received = ingescape.receivAgent.inputs["inputString"].read_iop()
        print(msg, received)
    except Exception as e:
        print(e)


def callbol(msg):
    received = ingescape.receivAgent.inputs["inputBol"].read_iop()
    print(msg, received)


def callimp(msg):
    print(msg)


if __name__ == '__main__':

    msgData = "Data received"

    ingescape = IngeScape("ingescape")
    ingescape.receivAgent = Agent("receivAgent")

    ingescape.receivAgent.load_definition_from_path("./receiv_def.json")
    ingescape.receivAgent.load_mapping_from_path("./receiv_map.json")

    ingescape.receivAgent.inputs["inputInt"].observe_iop(call_int, "message received INTEGER ")
    ingescape.receivAgent.inputs["inputData"].observe_iop(call_data, "message received DATA ")
    ingescape.receivAgent.inputs["inputDouble"].observe_iop(calldouble, "message received DOUBLE :")
    ingescape.receivAgent.inputs["inputString"].observe_iop(callstr, "message received STRING : ")
    ingescape.receivAgent.inputs["inputImp"].observe_iop(callimp, "message received IMP ")
    ingescape.receivAgent.inputs["inputBol"].observe_iop(callbol, "message received BOOL : ")

    ingescape.receivAgent.start_with_device("en0", 5679)

    i = 0
    while True:
        time.sleep(5)
        if i > 500:
            break
        i += 1

    time.sleep(1)
    ingescape.receivAgent.stop()
