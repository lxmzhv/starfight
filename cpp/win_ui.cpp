void PaintStar( HDC& hDC, POINT& sz )
{
   HBRUSH h_brush = CreateSolidBrush( STAR_COLOR );
   HBRUSH h_old_brush = (HBRUSH)SelectObject( hDC, h_brush );

   HPEN h_pen = CreatePen( PS_SOLID, 1, STAR_COLOR );
   HPEN h_old_pen = (HPEN)SelectObject( hDC, h_pen );

   Ellipse( hDC, int(sz.x/2-StarRadius*Scale), int(sz.y/2-StarRadius*Scale), int(sz.x/2+StarRadius*Scale), int(sz.y/2+StarRadius*Scale) );

   SelectObject( hDC, h_old_brush );
   SelectObject( hDC, h_old_pen );

   DeleteObject( h_brush );
   DeleteObject( h_pen );
}

void PaintShips( HDC& hDC, POINT& sz )
{
   HBRUSH h_ship1_brush = CreateSolidBrush( SHIP_COLOR1 );
   HBRUSH h_ship2_brush = CreateSolidBrush( SHIP_COLOR2 );
   HBRUSH h_dead1_brush = CreateSolidBrush( DEAD_SHIP_COLOR1 );
   HBRUSH h_dead2_brush = CreateSolidBrush( DEAD_SHIP_COLOR2 );
   HBRUSH h_ship_brush[4] = { h_ship1_brush, h_ship2_brush, h_dead1_brush, h_dead2_brush };

   HPEN h_ship1_pen = CreatePen( PS_SOLID, 1, SHIP_COLOR1 );
   HPEN h_ship2_pen = CreatePen( PS_SOLID, 1, SHIP_COLOR2 );
   HPEN h_dead1_pen = CreatePen( PS_SOLID, 1, DEAD_SHIP_COLOR1 );
   HPEN h_dead2_pen = CreatePen( PS_SOLID, 1, DEAD_SHIP_COLOR2 );
   HPEN h_ship_pen[4] = { h_ship1_pen, h_ship2_pen, h_dead1_pen, h_dead2_pen };

   HPEN h_red_pen = CreatePen( PS_SOLID, 1, RGB(0xFF,0,0) );
   HPEN h_wred_pen = CreatePen( PS_SOLID, 3, RGB(0xFF,0,0) );
   HPEN h_green_pen = CreatePen( PS_SOLID, 1, RGB(0,0xB0,0) );
   HPEN h_white_pen = CreatePen( PS_SOLID, 1, RGB( 0xFF, 0xFF, 0xFF ) );

   HBRUSH h_old_brush = (HBRUSH)SelectObject( hDC, h_ship1_brush );
   HPEN h_old_pen = (HPEN)SelectObject( hDC, h_white_pen );

   int r = int(ShipRadius*Scale);
   for( long own = 0; own < 2; own++ )
      for( long i = 0; i < ShipTotal; i++ )
         if( own==0 && ship_owners[i] == 0 || own != 0 && ship_owners[i] != 0 )
         {
            POINT p = { long(sz.x/2+ships_x[i]*Scale), long(sz.y/2-ships_y[i]*Scale) };
            POINT d = { long(cos(ships_angle[i])*ShipRadius*Scale), -long(sin(ships_angle[i])*ShipRadius*Scale) }; // Вектор атаки

         // Рисуем сопла
            if( ship_owners[i] && acceler[i] >= NULL_DOUBLE )
            {
               SelectObject( hDC, h_red_pen );
               double mul = acceler[i]/MaxAcceleration;
               MoveToEx( hDC, int(p.x - d.x*(1+mul)), int(p.y - d.y*(1+mul)), 0 );
               LineTo( hDC, p.x - d.x, p.y - d.y );
               SelectObject( hDC, h_wred_pen );
               LineTo( hDC, int(p.x - d.x*(1+mul/2)), int(p.y - d.y*(1+mul/2)) );
            }

         // Рисуем сам корабль
            SelectObject( hDC, ship_owners[i] ? h_ship_pen[ship_owners[i]-1] : h_ship_pen[owners[i]+1] );
            SelectObject( hDC, ship_owners[i] ? h_ship_brush[ship_owners[i]-1] : h_ship_brush[owners[i]+1] );
            Ellipse( hDC, p.x-r, p.y-r, p.x+r, p.y+r );

         // Энергия и целостность
            RECT rc = { p.x-r, p.y+r, p.x+r, p.y+r+4 };

            // Заливаем индикатор белым
            SelectObject( hDC, h_white_pen );
            MoveToEx( hDC, rc.left, rc.top+1, 0 );
            LineTo( hDC, rc.right, rc.top+1 );
            MoveToEx( hDC, rc.left, rc.top+3, 0 );
            LineTo( hDC, rc.right, rc.top+3 );

            // Энергия
            SelectObject( hDC, h_green_pen );
            MoveToEx( hDC, rc.left, rc.top+1, 0 );
            LineTo( hDC, int(rc.left+(rc.right-rc.left)*ships_energy[i]), rc.top+1 );

            // Целосность (здоровье)
            SelectObject( hDC, h_red_pen );
            MoveToEx( hDC, rc.left, rc.top+3, 0 );
            LineTo( hDC, int(rc.left+(rc.right-rc.left)*ships_health[i]), rc.top+3 );

         // Направление атаки
            SelectObject( hDC, h_red_pen );
            MoveToEx( hDC, p.x, p.y, 0 );
            LineTo( hDC, p.x+d.x, p.y+d.y );
         }

   // Атака
   double k = 155.0/MaxShootEnergy;
   for( long i = 0; i < ShootsCount; i++ )
   {
      HPEN h_shoot_pen = CreatePen( PS_SOLID, 1, RGB( int(shoots[i][4]*k)+100,0,0 ) );
      SelectObject( hDC, h_shoot_pen );
      MoveToEx( hDC, sz.x/2 + int(shoots[i][0]*Scale), sz.y/2 - int(shoots[i][1]*Scale), 0 );
      LineTo( hDC, sz.x/2 + int(shoots[i][2]*Scale), sz.y/2 - int(shoots[i][3]*Scale) );
      SelectObject( hDC, h_old_pen );
      DeleteObject( h_shoot_pen );
   }

   SelectObject( hDC, h_old_brush );
   SelectObject( hDC, h_old_pen );

   DeleteObject( h_ship1_brush );
   DeleteObject( h_ship2_brush );
   DeleteObject( h_dead1_brush );
   DeleteObject( h_dead2_brush );

   DeleteObject( h_ship1_pen );
   DeleteObject( h_ship2_pen );
   DeleteObject( h_dead1_pen );
   DeleteObject( h_dead2_pen );

   DeleteObject( h_red_pen );
   DeleteObject( h_wred_pen );
   DeleteObject( h_green_pen );
   DeleteObject( h_white_pen );
}

void PaintSpace( HDC hDC, POINT sz )
{
   HBRUSH h_bg = CreateSolidBrush( RGB( 0,0,0 ) );
   HPEN h_red_pen = CreatePen( PS_SOLID, 1, RGB(0x60,0,0) );
   HPEN h_green_pen = CreatePen( PS_SOLID, 1, RGB(0,0x60,0) );

   HBRUSH h_old_brush = (HBRUSH)SelectObject( hDC, h_bg );

// Рисуем фон
   Rectangle( hDC, 0, 0, sz.x, sz.y );

   HPEN h_old_pen = (HPEN)SelectObject( hDC, h_red_pen );

// Отмечаем опасные и благоприятные орбиты
   const int alarm_r = int(AlarmRadius()*Scale), dead_r = int(DeathRadius*Scale), best_r = int(SafeRadius()*Scale);
   int x = sz.x/2, y = sz.y/2;

   MoveToEx( hDC, x + alarm_r, y, 0 );
   AngleArc( hDC, x, y, alarm_r, 0, 360 );

   MoveToEx( hDC, x + dead_r, y, 0 );
   AngleArc( hDC, x, y, dead_r, 0, 360 );

   SelectObject( hDC, h_green_pen );
   MoveToEx( hDC, x + best_r, y, 0 );
   AngleArc( hDC, x, y, best_r, 0, 360 );

   SelectObject( hDC, h_old_brush );
   SelectObject( hDC, h_old_pen );

   DeleteObject( h_bg );
   DeleteObject( h_red_pen );
   DeleteObject( h_green_pen );
}

void WindowPaint( HDC& hDC, POINT& sz )
{
   PaintSpace( hDC, sz );
   PaintStar( hDC, sz );
   PaintShips( hDC, sz );
}

LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch( msg )
   {
      case WM_CREATE:
         SetTimer( hWnd, 1, SPEED, 0 );
         Init();
         return 0;
      case WM_PAINT:
      {
         PAINTSTRUCT paintStruct;
         HDC hDC = BeginPaint( hWnd, &paintStruct );

         RECT r;
         GetClientRect( hWnd, &r );
         POINT sz = { r.right-r.left, r.bottom - r.top };

         WindowPaint( hDC, sz );

         EndPaint( hWnd, &paintStruct );
         return 0;
      }
      case WM_TIMER:
      {
         const char* res = Step();
         if( !res )
         {
            InvalidateRect( hWnd, 0, false );
            UpdateWindow( hWnd );
         }
         else
         {
            KillTimer( hWnd, 1 );
            MessageBox( hWnd, res, "Конец", MB_OK );
            SetTimer( hWnd, 1, SPEED, 0 );
            end = true;
         }
         return 0;
      }
      case WM_CHAR:
      {
         long old_speed = SPEED;
         switch( wParam )
         {
            case '+':
               if( SPEED > 10 )   SPEED -= 10;
               break;
            case '-':
               SPEED += 10;
               break;
         }
         if( SPEED != old_speed )
         {
            KillTimer( hWnd, 1 );
            SetTimer( hWnd, 1, SPEED, 0 );
         }
         return 0;
      }
      case WM_DESTROY:
         KillTimer( hWnd, 1 );
         PostQuitMessage(0);
         return 0;
   }
   return DefWindowProc(hWnd,msg,wParam,lParam);
}
