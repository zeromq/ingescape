# -*- coding: utf-8 -*-
#  =========================================================================
# setup.py - fichier de configuration du module Ingescape
#
# Copyright (c) the Contributors as noted in the AUTHORS file.
# This file is part of Ingescape, see https://github.com/zeromq/ingescape.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# =========================================================================
#

__author__ = "vaugien"
__copyright__ = "Copyright (c) the Contributors as noted in the AUTHORS file.\
                  This file is part of Ingescape, see https://github.com/zeromq/ingescape.\
                  This Source Code Form is subject to the terms of the Mozilla Public\
                  License, v. 2.0. If a copy of the MPL was not distributed with this\
                  file, You can obtain one at http://mozilla.org/MPL/2.0/."
__license__ = "All rights reserved."
__version__ = "3.0.4"

import sys
from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext
import os
import platform
import struct

ingescape_src = ["./src/ingescape_python.c", "./src/admin.c",
                  "./src/channels.c",  "./src/core.c",
                  "./src/monitor.c", "./src/network.c", 
                  "./src/performance.c",
                  "./src/replay.c"]
ingescape_agent_src =["./src/agent.c"]
ingescape_include = ["./includes"]

macos_lib_dirs = './dependencies/macos/'
linux_lib_dirs = './dependencies/linux/'
windows_x64_lib_dirs = './dependencies/windows/x64/'
windows_x86_lib_dirs = './dependencies/windows/x86/'

extra_objects = []
librairies = []
librairies_dirs = []
ingescape_c_include = ['./dependencies/include/']

compile_args = []
link_args = []

if platform.system() == "Linux":
  extra_objects.append(linux_lib_dirs + 'libingescape.a')
  extra_objects.append(linux_lib_dirs + 'libzyre.a')
  extra_objects.append(linux_lib_dirs + 'libczmq.a')
  extra_objects.append(linux_lib_dirs + 'libzmq.a')
  extra_objects.append(linux_lib_dirs + 'libsodium.a')
  compile_args = ["-I/usr/local/include/python3.8", "-I/usr/local/include/python3.8", "-Wno-unused-result", "-Wsign-compare", "-g", "-fwrapv", "-O3", "-Wall"]
  link_args = ["-L/usr/local/lib", "-lcrypt", "-lpthread", "-ldl",  "-lutil", "-lm", "-lstdc++"]
elif platform.system() == "Darwin":
  extra_objects.append(macos_lib_dirs + 'libingescape.a')
  extra_objects.append(macos_lib_dirs + 'libzyre.a')
  extra_objects.append(macos_lib_dirs + 'libczmq.a')
  extra_objects.append(macos_lib_dirs + 'libzmq.a')
  extra_objects.append(macos_lib_dirs + 'libsodium.a')
elif platform.system() == "Windows":
  if platform.machine().endswith('64'):
    librairies = ["libzmq",'libingescape', 'libzyre', 'libczmq', 'libsodium', "ws2_32", "Iphlpapi", 'Rpcrt4']
    librairies_dirs.append(windows_x64_lib_dirs)
    sys.path.extend(windows_x64_lib_dirs)
    compile_args = ["-DINGESCAPE_STATIC"]


extension_ingescape = Extension("ingescape", ingescape_src + ingescape_agent_src ,
            include_dirs = ingescape_include + ingescape_c_include,
            libraries = librairies,
            library_dirs = librairies_dirs,
            extra_objects = extra_objects,
            extra_compile_args = compile_args,
            extra_link_args = link_args)

setup(name =  "ingescape",
      author = "Natanael Vaugien",
      author_email = "vaugien@ingenuity.io",
      url= "https://github.com/zeromq/ingescape",
      version =  "3.0.4",
      classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "License :: OSI Approved :: Mozilla Public License 2.0 (MPL 2.0)",
        "Operating System :: OS Independent",
      ],
      license =  "Copyright (c) the Contributors as noted in the AUTHORS file."
                  "This file is part of Ingescape, see https://github.com/zeromq/ingescape."
                  "This Source Code Form is subject to the terms of the Mozilla Public"
                  "License, v. 2.0. If a copy of the MPL was not distributed with this"
                  "file, You can obtain one at http://mozilla.org/MPL/2.0/.",
      ext_modules = [extension_ingescape])
