#!/usr/bin/python3

import ingescape as igs
import sys
import time

AGENT_NAME = "a"

INPUT_NAME = "in"
INPUT_VALUE = "KWy0JvqKwD0poJn8kPI6ljE9b2fgFy7Y6tLquHaS"
INPUT_COUNT_ACTUAL = 0
INPUT_COUNT_EXPECTED = 1

OUTPUT_NAME = "out"
OUTPUT_VALUE = "sM884o5ccMgffrvW9oBIXpuOypUsmGa1mgE9y4Lp"
OUTPUT_COUNT_ACTUAL = 0
OUTPUT_COUNT_EXPECTED = 1

OUTPUT_SAME_NAME_AS_INPUT = "in"
OUTPUT_SAME_NAME_COUNT_ACTUAL = 0
OUTPUT_SAME_NAME_COUNT_EXPECTED = 1

PARAM_NAME = "param"
PARAM_VALUE = "hC9tiuYOwoVHtm7Diqx6RP38zCTcmBvKDMLlkOK0"
PARAM_COUNT_ACTUAL = 0
PARAM_COUNT_EXPECTED = 1

SERVICE_NAME = "serv"
SERVICE_PARAM1 = "param1"
SERVICE_PARAM1_TYPE = igs.STRING_T
SERVICE_PARAM2 = "param2"
SERVICE_PARAM2_TYPE = igs.INTEGER_T
SERVICE_CALL_COUNT_ACTUAL = 0
SERVICE_CALL_COUNT_EXPECTED = 1

AGENT_ACTIVATION_ACTUAL = 0
AGENT_ACTIVATION_EXPECTED = 1

AGENT_DEACTIVATION_ACTUAL = 0
AGENT_DEACTIVATION_EXPECTED = 1

AGENT_KNOWS_US_EVENT_ACTUAL = 0
AGENT_KNOWS_US_EVENT_EXPECTED = 2 # 'main' and 'a'

AGENT_MUTE_ACTUAL = 0
AGENT_MUTE_EXPECTED = 1
AGENT_MUTE_STATE = False

def observe_iop_agent(agent, iop_type, name, value_type, value, my_data):
    pass

def testerIOPCallback_agent(agent, iop_type, name, value_type, value, my_data):
    pass

def testerServiceCallback(agent, sender_agent_name, sender_agent_uuid, service_name, arguments, token, my_data):
    pass

def service_callback(agent, sender_agent_name, sender_agent_uuid, service_name, arguments, token, my_data):
    pass

def input_obs(agent, iopType, name, valueType, value, my_data):
    global INPUT_COUNT_ACTUAL
    assert agent is not None
    # assert agent.name() == AGENT_NAME
    assert INPUT_NAME == name
    assert INPUT_VALUE == value
    INPUT_COUNT_ACTUAL = INPUT_COUNT_ACTUAL + 1

def output_obs(agent, iopType, name, valueType, value, my_data):
    global OUTPUT_COUNT_ACTUAL
    assert agent is not None
    # assert agent.name() == AGENT_NAME
    assert OUTPUT_NAME == name
    assert OUTPUT_VALUE == value
    OUTPUT_COUNT_ACTUAL = OUTPUT_COUNT_ACTUAL + 1

def output_same_name_obs(agent, iopType, name, valueType, value, my_data):
    global OUTPUT_SAME_NAME_COUNT_ACTUAL
    assert agent is not None
    # assert agent.name() == AGENT_NAME
    assert OUTPUT_SAME_NAME_AS_INPUT == name
    assert OUTPUT_VALUE == value
    OUTPUT_SAME_NAME_COUNT_ACTUAL = OUTPUT_SAME_NAME_COUNT_ACTUAL + 1

def param_obs(agent, iopType, name, valueType, value, my_data):
    global PARAM_COUNT_ACTUAL
    assert agent is not None
    # assert agent.name() == AGENT_NAME
    assert PARAM_NAME == name
    assert PARAM_VALUE == value
    PARAM_COUNT_ACTUAL = PARAM_COUNT_ACTUAL + 1

def service_cb(agent, sender_name, sender_uuid, service_name, args, token, my_data):
    global SERVICE_CALL_COUNT_ACTUAL
    SERVICE_CALL_COUNT_ACTUAL = SERVICE_CALL_COUNT_ACTUAL + 1

def agent_cb(agent, activated, my_data):
    global AGENT_ACTIVATION_ACTUAL
    global AGENT_DEACTIVATION_ACTUAL
    if activated:
        AGENT_ACTIVATION_ACTUAL = AGENT_ACTIVATION_ACTUAL + 1
    else:
        AGENT_DEACTIVATION_ACTUAL = AGENT_DEACTIVATION_ACTUAL + 1

def agent_event_cb(agent, event, uuid, name, event_data, my_data):
    global AGENT_KNOWS_US_EVENT_ACTUAL
    if event == igs.AGENT_KNOWS_US:
        assert name == AGENT_NAME or name == "main"
        AGENT_KNOWS_US_EVENT_ACTUAL = AGENT_KNOWS_US_EVENT_ACTUAL + 1

def mute_cb(agent, mute, my_data):
    global AGENT_MUTE_ACTUAL
    global AGENT_MUTE_STATE
    AGENT_MUTE_STATE = mute
    AGENT_MUTE_ACTUAL = AGENT_MUTE_ACTUAL + 1

print("[Agent API] Testing!")

igs.agent_set_name("main")
a = igs.Agent(AGENT_NAME, True)

print ("[Agent API] Testing observe input callbacks", end =" ")
a.input_create(INPUT_NAME, igs.STRING_T, None)
a.observe_input(INPUT_NAME, input_obs, None)
a.input_set_string(INPUT_NAME, INPUT_VALUE)
assert INPUT_COUNT_ACTUAL == INPUT_COUNT_EXPECTED
print ("OK")

print ("[Agent API] Testing observe output callbacks", end =" ")
a.output_create(OUTPUT_NAME, igs.STRING_T, None)
a.observe_output(OUTPUT_NAME, output_obs, None)
a.output_set_string(OUTPUT_NAME, OUTPUT_VALUE)
assert OUTPUT_COUNT_ACTUAL == OUTPUT_COUNT_EXPECTED

a.output_create(OUTPUT_SAME_NAME_AS_INPUT, igs.STRING_T, None)
a.observe_output(OUTPUT_SAME_NAME_AS_INPUT, output_same_name_obs, None)
a.output_set_string(OUTPUT_SAME_NAME_AS_INPUT, OUTPUT_VALUE)
assert INPUT_COUNT_ACTUAL == INPUT_COUNT_EXPECTED
assert OUTPUT_COUNT_ACTUAL == OUTPUT_COUNT_EXPECTED
assert OUTPUT_SAME_NAME_COUNT_ACTUAL == OUTPUT_SAME_NAME_COUNT_EXPECTED
print ("OK")

print ("[Agent API] Testing observe output callbacks", end =" ")
a.parameter_create(PARAM_NAME, igs.STRING_T, None)
a.observe_parameter(PARAM_NAME, param_obs, None)
a.parameter_set_string(PARAM_NAME, PARAM_VALUE)
assert PARAM_COUNT_ACTUAL == PARAM_COUNT_EXPECTED
print ("OK")

print ("[Agent API] Testing observe service calls", end =" ")
a.service_init(SERVICE_NAME, service_cb, None)
a.service_arg_add(SERVICE_NAME, SERVICE_PARAM1, SERVICE_PARAM1_TYPE)
a.service_arg_add(SERVICE_NAME, SERVICE_PARAM2, SERVICE_PARAM2_TYPE)
a.service_call(AGENT_NAME, SERVICE_NAME, ("coucou", 42), None)
assert SERVICE_CALL_COUNT_ACTUAL == SERVICE_CALL_COUNT_EXPECTED
print ("OK")

print ("[Agent API] Testing observe mute", end =" ")
a.observe_mute(mute_cb, None)
a.mute()
assert AGENT_MUTE_ACTUAL == AGENT_MUTE_EXPECTED
assert AGENT_MUTE_STATE
a.unmute()
assert not AGENT_MUTE_STATE
print ("OK")

print ("[Agent API] Testing observe agents (activation + events)")
b = igs.Agent("b", False)
b.observe(agent_cb, None)
b.observe_agent_event(agent_event_cb, None)

b.activate()
print ("[Agent API] - Activation", end=" ")
assert AGENT_ACTIVATION_ACTUAL == AGENT_ACTIVATION_EXPECTED
print ("OK")

print ("[Agent API] - Events", end=" ")
assert AGENT_KNOWS_US_EVENT_ACTUAL == AGENT_KNOWS_US_EVENT_EXPECTED
print ("OK")

print ("[Agent API] - Deactivation", end=" ")
b.deactivate()
assert AGENT_DEACTIVATION_ACTUAL == AGENT_DEACTIVATION_EXPECTED
print ("OK")

a = igs.Agent("a", False)
assert not a.is_activated()
a.activate()
assert a.is_activated()
a.deactivate()
assert not a.is_activated()
a.activate()

assert a.name() == "a"
a.set_name("agent")
assert a.name() == "agent"
a.set_family("agent_python")
assert a.family() == "agent_python"
a.set_family("new_family")
assert a.family() == "new_family"
assert a.state() == ""
a.set_state("Ok")
assert a.state() == "Ok"
assert a.is_muted() == False
a.mute()
assert a.is_muted() == True
a.unmute()
assert a.is_muted() == False

assert a.definition_load_str("invalid json") == igs.FAILURE
assert a.definition_load_file("/does not exist") == igs.FAILURE
assert len(igs.definition_json()) > 0
assert a.definition_version() == ""
a.definition_set_version("1.0")
assert a.definition_version() == "1.0"
assert a.definition_description() == ""
a.definition_set_description("description")
assert a.definition_description() == "description"

print ("[Agent API] Testing agent definition", end =" ")
assert a.input_create("toto", igs.BOOL_T, None) == igs.SUCCESS
a.observe_input("toto", observe_iop_agent, None)
assert a.input_count() == 1
assert a.output_create("toto", igs.BOOL_T, None) == igs.SUCCESS
assert a.output_count() == 1
assert a.parameter_create("toto", igs.BOOL_T, None) == igs.SUCCESS
assert a.parameter_count() == 1

assert not a.output_is_muted("toto")
a.output_mute("toto")
assert a.output_is_muted("toto")

assert a.input_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert a.input_count() == 2
assert a.input_exists("toto2")
assert a.input_type("toto2") == igs.STRING_T
assert a.input_list() == ["toto", "toto2"]
a.input_remove("toto2")
assert a.input_list() == ["toto"]
assert not a.input_exists("toto2")
assert a.input_count() == 1

assert a.output_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert a.output_count() == 2
assert a.output_exists("toto2")
assert a.output_type("toto2") == igs.STRING_T
assert a.output_list() == ["toto", "toto2"]
a.output_remove("toto2")
assert a.output_list() == ["toto"]
assert not a.output_exists("toto2")
assert a.output_count() == 1

assert a.parameter_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert a.parameter_count() == 2
assert a.parameter_exists("toto2")
assert a.parameter_type("toto2") == igs.STRING_T
assert a.parameter_list() == ["toto", "toto2"]
a.parameter_remove("toto2")
assert a.parameter_list() == ["toto"]
assert not a.parameter_exists("toto2")
assert a.parameter_count() == 1

a.clear_definition()
assert a.name() == "agent"
assert a.input_count() == 0
assert a.output_count() == 0
assert a.parameter_count() == 0
assert not a.input_exists("toto")
assert not a.output_exists("toto")
assert not a.parameter_exists("toto")
assert len(a.input_list()) == 0
assert len(a.output_list()) == 0
assert len(a.parameter_list()) == 0
print ("OK")

print ("[Agent API] Testing agent mappings", end =" ")
assert a.mapping_add("toto", "other_agent", "tata") > 0
map_id = a.mapping_add("toto", "other_agent", "tata")
assert a.mapping_add("toto", "other_agent", "tata") == map_id
assert a.mapping_count() == 1
assert a.mapping_remove_with_id(1234) == igs.FAILURE
assert a.mapping_count() == 1
assert a.mapping_remove_with_id(map_id) == igs.SUCCESS
assert a.mapping_count() == 0
assert a.mapping_add("toto", "other_agent", "tata") > 0
assert a.mapping_count() == 1
assert a.mapping_remove_with_name("toto", "other_agent", "tata") == igs.SUCCESS
assert a.mapping_count() == 0
assert a.mapping_remove_with_name("toto", "other_agent", "tata") == igs.FAILURE
print ("OK")

print ("[Agent API] Testing agent splits", end =" ")
assert a.split_add("toto", "other_agent", "tata") > 0
map_id = a.split_add("toto", "other_agent", "tata")
assert a.split_add("toto", "other_agent", "tata") == map_id
assert a.split_count() == 1
assert a.split_remove_with_id(1234) == igs.FAILURE
assert a.split_count() == 1
assert a.split_remove_with_id(map_id) == igs.SUCCESS
assert a.split_count() == 0
assert a.split_add("toto", "other_agent", "tata") > 0
assert a.split_count() == 1
assert a.split_remove_with_name("toto", "other_agent", "tata") == igs.SUCCESS
assert a.split_count() == 0
assert a.split_remove_with_name("toto", "other_agent", "tata") == igs.FAILURE
print ("OK")

print ("[Agent API] Testing agent services", end =" ")
assert a.service_exists("toto") == False
assert a.service_init("first_service", service_callback, None) == igs.SUCCESS
assert a.service_exists("first_service") == True
assert a.service_remove("first_service") == igs.SUCCESS
assert a.service_exists("first_service") == False
assert a.service_init("second_service", service_callback, None) == igs.SUCCESS
assert a.service_init("third_service", service_callback, None) == igs.SUCCESS
assert a.service_list() == ('second_service', 'third_service')
assert a.service_count() == 2
assert a.service_arg_add("second_service", "first_arg", igs.INTEGER_T) == igs.SUCCESS
assert a.service_arg_exists("second_service", "first_arg") == True
assert a.service_arg_remove("second_service", "first_arg") == igs.SUCCESS
assert a.service_arg_add("second_service", "first_arg", igs.INTEGER_T) == igs.SUCCESS
assert a.service_arg_exists("second_service", "first_arg") == True
assert a.service_args_list("second_service") == (('first_arg', igs.INTEGER_T),)
assert a.service_args_count("second_service") == 1
assert a.service_arg_add("second_service", "second_arg", igs.DOUBLE_T) == igs.SUCCESS
assert a.service_arg_exists("second_service", "second_arg") == True
assert a.service_args_list("second_service") == (('first_arg', igs.INTEGER_T), ('second_arg', igs.DOUBLE_T))
assert a.service_args_count("second_service") == 2
assert a.service_arg_add("second_service", "third_arg", igs.STRING_T) == igs.SUCCESS
assert a.service_arg_exists("second_service", "third_arg") == True
assert a.service_args_list("second_service") == (('first_arg', igs.INTEGER_T), ('second_arg', igs.DOUBLE_T), ('third_arg', igs.STRING_T))
assert a.service_args_count("second_service") == 3
print ("OK")

a.clear_definition()
a.set_name("a")
a.definition_set_description("One example for each type of IOP and call")
a.definition_set_version("1.0")

a.input_create("my_impulsion", igs.IMPULSION_T, None)
a.input_create("my_bool", igs.BOOL_T, True)
a.input_create("my_int", igs.INTEGER_T, 10)
a.input_create("my_double", igs.DOUBLE_T, 10.5)
a.input_create("my_string", igs.STRING_T, "Test_string")
a.input_create("my_data", igs.DATA_T, None)

a.input_create("my_impulsion_split", igs.IMPULSION_T, None)
a.input_create("my_bool_split", igs.BOOL_T, True)
a.input_create("my_int_split", igs.INTEGER_T, 10)
a.input_create("my_double_split", igs.DOUBLE_T, 10.5)
a.input_create("my_string_split", igs.STRING_T, "Test_string")
a.input_create("my_data_split", igs.DATA_T, None)

a.output_create("my_impulsion", igs.IMPULSION_T, None)
a.output_create("my_bool", igs.BOOL_T, True)
a.output_create("my_int", igs.INTEGER_T, 10)
a.output_create("my_double", igs.DOUBLE_T, 10.5)
a.output_create("my_string", igs.STRING_T, "Test_string")
a.output_create("my_data", igs.DATA_T, None)

a.parameter_create("my_impulsion", igs.IMPULSION_T, None)
a.parameter_create("my_bool", igs.BOOL_T, True)
a.parameter_create("my_int", igs.INTEGER_T, 10)
a.parameter_create("my_double", igs.DOUBLE_T, 10.5)
a.parameter_create("my_string", igs.STRING_T, "Test_string")
a.parameter_create("my_data", igs.DATA_T, None)

a.service_init("myService", testerServiceCallback, None)
a.service_arg_add("myService", "myBool", igs.BOOL_T)
a.service_arg_add("myService", "myInt", igs.INTEGER_T)
a.service_arg_add("myService", "myDouble", igs.DOUBLE_T)
a.service_arg_add("myService", "myString", igs.STRING_T)
a.service_arg_add("myService", "myData", igs.DATA_T)

a.observe_input("my_impulsion", testerIOPCallback_agent, None)
a.observe_input("my_bool", testerIOPCallback_agent, None)
a.observe_input("my_int", testerIOPCallback_agent, None)
a.observe_input("my_double", testerIOPCallback_agent, None)
a.observe_input("my_string", testerIOPCallback_agent, None)
a.observe_input("my_data", testerIOPCallback_agent, None)

a.mapping_add("my_impulsion", "partner", "sparing_impulsion")
a.mapping_add("my_bool", "partner", "sparing_bool")
a.mapping_add("my_int", "partner", "sparing_int")
a.mapping_add("my_double", "partner", "sparing_double")
a.mapping_add("my_string", "partner", "sparing_string")
a.mapping_add("my_data", "partner", "sparing_data")

a.split_add("my_impulsion_split", "partner", "sparing_impulsion")
a.split_add("my_bool_split", "partner", "sparing_bool")
a.split_add("my_int_split", "partner", "sparing_int")
a.split_add("my_double_split", "partner", "sparing_double")
a.split_add("my_string_split", "partner", "sparing_string")
a.split_add("my_data_split", "partner", "sparing_data")

print ("[Agent API] Testing IOP writeing and type conversion", end="")
a.input_set_impulsion("my_impulsion")
a.input_set_impulsion("my_bool")
assert not a.input_bool("my_bool")
a.input_set_impulsion("my_int")
assert a.input_int("my_int") == 0
a.input_set_impulsion("my_double")
assert a.input_double("my_double")  < 0.000001
a.input_set_impulsion("my_string")
readResult = a.input_string("my_string")
a.input_set_impulsion("my_data")
data = a.input_data("my_data")
assert(data is None)

a.input_set_bool("my_impulsion", True)
a.input_set_bool("my_bool", True)
assert a.input_bool("my_bool")
a.input_set_bool("my_int", True)
assert a.input_int("my_int") == 1
a.input_set_bool("my_double", True)
assert a.input_double("my_double") - 1.0 < 0.000001
a.input_set_bool("my_string", True)
readResult = a.input_string("my_string")
assert readResult == "1"
a.input_set_bool("my_data", True)
data = a.input_data("my_data")
assert bool(data) == True

a.input_set_int("my_impulsion", 3)
a.input_set_int("my_bool", 3)
assert a.input_int("my_bool")
a.input_set_int("my_int", 3)
assert a.input_int("my_int") == 3
a.input_set_int("my_double", 1)
assert a.input_double("my_double") - 3.0 < 0.000001
a.input_set_int("my_string", 3)
readResult = a.input_string("my_string")
assert readResult == "3"
a.input_set_int("my_data", 3)
data = a.input_data("my_data")
assert int.from_bytes(data, byteorder='little') == 3

a.input_set_double("my_impulsion", 3.3)
a.input_set_double("my_bool", 3.3)
assert a.input_bool("my_bool")
a.input_set_double("my_int", 3.3)
assert a.input_int("my_int") == 3
a.input_set_double("my_double", 3.3)
assert a.input_double("my_double") - 3.3 < 0.000001
a.input_set_double("my_string", 3.3)
readResult = a.input_string("my_string")
assert float(readResult) -3.3 < 0.000001
a.input_set_double("my_data", 3.3)
data = a.input_data("my_data")
#FIXME: error on cast
#assert data == 3.3 

a.input_set_string("my_impulsion", "True")
a.input_set_string("my_bool", "True")
assert a.input_bool("my_bool")
a.input_set_string("my_int", "3.3")
assert a.input_int("my_int") == 3
a.input_set_string("my_double", "3.3")
assert a.input_double("my_double") - 3.3 < 0.000001
a.input_set_string("my_string", "3.3")
readResult = a.input_string("my_string")
assert readResult == "3.3"

data = b"My data"

a.input_set_data("my_impulsion", data)
a.input_set_data("my_bool", data)
a.input_set_data("my_int", data)
a.input_set_data("my_double", data)
a.input_set_data("my_string", data)
a.input_set_data("my_data", data)
data = a.input_data("my_data")
assert data == b"My data"

a.input_set_bool("my_bool", True)
assert a.input_bool("my_bool")
assert a.input_int("my_bool") == 1
assert a.input_double("my_bool") - 1.0 < 0.000001
assert a.input_string("my_bool") == "true"
a.input_set_bool("my_bool", False)
assert not a.input_bool("my_bool")
assert a.input_int("my_bool") == 0
assert a.input_double("my_bool") < 0.000001
assert a.input_string("my_bool") == "false"
data_bool = bool(int.from_bytes(a.input_data("my_bool"), byteorder='little'))
assert data_bool == False

a.input_set_int("my_int", 3)
assert a.input_bool("my_int")
assert a.input_int("my_int") == 3
assert a.input_double("my_int") - 3.0 < 0.000001
assert int(a.input_string("my_int")) -3 ==0
a.input_set_int("my_int", 0)
assert not a.input_bool("my_int")
assert a.input_int("my_int") == 0
assert a.input_double("my_int") < 0.000001
assert int(a.input_string("my_int")) ==0
data_int = int.from_bytes(a.input_data("my_bool"), byteorder='little')
assert data_int == 0

a.input_set_double("my_double", 3.3)
assert a.input_bool("my_double")
assert a.input_int("my_double") == 3
assert a.input_double("my_double") - 3.3 < 0.000001
assert float(a.input_string("my_double")) -3.3 < 0.000001
a.input_set_double("my_double", 0.0)
assert not a.input_bool("my_double")
assert a.input_int("my_double") == 0
assert a.input_double("my_double") < 0.000001
assert float(a.input_string("my_double")) < 0.000001
data_int = int.from_bytes(a.input_data("my_bool"), byteorder='little')
assert data_int == 0

a.input_set_string("my_string", "true")
assert a.input_bool("my_string")
a.input_set_string("my_string", "false")
assert not a.input_bool("my_string")
a.input_set_string("my_string", "10.1")
assert a.input_int("my_string") == 10
assert a.input_double("my_string") - 10.1 < 0.000001
assert a.input_string("my_string") == "10.1"
data, size = a.input_data("my_string").decode('utf-8'), a.input_data("my_string")[1]
assert float(data[:-1]) == 10.1

data = b"my data"
a.input_set_data("my_data", bytes(data))
assert not a.input_bool("my_data")
assert a.input_int("my_data") == 0
assert a.input_double("my_data") < 0.000001
assert a.input_string("my_data") == None
data = a.input_data("my_data")
assert data == bytes(b"my data")
print ("OK")

print("[Agent API] Tests passed!")
