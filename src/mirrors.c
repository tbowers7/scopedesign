/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: mirrors.c
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

#include "sd_defs.h"                   // Main Package Headers

/* Include packages */
#include <math.h>

/* Local headers */
#include "mirrors.h"


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


