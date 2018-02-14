/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: imutil.c
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

/* Local headers */
#include "imutil.h"


/* Function to allocate space for a 2-D array of double */
double **imutil_alloc_2darray(long *size){

  /* Variable Declarations */
  int mm;
  double **new_array;

  new_array = (double **)malloc(size[1] * sizeof(double *));
  for(mm=0; mm<size[1]; mm++)
    new_array[mm] = (double *)calloc(size[0], sizeof(double));

  return new_array;
}


/* Function to free space occupied by 2-D array */
void imutil_free_2darray(double **array, long *size){
  
  /* Variable Declarations */
  int nn;

  for(nn=0; nn < size[1]; nn++)
    free(array[nn]);
  free(array);
    
  return;
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
  sub = imutil_alloc_2darray(s_size);
  
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
  trans = imutil_alloc_2darray(size);

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
  imutil_free_2darray(trans,size);

  return;
}
