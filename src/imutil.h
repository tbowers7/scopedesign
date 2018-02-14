/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: imutil.h
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


#ifndef IMUTIL_H
#define IMUTIL_H


/* ========================= */
/*   Function Declarations   */
/* ========================= */

double **imutil_alloc_2darray(long *);
void     imutil_free_2darray(double **, long *);
double  *imutil_2d_to_1d(double **, long *);
double **imutil_get_subsection(double **, long *, long *, long *);
void     imutil_transpose(double **, double **, int, int);


#endif  /* IMUTIL_H */
