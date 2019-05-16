# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from ingescape import *


class Entry(object):
    """
    Class corresponding to an entry from mapping
    """
    def __init__(self, from_output, agent, to_input):
        self.fromOutput = from_output
        self.agent = agent
        self.input = to_input


class Mapping(object):

    def __init__(self, name):
        self.name = name
        igs_setMappingName(name)
        self.description = None
        self.version = None
        self.entry = {}

    @staticmethod
    def get_mapping_name():
        """
        Mapping Name Getter
        """
        return igs_getMappingName()


    def set_mapping_description(self,descr):
        """
        Mapping description setter
        """
        recept = igs_setMappingDescription(descr)
        if recept == 1:
            self.description = descr
        return recept

    @staticmethod
    def get_mapping_description():
        """
        Mapping description getter
        """
        return igs_getMappingDescription()

    def set_mapping_version(self, vers):
        """
        Mapping version setter
        """
        recept = igs_setMappingVersion(vers)
        if recept == 1:
            self.version = vers
        return recept

    @staticmethod
    def get_mapping_version():
        """
        Mapping version getter
        """
        return igs_getMappingVersion()
