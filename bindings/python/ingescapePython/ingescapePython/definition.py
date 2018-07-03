# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from ingescape import *


class Definition(object):
    """
    Class for the definition of the Agent

    Ex : ingescape.agentPython.definition = Definition("nameOfDefinition)
    """
    def __init__(self, name):
        self.name = name                # name of the definition
        igs_setDefinitionName(name)    # initialize the definition
        self.description = None         # description of the definition
        self.version = None             # version of the definition

    def set_definition_description(self, description):
        """
        the agent definition description setter
                param description The string which contains the description of the agent. Can't be NULL.
                return The error.
                1 is OK,
                0 Agent description is NULL,
                -1 Agent description is empty
        """
        descript = igs_setDefinitionDescription(description)
        if descript == 1:
            self.description = descript
            return 1
        else:
            return descript

    def set_definition_version(self, version):
        """
        the agent definition version setter
                param version The string which contains the version of the agent. Can't be NULL.
                return The error.
                1 is OK,
                0 Agent version is NULL,
                -1 Agent version is empty
        """
        ret = igs_setDefinitionVersion(version)
        if ret == 1:
            self.description = ret
            return 1
        else:
            return ret

    @staticmethod
    def get_description_name():
        """
        the agent definition name getter
        """
        return igs_getDefinitionName().decode("utf8")

    @staticmethod
    def get_definition_description():
        """
        the agent definition description getter
        """
        return igs_getDefinitionDescription().decode("utf8")

    @staticmethod
    def get_description_version():
        """
        the agent definition version getter
        """
        return igs_getDefinitionVersion().decode("utf8")

    @staticmethod
    def get_definition():
        """
        the agent definition getter
                return The loaded definition string in json format (allocated).
                NULL if igs_definition_loaded was not initialized.
        """
        ret = igs_getDefinition()
        if ret is not None:
            return ret
        else:
            return None
