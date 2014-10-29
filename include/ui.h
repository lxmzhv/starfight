#pragma once

#include <gtk/gtk.h>
#include <libintl.h> /* it's needed for translation of strings via gettext */

//#include <game.h>

/* Macros for use with gettext */
#define _( x ) gettext( x )

void NewGame();
//void About( GtkAction *action, GtkWidget* window );
//void Rules( GtkAction *action, GtkWidget* window );
//void Preferences( GtkAction *action, GtkWidget* window );

//extern P<Game> game;
