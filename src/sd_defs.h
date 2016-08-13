/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 * 
 * FILE: sd_defs.h
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

#ifndef SD_DEFS_H
#define SD_DEFS_H


/* Include various GSL functions, defined as local */
#include <gsl/gsl_math.h>
#define hypot3 gsl_hypot3
#define nan    gsl_nan
#define posinf gsl_posinf
#define neginf gsl_neginf


/* Define values to be used */
#define N_RAYS 100000       // Number of rays to be used

/* Define symbolic integers for various surfaces */
#define OPTIC_PRI 20        // Primary Mirror
#define OPTIC_SEC 21        // Secondary Mirror
#define OPTIC_GRS 22        // Spherical Dispersion Grating
#define OPTIC_GRT 23        // Torroidal Dispersion Grating
#define OPTIC_FP  24        // Cassegrain Focal Plane
#define OPTIC_SF  25        // Rowland Circle Spectral Focus
#define OPTIC_NFP 26        // Newtonian Focal Plane

/* Define symbolic integers for TYPE of optical element */
#define OPTIC_PLANE    501     // Plane Mirror
#define OPTIC_PARABOLA 502     // Parabolic Mirror
#define OPTIC_SHPERE   503     // Spherical Mirror
#define OPTIC_HYPER    504     // Hyperbolic Mirror
#define OPTIC_CONVER   505     // Converging Lens
#define OPTIC_DIVER    506     // Diverging Lens

/* Define symbolic integers for NHAT */
#define NHAT_X 521     // Optical element is primarily normal to X
#define NHAT_Y 522     // Optical element is primarily normal to Y
#define NHAT_Z 523     // Optical element is primarily normal to Z

/* Define symbolic integers for TARGET type for ray initialization */
#define TARGET_POINT  601     // Single point source
#define TARGET_POINTS 602     // Multiple point sources
#define TARGET_IMAGE  605     // Use FITS image to generate ray angles



/* Typedef structures needed */

// "Ray", including position, direction, wavelength & lost flag
typedef struct{
  double x;        // Position with ray-trace environment
  double y;        // 
  double z;        // 
  double vx;       // Direction (unit vector)
  double vy;       // 
  double vz;       // 
  double lambda;   // Wavelength in Angstroms
  int lost;        // Indicates whether a ray has been "lost"
} scope_ray;


// Geometry of the problem, built as a structure for easier passing
typedef struct{
  double f;      // Focal Length of Primary
  double b;      // Back-plane distance (Focal plane, behind primary vertex)
  double v;      // Distance of primary vertex from 'starting point' (arb)
  double e;      // From focal-ratios: eps = (final+pri)/(final-pri)
  double Dp;     // Primary diameter
  double Ds;     // Secondary diameter (on-axis)
  double Rrc;    // Diameter of Rowland Circle = radius of grating curvature
  double alpha;  // In radians
  double d;      // lines/mm converted to Angstroms per line
} raytrace_geom;


// General Geometry for ScopeDesign Consumption
typedef struct{
  double a;      // Space-filler
} scope_geom;

// Optical Element Geometry
typedef struct{
  int type;      // TYPE of optical element (plane, parabola, etc.)
  double f;      // Focal Length of Optical Element
  double d;      // Diameter of Optical Element
  double cx;     // x-position of center of element
  double cy;     // y-position of center of element
  double cz;     // z-position of center of element
  double nx;     // N_x for center of element
  double ny;     // N_y for center of element
  double nz;     // N_z for center of element
  int nhat;      // Primary direction of nhat - set by setup_orient_optic()
} scope_optic;


// Parameters needed for passing to the GSL root-finding functions
typedef struct{
  scope_ray ray;
  raytrace_geom geom;
  int surf;
} scope_root_params;



#endif /* SD_DEFS_H */
