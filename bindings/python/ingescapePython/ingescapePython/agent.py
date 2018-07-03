# -*- coding: utf-8 -*-
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from ingescapePython.mapping import *
from ingescapePython.definition import *
from ingescapePython.iop import *
from ingescape import *
import ingescapePython.iop


class Agent(object):
    """
    Class Agent to initialize an agent and deal with all it parameters. 
    """
    def __init__(self, name):
        self.name = name
        igs_setAgentName(name)     # initialize the agent name
        self.definition = None      # definition of the Agent. Must be a Definition object
        self.inputs = {}            # Dictionary of inputs : {"nameOfInput" : Iop(nameOfInput, input)}
        self.outputs = {}           # Dictionary of outputs : {"nameOfOutput" : Iop(nameOfOutput, output)}
        self.parameters = {}        # Dictionary of parameters : {"nameOfParameters" : Iop(nameOfParameters, parameter)}
        self.mapping = None         # Mapping of the Agent. Must be a Mapping Object
        self.forcedstop = False
        self.freeze_observed = False            # True if freeze is observed
        self.forced_stoped_observed = False     # True if forced_stoped is observed

    def get_agent_name(self):
        """
        Return the name of the agent
        """
        self.name = igs_getAgentName().decode("utf8")
        return self.name

    @staticmethod
    def set_agent_state(state):
        """
        Set the state of the agent. Param state is a String 
        """
        return igs_setAgentState(state)

    @staticmethod
    def get_agent_state():
        """
        Return a String containing the state of the agent
        """
        return igs_getAgentState().decode("utf8")

    @staticmethod
    def start_with_device(device, port):
        """
        Start an agent on a specific network device and network port.
            param networkDevice is the name of the network device (ex: eth0, ens2 ...)
            param port is the network port number used
            return 1 if ok, else 0. 
        """
        return igs_startWithDevice(device, port)

    @staticmethod
    def start_with_ip(ip, port):
        """
        Start an agent on a specific network IP and network port.
                param ipAddress is the ip address on network
                param port is the network port number used
                return 1 if ok, else 0.
        """
        return igs_startWithIP(ip, port)

    @staticmethod
    def stop():
        """
        Stop the network layer of an agent.
                return 1 if ok, else 0.
        """
        return igs_stop()

    @staticmethod
    def mute():
        """
        Method to mute the agent
                return 1 if ok else 0.
        """
        return igs_mute()

    @staticmethod
    def un_mute():
        """
        Method to unmute the agent
                return 1 if ok else 0.
        """
        return igs_unmute()

    @staticmethod
    def is_muted():
        """
        Function to know if the agent is muted
                return true if it is muted else false.
        """
        return igs_isMuted()

    @staticmethod
    def freeze():
        """
        Freeze agent. Execute the associated FreezeCallback to the agent.
                by default no callback is defined.
                return 1 if ok, else 0
        """
        return igs_freeze()

    @staticmethod
    def un_freeze():
        """
        Unfreeze agent. Execute the associated FreezeCallback to the agent.
                by default no callback is defined.
                return 1 if ok, else 0
        """
        return igs_unfreeze()

    @staticmethod
    def set_can_be_frozen(canbefrozen):
        """
        set or unset forzen mode on the agent.
                param canBeFrozen is a bool to set or unset the verbose mode
        """
        return igs_setCanBeFrozen(canbefrozen)

    @staticmethod
    def is_frozen():
        """
        Return the frozon state of a agent.
                return True if frozen else False.
        """
        return igs_isFrozen()

    def die(self):
        """
        Terminate the agent and trigger the forcedStopCallbacks
        """
        self.forcedstop = True
        return igs_die()

    @staticmethod
    def get_inputs_number():
        """
        This function return the number of inputs.
                return The number of inputs.
                -1 the definition live is None.
                If an error occurs a igs_debug will be set.
        """
        return igs_getInputsNumber()

    @staticmethod
    def get_outputs_number():
        """
        This function return the number of outputs.
                return The number of outputs.
                -1 the definition live is None.
                If an error occurs a igs_debug will be set.
        """
        return igs_getOutputsNumber()

    @staticmethod
    def get_parameters_number():
        """
        This function return the number of parameters.
                return The number of parameters.
                -1 the definition live is None.
                If an error occurs a igs_debug will be set.
        """
        return igs_getParametersNumber()

    def get_inputs_list(self):
        """
        This function return a two dimensions table to get the list of inputs.
                return list the list of input's name.
                If an error occurs a igs_debug will be set
        """
        number = self.get_inputs_number()
        return igs_getInputsList(number)

    def get_outputs_list(self):
        """
        This function return a two dimensions table to get the list of outputs.
                return list the list of output's name.
                If an error occurs a igs_debug will be set
        """
        number = self.get_outputs_number()
        return igs_getOutputsList(number)

    def get_parameters_list(self):
        """
        This function return a two dimensions table to get the list of parameters.
                return list the list of parameter's name.
                If an error occurs a igs_debug will be set
        """
        number = self.get_parameters_number()
        return igs_getParametersList(number)

    @staticmethod
    def check_input_existence(name):
        """
        This function returns the state of the input existence.
                param name The string which contains the name of the input. Can't be None or empty.
                return A boolean "true" if the input exist.
        """
        return igs_checkInputExistence(name)

    @staticmethod
    def check_output_existence(name):
        """
        This function returns the state of the output existence.
                param name The string which contains the name of the output. Can't be None or empty.
                return A boolean "true" if the output exist.
        """
        return igs_checkOutputExistence(name)

    @staticmethod
    def check_parameter_existence(name):
        """
        This function returns the state of the parameter existence.
                param name The string which contains the name of the parameter. Can't be None or empty.
                return A boolean "true" if the parameter exist.
        """
        return igs_checkParameterExistence(name)

    def remove_input(self, name):
        """
        Remove and free an input for the agent
                param name The name of the Iop
                return The error :
                    1 is OK,
                    0 Definition loaded is None,
                    -1 Definition live is None,
                    -2 An error occurs while finding the iop by name
        """
        remove = igs_removeInput(name)
        if remove == 1:
            try:
                del self.inputs[name]
            except KeyError:
                print("Input was not in table")
        return remove

    def remove_output(self, name):
        """
        Remove and free an output for the agent
                param name The name of the Iop
                return The error :
                    1 is OK,
                    0 Definition loaded is None,
                    -1 Definition live is None,
                    -2 An error occurs while finding the iop by name
        """
        remove = igs_removeOutput(name)
        if remove == 1:
            try:
                del self.outputs[name]
            except KeyError:
                print("Output was not in table")
        return remove

    def remove_parameter(self, name):
        """
        Remove and free an parameter for the agent
                param name The name of the Iop
                return The error :
                    1 is OK,
                    0 Definition loaded is None,
                    -1 Definition live is None,
                    -2 An error occurs while finding the iop by name
        """
        remove = igs_removeParameter(name)
        if remove == 1:
            try:
                del self.parameters[name]
            except KeyError:
                print("Parameter was not in table")
        return remove

    def createiop(self, name, iop_type, value_type, value, size):
        """
        Add an Iop to the object Agent
                param name The name of the Iop
                param iop_type the type of the Iop : Iop_Type.input/output/parameter
                param value_type the type of value : Value_Type.INTEGER/BOOL/DOUBLE/IMPULSION/STRING/DATA
                param value the value of the Iop
                param size the size of param value
        """
        if iop_type == IopType.input:
            self.inputs[name] = Iop(iop_type, name, value_type, value, size)
        if iop_type == IopType.output:
            self.outputs[name] = Iop(iop_type, name, value_type, value, size)
        if iop_type == IopType.parameter:
            self.parameters[name] = Iop(iop_type, name, value_type, value, size)

    def load_definition(self, jsr_str):
        """load definition in variable 'igs_definition_loaded' & copy in 'igs_internal_definition"
                from a json string and in the attribut Definition of the Agent Object
                param json_str String in json format. Can't be None.
                return The error.
                1 is OK,
                0 json string is None,
                -1 Definition file has not been loaded"""
        defin = igs_loadDefinition(jsr_str)
        if defin == 1:
            self.definition = Definition(igs_getDefinitionName().decode('utf-8'))
            self.definition.description = igs_getDefinitionDescription().decode('utf-8')
            self.definition.version = igs_getDefinitionVersion().decode('utf-8')
        else:
            pass
        listinputs = self.get_inputs_list()
        listoutputs = self.get_outputs_list()
        listparameters = self.get_parameters_list()
        for inp in listinputs:
            typeiop = igs_getTypeForInput(inp.decode('utf-8'))
            valuetype = ingescapePython.iop.get_type_value_for_int(typeiop)
            self.createiop(inp.decode('utf-8'), IopType.input, valuetype, type_value[typeiop], type_size[typeiop])
        for out in listoutputs:
            typeiop = igs_getTypeForOutput(out.decode('utf-8'))
            valuetype = ingescapePython.iop.get_type_value_for_int(typeiop)
            self.createiop(out.decode('utf-8'), IopType.output, valuetype, type_value[typeiop], type_size[typeiop])
        for param in listparameters:
            typeiop = igs_getTypeForParameter(param.decode('utf-8'))
            valuetype = ingescapePython.iop.get_type_value_for_int(typeiop)
            self.createiop(param.decode('utf-8'), IopType.parameter, valuetype, type_value[typeiop], type_size[typeiop])
        return defin

    def load_definition_from_path(self, path_str):
        """
        load definition in variable 'igs_definition_loaded' & copy in 'igs_internal_definition"
                from a file path and in the attribut Definition of the Agent Object
                param file_path The string which contains the json file path. Can't be None.
                return The error.
                1 is OK,
                0 json string is None,
                -1 Definition file has not been loaded
        """
        defin = igs_loadDefinitionFromPath(path_str)
        if defin == 1:
            self.definition = Definition(igs_getDefinitionName().decode('utf-8'))
            self.definition.description = igs_getDefinitionDescription().decode('utf-8')
            self.definition.version = igs_getDefinitionVersion().decode('utf-8')
        else:
            pass
        listinputs = self.get_inputs_list()
        listoutputs = self.get_outputs_list()
        listparameters = self.get_parameters_list()
        for inp in listinputs:
            typeiop = igs_getTypeForInput(inp.decode('utf-8'))
            valuetype = ingescapePython.iop.get_type_value_for_int(typeiop)
            self.createiop(inp.decode('utf-8'), IopType.input, valuetype, type_value[typeiop], type_size[typeiop])
        for out in listoutputs:
            typeiop = igs_getTypeForOutput(out.decode('utf-8'))
            valuetype = ingescapePython.iop.get_type_value_for_int(typeiop)
            self.createiop(out.decode('utf-8'), IopType.output, valuetype, type_value[typeiop], type_size[typeiop])
        for param in listparameters:
            typeiop = igs_getTypeForParameter(param.decode('utf-8'))
            valuetype = ingescapePython.iop.get_type_value_for_int(typeiop)
            self.createiop(param.decode('utf-8'), IopType.parameter, valuetype, type_value[typeiop], type_size[typeiop])
        return defin

    def load_mapping(self, json):
        """
        load mapping in variable 'igs_internal_mapping' from a json string
                param json_str String in json format. Can't be None.
                return The error.
                1 is OK,
                0 json string is None or empty,
                -1 Mapping has not been loaded
        """
        recept = igs_loadMapping(json)
        if recept == 1:
            self.mapping = Mapping(igs_getMappingName().decode('utf-8'))
            self.mapping.version = self.mapping.get_mapping_version().decode('utf-8')
            self.mapping.description = self.mapping.get_mapping_description().decode('utf-8')
        return recept

    def load_mapping_from_path(self, path):
        """
        Load mapping in variable 'igs_internal_mapping' from a file path
                param file_path The string which contains the json file path. Can't be None.
                return The error.
                1 is OK,
                0 file path is None or empty,
                -1 Definition file has not been loaded
        """
        recept = igs_loadMappingFromPath(path)
        if recept == 1:
            self.mapping = Mapping(igs_getMappingName().decode('utf-8'))
            self.mapping.version = self.mapping.get_mapping_version().decode('utf-8')
            self.mapping.description = self.mapping.get_mapping_description().decode('utf-8')
        return recept

    def clear_mapping(self):
        """
        Clear the variable 'igs_internal_mapping' and free all structures inside and itself
                return The error.
                1 is OK,
                0 file path is None or empty
        """
        clearing = igs_clearMapping()
        if clearing == 1:
            try:
                self.mapping = None
            except NameError or AttributeError:
                print("Mapping was not loaded")
        return clearing

    @staticmethod
    def get_mapping():
        """
        The agent mapping getter
                return The loaded mapping string in json format.
                None if igs_internal_mapping was not initialized.
        """
        recept = igs_getMapping()
        return recept

    def add_mapping_entry(self, from_output, agent, to_input):
        """"
        This function allows the user to add a new mapping entry dynamically
                param fromOurInput The string which contains the name of the input to be mapped. Can't be None.
                param toAgent The string which contains the name of the extern agent. Can't be None.
                param withOutput The string which contains the name of the output of the extern agent to be mapped. Can't be None.
                return The error.
                    1 is OK.
                    0 Our input name to be mapped cannot be None or empty.
                    -1 Agent name to be mapped cannot be None or empty.
                    -2 Extern agent output name to be mapped cannot be None or empty.
        """
        recept = igs_addMappingEntry(from_output, agent, to_input)
        if self.mapping is not None:
            if recept > 0:
                pass
                # map_entry = Entry(from_output, agent, to_input)
                # self.mapping[recept] = map_entry
        return recept

    def remove_mapping_entry_with_id(self, id_entry):
        """
        This function allows the user to remove a mapping in table by its id
                param id_entry The id of the mapping. Cannot be negative.
                return The error.
                    1 is OK.
                    0 The id of the mapping cannot be negative.
                    -1 The structure igs_internal_mapping is None.
                    -2 The structure mapping out is None.
        """
        recept = igs_removeMappingEntryWithId(id_entry)
        if recept == 1:
            try:
                del self.mapping.entry[id_entry]
            except KeyError:
                print("Mapping entry was not in table")

        return recept

    def remove_mapping_entry_with_name(self, from_output, agent, to_input):
        """
        This function allows the user to remove a mapping in table by the input name, the extern agent's name, the extern agent's output
                param fromOurInput The string which contains the name of the input mapped. Can't be NULL.
                param toAgent The string which contains the name of the extern agent. Can't be NULL.
                param withOutput The string which contains the name of the output mapped of the extern agent. Can't be NULL.
                return The error.
                    1 is OK.
                    0 Our input name to be mapped cannot be NULL or empty.
                    -1 Agent name to be mapped cannot be NULL or empty.
                    -2 Extern agent output name to be mapped cannot be NULL or empty.
                    -3 The structure igs_internal_mapping is NULL.
                    -4 The structure mapping out is NULL.
        """
        recept = igs_removeMappingEntryWithName(from_output, agent, to_input)
        if recept == 1:
            try:
                for i in self.mapping.entry:
                    if i.fromOutput == from_output and i.input == to_input and i.agent == agent:
                        del self.mapping.entry[i]
            except KeyError:
                print("Mapping entry was not in table")

        return recept

    def observe_freeze(self, callback, args):
        """
        Add a igs_freezeCallback on an agent.
                param callback is a function
                param args is a tuple containing the arguments.
                    return 1 if ok,
                    else 0.
        """
        recept = igs_observeFreeze(callback, args)
        if recept == 1:
            self.freeze_observed = True
        return recept

    def observe_forced_stop(self, callback, args):
        """
        Add a igs_forcedStoppedCallback on an agent.
                param callback is a function
                param args is a tuple containing the arguments.
                    return 1 if ok,
                    else 0.
        """
        recept = igs_observeForcedStop(callback, args)
        if recept == 1:
            self.forced_stoped_observed = True
        return recept

    def create_input(self, name, typevalue, value, size):
        """
        Add an input to the Object and to the agent
                param name The name of the Iop
                param value_type the type of value : Value_Type.INTEGER/BOOL/DOUBLE/IMPULSION/STRING/DATA
                param value the value of the Iop
                param size the size of param value
                return The error.
                1 is OK,
                0 not able to add in definition loaded,
                -1 not able to add in definition live
        """
        input_created = Iop(IopType.input, name, typevalue, value, size)
        recept = igs_createInput(name, typevalue.value, value, size)
        if recept == 1:
            self.inputs[name] = input_created
        return recept

    def create_output(self, name, typevalue, value, size):
        """
        Add an output to the Object and to the agent
                param name The name of the Iop
                param value_type the type of value : Value_Type.INTEGER/BOOL/DOUBLE/IMPULSION/STRING/DATA
                param value the value of the Iop
                param size the size of param value
                return The error.
                1 is OK,
                0 not able to add in definition loaded,
                -1 not able to add in definition live
        """
        output_created = Iop(IopType.output, name, typevalue, value, size)
        recept = igs_createOutput(name, typevalue.value, value, size)
        if recept == 1:
            self.outputs[name] = output_created
        return recept

    def create_parameter(self, name, typevalue, value, size):
        """
        Add a parameter to the Object and to the agent
                param name The name of the Iop
                param value_type the type of value : Value_Type.INTEGER/BOOL/DOUBLE/IMPULSION/STRING/DATA
                param value the value of the Iop
                param size the size of param value
                return The error.
                1 is OK,
                0 not able to add in definition loaded,
                -1 not able to add in definition live
        """
        parameter_created = Iop(IopType.parameter, name, typevalue, value, size)
        recept = igs_createParameter(name, typevalue.value, value, size)
        if recept == 1:
            self.parameters[name] = parameter_created
        return recept

    def clear_definition(self):
        """
        Clear the internal definition of the agent.
                Free all members of the structure igs_definition_loaded & igs_internal_definition.
                But the pointer of these structure is not free and stay allocated.
                return 1 if ok,
                else 0
        """
        self.definition = None
        return igs_clearDefinition()

    @staticmethod
    def set_request_outputs_from_mapped_agent(notify):
        return igs_setRequestOutputsFromMappedAgents(notify)

    @staticmethod
    def get_request_outputs_from_mapped_agent():
        return igs_getRequestOutputsFromMappedAgents()

    @staticmethod
    def set_publishing_port(port):
        """
        Set the publishing port of the agent
        """
        igs_setPublishingPort(port)

    @staticmethod
    def set_discovery_interval(interval):
        """
        Set the interval of discovery
        """
        igs_setDiscoveryInterval(interval)


class IngeScape(object):
    """
    General Class to initialize ingescape, and handle the admin function
    """

    def __init__(self, name):
        """
        Inititalize the ingescape instance. Param name is the name of the Object
                Ex : ingescape = IngeScape("ingescape")
        """
        self.name = name    # name of the ingescape instance
        igs_setVerbose(True)   # print to stdout the ingescape log
        igs_setUseColorVerbose(True)

    @staticmethod
    def set_command_line(line):
        """
        Command line for the agent can be passed here for
                inclusion in the agent's headers. If not set, header is initialized with exec path.
        """
        recept = igs_setCommandLine(line)
        return recept

    @staticmethod
    def set_verbose(verbose):
        """
        Log in console. Param verbose is a Boolean
                Ex : ingescape.set_verbose(True)
        """
        recept = igs_setVerbose(verbose)
        return recept

    @staticmethod
    def get_verbose():
        """
        Return true if logged in console.
                Ex : ingescape.get_verbose(True)
        """
        return igs_isVerbose()

    @staticmethod
    def set_log_stream(stream):
        """
        Log in socket. Param stream is a Boolean
                Ex : ingescape.set_log_stream(True)
        """
        recept = igs_setLogStream(stream)
        return recept

    @staticmethod
    def get_log_stream():
        """
        Return true if logged in socket.
                Ex : ingescape.get_log_stream()
        """
        return igs_getLogStream()

    @staticmethod
    def set_log_in_file(use_log_file):
        """
        Log in file. Param stream is a Boolean
                        Ex : ingescape.set_log_in_file(True)
        """
        recept = igs_setLogInFile(use_log_file)
        return recept

    @staticmethod
    def get_log_in_file():
        """
        return True if logged in file.
                        Ex : ingescape.get_log_in_file(True)
        """
        return igs_getLogInFile()

    @staticmethod
    def set_use_color_verbose(use_color_verbose):
        """
        Log in console with color. Param verbose is a Boolean
               Ex : ingescape.set_verbose(True)
        """
        recept = igs_setUseColorVerbose(use_color_verbose)
        return recept

    @staticmethod
    def get_use_color_verbose():
        """
        Return True if logged in console with color.
               Ex : ingescape.get_verbose(True)
        """
        return igs_getUseColorVerbose()

    @staticmethod
    def set_log_path(path):
        """
        Log in file. Param verbose is a String containing the path to the file you want to write in
                Ex : ingescape.set_log_path(path)
        """
        recept = igs_setLogPath(path)
        return recept

    @staticmethod
    def get_log_path():
        """
        Return the path to the file you write log in.
                Ex : ingescape.get_log_path()
        """
        return igs_getLogPath()

