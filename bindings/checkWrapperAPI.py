#=========================================================================
#checkWrapperAPI.py
#Copyright (c) the Contributors as noted in the AUTHORS file.
#This file is part of Ingescape, see https://github.com/zeromq/ingescape.
#This Source Code Form is subject to the terms of the Mozilla Public
#License, v. 2.0. If a copy of the MPL was not distributed with this
#file, You can obtain one at http://mozilla.org/MPL/2.0/.
#=========================================================================

import sys
import os
import re
import getopt
import platform

ingescapeHeadersNames = ["ingescape.h", "igsagent.h"]
ingescapeHeadersPath = []
apis = []
exclusions = [
    "igs_observe_channels",
    "igs_channel_join",
    "igs_channel_leave",
    "igs_channel_shout_str",
    "igs_channel_shout_data",
    "igs_channel_whisper_str",
    "igs_channel_whisper_data",
    "igs_channel_shout_zmsg",
    "igs_channel_whisper_zmsg",
    "igs_peer_add_header",
    "igs_peer_remove_header",
    "igs_json_new",
    "igs_json_node_is_double",
    "igs_json_parse_from_file",
    "igs_json_parse_from_str",
    "igs_json_destroy",
    "igs_json_open_map",
    "igs_json_close_map",
    "igs_json_open_array",
    "igs_json_close_array",
    "igs_json_add_null",
    "igs_json_add_bool",
    "igs_json_add_int",
    "igs_json_add_double",
    "igs_json_add_string",
    "igs_json_print",
    "igs_json_insert_node",
    "igs_json_node_destroy",
    "igs_json_node_insert",
    "igs_json_node_is_integer",
    "igs_json_node_is_double"
    "igs_json_new",
    "igs_json_dump",
    "igs_json_compact_dump",
    "igs_json_node_for",
    "igs_json_node_parse_from_file",
    "igs_json_node_parse_from_str",
    "igs_json_node_dup",
    "igs_json_node_dump",
    "igs_json_node_find"
]
files = []

def usage():
    print(
"""\
Usage example:
     python checkWrapperAPI.py --headers usr/local/include/ingescape --sources /Users/aurelien/Documents/ingescape/bindings/csharp/src

Options:
  --sources                    : (Mandatory) Wrapper sources to check
  --headers                    : Ingescape header folder location (default: (unix) /usr/local/include/ingescape | (windows) C:\Program Files\Ingescape\library\include\ingescape)
  -h                           : Print this help message and exit
  -v, --verbose                : Verbose mode (default: silent)
""")


if __name__ == '__main__':
    if (platform.system() == "Windows"):
        headersFolder = "C:\Program Files\Ingescape\library\include\ingescape"
    else:
        headersFolder = "/usr/local/include/ingescape"
    filesFolder = ""
    verbose = False;
    # CLI option parsing
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ce:f:hpv",
                                   ["headers=","sources=","help", "verbose"])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(err)  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    for opt, opt_value in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-v", "--verbose"):
            verbose = True
        elif opt == "--headers":
            headersFolder = opt_value
        elif opt == "--sources":
            filesFolder = opt_value
        else:
            assert False, "unhandled option"

    if (len(filesFolder) == 0):
        usage()
        sys.exit()

    if not headersFolder.endswith('/'):
        headersFolder += '/'

    for headerName in ingescapeHeadersNames:
        ingescapeHeadersPath.append(os.path.join(headersFolder, headerName))

    # Check if ingescape headers exist in the given folder
    for header in ingescapeHeadersPath:
        if not (os.path.exists(header)):
            print(header + " not found")
            sys.exit()

    if not (os.path.exists(filesFolder)):
        print("Sources folder \"",filesFolder,"\" not found")
        sys.exit()

    for header in ingescapeHeadersPath:
        with open(header, 'r') as hFile:
            if (verbose):
                print("\nAPI from",header,":")
            for line in hFile:
                res = re.search("^INGESCAPE_EXPORT (\w+)(?:\s[\*]*\s|\s)(\w+)( ?)\(.*$", line)
                if res != None:
                    if (verbose):
                        print(res.group(2).strip())
                    apis.append(res.group(2).strip())

    apisToImplement = apis.copy()
    for exclusion in exclusions:
        if exclusion in apisToImplement:
            apisToImplement.remove(exclusion)

    for api in apis:
        for root, dirs, files in os.walk(filesFolder):
            for file in files:
                if not (file.startswith('.')): # Hidden files
                    with open(os.path.join(root, file), 'r') as srcFile:
                        for line in srcFile:
                            res = re.search(api, line)
                            if res != None:
                                if api in apisToImplement:
                                    apisToImplement.remove(api)
                                break

    print("\nNot implemented methods :")
    for apiToImplement in apisToImplement:
        print(apiToImplement)