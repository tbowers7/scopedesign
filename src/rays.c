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
#include <math.h>
#include <gsl/gsl_rng.h>               // Includes GSL's rng routine defs
#include <gsl/gsl_math.h>              // Includes the gsl_hypot3() function
#include <gsl/gsl_roots.h>             // Includes GSL's root-finder algorithms



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

double raytrace_free_distance(scope_ray ray, raytrace_geom geom, int surf){
  
  /* Variable declarations */

  /* Variables needed for GSL's root-finder algorithms */
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  double r = 0;
  double x_lo = 0.0, x_hi = 5.0;
  gsl_function F;
  
  raytrace_root_params params = {ray, geom, surf}; 
  
  /* If w/in Rowland Circle, reduce x_hi to 2.1 */
  if(surf == OPTIC_SF) x_hi = 2.1;


  F.function = &raytrace_distroot; 
  F.params = &params;
  
  /* Solve using GSL's Brent's Method Solver */
  //  T = gsl_root_fsolver_brent;
  T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc(T);
  gsl_root_fsolver_set(s, &F, x_lo, x_hi);
  
  
  do{
    iter++;
    status = gsl_root_fsolver_iterate(s);
    r = gsl_root_fsolver_root(s);
    x_lo = gsl_root_fsolver_x_lower(s);
    x_hi = gsl_root_fsolver_x_upper(s);
    status = gsl_root_test_interval(x_lo, x_hi, 0, 1.e-14);
  } while (status == GSL_CONTINUE && iter < max_iter);
  
  gsl_root_fsolver_free (s);
  
  return r;
}


double raytrace_distroot(double t, void *params){

  raytrace_root_params *p = (raytrace_root_params *)params; 

  double x1 = p->ray.x;
  double y1 = p->ray.y;
  double z1 = p->ray.z;

  double xa = p->ray.vx;
  double ya = p->ray.vy;
  double za = p->ray.vz;
  int surf  = p->surf;
  double z2;
   
  raytrace_geom geom = p->geom;
  
  /* Select proper optical surface, based on parameter passed */
  
  switch(surf){
  case(OPTIC_PRI):
    z2 = primary_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  case(OPTIC_SEC):
    z2 = secondary_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  case(OPTIC_FP):
    z2 = focalplane_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
    /*
  case(OPTIC_GRS):
    z2 = sph_grating_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  case(OPTIC_GRT):
    z2 = tor_grating_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  case(OPTIC_SF):
    z2 = detector_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
    */
  }
    
  /* Return condition on root */
  return z1 + t*za - z2;
}


/* Function for advancing rays along path */
void raytrace_advance_ray(scope_ray *beam, double d){
  
  beam->x = beam->x + d * beam->vx;
  beam->y = beam->y + d * beam->vy;
  beam->z = beam->z + d * beam->vz;
  
  return;  
}

/* Wrapper function to find the normal vector of a surface at a given point */
scope_ray raytrace_get_n(scope_ray pos, raytrace_geom geom, int surf){
  
  /* Variable declarations */
  scope_ray n;
  double x = pos.x;
  double y = pos.y;
  double z = pos.z;
  double f = geom.f;
  double v = geom.v;
  double b = geom.b;
  double e = geom.e;
  double esm1 = (e*e - 1.);
  double R = geom.Rrc;
  double alpha = geom.alpha;
  double norm;
  double xyfact;
  double xg,yg,zg;
  double Rt,xz_rad,beta;
  
  /* Select surface, and calculate n */

  /* PRIMARY MIRROR */
  if(surf == OPTIC_PRI){

    /* Normalization */
    norm = sqrt(x*x + y*y + 4.*f*f);
    
    n.x = -x / norm;
    n.y = -y / norm;
    n.z = 2.*f / norm;
    
  /* SECONDARY MIRROR */
  }else if(surf == OPTIC_SEC){
    
    /* Normalization */
    xyfact = (x*x + y*y)/esm1 + (b+f)*(b+f)/(4.*e*e);

    norm = sqrt(esm1*esm1 + (x*x + y*y)/xyfact);

    n.x = -x / sqrt(xyfact) / norm;
    n.y = -y / sqrt(xyfact) / norm;
    n.z = esm1 / norm;
   
  /* FOCAL PLANE */
  }else if(surf == OPTIC_FP){
    
    n.x = 0.0;
    n.y = 0.0;
    n.z = 1.0;
    
    /* SPHERICAL GRATING */
  }else if(surf == OPTIC_GRS){
    
    xg = -R*sin(alpha);
    yg = 0.0;
    zg = -(v+b);
      

    /* Normalization */
    norm = gsl_hypot3(xg-x, yg-y, zg- z);
    
    n.x = (xg - x) / norm;
    n.y = (yg - y) / norm;
    n.z = (zg - z) / norm;    
    
    /* TOROIDAL GRATING */
  }else if(surf == OPTIC_GRT){
    
    xg = -R*sin(alpha);
    yg = 0.0;
    zg = -(v+b);
    
    beta = asin(0.108);                      // Optimize for 1300A & 1900A

    Rt = R*(1. - cos(alpha)*cos(beta));            // R from torus equation

    xz_rad = sqrt((x-xg)*(x-xg) + (z-zg)*(z-zg));  // Simplification

    /* Normal vecotr */
    n.x = -(xg - x) * (Rt - xz_rad) / xz_rad;
    n.y = (yg - y);
    n.z = -(zg - z) * (Rt - xz_rad) / xz_rad;
    
    /* Normalization */
    norm = gsl_hypot3(n.x, n.y, n.z);

    n.x /= norm;
    n.y /= norm;
    n.z /= norm;
    
  }
  
  return n;
}

/* Function to reflect the incoming ray based on i.n = -o.n & ixn = oxn */
void raytrace_reflect(scope_ray *a, scope_ray n){
  
  /* Variable declarations */
  double xap, yap, zap;
  
  /* Calculate zap first */
  zap = (n.x*n.x + n.y*n.y - n.z*n.z)*a->vz - 2.*n.x*n.z*a->vx - 
    2.*n.y*n.z*a->vy;
  
  /* Calculate xap & yap based on zap */
  yap = a->vy + (n.y/n.z)*(zap - a->vz);
  xap = a->vx + (n.x/n.z)*(zap - a->vz);
  
  /* Place updated velocities in ray a */
  a->vx = xap;
  a->vy = yap;
  a->vz = zap;
  
  return;
}


