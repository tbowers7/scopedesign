/******** ray_trace.c ********/
/* Timothy Ellsworth Bowers
   13 October 2009

   This is the Ray-Trace project for ASTR 5760 (Instrumentation).  This
   project comes in three parts:

   A - Due Thursday, September 10, 2009
       Ray Trace a simple cassegrain (primary: 1-m f/3, confocal secondary,
       system f/20, with on-axis focus 100 mm behind primary vertex) at
       various incident angles.

   B - Due Thursday, September 24, 2009
       Add a Rowland circle spectrograph.  Grating: 3600 l/mm, R_c = 2000 mm,
       R=20,000 at 1500A.  Do ray-trace of system every 100A from 1200A - 
       2000A, assuming a matched cylindrical detector.

   C - Due Tueesday, October 13, 2009
       Make grating toroidal to account for the astagmatism (R_c in spectral
       dimension is unchanged.  Choose toroid carefully, and ray-trace same
       lambdas as part B.

   ---------------------------
   
   The program accepts the following command-line input:

   -v, --verbose      Be verbose in the output
   -w, --where        Print which optical surface we're at
   
   --usegrid          Start rays on a regular grid instead of random locations
   --single           Use a single ray straight down the optical axis
   
   --angle=ANGLE      Off-boresight angle of starting light IN ARCSEC (default
                      value: 0.0).
   --lambda=WAVELEN   Wavelength of the light ray IN ANGSTROMS (default: 1500A)

   --print_primary    Print out ray positions upon contact with the primary
   --print_secondary  Print out ray positions upon contact with the secondary
   --print_focalplane Print out ray positions upon contact with the focal plane
   --print_grating    Print out ray positions upon contact with the grating
   --print_detector   Print out ray positions upon contact with the detector

   --ftest            Run a test of the telescope f/#
   --stop_at_focus    Stop the ray trace at the focal plane
   
   ---------------------------
   
   Functions for this project are defined in the "ray_funcs.h" header
   file.
   
   Other information:
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argtable2.h>                 // Includes the ARGTABLE routine defs
#include <gsl/gsl_math.h>              // Includes the gsl_hypot3() function
#include <gsl/gsl_rng.h>               // Includes GSL's rng routine defs
#include <mylib/fileio.h>              // Included file I/O shortcuts
#include "ray_funcs.h"                 // Program-dependent defs

void print_usage();                    // Delaration for print_usage() function

int main(int argc, char *argv[]){
  
  /* Variable Declarations */
  int i,j,k,n_rays=121*121;
  double t,sec_rad[n_rays],delta;
  char filename[50];
  FILE *fpp;
  raytrace_geom geom;
  raytrace_ray *rays,normal,g,det_plane;
  const gsl_rng_type *T;
  gsl_rng *r;
  
  rays = (raytrace_ray *)malloc(n_rays * sizeof(raytrace_ray));
  
  /* Setup GSL's RNG */
  gsl_rng_env_setup();
  
  T = gsl_rng_ranlux389;
  r = gsl_rng_alloc(T);
  
    /* ARGTABLE SECTION -- EXTRA INDENT */
    // Variables needed
    int nerrors,be_verbose,iang,use_grid,do_foctest,stop_foc,single,be_where;
    int pp,ps,pf,pg,pd;                  // Print outputs
    double angle,wavelen;                // Angle in radians -- used in x dir
    
    // Build the ARGTABLE
    struct arg_int *ang  = arg_int0(NULL,"angle","<arcsec>",
				    "Off-axis light angle");
    struct arg_dbl *lam  = arg_dbl0("l","lambda","<angstroms>",
				    "Wavelength of light ray");
    struct arg_lit *ver  = arg_lit0("v","vebose","verbose output");
    struct arg_lit *where= arg_lit0("w","where","print where we're at");
    struct arg_lit *grd  = arg_lit0(NULL,"usegrid","Use a regular grid");
    struct arg_lit *st   = arg_lit0(NULL,"single",
				    "test with a single, on-axis ray");
    struct arg_lit *ft   = arg_lit0(NULL,"ftest",
				    "Perform f/# test (angle is ignored)");
    struct arg_lit *pri  = arg_lit0(NULL,"print_primary",
				    "print ray positions at primary");
    struct arg_lit *sec  = arg_lit0(NULL,"print_secondary",
				    "print ray positions at secondary");
    struct arg_lit *fp   = arg_lit0(NULL,"print_focalplane",
				    "print ray positions at focal plane");
    struct arg_lit *gr   = arg_lit0(NULL,"print_grating",
				    "print ray positions at grating");
    struct arg_lit *det  = arg_lit0(NULL,"print_detector",
				    "print ray positions at detector");
    struct arg_lit *sf   = arg_lit0(NULL,"stop_at_focus",
				    "stop the ray trace at focal plane");
    struct arg_lit *help = arg_lit0(NULL,"help","print this help");
    struct arg_end *end  = arg_end(20);
    void *argtable[] = {ang,lam,grd,pri,sec,fp,gr,det,ft,st,sf,where,ver,
			help,end};
    
    /* Check for null arguments */
    if (arg_nullcheck(argtable) != 0){
      printf("error: insufficient memory\n");  
      exit(1);
    }
    
    /* Initialize command line flags to defaults */
    ang->ival[0] = 0;                        // Incoming light angle  = 0
    lam->dval[0] = 1500;                     // Default = 1500A
    
    /* Parse the command line */
    nerrors = arg_parse(argc,argv,argtable);
    
    /* If help ---> */
    if(help->count){
      print_usage();
      arg_print_syntaxv(stderr, argtable, "\n\n");
      arg_print_glossary(stdout, argtable, " %-25s %s\n");  
      exit(1);
    }
    
    /* If any errors */
    if(nerrors > 0){
      arg_print_errors(stderr,end,"ray_trace");
      arg_print_syntaxv(stderr, argtable, "\n");
      exit(1);
    }
    
    /* Put parsed command options into usual variables */
    be_verbose = ver->count;
    be_where   = where->count;
    use_grid   = grd->count;
    do_foctest = ft->count;
    stop_foc   = sf->count;
    single     = st->count;
    pp         = pri->count;
    ps         = sec->count;
    pf         = fp->count;
    pg         = gr->count;
    pd         = det->count;
    iang       = ang->ival[0];
    angle      = (double)(iang) / 206265.;
    wavelen    = lam->dval[0];
    
  
  /* Define Gemetrical Parameters of the system -- EVERYTHING IN METERS */
    
  geom.f = 3.0;      // Focal length of primary
  geom.b = 0.100;    // Back-plane distance (focal plane, behind primary vertex)
  geom.v = 4.0;      // Distance of primary vertex from 'starting point' (arb)
  
  geom.Dp = 1.0;     // Primary Diameter
  
  geom.e = 23./17.;  // From focal-ratios: eps =  (final+pri)/(final-pri)
  
  geom.Ds = (geom.f + geom.b) * (geom.e - 1.) / 
    (6. * geom.e);   // Secondary Diameter (on-axis)
  
  // Quantities related to the Rowland Circle Spectrograph
  geom.Rrc = 2.0;    // Diamter of Rowland Circle = radius of grating curvature
  geom.d   = 10000./3.6; // 3600 l/mm converted to angstroms per line
  
  //geom.alpha = asin(0.20);
  //geom.alpha = asin(0.54); // Centering lambda=1500A at beta=0 angle (radians)
  
  /* Center the spectrum at 1600A */
  geom.alpha = asin(0.576);// Centering lambda=1600A at beta=0 angle (radians)
  
  printf("Eccentricity: %.5f, Secondary Diameter: %.2f cm\n",
	 geom.e,geom.Ds*100.);
  
  /* Build array of starting rays, including x & v */
  
  // Focal # test
  if(do_foctest){
    
    // Reallocate space, if not enough was allocated in the first place
    if(n_rays < 12)
      rays = (raytrace_ray *)malloc(12 * sizeof(raytrace_ray));
    
    n_rays = 12;
    for(i=0; i < n_rays; i++){
      
      rays[i].x = 0.5 * cos( (double)i * M_PI / 6.);
      rays[i].y = 0.5 * sin( (double)i * M_PI / 6.);
      rays[i].z = 0.;
      
      rays[i].vx = 0.;
      rays[i].vy = 0.;
      rays[i].vz = -1.0;
      
      rays[i].lost = 0;
      rays[i].lambda = wavelen;
      
    }
    // Single-ray test
  }else if(single){
    
    n_rays = 1;
    
    rays[0].x = 0.0;
    rays[0].y = 0.0;
    rays[0].z = 0.0;
    
    rays[0].vx = 0.0;
    rays[0].vy = 0.0;
    rays[0].vz = -1.0;
    
    rays[0].lost = 0;
    rays[0].lambda = wavelen;
    
    // Regular Grid
  }else if(use_grid)
    for(i=0; i<sqrt(n_rays); i++){
      for(j=0; j<sqrt(n_rays); j++){
	
	k = (int)(i*sqrt(n_rays) + j);
	/* Set positions */
	rays[k].x = (double)i * 0.01 - 0.6;
	rays[k].y = (double)j * 0.01 - 0.6;
	// rays[i+j].y = +(0.1 * (double)(j+1)) * cos( (double)i * M_PI / 6.);
	rays[k].z = +0.0;
	
	/* Set velocities */
	rays[k].vx = sin(angle);
	rays[k].vy = +0.0;
	rays[k].vz = -cos(angle);
	
	/* The ray is not yet lost */
	rays[k].lost = 0;
	rays[k].lambda = wavelen;
      }
    }
  // Random points
  else
    for(i=0; i< n_rays; i++){
      rays[i].x = gsl_rng_uniform(r)*1.2 - 0.6;  // Random from [-0.6,0.6]
      rays[i].y = gsl_rng_uniform(r)*1.2 - 0.6;  // Random from [-0.6,0.6]
      rays[i].z = +0.0;
      
      /* Set velocities */
      rays[i].vx = sin(angle);
      rays[i].vy = +0.0;
      rays[i].vz = -cos(angle);
      
      /* The ray is not yet lost */
      rays[i].lost = 0;
      rays[i].lambda = wavelen;
    }
  
  /* If selected, open file for primary ray locations */
  if(pp){
    sprintf(filename,"rt_primary_ang%-d.dat",iang);
    fpp = fileopenw(filename);
  }
  
  
  /* Loop over rays, from starting point to the primary mirror */
  if(be_where)
    printf("Rays headed towards the primary...\n");
  for(i=0; i<n_rays; i++){
    
    /* Use GSL root-finder to caluclate t for each ray from start -> primary */
    t = raytrace_free_distance(rays[i], geom, OPTIC_PRI);
    
    if(be_verbose)
      printf("\nValue of t returned from function: %.5f m\n",t);
    
    /* Advance ray to primary */
    raytrace_advance_ray(&rays[i],t);
    
    /* Check to see that ray actually hit the 1-m (diameter) primary */
    if(hypot(rays[i].x, rays[i].y) > (0.5 + 1.e-10))  // To catch round-off err
      rays[i].lost = 1;
    
    /* If selected, write out ray locations to file */
    if(pp && !rays[i].lost)
      fprintf(fpp,"%g\t%g\t%f\n",rays[i].x,rays[i].y,rays[i].z);
    
  } // End of loop: starting point to primary
  
  /* Close primary mirror file, if selected */
  if(pp)
    fclose(fpp);
  

  /* Loop over rays, find reflected v vector off primary */
  for(i=0; i<n_rays; i++){
    
    /* Only do calculation for rays not lost */
    if(!rays[i].lost){
      
      /* Find normal to the surface at reflection point */
      normal = raytrace_get_n(rays[i], geom, OPTIC_PRI);
      
      if(be_verbose)
	printf("\nNormal vector: [%.5f,%.5f,%.5f], 1-|n| = %.5e\n",normal.x,
	       normal.y,normal.z,1.-gsl_hypot3(normal.x,normal.y,normal.z));
      
      /* Reflect rays -- update velocities */
      raytrace_reflect(&rays[i],normal);
    }    
  } // End of loop: reflect off primary
  
  /* If selected, open file for secondary ray locations */
  if(ps){
    sprintf(filename,"rt_secondary_ang%-d.dat",iang);
    fpp = fileopenw(filename);
  }
  
  
  /* Loop over rays, from primary to secondary mirror */
  if(be_where)
    printf("Rays headed towards the secondary...\n");
  for(i=0; i<n_rays; i++){
    
    /* Only do calculation for rays not lost */
    if(!rays[i].lost){
      
      /* Use GSL root-finder to caluclate t for each ray from pri -> sec */
      t = raytrace_free_distance(rays[i], geom, OPTIC_SEC);
      
      if(be_verbose)
	printf("\nValue of t returned from function: %.5f m\n",t);
      
      /* Advance ray to sec */
      raytrace_advance_ray(&rays[i],t);
      sec_rad[i] = hypot(rays[i].x,rays[i].y);   // Check size of secondary
      if(be_verbose)
	printf("Radius of ray at seconday: %.5f cm\n",sec_rad[i]*100.);
      
      /* If selected, write out ray locations to file */
      if(ps)
	fprintf(fpp,"%g\t%g\t%f\n",rays[i].x,rays[i].y,rays[i].z);
    }
  } // End of loop: primary to secondary
  
  /* Close secondary mirror file, if selected */
  if(ps)
    fclose(fpp);
  
  
  /* Loop over rays, find reflected v vector off secondary */
  for(i=0; i<n_rays; i++){
    
    /* Only do calculation for rays not lost */
    if(!rays[i].lost){
      
      /* Find normal to the surface at reflection point */
      normal = raytrace_get_n(rays[i], geom, OPTIC_SEC);
      
      if(be_verbose)
	printf("\nNormal vector: [%.5f,%.5f,%.5f], 1-|n| = %.5e\n",normal.x,
	       normal.y,normal.z,1.-gsl_hypot3(normal.x,normal.y,normal.z));
      
      /* Reflect rays -- update velocities */
      raytrace_reflect(&rays[i],normal);
      
      if(do_foctest)
	printf("Opening angle of extreme rays (in f/#): %.5f\n",
	       0.5 / tan(acos(fabs(rays[i].vz))));
      
    }
  } // End of loop: reflect off secondary
  
  
  /*
   * BRANCHING POINT IN CODE:  EITHER TRACE TO THE FOCAL PLANE AND EXIT --
   * if() -- OR TRACE TO THE GRATING AND ONTO THE DETECTOR -- else --.
   */
  
  
  /* If selected, trace to the focal plane and exit */
  if(stop_foc){
    
    /* If selected, open file for focal plane ray locations */
    if(pf){
      sprintf(filename,"rt_focalplane_ang%-d.dat",iang);
      fpp = fileopenw(filename);
    }
    
    /* Loop over rays, from secondary to focal plane */
    for(i=0; i<n_rays; i++){
      
      /* Only do calculation for rays not lost */
      if(!rays[i].lost){
	
	/* Use GSL root-finder to caluclate t for each ray from sec -> f.p. */
	t = raytrace_free_distance(rays[i], geom, OPTIC_FP);
	
	if(be_verbose)
	  printf("\nValue of t returned from function: %.5f m\n",t);
	
	/* Advance ray to focal plane */
	raytrace_advance_ray(&rays[i],t);
	
	/* If selected, write out ray locations to file */
	if(pf)
	  fprintf(fpp,"%.10g   %.10g   %.10f\n",rays[i].x,rays[i].y,rays[i].z);
	
	
	if(be_verbose)
	  printf("Final location of ray: (%10.3g,%10.3g,%10.3f)\n",rays[i].x,
		 rays[i].y,rays[i].z);
      }
    } // End of loop: secondary to focal plane
    
    /* Close focal plane file, if selected */
    if(pf)
      fclose(fpp);
  }  
  
  
  /* Else, trace to the grating */
  else{
    
    /* If selected, open file for grating ray locations */
    if(pg){
      sprintf(filename,"rt_grating_ang%-d.dat",iang);
      fpp = fileopenw(filename);
    }
    
    /* Loop over rays, from secondary to diffraction grating */
    if(be_where)
      printf("Rays headed towards the grating...\n");
    for(i=0; i<n_rays; i++){
      
      /* Only do calculation for rays not lost */
      if(!rays[i].lost){
	
	/* Use GSL root-finder to caluclate t for each ray from sec -> grat */
	/* GRS == spherical grating, GRT == torroidal grating */
	t = raytrace_free_distance(rays[i], geom, OPTIC_GRT);
	
	if(be_verbose)
	  printf("\nValue of t returned from function: %.5f m\n",t);
	
	/* Advance ray to focal plane */
	raytrace_advance_ray(&rays[i],t);
	
	/* If selected, write out ray locations to file */
	if(pg)
	  fprintf(fpp,"%.10g   %.10g   %.10f\n",rays[i].x,rays[i].y,rays[i].z);
	
	
	if(be_verbose)
	  printf("Final location of ray: (%10.3g,%10.3g,%10.3f)\n",rays[i].x,
		 rays[i].y,rays[i].z);
      }
    } // End of loop: secondary to grating
    
    /* Close grating file, if selected */
    if(pg)
      fclose(fpp);
    
    
    /* Loop over rays, find reflected v vector off grating */
    for(i=0; i<n_rays; i++){
      
      /* Only do calculation for rays not lost */
      if(!rays[i].lost){
	
	/* Find normal to the surface at reflection point */
	normal = raytrace_get_n(rays[i], geom, OPTIC_GRT);
	
	/* Find the groove vector g at reflection point */
	g = raytrace_get_g(rays[i], geom, OPTIC_GRT, &delta);
	
	if(be_verbose){
	  printf("\nNormal vector:   [%.5f,%.5f,%.5f], 1-|n| = %.5e\n",normal.x,
		 normal.y,normal.z,1.-gsl_hypot3(normal.x,normal.y,normal.z));
	  printf("Grating vector:  [%.5f,%.5f,%.5f], 1-|n| = %.5e\n",
		 g.x,g.y,g.z,1.-gsl_hypot3(g.x,g.y,g.z));
	}
	
	/* Bounce light off grating -- update velocities */
	raytrace_vecray(-1.,geom.d/delta,&rays[i],normal,g);
	
	//	if(be_verbose)
	  printf("Outgoing vector:  [%.5f,%.5f,%.5f], 1-|n| = %.5e\n",
		 rays[i].vx,rays[i].vy,rays[i].vz,
		 1.-gsl_hypot3(rays[i].vx,rays[i].vy,rays[i].vz));
	  }
    } // End of loop: diffraction off grating
    
    
    /* If selected, open file for detector ray locations */
    if(pd){
      sprintf(filename,"rt_detector_ang%-d.dat",iang);
      fpp = fileopenw(filename);
    }
    
    /* Loop over rays, from grating to cylindrical detector */
    if(be_where)
      printf("Rays headed towards the detector...\n");
    for(i=0; i<n_rays; i++){
      
      /* Only do calculation for rays not lost */
      if(!rays[i].lost){
	
	/* Use GSL root-finder to caluclate t for each ray from grat -> det */
	t = raytrace_free_distance(rays[i], geom, OPTIC_SF);
	
	if(be_verbose)
	  printf("\nValue of t returned from function: %.5f m\n",t);
	
	/* Advance ray to focal plane */
	raytrace_advance_ray(&rays[i],t);
	
	/* If selected, write out ray locations to file */
	if(pd)
	  fprintf(fpp,"%.12g   %.12g   %.12f\n",rays[i].x,rays[i].y,rays[i].z);
	
	if(be_verbose)
	  printf("Final location of ray: (%10.3g,%10.3g,%10.3f)\n",rays[i].x,
		 rays[i].y,rays[i].z);
      }
    } // End of loop: grating to detector
    
    /* Close detector file, if selected */
    if(pd)
      fclose(fpp);
    
    
    /* MAP FINAL RAY POSITIONS ONT CYLINDRICAL DETECTOR */
    sprintf(filename,"detector_plane_lambda%-.2f.dat",rays[0].lambda);
    fpp = fileopenw(filename);
    
    for(i=0; i<n_rays; i++){
      
      /* Only do calculation for rays not lost */
      if(!rays[i].lost){
	
	det_plane = raytrace_mapdetector(rays[i], geom);
	
	if(be_verbose)
	  printf("Detector Plane Values: [%.5f,%.5f,%.5g]\n",
		 det_plane.x,det_plane.y,det_plane.z);
	
	fprintf(fpp,"%.12g     %.12g\n",det_plane.x,det_plane.y);
	
      }
    } // End of loop: maping detector
    
    fclose(fpp);
    
  } // End of the ELSE clause (i.e. secondary --> detector)
  
  /* Clean up */
  gsl_rng_free(r);
  
  return 0;
}


/* prints out usage information if command line arguments are not correct */
void print_usage(){
  
  printf("\nray_trace:\n");
  printf("Ray trace program for Instrumentation (ASTR 5760, F'09).  Traces\n");
  printf("rays through a Cassegrain Telescope / Corrected Rowland Circle\n");
  printf("Spectrograph.\n");
  printf("     Uses GSL, and ARGTABLE libraries.\n");
  printf("\n");
  printf("usage: ray_trace\n");
  
  return;
}
