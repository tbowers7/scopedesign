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

#if HAVE_SYSINFO
# include <sys/sysinfo.h>
#endif

/* Local headers */
#include "init.h"


int init_set_nrays(void){
  
#if HAVE_SYSINFO
    struct sysinfo s;
    sysinfo(&s);
#endif
  
  N_RAYS = 1e7;
  
  return 0;
}
