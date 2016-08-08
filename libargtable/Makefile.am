noinst_LIBRARIES = libargtable2.a

libargtable2_a_SOURCES = argtable2.c arg_end.c arg_rem.c arg_lit.c arg_int.c arg_dbl.c \
			arg_str.c arg_file.c
EXTRA_DIST = AUTHORS COPYING ChangeLog README

noinst_HEADERS = argtable2.h

# If the system provides getopt_long then we use it,
# otherwise we compile in the version supplied with argtable.
if !USE_SYS_GETOPTLONG
libargtable2_a_SOURCES += getopt.c getopt1.c getopt.h
endif

# If the system provides regex then we may compile arg_rex
# otherwise we must omit it.
if USE_ARGREX
libargtable2_a_SOURCES += arg_rex.c
endif

# If the system provides strptime then we may compile arg_date
# otherwise we must omit it.
if USE_ARGDATE
libargtable2_a_SOURCES += arg_date.c
endif


