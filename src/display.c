/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * Timothy P. Ellsworth Bowers
 *
 * FILE: display.c
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

#include "sd_defs.h"                   // Main Package Headers

/* Include packages */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>                    // Contains "usleep(), ttyslot()"
#include <xpa.h>                       // XPA Headers for communication with DS9
#include <fitsio.h>                    // CFITSIO version #
#include <gsl/gsl_version.h>           // GSL version #

#include <sys/ioctl.h>                 // Contains the TIOCGWINSZ definition

/* Local headers */
#include "display.h"

/* External-Scope Variables -- DS9 XPA Port Information */
char *active_ports[NXPA];              // Port information for open DS9 windows
char *ds9_port;                        // This is the port we are using


/* Procure an open DS9 window for use with ScopeDesign, check status via XPA */
void *display_ds9_open(void *status){
  
  /* Declare XPA-related variables */
  int  i,j,got;
  size_t  lens[NXPA];
  char *bufs[NXPA], *names[NXPA], *messages[NXPA];    // Needed by XPAGet()
  
  /* Declare local function variables */
  int stop=0;
  int wombat=1;
  extern char *active_ports[NXPA];
  extern char *ds9_port;
  char *extant_ports[NXPA];
  char *token,*cp,*save;
  char command[100];
  int nopen,good[NXPA];
  bool verbose = true;
  char *ds9_filename,ds9_title[50];
  
  /* Check for the existance of currently-open DS9 window */
  /* Note: the XPAGet() routine searches ONLY for windows owned by the current
     username... it ignores windows owned by other users of the machine. */
  got = XPAGet(NULL, "ds9", "file", NULL, bufs, lens, names, messages, NXPA);
  nopen = got;
  
  /* If active_ports DS9 windows found, get connection information */
  if( nopen > 0 ) {
    printf("*** Found %d extant DS9 window(s) open on this system. ***\n",
	   nopen);
    display_get_ports(names,nopen,true); // Place port info in active_ports
    for(i=0;i<nopen;i++){
      extant_ports[i] = strdup(active_ports[i]);  // Copy list of extant ports
      
      /* Free arrays from XPAGet(), otherwise memory fills... */
      if( bufs[i] )     free(bufs[i]);
      if( names[i] )    free(names[i]);
      if( messages[i] ) free(messages[i]);
    }
  } else {
    /* If no open windows, we're going to need to FORCE_NEW */
    *((int *)status) = DS9_FORCE_NEW;
  }  
  
  
  /* =======================================================================*/
  /* At this point, port information is in active_ports[k] and loaded       */
  /* filenames are in bufs[k]                                               */
  /* =======================================================================*/
  
  
  
  /* Control Loop -- Use input status to determine how to proceed */
  do
    switch(*((int *)status)){
    case(DS9_FORCE_NEW):
      
      /****************************************************/
      /* Open windows be damned, we're opening a new one! */
      
      printf("Opening a new DS9 window for use with ScopeDesign...\n");
      
      /* Create the system command and send forth! */
      sprintf(ds9_title,"ScopeDesign_%d",ttyslot());                // Unique!
      sprintf(command,"%s -title %s -xpa yes &",DS9_PATH,ds9_title);
      system(command);                  // Execute the command
      
      /* Wait until the new window opens, then register its port information */
      do{
	usleep(500);  // Cool your heels while waiting for new window to open
	
	/* Get the DS9 frame information for window with this title */
	got = XPAGet(NULL, ds9_title, "frame", "frameno", 
		     bufs, lens, names, messages, NXPA);
	
	
	if(got == 1)  // If a window appears with the proper title...
	  break;
      }while(1);
      
      /* Extract port information for all open DS9 windows */
      display_get_ports(names,got,false);
      
      /* Compare extant_ports and active_ports one by one; find the new one! */
      for(i=0;i<got;i++){     // Loop through active_ports
	good[i] = 1;
	for(j=0;j<nopen;j++){ // Compare with extant_ports
	  if( strcmp(active_ports[i],extant_ports[j]) == 0 ){
	    good[i] = 0; 
	    break;            // If match found reset good[i], go on to next i
	  }
	}	  /* Free arrays from XPAGet(), otherwise memory fills... */
	if( bufs[i] )     free(bufs[i]);
	if( names[i] )    free(names[i]);
	if( messages[i] ) free(messages[i]);
      }
      
      /* Assign cannonical PORT value to the 'good' one */
      for(i=0;i<got;i++){
	if(good[i])
	  ds9_port = strdup(active_ports[i]);
      }
      printf("New DS9 window open with port: %s\n",ds9_port);
      
      wombat = 0;     // Set wombat to break the do...while loop.
      break;
      
      
    case(DS9_CANIBALIZE):
      
      /******************************************************************/
      /* I hope you saved what was in that DS9 window... it's mine now! */
      /* Note: This will only canibalize windows owned by the current user */
      
      /* First check for a blank DS9 window */
      for(i=0;i<nopen;i++){
	
	/* The filenames in bufs[i] contain a "\n" at the end */
	if( strlen(bufs[i]) == 1 ){            // If blank (i.e. no open file),
	  ds9_port = strdup(active_ports[i]);  // set ds9_port,
	  wombat = 0;                          // and break the loop.
	}
      } // If blank window, take it and move on...
      
      /* If no blank window, then take over the last one */
      if(wombat !=0)
	ds9_port = strdup(active_ports[nopen-1]);
      
      printf("Using extant DS9 window with port: %s\n",ds9_port);
      
      wombat = 0;     // Set wombat to break the do...while loop.
      break;
      
      
    case(DS9_WHATEVER):
      
      /**************************************************/
      /* The ultimate case of "meh", if I ever saw one. */
      
      /* Check to see if any of the extant DS9 windows are blank */
      for(i=0;i<nopen;i++){
	
	/* The filenames in bufs[i] contain a "\n" at the end */
	if( strlen(bufs[i]) == 1 ){            // If blank (i.e. no open file),
	  ds9_port = strdup(active_ports[i]);  // set ds9_port,
	  wombat = 0;                          // and break the loop.
	}
      }
      
      /* If no blank windows were found, set status = DS9_FORCE_NEW */
      if(wombat != 0)
	*((int *)status) = DS9_FORCE_NEW;
      else
	printf("Using extant blank DS9 window with port: %s\n",ds9_port);
      break;
      
      
    default:
      
      /************************************************************/
      /* If no proper input status, force WHATEVER and loop again */
      
      *((int *)status) = DS9_WHATEVER;  
      
    }                 // End of switch statement 
  while(wombat);      // do...while loop
  
  return status;
}


/* Close DS9 Window using XPA interface */
void display_ds9_close(){
  
  /* Declare Variables */
  int  i, got;
  size_t  len;
  char *buf;
  char *names[NXPA];
  char *messages[NXPA];
  extern char *ds9_port;
  
  printf("Closing DS9 window at port %s...\n",ds9_port);
  
  got = XPASet(NULL, ds9_port, "exit", NULL, buf, len, names, messages, NXPA);
  /* error processing */
  for(i=0; i<got; i++){
    if( messages[i] ){
      fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]);
    }
    if( names[i] )    free(names[i]);
    if( messages[i] ) free(messages[i]);
  }
  
  
  return;
}










/***************************************************************************/
/* Communicate with DS9 */
int display_ds9_talk(int action, scope_display *display){
  
  /* Declare Variables */
  int retval;
  char *handle = "Fun times.";
  
  /* Choose the necessary evil */
  switch(action)
    {
    case DS9_GET:
      retval = display_ds9_xpa_read(display);
      break;
    case DS9_SET:
      /* Determine which handle changed */
      display_ds9_xpa_set(display, handle);
      break;
    default:
      printf("Danger, Will Robinson, no correct action specified!\n");
      retval = -1;
    }
  
  return retval;
}


int display_ds9_xpa_set(scope_display *display, char *handle){

  /* Declare Variables */
  int status=0;
  XPA xpa;
  
  int  i, got;
  size_t  len;
  char *buf;
  char *names[NXPA];
  char *messages[NXPA];
  char template[100];
  extern char *ds9_port;
  
  /***************************************/  
  size_t  lens[NXPA];
  char *bufs[NXPA];
  /* got = XPAGet(NULL, "ds9", "cmap", NULL, bufs, lens, names, messages, */
  /* 	       NXPA); */
  /* for(i=0; i<got; i++){ */
  /*   if( messages[i] == NULL ){ */
  /*     /\* Process buf Contents *\/ */
  /*     printf("XPA Connection: %s\nColor Map: >>%s<< (%d)\n", */
  /* 	     names[i],bufs[i],lens[i]); */
  /*     free(bufs[i]); */
  /*   } */
  /*   else */
  /*     fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]); */
  /*   if(names[i]) free(names[i]); */
  /*   if(messages[i]) free(messages[i]); */
  /* } */
  /* got = XPAGet(NULL, "ds9", "file", NULL, bufs, lens, names, messages, */
  /* 	       NXPA); */
  /* for(i=0; i<got; i++){ */
  /*   if( messages[i] == NULL ){ */
  /*     /\* Process buf Contents *\/ */
  /*     printf("Filename: >>%s<< (%d)\n",bufs[i],lens[i]); */
  /*     free(bufs[i]); */
  /*   } */
  /*   else */
  /*     fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]); */
  /*   if(names[i]) free(names[i]); */
  /*   if(messages[i]) free(messages[i]); */
  /* } */
  /* got = XPAGet(NULL, "ds9", "zoom", NULL, bufs, lens, names, messages, */
  /* 	       NXPA); */
  /* for(i=0; i<got; i++){ */
  /*   if( messages[i] == NULL ){ */
  /*     /\* Process buf Contents *\/ */
  /*     printf("Zoom Level: >>%s<< (%d)\n",bufs[i],lens[i]); */
  /*     free(bufs[i]); */
  /*   } */
  /*   else */
  /*     fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]); */
  /*   if(names[i]) free(names[i]); */
  /*   if(messages[i]) free(messages[i]); */
  /* } */
  /***************************************/  
  
  /* printf("Holding horses...\n"); */
  /* sleep(5);  // Hold your horses! */
  
  xpa = XPAOpen(NULL);
  sprintf(template,"file %s/new-image.fits",DATADIR);
  
  printf("%s (%d)",template,len);
  
  printf("Going to try to open %s\n",template);
  got = XPASet(xpa, ds9_port, template, NULL, buf, len,
	       names, messages, NXPA);
  
  printf("Got = %d\n",got);
  /* error processing */
  for(i=0; i<got; i++){
    printf("Names: %s, Messages: %s, Sent: %s, Len: %d\n",
	   names[i],messages[i],template,len);
    if( names[i] )    free(names[i]);
    if( messages[i] ) free(messages[i]);
  }
  
  /* printf("Holding horses...\n"); */
  /* sleep(5);  // Hold your horses! */
  
  sprintf(template,"cmap %s",CMAP_BB);
  
  
  got = XPASet(xpa, ds9_port, template, NULL, buf, len,
	       names, messages, NXPA);
  
  printf("Got = %d\n",got);
  /* error processing */
  for(i=0; i<got; i++){
    printf("Names: %s, Messages: %s, Buffer: %s\n",names[i],messages[i],buf);
    if( names[i] )    free(names[i]);
    if( messages[i] ) free(messages[i]);
  }
  
  /* printf("Holding horses...\n"); */
  /* sleep(5);  // Hold your horses! */
  
  sprintf(template,"zoom 0.25");
  
  got = XPASet(xpa, ds9_port, template, NULL, buf, len,
	       names, messages, NXPA);
  
  printf("Got = %d\n",got);
  /* error processing */
  for(i=0; i<got; i++){
    printf("Names: %s, Messages: %s, Buffer: %s\n",names[i],messages[i],buf);
    if( names[i] )    free(names[i]);
    if( messages[i] ) free(messages[i]);
  }
  
  /* got = XPAGet(NULL, "ds9", "file", NULL, bufs, lens, names, messages, */
  /* 	       NXPA); */
  
  
  
  
  
  return status;
}


int display_ds9_xpa_read(scope_display *display){
  
  /* Declare Variables */
  
  printf("We're deep within the bowels of the code, executing display_ds9_xpa_read().\n");
  /* Go through the handles, executing XPAGet() for each one and loading them
     into the display structure */
  
  
  return 0;
}


void display_open_test(){
  
  
  
}


void display_get_ports(char **namstr, int ngot, bool verbose){
  
  /* Declare Variables */
  int i;
  char *cp,*token,*save;
  extern char *active_ports[NXPA];
  
  
  /* If active_ports DS9 windows found, get connection information */
  if(ngot){
    for(i=0;i<ngot;i++){
      /* Do string magic to extract the portion after SPACE ... */
      cp = strdup(namstr[i]);           // Make a copy of the name/port sequence
      token = strtok_r(cp," ",&save);   // This is the DS9:ds9 'name'
      token = strtok_r(NULL," ",&save); // This is the port information
      active_ports[i] = strdup(token);
    }
    
    /* Print out information if verbose set */
    if(verbose){
      printf("\t=== Currently open DS9 ports: ===\n");
      for(i=0;i<ngot;i++)
	printf("\t    %s\n",active_ports[i]);
      
    }
  }

  return;
}


/* Display a splash screen or message to terminal */
int display_splash(int input){
  
  /* Declare Variables */
  int i,status = 0;
  char *banner,*line,*text;
  
  /* Split the code here based on whether or not HAVE_GTK is enabled */
#if HAVE_GTK
  
  /************************************************************/
  /* Make some fancy splash screen that wows and impresses... */
  
#else
  
  /****************************************************************/
  /* Print out a text "splash" at the start of the program output */
  
  /* The last known screen width. */
  int screen_width = display_determine_screen_width ();
  
  /* Check output from function, and set width, if necessary */
  if (!screen_width)
    screen_width = DEFAULT_SCREEN_WIDTH;
  else if (screen_width < MINIMUM_SCREEN_WIDTH)
    screen_width = MINIMUM_SCREEN_WIDTH;
  
  /* Make banner of '*' the width of the screen */
  banner = (char *)malloc(sizeof(char) * (screen_width+1));
  text   = (char *)malloc(sizeof(char) * (screen_width+1));
  line   = (char *)malloc(sizeof(char) * (screen_width+1));
  banner[0] = '\0';
  for(i=0;i<screen_width;i++)
    strcat(banner,"*\0");
  
  /* Print out splash to screen */
  printf("\n%s\n",banner);
  
  sprintf(text,"ScopeDesign version %s",PACKAGE_VERSION);  // Line 1
  display_splash_text(text,line,screen_width);
  
  sprintf(text,"Written by Timothy Ellsworth Bowers");  // Line 2
  display_splash_text(text,line,screen_width);
  
  sprintf(text,"");  // Line 3
  display_splash_text(text,line,screen_width);
  
  sprintf(text,"Using DS9 found at %s",DS9_PATH);  // Line 4
  display_splash_text(text,line,screen_width);
  
  sprintf(text,"Using CFITSIO version %d.%d",CFITSIO_MAJOR,CFITSIO_MINOR); 
  display_splash_text(text,line,screen_width);
  
  sprintf(text,"Using GSL version %s",GSL_VERSION);  // Line 6
  display_splash_text(text,line,screen_width);
  
  printf("%s\n\n",banner);
  
  /* Done */
  free(banner);
  free(line);
  free(text);
#endif // No HAVE_GTK... splash to screen
  
  return status;
}

void display_splash_text(char *text, char *line, int width){
  
  /* Declare Variables */
  int i,more;
  
  line[0] = '\0';                    // NULL string
  strcat(line,"*   ");               // Opening space
  strcat(line,text);                 // Input text
  more = width - strlen(line) - 1;   // How much more space available?
  for(i=0;i<more;i++)                // Loop through available space
    strcat(line," ");                // And add a space
  strcat(line,"*\0");                // End with '*' and a NULL
  printf("%s\n",line);               // Print it!
  
  return;
}


/* Routine pulled from GNU WGET (src/utils.c) */
int display_determine_screen_width(void){
  
  /* If there's a way to get the terminal size using POSIX
     tcgetattr(), somebody please tell me.  */
#ifdef TIOCGWINSZ
  int fd;
  struct winsize wsz;
  
  fd = fileno (stderr);
  if (ioctl (fd, TIOCGWINSZ, &wsz) < 0)
    return 0;                   /* most likely ENOTTY */
  
  return wsz.ws_col;
#elif defined(WINDOWS)
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo (GetStdHandle (STD_ERROR_HANDLE), &csbi))
    return 0;
  return csbi.dwSize.X;
#else  /* neither TIOCGWINSZ nor WINDOWS */
  return 0;
#endif /* neither TIOCGWINSZ nor WINDOWS */
  
}
