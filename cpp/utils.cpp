#include <starfight.h>

void Gravitation( double x, double y, double& gx, double& gy )
{
   double koeff = G / (sqr(x) + sqr(y));
   gx = - x*koeff;
   gy = - y*koeff;
}

// The distance where gravitation is equal to MaxAcceleration
double HighGravitationRadius()
{
  return G / MaxAcceleration;
}

double StarEnergy( double distance )
{
  return EnergyFromStar / sqr(distance);
}

double StarEnergyDistance( double energy )
{
  if( energy < NULL_DOUBLE )
    return LARGE_DOUBLE;
  return sqrt(EnergyFromStar / energy);
}

double AlarmRadius()
{
  return Max( HighGravitationRadius(), StarEnergyDistance(MaxAccEnergy) );
}

double DeathRadius()
{
  return StarEnergyDistance( EnergyPerTurn );
}

double SafeRadius()
{
  return (2*AlarmRadius() + DeathRadius())/3;
}

// Разница углов по модулю (результат от 0 до PI)
double AngleDiff( double ang1, double ang2 )
{
   double diff = Abs( ang1 - ang2 );
   return diff > PI ? TWO_PI - diff : diff;
}

// Разница углов не по модулю (результат от -PI до PI)
double AngleDiffSign( double ang1, double ang2 )
{
   double chg_ang = ang2 - ang1;
   if( chg_ang > PI )          chg_ang -= TWO_PI;
   else if( chg_ang < -PI )    chg_ang += TWO_PI;
   return chg_ang;
}

// Область значений для косинуса и синуса
double CosSinVal( double val )
{
   return val > 1 ? 1 : val < -1 ? -1 : val;
}

double CorrectAngle( double angle )
{
   double res = angle;
   while( res < 0 )      res += TWO_PI;
   while( res >= TWO_PI )  res -= TWO_PI;
   return res;
}

// Определить угол наклона вектора
double VectorAngle( double x1, double y1, double x2, double y2 )
{
   double x = x2-x1, y = y2-y1, d = sqrt( sqr(x) + sqr(y) );
   if( d < NULL_DOUBLE )   return 0;
   double ang = acos( CosSinVal(x/d) );
   if( y < 0 )   ang = TWO_PI - ang;
   return ang;
}

// Определить угол наклона вектора
double VectorAngle( double x, double y, double* dist )
{
   double d = dist ? *dist : sqrt( sqr(x) + sqr(y) );
   double ang = acos( CosSinVal(x/d) );
   return y >= 0 ? ang : TWO_PI - ang;
}

double LimAngle( double chg_angle )
{
   return chg_angle > MaxChangeAngle ? MaxChangeAngle : chg_angle < -MaxChangeAngle ? -MaxChangeAngle : chg_angle;
}

double LimAcceler( double& acc )
{
   return acc >= MaxAcceleration ? MaxAcceleration : acc < 0 ? 0 : acc;
}

double LimShootEnergy( double& shoot_energy )
{
   return shoot_energy > MaxShootEnergy ? MaxShootEnergy : shoot_energy < 0 ? 0 : shoot_energy;
}

// Пересекаем луч (a*x + b*y = c) с кругом (центр: {x0,y0}, радиус: r)
// причём, a = -sin(deg), b = cos(deg), где deg - угол наклона прямой относительно оси Ох
// (x, y) - точка выстрела (нужна для однозначности ответа)
// вернёт квадрат расстояния до пересечения или -1
double Intersect( double a, double b, double c, double x, double y, double x0, double y0, double r, double* xx, double* yy )
{
// Решаем систему - ищем точки пересечения прямой с окружностью
   double x1, x2, y1, y2, a2 = sqr(a), b2 = sqr(b), a2_b2 = a2+b2, d = sqr(r)*a2_b2 - sqr(a*x0+b*y0-c);
   if( d < 0 )   return -1;
   if( Abs(a) > 0.01 )
   {
      double s1 = b*c + a2*y0 - a*b*x0;
      y1 = (s1 + a*sqrt(d)) / a2_b2;
      x1 = (c - b*y1)/a;
      y2 = (s1 - a*sqrt(d)) / a2_b2;
      x2 = (c - b*y2)/a;
   }
   else // Abs(b) > 0.01, т.к. a*a + b*b = cos(deg)*cos(deg) + sin(deg)*sin(deg) = 1
   {
      double s1 = a*c + b2*x0 - a*b*y0;
      x1 = (s1 + b*sqrt(d)) / a2_b2;
      y1 = (c - a*x1)/b;
      x2 = (s1 - b*sqrt(d)) / a2_b2;
      y2 = (c - a*x2)/b;
   }

// Определяем лежат ли точки пересечения на луче
   double vx1 = x1 - x, vy1 = y1 - y;
   if( Abs(vx1) > Abs(vy1) )
   {   if( sign(vx1)!=sign(b) )   return -1;   }
   else
   {   if( sign(vy1)!=sign(-a) )   return -1;  }

// Определяем какая из точек ближе к точке выстрела
   double d1 = sqr(vx1) + sqr(vy1), d2 = sqr(x2-x)+sqr(y2-y);
   if( d1 < d2 )
   {
      if( xx )   *xx = x1;
      if( yy )   *yy = y1;
      return d1;
   }
   if( xx )   *xx = x2;
   if( yy )   *yy = y2;
   return d2;
}
