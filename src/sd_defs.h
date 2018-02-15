/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: sd_defs.h
 * 
 * Copyright (C) 2016-2018  Timothy P. Ellsworth Bowers
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

#ifndef SD_DEFS_H
#define SD_DEFS_H


#if HAVE_CONFIG_H
# include <config.h>
#endif

/* Check system type from config.h */
#if HAVE_SYSINFO
# define HAVE__LINUX 1
# define HAVE__MAC   0
# define HAVE__OTHER 0
# define SD_SYS      50  // Linux
#elif HAVE_SYSCTL
# define HAVE__LINUX 0
# define HAVE__MAC   1
# define HAVE__OTHER 0
# define SD_SYS      51 // Mac
#else
# define HAVE__LINUX 0
# define HAVE__MAC   0
# define HAVE__OTHER 1
# define SD_SYS      52 // Other
#endif



/* Include various GSL functions, defined as local */
#include <gsl/gsl_math.h>
#define hypot  gsl_hypot     // Force use of GSL version over system version
#define hypot3 gsl_hypot3
#define nan    GSL_NAN
#define posinf GSL_POSINF
#define neginf GSL_NEGINF

#if HAVE__BOOL
#  include <stdbool.h>
#else
#  define true  1            // Define true and false for use with type int
#  define false 0
#endif

/* Define N_RAYS as GLOBAL variable, to be set upon initialization */
wombat unsigned long N_RAYS; // Number of rays to be used

/* Define SYS_RAM as GLOBAL variable, to be set upon initialization */
wombat double SYS_RAM;       // System RAM in MB

/* Define symbolic integers for various surfaces */
#define OPTIC_INF 20         // Infinitely far away... (or initial point)
#define OPTIC_PRI 21         // Primary Mirror
#define OPTIC_SEC 22         // Secondary Mirror
#define OPTIC_TRI 23         // Tertiary Mirror
#define OPTIC_QUA 24         // Quaternary Mirror
#define OPTIC_QUI 25         // Quinary Mirror
#define OPTIC_SEN 26         // Senary Mirror
#define OPTIC_SEP 27         // Septenary Mirror
#define OPTIC_OCT 28         // Octonary Mirror
#define OPTIC_NON 29         // Nonary Mirror
#define OPTIC_DEN 30         // Denary Mirror
#define OPTIC_NFP 31         // Newtonian Focal Plane
#define OPTIC_CFP 32         // Cassegrain Focal Plane

/* Obsolete defines... need to clean out rays.c */
#define OPTIC_SF  666
#define OPTIC_FP  667
#define OPTIC_GRS 668
#define OPTIC_GRT 669

/* Define symbolic integers for TYPE of optical element */
#define OPTIC_PLANE    501   // Plane Mirror
#define OPTIC_PARABOLA 502   // Parabolic Mirror
#define OPTIC_SHPERE   503   // Spherical Mirror
#define OPTIC_HYPER    504   // Hyperbolic Mirror
#define OPTIC_CONVERG  505   // Converging Lens
#define OPTIC_DIVERG   506   // Diverging Lens

/* Define symbolic integers for NHAT */
#define NHAT_X 521     // Optical element is primarily normal to X
#define NHAT_Y 522     // Optical element is primarily normal to Y
#define NHAT_Z 523     // Optical element is primarily normal to Z

/* Define symbolic integers for TARGET type for ray initialization */
#define TARGET_POINT  601    // Single point source
#define TARGET_POINTS 602    // Multiple point sources
#define TARGET_IMAGE  605    // Use FITS image to generate ray angles

/* Define symbolic integers for DS9 communication */
#define DS9_FORCE_NEW  450   // Force new DS9 window regardless of extant
#define DS9_CANIBALIZE 451   // Force canibalization of existing DS9 window
#define DS9_WHATEVER   452   // If extant, use (with conditions), otherwise new
#define DS9_GET        453   // Read XPA handles from DS9
#define DS9_SET        454   // Set new XPA handle to DS9

/* Typedef structures needed */

// "Ray", including position, direction, wavelength & lost flag
typedef struct{
  double x;        // Position within ray-trace environment
  double y;        // 
  double z;        // 
  double vx;       // Direction (unit vector)
  double vy;       // 
  double vz;       // 
  double lambda;   // Wavelength in Angstroms
#if HAVE__BOOL
  bool   lost;     // Indicates whether a ray has been "lost"
#else
  int    lost;     // Use type int if BOOL type not available
#endif
} scope_ray;

// Optical Element Geometry
typedef struct{
  int    type;   // TYPE of optical element (plane, parabola, etc.)
  double f;      // Focal Length of Optical Element
  double dmaj;   // Major Diameter of Optical Element
  double dmin;   // Minor Diameter of Optical Element
  int    vmin;   // Direction of Minor Diameter (NHAT integers, 0 if N/A)
  double cx;     // x-position of center of element
  double cy;     // y-position of center of element
  double cz;     // z-position of center of element
  double nx;     // N_x for center of element
  double ny;     // N_y for center of element
  double nz;     // N_z for center of element
  int    nhat;   // Primary direction of nhat - set by setup_orient_optic()
} scope_optic;

// Structure for Obstruction / Reflection / Refraction information
typedef struct{
  int  elem;     // Symbolic integer for element
#if HAVE__BOOL
  bool block;    // Does this element block light? (i.e. set lost = true?)
  bool reflect;  // Does this element reflect light?
  bool refract;  // Does this element refract light?
#else
  int  block;    // Use type int if BOOL type not available
  int  reflect;
  int  refract;
#endif
} scope_element;


// Telescope Structure for ScopeDesign Consumption
typedef struct{
  scope_optic primary;      // Provide space for 10 optical elements
  scope_optic secondary;    // 
  scope_optic tertiary;     // 
  scope_optic quaternary;   //
  scope_optic quinary;      //
  scope_optic senary;       //
  scope_optic septenary;    //
  scope_optic octonary;     //
  scope_optic nonary;       //
  scope_optic denary;       //
} scope_scope;



// Structure containing DS9 XPA handles for the open window
typedef struct{
  char  *file;               // Filename
  int    frame;              // Frame Number
  char  *cmap;               // Colormap
  char  *scale;              // Color scale type
  double min_scale;          // Color scale minimum
  double max_scale;          // Color scale maximum
  int    zoom;               // Zoom
} scope_display;


// Leftover structure from raytrace program... still here to allow compile...
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



// Parameters needed for passing to the GSL root-finding functions
typedef struct{
  scope_ray ray;
  raytrace_geom geom;
  int surf;
} scope_root_params;



#endif /* SD_DEFS_H */
