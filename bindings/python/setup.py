# -*- coding: utf-8 -*-
#distutils: language = c
#distutils: sources = /usr/local/Frameworks/
__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "8.0.1"

import sys
from setuptools import find_packages
from distutils.core import setup, Extension

sys.path.extend("/usr/local/include/")

link_args = ["-lingescape"]
inc_dirs = ["/usr/local/include/"]
src = ["./ingescapeWrapper/ingescapeWrapp/ingescapePython.c",
       "./ingescapeWrapper/ingescapeWrapp/admin.c", 
       "./ingescapeWrapper/ingescapeWrapp/data.c", 
       "./ingescapeWrapper/ingescapeWrapp/definition.c", 
       "./ingescapeWrapper/ingescapeWrapp/mapping.c", 
       "./ingescapeWrapper/ingescapeWrapp/freezecallback.c", 
       "./ingescapeWrapper/ingescapeWrapp/init.c", 
       "./ingescapeWrapper/ingescapeWrapp/input.c", 
       "./ingescapeWrapper/ingescapeWrapp/observecallback.c", 
       "./ingescapeWrapper/ingescapeWrapp/output.c", 
       "./ingescapeWrapper/ingescapeWrapp/parameter.c", 
       "./ingescapeWrapper/ingescapeWrapp/start.c", 
       "./ingescapeWrapper/ingescapeWrapp/stopcallback.c", 
       "./ingescapeWrapper/ingescapeWrapp/advanced.c"]

# the c extension module

extension_mod = Extension("ingescape", src, extra_compile_args=["-g"],  extra_link_args=[ "-lingescape"] )
setup(name                = "ingescape",
      version             = "8.0.1",
      license             = "Copyright © 2018 ingenuity. All rights reserved.",
      ext_modules         = [extension_mod],
      packages=find_packages(),
      include_package_data=True,
      entry_points={}
      )

