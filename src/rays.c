/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: rays.c
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
#include <math.h>
#include <gsl/gsl_rng.h>               // Includes GSL's rng routine defs
#include <gsl/gsl_roots.h>             // Includes GSL's root-finder algorithms
#include <gsl/gsl_histogram2d.h>       // Includes GSL's 2-D histograms

/* Local headers */
#include "rays.h"
#include "vectors.h"


scope_ray *rays_initialize(int ray_setup, int *ray_status, double *overshoot){
  
  /* Variable Declarations */
  long i,j;
  scope_ray *rays,normal,g,det_plane;
  double angle=0.;
  
  printf("Initializing %0.3e rays...\n",(double)N_RAYS);
  rays = (scope_ray *)malloc(N_RAYS * sizeof(scope_ray));
  *ray_status = 2222;

  /* Initialize ray position randomly across the aperture */ 
  /* Start GSL's RNG */
  const gsl_rng_type *T;
  gsl_rng *r;
  
  gsl_rng_env_setup();
  // T = gsl_rng_ranlux389;  // Second slowest
  T = gsl_rng_taus2;
  r = gsl_rng_alloc(T);
  
  /* Assign random starting point for rays */
  double radius = 1.0;
  double x,y;
  
  for(i=0,j=0;i<N_RAYS;i++,j++){    // j counts the # of times the loop executes
    x = gsl_rng_uniform(r)*2. - 1.;
    y = gsl_rng_uniform(r)*2. - 1.;
    if(x*x + y*y > 1.){             // If outside the circle,
      i--;                          // decriment i,
      continue;                     // and try again.
    }
    
    rays[i].x = x*radius;
    rays[i].y = y*radius;
    rays[i].z = +10.;                                    // Start way up high
    
    rays[i].lost = false;                                // Not lost yet
  }
  *overshoot = (double)j/(double)i;

  
  
  /* Initialize ray direction based on setup criteria */
  switch(ray_setup){
  case(TARGET_POINT):
    printf("Serving up a single point source...\n");
    
    for(i=0;i<N_RAYS;i++){
      rays[i].vx = sin(angle);
      rays[i].vy = 0;
      rays[i].vz = -cos(angle);
    }
    
    
    
    
    
    break;
  case(TARGET_POINTS):
    
    
    
    break;
  case(TARGET_IMAGE):
    
    i=0;
    /* Variable Declaration */
    int stat;
    gsl_histogram2d imghist;
    gsl_histogram2d_pdf imgpdf;
    double x,y;
    
    stat = gsl_histogram2d_pdf_sample(&imgpdf, 
				      gsl_rng_uniform(r), gsl_rng_uniform(r),
				      &x, &y);
    
    rays[i].x = x;
    rays[i].y = y;
    rays[i].z = +10.;
    
    break;
    
  default:
    printf("I am defaulting on ray direction.\n");
  }
  
  
  
  
  
  
  
  /* Clean up */
  gsl_rng_free(r);
  
  
  return rays;
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
  
  scope_root_params params = {ray, geom, surf}; 
  
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

  scope_root_params *p = (scope_root_params *)params; 

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
  
  /*
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
  case(OPTIC_GRS):
    z2 = sph_grating_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  case(OPTIC_GRT):
    z2 = tor_grating_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  case(OPTIC_SF):
    z2 = detector_z(x1 + t*xa, y1 + t*ya, &geom);
    break;
  }
  */
  z2 = 0.;   // Temporary fix to eliminate compiler warnings (2/15/18)
  
  /* Return condition on root */
  return z1 + t*za - z2;
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
  switch(surf){
  case(OPTIC_PRI):
    
    /* Normalization */
    norm = sqrt(x*x + y*y + 4.*f*f);
    n.x = -x / norm;
    n.y = -y / norm;
    n.z = 2.*f / norm;
    break;
    
    /* SECONDARY MIRROR */
  case(OPTIC_SEC):
    
    /* Normalization */
    xyfact = (x*x + y*y)/esm1 + (b+f)*(b+f)/(4.*e*e);
    norm = sqrt(esm1*esm1 + (x*x + y*y)/xyfact);
    n.x = -x / sqrt(xyfact) / norm;
    n.y = -y / sqrt(xyfact) / norm;
    n.z = esm1 / norm;
    break;
    
    /* FOCAL PLANE */
  case(OPTIC_FP):
    
    n.x = 0.0;
    n.y = 0.0;
    n.z = 1.0;
    
    /* SPHERICAL GRATING */
  case(OPTIC_GRS):
    
    xg = -R*sin(alpha);
    yg = 0.0;
    zg = -(v+b);
    
    /* Normalization */
    norm = hypot3(xg-x, yg-y, zg- z);
    n.x = (xg - x) / norm;
    n.y = (yg - y) / norm;
    n.z = (zg - z) / norm;    
    break;
    
    /* TOROIDAL GRATING */
  case(OPTIC_GRT):
    
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
    norm = hypot3(n.x, n.y, n.z);
    n.x /= norm;
    n.y /= norm;
    n.z /= norm;
    break;
    
  default:
    printf("Help, something's gone horribly wrong!\n");
  }
  
  return n;
}


/* Function to reflect the incoming ray based on i.n = -o.n && ixn = oxn  */
/* Reflected directions are placed back into *a, and a status is returned */
int rays_reflect(scope_ray *a, scope_ray n){
  
  /* Variable declarations */
  int status = 0;
  
  /* Determine the primary direction of n, and use the requisite subfunction
     to solve for that component of o first. */
  switch(vectors_primary(n.vx, n.vy, n.vz)){
  case NHAT_X:
    rays_reflect_x(a, n);
    break;
  case NHAT_Y:
    rays_reflect_y(a, n);
    break;
  case NHAT_Z:
    rays_reflect_z(a, n);
    break;
  default:
    status = -1;
    printf("Woah, Nellie!  Major problem here!  Abort, abort, abort!\n");
  }
  return status;
}


/*************************************************************************/
/* The following functions are type void.  There are no internal checks, */
/* so no status return values are required.  These functions replace the */
/* input values with the output values, so are destructive to            */
/* information.                                                          */
/*************************************************************************/

/* Function for advancing rays along path; x = x0 + v*t */
void rays_advance_ray(scope_ray *beam, double d){
  
  beam->x = beam->x + d * beam->vx;
  beam->y = beam->y + d * beam->vy;
  beam->z = beam->z + d * beam->vz;
  
  return;
}






/* Functions to reflect the incoming ray based on i.n = -o.n && ixn = oxn */
/* Reflected directions are placed back into *a, and a status is returned */
void rays_reflect_x(scope_ray *a, scope_ray n){
  
  /* Variable declarations */
  double ox, oy, oz;                     // Computed output vector components
  
  /* Calculate ox first */
  ox = (n.vy*n.vy + n.vz*n.vz - n.vx*n.vx)*a->vx - 2.*n.vx*n.vz*a->vz - 
    2.*n.vx*n.vy*a->vy;
  ox /= (n.vx*n.vx + n.vy*n.vy + n.vz*n.vz);           // Norm SHOULD == 1
  
  /* Calculate oy & oz based on ox */
  oy = a->vy + (n.vy/n.vx)*(ox - a->vx);
  oz = a->vz + (n.vz/n.vx)*(ox - a->vx);
  
  /* Place updated velocities in ray a */
  a->vx = ox;
  a->vy = oy;
  a->vz = oz;
  
  return;
}

void rays_reflect_y(scope_ray *a, scope_ray n){
  
  /* Variable declarations */
  double ox, oy, oz;                     // Computed output vector components
  
  /* Calculate oy first */
  oy = (n.vz*n.vz + n.vx*n.vx - n.vy*n.vy)*a->vy - 2.*n.vx*n.vy*a->vx - 
    2.*n.vy*n.vz*a->vz;
  oy /= (n.vx*n.vx + n.vy*n.vy + n.vz*n.vz);           // Norm SHOULD == 1
  
  /* Calculate ox & oz based on oy */
  ox = a->vx + (n.vx/n.vy)*(oy - a->vy);
  oz = a->vz + (n.vz/n.vy)*(oy - a->vy);
  
  /* Place updated velocities in ray a */
  a->vx = ox;
  a->vy = oy;
  a->vz = oz;
  
  return;
}

void rays_reflect_z(scope_ray *a, scope_ray n){
  
  /* Variable declarations */
  double ox, oy, oz;                     // Computed output vector components
  
  /* Calculate oz first */
  oz = (n.vx*n.vx + n.vy*n.vy - n.vz*n.vz)*a->vz - 2.*n.vx*n.vz*a->vx - 
    2.*n.vy*n.vz*a->vy;
  oz /= (n.vx*n.vx + n.vy*n.vy + n.vz*n.vz);           // Norm SHOULD == 1
  
  /* Calculate ox & oy based on oz */
  ox = a->vx + (n.vx/n.vz)*(oz - a->vz);
  oy = a->vy + (n.vy/n.vz)*(oz - a->vz);
  
  /* Place updated velocities in ray a */
  a->vx = ox;
  a->vy = oy;
  a->vz = oz;
  
  return;
}


