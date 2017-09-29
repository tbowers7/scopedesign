BASE_SRCS =	xpa.c xpaio.c command.c acl.c remote.c clipboard.c port.c \
		tcp.c client.c word.c xalloc.c find.c xlaunch.c timedconn.c

TCL_SRCS =	tclloop.c tcl.c 

XT_SRCS =	xtloop.c

GTK_SRCS =	gtkloop.c

INCL	=	xpa.h xpap.h xport.h tcp.h word.h xalloc.h find.h xlaunch.h \
		timedconn.h prsetup.h

noinst_LIBRARIES = libxpa.a

libxpa_a_SOURCES = ${BASE_SRCS} ${TCL_SRCS} ${XT_SRCS} ${GTK_SRCS}

EXTRA_DIST = BUGS COPYRIGHT LICENSE README

noinst_HEADERS = ${INCL}

# The program XPANS is needed by DS9 to register port information
# The programs XPASET and XPAGET are useful utilities
bin_PROGRAMS   = xpans xpaset xpaget
xpans_SOURCES  = xpans.c xpap.h
xpaset_SOURCES = xpaset.c xpap.h
xpaget_SOURCES = xpaget.c xpap.h
LDADD          = libxpa.a

