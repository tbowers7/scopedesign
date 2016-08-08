/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: main.c
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>
#include <argtable2.h>

#include "sd_defs.h"
#include <config.h>

/* Test Code */
#include "rays.h"
#include "illumination.h"
#include "setup.h"
/* Test Code */

void print_usage();                    // Delaration for print_usage() function




/* Test code from the JUPITER project */
//
//
//

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_PTHREAD_H
# include <pthread.h>
#endif

static void * print_it(void * data)
{
	printf("Hello from %s!\n", (char *)data);
	return 0;
}
	
//
//
//
/* Test code from the JUPITER project */


/* Try to open DS9 in a separate thread */
static void * open_ds9(void * command)
{
  
  printf("%s\n",command);
  system(command);                  // Execute the command
  
  
}

/* ================= */
/*   MAIN FUNCTION   */
/* ================= */

int main(int argc, char *argv[])
{
  
  /* Test the execution of ds9 */
  printf("From config.h, path to ds9: %s\n",DS9_PATH);
  char command[500];
  sprintf(command,"%s %s/new-image.fits -dsssao frame new -frame lock wcs -frame first",DS9_PATH,DATADIR);   // Place the command into the varaible
  pthread_t tid1;
  pthread_create(&tid1, 0, open_ds9, command);	
  
  /* Variable Declarations */
  int wfp_stat=0,ir_stat=0;               // Status variables
  
  
  
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
      goto exit;
    }
  
  
 exit:
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
  
  return exitcode;
  
  /*
    

 
  /* TEST CODE */
  ir_stat  = rays_initialize();
  //  wfp_stat = write_focal_plane();
  
  printf("Everything's fine!  %d %d\n",ir_stat,wfp_stat);
  /* TEST CODE */
  
  
  
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
  
  
  /* Make test optic */
  int ss=0;
  scope_optic optic;
  
  optic.nx = 0.0;
  optic.ny = 0.0;
  optic.nz = -1.0;
  
  ss = setup_orient_optic(&optic);
  /* Add error check for status = -1 */
  printf("Value of status, NHAT: %d, %d\n",ss,optic.nhat);
  
  
  
  
  
  /* Test code from the JUPITER project */
  
  pthread_t tid;
  pthread_create(&tid, 0, print_it, argv[0]);	
  pthread_join(tid, 0);
  printf("We went down path #1.\n");
  
  /* Test code from the JUPITER project */
  
  
  
  
  pthread_join(tid1, 0);  // Join back the ds9 thread before quit.

  return 0;
}



/* prints out usage information if command line arguments are not correct */
void print_usage(){
  
  printf("\nscopedesign:\n");
  printf("Ray trace program for Instrumentation (ASTR 5760, F'09).  Traces\n");
  printf("rays through a Cassegrain Telescope / Corrected Rowland Circle\n");
  printf("Spectrograph.\n");
  printf("     Uses GSL, CFITSIO, and ARGTABLE libraries.\n");
  printf("\n");
  printf("usage: scopedesign\n");
  
  return;
}
