/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: demo.c
 * 
 * Copyright (C) 2016-2021  Timothy P. Ellsworth Bowers
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
#include <gsl/gsl_math.h>              // Includes Constants

/* Local headers */
#include "demo.h"
#include "setup.h"

/* Have a hard-wired Newtonian telescope as a DEMO, but also for code testing */
void demo_newtonian(scope_scope *telescope,
		    scope_element *elements,
		    int *nelem){

  telescope->name = (char *)malloc(sizeof(char) * 256);
  sprintf(telescope->name,"Newtonian Demo");
  
  /* Set up the primary mirror */
  telescope->primary.type = OPTIC_PARABOLA;
  telescope->primary.dmaj = 10. *(2.54/100.);   // 10" mirror, keep everything in meters
  telescope->primary.dmin = 10. *(2.54/100.);   // 10" mirror, keep everything in meters
  telescope->primary.vmin = 0.;                 // Axially symmetric
  telescope->primary.f    = telescope->primary.dmaj * 6.; // f/6 parabola
  telescope->primary.cx   = 0.;                 // Center mirror at (0,0,0)
  telescope->primary.cy   = 0.;
  telescope->primary.cz   = 0.;
  telescope->primary.nx   = 0.;                 // Point mirror straight up along z-hat
  telescope->primary.ny   = 0.;
  telescope->primary.nz   = 1.;
  telescope->primary.nhat = setup_orient_optic(&telescope->primary);
  
  /* Set up the secondary mirror */
  telescope->secondary.type = OPTIC_PLANE;
  telescope->secondary.dmaj = 2. *(2.54/100.) * M_SQRT2; // 2" plane mirror -- projection
  telescope->secondary.dmin = 2. *(2.54/100.);           // 2" plane mirror -- projection
  telescope->secondary.vmin = NHAT_Y;                    // Minor axis along y-direction
  telescope->secondary.f    = posinf;
  telescope->secondary.cx   = 0.;
  telescope->secondary.cy   = 0.;
  telescope->secondary.cz   = telescope->primary.dmaj * 0.9;       // 90% of the way to focus?
  telescope->secondary.nx   = M_SQRT1_2;                 // (1,0,-1)
  telescope->secondary.ny   = 0.;
  telescope->secondary.nz   = -M_SQRT1_2;
  telescope->secondary.nhat = setup_orient_optic(&telescope->secondary);
  
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
