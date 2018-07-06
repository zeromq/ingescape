--ingescapePython -- python library for INGESCAPE--
===================================================

Created by vaugien.
Copyright © 2018 ingenuity. All rights reserved.

This library is a python extension of the C
INGESCAPE library.

-------------------DEPENDENCIES--------------------
===================================================

Dependencies :
	-libingescape.dylib
	-libzyre.dylib
	-libyajl.dylib
    	-libzmq.5.dylib
    	-libczmq.4.dylib

Python3 and python3-dev for python3's Python.h

-------------------INSTALLATION--------------------
===================================================

In a terminal :
    cd /ingescapeWrapper/ingescapeWrapp
    python3 setup.py build
    sudo python3 setup.py install

    cd ../../ingescapepython
    python3 setup.py install

---------------------EXAMPLES----------------------
===================================================

see examples/receiveAgentPython
	python3 receive.py
or  examples/sendAgentPython
	python3 send.py	

