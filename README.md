# ScopeDesign

Tool for designing homebuilt telescopes, and an experimental testbed for
students to learn how light propagates through an optical system.




## Required external packages

This piece of OSS requires several external packages in order to compile and/or
run.

1. [DS9](http://ds9.si.edu/site/Home.html) -- For display of FITS images created by the program.

2. [CFITSIO](http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html) -- A library of C
functions for reading and writing FITS files to disk.

3. [GSL](https://www.gnu.org/software/gsl/) -- Contains scientific and other
routines required by the application.

4. [The TPEB library](http://casa.colorado.edu/~ellswotp/scopedesign/) -- 
Various and sundry "helper" or "wrapper" routines.  The ones used by ScopeDesign
may eventually be included as an additional "convenience library".

There are a couple "data" files used as examples in the code that are part of
the [distribution tarball](http://casa.colorado.edu/~ellswotp/scopedesign/) but
not included in the GitHub repository.


## Plan for this application

~~~~
  /************ CODE OUTLINE ************/
  /* 
     0a. Open and/or canibalize a DS9 window for displaying FITS images.
     0b. Let the user know what the heck you are doing to their computer.
     1.  Set up the geometry of the telescope (optic location, shape, etc.)
     2.  Set up the illumination environment (point source, flat, image, etc.)
     3.  Initialize the rays based on illumination environment.
     4.  Propagate the rays through the system.
     4a. Check if rays hit obstructing elements and/or connect with next element
     4b. Reflect / Refract ray off / through element.
     4c. Propagate ray to next element, repeating 4a-4c.
     5.  At each element / pupil, record illumination pattern, if requested.
     6.  Allow for adustment of #1 or #2 and re-run of program.
     7.  Other?
  */
~~~~