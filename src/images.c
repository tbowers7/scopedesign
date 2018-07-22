/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: images.c
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

#define wombat extern                  // wombat == protect on N_RAYS
#include "sd_defs.h"                   // Main Package Header
#undef wombat

/* Include packages */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_errno.h>       // Contains GSL's error-handling
#include <gsl/gsl_histogram2d.h> // Contains GSL's 2-D Histograms

/* Local headers */
#include "images.h"
#include "fitsw.h"

/***** Array Allocation and Freeing Functions *****/

/* Function to allocate space for a 2-D array of double */
/* Note: This function is compatible with FITS standards,
   which are COLUMN-MAJOR!  Therefore, indexing of the
   output array goes: array[y][x]!!! */
double **images_alloc_2darray(long *size){
  
  /* Variable Declarations */
  int mm;
  double **new_array;
  
  new_array = (double **)malloc(size[1] * sizeof(double *));
  for(mm=0; mm<size[1]; mm++)
    new_array[mm] = (double *)calloc(size[0], sizeof(double));
  
  return new_array;
}


/* Function to free space occupied by 2-D array */
void images_free_2darray(double **array, long *size){
  
  /* Variable Declarations */
  int mm;

  for(mm=0; mm < size[1]; mm++)
    free(array[mm]);
  free(array);
    
  return;
}


/***** High-Level Write-to-File Functions *****/

/* Function (in progress) to write ray positions to a FITS file. */
char *images_write_pos(scope_ray *rays, int location, char *telname,
		       int *status){
  
  /* Variable Declarations */
  int  gsl_status, bitpix;
  long i,j,nx,ny;
  char fn[FLEN_FILENAME];            // CFITSIO max length of filename
  
  
  /* Allocate 2-D Histogram to accumulate locations */
  /* NOTE: in the future, will need to pass in geometry descriptors... for now
     just work on test situation in main(). */
  
  nx = 440;          // NBINS in the x direction
  ny = 220;          // NBINS in the y direction
  
  gsl_histogram2d *h = gsl_histogram2d_alloc(nx, ny);
  long naxes[2] = {nx,ny};
  double **imarr = images_alloc_2darray(naxes);
  
  
  /* Set range for histogram */
  gsl_histogram2d_set_ranges_uniform (h,           // NOTE: Need to
                                      -2.2, 2.2,   // set these dynamically
                                      -1.1, 1.1);  // based on situation
  
  /* Loop through rays and accumulate into bins */
  for(i=0;i<N_RAYS;i++){
    gsl_status = gsl_histogram2d_increment(h, rays[i].x, rays[i].y);
    if(gsl_status)
      printf("We have an error, errno=%d\n",gsl_status);
  }
  
  /* Convert 2-D Histogram into a standard array for writing to FITS */
  for(i=0;i<nx;i++)
    for(j=0;j<ny;j++)
      imarr[j][i] = gsl_histogram2d_get(h, i, j);  // Column-Major IMAGE Array
  
  /* All done with gsl_histogram, free it */
  gsl_histogram2d_free(h);
  
  
  /* We now have a (column-major) array ready for writing. */
  
  /* Use SWITCH statement to get correct filename to correspond with location */
  switch(location)
    {
    case OPTIC_INF:
      sprintf(fn,"initial_illumination.fits");
      bitpix = ULONG_IMG;
      break;
    case OPTIC_PRI:
      sprintf(fn,"primary_illumination.fits");
      bitpix = ULONG_IMG;
      break;
    case OPTIC_SEC:
      sprintf(fn,"secondary_illumination.fits");
      bitpix = ULONG_IMG;
      break;
      
      
    default:
      sprintf(fn,"test_data.fits");
    }
  
  
  /* Write it out! */
  fitsw_write2file(fn, naxes, imarr, bitpix, telname, status);
  
  printf("In-function value of status: %d\n",*status);
  
  return strdup(fn);            // Return a properly malloc'd and sized string
}


/* Function (in progress) to write ray angles to a FITS file. */
char *images_write_ang(scope_ray *rays, int location, char *telname,
		       int *status){
  
  /* Variable Declarations */
  int  gsl_status, bitpix;
  long i,j,nx,ny;
  char fn[FLEN_FILENAME];            // CFITSIO max length of filename
  
  
  /* Allocate 2-D Histogram to accumulate locations */
  /* NOTE: in the future, will need to pass in geometry descriptors... for now
     just work on test situation in main(). */
  
  nx = 220;          // NBINS in the x direction
  ny = 220;          // NBINS in the y direction
  
  gsl_histogram2d *h = gsl_histogram2d_alloc(nx, ny);
  long naxes[2] = {nx,ny};
  double **imarr = images_alloc_2darray(naxes);
  
  
  /* Set range for histogram */
  gsl_histogram2d_set_ranges_uniform (h,           // NOTE: Need to
                                      -1.1, 1.1,   // set these dynamically
                                      -1.1, 1.1);  // based on situation
  
  /* Loop through rays and accumulate into bins */
  for(i=0;i<N_RAYS;i++){
    gsl_status = gsl_histogram2d_increment(h, rays[i].vx, rays[i].vy);
    if(gsl_status)
      printf("We have an error, errno=%d\n",gsl_status);
  }
  
  /* Convert 2-D Histogram into a standard array for writing to FITS */
  for(i=0;i<nx;i++)
    for(j=0;j<ny;j++)
      imarr[j][i] = gsl_histogram2d_get(h, i, j);  // Column-Major IMAGE Array
  
  /* All done with gsl_histogram, free it */
  gsl_histogram2d_free(h);
  
  
  /* We now have a (column-major) array ready for writing. */
  
  /* Use SWITCH statement to get correct filename to correspond with location */
  switch(location)
    {
    case OPTIC_INF:
      sprintf(fn,"initial_angle.fits");
      bitpix = ULONG_IMG;
      break;
    case OPTIC_PRI:
      sprintf(fn,"primary_angle.fits");
      bitpix = ULONG_IMG;
      break;
    case OPTIC_SEC:
      sprintf(fn,"secondary_angle.fits");
      bitpix = ULONG_IMG;
      break;
      
      
    default:
      sprintf(fn,"test_data.fits");
    }
  
  
  /* Write it out! */
  fitsw_write2file(fn, naxes, imarr, bitpix, telname, status);
  
  printf("In-function value of status: %d\n",*status);
  
  return strdup(fn);            // Return a properly malloc'd and sized string
}





/***** Other Left-Over Functions, Possibly to Use *****/

int write_focal_plane(char *telname){
  
  char *fn="filename";
  char *hd="hd";
  double **array;
  long size[2];
  int status=0, bitpix=DOUBLE_IMG;
  
  printf("We're in illumunation.c...\n");
  
  fitsw_write2file(fn, size, array, bitpix, telname, &status);
  
  return status;
  
  
}



/* Function for reading 2-D image array into 1-D array for statistics */
/* IMPORTANT: returned array must be freed by calling function */
double *imutil_2d_to_1d(double **two_d, long *arrsize){
  
  /* Variable Declarations */
  int p,q;
  double *one_d;
  
  /* Allocate space for array */
  one_d = (double *)calloc(arrsize[0] * arrsize[1], sizeof(double));
  
  /* Loop over array, reading into 1-D array */
  for(p=0;p<arrsize[1];p++)
    for(q=0;q<arrsize[0];q++)
      one_d[p * arrsize[0] + q] = two_d[p][q];
  
  return one_d;
}


/* Function returns an image subsection of a given size */
double **imutil_get_subsection(double **full, long *f_size, long *s_size, long *start){
  
  /* Variable Declarations */
  int a,b;
  double **sub;
  
  /* Allocate space for subsection array */
  sub = images_alloc_2darray(s_size);
  
  /* Check that subsection does not go beyond the bounds of the image */
  if(start[0] + s_size[0] - 1 > f_size[0] ||
     start[1] + s_size[1] - 1 > f_size[1]){
    fprintf(stderr,"Image subsection goes outside bounds of image!\n");
    return sub;
  }
  
  for(a = 0; a < s_size[1]; a++)                   // y rows
    for(b =0; b < s_size[0]; b++)                  // x columns
      sub[a][b] = full[a+start[1]][b+start[0]];
  
  return sub;    
}




/* imutil_transpose() takes an (n x m) array and builds the (m x n) transpose.
   Care is taken in case input and transpose are the same array in calling
   routine. */
void imutil_transpose(double **a, double **at, int n, int m){

  /* Variable Declarations */
  int x,y;
  long size[2]  = {n,m};
  double **trans;
  
  /* Allocate transpose array */
  trans = images_alloc_2darray(size);

  /* Build the transpose array */
  for(x=0;x<n;x++){
    for(y=0;y<m;y++){
      trans[y][x] = a[x][y];
    }
  }

  /* Place the transpose in the output 'at' array */
  for(x=0;x<n;x++){
    for(y=0;y<m;y++){
      at[y][x] = trans[y][x];
    }
  }
  
  /* Free the temproary transpose array */
  images_free_2darray(trans,size);

  return;
}
