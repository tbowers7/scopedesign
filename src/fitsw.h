/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: fitsw.h
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


#ifndef FITSW_H
#define FITSW_H


#ifndef HAVE_FITSIO_H
# define HAVE_FITSIO_H
# include <fitsio.h>
#endif


#define FITSW_NOFILE_EXIT 2104  // Codes used by fitsw_catcherror()
#define FITSW_NOFILE_CONT 2105
#define FITSW_EOF_ERROR   2106


/* ========================= */
/*   Function Declarations   */
/* ========================= */

fitsfile *fitsw_open_read(char *filename, int *status);
fitsfile *fitsw_open_readwrite(char *filename, int *status);
double  **fitsw_read2array(fitsfile *fitsfp, long xystart[2], long xysize[2],
			      int data_type, int *status);
void      fitsw_write2file(char *fileout, char *copyhdr, double **array, 
			  long subsize[2], int *status);
void      fitsw_catcherror(int *status);

#endif  /* FITSW_H */
