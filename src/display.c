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

char *ds9_port[NXPA];                  // Port information for used DS9 window
char *theport;

/* Open a blank DS9 window, and check for operational status using XPA */
void *display_open_ds9(void *status){
  
  /* Declare XPA-related variables */
  int  i,j,got,nopen;
  size_t  lens[NXPA];
  char *bufs[NXPA], *names[NXPA], *messages[NXPA];    // Needed by XPAGet()
  XPA xpa = XPAOpen(NULL);
  
  /* Declare local function variables */
  int stop=0;
  int wombat = 1;
  extern char *ds9_port[NXPA];
  extern char *theport;
  char *ports[NXPA];
  char *token,*cp,*save;
  char command[100];
  int good[NXPA];
  
  
  /* Check for the existance of currently-open DS9 window */
  got = XPAGet(xpa, "ds9", "file", NULL, bufs, lens, names, messages,
	       NXPA);
  nopen = got;
  printf("At start of display_open_ds9, # of currently open DS9 windows: %d\n",
	 nopen);
  
  /* If ds9_port DS9 windows found, get connection information */
  if(nopen)
    display_get_ports(names,nopen);
  
  for(i=0;i<nopen;i++)
    ports[i] = strdup(ds9_port[i]);
  
  /* At this point, port information is in ds9_ports[k] and loaded filename
     is in bufs[k] */
  
  
  
  /* Control Loop -- Use input status to determine how to proceed */
  do
    switch(*((int *)status)){
    case(DS9_FORCE_NEW):
      printf("We're going to force a new DS9 window!\n");
      
      printf("Despite the existance of %d DS9 windows, open a new one...\n",
	     got);
      sprintf(command,"%s &",DS9_PATH);
      printf("Opening new DS9 window for use with ScopeDesign...\n");
      system(command);                  // Execute the command
      
      /* Wait until the new window opens, then register its port information */
      do{
	sleep(1);  // Cool your heels while waiting for new window to open
	
	/* Get the DS9 frame information */
	got = XPAGet(xpa, "ds9", "frame", "frameno", 
		     bufs, lens, names, messages, NXPA);
	printf("nopen = %d, got = %d\n",nopen,got);
	if(got != nopen)
	  stop = 1;
      }while(!stop);
      
      printf("Okalie dokalie, new window is open... now what?\n");
      /* Extract port information for all open DS9 windows */
      display_get_ports(names,got);
      
      /* Compare ports and ds9_port one by one, finding the new one */
      for(i=0;i<got;i++){
	good[i] = 1;
	for(j=0;j<nopen;j++){
	  if( strcmp(ds9_port[i],ports[j]) == 0 ){
	    good[i] = 0; 
	    break;    // If match found, go on to next i
	  }
	}
      }
      
      /* Assign cannonical PORT value to the 'good' one */
      for(i=0;i<got;i++){
	if(good[i])
	  theport = strdup(ds9_port[i]);
      }
      
      
      wombat = 0;
      break;
    case(DS9_CANIBALIZE):
      printf("We're going to canibalize an existing DS9 window!\n");
      wombat = 0;
      break;
    case(DS9_WHATEVER):
      printf("We're going to do whatever!\n");
      wombat = 0;
      break;
    default:
      *((int *)status) = DS9_WHATEVER;      // If no input, force WHATEVR
    }while(wombat);
  
  
  printf("Value of status: %d\n",*((int *)status));
  
  
  
  /* /\* Declare Variables *\/ */
  /* sprintf(command,"%s test_data.fits -zoom 2 &",DS9_PATH); */

  /* printf("Opening DS9 window for use with ScopeDesign...\n"); */
  /* system(command);                  // Execute the command */

  
  /* Now that the DS9 is opening, check for when it is functional using XPA
     before returning this thread back to the main program. */

  /* Declare More Variables */
  while(!stop){
    
    sleep(10);    // Give DS9 a moment to load
    
    /* Get the DS9 frame information */
    got = XPAGet(xpa, "ds9", "frame", "frameno", bufs, lens, names, messages,
		 NXPA);
    
    for(i=0; i<got; i++){
      if( messages[i] == NULL ){
	free(bufs[i]);
	stop=1;
      }
      else{
	/* error processing */
	fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]);
      }
      if( names[i] )
	free(names[i]);
      if( messages[i] )
	free(messages[i]);
    }
    
  }  
  
  
  got = XPAGet(xpa,ds9_port[0],"file",NULL,bufs,lens,names,messages,NXPA);
  printf("Okay... we've got %d open DS9 windows\n",got);
  
  
  printf("DS9 window is open and ready for use!\n");
    
    
  
  
  return status;
}


/* Close DS9 Window using XPA interface */
void display_close_ds9(){
  
  XPA xpa;
  
  int  i, got;
  size_t  len;
  char *buf;
  char *names[NXPA];
  char *messages[NXPA];
  
  xpa = XPAOpen(NULL);
  got = XPASet(xpa, "ds9", "exit", NULL, buf, len, names, messages, NXPA);
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
  extern char *theport;
  
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
  got = XPASet(xpa, theport, template, NULL, buf, len,
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
  
  
  got = XPASet(xpa, theport, template, NULL, buf, len,
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
  
  got = XPASet(xpa, theport, template, NULL, buf, len,
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


void display_get_ports(char **namstr, int ngot){
  
  /* Declare Variables */
  int i;
  char *cp,*token,*save;
  extern char *ds9_port[NXPA];
  
  
  /* If ds9_port DS9 windows found, get connection information */
  if(ngot){
    for(i=0;i<ngot;i++){
      /* Do string magic to extract the portion after SPACE ... */
      cp = strdup(namstr[i]);           // Make a copy of the name/port sequence
      token = strtok_r(cp," ",&save);   // This is the DS9:ds9 'name'
      token = strtok_r(NULL," ",&save); // This is the port information
      ds9_port[i] = strdup(token);
    }
    
    printf("=== Here are the currently open DS9 ports: ===\n");
    for(i=0;i<ngot;i++)
      printf("%s\n",ds9_port[i]);
    
  }


  return;



}
