/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: fitsw.c
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
#include <string.h>
#include <time.h>                      // To update FITS headers

/* Local headers */
#include "fitsw.h"                  // Contains <fitsio.h> include
#include "images.h"

/***** Public-Facing Functions *****/

/* Function to write array to FITS file */
/* NOTE: FUNCTION CURRENTLY COPIES HEADER FROM ANOTHER FILE... MODIFY TO 
   CREATE NEW HEADER AND POPULATE IT WITH REASONABLE THINGS! */
void fitsw_write2file(char *fileout, long naxes[2], double **array, 
		      int bitpix, char *telname, int *status){
  
  /* Variable Declarations & Initializations */
  int k;
  long fpixel[2];
  char buf_date[FLEN_VALUE],buf_time[FLEN_VALUE],*mod_comm,*fn;
  fitsfile *fitsfp;
  time_t now;
  struct tm *ptr;

  /* Set CFITSIO status = 0 before we begin */
  *status = 0;
  
  
  
  /* Open FITS file for writing, overwrite existing file */
  fn = (char *)malloc(sizeof(char) * strlen(fileout)+1);
  sprintf(fn,"!%s",fileout);     // CFITSIO will overwrite file prepended w/ "!"
  if( fits_create_file(&fitsfp, fn, status) )
    fw_catcherror(status);       // Send pointer not value
  free(fn);                      // Free "!" filename
  
  /* Create image HDU */
  int naxis = 2;                 // Routine is specific for 2-D images
  if( fits_create_img(fitsfp, bitpix, naxis, naxes, status) )
    fw_catcherror(status);       // Send pointer not value
  
  /* Write the UT date and time of file creation */
  time(&now);
  ptr = gmtime(&now);
  strftime(buf_date, FLEN_VALUE, "%Y-%m-%d", ptr);
  strftime(buf_time, FLEN_VALUE, "%H:%M:%S", ptr);
  fits_update_key(fitsfp, TSTRING, "DATE-OBS", buf_date,
		  "UT date of observation", status);
  fits_update_key(fitsfp, TSTRING, "TIME-OBS", buf_time,
		  "UT time of observation", status);
  
  /* Add various FITS keywords, specific to ScopeDesign */
  fits_write_key(fitsfp, TSTRING, "OBSERVAT", "ScopeDesign Ray-Tracing Program",
		 NULL, status);
  fits_write_key(fitsfp, TSTRING, "TELESCOP", telname,
		 "modeled telescope for ray trace", status);
  fits_write_key(fitsfp, TSTRING, "FILENAME", fileout, NULL, status);
  
  /* Write array to file */
  fpixel[0] = 1;
  for(k=0; k < naxes[1]; k++){       // Loop over size[1] rows (i.e. y)
    fpixel[1] = k + 1; 
    if(fits_write_pix(fitsfp, TDOUBLE, fpixel, naxes[0], array[k], 
  		      status)){
      fits_report_error(stderr,*status);
      break;
    }
  }
  
  /* Clean up */
  fits_close_file(fitsfp, status);
  
  /* Report any CFITSIO errors to stderr */
  if(!*status)
    fw_catcherror(status);    // Send pointer not value
  
  return;
  
}






/* Routine for reading FITS into array, starting at point, and w/ size */
/* This version assumes an open FITS file, and accepts the fitsfile pointer */
double **fitsw_read2array(fitsfile *fitsfp, long xystart[2], long xysize[2],
			      int data_type, int *status){
  
  /* Variable declarations & Initilaztion */
  int i,naxis,bitpix;
  long fpixel[2],naxes[2];
  *status=0;
  
  /* Allocate space for array */
  double **array = images_alloc_2darray(xysize);
  
  
  /* Read in FITS file using CFITSIO library routines - w/ error checking */
  /* Note: xystart[2] SHOULD be in 'array' notation (i.e. 0-1023), and this
     routine needs to convert these into 'human' notation (i.e. 1-1024). */
  
  /* ERROR CHECKING & CONVERTING NOTATION */
    /* Get image file parameters */
    if(fits_get_img_param(fitsfp, 2, &bitpix, &naxis, naxes, status)){
      fw_catcherror(status);    // Send pointer not value
    }    
    /* Error catching -- number of axes */
    if(naxis != 2){
      fprintf(stderr,"Error: only 2D images are supported.\n\n");
      fits_close_file(fitsfp,status);
      exit(1);
    }

    /* Notation Shift */
    long xstart = xystart[0] + 1;
    long ystart = xystart[1] + 1;
    

  /* Check to see if subsection goes beyond image bounds */
  // First Check x
  if( xstart < 1 || (xstart + xysize[0] - 1) > naxes[0]){
    fprintf(stderr,"Error: subsection is out of bounds\n");
    fits_close_file(fitsfp,status);
    exit(1);
  }
  // Then Check y
  if( ystart < 1 || (ystart + xysize[1] - 1) > naxes[1]){
    fprintf(stderr,"Error: subsection is out of bounds\n");
    fits_close_file(fitsfp,status);
    exit(1);
  }
  
  
  /* Read in the FITS file */  
  fpixel[0] = xstart;              // Set initial coordinate in the x-axis.
  for(i=0; i < xysize[1]; i++){    // Loop over xysize[1] (i.e. y) rows
    fpixel[1] = ystart + i;        // Set initial y-coordinate here...
    
    if(fits_read_pix(fitsfp, data_type, fpixel, xysize[0], NULL,
		     array[i], NULL, status)){
      fits_report_error(stderr,*status);
      break;
    }
  }
  
  return array;
}


/***** Private Functions Internal to Fitsw *****/

/* Routine for opening FITS file READONLY with error checking */
fitsfile *fw_open_r(char *filename, int *status){
  
  fitsfile *fitsfp;
  
  if(fits_open_file(&fitsfp, filename, READONLY, status)){
    fw_catcherror(status);    // Send pointer not value
  }
  
  return fitsfp;
}


/* Routine for opening FITS file READONLY with error checking */
fitsfile *fw_open_rw(char *filename, int *status){
  
  fitsfile *fitsfp;
  
  if(fits_open_file(&fitsfp, filename, READWRITE, status)){
    fw_catcherror(status);    // Send pointer not value
  }
  
  return fitsfp;
}


/* Function to create basic header for FITS file */
void fw_make_header(){

  return;
}


/* Function to catch errors thrown by CFITSIO */
void fw_catcherror(int *status){
  fits_report_error(stderr,*status);  // Report error using FITSIO routine
  
  if(*status){                        // If no error, go on...
    /* Add error-catching code here */
    switch(*status){
      
    case FILE_NOT_OPENED :
      printf("We encountered this one!\n");
      break;
      
    case WRITE_ERROR :
      printf("This is a write error!\n");
      break;
      
    case FITSW_NOFILE_EXIT :
      printf("This is using a new error code!\n");
      break;
      
    default :
      printf("We hit the default... exiting.\n");
      exit(1);
    }
    
    printf("We need to add some error-catching code here...\n");
    
    exit(1);
  }
    
  return;
}
