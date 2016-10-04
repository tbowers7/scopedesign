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

/* Assumed screen width if we can't find the real value.  */
#define DEFAULT_SCREEN_WIDTH 80
/* Minimum screen width we'll try to work with. */
#define MINIMUM_SCREEN_WIDTH 45



/* Function declarations */

/* Public Functions */
void *display_ds9_open(void *status);
int   display_ds9_talk(int action, scope_display *display);
void  display_ds9_close();
int   display_splash(int input);


/* Internal Functions */
void  display_open_test();
void  display_get_ports(char **namstr, int ngot, bool verbose);

void  display_splash_text(char *test, char *line, int width);
int   display_determine_screen_width(void);

int   display_ds9_xpa_set(scope_display *display, char *handle);
int   display_ds9_xpa_read(scope_display *display);

#endif  /* DISPLAY_H */



