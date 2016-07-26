/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: rays.c
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

#include "sd_defs.h"

/* Include packages */
#include <gsl/gsl_rng.h>               // Includes GSL's rng routine defs
#include <gsl/gsl_math.h>              // Includes the gsl_hypot3() function




#include "rays.h"

int initialize_rays(){
  
  /* Variable Declarations */
  int i;
  scope_ray *rays,normal,g,det_plane;
  
  printf("We're in rays.c...\n");
  
  rays = (scope_ray *)malloc(N_RAYS * sizeof(scope_ray));
  /* for(i=0;i<N_RAYS;i++){ */
  /*   printf("%d\n",rays[i].lost); */
  /* } */
  
  /* Setup GSL's RNG */
  
  const gsl_rng_type *T;
  gsl_rng *r;
  
  gsl_rng_env_setup();
  T = gsl_rng_ranlux389;
  r = gsl_rng_alloc(T);
  
  for(i=0; i < N_RAYS; i++){
    rays[i].x = 0;
  }
  
  
  /* Clean up */
  gsl_rng_free(r);
  
  return 1;
  
}

