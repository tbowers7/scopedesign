/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: setup.c
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
#include "sd_defs.h"        // Contains header information for scopedesign

#include <stdio.h>
#include <gsl/gsl_vector.h> // Contains GSL's vector headers

int setup_orient_optic(scope_optic *optic){
  
  int status=0;
  double norm;
  gsl_vector *n;
  
  /* Normalize the components, in case this was not done previously */  
  norm = hypot3(optic->nx, optic->ny, optic->nz);
  
  /* Create a vector using absolute values only, since we care only about
     which axis is primary, not +/-. */
  n = gsl_vector_alloc(3);                   // Make norm vector
  gsl_vector_set(n,0,fabs(optic->nx/norm));  // Set fabs(x)
  gsl_vector_set(n,1,fabs(optic->ny/norm));  // Set fabs(y)
  gsl_vector_set(n,2,fabs(optic->nz/norm));  // Set fabs(z)
  
  /* Set optic.nhat based on which element is largest */
  switch(gsl_vector_max_index(n)){
  case 0:
    optic->nhat = NHAT_X;
    break; 
  case 1:
    optic->nhat = NHAT_Y;
    break; 
  case 2:
    optic->nhat = NHAT_Z;
    break; 
  default:
    printf("This should never run.  Take a shot of gin.\n");
    status = -1;       // Fail code
  }
  
  gsl_vector_free(n);  // Free space
  
  return status;
}

