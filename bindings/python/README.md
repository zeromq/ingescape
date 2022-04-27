# Ingescape python - Python module based on [Ingescape](#https://github.com/zeromq/ingescape)

**[License](#License)**

**[Dependencies](#Dependencies)**

**[Installing Ingescape](#Installing-Ingescape)**

**[Using Ingescape in Python](#Using-Ingescape-in-Python)**

**[API Summary](#api-summary)**


## License
Copyright (c) the Contributors as noted in the AUTHORS file.
This file is part of Ingescape, see https://github.com/zeromq/ingescape.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.


## Dependencies

Ingescape python relies on the following libraries:

- Python 3.6+ 
- setuptools, wheel


## Installing Ingescape

Ingescape python is designed to be installed trought pip :

	python3 -m pip install setuptools wheel
	python3 -m pip install ingescape

## Using Ingescape in Python

A full example is available in examples/echo_example.py


## API Summary
The Ingescape module provides :

 - Global functions to use when you only have one agent in your process or application.
 - An _Agent_ class that provides all the specific functions dedicated to several agents inside the same process.

