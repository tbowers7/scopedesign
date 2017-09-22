/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: init.c
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

#define wombat                         // wombat == protect on N_RAYS
#include "sd_defs.h"                   // Main Package Header
#undef wombat

/* Include packages */
#include <stdio.h>
#include <gsl/gsl_math.h>

#if HAVE__LINUX                        // Linux system information
# include <sys/sysinfo.h>
#endif

#if HAVE__MAC                          // Mac system information
# include <sys/types.h>
# include <sys/sysctl.h>
#endif

/* Local headers */
#include "init.h"
#include "rays.h"


int init_get_sysinfo(void){
  
  /* Pull information based on system type */
#if HAVE__LINUX
  struct sysinfo s;
  sysinfo(&s);
  SYS_RAM = (double)s.totalram / 1024. / 1024.;  // in MB
  
#elif HAVE__MAC
  size_t size;
  unsigned long long memsize;
  sysctlbyname("HW_MEMSIZE", &memsize, &size, NULL, 0);
  SYS_RAM = (double)memsize / 1024. / 1024.;   // In MB
  
#else
  SYS_RAM = 0;
  
#endif
  
  return 0;
}


int init_set_nrays(void){
  
  /* Variable Declarations */
  size_t raysize;
  int memsize;
  
  /* Check against system type -- if SYS_RAM == 0, default to set value */
  if(SD_SYS == 52){
    N_RAYS = 1e7;
    return 1;
  }
  
  /*********************************************************/
  /* We want the RAM used by a set of rays to be 4GB or    */
  /* 1/4 of the total system memory, whichever is smaller. */
  /*********************************************************/
  
  /* Determine amount of "usable" memory, given dictum */
  memsize = GSL_MIN_INT( 4096, (int)floor((float)SYS_RAM / 4.) );
  
  /* Get size of a single ray */
  raysize = sizeof(scope_ray);
  
  /* Compute number of rays that will fit within the RAM */
  N_RAYS = (unsigned int) floor( (double)memsize * 1024. * 1024. / 
				 (double)raysize );
  
  return 0;
}
