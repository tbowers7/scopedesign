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
#include <unistd.h>
#include <xpa.h>                       // XPA Headers for communication with DS9
#include <config.h>

/* Local headers */
#include "display.h"

#define NXPA 10


/* Open a blank DS9 window, and check for operational status using XPA */
void *display_open_ds9(void *status){

  /* Declare Variables */
  char command[100];
  sprintf(command,"%s test_data.fits -zoom 2 &",DS9_PATH);

  printf("Opening DS9 window for use with ScopeDesign...\n");
  system(command);                  // Execute the command
  
  
  /* Now that the DS9 is opening, check for when it is functional using XPA
     before returning this thread back to the main program. */

  /* Declare More Variables */
  int  i, got, stop=0;
  size_t  lens[NXPA];
  char *bufs[NXPA];
  char *names[NXPA];
  char *messages[NXPA];
  
  while(!stop){
    
    sleep(1);    // Give DS9 a moment to load

    /* Get the DS9 frame information */
    got = XPAGet(NULL, "ds9", "frame", "frameno", bufs, lens, names, messages,
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
  
  /***************************************/  
  size_t  lens[NXPA];
  char *bufs[NXPA];
  got = XPAGet(NULL, "ds9", "cmap", NULL, bufs, lens, names, messages,
	       NXPA);
  for(i=0; i<got; i++){
    if( messages[i] == NULL ){
      /* Process buf Contents */
      printf("XPA Connection: %s\nColor Map: >>%s<< (%d)\n",
	     names[i],bufs[i],lens[i]);
      free(bufs[i]);
    }
    else
      fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]);
    if(names[i]) free(names[i]);
    if(messages[i]) free(messages[i]);
  }
  got = XPAGet(NULL, "ds9", "file", NULL, bufs, lens, names, messages,
	       NXPA);
  for(i=0; i<got; i++){
    if( messages[i] == NULL ){
      /* Process buf Contents */
      printf("Filename: >>%s<< (%d)\n",bufs[i],lens[i]);
      free(bufs[i]);
    }
    else
      fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]);
    if(names[i]) free(names[i]);
    if(messages[i]) free(messages[i]);
  }
  got = XPAGet(NULL, "ds9", "zoom", NULL, bufs, lens, names, messages,
	       NXPA);
  for(i=0; i<got; i++){
    if( messages[i] == NULL ){
      /* Process buf Contents */
      printf("Zoom Level: >>%s<< (%d)\n",bufs[i],lens[i]);
      free(bufs[i]);
    }
    else
      fprintf(stderr, "ERROR: %s (%s)\n", messages[i], names[i]);
    if(names[i]) free(names[i]);
    if(messages[i]) free(messages[i]);
  }
  /***************************************/  
  
  printf("Holding horses...\n");
  sleep(5);  // Hold your horses!
  
  xpa = XPAOpen(NULL);
  sprintf(template,"file %s/new-image.fits",DATADIR);
  
  printf("%s (%d)",template,len);
  
  printf("Going to try to open %s\n",template);
  got = XPASet(xpa, "ds9", template, NULL, buf, len,
	       names, messages, NXPA);
  
  printf("Got = %d\n",got);
  /* error processing */
  for(i=0; i<got; i++){
    printf("Names: %s, Messages: %s, Sent: %s, Len: %d\n",
	   names[i],messages[i],template,len);
    if( names[i] )    free(names[i]);
    if( messages[i] ) free(messages[i]);
  }
  
  printf("Holding horses...\n");
  sleep(5);  // Hold your horses!
  
  sprintf(template,"cmap %s",CMAP_A);
  
  
  got = XPASet(xpa, "ds9", template, NULL, buf, len,
	       names, messages, NXPA);
  
  printf("Got = %d\n",got);
  /* error processing */
  for(i=0; i<got; i++){
    printf("Names: %s, Messages: %s, Buffer: %s\n",names[i],messages[i],buf);
    if( names[i] )    free(names[i]);
    if( messages[i] ) free(messages[i]);
  }
  
  printf("Holding horses...\n");
  sleep(5);  // Hold your horses!
  
  sprintf(template,"zoom 0.25");
  
  got = XPASet(xpa, "ds9", template, NULL, buf, len,
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
