#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([scopedesign], [0.1.1], [tbowers@email.arizona.edu])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB          dnl Needed by ARGTABLE
AC_PROG_LN_S

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h unistd.h getopt.h limits.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt hypot])
AC_CHECK_FUNCS([memset regcomp])  dnl Needed by ARGTABLE
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_CHECK_FUNCS([bzero strchr strcspn strrchr strtol])
AC_CHECK_FUNC(getopt_long, SYS_GETOPTLONG=1, SYS_GETOPTLONG=0)
AC_CHECK_FUNC(regcomp,     SYS_REGEX=1,      SYS_REGEX=0)
AC_CHECK_FUNC(strptime,    SYS_STRPTIME=1,   SYS_STRPTIME=0)

# Define automake conditionals
AM_CONDITIONAL(USE_SYS_GETOPTLONG, test "$SYS_GETOPTLONG" = "1")
AM_CONDITIONAL(USE_ARGREX,         test "$SYS_REGEX" = "1")
AM_CONDITIONAL(USE_ARGDATE,        test "$SYS_STRPTIME" = "1")


# Checks for command line options
AC_ARG_ENABLE([async-exec],
  [AS_HELP_STRING([--disable-async-exec], 
    [disable asynchronous execution @<:@default: no@:>@])],
  [async_exec=${enableval}],
  [async_exec=yes])

if test "x${async_exec}" = xyes; then
  AC_MSG_RESULT( Checking PTHREADS Library ==============================)
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

AC_ARG_ENABLE([need-fft],
  [AS_HELP_STRING([--enable-fft], 
    [enable FFT libraries @<:@default: no@:>@])],
  [need_fft=${enableval}],
  [need_fft=no])


dnl =============================================================
# Check for GSL (code from the GSL manual)
AC_MSG_RESULT( Checking GSL Library ===================================)
AC_CHECK_LIB([m],[cos])
AC_CHECK_LIB([gslcblas],[cblas_dgemm])
AC_CHECK_LIB([gsl],[gsl_blas_dgemm])

dnl =============================================================
dnl Checking Code from Alex's pofd_fitter
dnl =============================================================
# Check for FFTW library and headers
if test "x${need_fft}" = xyes; then
  AC_LANG_C
     
  AC_ARG_WITH(fftwinc,
          [  --with-fftwinc=VAL       VAL=-IVAL, has fftw3 include files ],
          [fftwinc="$withval/" CPPFLAGS="$CPPFLAGS -I$fftwinc"],
          [fftwinc=] )
  AC_ARG_WITH(fftwlib, 
              [  --with-fftwlib=DIR      DIR=-LDIR, has libfftw3 ],
              fftwlib="$withval/",fftwlib= )
  
  AC_MSG_RESULT( Checking FFTW3 Library =================================)
  
  # Check for FFTW library first
  if test $fftwlib; then
    AC_MSG_RESULT( Checking User specified FFTW3 ==========================)
    if test $rpathset; then
       LDFLAGS="$LDFLAGS -L$fftwlib -Wl,--rpath -Wl,$fftwlib"
    else
       LDFLAGS="$LDFLAGS -L$fftwlib"
    fi
    LIBS="$LIBS -lfftw3"
    AC_TRY_LINK( [#include<fftw3.h>],
                 [fftw_plan plan;],
                 [echo "Found libFFTW3"],
                 [AC_MSG_ERROR("FFTW3 library not found in user specified location")])
  else
    AC_CHECK_LIB( fftw3, main, ,
        AC_MSG_ERROR("libfftw3 not found; use --with-fftwlib"), [-lm -lfftw3] )
  fi
  
  # Check for FFTW header next
  AC_CHECK_HEADERS([fftw3.h],,
    [ if test $fftwinc; then
     AC_MSG_ERROR("fftw3 headers not in location specified by --with-fftwinc")
     elif test $fftwlib; then
      AC_CHECK_HEADER($fftwlib/../include/fftw3.h,
       [fftwinc="$fftwlib/../include/" CPPFLAGS="$CPPFLAGS -I$fftwinc"],
        AC_MSG_ERROR("Can't find fftw3 includes in user specified location; use --
  with-fftwinc"))
      else
        AC_MSG_ERROR("Can't find fftw3 includes; use --with-fftwinc")
     fi ] )
fi

dnl =============================================================
# Check for CFITSIO library and headers
AC_LANG_C

AC_ARG_WITH( cfitsiolib, 
        [  --with-cfitsiolib=DIR   DIR=-LDIR, has libcfitsio.a],
        cfitsiolib="$withval/" , cfitsiolib= )
if test $cfitsiolib; then
  if test $rpathset; then
          LDFLAGS="$LDFLAGS -L$cfitsiolib -Wl,--rpath -Wl,$cfitsiolib"
  else
       LDFLAGS="$LDFLAGS -L$cfitsiolib"
  fi
fi

AC_MSG_RESULT( Checking CFITSIO Library ===============================)

# Check for CFITSIO library first
AC_CHECK_LIB(cfitsio,main,,
         [AC_MSG_ERROR("Can't find libcfitsio; use --with-cfitsiolib")], 
            [-lm "-L$cfitsiolib"])

# Check for CFITSIO header next
AC_ARG_WITH(cfitsioinc,
     [  --with-cfitsioinc=VAL   VAL=-IVAL, has cfitsio include files ],
        [cfitsioinc="$withval/" CPPFLAGS="$CPPFLAGS -I$cfitsioinc"],
     [cfitsioinc=] )
AC_CHECK_HEADER(fitsio.h,,
  [ if test $cfitsioinc; then
   AC_MSG_ERROR("cfitsio headers not in location specified by --with-cfitsioinc")
   elif test $cfitsiolib; then
    AC_CHECK_HEADER($cfitsiolib/../include/fitsio.h,
     [cfitsioinc="$cfitsiolib/../include/" CPPFLAGS="$CPPFLAGS -I$cfitsioinc"],
      AC_MSG_ERROR("Can't find cfitsio includes; use --with-cfitsioinc"))
    else
      AC_MSG_ERROR("Can't find cfitsio includes; use --with-cfitsioinc")
   fi ] )


dnl =============================================================
# Check for TPEB library and headers
AC_LANG_C

AC_ARG_WITH( tpeblib, 
        [  --with-tpeblib=DIR   DIR=-LDIR, has libtpeb.a],
        tpeblib="$withval/" , tpeblib= )
if test $tpeblib; then
  if test $rpathset; then
          LDFLAGS="$LDFLAGS -L$tpeblib -Wl,--rpath -Wl,$tpeblib"
  else
       LDFLAGS="$LDFLAGS -L$tpeblib"
  fi
fi
AC_MSG_RESULT( Checking TPEB Library ==================================)

# Check for TPEB library first
AC_CHECK_LIB(tpeb,main,,
         [AC_MSG_ERROR("Can't find libtpeb; use --with-tpeblib")], 
            [-lm "-L$tpeblib"])

# Check for TPEB header next
AC_ARG_WITH(tpebinc,
     [  --with-tpebinc=VAL   VAL=-IVAL, has tpeb include files ],
        [tpebinc="$withval/" CPPFLAGS="$CPPFLAGS -I$tpebinc"],
     [tpebinc=] )
AC_CHECK_HEADER(tpeb.h,,
  [ if test $tpebinc; then
   AC_MSG_ERROR("tpeb headers not in location specified by --with-tpebinc")
   elif test $tpeblib; then
    AC_CHECK_HEADER($tpeblib/../include/tpeb.h,
     [tpebinc="$tpeblib/../include/" CPPFLAGS="$CPPFLAGS -I$tpebinc"],
      AC_MSG_ERROR("Can't find tpeb includes; use --with-tpebinc"))
    else
      AC_MSG_ERROR("Can't find tpeb includes; use --with-tpebinc")
   fi ] )

dnl =============================================================


dnl =============================================================
# Check for XPA library and headers
AC_LANG_C

AC_ARG_WITH( xpalib, 
        [  --with-xpalib=DIR   DIR=-LDIR, has libxpa.a],
        xpalib="$withval/" , xpalib= )
if test $xpalib; then
  if test $rpathset; then
          LDFLAGS="$LDFLAGS -L$xpalib -Wl,--rpath -Wl,$xpalib"
  else
       LDFLAGS="$LDFLAGS -L$xpalib"
  fi
fi
AC_MSG_RESULT( Checking XPA Library ==================================)

# Check for XPA library first
AC_CHECK_LIB(xpa,main,,
         [AC_MSG_ERROR("Can't find libxpa; use --with-xpalib")], 
            [-lm "-L$xpalib"])

# Check for XPA header next
AC_ARG_WITH(xpainc,
     [  --with-xpainc=VAL   VAL=-IVAL, has xpa include files ],
        [xpainc="$withval/" CPPFLAGS="$CPPFLAGS -I$xpainc"],
     [xpainc=] )
AC_CHECK_HEADER(xpa.h,,
  [ if test $xpainc; then
   AC_MSG_ERROR("xpa headers not in location specified by --with-xpainc")
   elif test $xpalib; then
    AC_CHECK_HEADER($xpalib/../include/xpa.h,
     [xpainc="$xpalib/../include/" CPPFLAGS="$CPPFLAGS -I$xpainc"],
      AC_MSG_ERROR("Can't find xpa includes; use --with-xpainc"))
    else
      AC_MSG_ERROR("Can't find xpa includes; use --with-xpainc")
   fi ] )

dnl =============================================================



# Check for installed DS9 for display of FITS images produced by the program
# If no DS9 found, then exit configuration
AC_MSG_RESULT( Checking for a FITS display utility ====================)
AC_PATH_PROG([ds9_path],[ds9],[no]) 
if test "x${ds9_path}" = xno; then
   echo         "--------------------------------------------------"
   AC_MSG_ERROR("The FITS viewer DS9 not found!  Install DS9 first!")
else
   AC_DEFINE_UNQUOTED([DS9_PATH],["$ds9_path"],[Callable ds9 Path])	
fi

dnl === Lifted from the nightfall OSS package
# Make sure data directory is included in the config.h file!
mydataroot="${datadir}/${PACKAGE}"
AC_SUBST(mydataroot)
my_save_prefix="${prefix}"
if test "X$prefix" = "XNONE"; then
    prefix="$ac_default_prefix"
else
    prefix="$prefix"
fi
PREFIX=`eval echo ${prefix}` ;          PREFIX=`eval echo ${PREFIX}`
AC_DEFINE_UNQUOTED([PREFIX], ["$PREFIX"], [Installation prefix])
DATADIR=`eval echo ${mydataroot}` ;     DATADIR=`eval echo ${DATADIR}`
AC_DEFINE_UNQUOTED([DATADIR], ["$DATADIR"], [Installation datadir])
prefix="${my_save_prefix}"
dnl === Lifted from the nightfall OSS package


# These are the Makefiles that need to be built!
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 libargtable/Makefile
		 data/Makefile])
AC_OUTPUT

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'

 Package features:
   Async Execution: ${async_exec}
   Use of FFT Libraries: ${need_fft}

 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything

--------------------------------------------------"
