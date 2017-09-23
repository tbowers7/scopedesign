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

#define wombat extern                  // wombat == protect on N_RAYS
#include "sd_defs.h"                   // Main Package Header
#undef wombat

/* Include packages */
#include <string.h>
#include <fitsio.h>              // Contains CFITSIO function declarations
#include <gsl/gsl_errno.h>       // Contains GSL's error-handling
#include <gsl/gsl_histogram2d.h> // Contains GSL's 2-D Histograms

/* Local headers */
#include "illumination.h"
#include "fitsw.h"
#include "imutil.h"

int write_focal_plane(){
  
  char *fn="filename";
  char *hd="hd";
  double **array;
  long size[2];
  int status=0;
  
  printf("We're in illumunation.c...\n");
  
  fitsw_write2file(fn, hd,  array, size, &status);
  
  return status;
  
  
}


/* Function (in progress) to write ray locations to a FITS file. */
char *illum_write_locations(scope_ray *rays, int location, int *status){
  
  /* Variable Declarations */
  int  errval;
  long i,j,k,nx,ny;
  char fn[256];
  *status = 0;
  
  /* Allocate 2-D Histogram to accumulate locations */
  /* NOTE: in the future, will need to pass in geometry descriptors... for now
     just work on test situation in main(). */
  
  nx = 220;          // NBINS in the x direction
  ny = 220;          // NBINS in the y direction
  
  gsl_histogram2d *h = gsl_histogram2d_alloc(nx, ny);
  long n_sq[2] = {nx,ny};
  double **imarr = imutil_alloc_2darray(n_sq);
  
  
  /* Set range for histogram */
  gsl_histogram2d_set_ranges_uniform (h, 
                                      -1.1, 1.1,
                                      -1.1, 1.1);
  
  /* Loop through rays and accumulate into bins */
  for(i=0;i<N_RAYS;i++){
    errval = gsl_histogram2d_increment(h, rays[i].x, rays[i].y);
    if(errval)
      printf("We have an error, errno=%d\n",errval);
  }
  
  /* Convert 2-D Histogram into a standard array for writing to FITS */
  for(i=0;i<nx;i++)
    for(j=0;j<ny;j++)
      imarr[i][j] = gsl_histogram2d_get(h, i, j);
  
  
  /* Use SWITCH statement to get correct filename to correspond with location */
  switch(location)
    {
    case OPTIC_INF:
      sprintf(fn,"initial_illumination.fits");
      break;
    case OPTIC_PRI:
      sprintf(fn,"primary_illumination.fits");
      break;
    case OPTIC_SEC:
      sprintf(fn,"secondary_illumination.fits");
      break;
      
      
    default:
      sprintf(fn,"test_data.fits");
    }
  
  
  /* Write it out! */
  char *hd = (char *)malloc(100 * sizeof(char));
  sprintf(hd,"%s/new-image.fits",DATADIR);
  
  fitsw_write2file(fn, hd,  imarr, n_sq, status);
  
  printf("In-function value of status: %d\n",*status);
  
  /* Clean up */
  gsl_histogram2d_free(h);
  
  return strdup(fn);            // Return a properly malloc'd and sized string
}
