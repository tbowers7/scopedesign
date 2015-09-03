/******** ray_funcs.h ********/
/* Timothy Ellsworth Bowers
   13 October 2009

   Header file with structure definitions and function declarations for the 
   subroutine functions needed for the Ray Trace Project (ASTR 5760, F09).
*/


/* Define symbolic integers for the various surfaces in the problem */
#define OPTIC_PRI 20        // Primary Mirror
#define OPTIC_SEC 21        // Secondary Mirror
#define OPTIC_GRS 22        // Spherical Dispersion Grating
#define OPTIC_GRT 23        // Torroidal Dispersion Grating
#define OPTIC_FP  24        // Cassegrain Focal Plane
#define OPTIC_SF  25        // Rowland Circle Spectral Focus

/* Typedef structures needed for this problem */

// "Ray", including position, direction, wavelength & lost flag
typedef struct{
  double x;
  double y;
  double z;
  double vx;
  double vy;
  double vz;
  double lambda;
  int lost;
} raytrace_ray;

// Geometry of the problem, built as a structure for easier passing
typedef struct{
  double f;
  double b;
  double v;
  double e;
  double Dp;
  double Ds;
  double Rrc;
  double alpha;          // In radians
  double d;
} raytrace_geom;

// Parameters needed for passing to the GSL root-finding functions
typedef struct{
  raytrace_ray ray;
  raytrace_geom geom;
  int surf;
} raytrace_root_params;


/* Function declarations */
double       primary_z(double, double, raytrace_geom *);
double       secondary_z(double, double, raytrace_geom *);
double       focalplane_z(double, double, raytrace_geom *);
double       sph_grating_z(double, double, raytrace_geom *);
double       tor_grating_z(double, double, raytrace_geom *);
double       detector_z(double, double, raytrace_geom *);

double       raytrace_free_distance(raytrace_ray, raytrace_geom, int);
double       raytrace_distroot(double, void*);
void         raytrace_advance_ray(raytrace_ray *, double);
raytrace_ray raytrace_get_n(raytrace_ray, raytrace_geom, int);
raytrace_ray raytrace_get_g(raytrace_ray, raytrace_geom, int, double *);
void         raytrace_reflect(raytrace_ray *, raytrace_ray);
void         raytrace_vecray(double, double, raytrace_ray *,
			     raytrace_ray, raytrace_ray);
raytrace_ray raytrace_mapdetector(raytrace_ray, raytrace_geom);
