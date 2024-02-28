import signal
import ingescape as igs
import sys
import time

port = 1995
agentName = "tester"
networkDevice = "en0"
verbose = False
is_interrupted = False

def signal_handler(signal_received, frame):
    global is_interrupted
    print("\n", signal.strsignal(signal_received), sep="")
    is_interrupted = True

signal.signal(signal.SIGINT, signal_handler)

print("[Global API] Testing!")

def observe_io(io_type, name, value_type, value, my_data):
    pass

def service_callback(sender_agent_name, sender_agent_uuid, service_name, arguments, token, my_data):
    pass

def testerServiceCallback(sender_agent_name, sender_agent_uuid, service_name, arguments, token, my_data):
    pass

def testerIOCallback(io_type, name, value_type, value, my_data):
    pass

print ("[Global API] Net ifaces and devices", end =" ")
print("igs.net_devices_list()")
print(f'{igs.net_devices_list()}')
print("igs.net_addresses_list()")
print(f'{igs.net_addresses_list()}')
assert len(igs.net_devices_list()) == len(igs.net_addresses_list())
print ("OK")

print ("[Global API] Testing agent name", end =" ")
assert igs.agent_name() == "no_name"
assert igs.definition_class() == ""
igs.agent_set_name("simple Demo Agent")
assert igs.agent_name() == "simple_Demo_Agent"
print("CLASS", igs.definition_class())
assert igs.definition_class() == igs.agent_name()
igs.agent_set_name("tester")
assert igs.agent_name() == "tester"
print("CLASS", igs.definition_class())
assert igs.definition_class() == igs.agent_name()
print ("OK")

print ("[Global API] Testing agent family and uuid", end =" ")
assert igs.agent_family() == ""
igs.agent_set_family("family_test")
assert igs.agent_family() == "family_test"
assert igs.agent_uuid() != ""
print ("OK")

print ("[Global API] Testing agent logs", end =" ")
assert not igs.log_console()
assert not igs.log_syslog()
assert not igs.log_console_color()
assert not igs.log_stream()
assert not igs.log_file()
assert igs.log_file_path() == ""
igs.log_set_console(True)
assert igs.log_console()
igs.log_set_syslog(True)
assert igs.log_syslog()
igs.log_set_stream(True)
assert igs.log_stream()
igs.log_set_file_path("/tmp/log.txt")
assert igs.log_file_path() == "/tmp/log.txt"
igs.log_set_file_path("")
assert igs.log_file_path() == ""
igs.log_set_console_level(igs.LOG_TRACE)
assert igs.log_console_level() == igs.LOG_TRACE
igs.log_include_services(True)
igs.log_no_warning_if_undefined_service(True)
print ("OK")
igs.log_set_console(False)
igs.log_set_syslog(False)


assert igs.version() > 0
assert igs.protocol() >= 2

print ("[Global API] Security")
print ("Test certificates are in the tests/security directory")
print ("Wrong path")
assert igs.enable_security("/does/not/exist", "/does/not/exist") == igs.FAILURE
print ("Wrong parameter types")
try:
    igs.enable_security(42, None) == igs.FAILURE
    assert False, "An exception must be thrown"
except:
    pass
print ("OK path")
assert igs.enable_security("./security/private.cert_secret", "./security") == igs.SUCCESS
print ("Disable security")
assert igs.disable_security() == igs.SUCCESS
print ("OK")

assert not igs.mapping_outputs_request()
igs.mapping_set_outputs_request(True)
assert igs.mapping_outputs_request()

assert not igs.is_started()
igs.agent_set_state("my state")
assert igs.agent_state() == "my state"

assert not igs.agent_is_muted()
igs.agent_mute()
assert igs.agent_is_muted()
igs.agent_unmute()
assert not igs.agent_is_muted()

assert not igs.is_frozen()
igs.freeze()
assert igs.is_frozen()
igs.unfreeze()
assert not igs.is_frozen()

print ("[Global API] Testing agent definition", end =" ")
assert igs.input_count() == 0
assert igs.output_count() == 0
assert igs.attribute_count() == 0
assert igs.parameter_count() == 0
assert not igs.input_exists("toto")
assert not igs.output_exists("toto")
assert not igs.attribute_exists("toto")
assert not igs.parameter_exists("toto")
assert len(igs.input_list()) == 0
assert len(igs.output_list()) == 0
assert len(igs.attribute_list()) == 0
assert len(igs.parameter_list()) == 0

assert not igs.output_is_muted("toto")

assert not igs.input_bool("toto")
assert igs.input_int("toto") == 0
assert igs.input_double("toto") == 0.0
assert not igs.input_string("toto") is None
assert igs.input_data("toto") is None

assert not igs.output_bool("toto")
assert igs.output_int("toto") == 0
assert igs.output_double("toto") == 0.0
assert not igs.output_string("toto") is None
assert igs.output_data("toto") is None

assert not igs.attribute_bool("toto")
assert igs.attribute_int("toto") == 0
assert igs.attribute_double("toto") == 0.0
assert not igs.attribute_string("toto") is None
assert igs.attribute_data("toto") is None

assert not igs.parameter_bool("toto")
assert igs.parameter_int("toto") == 0
assert igs.parameter_double("toto") == 0.0
assert not igs.parameter_string("toto") is None
assert igs.parameter_data("toto") is None

assert igs.definition_load_str("invalid json") == igs.FAILURE
assert igs.definition_load_file("/does not exist") == igs.FAILURE
assert len(igs.definition_json()) > 0

assert igs.agent_name() == "tester"
assert igs.definition_version() == ""
assert igs.definition_description() == ""
assert igs.definition_package() == ""

igs.definition_set_version("version")
assert igs.definition_version() == "version"
igs.definition_set_description("description")
assert igs.definition_description() == "description"
igs.definition_set_package("com.ingescape.python.test")
assert igs.definition_package() == "com.ingescape.python.test"
igs.definition_set_class("TestClass")
assert igs.definition_class() == "TestClass"

assert igs.input_create("toto", igs.BOOL_T, None) == igs.SUCCESS
igs.observe_input("toto", observe_io, None)
igs.input_set_description("toto", "This is toto's description")
#NOTE: No API available to access this description
igs.input_set_detailed_type("toto", "custom_type", "its specification")
#NOTE: No API available to access this detailed type
assert igs.input_count() == 1
assert igs.output_create("toto", igs.BOOL_T, None) == igs.SUCCESS
igs.output_set_description("toto", "This is toto's description")
#NOTE: No API available to access this description
igs.output_set_detailed_type("toto", "custom_type", "its specification")
#NOTE: No API available to access this detailed type
assert igs.output_count() == 1
assert igs.attribute_create("toto", igs.BOOL_T, None) == igs.SUCCESS
igs.attribute_set_description("toto", "This is toto's description")
#NOTE: No API available to access this description
igs.attribute_set_detailed_type("toto", "custom_type", "its specification")
#NOTE: No API available to access this detailed type
assert igs.attribute_count() == 1
igs.attribute_remove("toto")
assert igs.parameter_create("toto", igs.BOOL_T, None) == igs.SUCCESS
igs.parameter_set_description("toto", "This is toto's description")
#NOTE: No API available to access this description
#NOTE: No detailed type for parameters. Parameters were already obsolete when detailed type was introduced
assert igs.parameter_count() == 1

assert not igs.output_is_muted("toto")
igs.output_mute("toto")
assert igs.output_is_muted("toto")

assert igs.input_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert igs.input_count() == 2
assert igs.input_exists("toto2")
assert igs.input_type("toto2") == igs.STRING_T
igs.input_remove("toto2")
assert not igs.input_exists("toto2")
assert igs.input_count() == 1

assert igs.output_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert igs.output_count() == 2
assert igs.output_exists("toto2")
assert igs.output_type("toto2") == igs.STRING_T
igs.output_remove("toto2")
assert not igs.output_exists("toto2")
assert igs.output_count() == 1

assert igs.attribute_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert igs.attribute_count() == 2
assert igs.attribute_exists("toto2")
assert igs.attribute_type("toto2") == igs.STRING_T
igs.attribute_remove("toto2")
assert not igs.attribute_exists("toto2")
assert igs.attribute_count() == 1

assert igs.parameter_create("toto2", igs.STRING_T, None) == igs.SUCCESS
assert igs.parameter_count() == 2
assert igs.parameter_exists("toto2")
assert igs.parameter_type("toto2") == igs.STRING_T
igs.parameter_remove("toto2")
assert not igs.parameter_exists("toto2")
assert igs.parameter_count() == 1

igs.clear_definition()
assert igs.agent_name() == "tester"
assert igs.input_count() == 0
assert igs.output_count() == 0
assert igs.attribute_count() == 0
assert igs.parameter_count() == 0
assert not igs.input_exists("toto")
assert not igs.output_exists("toto")
assert not igs.attribute_exists("toto")
assert not igs.parameter_exists("toto")
assert len(igs.input_list()) == 0
assert len(igs.output_list()) == 0
assert len(igs.attribute_list()) == 0
assert len(igs.parameter_list()) == 0
print ("OK")

print ("[Global API] Testing agent mappings", end =" ")
assert igs.mapping_add("toto", "other_agent", "tata") > 0
map_id = igs.mapping_add("toto", "other_agent", "tata")
assert igs.mapping_add("toto", "other_agent", "tata") == map_id
assert igs.mapping_count() == 1
assert igs.mapping_remove_with_id(1234) == igs.FAILURE
assert igs.mapping_count() == 1
assert igs.mapping_remove_with_id(map_id) == igs.SUCCESS
assert igs.mapping_count() == 0
assert igs.mapping_add("toto", "other_agent", "tata") > 0
assert igs.mapping_count() == 1
assert igs.mapping_remove_with_name("toto", "other_agent", "tata") == igs.SUCCESS
assert igs.mapping_count() == 0
assert igs.mapping_remove_with_name("toto", "other_agent", "tata") == igs.FAILURE
assert igs.clear_mappings() == igs.SUCCESS
assert igs.mapping_count() == 0

assert igs.mapping_add("toto", "other_agent", "tata") > 0
assert igs.mapping_add("titi", "other_agent", "tata") > 0
assert igs.mapping_count() == 2
assert igs.clear_mappings() == igs.SUCCESS
assert igs.mapping_count() == 0

assert igs.mapping_add("toto", "other_agent", "tata") > 0
assert igs.mapping_add("titi", "other_agent", "tata") > 0
assert igs.clear_mappings_for_input("toto") == igs.SUCCESS
assert igs.mapping_count() == 1
assert igs.clear_mappings_for_input("titi") == igs.SUCCESS
assert igs.mapping_count() == 0
print ("OK")

print ("[Global API] Testing agent splits", end =" ")
assert igs.split_add("toto", "other_agent", "tata") > 0
map_id = igs.split_add("toto", "other_agent", "tata")
assert igs.split_add("toto", "other_agent", "tata") == map_id
assert igs.split_count() == 1
assert igs.split_remove_with_id(1234) == igs.FAILURE
assert igs.split_count() == 1
assert igs.split_remove_with_id(map_id) == igs.SUCCESS
assert igs.split_count() == 0
assert igs.split_add("toto", "other_agent", "tata") > 0
assert igs.split_count() == 1
assert igs.split_remove_with_name("toto", "other_agent", "tata") == igs.SUCCESS
assert igs.split_count() == 0
assert igs.split_remove_with_name("toto", "other_agent", "tata") == igs.FAILURE
print ("OK")

print ("[Global API] Testing agent services", end =" ")
assert igs.service_exists("toto") == False
assert igs.service_init("first_service", service_callback, None) == igs.SUCCESS
assert igs.service_exists("first_service") == True
assert igs.service_remove("first_service") == igs.SUCCESS
assert igs.service_exists("first_service") == False
assert igs.service_init("second_service", service_callback, None) == igs.SUCCESS
assert igs.service_init("third_service", service_callback, None) == igs.SUCCESS
assert igs.service_list() == ('second_service', 'third_service')
assert igs.service_count() == 2
assert igs.service_arg_add("second_service", "first_arg", igs.INTEGER_T) == igs.SUCCESS
assert igs.service_arg_exists("second_service", "first_arg") == True
assert igs.service_arg_remove("second_service", "first_arg") == igs.SUCCESS
assert igs.service_arg_add("second_service", "first_arg", igs.INTEGER_T) == igs.SUCCESS
assert igs.service_arg_exists("second_service", "first_arg") == True
assert igs.service_args_list("second_service") == (('first_arg', igs.INTEGER_T),)
assert igs.service_args_count("second_service") == 1
assert igs.service_arg_add("second_service", "second_arg", igs.DOUBLE_T) == igs.SUCCESS
assert igs.service_arg_exists("second_service", "second_arg") == True
assert igs.service_args_list("second_service") == (('first_arg', igs.INTEGER_T), ('second_arg', igs.DOUBLE_T))
assert igs.service_args_count("second_service") == 2
assert igs.service_arg_add("second_service", "third_arg", igs.STRING_T) == igs.SUCCESS
assert igs.service_arg_exists("second_service", "third_arg") == True
assert igs.service_args_list("second_service") == (('first_arg', igs.INTEGER_T), ('second_arg', igs.DOUBLE_T), ('third_arg', igs.STRING_T))
assert igs.service_args_count("second_service") == 3

assert not igs.service_has_replies("second_service")
assert igs.service_reply_names("second_service") == []
assert igs.service_reply_add("second_service", "reply_service") == igs.SUCCESS
assert igs.service_has_replies("second_service")
assert igs.service_reply_arg_add("second_service", "reply_service", "first_arg", igs.INTEGER_T) == igs.SUCCESS
assert igs.service_reply_arg_add("second_service", "reply_service", "second_arg", igs.STRING_T) == igs.SUCCESS
assert igs.service_reply_args_count("second_service", "reply_service") == 2
assert igs.service_reply_args_list("second_service", "reply_service") == (("first_arg", igs.INTEGER_T), ("second_arg", igs.STRING_T))
assert igs.service_reply_names("second_service") == ["reply_service"]
assert igs.service_reply_arg_remove("second_service", "reply_service", "second_arg") == igs.SUCCESS
assert igs.service_reply_arg_remove("second_service", "reply_service", "first_arg") == igs.SUCCESS
assert igs.service_reply_args_count("second_service", "reply_service") == 0
assert igs.service_reply_args_list("second_service", "reply_service") == ()
assert igs.service_reply_remove("second_service", "reply_service") == igs.SUCCESS
assert igs.service_reply_names("second_service") == []
print ("OK")

print ("[Global API] Testing channels", end =" ")
assert igs.peer_add_header("publisher", "toto") == igs.FAILURE
assert igs.peer_add_header("logger", "toto") == igs.FAILURE
assert igs.peer_add_header("pid", "toto") == igs.FAILURE
assert igs.peer_add_header("commandline", "toto") == igs.FAILURE
assert igs.peer_add_header("hostname", "toto") == igs.FAILURE
assert igs.peer_remove_header("bogus key") == igs.FAILURE
assert igs.peer_remove_header("publisher") == igs.FAILURE
assert igs.peer_remove_header("logger") == igs.FAILURE
assert igs.peer_remove_header("pid") == igs.FAILURE
assert igs.peer_remove_header("commandline") == igs.FAILURE
assert igs.peer_remove_header("hostname") == igs.FAILURE
assert igs.peer_add_header("new key", "toto") == igs.SUCCESS
assert igs.peer_remove_header("new key") == igs.SUCCESS
assert igs.peer_remove_header("new key") == igs.FAILURE
assert igs.channel_join("toto") == igs.FAILURE
igs.channel_leave("toto")
assert igs.peer_add_header("new key", "toto") == igs.SUCCESS
print ("OK")

igs.clear_definition()
igs.agent_set_name(agentName)
igs.definition_set_description("One example for each type of IO and call")
igs.definition_set_version("1.0")

print ("[Global API] Testing initial value of IO", end =" ")
igs.input_create("toto", igs.INTEGER_T, 10)
assert igs.input_int("toto") == 10
igs.input_remove("toto")
igs.input_create("toto", igs.DOUBLE_T, 10.5)
assert igs.input_double("toto") - 10.5 < 0.000001
igs.input_remove("toto")
igs.input_create("toto", igs.BOOL_T, True)
assert igs.input_bool("toto") == True
igs.input_remove("toto")
igs.input_create("toto", igs.STRING_T, "Hello world")
assert igs.input_string("toto") == "Hello world"
igs.input_remove("toto")
print ("OK")

igs.input_create("my_impulsion", igs.IMPULSION_T, None)
igs.input_create("my_bool", igs.BOOL_T, True)
igs.input_create("my_int", igs.INTEGER_T, 10)
igs.input_create("my_double", igs.DOUBLE_T, 10.5)
igs.input_create("my_string", igs.STRING_T, "Test_string")
igs.input_create("my_data", igs.DATA_T, None)
igs.input_create("my_impulsion_split", igs.IMPULSION_T, None)
igs.input_create("my_bool_split", igs.BOOL_T, True)
igs.input_create("my_int_split", igs.INTEGER_T, 10)
igs.input_create("my_double_split", igs.DOUBLE_T, 10.5)
igs.input_create("my_string_split", igs.STRING_T, "Test_string")
igs.input_create("my_data_split", igs.DATA_T, None)
igs.output_create("my_impulsion", igs.IMPULSION_T, None)
igs.output_create("my_bool", igs.BOOL_T, True)
igs.output_create("my_int", igs.INTEGER_T, 10)
igs.output_create("my_double", igs.DOUBLE_T, 10.5)
igs.output_create("my_string", igs.STRING_T, "Test_string")
igs.output_create("my_data", igs.DATA_T, None)
igs.attribute_create("my_impulsion", igs.IMPULSION_T, None)
igs.attribute_create("my_bool", igs.BOOL_T, True)
igs.attribute_create("my_int", igs.INTEGER_T, 10)
igs.attribute_create("my_double", igs.DOUBLE_T, 10.5)
igs.attribute_create("my_string", igs.STRING_T, "Test_string")
igs.attribute_create("my_data", igs.DATA_T, None)
#Retrocompatibility test. No creation expected. Console output remaining that the function is deprecated and explaining the parameter is already existed and can't be created
igs.parameter_create("my_impulsion", igs.IMPULSION_T, None)
igs.parameter_create("my_bool", igs.BOOL_T, True)
igs.parameter_create("my_int", igs.INTEGER_T, 10)
igs.parameter_create("my_double", igs.DOUBLE_T, 10.5)
igs.parameter_create("my_string", igs.STRING_T, "Test_string")
igs.parameter_create("my_data", igs.DATA_T, None)
igs.service_init("myService", testerServiceCallback, None)
igs.service_arg_add("myService", "myBool", igs.BOOL_T)
igs.service_arg_add("myService", "myInt", igs.INTEGER_T)
igs.service_arg_add("myService", "myDouble", igs.DOUBLE_T)
igs.service_arg_add("myService", "myString", igs.STRING_T)
igs.service_arg_add("myService", "myData", igs.DATA_T)

igs.observe_input("my_impulsion", testerIOCallback, None)
igs.observe_input("my_bool", testerIOCallback, None)
igs.observe_input("my_int", testerIOCallback, None)
igs.observe_input("my_double", testerIOCallback, None)
igs.observe_input("my_string", testerIOCallback, None)
igs.observe_input("my_data", testerIOCallback, None)

igs.mapping_add("my_impulsion", "partner", "sparing_impulsion")
igs.mapping_add("my_bool", "partner", "sparing_bool")
igs.mapping_add("my_int", "partner", "sparing_int")
igs.mapping_add("my_double", "partner", "sparing_double")
igs.mapping_add("my_string", "partner", "sparing_string")
igs.mapping_add("my_data", "partner", "sparing_data")

igs.split_add("my_impulsion_split", "partner", "sparing_impulsion")
igs.split_add("my_bool_split", "partner", "sparing_bool")
igs.split_add("my_int_split", "partner", "sparing_int")
igs.split_add("my_double_split", "partner", "sparing_double")
igs.split_add("my_string_split", "partner", "sparing_string")
igs.split_add("my_data_split", "partner", "sparing_data")

print ("[Global API] Testing IO writing and type conversion", end =" ")
igs.input_set_impulsion("my_impulsion")
igs.input_set_impulsion("my_bool")
assert not igs.input_bool("my_bool")
igs.input_set_impulsion("my_int")
assert igs.input_int("my_int") == 0
igs.input_set_impulsion("my_double")
assert igs.input_double("my_double")  < 0.000001
igs.input_set_impulsion("my_string")
readResult = igs.input_string("my_string")
igs.input_set_impulsion("my_data")
data = igs.input_data("my_data")
assert(data is None)

igs.input_set_bool("my_impulsion", True)
igs.input_set_bool("my_bool", True)
assert igs.input_bool("my_bool")
igs.input_set_bool("my_int", True)
assert igs.input_int("my_int") == 1
igs.input_set_bool("my_double", True)
assert igs.input_double("my_double") - 1.0 < 0.000001
igs.input_set_bool("my_string", True)
readResult = igs.input_string("my_string")
assert readResult == "1"
igs.input_set_bool("my_data", True)
data = igs.input_data("my_data")
assert bool(data) == True

igs.input_set_int("my_impulsion", 3)
igs.input_set_int("my_bool", 3)
assert igs.input_int("my_bool")
igs.input_set_int("my_int", 3)
assert igs.input_int("my_int") == 3
igs.input_set_int("my_double", 1)
assert igs.input_double("my_double") - 3.0 < 0.000001
igs.input_set_int("my_string", 3)
readResult = igs.input_string("my_string")
assert readResult == "3"
igs.input_set_int("my_data", 3)
data = igs.input_data("my_data")
assert int.from_bytes(data, byteorder='little') == 3

igs.input_set_double("my_impulsion", 3.3)
igs.input_set_double("my_bool", 3.3)
assert igs.input_bool("my_bool")
igs.input_set_double("my_int", 3.3)
assert igs.input_int("my_int") == 3
igs.input_set_double("my_double", 3.3)
assert igs.input_double("my_double") - 3.3 < 0.000001
igs.input_set_double("my_string", 3.3)
readResult = igs.input_string("my_string")
assert float(readResult) -3.3 < 0.000001
igs.input_set_double("my_data", 3.3)
data = igs.input_data("my_data")
#FIXME: error on cast
#assert data == 3.3

igs.input_set_string("my_impulsion", "True")
igs.input_set_string("my_bool", "True")
assert igs.input_bool("my_bool")
igs.input_set_string("my_int", "3.3")
assert igs.input_int("my_int") == 3
igs.input_set_string("my_double", "3.3")
assert igs.input_double("my_double") - 3.3 < 0.000001
igs.input_set_string("my_string", "3.3")
readResult = igs.input_string("my_string")
assert readResult == "3.3"
igs.input_set_string("my_data", "3.3")
assert igs.input_string("my_data") == ""
data = igs.input_data("my_data")
assert data == None

data = b"My data"
data_size = sys.getsizeof(data) -33

igs.input_set_data("my_impulsion", data)
igs.input_set_data("my_bool", data)
igs.input_set_data("my_int", data)
igs.input_set_data("my_double", data)
igs.input_set_data("my_string", data)
igs.input_set_data("my_data", data)
data = igs.input_data("my_data")
assert data == b"My data"

igs.input_set_bool("my_bool", True)
assert igs.input_bool("my_bool")
assert igs.input_int("my_bool") == 1
assert igs.input_double("my_bool") - 1.0 < 0.000001
assert igs.input_string("my_bool") == "true"
igs.input_set_bool("my_bool", False)
assert not igs.input_bool("my_bool")
assert igs.input_int("my_bool") == 0
assert igs.input_double("my_bool") < 0.000001
assert igs.input_string("my_bool") == "false"
data_bool = bool(int.from_bytes(igs.input_data("my_bool"), byteorder='little'))
assert data_bool == False

igs.input_set_int("my_int", 3)
assert igs.input_bool("my_int")
assert igs.input_int("my_int") == 3
assert igs.input_double("my_int") - 3.0 < 0.000001
assert int(igs.input_string("my_int")) -3 ==0
igs.input_set_int("my_int", 0)
assert not igs.input_bool("my_int")
assert igs.input_int("my_int") == 0
assert igs.input_double("my_int") < 0.000001
assert int(igs.input_string("my_int")) ==0
data_int = int.from_bytes(igs.input_data("my_bool"), byteorder='little')
assert data_int == 0

igs.input_set_double("my_double", 3.3)
assert igs.input_bool("my_double")
assert igs.input_int("my_double") == 3
assert igs.input_double("my_double") - 3.3 < 0.000001
assert float(igs.input_string("my_double")) -3.3 < 0.000001
igs.input_set_double("my_double", 0.0)
assert not igs.input_bool("my_double")
assert igs.input_int("my_double") == 0
assert igs.input_double("my_double") < 0.000001
assert float(igs.input_string("my_double")) < 0.000001
data_int = int.from_bytes(igs.input_data("my_bool"), byteorder='little')
assert data_int == 0

igs.input_set_string("my_string", "true")
assert igs.input_bool("my_string")
igs.input_set_string("my_string", "false")
assert not igs.input_bool("my_string")
igs.input_set_string("my_string", "10.1")
assert igs.input_int("my_string") == 10
assert igs.input_double("my_string") - 10.1 < 0.000001
assert igs.input_string("my_string") == "10.1"
data, size = igs.input_data("my_string").decode('utf-8'), igs.input_data("my_string")[1]
assert float(data[:-1]) == 10.1

data = b"my data"
igs.input_set_data("my_data", bytes(data))
assert not igs.input_bool("my_data")
assert igs.input_int("my_data") == 0
assert igs.input_double("my_data") < 0.000001
assert igs.input_string("my_data") == ""
data = igs.input_data("my_data")
assert data.decode('utf-8') == "my data"
print ("OK")

igs.clear_context()

print("[Global API] Tests passed!")
