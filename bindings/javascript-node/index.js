/*  =========================================================================
    index.js - To load an existing prebuild. If not, fallback to n-api. 

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

var ingescape = require('node-gyp-build')(__dirname);
module.exports = ingescape;
