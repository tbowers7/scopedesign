#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([scopedesign], [0.1.0], [tbowers@email.arizona.edu])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for command line options
AC_ARG_ENABLE([async-exec],
  [AS_HELP_STRING([--disable-async-exec], 
    [disable asynchronous execution @<:@default: no@:>@])],
  [async_exec=${enableval}],
  [async_exec=yes])

if test "x${async_exec}" = xyes; then
  have_pthreads=no
  AC_SEARCH_LIBS([pthread_create], [pthread], 
    [have_pthreads=yes])

  if test "x${have_pthreads}" = xyes; then
    AC_CHECK_HEADERS([pthread.h], [], 
      [have_pthreads=no])
  fi

  if test "x${have_pthreads}" = xno; then
    echo "---------------------------------------"
    echo "Unable to find pthreads on this system."
    echo "Building a single-threaded version.    "
    echo "---------------------------------------"
    async_exec=no
  fi
fi

if test "x${async_exec}" = xyes; then
  AC_DEFINE([ASYNC_EXEC], 1, [async exec enabled])
fi

# Check for GSL_RNG
AC_SEARCH_LIBS([gsl_rng_env_setup],[gsl],[have_gsl_rng=yes])
if test "x${have_gsl_rng}" = xyes; then
   AC_CHECK_HEADERS([gsl/gsl_rng.h],[],[have_gsl_rng=no])
fi

# Check for CFITSIO
AC_SEARCH_LIBS([fits_open_file],[cfitsio],[have_fitsio=yes],[have_fitsio=no])
if test "x${have_fitsio}" = xyes; then
   AC_CHECK_HEADERS([fitsio.h],[],[have_fitsio=no])
fi
if test "x${have_fitsio}" = xno; then
   echo "No FITSIO found!"
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.


# Check for installed DS9 for display of FITS images produced by the program
AC_CHECK_PROG([have_ds9],[ds9],[yes],[no])
if test "x${have_ds9}" = xno; then
   echo "No DS9 found!"
fi

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'

 Package features:
   Async Execution: ${async_exec}

 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything

--------------------------------------------------"
