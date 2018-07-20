/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: main.c
 * 
 * Copyright (C) 2016-2018  Timothy P. Ellsworth Bowers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define wombat extern                  // wombat == protect on N_RAYS
#include "sd_defs.h"                   // Main Package Header
#undef wombat

/* Include packages */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <string.h>
//#include <math.h>
#include <argtable2.h>
#if HAVE_PTHREAD_H
# include <pthread.h>
#endif


/* Local headers */
/* Test Code */
#include "init.h"
#include "rays.h"
#include "images.h"
#include "setup.h"
#include "display.h"
#include "ui.h"

/* Test Code */


/* Define the argument structure containing the relevant information... */
typedef struct{
  int i;
} args;




/* Declare functions to be consumed here only */
static void  print_usage();            // Delaration for print_usage() function
static void *print_it(void *data);     // print_it from the Jupiter project
static void  parse_argtable(int argc, char *argv[]);


/* ================= */
/*   MAIN FUNCTION   */
/* ================= */

int main(int argc, char *argv[])
{
  
  /* Variable Declarations */
  int           i,wfp_stat=0,ir_stat=0;               // Status variables
  scope_ray    *rays;
  double        over;
  char         *fn_startpos,*fn_startang;
  scope_display display_str;
  
  
  /* TEST ARGTABLE */
  parse_argtable(argc, argv);
  
  /************ CODE OUTLINE ************/
  /* 
     1.  Set up the geometry of the telescope (optic location, shape, etc.)
     2.  Set up the illumination environment (point source, flat, image, etc.)
     3.  Initialize the rays based on illumination environment.
     4.  Propagate the rays through the system.
     4a. Check if rays hit obstructing elements and/or connect with next element
     4b. Reflect / Refract ray off / through element.
     4c. Propagate ray to next element, repeating 4a-4c.
     5.  At each element / pupil, record illumination pattern, if requested.
     6.  Allow for adustment of #1 or #2 and re-run of program.
     7.  Other?
  */
  
  /* Initialize system type and check available RAM */
  init_get_sysinfo();
  
  /* Display a splash screen! */
  display_splash(0);
#if HAVE_GTK3
  ui_example_window();
#endif
  
  /* Initialize N_RAYS */
  init_set_nrays();
  
  
  /* Open DS9 in a separate thread while the code computes the geometry and
     initializes the gazillion rays needed. */
  int       ds9stat = DS9_WHATEVER; //DS9_CANIBALIZE;
  pthread_t tid_ds9;
  pthread_create(&tid_ds9, 0, display_ds9_open, &ds9stat);
  
  
  
  /* Set up the telescope geometry */
  int            sval,nelem;
  scope_scope    telescope;
  scope_element *elements;
  
  sval = setup_initialize_geometry(&telescope,elements,&nelem);
  printf("Number of elements rays must interact with: %d\n",nelem);
  
  
  /* Set up the illumination environment */
  sval = setup_initialize_illumination(TARGET_POINT);
  
  
  
  
  /* Initialize the rays, and write out FITS containing:
     starting positions
     starting angles */
  rays = rays_initialize(TARGET_POINT, &ir_stat, &over);
  
  printf("N_RAYS = %u\n",N_RAYS);
  
  
  printf("Ray status = %d, Overshoot = %0.3f, Theory = %0.3f\n",
	 ir_stat,over,4./M_PI);
  
  /* Write out initial position and angles */
  fn_startpos = images_write_pos(rays, OPTIC_INF, telescope.name,
				 &wfp_stat);
  printf("File location and status: %s %d\n",fn_startpos, wfp_stat);
  fn_startang = images_write_ang(rays, OPTIC_INF, telescope.name,
				 &wfp_stat);
  printf("File location and status: %s %d\n",fn_startang, wfp_stat);
  
    
  /* Display ray starting location in the DS9 window */
  display_ds9_talk(DS9_GET, &display_str);
  
  
  
  /* Invent FOR loop here to loop through optical elements that
     rays must interact with, extracted from the elements structure */
  
  
  
  
  
  
  printf("Memory check: scope_ray: %d, double: %d, int: %d, bool %d\n",
	 sizeof(scope_ray),sizeof(double),sizeof(int),sizeof(bool));
  printf("Rays: %0.3e\n",sizeof(scope_ray)*(double)N_RAYS);
  
  free(rays);
  free(elements);
  free(fn_startpos);  
  
  /* Rejoin DS9 thread here... */
  printf("Pausing here until the Open_DS9 thread rejoins...\n");
  pthread_join(tid_ds9, 0);

  /* TEST CODE */
  
  
  
  
  
  /* Make test optic */
  int ss=0;
  scope_optic optic;
  double norm;
  
  optic.nx = 0.0;
  optic.ny = 0.0;
  optic.nz = -1.0;
  norm = hypot3(optic.nx,optic.ny,optic.nz);
  
  ss = setup_orient_optic(&optic);
  /* Add error check for status = -1 */
  printf("Value of status, NHAT: %d, %d\n",ss,optic.nhat);
  
  
  /* Make test ray */
  scope_ray mir;
  scope_ray test;
  
  test.vx = 0;
  test.vy = 0;
  test.vz = +1.0;
  mir.vx = optic.nx/norm;
  mir.vy = optic.ny/norm;
  mir.vz = optic.nz/norm;
  
  
  printf("Optic:  %+0.3f %+0.3f %+0.3f %0.3f\n",mir.vx,mir.vy,mir.vz,
	 hypot3(mir.vx,mir.vy,mir.vz));
  printf("Input:  %+0.3f %+0.3f %+0.3f %0.3f\n",test.vx,test.vy,test.vz,
	 hypot3(test.vx,test.vy,test.vz));
  
  rays_reflect(&test, mir);
  
  printf("Output: %+0.3f %+0.3f %+0.3f %0.3f\n",test.vx,test.vy,test.vz,
	 hypot3(test.vx,test.vy,test.vz));
  
  
  // sleep(2);
  
  // display_ds9_talk();
  
  sleep(5);
  
  display_ds9_close();
  
  return 0;
}



/* prints out usage information if command line arguments are not correct */
static void print_usage(){
  
  printf("\nscopedesign:\n");
  printf("Ray trace program for Instrumentation (ASTR 5760, F'09).  Traces\n");
  printf("rays through a Cassegrain Telescope / Corrected Rowland Circle\n");
  printf("Spectrograph.\n");
  printf("     Uses GSL, CFITSIO, and ARGTABLE libraries.\n");
  printf("\n");
  printf("usage: scopedesign\n");
  
  return;
}



/* Simple print statement from Jupiter project */
static void *print_it(void * data)
{
  printf("Hello from %s!\n", (char *)data);
  return 0;
}



static void parse_argtable(int argc, char *argv[]){
  /* TEST ARGTABLE */
  struct arg_lit  *list    = arg_lit0("lL",NULL,                      "list files");
  struct arg_lit  *recurse = arg_lit0("R",NULL,                       "recurse through subdirectories");
  struct arg_int  *repeat  = arg_int0("k","scalar",NULL,              "define scalar value k (default is 3)");
  struct arg_str  *defines = arg_strn("D","define","MACRO",0,argc+2,  "macro definitions");
  struct arg_file *outfile = arg_file0("o",NULL,"<output>",           "output file (default is \"-\")");
  struct arg_lit  *verbose = arg_lit0("v","verbose,debug",            "verbose messages");
  struct arg_lit  *help    = arg_lit0(NULL,"help",                    "print this help and exit");
  struct arg_lit  *version = arg_lit0(NULL,"version",                 "print version information and exit");
  struct arg_file *infiles = arg_filen(NULL,NULL,NULL,1,argc+2,       "input file(s)");
  struct arg_end  *end     = arg_end(20);
  void* argtable[] = {list,recurse,repeat,defines,outfile,verbose,help,version,infiles,end};
  const char* progname = "myprog";
  int nerrors;
  int exitcode=0;
  
  
  /* verify the argtable[] entries were allocated sucessfully */
  if (arg_nullcheck(argtable) != 0)
    {
      /* NULL entries were detected, some allocations must have failed */
      printf("%s: insufficient memory\n",progname);
      exitcode=1;
    }
  
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
  
  // printf("ARGTABLE exit code (really): %d\n",exitcode);

  return;
}
