# =========================================================================
# ingescape - type stub for the Python binding (PEP 561)
#
# Copyright (c) the Contributors as noted in the AUTHORS file.
# This file is part of Ingescape, see https://github.com/zeromq/ingescape.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#=========================================================================
# This stub documents the public API of the compiled ``ingescape`` extension
# module: parameter types, return types and a description for every function,
# method and constant. Descriptions follow the C headers (ingescape.h and
# igsagent.h). Notes about C pointers that must be freed by the caller are
# omitted on purpose: memory management is fully handled inside the binding.
#
# Most functions wrap a C function returning ``igs_result_t``; in Python they
# return an ``int`` equal to ``SUCCESS`` (0) or ``FAILURE`` (-1). Getters return
# native Python types (bool, int, float, str, bytes, list, tuple).

from typing import Any, Callable, List, Optional, Tuple

# ---------------------------------------------------------------------------
# Result codes (igs_result_t)
SUCCESS: int  # operation succeeded (0)
FAILURE: int  # operation failed (-1)

# Agent/peer events (igs_agent_event_t), passed as first argument of an
# agent-event callback.
PEER_ENTERED: int              # a new peer has entered the network
PEER_EXITED: int               # a peer has left the network (clean exit or timeout)
AGENT_ENTERED: int             # an agent carried by a peer has entered the network
AGENT_UPDATED_DEFINITION: int  # an agent has updated its definition
AGENT_KNOWS_US: int            # an agent is ready to communicate with us (services callable)
AGENT_EXITED: int              # an agent has left the network
AGENT_UPDATED_MAPPING: int     # an agent has updated its mapping
AGENT_WON_ELECTION: int        # THIS agent has won an election
AGENT_LOST_ELECTION: int       # THIS agent has lost an election

# IO types (igs_io_type_t)
INPUT_T: int
OUTPUT_T: int
ATTRIBUTE_T: int

# IO value types (igs_io_value_type_t)
INTEGER_T: int
DOUBLE_T: int
STRING_T: int
BOOL_T: int
IMPULSION_T: int
DATA_T: int
UNKNOWN_T: int

# Log levels (igs_log_level_t)
LOG_TRACE: int
LOG_DEBUG: int
LOG_INFO: int
LOG_WARN: int
LOG_ERROR: int
LOG_FATAL: int

# Network monitor events (igs_monitor_event_t)
NETWORK_OK: int                       # the network is OK
NETWORK_DEVICE_NOT_AVAILABLE: int     # our network device is not available
NETWORK_ADDRESS_CHANGED: int          # the IP address of our network device changed
NETWORK_OK_AFTER_MANUAL_RESTART: int  # agent manually restarted and network is now OK

# ---------------------------------------------------------------------------
# Callback signatures
#
# The binding invokes the callable you register with the positional arguments
# described below. ``my_data`` is the opaque object you passed when registering.

# Observe an input/output/attribute/parameter change:
#   (io_type, name, value_type, value, my_data)
# value is bool | int | float | str | bytes | None depending on value_type.
IoObserveCallback = Callable[[int, str, int, Any, Any], None]

# Service implementation, invoked when our agent receives a service call:
#   (sender_agent_name, sender_agent_uuid, service_name, arguments, token, my_data)
# arguments is a tuple of the received argument values.
ServiceCallback = Callable[[str, str, str, tuple, Optional[str], Any], None]

# Mute state change: (is_muted, my_data)
MuteCallback = Callable[[bool, Any], None]

# Freeze state change: (is_frozen, my_data)
FreezeCallback = Callable[[bool, Any], None]

# Forced stop from the network: (my_data)
ForcedStopCallback = Callable[[Any], None]

# Agent/peer event: (event, uuid, name, event_data, my_data)
# event_data is dict (peer headers) | str (json/election name) | None depending on event.
AgentEventCallback = Callable[[int, str, str, Any, Any], None]

# Network monitor event: (event, device, ip_address, my_data)
MonitorCallback = Callable[[int, str, str, Any], None]

# Timer tick: (timer_id, my_data)
TimerCallback = Callable[[int, Any], None]


# ===========================================================================
# Module-level functions
# ===========================================================================

# --- Start & stop ----------------------------------------------------------
def start_with_device(network_device: str, port: int) -> int:
    """Start ingescape on a network device using UDP self-discovery on ``port``."""
    ...
def start_with_ip(ip_address: str, port: int) -> int:
    """Start ingescape on the network device bearing ``ip_address``, discovery on ``port``."""
    ...
def stop() -> int:
    """Stop ingescape. MUST NEVER be called from within an ingescape callback."""
    ...
def is_started() -> bool:
    """Return whether ingescape is currently started."""
    ...

# --- Agent name ------------------------------------------------------------
def agent_set_name(agent_name: str) -> int:
    """Set the agent name (usually the first function to call)."""
    ...
def agent_name() -> str:
    """Return the agent name."""
    ...
def agent_family() -> str:
    """Return the agent family. Default is an empty string."""
    ...
def agent_set_family(agent_family: str) -> int:
    """Set the agent family. Max length is 64 characters."""
    ...
def agent_uuid() -> str:
    """Return the agent UUID."""
    ...

# --- Agent state -----------------------------------------------------------
def agent_set_state(agent_state: str) -> int:
    """Set the agent free-text state string."""
    ...
def agent_state() -> str:
    """Return the agent free-text state string."""
    ...

# --- Mute ------------------------------------------------------------------
def agent_mute() -> int:
    """Mute the agent: it stops publishing on its outputs."""
    ...
def agent_unmute() -> int:
    """Unmute the agent."""
    ...
def agent_is_muted() -> bool:
    """Return whether the agent is muted."""
    ...

# --- Freeze ----------------------------------------------------------------
def freeze() -> int:
    """Freeze the agent. The internal semantics of freeze and
    unfreeze for a given agent are up to the developer and can
    be controlled using callbacks and bserve_freeze"""
    ...
def unfreeze() -> int:
    """Unfreeze the agent."""
    ...
def is_frozen() -> bool:
    """Return whether the agent is frozen."""
    ...

# --- Read inputs -----------------------------------------------------------
def input_bool(input_name: str) -> bool:
    """Return the current value of a BOOL input."""
    ...
def input_int(input_name: str) -> int:
    """Return the current value of an INTEGER input."""
    ...
def input_double(input_name: str) -> float:
    """Return the current value of a DOUBLE input."""
    ...
def input_string(input_name: str) -> str:
    """Return the current value of a STRING input."""
    ...
def input_data(input_name: str) -> bytes:
    """Return the current value of a DATA input."""
    ...

# --- Read outputs ----------------------------------------------------------
def output_bool(output_name: str) -> bool:
    """Return the current value of a BOOL output."""
    ...
def output_int(output_name: str) -> int:
    """Return the current value of an INTEGER output."""
    ...
def output_double(output_name: str) -> float:
    """Return the current value of a DOUBLE output."""
    ...
def output_string(output_name: str) -> str:
    """Return the current value of a STRING output."""
    ...
def output_data(output_name: str) -> bytes:
    """Return the current value of a DATA output."""
    ...

# --- Read attributes -------------------------------------------------------
def attribute_bool(attribute_name: str) -> bool:
    """Return the current value of a BOOL attribute."""
    ...
def attribute_int(attribute_name: str) -> int:
    """Return the current value of an INTEGER attribute."""
    ...
def attribute_double(attribute_name: str) -> float:
    """Return the current value of a DOUBLE attribute."""
    ...
def attribute_string(attribute_name: str) -> str:
    """Return the current value of a STRING attribute."""
    ...
def attribute_data(attribute_name: str) -> bytes:
    """Return the current value of a DATA attribute."""
    ...

# --- Read parameters (deprecated: renamed 'attributes') --------------------
def parameter_bool(parameter_name: str) -> bool:
    """Return the current value of a BOOL parameter (deprecated, use attributes)."""
    ...
def parameter_int(parameter_name: str) -> int:
    """Return the current value of an INTEGER parameter (deprecated, use attributes)."""
    ...
def parameter_double(parameter_name: str) -> float:
    """Return the current value of a DOUBLE parameter (deprecated, use attributes)."""
    ...
def parameter_string(parameter_name: str) -> str:
    """Return the current value of a STRING parameter (deprecated, use attributes)."""
    ...
def parameter_data(parameter_name: str) -> bytes:
    """Return the current value of a DATA parameter (deprecated, use attributes)."""
    ...

# --- Write inputs ----------------------------------------------------------
def input_set_bool(input_name: str, value: bool) -> int:
    """Write a BOOL value into an input."""
    ...
def input_set_int(input_name: str, value: int) -> int:
    """Write an INTEGER value into an input."""
    ...
def input_set_double(input_name: str, value: float) -> int:
    """Write a DOUBLE value into an input."""
    ...
def input_set_string(input_name: str, value: str) -> int:
    """Write a STRING value into an input."""
    ...
def input_set_impulsion(input_name: str) -> int:
    """Trigger an IMPULSION input."""
    ...
def input_set_data(input_name: str, value: bytes) -> int:
    """Write a DATA value into an input."""
    ...

# --- Write outputs ---------------------------------------------------------
def output_set_bool(output_name: str, value: bool) -> int:
    """Write a BOOL value into an output and publish it."""
    ...
def output_set_int(output_name: str, value: int) -> int:
    """Write an INTEGER value into an output and publish it."""
    ...
def output_set_double(output_name: str, value: float) -> int:
    """Write a DOUBLE value into an output and publish it."""
    ...
def output_set_string(output_name: str, value: str) -> int:
    """Write a STRING value into an output and publish it."""
    ...
def output_set_impulsion(output_name: str) -> int:
    """Publish an IMPULSION output."""
    ...
def output_set_data(output_name: str, value: bytes) -> int:
    """Write a DATA value into an output and publish it."""
    ...

# --- Write attributes ------------------------------------------------------
def attribute_set_bool(attribute_name: str, value: bool) -> int:
    """Write a BOOL value into an attribute."""
    ...
def attribute_set_int(attribute_name: str, value: int) -> int:
    """Write an INTEGER value into an attribute."""
    ...
def attribute_set_double(attribute_name: str, value: float) -> int:
    """Write a DOUBLE value into an attribute."""
    ...
def attribute_set_string(attribute_name: str, value: str) -> int:
    """Write a STRING value into an attribute."""
    ...
def attribute_set_data(attribute_name: str, value: bytes) -> int:
    """Write a DATA value into an attribute."""
    ...

# --- Write parameters (deprecated) -----------------------------------------
def parameter_set_bool(parameter_name: str, value: bool) -> int:
    """Write a BOOL value into a parameter (deprecated, use attributes)."""
    ...
def parameter_set_int(parameter_name: str, value: int) -> int:
    """Write an INTEGER value into a parameter (deprecated, use attributes)."""
    ...
def parameter_set_double(parameter_name: str, value: float) -> int:
    """Write a DOUBLE value into a parameter (deprecated, use attributes)."""
    ...
def parameter_set_string(parameter_name: str, value: str) -> int:
    """Write a STRING value into a parameter (deprecated, use attributes)."""
    ...
def parameter_set_data(parameter_name: str, value: bytes) -> int:
    """Write a DATA value into a parameter (deprecated, use attributes)."""
    ...

# --- IO value type ---------------------------------------------------------
def input_type(input_name: str) -> int:
    """Return the value type of an input."""
    ...
def output_type(output_name: str) -> int:
    """Return the value type of an output."""
    ...
def attribute_type(attribute_name: str) -> int:
    """Return the value type of an attribute."""
    ...
def parameter_type(parameter_name: str) -> int:
    """Return the value type of a parameter."""
    ...

# --- IO count --------------------------------------------------------------
def input_count() -> int:
    """Return the number of inputs."""
    ...
def output_count() -> int:
    """Return the number of outputs."""
    ...
def attribute_count() -> int:
    """Return the number of attributes."""
    ...
def parameter_count() -> int:
    """Return the number of parameters (deprecated, use attributes)."""
    ...

# --- IO existence ----------------------------------------------------------
def input_exists(input_name: str) -> bool:
    """Return whether an input with this name exists."""
    ...
def output_exists(output_name: str) -> bool:
    """Return whether an output with this name exists."""
    ...
def attribute_exists(attribute_name: str) -> bool:
    """Return whether an attribute with this name exists."""
    ...
def parameter_exists(parameter_name: str) -> bool:
    """Return whether a parameter with this name exists (deprecated)."""
    ...

# --- IO list ---------------------------------------------------------------
def input_list() -> List[str]:
    """Return the list of input names."""
    ...
def output_list() -> List[str]:
    """Return the list of output names."""
    ...
def attribute_list() -> List[str]:
    """Return the list of attribute names."""
    ...
def parameter_list() -> List[str]:
    """Return the list of parameter names (deprecated, use attributes)."""
    ...

# --- Mute outputs ----------------------------------------------------------
def output_mute(output_name: str) -> int:
    """Mute a single output."""
    ...
def output_unmute(output_name: str) -> int:
    """Unmute a single output."""
    ...
def output_is_muted(output_name: str) -> bool:
    """Return whether a single output is muted."""
    ...

# --- Definition ------------------------------------------------------------
def definition_load_str(json_str: str) -> int:
    """Load the agent definition from a JSON string."""
    ...
def definition_load_file(file_path: str) -> int:
    """Load the agent definition from a JSON file."""
    ...
def clear_definition() -> int:
    """Clear the agent definition."""
    ...
def definition_json() -> str:
    """Return the agent definition as a JSON string."""
    ...
def definition_set_class(class_name: str) -> int:
    """Set the agent class. Default is the agent name."""
    ...
def definition_class() -> str:
    """Return the agent class."""
    ...
def definition_set_package(package: str) -> int:
    """Set the agent package, usually a '::' separated hierarchy."""
    ...
def definition_package() -> str:
    """Return the agent package."""
    ...
def definition_set_description(description: str) -> int:
    """Set the agent free-text description."""
    ...
def definition_description() -> str:
    """Return the agent free-text description."""
    ...
def definition_set_version(version: str) -> int:
    """Set the agent version."""
    ...
def definition_version() -> str:
    """Return the agent version."""
    ...

# --- Remove IOs ------------------------------------------------------------
def input_remove(input_name: str) -> int:
    """Remove an input from the definition."""
    ...
def output_remove(output_name: str) -> int:
    """Remove an output from the definition."""
    ...
def attribute_remove(attribute_name: str) -> int:
    """Remove an attribute from the definition."""
    ...
def parameter_remove(parameter_name: str) -> int:
    """Remove a parameter from the definition (deprecated)."""
    ...

# --- Create IOs ------------------------------------------------------------
def input_create(input_name: str, value_type: int, value: Any) -> int:
    """Create an input with an initial value."""
    ...
def output_create(output_name: str, value_type: int, value: Any) -> int:
    """Create an output with an initial value."""
    ...
def attribute_create(attribute_name: str, value_type: int, value: Any) -> int:
    """Create an attribute with an initial value."""
    ...
def parameter_create(parameter_name: str, value_type: int, value: Any) -> int:
    """Create a parameter with an initial value (deprecated, use attributes)."""
    ...

# --- Clear IO value --------------------------------------------------------
def clear_input(input_name: str) -> int:
    """Clear the in-memory value of an input without writing an empty value."""
    ...
def clear_output(output_name: str) -> int:
    """Clear the in-memory value of an output without writing an empty value."""
    ...
def clear_attribute(attribute_name: str) -> int:
    """Clear the in-memory value of an attribute."""
    ...
def clear_parameter(parameter_name: str) -> int:
    """Clear the in-memory value of a parameter (deprecated)."""
    ...
def clear_context() -> int:
    """Clean the whole ingescape context. Not to be used in production."""
    ...

# --- Mapping ---------------------------------------------------------------
def mapping_load_str(json_str: str) -> int:
    """Load the agent mapping from a JSON string."""
    ...
def mapping_load_file(file_path: str) -> int:
    """Load the agent mapping from a JSON file."""
    ...
def clear_mappings() -> int:
    """Clear all mappings with all agents."""
    ...
def mapping_json() -> str:
    """Return the agent mapping as a JSON string."""
    ...
def mapping_count() -> int:
    """Return the number of entries in the mapping."""
    ...
def mapping_add(from_our_input: str, to_agent: str, with_output: str) -> int:
    """Map one of our inputs onto another agent's output. Returns the mapping id (0 on failure)."""
    ...
def mapping_remove_with_id(id: int) -> int:
    """Remove a mapping entry by its id."""
    ...
def mapping_remove_with_name(from_our_input: str, to_agent: str, with_output: str) -> int:
    """Remove a mapping entry by input/agent/output names."""
    ...
def clear_mappings_with_agent(agent_name: str) -> int:
    """Clear our mappings with a given agent."""
    ...
def clear_mappings_for_input(input_name: str) -> int:
    """Clear all mappings for one of our inputs."""
    ...
def mapping_set_outputs_request(notify: bool) -> int:
    """Ask mapped agents to send their current output values upon mapping. Default is disabled."""
    ...
def mapping_outputs_request() -> bool:
    """Return whether mapped agents are asked to send their current output values upon mapping"""
    ...

# --- Splits ----------------------------------------------------------------
def split_count() -> int:
    """Return the number of split entries."""
    ...
def split_add(from_our_input: str, to_agent: str, with_output: str) -> int:
    """Add a split entry. Returns the split id (0 on failure)."""
    ...
def split_remove_with_id(id: int) -> int:
    """Remove a split entry by its id."""
    ...
def split_remove_with_name(from_our_input: str, to_agent: str, with_output: str) -> int:
    """Remove a split entry by input/agent/output names."""
    ...

# --- Command line ----------------------------------------------------------
def set_command_line(command_line: str) -> int:
    """Set the agent command line included in its headers."""
    ...
def command_line() -> str:
    """Return the agent command line."""
    ...

# --- Observe IOs, freeze, forced stop, mute, agent events ------------------
def observe_input(input_name: str, callback: IoObserveCallback, my_data: Any) -> int:
    """Register a callback called whenever an input changes."""
    ...
def observe_output(output_name: str, callback: IoObserveCallback, my_data: Any) -> int:
    """Register a callback called whenever an output changes."""
    ...
def observe_attribute(attribute_name: str, callback: IoObserveCallback, my_data: Any) -> int:
    """Register a callback called whenever an attribute changes."""
    ...
def observe_parameter(parameter_name: str, callback: IoObserveCallback, my_data: Any) -> int:
    """Register a callback called whenever a parameter changes (deprecated)."""
    ...
def observe_freeze(callback: FreezeCallback, my_data: Any) -> int:
    """Register a callback called when the agent is frozen or unfrozen."""
    ...
def observe_forced_stop(callback: ForcedStopCallback, my_data: Any) -> int:
    """Register a callback called when ingescape is stopped from the network."""
    ...
def observe_mute(callback: MuteCallback, my_data: Any) -> int:
    """Register a callback called when the agent is muted or unmuted."""
    ...
def observe_agent_events(callback: AgentEventCallback, my_data: Any) -> int:
    """Register a callback called on agent/peer network events (see *_ENTERED/*_EXITED constants)."""
    ...

# --- Resource files --------------------------------------------------------
def definition_set_path(path: str) -> int:
    """Set the file path used by ``definition_save``."""
    ...
def mapping_set_path(path: str) -> int:
    """Set the file path used by ``mapping_save``."""
    ...
def definition_save() -> int:
    """Write the current definition to disk at the configured path."""
    ...
def mapping_save() -> int:
    """Write the current mapping to disk at the configured path."""
    ...
def version() -> int:
    """Return the agent version."""
    ...
def protocol() -> int:
    """Return the agent protocol."""
    ...

# --- Network configuration & channels --------------------------------------
def net_set_publishing_port(port: int) -> int:
    """Set the port used to publish outputs."""
    ...
def net_set_discovery_interval(interval: int) -> int:
    """Set the self-discovery interval, in milliseconds."""
    ...
def channel_join(channel: str) -> int:
    """Join a channel (Zyre group)."""
    ...
def channel_leave(channel: str) -> int:
    """Leave a channel (Zyre group)."""
    ...
def channel_shout_str(channel: str, msg: str) -> int:
    """Send a string message to all peers in a channel."""
    ...
def channel_shout_data(channel: str, data: bytes, size: int) -> int:
    """Send a binary message of ``size`` bytes to all peers in a channel."""
    ...
def channel_whisper_str(agent_name_or_agent_id_or_peer_id: str, msg: str) -> int:
    """Send a string message privately to an agent (by name, agent id or peer id)."""
    ...
def channel_whisper_data(agent_name_or_agent_id_or_peer_id: str, data: bytes, size: int) -> int:
    """Send a binary message of ``size`` bytes privately to an agent."""
    ...
def peer_add_header(key: str, value: str) -> int:
    """Add a Zyre header advertised by our peer."""
    ...
def peer_remove_header(key: str) -> int:
    """Remove a Zyre header advertised by our peer."""
    ...
def net_devices_list() -> List[str]:
    """Return the list of network devices with broadcast capabilities."""
    ...
def net_addresses_list() -> List[str]:
    """Return the list of IP addresses of broadcast-capable network devices."""
    ...

# --- Services --------------------------------------------------------------
def service_call(agent_name_or_uuid: str, service_name: str, tuple_arguments: Any,
                 token: Optional[str]) -> int:
    """Call a service hosted by another agent.

    :param agent_name_or_uuid: target agent name or UUID.
    :param service_name: name of the service to call.
    :param tuple_arguments: tuple/list of Python values matching the service arguments,
        or None for a service with no argument.
    :param token: optional routing token for replies (may be None).
    """
    ...
def service_init(service_name: str, callback: ServiceCallback, my_data: Any) -> int:
    """Create a service and attach its callback."""
    ...
def service_remove(service_name: str) -> int:
    """Remove a service."""
    ...
def service_arg_add(service_name: str, argument_name: str, argument_type: int) -> int:
    """Add an argument to a service."""
    ...
def service_arg_remove(service_name: str, arg_name: str) -> int:
    """Remove the first argument with its name from a service."""
    ...
def service_set_description(service_name: str, description: str) -> int:
    """Set the description of a service."""
    ...
def service_description(service_name: str) -> str:
    """Return the description of a service."""
    ...
def service_arg_set_description(service_name: str, arg_name: str, description: str) -> int:
    """Set the description of a service argument."""
    ...
def service_arg_description(service_name: str, arg_name: str) -> str:
    """Return the description of a service argument."""
    ...
def service_reply_set_description(service_name: str, reply_name: str, description: str) -> int:
    """Set the description of a service reply."""
    ...
def service_reply_description(service_name: str, reply_name: str) -> str:
    """Return the description of a service reply."""
    ...
def service_reply_arg_set_description(service_name: str, reply_name: str, arg_name: str,
                                      description: str) -> int:
    """Set the description of a service reply argument."""
    ...
def service_reply_arg_description(service_name: str, reply_name: str, arg_name: str) -> str:
    """Return the description of a service reply argument."""
    ...
def service_reply_add(service_name: str, reply_name: str) -> int:
    """Add an optional reply to a service."""
    ...
def service_reply_remove(service_name: str, reply_name: str) -> int:
    """Remove a reply from a service."""
    ...
def service_reply_arg_add(service_name: str, reply_name: str, arg_name: str, type: int) -> int:
    """Add an argument to a service reply."""
    ...
def service_reply_arg_remove(service_name: str, reply_name: str, arg_name: str) -> int:
    """Remove the first argument with this name from a service reply."""
    ...
def service_count() -> int:
    """Return the number of services of our agent."""
    ...
def service_exists(service_name: str) -> bool:
    """Return whether a service with this name exists."""
    ...
def service_list() -> Tuple[str, ...]:
    """Return the names of the services of our agent."""
    ...
def service_args_count(service_name: str) -> int:
    """Return the number of arguments of a service."""
    ...
def service_args_list(service_name: str) -> Tuple[Tuple[str, int], ...]:
    """Return the service arguments as a tuple of (name, value_type) pairs."""
    ...
def service_arg_exists(service_name: str, argument_name: str) -> bool:
    """Return whether a service has an argument with this name."""
    ...
def service_has_replies(service_name: str) -> bool:
    """Return whether a service has at least one reply."""
    ...
def service_has_reply(service_name: str, reply_name: str) -> bool:
    """Return whether a service has a reply with this name."""
    ...
def service_reply_names(service_name: str) -> List[str]:
    """Return the list of reply names of a service."""
    ...
def service_reply_args_count(service_name: str, reply_name: str) -> int:
    """Return the number of arguments of a service reply."""
    ...
def service_reply_args_list(service_name: str, reply_name: str) -> Tuple[Tuple[str, int], ...]:
    """Return the reply arguments as a tuple of (name, value_type) pairs."""
    ...
def service_reply_arg_exists(service_name: str, reply_name: str, argument_name: str) -> bool:
    """Return whether a service reply has an argument with this name."""
    ...

# --- Logs ------------------------------------------------------------------
def log_set_console(verbose: bool) -> int:
    """Enable or disable logs in the console (ERROR and FATAL are always shown)."""
    ...
def log_console() -> bool:
    """Return whether console logs are enabled."""
    ...
def log_set_syslog(verbose: bool) -> int:
    """Enable or disable system logs (UNIX only)."""
    ...
def log_syslog() -> bool:
    """Return whether system logs are enabled."""
    ...
def log_set_stream(verbose: bool) -> int:
    """Enable or disable logs stream."""
    ...
def log_stream() -> bool:
    """Return whether logs stream is enabled."""
    ...
def log_set_file(verbose: bool, file: Optional[str]) -> int:
    """Enable or disable logs in a file. If ``file`` is None, the default path is used."""
    ...
def log_file() -> bool:
    """Return whether file logs are enabled."""
    ...
def log_set_console_color(verbose: bool) -> int:
    """Enable or disable colors in console logs."""
    ...
def log_console_color() -> bool:
    """Return whether console log colors are enabled."""
    ...
def log_set_file_path(path: str) -> int:
    """Set the log file path."""
    ...
def log_file_path() -> str:
    """Return the log file path."""
    ...
def log_set_console_level(level: int) -> int:
    """Set the console log level (one of the LOG_* constants). Default is LOG_WARN."""
    ...
def log_console_level() -> int:
    """Return the console log level (one of the LOG_* constants)."""
    ...
def log_set_file_level(level: int) -> int:
    """Set the file log level (one of the LOG_* constants)."""
    ...
def log_set_file_max_line_length(size: int) -> int:
    """Set the maximum line length in log files."""
    ...
def log_include_data(enable: bool) -> int:
    """Log details of DATA IOs in log files. Default is False."""
    ...
def log_include_services(enable: bool) -> int:
    """Log details about service calls/executions in log files. Default is False."""
    ...
def log_no_warning_if_undefined_service(enable: bool) -> int:
    """Disable the warning emitted when an unknown service is called. Default is False."""
    ...
def trace(value: str) -> int:
    """Emit a TRACE-level log message."""
    ...
def debug(value: str) -> int:
    """Emit a DEBUG-level log message."""
    ...
def info(value: str) -> int:
    """Emit an INFO-level log message."""
    ...
def warn(value: str) -> int:
    """Emit a WARN-level log message."""
    ...
def error(value: str) -> int:
    """Emit an ERROR-level log message."""
    ...
def fatal(value: str) -> int:
    """Emit a FATAL-level log message."""
    ...

# --- Elections -------------------------------------------------------------
def election_join(election_name: str) -> int:
    """Join a named election between agents on separate peers."""
    ...
def election_leave(election_name: str) -> int:
    """Leave a named election."""
    ...

# --- Real-time -------------------------------------------------------------
def rt_get_current_timestamp() -> int:
    """In an input/service callback, return the timestamp (in microseconds) of the received info."""
    ...
def rt_set_timestamps(enable: bool) -> int:
    """Enable timestamps on published outputs and service calls."""
    ...
def rt_set_time(microseconds: int) -> int:
    """Manually set the current time (in microseconds) used for timestamps."""
    ...
def rt_time() -> int:
    """Return the current real-time clock value in microseconds."""
    ...
def rt_timestamps() -> bool:
    """Return whether timestamps are enabled."""
    ...
def rt_set_synchronous_mode(enable: bool) -> int:
    """Enable synchronous mode: outputs are published only when ``rt_set_time`` is called."""
    ...
def rt_synchronous_mode() -> bool:
    """Return whether synchronous mode is enabled."""
    ...

# --- Brokers ---------------------------------------------------------------
def broker_add(broker_endpoint: str) -> int:
    """Add a broker endpoint (tcp://ip:port) to connect to."""
    ...
def clear_brokers() -> int:
    """Remove all declared brokers."""
    ...
def broker_enable_with_endpoint(our_broker_endpoint: str) -> int:
    """Make our agent a broker reachable at the given endpoint when starting."""
    ...
def broker_set_advertized_endpoint(advertised_endpoint: str) -> int:
    """Set the endpoint advertised to brokers (e.g. a public/NAT address)."""
    ...
def start_with_brokers(agent_endpoint: str) -> int:
    """Start the agent using brokers; ``agent_endpoint`` is where our agent can be reached."""
    ...

# --- Security --------------------------------------------------------------
def enable_security(private_certificate_file: str, public_certificates_directory: str) -> int:
    """Enable end-to-end encryption and authentication."""
    ...
def disable_security() -> int:
    """Disable security."""
    ...
def broker_add_secure(broker_endpoint: str, path_to_public_certificate_for_broker: str) -> int:
    """Add a secure broker endpoint together with its public certificate path."""
    ...

# --- IPC -------------------------------------------------------------------
def set_ipc(allow: bool) -> int:
    """Allow or disallow inter-process communication transports. Default is True."""
    ...
def has_ipc() -> bool:
    """Return whether IPC transports are allowed."""
    ...
def set_ipc_dir(path: str) -> int:
    """Set the IPC folder path (UNIX only). Default is /tmp/ingescape/."""
    ...
def ipc_dir() -> str:
    """Return the IPC folder path (UNIX only)."""
    ...

# --- Network advanced ------------------------------------------------------
def net_set_log_stream_port(port: int) -> int:
    """Set the port used for the log socket stream."""
    ...
def net_set_timeout(duration: int) -> int:
    """Set the network timeout, in milliseconds."""
    ...
def net_raise_sockets_limit() -> int:
    """Raise the open sockets limit (UNIX only); call before any ingescape/ZeroMQ activity."""
    ...
def net_set_high_water_marks(hwm_value: int) -> int:
    """Set the high water marks for the publish/subscribe sockets (0 disables them)."""
    ...
def net_performance_check(peer_id: str, msg_size: int, msg_nbr: int) -> int:
    """Send ``msg_nbr`` messages of ``msg_size`` bytes to a peer and log performance results."""
    ...

# --- Monitor ---------------------------------------------------------------
def monitor_start(period: int) -> int:
    """Start the network monitor with a polling period in milliseconds."""
    ...
def monitor_start_with_network(period: int, network_device: str, port: int) -> int:
    """Start the network monitor bound to a specific device and port."""
    ...
def monitor_stop() -> int:
    """Stop the network monitor."""
    ...
def monitor_is_running() -> bool:
    """Return whether the network monitor is running."""
    ...
def monitor_set_start_stop(flag: bool) -> int:
    """When enabled, automatically restart/stop the agent on IP/device changes."""
    ...
def observe_monitor(callback: MonitorCallback, my_data: Any) -> int:
    """Register a callback called on network monitor events (see NETWORK_* constants)."""
    ...

# --- Constraints, descriptions and detailed types --------------------------
def constraints_enforce(enforce: bool) -> int:
    """Enable or disable enforcement of IO constraints. Default is disabled."""
    ...
def input_add_constraint(name: str, constraint: str) -> int:
    """Add a constraint to an input (e.g. 'min 0', 'max 10', '[0, 10]', '~ regex')."""
    ...
def output_add_constraint(name: str, constraint: str) -> int:
    """Add a constraint to an output."""
    ...
def attribute_add_constraint(name: str, constraint: str) -> int:
    """Add a constraint to an attribute."""
    ...
def parameter_add_constraint(name: str, constraint: str) -> int:
    """Add a constraint to a parameter (deprecated)."""
    ...
def input_set_description(name: str, description: str) -> int:
    """Set the description of an input."""
    ...
def input_description(name: str) -> str:
    """Return the description of an input."""
    ...
def output_set_description(name: str, description: str) -> int:
    """Set the description of an output."""
    ...
def output_description(name: str) -> str:
    """Return the description of an output."""
    ...
def attribute_description(name: str) -> str:
    """Return the description of an attribute."""
    ...
def attribute_set_description(name: str, description: str) -> int:
    """Set the description of an attribute."""
    ...
def parameter_set_description(name: str, description: str) -> int:
    """Set the description of a parameter (deprecated)."""
    ...
def input_set_detailed_type(name: str, type_name: str, specification: str) -> int:
    """Attach a descriptive detailed type and specification to an input."""
    ...
def output_set_detailed_type(name: str, type_name: str, specification: str) -> int:
    """Attach a descriptive detailed type and specification to an output."""
    ...
def attribute_set_detailed_type(name: str, type_name: str, specification: str) -> int:
    """Attach a descriptive detailed type and specification to an attribute."""
    ...

# --- Timers ----------------------------------------------------------------
def timer_start(delay: int, times: int, callback: TimerCallback, my_data: Any) -> int:
    """Start a timer calling ``callback`` ``times`` times every ``delay`` ms (0 = forever).

    Timers MUST be created after starting an agent. Returns the timer id, or -1 on error.
    """
    ...
def timer_stop(id: int) -> int:
    """Stop a timer by its id."""
    ...


# ===========================================================================
# Agent class (igsagent) - multiple agents in the same process
# ===========================================================================

class Agent:
    """An ingescape agent. Several Agent instances can coexist in one process.

    :param name: the agent name.
    :param activate_immediately: whether to activate the agent on creation.
    """

    def __init__(self, name: str, activate_immediately: bool = True) -> None: ...

    # --- Activation --------------------------------------------------------
    def activate(self) -> int:
        """Activate the agent on the network."""
        ...
    def deactivate(self) -> int:
        """Deactivate the agent."""
        ...
    def is_activated(self) -> bool:
        """Return whether the agent is activated."""
        ...
    def observe(self, callback: AgentEventCallback, my_data: Any) -> int:
        """Register a callback for this agent's network events (see *_ENTERED/*_EXITED constants)."""
        ...

    # --- Logging -----------------------------------------------------------
    def trace(self, msg: str) -> int:
        """Emit a TRACE-level log message attributed to this agent."""
        ...
    def debug(self, msg: str) -> int:
        """Emit a DEBUG-level log message attributed to this agent."""
        ...
    def info(self, msg: str) -> int:
        """Emit an INFO-level log message attributed to this agent."""
        ...
    def warn(self, msg: str) -> int:
        """Emit a WARN-level log message attributed to this agent."""
        ...
    def error(self, msg: str) -> int:
        """Emit an ERROR-level log message attributed to this agent."""
        ...
    def fatal(self, msg: str) -> int:
        """Emit a FATAL-level log message attributed to this agent."""
        ...

    # --- Identity & control ------------------------------------------------
    def name(self) -> str:
        """Return the agent name."""
        ...
    def set_name(self, name: str) -> int:
        """Set the agent name."""
        ...
    def family(self) -> str:
        """Return the agent family."""
        ...
    def set_family(self, family: str) -> int:
        """Set the agent family."""
        ...
    def uuid(self) -> str:
        """Return the agent UUID."""
        ...
    def state(self) -> str:
        """Return the agent free-text state string."""
        ...
    def set_state(self, state: str) -> int:
        """Set the agent free-text state string."""
        ...
    def mute(self) -> int:
        """Mute the agent."""
        ...
    def unmute(self) -> int:
        """Unmute the agent."""
        ...
    def is_muted(self) -> bool:
        """Return whether the agent is muted."""
        ...
    def observe_mute(self, callback: MuteCallback, my_data: Any) -> int:
        """Register a callback called when this agent is muted or unmuted."""
        ...
    def observe_agent_event(self, callback: AgentEventCallback, my_data: Any) -> int:
        """Register a callback called on this agent's network events."""
        ...

    # --- Definition --------------------------------------------------------
    def definition_load_str(self, definition_json: str) -> int:
        """Load this agent's definition from a JSON string."""
        ...
    def definition_load_file(self, path: str) -> int:
        """Load this agent's definition from a JSON file."""
        ...
    def clear_definition(self) -> int:
        """Clear this agent's definition."""
        ...
    def definition_json(self) -> str:
        """Return this agent's definition as a JSON string."""
        ...
    def definition_set_class(self, class_name: str) -> int:
        """Set this agent's class."""
        ...
    def definition_class(self) -> str:
        """Return this agent's class."""
        ...
    def definition_set_package(self, package: str) -> int:
        """Set this agent's package."""
        ...
    def definition_package(self) -> str:
        """Return this agent's package."""
        ...
    def definition_set_description(self, description: str) -> int:
        """Set this agent's free-text description."""
        ...
    def definition_description(self) -> str:
        """Return this agent's free-text description."""
        ...
    def definition_set_version(self, version: str) -> int:
        """Set this agent's version."""
        ...
    def definition_version(self) -> str:
        """Return this agent's version."""
        ...

    def input_create(self, name: str, value_type: int, value: Any) -> int:
        """Create an input with an initial value."""
        ...
    def output_create(self, name: str, value_type: int, value: Any) -> int:
        """Create an output with an initial value."""
        ...
    def attribute_create(self, name: str, value_type: int, value: Any) -> int:
        """Create an attribute with an initial value."""
        ...
    def parameter_create(self, name: str, value_type: int, value: Any) -> int:
        """Create a parameter with an initial value (deprecated)."""
        ...

    def input_remove(self, name: str) -> int:
        """Remove an input."""
        ...
    def output_remove(self, name: str) -> int:
        """Remove an output."""
        ...
    def attribute_remove(self, name: str) -> int:
        """Remove an attribute."""
        ...
    def parameter_remove(self, name: str) -> int:
        """Remove a parameter (deprecated)."""
        ...

    def input_type(self, name: str) -> int:
        """Return the value type of an input."""
        ...
    def output_type(self, name: str) -> int:
        """Return the value type of an output."""
        ...
    def attribute_type(self, name: str) -> int:
        """Return the value type of an attribute."""
        ...
    def parameter_type(self, name: str) -> int:
        """Return the value type of a parameter."""
        ...

    def input_count(self) -> int:
        """Return the number of inputs."""
        ...
    def output_count(self) -> int:
        """Return the number of outputs."""
        ...
    def attribute_count(self) -> int:
        """Return the number of attributes."""
        ...
    def parameter_count(self) -> int:
        """Return the number of parameters (deprecated)."""
        ...

    def input_list(self) -> List[str]:
        """Return the list of input names."""
        ...
    def output_list(self) -> List[str]:
        """Return the list of output names."""
        ...
    def attribute_list(self) -> List[str]:
        """Return the list of attribute names."""
        ...
    def parameter_list(self) -> List[str]:
        """Return the list of parameter names (deprecated)."""
        ...

    def input_exists(self, name: str) -> bool:
        """Return whether an input with this name exists."""
        ...
    def output_exists(self, name: str) -> bool:
        """Return whether an output with this name exists."""
        ...
    def attribute_exists(self, name: str) -> bool:
        """Return whether an attribute with this name exists."""
        ...
    def parameter_exists(self, name: str) -> bool:
        """Return whether a parameter with this name exists (deprecated)."""
        ...

    # --- Read/write IOs ----------------------------------------------------
    def input_bool(self, name: str) -> bool:
        """Return the current value of a BOOL input."""
        ...
    def input_int(self, name: str) -> int:
        """Return the current value of an INTEGER input."""
        ...
    def input_double(self, name: str) -> float:
        """Return the current value of a DOUBLE input."""
        ...
    def input_string(self, name: str) -> str:
        """Return the current value of a STRING input."""
        ...
    def input_data(self, name: str) -> bytes:
        """Return the current value of a DATA input."""
        ...

    def output_bool(self, name: str) -> bool:
        """Return the current value of a BOOL output."""
        ...
    def output_int(self, name: str) -> int:
        """Return the current value of an INTEGER output."""
        ...
    def output_double(self, name: str) -> float:
        """Return the current value of a DOUBLE output."""
        ...
    def output_string(self, name: str) -> str:
        """Return the current value of a STRING output."""
        ...
    def output_data(self, name: str) -> bytes:
        """Return the current value of a DATA output."""
        ...

    def attribute_bool(self, name: str) -> bool:
        """Return the current value of a BOOL attribute."""
        ...
    def attribute_int(self, name: str) -> int:
        """Return the current value of an INTEGER attribute."""
        ...
    def attribute_double(self, name: str) -> float:
        """Return the current value of a DOUBLE attribute."""
        ...
    def attribute_string(self, name: str) -> str:
        """Return the current value of a STRING attribute."""
        ...
    def attribute_data(self, name: str) -> bytes:
        """Return the current value of a DATA attribute."""
        ...

    def parameter_bool(self, name: str) -> bool:
        """Return the current value of a BOOL parameter (deprecated)."""
        ...
    def parameter_int(self, name: str) -> int:
        """Return the current value of an INTEGER parameter (deprecated)."""
        ...
    def parameter_double(self, name: str) -> float:
        """Return the current value of a DOUBLE parameter (deprecated)."""
        ...
    def parameter_string(self, name: str) -> str:
        """Return the current value of a STRING parameter (deprecated)."""
        ...
    def parameter_data(self, name: str) -> bytes:
        """Return the current value of a DATA parameter (deprecated)."""
        ...

    def input_set_bool(self, name: str, value: bool) -> int:
        """Write a BOOL value into an input."""
        ...
    def input_set_int(self, name: str, value: int) -> int:
        """Write an INTEGER value into an input."""
        ...
    def input_set_double(self, name: str, value: float) -> int:
        """Write a DOUBLE value into an input."""
        ...
    def input_set_string(self, name: str, value: str) -> int:
        """Write a STRING value into an input."""
        ...
    def input_set_impulsion(self, name: str) -> int:
        """Trigger an IMPULSION input."""
        ...
    def input_set_data(self, name: str, value: bytes) -> int:
        """Write a DATA value into an input."""
        ...

    def output_set_bool(self, name: str, value: bool) -> int:
        """Write a BOOL value into an output and publish it."""
        ...
    def output_set_int(self, name: str, value: int) -> int:
        """Write an INTEGER value into an output and publish it."""
        ...
    def output_set_double(self, name: str, value: float) -> int:
        """Write a DOUBLE value into an output and publish it."""
        ...
    def output_set_string(self, name: str, value: str) -> int:
        """Write a STRING value into an output and publish it."""
        ...
    def output_set_impulsion(self, name: str) -> int:
        """Publish an IMPULSION output."""
        ...
    def output_set_data(self, name: str, value: bytes) -> int:
        """Write a DATA value into an output and publish it."""
        ...

    def attribute_set_bool(self, name: str, value: bool) -> int:
        """Write a BOOL value into an attribute."""
        ...
    def attribute_set_int(self, name: str, value: int) -> int:
        """Write an INTEGER value into an attribute."""
        ...
    def attribute_set_double(self, name: str, value: float) -> int:
        """Write a DOUBLE value into an attribute."""
        ...
    def attribute_set_string(self, name: str, value: str) -> int:
        """Write a STRING value into an attribute."""
        ...
    def attribute_set_data(self, name: str, value: bytes) -> int:
        """Write a DATA value into an attribute."""
        ...

    def parameter_set_bool(self, name: str, value: bool) -> int:
        """Write a BOOL value into a parameter (deprecated)."""
        ...
    def parameter_set_int(self, name: str, value: int) -> int:
        """Write an INTEGER value into a parameter (deprecated)."""
        ...
    def parameter_set_double(self, name: str, value: float) -> int:
        """Write a DOUBLE value into a parameter (deprecated)."""
        ...
    def parameter_set_string(self, name: str, value: str) -> int:
        """Write a STRING value into a parameter (deprecated)."""
        ...
    def parameter_set_data(self, name: str, value: bytes) -> int:
        """Write a DATA value into a parameter (deprecated)."""
        ...

    def clear_input(self, name: str) -> int:
        """Clear the in-memory value of an input."""
        ...
    def clear_output(self, name: str) -> int:
        """Clear the in-memory value of an output."""
        ...
    def clear_attribute(self, name: str) -> int:
        """Clear the in-memory value of an attribute."""
        ...
    def clear_parameter(self, name: str) -> int:
        """Clear the in-memory value of a parameter (deprecated)."""
        ...

    def observe_input(self, name: str, callback: IoObserveCallback, my_data: Any) -> int:
        """Register a callback called whenever an input changes."""
        ...
    def observe_output(self, name: str, callback: IoObserveCallback, my_data: Any) -> int:
        """Register a callback called whenever an output changes."""
        ...
    def observe_attribute(self, name: str, callback: IoObserveCallback, my_data: Any) -> int:
        """Register a callback called whenever an attribute changes."""
        ...
    def observe_parameter(self, name: str, callback: IoObserveCallback, my_data: Any) -> int:
        """Register a callback called whenever a parameter changes (deprecated)."""
        ...

    def output_mute(self, name: str) -> int:
        """Mute a single output."""
        ...
    def output_unmute(self, name: str) -> int:
        """Unmute a single output."""
        ...
    def output_is_muted(self, name: str) -> bool:
        """Return whether a single output is muted."""
        ...

    # --- Mapping -----------------------------------------------------------
    def mapping_load_str(self, mapping_json: str) -> int:
        """Load this agent's mapping from a JSON string."""
        ...
    def mapping_load_file(self, path: str) -> int:
        """Load this agent's mapping from a JSON file."""
        ...
    def mapping_json(self) -> str:
        """Return this agent's mapping as a JSON string."""
        ...
    def mapping_count(self) -> int:
        """Return the number of entries in the mapping."""
        ...
    def clear_mappings(self) -> int:
        """Clear all mappings with all agents."""
        ...
    def clear_mappings_with_agent(self, agent_name: str) -> int:
        """Clear our mappings with a given agent."""
        ...
    def clear_mappings_for_input(self, agent_name: str) -> int:
        """Clear all mappings for one of our inputs."""
        ...
    def mapping_add(self, from_our_input: str, to_agent: str, with_output: str) -> int:
        """Map one of our inputs onto another agent's output. Returns the mapping id (0 on failure)."""
        ...
    def mapping_remove_with_id(self, id: int) -> int:
        """Remove a mapping entry by its id."""
        ...
    def mapping_remove_with_name(self, from_our_input: str, to_agent: str, with_output: str) -> int:
        """Remove a mapping entry by input/agent/output names."""
        ...
    def split_count(self) -> int:
        """Return the number of split entries."""
        ...
    def split_add(self, from_our_input: str, to_agent: str, with_output: str) -> int:
        """Add a split entry. Returns the split id (0 on failure)."""
        ...
    def split_remove_with_id(self, id: int) -> int:
        """Remove a split entry by its id."""
        ...
    def split_remove_with_name(self, from_our_input: str, to_agent: str, with_output: str) -> int:
        """Remove a split entry by input/agent/output names."""
        ...
    def mapping_outputs_request(self) -> bool:
        """Return whether mapped agents are asked to send their current output values."""
        ...
    def mapping_set_outputs_request(self, notify: bool) -> int:
        """Ask mapped agents to send their current output values upon mapping."""
        ...

    # --- Services ----------------------------------------------------------
    def service_call(self, agent_name_or_uuid: str, service_name: str, argument_tuple: Any,
                     token: Optional[str]) -> int:
        """Call a service on another agent (see module-level ``service_call``)."""
        ...
    def service_init(self, service_name: str, callback: ServiceCallback, my_data: Any) -> int:
        """Create a service  and attach its implementation callback."""
        ...
    def service_remove(self, service_name: str) -> int:
        """Remove a service from this agent."""
        ...
    def service_set_description(self, service_name: str, description: str) -> int:
        """Set the description of a service."""
        ...
    def service_description(self, service_name: str) -> str:
        """Return the description of a service."""
        ...
    def service_arg_set_description(self, service_name: str, arg_name: str, description: str) -> int:
        """Set the description of a service argument."""
        ...
    def service_arg_description(self, service_name: str, arg_name: str) -> str:
        """Return the description of a service argument."""
        ...
    def service_reply_set_description(self, service_name: str, reply_name: str, description: str) -> int:
        """Set the description of a service reply."""
        ...
    def service_reply_description(self, service_name: str, reply_name: str) -> str:
        """Return the description of a service reply."""
        ...
    def service_reply_arg_set_description(self, service_name: str, reply_name: str, arg_name: str,
                                          description: str) -> int:
        """Set the description of a service reply argument."""
        ...
    def service_reply_arg_description(self, service_name: str, reply_name: str, arg_name: str) -> str:
        """Return the description of a service reply argument."""
        ...
    def service_reply_add(self, service_name: str, reply_name: str) -> int:
        """Add an optional reply to a service."""
        ...
    def service_reply_remove(self, service_name: str, reply_name: str) -> int:
        """Remove an optional reply from a service."""
        ...
    def service_reply_arg_add(self, service_name: str, reply_name: str, arg_name: str, type: int) -> int:
        """Add an argument to a service reply."""
        ...
    def service_reply_arg_remove(self, service_name: str, reply_name: str, arg_name: str) -> int:
        """Remove the first argument with this name from a service reply."""
        ...
    def service_has_replies(self, service_name: str) -> bool:
        """Return whether a service declares at least one reply."""
        ...
    def service_has_reply(self, service_name: str, reply_name: str) -> bool:
        """Return whether a service declares a reply with this name."""
        ...
    def service_reply_names(self, service_name: str) -> List[str]:
        """Return the reply names declared by a service."""
        ...
    def service_reply_args_count(self, service_name: str, reply_name: str) -> int:
        """Return the number of arguments of a service reply."""
        ...
    def service_reply_args_list(self, service_name: str, reply_name: str) -> Tuple[Tuple[str, int], ...]:
        """Return the reply arguments as a tuple of (name, value_type) pairs."""
        ...
    def service_reply_arg_exists(self, service_name: str, reply_name: str, argument_name: str) -> bool:
        """Return whether a service reply has an argument with this name."""
        ...
    def service_arg_add(self, service_name: str, arg_name: str, value_type: int) -> int:
        """Add an argument to a service."""
        ...
    def service_arg_remove(self, service_name: str, arg_name: str) -> int:
        """Remove the first argument with this name from a service."""
        ...
    def service_count(self) -> int:
        """Return the number of services offered by this agent."""
        ...
    def service_exists(self, service_name: str) -> bool:
        """Return whether a service with this name exists."""
        ...
    def service_list(self) -> Tuple[str, ...]:
        """Return the names of the services offered by this agent."""
        ...
    def service_args_count(self, service_name: str) -> int:
        """Return the number of arguments of a service."""
        ...
    def service_args_list(self, service_name: str) -> Tuple[Tuple[str, int], ...]:
        """Return the service arguments as a tuple of (name, value_type) pairs."""
        ...
    def service_arg_exists(self, service_name: str, arg_name: str) -> bool:
        """Return whether a service has an argument with this name."""
        ...

    # --- Elections ---------------------------------------------------------
    def election_join(self, election_name: str) -> int:
        """Join a named election."""
        ...
    def election_leave(self, election_name: str) -> int:
        """Leave a named election."""
        ...

    # --- Real-time ---------------------------------------------------------
    def rt_get_current_timestamp(self) -> int:
        """In an input/service callback, return the timestamp (in microseconds) of the received info."""
        ...
    def rt_set_timestamps(self, enable: bool) -> int:
        """Enable timestamps on this agent's published outputs and service calls."""
        ...
    def rt_timestamps(self) -> bool:
        """Return whether timestamps are enabled."""
        ...
    def rt_set_synchronous_mode(self, enable: bool) -> int:
        """Enable synchronous mode for this agent."""
        ...
    def rt_synchronous_mode(self) -> bool:
        """Return whether synchronous mode is enabled."""
        ...

    # --- Administration ----------------------------------------------------
    def definition_set_path(self, path: str) -> int:
        """Set the file path used by ``definition_save``."""
        ...
    def definition_save(self) -> int:
        """Write the current definition to disk at the configured path."""
        ...
    def mapping_set_path(self, path: str) -> int:
        """Set the file path used by ``mapping_save``."""
        ...
    def mapping_save(self) -> int:
        """Write the current mapping to disk at the configured path."""
        ...

    # --- Constraints & descriptions ----------------------------------------
    def constraints_enforce(self, enforce: bool) -> int:
        """Enable or disable enforcement of IO constraints for this agent."""
        ...
    def input_add_constraint(self, name: str, constraint: str) -> int:
        """Add a constraint to an input."""
        ...
    def output_add_constraint(self, name: str, constraint: str) -> int:
        """Add a constraint to an output."""
        ...
    def attribute_add_constraint(self, name: str, constraint: str) -> int:
        """Add a constraint to an attribute."""
        ...
    def parameter_add_constraint(self, name: str, constraint: str) -> int:
        """Add a constraint to a parameter (deprecated)."""
        ...
    def input_set_description(self, name: str, description: str) -> int:
        """Set the description of an input."""
        ...
    def input_description(self, name: str) -> str:
        """Return the description of an input."""
        ...
    def output_set_description(self, name: str, description: str) -> int:
        """Set the description of an output."""
        ...
    def output_description(self, name: str) -> str:
        """Return the description of an output."""
        ...
    def attribute_set_description(self, name: str, description: str) -> int:
        """Set the description of an attribute."""
        ...
    def attribute_description(self, name: str) -> str:
        """Return the description of an attribute."""
        ...
    def parameter_set_description(self, name: str, description: str) -> int:
        """Set the description of a parameter (deprecated)."""
        ...
    def input_set_detailed_type(self, input_name: str, type_name: str, specification: str) -> int:
        """Attach a descriptive detailed type and specification to an input."""
        ...
    def output_set_detailed_type(self, output_name: str, type_name: str, specification: str) -> int:
        """Attach a descriptive detailed type and specification to an output."""
        ...
    def attribute_set_detailed_type(self, attribute_name: str, type_name: str, specification: str) -> int:
        """Attach a descriptive detailed type and specification to an attribute."""
        ...
