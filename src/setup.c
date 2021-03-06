/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: setup.c
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
#include <gsl/gsl_vector.h>            // Contains GSL's vector headers

/* Local headers */
#include "setup.h"
#include "vectors.h"
#include "demo.h"


int setup_orient_optic(scope_optic *optic){
  
  int nhat;
  
  /* Compute the primary direction of NHAT */
  nhat = vectors_primary(optic->nx, optic->ny, optic->nz);
  
  /* Test that the vectors routine did not fail */
  if(nhat > 0){
    optic->nhat = nhat;
    return 0;             // Return value = 0 is a good thing
  } else {
    return nhat;          // Return value = -1 is NOT a good thing
  }
}


/* Function to initialize the geometry to be used */
int setup_initialize_geometry(scope_scope *scope,         // Info about elements
			      scope_element *elements,    // Order of impact
			      int *nelem){                // How many impacts?
  
  /* In principle, this function could call an external .txt configuration
     file, or parsed information from the command line, or just about anything.
     
     For now, let's initialize the demo Newtonian telescope and go on!
  */
  
  /* Variable Declarations */
  
  /* Initialize the Newtonian */
  demo_newtonian(scope, elements, nelem);
  
  return 0;
}


/* Function to initialize the illumination environment */
int setup_initialize_illumination(int value){
  
  
  
  
  
  
  
  
  
  return 0;
}
