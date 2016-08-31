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
#include <unistd.h>                    // Contains "sleep()"
#include <xpa.h>                       // XPA Headers for communication with DS9
#include <config.h>                    // Holds the keys to DS9_PATH

/* Local headers */
#include "display.h"

/* External-Scope Variables -- DS9 XPA Port Information */
char *active_ports[NXPA];              // Port information for used DS9 window
char *ds9_port;


/* Open a blank DS9 window, and check for operational status using XPA */
void *display_open_ds9(void *status){
  
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
  
  
  /* Check for the existance of currently-open DS9 window */
  got = XPAGet(NULL, "ds9", "file", NULL, bufs, lens, names, messages, NXPA);
  nopen = got;
  
  /* If active_ports DS9 windows found, get connection information */
  if(nopen > 0){
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
  }
  
  
  /* =======================================================================*/
  /* At this point, port information is in active_ports[k] and loaded 
     filenames are in bufs[k]                                                 */
  /* =======================================================================*/
  
  
  
  /* Control Loop -- Use input status to determine how to proceed */
  do
    switch(*((int *)status)){
    case(DS9_FORCE_NEW):
      
      /****************************************************/
      /* Open windows be damned, we're opening a new one! */
      
      printf("Opening a new DS9 window for use with ScopeDesign...\n");
      
      /* Create the system command and send forth! */
      sprintf(command,"%s &",DS9_PATH);
      system(command);                  // Execute the command
      
      /* Wait until the new window opens, then register its port information */
      do{
	sleep(1);  // Cool your heels while waiting for new window to open
	
	/* Get the DS9 frame information */
	got = XPAGet(NULL, "ds9", "frame", "frameno", 
		     bufs, lens, names, messages, NXPA);
	
	if(got != nopen)
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
      
      printf("We're going to canibalize an existing DS9 window, if we can!\n");
      
      if( nopen == 0 ){
	printf("No open DS9 window, run DS9_FORCE_NEW code...\n");
	*((int *)status) = DS9_FORCE_NEW;
	break;            // Do not change wombat, loop back to FORCE_NEW
      }      
      
      printf("Insert code here to canibalize existing window...\n");
      
      
      wombat = 0;     // Set wombat to break the do...while loop.
      break;


    case(DS9_WHATEVER):
      
      /**************************************************/
      /* The ultimate case of "meh", if I ever saw one. */
      
      printf("We're going to do whatever!\n");
      
      if( nopen == 0 ){
	printf("No open DS9 window, run DS9_FORCE_NEW code...\n");
	*((int *)status) = DS9_FORCE_NEW;
	break;            // Do not change wombat, loop back to FORCE_NEW
      }      
      
      /* Check to see if any of the extant DS9 windows are blank */
      for(i=0;i<nopen;i++){
	printf("File open in window #%d is: %s\n",i+1,bufs[i]);
      }
      
      
      
      wombat = 0;     // Set wombat to break the do...while loop.
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
void display_close_ds9(){
  
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
int display_talk_ds9(){
  
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
