#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([scopedesign], [0.2.2], [tbowers@email.arizona.edu])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

# save LDFLAGS
XLDFLAGS="$LDFLAGS"

AC_EXEEXT
if test x"${EXEEXT}" = "xno"; then
  EXEEXT=""
fi

# Check for required programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB          dnl Needed by ARGTABLE, XPA


# Checks for header files.
dnl ScopeDesign requirements
AC_HEADER_STDC                dnl Standard Basic C Headers
AC_CHECK_HEADERS([getopt.h sys/ioctl.h])

dnl LIBARGTABLE requirements
AC_CHECK_HEADERS([libintl.h limits.h])

dnl LIBXPA requirements
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netdb.h netinet/in.h pwd.h setjmp.h sys/un.h sys/socket.h sys/time.h])



# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_STRUCT_TM 
AC_CHECK_SIZEOF(long)       dnl == Needed by XPA
SZ_LONG=$ac_cv_sizeof_long
AC_SUBST(SZ_LONG)
AC_CHECK_TYPES([socklen_t], [], [], [#include <sys/socket.h>])



# Checks for library functions.
dnl ScopeDesign requirements
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt strdup])



dnl LIBARGTABLE requirements
AC_CHECK_FUNCS([bzero memset regcomp strchr strcspn strrchr strtol])
AC_CHECK_FUNC(getopt_long, SYS_GETOPTLONG=1, SYS_GETOPTLONG=0)
AC_FUNC_STRTOD

dnl LIBXPA requirements
AC_CHECK_FUNCS([alarm atexit dup2 getcwd gethostname inet_ntoa memmove putenv setenv strcasecmp strncasecmp strpbrk strstr])
AC_CHECK_FUNCS(gethostbyname)
if test $ac_cv_func_gethostbyname = no; then
  AC_CHECK_LIB(nsl, gethostbyname, EXTRA_LIBS="$EXTRA_LIBS -lnsl")
fi                                                                          
AC_FUNC_FORK
AC_FUNC_REALLOC


dnl === Checks of unknown origin or utility...
dnl AC_FUNC_STRFTIME
dnl AC_CHECK_FUNC(regcomp,     SYS_REGEX=1,      SYS_REGEX=0)
dnl AC_CHECK_FUNC(strptime,    SYS_STRPTIME=1,   SYS_STRPTIME=0)
dnl AC_CHECK_FUNCS(strchr memcpy snprintf atexit setenv)
dnl AC_CHECK_FUNCS(connect)
dnl if test $ac_cv_func_connect = no; then
dnl   AC_CHECK_LIB(socket, connect,  EXTRA_LIBS="$EXTRA_LIBS -lsocket")
dnl fi                                                                          




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
AC_MSG_RESULT( Checking other libraries ===============================)


dnl ===============================================================
dnl BEGIN XPA-specific code
AC_MSG_CHECKING(for threaded xpans)
AC_ARG_ENABLE(threaded-xpans, [  --enable-threaded-xpans    build threaded xpans],
    [fun_ok=$enableval], [fun_ok=no])
if test "$fun_ok" = "yes"; then
  AC_MSG_RESULT($fun_ok)
  AC_CHECK_LIB(pthread, pthread_create, have_pthread=yes)
  if test  x"${have_pthread}" = x"yes"; then
    AC_DEFINE([HAVE_LIBPTHREAD],[1],[libpthread])
    AC_DEFINE([_REENTRANT],[1],[reentrant])
    TLIB="-lpthread"
  else
    AC_MSG_ERROR([no threads found ... can't use enable-threaded-xpans], 1)
  fi
else
  AC_MSG_RESULT($fun_ok)
fi
AC_SUBST(TLIB)

AC_MSG_CHECKING(for shared library build)
AC_ARG_ENABLE(shared, [  --enable-shared    build shared libraries],
    [fun_ok=$enableval], [fun_ok=no])
if test "$fun_ok" != "no"; then
  fpic="yes"
  DOSHARED=shlib
  AC_SUBST(DOSHARED)
  if test "$fun_ok" = "link"; then
    LLIB="-L. -l$PACKAGE_NAME"
  else
    LLIB='$(LIB)'
  fi
else
  DOSHARED=""
  LLIB='$(LIB)'
fi
AC_SUBST(LLIB)
AC_MSG_RESULT($fun_ok)

AC_MSG_CHECKING(for request to use posix_spawn)
AC_ARG_ENABLE(posix_spawn, [  --enable-posix_spawn    use posix_spawn() if available],
    [fun_ok=$enableval], [fun_ok=no])
AC_MSG_RESULT($fun_ok)
if test "$fun_ok" = "yes"; then
  AC_CHECK_FUNCS(posix_spawn _NSGetEnviron)
  AC_CHECK_HEADERS(crt_externs.h)
fi

AC_PATH_XTRA
if test x"${have_x}" = "xyes"; then
    AC_DEFINE([HAVE_XT],[1],[Define to 1 if the system has XT])
fi

SC_PATH_TCLCONFIG
if test x"${no_tcl}" = x ; then
    SC_LOAD_TCLCONFIG
    if test -r $TCL_PREFIX/include/tcl.h; then
      TCL_CFLAGS="$TCL_INCLUDE_SPEC"
      if test x"$DOSHARED" != x -a x"$TCL_SUPPORTS_STUBS" = x1; then
        TCL_LIBS="$TCL_STUB_LIB_SPEC"
        TCL_CFLAGS="$TCL_CFLAGS -DUSE_TCL_STUBS=1"
        AC_MSG_RESULT([Tcl support will utilize stubs library: $TCL_LIBS])
      else
        TCL_LIBS="$TCL_LIB_SPEC"
        AC_MSG_RESULT([Tcl support will utilize library: $TCL_LIBS])
      fi
      AC_DEFINE([HAVE_TCL],[1],[Define to 1 if the system has Tcl])
    else
      if test x"${with_tclconfig}" != x ; then
        TCL_CFLAGS="$TCL_INCLUDE_SPEC"
        TCL_LIBS="$TCL_LIB_SPEC"
        AC_DEFINE([HAVE_TCL],[1],[Define to 1 if the system has Tcl])
        AC_MSG_RESULT([warning: tcl.h not found with --with-tcl ... tcl build might fail])
      else
        AC_MSG_RESULT([$TCL_PREFIX/include/tcl.h not found ... use --with-tcl to build tcl explicitly])
      fi
    fi
fi
AC_SUBST(TCL_CFLAGS)
AC_SUBST(TCL_LIBS)

AC_MSG_CHECKING(for incorporation of thread support)
AC_ARG_WITH(threads,
    [  --with-threads          build for use in threaded programs], thr=1, thr=0)
if test x"$thr" = x1 ; then
  AC_MSG_RESULT(yes)
  CFLAGS="$CFLAGS -D_REENTRANT"
else
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(for gtk)
AC_ARG_WITH(gtk,
    [  --with-gtk=<path>       include directory for gtk e.g. /usr/include/gtk-1.2], havelib=1, havelib=0)
if test x"$havelib" = x1 ; then
  AC_MSG_RESULT(yes ($withval))
  GTK_CFLAGS="`pkg-config gtk+-2.0 --cflags` -DHAVE_GTK=1"
  GTK_LIBS="`pkg-config gtk+-2.0 --libs`"
  AC_DEFINE([HAVE_GTK],[1],[Define to 1 is the system has GTK])
else
  GTK_CFLAGS=""
  GTK_LIBS=""
  AC_MSG_RESULT(no)
fi
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# restore LDFLAGS
LDFLAGS="$LDFLAGS $XLDFLAGS"

AC_MSG_CHECKING([$host_os configuration])
case $host_os in
    *cygwin*|*Cygwin* )
        AC_DEFINE([HAVE_CYGWIN],[1],[Define to 1 is the system IS cygwin])
        AC_MSG_RESULT([flagging Cygwin])
        ;;
    *mingw32*|*Mingw32*)
        CFLAGS="$CFLAGS -mconsole"
	EXTRA_LIBS="$EXTRA_LIBS -lwsock32"
        AC_DEFINE([HAVE_MINGW32],[1],[Define to 1 if the system IS mingw32])
        AC_MSG_RESULT([flagging MinGW])
        ;;
    *darwin*|*Darwin*)
	LDFLAGS="$LDFLAGS $CFLAGS"
        G=`$CC -v 2>&1 | grep version | awk '{print $3}' | awk -F. '{print $1$2}'`
        if test x"$G" != x -a "$G" -lt 42; then
	  CFLAGS="$CFLAGS -no-cpp-precomp"
        fi
        if test x"$TCL_PREFIX" = x"/usr/local"; then
	  TCL_CFLAGS=""
          AC_MSG_RESULT([removing -I/usr/local/include])
        fi
	;;
    *osf*|*Osf*)
	  AC_CHECK_LIB(db, snprintf, EXTRA_LIBS="$EXTRA_LIBS -ldb")
        ;;
    * )
        if test x"$fpic" = x"yes" ; then
          if test "$CC" = "gcc" -o `$CC -v 2>&1 | grep -c gcc` != "0" ; then
	    CFLAGS="$CFLAGS -fPIC"
	    AC_MSG_RESULT([adding -fPIC to gcc])
	  else
            AC_MSG_RESULT(none)
	  fi
	else
            AC_MSG_RESULT(none)
	fi
        ;;
esac


AC_SUBST(EXTRA_LIBS)
dnl END XPA-specific code
dnl ===============================================================






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
                 libxpa/Makefile
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
