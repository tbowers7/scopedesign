/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: demo.c
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

/* Include packages */
#include "sd_defs.h"                   // Main Package Headers

/* Include packages */
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_math.h>              // Includes Constants

/* Local headers */
#include "demo.h"
#include "setup.h"

/* Have a hard-wired Newtonian telescope as a DEMO, but also for code testing */
void demo_newtonian(scope_optic *primary, 
		    scope_optic *secondary, 
		    scope_element *elements,
		    int *nelem){
  
  /* Set up the primary mirror */
  primary->type = OPTIC_PARABOLA;
  primary->dmaj = 10. *(2.54/100.);   // 10" mirror, keep everything in meters
  primary->dmin = 10. *(2.54/100.);   // 10" mirror, keep everything in meters
  primary->vmin = 0.;                 // Axially symmetric
  primary->f    = primary->dmaj * 6.; // f/6 parabola
  primary->cx   = 0.;                 // Center mirror at (0,0,0)
  primary->cy   = 0.;
  primary->cz   = 0.;
  primary->nx   = 0.;                 // Point mirror straight up along z-hat
  primary->ny   = 0.;
  primary->nz   = 1.;
  primary->nhat = setup_orient_optic(&primary);
  
  /* Set up the secondary mirror */
  secondary->type = OPTIC_PLANE;
  secondary->dmaj = 2. *(2.54/100.) * M_SQRT2; // 2" plane mirror -- projection
  secondary->dmin = 2. *(2.54/100.);           // 2" plane mirror -- projection
  secondary->vmin = NHAT_Y;                    // Minor axis along y-direction
  secondary->f    = posinf;
  secondary->cx   = 0.;
  secondary->cy   = 0.;
  secondary->cz   = primary->dmaj * 0.9;       // 90% of the way to focus?
  secondary->nx   = M_SQRT1_2;                 // (1,0,-1)
  secondary->ny   = 0.;
  secondary->nz   = -M_SQRT1_2;
  secondary->nhat = setup_orient_optic(&secondary);
  
  /* Tell the calling function how many elements light passed by or reflects
     off of before coming to the focal plane */
  *nelem = 3;
  elements = (scope_element *)calloc( *nelem, sizeof(scope_element) );
  
  /* Define elements */
  
  /* Light passes the secondary first, blocking some rays, but no reflections */
  elements[0].elem    = OPTIC_SEC;
  elements[0].block   = true;      // lost=true for those that hit
  elements[0].reflect = false;
  elements[0].refract = false;
  
  /* Light hits the primary next, reflecting those that hit it */
  elements[1].elem    = OPTIC_PRI;
  elements[1].block   = false;     // lost=true for those that miss
  elements[1].reflect = true;
  elements[1].refract = false;
  
  /* Light hits the secondary next, reflecting those that hit it */
  elements[2].elem    = OPTIC_SEC;
  elements[2].block   = false;     // lost=true for those that miss
  elements[2].reflect = true;
  elements[2].refract = false;
  
  return;
}
