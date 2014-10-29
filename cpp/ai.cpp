#include <memory.h>
#include <ai.h>

// ���������� ���� �������� � ��������� �� ����. ��� �� ���. � ���. ����������
void AI::RestorePrevMove( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal] )
{
   double gx, gy;
   for( long i = 0; i < ShipTotal; i++ )
      if( owner[i] )
         if( MoveCount )
         {
            PrevChgAngle[i] = angle[i] - PrevAngle[i];
            Gravitation( PrevX[i], PrevY[i], gx, gy );
            PrevAcc[i] = sqrt( sqr(x[i]-PrevX[i]-gx) + sqr(y[i]-PrevY[i]-gy) );
         }
         else
         {
            PrevChgAngle[i] = MaxChangeAngle;
            PrevAcc[i] = MaxAcceleration;
         }
}

// ��������� �������������� ����� ���������� ���� ��������
void AI::CalcNewCoordinates( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal],
                         double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal] )
{
// �������� ���������� ��������� ���� ����� � ��������� ���������� ��� �������� ����
   RestorePrevMove( owner, x, y, vx, vy, angle );

   double gx, gy, ang, acc;
   for( long i = 0; i < ShipTotal; i++ )
      if( owner[i] )
      {
         Gravitation( x[i], y[i], gx, gy );

         ang = CorrectAngle( angle[i] + PrevChgAngle[i] );

         acc = PrevAcc[i];
         if( EnergyForAcceleration*acc > energy[i] )   acc = energy[i]/EnergyForAcceleration;

         NewX[i] = x[i] + vx[i] + gx + acc*cos(ang);
         NewY[i] = y[i] + vy[i] + gy + acc*sin(ang);
      }
}

// ��������� ���� �� ���-������ ������ �� ����� ����
// our - ���, ��� ��������
// enemy - � ���� ����� �������
bool AI::TestShoot( long our, long enemy, double x, double y, double angle, Player owner[ShipTotal] )
{
// �������, ���� �� ���-������ ����� ����� ������� � �����������
   double a = -sin(angle), b = cos(angle), c = a*x + b*y;

   if( Intersect( a, b, c, x, y, 0, 0, StarRadius ) > NULL_DOUBLE )
      return false;
   for( int i = 0; i < ShipTotal; i++ )
      if( i != our && i != enemy && owner[i] )
         if( Intersect( a, b, c, x, y, NewX[i], NewY[i], ShipRadius ) >= NULL_DOUBLE )
            return false;
   return true;
}

// ����� ������� ����� ��������� ������� NewX � NewY
bool AI::FindShoot( long our, long enemy, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal],
                double& chg_ang, double& acc )
{
   if( !owner[enemy] )   return false;

// ����������, ���� ��� ����� ���� ���������� � �������
   double gx, gy;
   Gravitation( x[our], y[our], gx, gy );
   double xx = x[our] + vx[our] + gx;
   double yy = y[our] + vy[our] + gy;

// ���������� ������� ������ ����������, ���������� �� ���� � ������� ������� ������������ ������ �������
   double dx = NewX[enemy] - xx, dy = NewY[enemy] - yy;
   double d = sqrt( sqr(dx) + sqr(dy) );
   double en_ang = VectorAngle( dx, dy, &d );
   double d_a = asin( CosSinVal(ShipRadius/d) );

// ����������, �������� �� ��������� � ���� ���������
   chg_ang = AngleDiffSign( angle[our], en_ang );
   if( Abs(chg_ang) > MaxChangeAngle + d_a )
      return false;

// �������, ���� �� ���-������ ����� ����� ������� � �����������
   chg_ang = LimAngle(chg_ang);
   if( !TestShoot( our, enemy, xx, yy, CorrectAngle( angle[our] + chg_ang ), owner ) )
      return false;

   acc = Min( MaxAcceleration/2, d-2 );
   return true;
}

// ����� ������� ����� ��������� ������� NewX � NewY
bool AI::Shoot( long num, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
            double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal],
            double& chg_ang, double& acc )
{
   double r = 1e+10, dist2, ca = 0, a = 0;
   bool fire = false;
   for( int i = 0; i < ShipTotal; i++ )
      if( owner[i] && owner[i]!=player && i != num && FindShoot( num, i, owner, x, y, vx, vy, angle, energy, ca, a ) )
         if( (dist2 = sqr(NewX[i]-NewX[num]) + sqr(NewY[i]-NewY[num])) < r  )
         {
            chg_ang = ca;
            acc = a;
            r = dist2;
            fire = true;
         }
   return fire;
}

bool AI::FindCapture( long our, long enemy, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                double vx[ShipTotal], double vy[ShipTotal], double energy[ShipTotal],
                double& give_energy )
{
   if( !owner[enemy] )   return false;

// ����������, ���� ��� ����� ���� ���������� � �������
   double gx, gy;
   Gravitation( x[our], y[our], gx, gy );
   double xx = x[our] + vx[our] + gx;
   double yy = y[our] + vy[our] + gy;

// ���������� ������� ������ ����������, ���������� �� ���� � ������� ������� ������������ ������ �������
   double dx = NewX[enemy] - xx, dy = NewY[enemy] - yy;
   double d = sqrt( sqr(dx) + sqr(dy) );
   //double en_ang = VectorAngle( dx, dy, &d );
   //double d_a = asin( CosSinVal(ShipRadius/d) );

// ����������, �������� �� ��������� � ���� ���������
   //chg_ang = AngleDiffSign( angle[our], en_ang );
   //if( Abs(chg_ang) > MaxChangeAngle + d_a )
   //   return false;
   if( d > EnergyTransferRadius )   return false;

// ���������� ������� �� �������, ��� �������
   double enemy_r2 = sqr(NewX[enemy]) + sqr(NewY[enemy]);
   double g_energy = energy[enemy]*EnergyToEnemy;
   if( g_energy >= energy[our] + EnergyPerTurn )   return false;

   give_energy = EnergyPerTurn + (g_energy > 0 ? g_energy : 0);

// �������, ���� �� ���-������ ����� ����� ������� � �����������
   //chg_ang = LimAngle(chg_ang);
   //if( !TestShoot( our, enemy, xx, yy, CorrectAngle( angle[our] + chg_ang ), owner ) )
   //   return false;

   //acc = Min( MaxAcceleration/2, d-2 );
   return true;
}

// ����� ������� ����� ��������� ������� NewX � NewY
bool AI::Capture( long num, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
              double vx[ShipTotal], double vy[ShipTotal], double energy[ShipTotal], double give_energy[ShipTotal][ShipTotal] )
{
   for( int i = 0; i < ShipTotal; i++ )
      if( owner[i] && owner[i]!=player && i != num && FindCapture( num, i, owner, x, y, vx, vy, energy, give_energy[num][i] ) )
         return true;
   return false;
}

// ����������� � ���������� ������������� ����������
// ����� ������� ����� ��������� ������� NewX � NewY
bool AI::TurnToShoot( int num, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                  double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal], double& chg_ang, double& shoot_e, double* distance, long max_num, long* cur )
{
// ����������, ���� ��� ����� ���� ���������� � �������
   double gx, gy;
   Gravitation( x[num], y[num], gx, gy );
   double xx = x[num] + vx[num] + gx;
   double yy = y[num] + vy[num] + gy;

// ���������� ������ �������� ������������� ����������
   double r = 1e+10, en_ang;
   double dist, dx, dy, ang;
   long n = -1;
   for( int i = 0; i < ShipTotal; i++ )
      if( owner[i] && owner[i]!=player && i != num )
      {
      // ���������� ���������� �� ����������
         dx = NewX[i] - xx;
         dy = NewY[i] - yy;
         dist = sqrt( sqr(dx) + sqr(dy) );

         if( distance && cur )
         {
            distance[*cur] = dist;
            *cur = (*cur+1)%max_num;
         }

         if( dist < r  )
         {
         // ���������� ���� ����� ����
            ang = VectorAngle( dx, dy, &dist );

         // ����������, ���� �� ���-������ ������ �� ����� ����
            if( TestShoot( num, i, xx, yy, ang, owner ) )
            {
               r = dist;
               n = i;
               en_ang = ang;
            }
         }
      }

   if( n < 0 )   return false;

// ���������� ��������� ���� �����
   chg_ang = AngleDiffSign( angle[num], en_ang );
   shoot_e = Abs(chg_ang) <= MaxChangeAngle ? MaxShootEnergy : 0;
   chg_ang = LimAngle( chg_ang );

// ���� ������ ���������� ���������, ���������� ����� �� ������ ��������
   if( distance && shoot_e > NULL_DOUBLE )
   {
      double min = 1e+20, max = 0, *end = distance + max_num;
      for( double* d = distance; d < end; d++ )
      {
         if( min > *d )   min = *d;
         if( max < *d )   max = *d;
      }
      double atack_d = min + (max - min)/4;
      if( r - atack_d > NULL_DOUBLE )   shoot_e = 0;
   }

   return true;
}

void AI::CalcMove( int num, double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal], 
               double angle[ShipTotal], double energy[ShipTotal], double& chg_ang, double& acc )
{
   double v_x = vx[num], v_y = vy[num], x0 = x[num], y0 = y[num];
   double v2 = sqr(v_x) + sqr(v_y), v = sqrt(v2);
   double r = v2 > NULL_DOUBLE ? G/v2 : SafeRadius(); // ������ ������ ��� ������� ��������
   if( r > SafeRadius() ) r = AlarmRadius();
   if( r <= StarRadius ) r = AlarmRadius();

// ��������� � �� ��������� �� �� �� ������ ��� ������ (������� r)
   double d2 = sqr(x0) + sqr(y0), d = sqrt(d2);

   // ���������� ���� ����� �������� �������� � ������ �������� �������
   double xx = x0 + v_x, yy = y0 + v_y;
   double dd2 = sqr(xx) + sqr(yy);
   double v_ang = acos( CosSinVal( (v2 + d2 - dd2)/(2*d*v) ) );
   double d_v_ang = Abs(v_ang - HALF_PI);
   double d_r = Abs(d-r);

// ��������� �������� �� �����������
   bool alarm = false;//true; // ����������� ��������
   double alarm_angle = 0;
   if( d < AlarmRadius() )         alarm_angle = PI;
   else if( d > DeathRadius() )  alarm_angle = 0;
   else                          alarm = false;

   if( !alarm )
      if( d_r < ORBIT_R_ERROR && d_v_ang < ORBIT_ANG_ERROR )
      {
   // !!! ����� ������������ ���� ����� ����� ������
         chg_ang = 0;
         acc = 0;
         return;
      }

// ������� �� ������
   // ������� ���� ����� ������-�������� � ������� �������� ��������
   double gx, gy;
   Gravitation( x0, y0, gx, gy );
   double res_v_x = v_x + gx, res_v_y = v_y + gy;
   double d_res2 = sqr(res_v_x) + sqr(res_v_y), d_res = sqrt(d_res2);

   dd2 = sqr(x0 + res_v_x) + sqr(y0 + res_v_y);

   double res_v_ang = acos( CosSinVal( (d_res2 + d2 - dd2)/(2*d*d_res) ) );

   double ang_diff = alarm ? Abs(res_v_ang - alarm_angle) : Abs(res_v_ang - HALF_PI);
   if( ang_diff < ORBIT_ANG_ERROR )
   {
// !!! ����� ������������ ���� ����� ����� ������
      chg_ang = 0;
      acc = alarm ? MaxAcceleration : 0;
   }
   else
   {
   // ���������� ���� ����� ��������� ������� ������ ��������
      int sgn = sign( y0*res_v_x - x0*res_v_y ); // ���������� ��� ������ �������� (����� ��� ������), ������ ������ - ��������� �� ��� Oz
      if( !alarm && res_v_ang > HALF_PI )   sgn *= -1;
      if( !sgn )   sgn = 1; // �� ����� ���� ������������
   // ������ sgn ��������, � ����� ������� ����� �������� ������ �������� (+ �� �������, - ������ �������)
   // ���������� ��������� ��������� ��� ���� �����, ����� ��������� ������ �������� � ������ �����������
      // ����� ���� ������� ������� ��������
      double res_v_angle = acos( CosSinVal(res_v_x/d_res) );
      if( res_v_y < 0 )   res_v_angle = TWO_PI - res_v_angle;
      // ������� ��������� ��������� ��� ���� �����, ����� ��������� ������� ������ ��������
      double need_angle = CorrectAngle( res_v_angle + sgn*HALF_PI );

   // ����������, ��������� ��� ��������� ����� ������� ���� �����
      double cur_ang = angle[num];
      if( need_angle > PI )   sgn = (cur_ang <= need_angle && cur_ang >= (need_angle-PI) ) ? 1 : -1;
      else                    sgn = (cur_ang >= need_angle && cur_ang <= (need_angle+PI) ) ? -1 : 1;
      chg_ang = sgn*MaxChangeAngle;

   // ���������� ���������
      double new_ang = CorrectAngle( cur_ang + chg_ang );
      double diff = Abs(need_angle - new_ang);
      if( diff > PI )   diff = TWO_PI - diff;
      double r_speed = d_r/10;
      if( diff < HALF_PI )   acc = MaxAcceleration*( ang_diff/HALF_PI + r_speed );
   }
}

bool AI::MoveToOrbit( int num, double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal],
                  double angle[ShipTotal], double energy[ShipTotal], double& chg_ang, double& acc )
{
   double v_x = vx[num], v_y = vy[num], x0 = x[num], y0 = y[num];
   chg_ang = acc = 0;
   double max_acceleration = Min( MaxAcceleration, energy[num]/EnergyForAcceleration );
   double max_acc2 = sqr(max_acceleration);

// ������� ������� ������ �������� (��� ����� ��������� ����������) � ������� ��������������
   double gx, gy;
   Gravitation( x0, y0, gx, gy );
   double new_vx = v_x + gx, new_vy = v_y + gy;
   double new_x = x0 + new_vx, new_y = y0 + new_vy;
   double d2 = sqr(new_x) + sqr(new_y), d = sqrt(d2);
   double v2 = sqr(new_vx) + sqr(new_vy), v = sqrt(v2);
   double need_v2 = G/d;

// ���������� ������ ������ ��� ������� ��������
   double r = v2 > NULL_DOUBLE ? G/v2 : SafeRadius();
   if( r <= StarRadius+3*ShipRadius ) r = StarRadius + 5*ShipRadius;
   Maximize( r, 1.05*AlarmRadius() );
   if( r > 1.1*SafeRadius() ) r = SafeRadius();
   Minimize( r, DeathRadius() );
   double rv = sqr(G/r); // ����������� �������� (�� ������ ��������� � v)

// ������� ���� ����� ������-�������� � ������� �������� ��������
   double c2 = sqr(new_x + new_vx) + sqr(new_y + new_vy); // ������ ������� ������������
   double v_ang = acos( CosSinVal( (v2 + d2 - c2)/(2*v*d) ) ); // ���� ����� ������-�������� � ������� �������� ��������

// ��������� ��������� ��������� ������� �������� � ����� ��������������
   double delta = asin( CosSinVal( G/(2*rv*d2) ) ); // ������� ������� ����� ����������� � ������ � ��������� ����������� ���������
   double best_ang = v_ang; // ���� ����� ������ �������� � ������ ���������� ������� �������� � ������ �����
   if( d > r )
   {
      double gamma = asin( CosSinVal(r/d) ); // �������� �������� ������� ������ ��� ������ �����
      best_ang = delta + QUARTER_PI + gamma/2;
   }
   else if( d < r )
   {
      double star_d = d - StarRadius; // ���������� �� ����������� ������ �� �������� ���������
      double star_r = r - StarRadius; // ���������� �� ������ �� ����������� ������
      double gamma = (HALF_PI - delta) * star_d/star_r;
      best_ang = PI - gamma;
   }

// ����� ���� ������� ������� ��������
   double new_v_angle = VectorAngle( new_vx, new_vy, &v );
   double new_v_ang2 = CorrectAngle( new_v_angle + PI );

   double need_angle = 0; // ����������� ��������� ���� �����

   double ang_diff = Abs(v_ang - best_ang);
   double middle_ang = 0;
   if( v >= max_acceleration )
   {
      double max_ang = asin( CosSinVal(max_acceleration/v) ); // ������������ ������� ��������� ������� ��������   
      middle_ang = HALF_PI - Min( ang_diff, max_ang );
   }
   else
      middle_ang = PI - ang_diff;

   bool in_an_orbit = ang_diff < ORBIT_ANG_ERROR && Abs(rv-v) < ORBIT_V_ERROR;
   if( in_an_orbit )   return false;

// ���������� ���� ����� ��������� ������� ������ ��������
   int sgn = sign( new_y*new_vx - new_x*new_vy ); // ���������� ��� ������ �������� (����� ��� ������), ������ ������ - ��������� �� ��� Oz
   if( v_ang > best_ang )   sgn *= -1;
   if( !sgn )   sgn = 1; // �� ����� ���� ������������
// ������ sgn ��������, � ����� ������� ����� �������� ������ �������� (+ �� �������, - ������ �������)
// ���������� ��������� ��������� ��� ���� �����, ����� ��������� ������ �������� � ������ �����������
   //double rot_ang = middle_ang/2 + (r > d ? HALF_PI : 0);
   //double rot_ang = r > d ? 3*middle_ang/2 : middle_ang/2;
   //double rot_ang = need_v2 > v2 || r > d ? 3*middle_ang/2 : middle_ang/2;
   //double rot_ang = r > d ? 3*QUARTER_PI : QUARTER_PI;
   double mul = 0.5; // ���������� (1.5 - ���������)
   if( r > d )
      if( v_ang < HALF_PI )     mul = 1;
      else
        if( v2 > need_v2 )   mul = 0.5;
        else                 mul = 1.5;
   else
      if( v_ang >= HALF_PI )   mul = 0.5;
      else
        if( v2 > need_v2 )  mul = 0.5;
        else                mul = 1.5;
   double rot_ang = middle_ang*mul;
   need_angle = CorrectAngle( new_v_ang2 - sgn*rot_ang );

// ����������, ��������� ��� ��������� ����� ������� ���� �����
   double cur_ang = angle[num];
   sgn = need_angle > PI ? (cur_ang <= need_angle && cur_ang >= (need_angle-PI) ) ? 1 : -1 :
                           (cur_ang >= need_angle && cur_ang <= (need_angle+PI) ) ? -1 : 1;
   chg_ang = sgn*Min( MaxChangeAngle, AngleDiff(cur_ang,need_angle) );

// ���������� ���������
   double new_ang = CorrectAngle( cur_ang + chg_ang );
   double x_acc = cos(new_ang)*max_acceleration/100, y_acc = sin(new_ang)*max_acceleration/100; // ������������ ������� ��������
   double new_vx2 = new_vx + x_acc, new_vy2 = new_vy + y_acc;
   double new_v2_angle = VectorAngle( new_vx2, new_vy2 );

   double diff_new_v = AngleDiff(need_angle,new_v_angle), diff_newv2 = AngleDiff(need_angle,new_v2_angle);
   if( diff_newv2 < diff_new_v )
   {
   // ��� ������ ����� ����� ������ ������� ��������
      double diff_new_v2 = AngleDiff(new_ang,new_v_ang2);
      double new_v_len = sqrt( v2 + max_acc2 - 2*v*max_acceleration*cos(diff_new_v2) );
   // � ������ �������� ������������ ���� ��������� ������� ��������
      double max_ang_diff = new_v_len > NULL_DOUBLE ? asin( CosSinVal( sin(diff_new_v2)*max_acceleration/new_v_len ) ) : 0;
   // ���������� ����������� ���������
      if( ang_diff >= max_ang_diff )
         acc = max_acceleration;
      else
      {
         double sin_val = sin(ang_diff+diff_new_v2);
         acc = sin_val > NULL_DOUBLE ? v*sin(ang_diff)/sin_val : 0;
      }
   }
   return true;
}

bool AI::MoveToOrbit2( int num, double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal],
                   double angle[ShipTotal], double energy[ShipTotal], double& chg_ang, double& acc )
{
   double v_x = vx[num], v_y = vy[num], x0 = x[num], y0 = y[num];
   chg_ang = acc = 0;
   double max_acceleration = Min( MaxAcceleration, energy[num]/EnergyForAcceleration );

// ������� ������� ������ �������� � ������� �������������� (��� ����� ��������� ����������)
   double gx, gy;
   Gravitation( x0, y0, gx, gy );
   double new_vx = v_x + gx, new_vy = v_y + gy;
   double new_x = x0 + new_vx, new_y = y0 + new_vy;
   double d2 = sqr(new_x) + sqr(new_y), d = sqrt(d2);
   double v2 = sqr(new_vx) + sqr(new_vy), v = sqrt(v2);

// ���������� ������ ������ ��� ������� ��������
   double r = v2 > NULL_DOUBLE ? G/v2 : SafeRadius();
   if( r > SafeRadius() ) r = SafeRadius();
   Maximize(r, StarRadius + 2*ShipRadius);
   double rv = sqr(G/r); // ����������� �������� (�� ������ ��������� � v)

// ��������� ��������� ��������� ������� �������� � ����� ��������������
   double delta = asin( CosSinVal( G/(2*rv*d2) ) ); // ������� ������� ����� ����������� � ������ � ��������� ����������� ���������
   double best_ang = 0; // ���� ����� ������ �������� � ������ ���������� ������� �������� � ������ �����
   if( d >= r )
   {
      double gamma = asin( CosSinVal(r/d) ); // �������� �������� ������� ������ ��� ������ �����
      best_ang = PI - (delta + QUARTER_PI + gamma/2);
   }
   else
   {
      double star_d = d - StarRadius; // ���������� �� ����������� ������ �� �������� ���������
      double star_r = r - StarRadius; // ���������� �� ������ �� ����������� ������
      best_ang = (HALF_PI - delta) * star_d/star_r;
   }

// ���������� ���� ����� ��������� ������� ������ ��������
   int sgn = sign( new_x*new_vy - new_y*new_vx); // ���������� ��� ������ �������� (����� ��� ������), ������ ������ - ��������� �� ��� Oz
   if( sgn < 0 )   best_ang = - best_ang;

// ���������� ���������� ������� ������� ��������
   double k = rv/d, x1 = new_x*k, y1 = new_y*k;
   double cos_a = cos(best_ang), sin_a = sin(best_ang);
   double best_vx = cos_a*x1 - sin_a*y1, best_vy = sin_a*x1 + cos_a*y1; //������� ������� �� ���� best_ang

   double dir_x = best_vx - new_vx, dir_y = best_vy - new_vy; // ������ ���������
   double dir_d2 = sqr(dir_x) + sqr(dir_y), dir_d = sqrt(dir_d2);

   if( dir_d < NULL_DOUBLE )   return false;

   double need_angle = acos( CosSinVal(dir_x/dir_d) ); // ����������� ��������� ���� �����
   if( dir_y < 0 )   need_angle = TWO_PI - need_angle;

// ����������, ��������� ��� ��������� ����� ������� ���� �����
   double cur_ang = angle[num];
   sgn = need_angle > PI ? (cur_ang <= need_angle && cur_ang >= (need_angle-PI) ) ? 1 : -1 :
                           (cur_ang >= need_angle && cur_ang <= (need_angle+PI) ) ? -1 : 1;
   chg_ang = sgn*Min( MaxChangeAngle, AngleDiff(cur_ang,need_angle) );

// ����� ���� ������� ������� ��������
   double new_v_angle = 0;
   if( v > NULL_DOUBLE )
   {
      new_v_angle = acos( CosSinVal(new_vx/v) );
      if( new_vy < 0 )   new_v_angle = TWO_PI - new_v_angle;
   }
   else
      new_v_angle = CorrectAngle( need_angle + HALF_PI );
   double new_v_ang2 = CorrectAngle( new_v_angle + PI );

// ���������� ����� �� ���������� ������
   double new_ang = CorrectAngle( cur_ang + chg_ang );
   double diff_need = AngleDiff(new_ang,need_angle);
   double diff_new_v = AngleDiff(new_ang,new_v_angle), diff_new_v2 = AngleDiff(new_ang,new_v_ang2);
   if( diff_need < diff_new_v || diff_need < diff_new_v2 )
      acc = Min( dir_d, max_acceleration );
   return true;
}

bool AI::IsCriticalState( double x, double y )
{
   double d = sqrt( sqr(x) + sqr(y) );
   return d < AlarmRadius() || d > DeathRadius();
}

void AI::ClearMove( double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] )
{
   memset( give_energy, 0, sizeof(double)*ShipTotal*ShipTotal );
   memset( chg_angle, 0, sizeof(double)*ShipTotal );
   memset( acceler, 0, sizeof(double)*ShipTotal );
   memset( shoot_sector, 0, sizeof(double)*ShipTotal );
}

void AI::Move1( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], 
           double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal], double health[ShipTotal], 
           double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] )
{

   ClearMove( chg_angle, acceler, shoot_sector, give_energy );
   CalcNewCoordinates( owner, x, y, vx, vy, angle, energy );

   for( int i = 0; i < ShipTotal; i++ )
      if( owner[i] == player )
      {
         if( !MoveToOrbit( i, x, y, vx, vy, angle, energy, chg_angle[i], acceler[i] ) )
            if( energy[i] >= MIN_ENERGY && Shoot( i, owner, x, y, vx, vy, angle, energy, chg_angle[i], acceler[i] ) )
            {
               shoot_sector[i] = MaxShootEnergy;
               acceler[i] = 0;
            }
            else
            {
               TurnToShoot( i, owner, x, y, vx, vy, angle, chg_angle[i], shoot_sector[i] );
               if( energy[i] < MIN_ENERGY )
                  shoot_sector[i] = 0;
            }
      }
}

void AI::Move2( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], 
           double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal], double health[ShipTotal], 
           double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] )
{
   ClearMove( chg_angle, acceler, shoot_sector, give_energy );
   CalcNewCoordinates( owner, x, y, vx, vy, angle, energy );

   for( int i = 0; i < ShipTotal; i++ )
      if( owner[i] == player )
         if( !MoveToOrbit( i, x, y, vx, vy, angle, energy, chg_angle[i], acceler[i] ) )
            TurnToShoot( i, owner, x, y, vx, vy, angle, chg_angle[i], shoot_sector[i] );
}

void AI::Move3( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], 
           double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal], double health[ShipTotal], 
           double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] )
{
   ClearMove( chg_angle, acceler, shoot_sector, give_energy );
   CalcNewCoordinates( owner, x, y, vx, vy, angle, energy );

   const long mem_len = 500;
   static double distances[ShipTotal*mem_len];
   static long last[ShipTotal];

   for( int i = 0; i < ShipTotal; i++ )
      if( owner[i] == player )
         if( !MoveToOrbit2( i, x, y, vx, vy, angle, energy, chg_angle[i], acceler[i] ) )
            if( !Capture( i, owner, x, y, vx, vy, energy, give_energy ) )
            {
               TurnToShoot( i, owner, x, y, vx, vy, angle, chg_angle[i], shoot_sector[i], distances + mem_len*i, mem_len, last+i );
               if( energy[i] < MIN_ENERGY )   shoot_sector[i] = 0;
            }
}
