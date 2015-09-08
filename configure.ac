#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([scopedesign], [0.1], [tbowers7@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
#AC_CONFIG_SRCDIR([raytrace/ray_funcs.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-largtable2':
AC_CHECK_LIB([argtable2], [main])
# FIXME: Replace `main' with a function in `-lgsl':
AC_CHECK_LIB([gsl], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
