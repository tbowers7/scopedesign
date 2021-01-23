/* ScopeDesign
 * 
 * A tool for determining the optical consequences of telescope design
 * through ray tracing and simulated focal planes.
 * 
 * FILE: ui.c
 * 
 * Copyright (C) 2016-2021  Timothy P. Ellsworth Bowers
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

#define wombat extern                  // wombat == protect on N_RAYS
#include "sd_defs.h"                   // Main Package Header
#undef wombat

#if HAVE_GTK3

/* Include packages */
#include <gtk/gtk.h>

/* Local headers */
#include "ui.h"

static void print_hello( GtkWidget *widget,
			 gpointer   data ){
  g_print ("The button was clicked!!!!!\n");
}

static void activate( GtkApplication *app,
		      gpointer        user_data){

  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  window = gtk_application_window_new( app );
  gtk_window_set_title( GTK_WINDOW( window ), "ScopeDesign" );
  gtk_window_set_default_size( GTK_WINDOW( window ), 400, 300 );

  button_box = gtk_button_box_new( GTK_ORIENTATION_HORIZONTAL );
  gtk_container_add( GTK_CONTAINER( window ), button_box );

  button = gtk_button_new_with_label( "Click me to continue..." );
  g_signal_connect( button, "clicked", G_CALLBACK( print_hello ), NULL );
  g_signal_connect_swapped( button, "clicked", G_CALLBACK( gtk_widget_destroy ),
			    window );
  gtk_container_add( GTK_CONTAINER( button_box ), button );

  gtk_widget_show_all( window );
}

int ui_example_window (){
  GtkApplication *app;
  int status;
  int argc = 1;
  char *argv[1];
  
  sprintf(argv[0], "main\0");
  
  app = gtk_application_new ("org.github.tbowers7.scopedesign", 
			     G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  
  return status;
}

#endif
