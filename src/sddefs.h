/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 * 
 * FILE: sddefs.h
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

#ifndef SDDEFS_H
#define SDDEFS_H


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



/* Typedef structures needed */

// "Ray", including position, direction, wavelength & lost flag
typedef struct{
  double x;        // Position with ray-trace environment
  double y;        // 
  double z;        // 
  double vx;       // Direction (unit vector)
  double vy;       // 
  double vz;       // 
  double lambda;   // Wavelength
  int lost=0;      // Indicates whether a ray has been "lost"
} scope_ray;


#endif /* SDDEFS_H */
