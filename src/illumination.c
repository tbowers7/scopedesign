/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: illumination.c
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
#include "sd_defs.h"

#ifndef HAVE_FITSIO_H
#define HAVE_FITSIO_H
#include <fitsio.h>       // Contains CFITSIO function declarations
#endif

#ifndef HAVE_TPEB_H
#define HAVE_TPEB_H
#include <tpeb.h>         // Contains TPEB Library function declarations
#endif

int write_focal_plane(){
  
  char *fn="filename";
  char *hd="hd";
  double **array;
  long size[2];
  int status=0;
  
  
  fits_wrap_write2file(fn, hd,  array, size, &status);
  
  return status;
  
  
}

