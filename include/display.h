#pragma once

#include <gtk/gtk.h>
#include <game.h>

const int ShipTrajDisplayLength = 1000;

extern double Scale;
extern DPoint2D Center;
extern int SelectedShip;

void DrawField( GtkWidget* widget, GdkGC* gc );
int XToScreen( double x, GdkPoint sz );
int YToScreen( double y, GdkPoint sz );
double ScreenToX( int x, GdkPoint sz );
double ScreenToY( int y, GdkPoint sz );
