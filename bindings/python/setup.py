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

macos_lib_dirs = '/usr/local/lib/' #TODO: use lib path instead of hardcoding
linux_lib_dirs = '/usr/local/lib/'
windows_x64_lib_dirs = 'C:\\Program Files\\ingescape\\lib\\'
windows_x86_lib_dirs = 'C:\\Program Files (x86)\\ingescape\\lib\\'

macos_lib_dirs_from_artifacts = './dependencies/macos/'
linux_lib_dirs_from_artifacts = './dependencies/linux/'
windows_x64_lib_dirs_from_artifacts = './dependencies/windows/x64/'
windows_x86_lib_dirs_from_artifacts = './dependencies/windows/x86/'

extra_objects = []
librairies = []
librairies_dirs = []
ingescape_c_include = ['./dependencies/include/']

compile_args = []
link_args = []

manual_compiler_args = os.environ.get('FROM_SOURCES', default=None)

if platform.system() == "Linux":
  if manual_compiler_args:
    extra_objects.append(linux_lib_dirs_from_artifacts + 'libingescape.a')
    extra_objects.append(linux_lib_dirs_from_artifacts + 'libzyre.a')
    extra_objects.append(linux_lib_dirs_from_artifacts + 'libczmq.a')
    extra_objects.append(linux_lib_dirs_from_artifacts + 'libzmq.a')
    extra_objects.append(linux_lib_dirs_from_artifacts + 'libsodium.a')
  else:
    extra_objects.append(linux_lib_dirs + 'libingescape.a')
    extra_objects.append(linux_lib_dirs + 'libzyre.a')
    extra_objects.append(linux_lib_dirs + 'libczmq.a')
    extra_objects.append(linux_lib_dirs + 'libzmq.a')
    extra_objects.append(linux_lib_dirs + 'libsodium.a')
  compile_args = ["-I/usr/local/include/python3.8", "-I/usr/local/include/python3.8", "-Wno-unused-result", "-Wsign-compare", "-g", "-fwrapv", "-O3", "-Wall"]
  link_args = ["-L/usr/local/lib", "-lcrypt", "-lpthread", "-ldl",  "-lutil", "-lm", "-lstdc++"]
elif platform.system() == "Darwin":
  if manual_compiler_args:
    extra_objects.append(macos_lib_dirs_from_artifacts + 'libingescape.a')
    extra_objects.append(macos_lib_dirs_from_artifacts + 'libzyre.a')
    extra_objects.append(macos_lib_dirs_from_artifacts + 'libczmq.a')
    extra_objects.append(macos_lib_dirs_from_artifacts + 'libzmq.a')
    extra_objects.append(macos_lib_dirs_from_artifacts + 'libsodium.a')
  else:
    extra_objects.append(macos_lib_dirs + 'libingescape.a')
    extra_objects.append(macos_lib_dirs + 'libzyre.a')
    extra_objects.append(macos_lib_dirs + 'libczmq.a')
    extra_objects.append(macos_lib_dirs + 'libzmq.a')
    extra_objects.append(macos_lib_dirs + 'libsodium.a')
elif platform.system() == "Windows":
  if platform.machine().endswith('64'):
    librairies = ["libzmq",'libingescape', 'libzyre', 'libczmq', 'libsodium', "ws2_32", "Iphlpapi", 'Rpcrt4']
    if manual_compiler_args:
      librairies_dirs.append(windows_x64_lib_dirs_from_artifacts)
      sys.path.extend(windows_x64_lib_dirs_from_artifacts)
    else:
      librairies_dirs.append(windows_x64_lib_dirs)
      sys.path.extend(windows_x64_lib_dirs)
    compile_args = ["-DINGESCAPE_STATIC"]

#Use an environment variable instead of "install-option" to add the compile arg. We are not able to use 'python wheels' with 'install-option' 

if manual_compiler_args:
    compile_args.append("-DFROM_SOURCES")

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
