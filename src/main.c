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
//#include <argtable2.h>

#include "sddefs.h"

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





int main(int argc, char *argv[])
{

  /* Variable Declarations */
  int wfp_stat=0,ir_stat=0;               // Status variables

  
  ir_stat  = initialize_rays;
  wfp_stat = write_focal_plane;
  
  printf("Everything's fine!\n");
  
  
  /* Test code from the JUPITER project */
  //

  pthread_t tid;
  pthread_create(&tid, 0, print_it, argv[0]);	
  pthread_join(tid, 0);
  printf("We went down path #1.\n");

  //
  /* Test code from the JUPITER project */
  
  
  
  
  
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
