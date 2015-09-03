CFLAGS+=-I/sw/include
CFLAGS+=-L/sw/lib
#CFLAGS+=-g
#CFLAGS+=-lfftw3
#CFLAGS+=-v
CFLAGS+= -lm
CFLAGS+= -lgsl
#CFLAGS+= -lcfitsio
#CFLAGS+= -DHAVE_INLINE
#CFLAGS+= -lgslcblas
CFLAGS+= -largtable2
CFLAGS+= -Wall
FIO = /d1/library/fileio.o
RDF = /d1/library/read_dat_files.o
MCF = /d1/library/myfitsio.o
IMC = /d1/library/image_combine.o
IMS = /d1/library/image_statistics.o
IMU = /d1/library/image_utils.o
SF  = /d1/library/stellar_fit.o
AST = /d1/library/astrometry.o
#NEV = /d1/library/neville.o
CC = gcc-4

ray_trace: ray_trace.c ray_funcs.c
	$(CC) $(CFLAGS) -o ray_trace ray_trace.c ray_funcs.c $(FIO)
