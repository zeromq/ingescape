#!/usr/bin/env bash

ZEROMQ_REPO_BASE_URL_DEBIAN="http://download.opensuse.org/repositories/network:/messaging:/zeromq:/git-stable/Debian"
ZEROMQ_REPO_URL_DEBIAN_9="${ZEROMQ_REPO_BASE_URL_DEBIAN}_9.0/"
ZEROMQ_REPO_URL_DEBIAN_10="${ZEROMQ_REPO_BASE_URL_DEBIAN}_Next/"


OS=""
VER=""
ARCH=""


function _check_sudo {
    if [[ $EUID = 0 ]]
    then
        $1
    else
        sudo $1
    fi
}

function discover_os {
    if [ -f /etc/os-release ]; then
        # freedesktop.org and systemd
        . /etc/os-release
        OS=$NAME
        # As of March 1st 2019, there is no VERSION_ID in os-release for buster (testing). The following condition checks that
        if [[ -z ${VERSION_ID+x} ]]
        then
            VER="10"
        else
            VER=$VERSION_ID
        fi
    elif type lsb_release >/dev/null 2>&1; then
        # linuxbase.org
        OS=$(lsb_release -si)
        VER=$(lsb_release -sr)
    elif [ -f /etc/lsb-release ]; then
        # For some versions of Debian/Ubuntu without lsb_release command
        . /etc/lsb-release
        OS=$DISTRIB_ID
        VER=$DISTRIB_RELEASE
    elif [ -f /etc/debian_version ]; then
        # Older Debian/Ubuntu/etc.
        OS=Debian
        VER=$(cat /etc/debian_version)
    elif [ -f /etc/SuSe-release ]; then
        # Older SuSE/etc.
        OS=SuSE
        VER=$(cat /etc/SuSe-version)
    elif [ -f /etc/redhat-release ]; then
        # Older Red Hat, CentOS, etc.
        OS=REHL
        VER=$(cat /etc/redhat-version)
    else
        # Fall back to uname, e.g. "Linux <version>", also works for BSD, etc.
        OS=$(uname -s)
        VER=$(uname -r)
    fi
}

function discover_arch {
    case $(uname -m) in
    x86_64)
        ARCH=x64  # x64
        ;;
    i*86)
        ARCH=x86  # x86
        ;;
    armv7)
        ARCH=x86  # ARMv7 (armhf)
        ;;
    *)
        # leave ARCH as-is
        ;;
    esac
}

function install_libsodium {
    if [[ "$OS" =~ "Debian" ]]
    then
        if [[ "$VER" =~ "9" ]]
        then
            apt install -y libsodium18
        elif [[ "$VER" =~ "10" ]]
        then
            apt install -y libsodium23
        fi
    elif [[ "$OS" =~ "CentOS" ]]
    then
        yum install -y libsodium18
    else
        git clone --depth 1 -b stable https://github.com/jedisct1/libsodium.git
        cd libsodium
        ./autogen.sh && ./configure && make check
        _check_sudo "make install"
        cd ..
    fi
}

function install_libzmq {
    if [[ "$OS" =~ "Debian" ]]
    then
        apt install -y libzmq5
    elif [[ "$OS" =~ "CentOS" ]]
    then
        yum install -y libzmq5
    else
        git clone git://github.com/zeromq/libzmq.git
        cd libzmq
        ./autogen.sh
        # do not specify "--with-libsodium" if you prefer to use internal tweetnacl
        # security implementation (recommended for development)
        ./configure
        make check
        _check_sudo "make install"
        _check_sudo ldconfig
        cd ..
    fi
}

function install_czmq {
    if [[ "$OS" =~ "Debian" ]]
    then
        apt install -y czmq
    elif [[ "$OS" =~ "CentOS" ]]
    then
        yum install -y czmq
    else
        git clone git://github.com/zeromq/czmq.git
        cd czmq
        ./autogen.sh && ./configure && make check
        _check_sudo "make install"
        _check_sudo ldconfig
        cd ..
    fi
}

function install_zyre {
    if [[ "$OS" =~ "Debian" ]]
    then
        apt install -y zyre
    elif [[ "$OS" =~ "CentOS" ]]
    then
        yum install -y zyre
    else
        git clone git://github.com/zeromq/zyre.git
        cd zyre
        ./autogen.sh && ./configure && make check
        _check_sudo "make install"
        _check_sudo ldconfig
        cd ..
    fi
}

function setup_repos {
    case $OS in
        *Debian*)
            if grep -Fq "${ZEROMQ_REPO_BASE_URL_DEBIAN}" /etc/apt/sources.list
            then
                echo "ZeroMQ package repository already present in /etc/apt/sources.list"
            else
                case $VER in
                    *10*)
                        ## Debian buster
                        echo "" >> /etc/apt/sources.list
                        echo "# ZeroMQ repository (added by ingescape)" >> /etc/apt/sources.list
                        echo "deb ${ZEROMQ_REPO_URL_DEBIAN_10} ./" >> /etc/apt/sources.list
                        wget ${ZEROMQ_REPO_URL_DEBIAN_10}Release.key -O- | apt-key add
                        ;;

                    *9*)
                        ## Debian stretch
                        echo "" >> /etc/apt/sources.list
                        echo "# ZeroMQ repository (added by ingescape)" >> /etc/apt/sources.list
                        echo "deb ${ZEROMQ_REPO_URL_DEBIAN_9} ./" >> /etc/apt/sources.list
                        wget ${ZEROMQ_REPO_URL_DEBIAN_9}Release.key -O- | apt-key add
                        ;;
                esac
            fi

            # Update package index
            apt update

            ;;
        *CentOS*)
            case $VER in
                *6*)
                    ## CentOS 6
                    yum-config-manager --add-repo https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/CentOS_6/network:messaging:zeromq:release-stable.repo
                    yum-config-manager --enable https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/CentOS_6/network:messaging:zeromq:release-stable.repo
                    ;;

                *7*)
                    ## CentOS 7
                    yum-config-manager --add-repo https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/CentOS_7/network:messaging:zeromq:release-stable.repo
                    yum-config-manager --enable https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/CentOS_7/network:messaging:zeromq:release-stable.repo
                    ;;
            esac

            # No need to update index for YUM

            ;;
        *)
            # No repository to add
            ;;
    esac
}

function install_deps {
    setup_repos

    install_libsodium
    install_libzmq
    install_czmq
    install_zyre
}

function install_ingescape {
    if [[ "$OS" =~ "Debian" ]]
    then
        # Debian
        dpkg -i ingescape-0.9.0-Linux.deb
        apt install -fy
    elif [[ "$OS" =~ "CentOS" ]]
    then
        rpm -Uvh ingescape-0.9.0-Linux.rpm
    #else
        #TODO install from ZIP
    fi

}

#init
set -o errexit
set -o nounset
set -o xtrace
set -o pipefail

discover_os
discover_arch
install_deps
install_ingescape


