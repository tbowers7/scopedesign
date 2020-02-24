/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: rays.h
 * 
 * Copyright (C) 2016-2020  Timothy P. Ellsworth Bowers
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

#ifndef RAYS_H
#define RAYS_H


/* Function declarations */
scope_ray *rays_initialize(int ray_setup, int *ray_status, double *overshoot);
double     raytrace_free_distance(scope_ray ray, raytrace_geom geom, int surf);
double     raytrace_distroot(double t, void *params);
void       rays_advance_ray(scope_ray *beam, double d);
scope_ray  raytrace_get_n(scope_ray pos, raytrace_geom geom, int surf);
int        rays_reflect(scope_ray *a, scope_ray n);
void       rays_reflect_x(scope_ray *a, scope_ray n);
void       rays_reflect_y(scope_ray *a, scope_ray n);
void       rays_reflect_z(scope_ray *a, scope_ray n);



#endif  /* RAYS_H */
