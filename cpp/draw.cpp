#include <starfight.h>
#include <display.h>

double Scale = 300.0/(2*InitShipDistance);
DPoint2D Center(0,0);

GdkColor RGB( double red, double green, int blue )
{
  GdkColor col;

  col.pixel = 0;
  col.red   = red   * 0xFFFF / 0xFF;
  col.green = green * 0xFFFF / 0xFF;
  col.blue  = blue  * 0xFFFF / 0xFF;

  return col;
}

const GdkColor BLACK            = RGB(0,0,0);
const GdkColor RED              = RGB(0xFF,0,0);
const GdkColor DARK_RED         = RGB(0x80,0,0);
const GdkColor ORANGE           = RGB(0x80,0x80,0);
const GdkColor GREEN            = RGB(0,0xB0,0);
const GdkColor CYAN             = RGB(0,0x70,0xFF);
const GdkColor WHITE            = RGB(0xFF, 0xFF, 0xFF);
const GdkColor STAR_COLOR       = RGB(0xD0,0x40,0);
const GdkColor SHIP_COLOR1      = RGB(0,0xA0,0);
const GdkColor SHIP_COLOR2      = RGB(0,0x70,0xF0);
const GdkColor DIM_SHIP_COLOR1  = RGB(0,0x20,0x10);
const GdkColor DIM_SHIP_COLOR2  = RGB(0,0x10,0x30);

static void DrawCircle( GdkDrawable *drawable, GdkGC* gc, bool filled,
                        const GdkPoint& center, int radius,
                        const GdkColor& color )
{
  gdk_gc_set_rgb_fg_color( gc, &color );
  gdk_draw_arc( drawable, gc, filled, center.x-radius, center.y-radius,
                2*radius, 2*radius, 0, 64*360 );
}

void DrawShips( GdkDrawable *drawable, GdkGC* gc, GdkPoint sz )
{
  GdkColor color;
  GdkRectangle rc;

  int r = int(ShipRadius*Scale);
  for( int j = 0; j < 2; ++j )
    for( int i = 0; i < ShipTotal; i++ )
      {
        // Draw trajectory
        if( ship_owners[i] != pNONE && i == SelectedShip )
        {
          GdkPoint gp1, gp2;
          DPoint2D pnt[2], g, *p1 = NULL, *p2 = NULL;
          DPoint2D v( ships_vx[i], ships_vy[i] );
          double angle = ships_angle[i];
          double angle_chg = chg_angle[i];
          double energy = ships_energy[i];
          double acc = Acceler[i];

          pnt[0].Init( ships_x[i], ships_y[i] );

          GdkColor base_col = (ship_owners[i] == pWHITE ? SHIP_COLOR1 : SHIP_COLOR2), col;
          double brightness;

          for( int n = 0; n < ShipTrajDisplayLength; ++n )
          {
            p1 = &pnt[n%2];
            p2 = &pnt[1-n%2];

            *p2 = *p1;

            MoveShip( *p2, v, angle, angle_chg, energy, acc );

            gp1.x = XToScreen(p1->x, sz);
            gp1.y = YToScreen(p1->y, sz);
            gp2.x = XToScreen(p2->x, sz);
            gp2.y = YToScreen(p2->y, sz);

            brightness = 0.1 + 0.5*(1 - 1.0*n/ShipTrajDisplayLength);
            col.red   = guint16( base_col.red * brightness );
            col.green = guint16( base_col.green * brightness );
            col.blue  = guint16( base_col.blue * brightness );
            gdk_gc_set_rgb_fg_color( gc, &col );
            gdk_draw_line( drawable, gc, gp1.x, gp1.y, gp2.x, gp2.y );

            angle_chg = 0;
            acc = 0;
          }
        }

        GdkPoint p = { XToScreen(ships_x[i], sz), YToScreen(ships_y[i], sz) };
        GdkPoint d = { int(cos(ships_angle[i])*ShipRadius*Scale),
                      -int(sin(ships_angle[i])*ShipRadius*Scale) }; // Atack direction

        // Draw nozzle fire
        if( ship_owners[i] != pNONE && Acceler[i] >= NULL_DOUBLE )
          {
          double mul = Acceler[i]/MaxAcceleration;
          gdk_gc_set_rgb_fg_color( gc, &RED );
          gdk_draw_line( drawable, gc, int(p.x - d.x*(1+mul)), int(p.y - d.y*(1+mul)), p.x - d.x, p.y - d.y );
          gdk_draw_line( drawable, gc, p.x - d.x, p.y - d.y, int(p.x - d.x*(1+mul/2)), int(p.y - d.y*(1+mul/2)) );
          }

        if( j==0 ? ship_owners[i] != pNONE : ship_owners[i] == pNONE )
          continue; // this is needed to display destroyed ships first

        // Draw the vehicle
        DrawCircle( drawable, gc, true, p, r, ship_owners[i]==pWHITE ? SHIP_COLOR1 :
                                              ship_owners[i]==pBLACK ? SHIP_COLOR2 :
                                              Owners[i]==pWHITE ? DIM_SHIP_COLOR1 :
                                                                  DIM_SHIP_COLOR2 );
        if( i == SelectedShip )
          DrawCircle( drawable, gc, false, p, r, RED );

        // Draw state bars
        rc.x = p.x-r;
        rc.y = p.y+r;
        rc.width = 2*r;
        rc.height = 4;

        // Fill the bars with white
        gdk_gc_set_rgb_fg_color( gc, &WHITE );
        gdk_draw_line( drawable, gc, rc.x, rc.y+1, rc.x + rc.width, rc.y+1 );
        gdk_draw_line( drawable, gc, rc.x, rc.y+3, rc.x + rc.width, rc.y+3 );

        // Energy
        if( ships_energy[i] > 0 )
        {
          gdk_gc_set_rgb_fg_color( gc, &CYAN );
          gdk_draw_line( drawable, gc, rc.x, rc.y+1, int(rc.x+rc.width*ships_energy[i]), rc.y+1 );
        }

        // Health
        if( ships_health[i] > 0 && ship_owners[i] != pNONE )
        {
          gdk_gc_set_rgb_fg_color( gc, &RED );
          gdk_draw_line( drawable, gc, rc.x, rc.y+3, int(rc.x+rc.width*ships_health[i]), rc.y+3 );
        }

        // Atack direction
        gdk_gc_set_rgb_fg_color( gc, &RED );
        gdk_draw_line( drawable, gc, p.x, p.y, p.x+d.x, p.y+d.y );
      }

  // Laser atacks
  double k = 155.0/MaxShootEnergy;
  for( int i = 0; i < ShootsCount; i++ )
  {
    color = RGB( int(Shoots[i][4]*k)+100, 0, 0 );
    gdk_gc_set_rgb_fg_color( gc, &color );
    gdk_draw_line( drawable, gc, XToScreen(Shoots[i][0], sz),
                                 YToScreen(Shoots[i][1], sz),
                                 XToScreen(Shoots[i][2], sz),
                                 YToScreen(Shoots[i][3], sz) );
  }
}

void DrawField( GtkWidget* widget, GdkGC* gc )
{
  GdkDrawable *drawable = GDK_DRAWABLE( widget->window );
  GdkPoint     sz = { widget->allocation.width, widget->allocation.height };
  GdkPoint     center = {XToScreen(0, sz), YToScreen(0, sz)};

  // Fill the Universe with black
  gdk_gc_set_rgb_fg_color( gc, &BLACK );
  gdk_draw_rectangle( drawable, gc, TRUE, 0, 0, sz.x, sz.y );

  // Draw the star
  DrawCircle( drawable, gc, true, center, int(StarRadius*Scale), STAR_COLOR );

  // Draw the dangerous area
  DrawCircle( drawable, gc, false, center, int(AlarmRadius()*Scale), ORANGE );

  // Draw the best area
  DrawCircle( drawable, gc, false, center, int(SafeRadius()*Scale), GREEN );

  // Draw the dead area
  DrawCircle( drawable, gc, false, center, int(DeathRadius()*Scale), DARK_RED );

  // Draw the vehicles
  DrawShips( drawable, gc, sz );
}
