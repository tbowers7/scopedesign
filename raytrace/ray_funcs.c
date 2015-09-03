/******** ray_funcs.c ********/
/* Timothy Ellsworth Bowers
   13 October 2009

   Subroutine functions needed for the Ray Trace Project (ASTR 5760, F09).
*/

#include "ray_funcs.h"
#include <math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>


/* Function for calculating the surface of the primary mirror z = f(x,y) */
double primary_z(double x, double y, raytrace_geom *geom){

  double f = geom->f;
  double v = geom->v;
  
  return (x*x + y*y)/(4.*f) - v;
}


/* Function for calculating the surface of the secondary mirror z = f(x,y) */
double secondary_z(double x, double y, raytrace_geom *geom){
  
  double e = geom->e;
  double f = geom->f;
  double b = geom->b;
  double v = geom->v;
  
  return sqrt( (x*x + y*y)/(e*e -1.) + (f+b)*(f+b)/(4.*e*e)) -
    (v - f/2. + b/2.);
}


/* Function for calculating the surface of the focal plane z = f(x,y) */
double focalplane_z(double x, double y, raytrace_geom *geom){

  return -(geom->v + geom->b);
}


/* Function for calculating the surface of the sphrical grating z = f(x,y) */
double sph_grating_z(double x, double y, raytrace_geom *geom){

  double b     = geom->b;
  double v     = geom->v;
  double R     = geom->Rrc;
  double alpha = geom->alpha;

  double x0 = -R * sin(alpha);
  double y0 = 0.;
  double z0 = -(v+b);
  
  return z0 - sqrt( R*R - (x-x0)*(x-x0) - (y-y0)*(y-y0) );
}


/* Function for calculating the surface of the torroidal grating z = f(x,y) */
double tor_grating_z(double x, double y, raytrace_geom *geom){

  double b     = geom->b;
  double v     = geom->v;
  double R     = geom->Rrc;
  double alpha = geom->alpha;

  double x0 = -R * sin(alpha);
  double y0 = 0.;
  double z0 = -(v+b);

  double beta = asin(0.108);             // Optimize for 1300A & 1900A
  
  double big_Rt = R*(1. - cos(alpha)*cos(beta));
  double lit_rt = R*cos(alpha)*cos(beta);

  double R_rad = big_Rt + sqrt( lit_rt*lit_rt - (y-y0)*(y-y0) );

  return z0 - sqrt( R_rad*R_rad - (x-x0)*(x-x0) );
}

/* Function for calculating the surface of the cylindrical detector */
double detector_z(double x, double y, raytrace_geom *geom){

  double b     = geom->b;
  double v     = geom->v;
  double R     = geom->Rrc;
  double alpha = geom->alpha;

  double x0 = -(R/2.) * sin(alpha);
  double z0 = -(v+b) - (R/2.)*cos(alpha);
  
  return z0 + sqrt( (R/2.)*(R/2.) - (x - x0)*(x-x0));
  
}


double raytrace_free_distance(raytrace_ray ray, raytrace_geom geom, int surf){
  
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

  if(surf == OPTIC_PRI)
    z2 = primary_z(x1 + t*xa, y1 + t*ya, &geom);
  else if(surf == OPTIC_SEC)
    z2 = secondary_z(x1 + t*xa, y1 + t*ya, &geom);
  else if(surf == OPTIC_FP)
    z2 = focalplane_z(x1 + t*xa, y1 + t*ya, &geom);
  else if(surf == OPTIC_GRS)
    z2 = sph_grating_z(x1 + t*xa, y1 + t*ya, &geom);
  else if(surf == OPTIC_GRT)
    z2 = tor_grating_z(x1 + t*xa, y1 + t*ya, &geom);
  else if(surf == OPTIC_SF)
    z2 = detector_z(x1 + t*xa, y1 + t*ya, &geom);

  /* Return condition on root */
  return z1 + t*za - z2;
}


/* Function for advancing rays along path */
void raytrace_advance_ray(raytrace_ray *beam, double d){
  
  beam->x = beam->x + d * beam->vx;
  beam->y = beam->y + d * beam->vy;
  beam->z = beam->z + d * beam->vz;
  
  return;  
}

/* Wrapper function to find the normal vector of a surface at a given point */
raytrace_ray raytrace_get_n(raytrace_ray pos, raytrace_geom geom, int surf){
  
  /* Variable declarations */
  raytrace_ray n;
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


/* Function to find the grating groove vector g and line spacing correction */
raytrace_ray raytrace_get_g(raytrace_ray pos, raytrace_geom geom, int surf,
			    double *del){
  
  /* Variable delcarations */
  raytrace_ray x,n,g;
  double norm;
  double alpha = geom.alpha;
  
  /* Define x to be the vector tangent to the "vertex" of the spherical
     grating, in the x-z plane */
  x.x = sin(M_PI/2. - alpha);
  x.y = 0.;
  x.z = cos(M_PI/2.  - alpha);
  
  /* Normal vector at this point */
  n = raytrace_get_n(pos,geom,surf);
  
  /* g = (x x n)/|x x n| */
  g.x = x.y*n.z - x.z*n.y;
  g.y = x.z*n.x - x.x*n.z;
  g.z = x.x*n.y - x.y*n.x;
  
  /* Normalize g to 1 */
  norm = gsl_hypot3(g.x,g.y,g.z);
  
  g.x /= norm;
  g.y /= norm;
  g.z /= norm;
  
  /* Line spacing correction del = x cross (n_xz) */
  norm = hypot(n.x,n.z);
  n.x /= norm;
  n.z /= norm;
  
  *del = gsl_hypot3( 0. , x.z*n.x - x.x*n.z,  0.);
  return g;
}


/* Function to reflect the incoming ray based on i.n = -o.n & ixn = oxn */
void raytrace_reflect(raytrace_ray *a, raytrace_ray n){
  
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


/* Function to bounce light off a diffraction grating
   Adapted and translated from the FORTRAN subroutine vecray from Jim Green */
void raytrace_vecray(double m, double d, raytrace_ray *ray,
		     raytrace_ray n, raytrace_ray g){
  
  /* Comments from the vecray.f code:
     m = order
     d = line spacing (angstroms/line)
     l = wavelength (angstroms)
     N is plane normal, pointing up on side that rays hit
     G is vector in direction of lines on grating
     I is incident vector pointing in ray prop. direction
     o is output vector pointing in ray prop. direction
     x indicates primed coordinate systemc
     xx indicates double primed coordinate system
  */
  
  /* Variable Declarations */
  double l,a,g1,g2,g3,n1,n2,n3,i1,i2,i3,o1,o2,o3,xn1,xn2;
  double r11,r12,r13,r21,r22,r23,r31,r32,r33;
  double rt11,rt12,rt13,rt21,rt22,rt23,rt31,rt32,rt33;
  double xxi1,xxi3,xxo1,xxo2,xxo3;
  
  /* Place raytrace_ variables into routine variables */
  g1 = g.x;
  g2 = g.y;
  g3 = g.z;
  
  n1 = n.x;
  n2 = n.y;
  n3 = n.z;
  
  i1 = ray->vx;
  i2 = ray->vy;
  i3 = ray->vz;
  
  l = ray->lambda;
  
  /* Line-for-line translation of FORTRAN routine */
  
  goto line60;
 line50: ;
  g1 = -g1;                  // Turn g -> -g if it was defined in the "wrong"
  g2 = -g2;                  // way initally.  There is an inherent sign
  g3 = -g3;                  // ambigutiy in g, and this corrects for it.
 line60: ;  
  
  a = sqrt(1.0 - g3*g3);     // Normalization
  
  xn1 = ( n1*g2 - n2*g1 ) / a;
  xn2 = g3*( n1*g1 + n2*g2 ) / a  - a*n3;
  
  /* Comments:  
     R is the transform from basic to xx coordinates
     in xx coordinates, g is in the z direction
     RT is the return transform
  */
  
  r11 = ( xn2*g2 - xn1*g3*g1 )/a;
  r12 = -( xn2*g1 + xn1*g2*g3 )/a;
  r13 = xn1*a;
  r21 = (xn1*g2 + xn2*g3*g1)/a;
  r22 = ( -xn1*g1 + xn2*g2*g3 )/a;
  r23 = -a*xn2;
  r31 = g1;
  r32 = g2;
  r33 = g3;
  
  rt11 = r11;
  rt12 = r21;
  rt13 = r31;
  rt21 = r12;
  rt22 = r22;
  rt23 = r32;
  rt31 = r13;
  rt32 = r23;
  rt33 = r33;
  
  xxi1 = i1*r11 + i2*r12 + i3*r13;
  
  if(xxi1 < 0.) goto line50;
  
  xxi3 = i1*r31 + i2*r32 + i3*r33;
  
  xxo1 = (m*l/d) + xxi1;
  xxo3 = xxi3;
  xxo2 = sqrt ( 1.0 - xxo1*xxo1 - xxo3*xxo3 );
  
  o1 = xxo1*rt11 + xxo2*rt12 + xxo3*rt13;
  o2 = xxo1*rt21 + xxo2*rt22 + xxo3*rt23;
  o3 = xxo1*rt31 + xxo2*rt32 + xxo3*rt33;
  
  
  /* Place output ray back into raytrace_ variables */
  ray->vx = o1;
  ray->vy = o2;
  ray->vz = o3;
  
  return;
}


/* Function for mapping the (x,y,z) coordinates onto the (RADIUS,x,y) of
   the cylindrical detector */
raytrace_ray raytrace_mapdetector(raytrace_ray pos, raytrace_geom geom){
  
  /* Variable Declarations */
  double x0,z0;
  raytrace_ray det;
  double alpha = geom.alpha;
  double x = pos.x;
  double y = pos.y;
  double z = pos.z;
  double R = geom.Rrc;
  double v = geom.v;
  double b = geom.b;
  
  /* Find center of the cylindrical detector (raduis of curvature) */
  x0 = -(R/2.)*sin(alpha);
  z0 = -(v+b) - (R/2.)*cos(alpha);
  
  /* Map */
  det.z = (R/2.) - sqrt( (x-x0)*(x-x0) + (z-z0)*(z-z0));   // Height above det.
  det.x = (R/2.-det.z) * (M_PI/2. + alpha - atan2(z-z0,x-x0));   // detector-x
  det.y = y;                                                     // detector-y
  
  return det;
}
