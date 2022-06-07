#!/bin/bash

# =========================================================================
# build_and_run_tests - Script used to build, install and test the binding
# 
# Copyright (c) the Contributors as noted in the AUTHORS file.
# This file is part of Ingescape, see https://github.com/zeromq/ingescape.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# =========================================================================
#

set -e

SCRIPT_DIR=$(cd `dirname $0`; pwd)

python3 -m pip install $SCRIPT_DIR/..

echo -e "\n\n**** GLOBAL API ***************************"
python3 $SCRIPT_DIR/global_api.py
echo -e "\n\n**** AGENT API ***************************"
python3 $SCRIPT_DIR/agent_api.py
