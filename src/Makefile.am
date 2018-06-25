bin_PROGRAMS = scopedesign
scopedesign_SOURCES = main.c sd_defs.h init.c init.h rays.c rays.h \
	images.c images.h mirrors.c mirrors.h setup.c setup.h \
	display.c display.h vectors.c vectors.h demo.c demo.h ui.c ui.h \
	fitswrap.c fitswrap.h

scopedesign_CPPFLAGS = -I$(top_srcdir)/libargtable -I$(top_srcdir)/libxpa $(GTK_CFLAGS)
scopedesign_LDADD = ../libargtable/libargtable2.a ../libxpa/libxpa.a $(GTK_LIBS)
