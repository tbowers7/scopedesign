/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <argtable2.h>
#include <gsl/gsl_math.h>              // Includes the gsl_hypot3() function
#include <gsl/gsl_rng.h>               // Includes GSL's rng routine defs

void print_usage();                    // Delaration for print_usage() function





int main(int argc, char *argv[])
{


















}



/* prints out usage information if command line arguments are not correct */
void print_usage(){
  
  printf("\nray_trace:\n");
  printf("Ray trace program for Instrumentation (ASTR 5760, F'09).  Traces\n");
  printf("rays through a Cassegrain Telescope / Corrected Rowland Circle\n");
  printf("Spectrograph.\n");
  printf("     Uses GSL, and ARGTABLE libraries.\n");
  printf("\n");
  printf("usage: ray_trace\n");
  
  return;
}
