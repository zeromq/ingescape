set(CMAKE_OS_NAME ${CMAKE_SYSTEM_NAME})
set(CMAKE_OS_VERSION ${CMAKE_SYSTEM_VERSION})

if (UNIX)
    if (APPLE)
        set (CMAKE_OS_NAME "OSX" CACHE STRING "Operating system name" FORCE)
        execute_process(COMMAND sw_vers -productVersion OUTPUT_VARIABLE CMAKE_OS_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    else (APPLE)
        ## Check for Debian GNU/Linux _______________
        find_file (DEBIAN_FOUND debian_version debconf.conf PATHS /etc)
        if (DEBIAN_FOUND)
            set (CMAKE_OS_NAME "Debian" CACHE STRING "Operating system name" FORCE)
            file (STRINGS "/etc/debian_version" CMAKE_OS_VERSION)
            string (REGEX REPLACE "([0-9]+)(\\.[0-9]+)*" "\\1" CMAKE_OS_VERSION "${CMAKE_OS_VERSION}")

            ## Extra check for Ubuntu _______________
            find_file (UBUNTU_EXTRA legal issue PATHS /etc)
            if (UBUNTU_EXTRA)
                ## Scan contents of file
                file (STRINGS ${UBUNTU_EXTRA} UBUNTU_FOUND REGEX "Ubuntu [0-9]+\\.[0-9]+(\\.[0-9]+)?")
                ## Check result of string search
                if (UBUNTU_FOUND)
                    set (CMAKE_OS_NAME "Ubuntu" CACHE STRING "Operating system name" FORCE)
                    execute_process(COMMAND source /etc/lsb-release)
                    set (CMAKE_OS_VERSION "${DISTRIB_RELEASE}")
                    set (DEBIAN_FOUND FALSE)
                endif (UBUNTU_FOUND)
            endif (UBUNTU_EXTRA)
        endif (DEBIAN_FOUND)

        ## Check for Fedora _________________________
        find_file (FEDORA_FOUND fedora-release PATHS /etc)
        if (FEDORA_FOUND)
            set (CMAKE_OS_NAME "Fedora" CACHE STRING "Operating system name" FORCE)
        endif (FEDORA_FOUND)

        ## Check for RedHat _________________________
        find_file (REDHAT_FOUND redhat-release inittab.RH PATHS /etc)
        if (REDHAT_FOUND)
            set (CMAKE_OS_NAME "RedHat" CACHE STRING "Operating system name" FORCE)

            ## Extra check for CentOS _______________
            ## Scan contents of file
            file (STRINGS ${REDHAT_FOUND} CENTOS_FOUND REGEX "CentOS .* ([0-9]+)(\\.[0-9]+)*")
            ## Check result of string search
            if (CENTOS_FOUND)
                set (CMAKE_OS_NAME "CentOS" CACHE STRING "Operating system name" FORCE)
                string (REGEX REPLACE "CentOS .* ([0-9]+)(\\.[0-9]+)+.*" "\\1" CMAKE_OS_VERSION "${CENTOS_FOUND}")
                set (REDHAT_FOUND FALSE)
            endif (CENTOS_FOUND)
        endif (REDHAT_FOUND)
    endif (APPLE)
endif (UNIX)

message(STATUS "- OS Name     ${CMAKE_OS_NAME}")
message(STATUS "- OS Version  ${CMAKE_OS_VERSION}")
