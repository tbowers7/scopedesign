/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: init.c
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
  int mib[2];
  int64_t physical_memory;
  size_t length;
  mib[0] = CTL_HW;
  mib[1] = HW_MEMSIZE;
  length = sizeof(int64_t);
  sysctl(mib, 2, &physical_memory, &length, NULL, 0);
  SYS_RAM = (double)physical_memory / 1024. / 1024.;   // In MB
  
#else
  SYS_RAM = 0.;
  
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
