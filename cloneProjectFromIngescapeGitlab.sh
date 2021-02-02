#!/bin/bash

set -e

SCRIPT_DIR=$(cd `dirname $0`; pwd)
SCRIPT_NAME=$(basename $0)

function print_usage {
    echo "Usage: $SCRIPT_NAME <project_path> ..."
    echo "  <project_path> : subpath of the project to clone from https://gitlab.ingescape.com/ (at least one required)"
    echo ""
    echo "Example :"
    echo "To clone a project reachable at https://gitlab.ingescape.com/learn/firstfullagent"
    echo "type '$SCRIPT_NAME learn/firstfullagent'."
    echo "Based on your specific script location, a directory based on the project url will be"
    echo "created under $SCRIPT_DIR/../work,"
    echo "i.e. inside a 'work' locate at the same place as the ingescape library."
    echo ""
    echo "Several <project_path> seperated by a space may be given to clone several projects at once."
    echo ""
}

projects=""

while [[ $# -gt 0 ]]; do
    projects="${projects} $1"
    shift
done

if [ -z $projects ]
then
    echo "ERROR: At least one <project_path> is REQUIRED as argument"
    echo ""
    print_usage
    exit 200
fi

mkdir -p ${SCRIPT_DIR}/../work
work_path=$(cd ${SCRIPT_DIR}/../work; pwd)
echo "####################################"
echo "Work  directory    : ${work_path}"

for p in $(echo ${projects})
do
    echo "-----------------------------------"
    echo "Requested project  : ${p}"
    if [[ "${p}" =~ "^/" ]]
    then
        p=$(echo ${p} | cut -d'/' -f2-)
    fi

    splitted_path=$(sed 's@/@ @g' <<< ${p})
    clone_dir_name=$(echo ${splitted_path} | sed 's/ /-/g')
    echo "Clone directory    : ${clone_dir_name}"

    if [ -d ${work_path}/${clone_dir_name} ]
    then
        echo "Directory ${work_path}/${clone_dir_name} already exists. Ignoring."
    else
        echo "Running 'git clone ssh://git@gitlab.ingescape.com:22222/${p} ${work_path}/${clone_dir_name}'"
        git clone --recurse-submodules ssh://git@gitlab.ingescape.com:22222/${p} ${work_path}/${clone_dir_name}
    fi
done
echo "-----------------------------------"

exit 0
