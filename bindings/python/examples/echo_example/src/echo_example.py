#!/usr/bin/env -P /usr/bin:/usr/local/bin python3 -B
# coding: utf-8

# =========================================================================
# echo_example.py
#
# Copyright (c) the Contributors as noted in the AUTHORS file.
# This file is part of Ingescape, see https://github.com/zeromq/ingescape.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# =========================================================================


import ingescape as igs
import sys


class Singleton(type):
    _instances = {}
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


class EchoExample(metaclass=Singleton):
    def __init__(self):
        # inputs
        self.boolI = None
        self.integerI = None
        self.doubleI = None
        self.stringI = None
        self.dataI = None

        # outputs
        self._boolO = True
        self._integerO = 0
        self._doubleO = 0
        self._stringO = ""
        self._dataO = b'\x00'

    # outputs
    @staticmethod
    def set_impulsionO(self):
        igs.output_set_impulsion("impulsion")

    @property
    def boolO(self):
        return self._boolO

    @boolO.setter
    def boolO(self, value):
        self._boolO = value
        if self._boolO is not None:
            igs.output_set_bool("bool", self._boolO)
    @property
    def integerO(self):
        return self._integerO

    @integerO.setter
    def integerO(self, value):
        self._integerO = value
        if self._integerO is not None:
            igs.output_set_int("integer", self._integerO)
    @property
    def doubleO(self):
        return self._doubleO

    @doubleO.setter
    def doubleO(self, value):
        self._doubleO = value
        if self._doubleO is not None:
            igs.output_set_double("double", self._doubleO)
    @property
    def stringO(self):
        return self._stringO

    @stringO.setter
    def stringO(self, value):
        self._stringO = value
        if self._stringO is not None:
            igs.output_set_string("string", self._stringO)
    @property
    def dataO(self):
        return self._dataO

    @dataO.setter
    def dataO(self, value):
        self._dataO = value
        if self._dataO is not None:
            igs.output_set_data("data", value)

    # services
    def receive_values(self, sender_agent_name, sender_agent_uuid, boolV, integer, double, string, data, token, my_data):
        igs.info(f"Service receive_values called by {sender_agent_name} ({sender_agent_uuid}) with argument_list {boolV, integer, double, string, data} and token '{token}''")

    def send_values(self, sender_agent_name, sender_agent_uuid, token, my_data):
        print(f"Service send_values called by {sender_agent_name} ({sender_agent_uuid}), token '{token}' sending values : {self.boolO, self.integerO, self.doubleO, self.stringO, self.dataO}")
        igs.info(sender_agent_uuid, "receive_values", (self.boolO, self.integerO, self.doubleO, self.stringO, self.dataO), token)


