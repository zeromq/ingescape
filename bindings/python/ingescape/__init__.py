# =========================================================================
# ingescape - Python binding package
#
# Copyright (c) the Contributors as noted in the AUTHORS file.
# This file is part of Ingescape, see https://github.com/zeromq/ingescape.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# =========================================================================
#
# Thin package wrapper around the compiled extension ``ingescape._ingescape``.
# It exists so that PEP 561 type information (py.typed + __init__.pyi) can be
# shipped next to the compiled module. The public API is unchanged: every name
# (functions, the Agent class and the constants) is re-exported here, so
# ``import ingescape`` and ``from ingescape import Agent`` keep working.

from ingescape._ingescape import *  # noqa: F401,F403
