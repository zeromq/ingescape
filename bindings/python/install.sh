#!/bin/sh
#python 3 has to be installed on your system before continuing.
#ingeScape has to be installed on your system before continuing so do libsodium zyre and czmq



cd ingescapeWrapper/ingescapeWrapp
python3 setup.py build
sudo python3 setup.py install
cd ../../ingescapepython
python3 setup.py install