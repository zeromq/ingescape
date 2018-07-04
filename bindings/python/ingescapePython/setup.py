#!/usr/bin/env python
# -*- coding: utf-8 -*-

__author__ = "vaugien"
__copyright__ = "Copyright © 2018 ingenuity."
__license__ = "All rights reserved."
__version__ = "0.1"

from setuptools import setup, find_packages

from ingescapePython import *

setup(

    name='ingescapePython',

    version=__version__,

    packages=find_packages(),

    description="wrapper for the ingescape library",

    long_description=open('README.txt').read(),

    include_package_data=True,

    author="vaugien",

    entry_points={},

    license="Copyright © 2018 ingenuity. All rights reserved",

)
