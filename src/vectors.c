/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: vectors.c
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
#include "vectors.h"


int vectors_primary(double x, double y, double z){
  
  int retval;
  double norm;
  gsl_vector *n;
  
  /* Normalize the components, in case this was not done previously */  
  norm = hypot3(x, y, z);
  
  /* Create a vector using absolute values only, since we care only about
     which axis is primary, not +/-. */
  n = gsl_vector_alloc(3);           // Make norm vector
  gsl_vector_set(n,0,fabs(x/norm));  // Set fabs(x)
  gsl_vector_set(n,1,fabs(y/norm));  // Set fabs(y)
  gsl_vector_set(n,2,fabs(z/norm));  // Set fabs(z)
  
  /* Set optic.nhat based on which element is largest */
  switch(gsl_vector_max_index(n)){
  case 0:
    retval = NHAT_X;
    break; 
  case 1:
    retval = NHAT_Y;
    break; 
  case 2:
    retval = NHAT_Z;
    break; 
  default:
    printf("This should never run.  Take a shot of gin.\n");
    retval = -1;       // Fail code
  }
  
  /* Free the GSL vector */
  gsl_vector_free(n);
  
  return retval;
}
