# -*- coding: utf-8 -*-
#distutils: language = c
#distutils: sources = /usr/local/Frameworks/
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "8.0.1"

import sys
from distutils.core import setup, Extension

sys.path.extend("/usr/local/include/")

link_args = ["-lingescape"]
inc_dirs = ["/usr/local/include/"]
src = ["ingescapePython.c", "admin.c", "data.c", "definition.c", "mapping.c", "freezecallback.c", "init.c", "input.c", "observecallback.c", "output.c", "parameter.c", "start.c", "stopcallback.c", "advanced.c", "call.c"]

# the c extension module

extension_mod = Extension("ingescape", src, extra_compile_args=["-g"],  extra_link_args=[ "-lingescape"] )
setup(name                = "ingescape",
      version             = "8.0.1",
      license             = "Copyright © 2018-2020 ingenuity. All rights reserved.",
      ext_modules         = [extension_mod])
