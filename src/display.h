/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 * 
 * FILE: display.h
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


#ifndef DISPLAY_H
#define DISPLAY_H


#define CMAP_GRAY "grey"
#define CMAP_BB   "bb"
#define CMAP_A    "a"
#define CMAP_B    "b"

#define NXPA 10


/* Function declarations */
void *display_open_ds9(void *status);
int  display_talk_ds9();

void display_close_ds9();
void display_open_test();
void display_get_ports(char **namstr, int ngot, bool verbose);


#endif  /* DISPLAY_H */



