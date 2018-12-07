#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

MY_LIBRARY_VERSION="0.8.1"

AC_INIT([Ingescape], [m4_esyscmd_s([builds/m4-scripts/gen-ingescape-version src/admin.c])], [contact@ingenuity.io], [libingescape], [http://www.ingescape.com])
AC_CONFIG_AUX_DIR(autotool-build)
AC_CONFIG_MACRO_DIR(autotool-build)
AC_CONFIG_SRCDIR([src/network.c])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Package version is defined in config.h
PV_MAJOR=`echo $PACKAGE_VERSION | cut -d . -f 1`
PV_MINOR=`echo $PACKAGE_VERSION | cut -d . -f 2`
PV_PATCH=`echo $PACKAGE_VERSION | cut -d . -f 3`


# Set library version format : current:revision:age
# Numbers are transforme by libtools in : (current - age).age.revision.<extention>
#LTVER="8:1:8"
AC_SUBST(LTVER, "$(($PV_MAJOR + $PV_MINOR)):$PV_PATCH:$PV_MINOR")
# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_LIBTOOL_WIN32_DLL

# Checks for libraries.
AC_CHECK_LIB([czmq], [zactor_new])
AC_CHECK_LIB([sodium], [sodium_init])
AC_CHECK_LIB([yajl], [yajl_tree_parse])
AC_CHECK_LIB([zmq], [zmq_ctx_new])
AC_CHECK_LIB([zyre], [zyre_new])

# Checks for header files.
AC_USE_SYSTEM_EXTENSIONS
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Check compiler flags
AC_MSG_CHECKING([whether compiler understands -Wall])
old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],
  AC_MSG_RESULT(yes),
  AC_MSG_RESULT(no)
  CFLAGS="$old_CFLAGS")

AC_MSG_CHECKING([whether compiler understands -Wextra])
old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wextra"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],
  AC_MSG_RESULT(yes),
  AC_MSG_RESULT(no)
  CFLAGS="$old_CFLAGS")

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([gethostname gettimeofday memmove memset socket strdup strerror strndup])

ingescape_for_windows="no"

# OS-specific tests
AC_CANONICAL_HOST
case "${host_os}" in
  *mingw*|*cygwin*)
    CFLAGS="${CFLAGS} -static-libgcc"
    LDFLAGS="${LDFLAGS} -lws2_32 -no-undefined -avoid-version"
    ingescape_for_windows="yes"
    ;;
esac

AM_CONDITIONAL(INGESCAPE_FOR_WINDOWS, test "x$ingescape_for_windows" = "xyes")

# Activate shared and static libraries
LT_INIT([dlopen shared win32-dll])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

# Print configure summary and list make options
AC_DEFUN([AX_SUMMARIZE_ENV],
[
BUILD_USER=${USER}
BUILD_ARCH=${host}
BUILD_HOST=${ac_hostname}
BUILD_DATE=$(date +'%F %H:%M')

AC_DEFINE_UNQUOTED([BUILD_USER],     "${BUILD_USER}",     [The fine user who built the package])
AC_DEFINE_UNQUOTED([BUILD_ARCH],     "${BUILD_ARCH}",     [Architecture of the build host])
AC_DEFINE_UNQUOTED([BUILD_HOST],     "${BUILD_HOST}",     [Build host name])
AC_DEFINE_UNQUOTED([BUILD_DATE],     "${BUILD_DATE}",     [Build date])

AC_SUBST(BUILD_USER)
AC_SUBST(BUILD_ARCH)
AC_SUBST(BUILD_HOST)
AC_SUBST(BUILD_DATE)
AC_SUBST(BUILD_VERSION)
])

AC_DEFUN([AX_SUMMARIZE_CONFIG],
[
echo
echo '##########################################################################'
echo '#                                SUMMARY                                 #'
echo '##########################################################################'
echo
echo Package version............... : $PACKAGE-$VERSION
echo
echo C compiler.................... : $CC
echo C compiler flags.............. : $CFLAGS
echo C Linker flags.................: $LDFLAGS
echo Configure date................ : $BUILD_DATE
echo Build architecture............ : $BUILD_ARCH
echo Build host.................... : $BUILD_HOST
echo Build user.................... : $USER
echo Install dir................... : $prefix
echo
echo '##########################################################################'
echo
echo Configure complete! Now proceed with:
echo "    - 'make'                  compile the project"
echo "    - 'make install'          install the project to $prefix"
echo
])

AX_SUMMARIZE_ENV
AX_SUMMARIZE_CONFIG
