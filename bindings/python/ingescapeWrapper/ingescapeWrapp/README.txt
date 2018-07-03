INGESCAPE WRAPPER - Python/C extension for INGESCAPE
==============================================

Multi Agent Supervision Transport Integration 
and Control

Created by vaugien.
Copyright © 2018 ingenuity. All rights reserved.

-----------------INSTALLATION-----------------
==============================================

In a terminal :
	-sudo python3 setup.py build
	-sudo python3 setup.py install


-----------------DEPENDENCIES-----------------
==============================================

Dependencies :
	-Python.frameworks 3.6.4
	-libingescape.dylib
	-libzyre.dylib
	-libyajl.dylib
    -libzmq.5.dylib
    -libczmq.4.dylib


-------------------WARNINGS-------------------
==============================================
 -All function dealing with data expect a byte
 object as myData parameter.
-When dealing with data you have to substarct
33 to the size.

-------------------EXAMPLES-------------------
==============================================

In a python3 script:
|
|from ingescape import *
|import pickle
|import sys
|
|def callback_for_input(msg):
|   int_received = igs_readInputAsInt("input_int")
|   str_received = igs_readInputAsInt("input_int")
|   message_to_send = pickle.dumps(str_received + str(int_received))
|   size = sys.getsizeof(message_to_send)-33
|   igs_writeOutputAsData("output_data", message_to_send, size)
|   print(msg)
|
|igs_setAgentName("demoAgent")
|igs_setDefinitionName("definitionForDemoAgent")
|igs_setDefinitionversion("0.0.1")
|igs_setDefinitionDescription("simple definition for the example")
|
|igs_createInput("input_int", 1, 0, 0)
|igs_createInput("input_str", 3, "", 0)
|
|igs_createOutput("output_data", 6, 0, 0)
|
|igs_observeInput("input_int", callback_for_input, "int received")
|igs_observeInput("input_str", callback_for_input, "str received")
|
|

