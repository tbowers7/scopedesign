bin_PROGRAMS = scopedesign
scopedesign_SOURCES = main.c sd_defs.h init.c init.h rays.c rays.h \
	illumination.c illumination.h mirrors.c mirrors.h setup.c setup.h \
	display.c display.h vectors.c vectors.h demo.c demo.h

scopedesign_CPPFLAGS = -I$(top_srcdir)/libargtable -I$(top_srcdir)/libxpa
scopedesign_LDADD = ../libargtable/libargtable2.a ../libxpa/libxpa.a
