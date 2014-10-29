#include <memory.h>
#include <game.h>

Player ship_owners[ShipTotal]; // pNONE - Dead, pWHITE - First, pBLACK - Second
double ships_x[ShipTotal];
double ships_y[ShipTotal];
double ships_vx[ShipTotal];
double ships_vy[ShipTotal];
double ships_angle[ShipTotal];
double ships_energy[ShipTotal];
double ships_health[ShipTotal];

double chg_angle[ShipTotal];
double Acceler[ShipTotal];
double shoot_energy[ShipTotal];
double give_energy[ShipTotal][ShipTotal];

double chg_angle2[ShipTotal];
double Acceler2[ShipTotal];
double shoot_energy2[ShipTotal];
double give_energy2[ShipTotal][ShipTotal];

int move_cnt = 0;

int ShootsCount; // Количество выстрелов в массиве Shoots
double Shoots[ShipTotal][5]; // Выстрелы для прорисовки: (x1,y1) - (x2,y2), energy
Player Owners[ShipTotal];    // needed to display destroyed ships

AI player1(pWHITE), player2(pBLACK);

void ClearMove()
{
  memset( chg_angle,    0, sizeof(double)*ShipTotal );
  memset( Acceler,      0, sizeof(double)*ShipTotal );
  memset( shoot_energy, 0, sizeof(double)*ShipTotal );
  memset( give_energy,  0, sizeof(double)*ShipTotal );

  memset( chg_angle2,    0, sizeof(double)*ShipTotal );
  memset( Acceler2,      0, sizeof(double)*ShipTotal );
  memset( shoot_energy2, 0, sizeof(double)*ShipTotal );
  memset( give_energy2,  0, sizeof(double)*ShipTotal );
}

void InitGame()
{
  double angle = 0, step = TWO_PI/ShipTotal, init_v = sqrt(G/InitShipDistance);
  for( int i = 0; i < ShipTotal; i++, angle += step )
  {
    ships_health[i] = ships_energy[i] = 1;
    ships_x[i] = InitShipDistance*cos(angle);
    ships_y[i] = InitShipDistance*sin(angle);
    ships_vx[i] = -init_v*cos(angle);
    ships_vy[i] = -init_v*sin(angle);
    Owners[i] = ship_owners[i] = (i < ShipCount ? pWHITE : pBLACK);
    ships_angle[i] = CorrectAngle( angle + PI );
  }
}

int FindShip( double x, double y )
{
  double dx, dy;

  for( int i = 0; i < ShipTotal; ++i )
  {
    dx = x-ships_x[i];
    dy = y-ships_y[i];
    if( sqrt( dx*dx + dy*dy ) < ShipRadius )
      return i;
  }
  return -1;
}

int NextAliveShip( int num, bool direction )
{
  int dir = (direction ? 1 : -1);
  for( int i = 1, n; i <= ShipTotal; ++i )
  {
    n = (num+i*dir+ShipTotal)%ShipTotal;
    if( ship_owners[n] != pNONE )
      return n;
  }
  return -1;
}

static void CollideShips( double x1, double y1, double& vx1, double& vy1,
                          double x2, double y2, double& vx2, double& vy2 )
{
  double dx, dy, dl, ex, ey, p1, q1, p2, q2, a;

  dx = x1 - x2;
  dy = y1 - y2;

  dl = sqrt(dx*dx + dy*dy);

  dx /= dl;
  dy /= dl;

  // {ex, ey} is orthogonal to {dx, dy}
  ex = dy;
  ey = -dx;

  // compute projections of {vx1,vy1} and {vx2,vy2} on {dx,dy} and {ex,ey}
  p1 = vx1*dx + vy1*dy;
  q1 = vx1*ex + vy1*ey;

  p2 = vx2*dx + vy2*dy;
  q2 = vx2*ex + vy2*ey;

  // compute acceleration for the first ship in coordinate p.
  a = (p2 - p1)/(1+CollideEnergyDispersion);

  // accelerate both ships
  p1 += a;
  p2 -= a;

  // transform coordinates back from {p,q} to {x,y}
  vx1 = p1*dx + q1*ex;
  vy1 = p1*dy + q1*ey;

  vx2 = p2*dx + q2*ex;
  vy2 = p2*dy + q2*ey;
}

void MoveShip( DPoint2D& r, DPoint2D& v, double& angle, double chg_angle, double& energy, double& acc )
{
  DPoint2D g, dir;
  double acc_energy;

  // Change atack direction
  angle = CorrectAngle( angle + LimAngle(chg_angle) ); 

  // Ship engine
  acc = LimAcceler(acc);
  acc_energy = EnergyForAcceleration*acc;
  if( acc_energy > energy )
  {
    acc_energy = energy;
    acc = acc_energy/EnergyForAcceleration;
  }
  energy -= acc_energy;

  // Star gravitation
  Gravitation( r.x, r.y, g.x, g.y );

  // Change the speed and position
  dir.Init( cos(angle), sin(angle) );
  v += dir*acc + g;
  r += v;
}

void MoveShip( double& x, double& y, double& vx, double& vy, double& angle, double chg_angle, double& energy, double& acc )
{
  DPoint2D r( x, y ), v( vx, vy );

  MoveShip( r, v, angle, chg_angle, energy, acc );

  x = r.x;  y = r.y;
  vx = v.x; vy = v.y;
}

const char* Step()
{
  /* int cnt[2] = { 0, 0 };
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] )
      cnt[ship_owners[i]-1]++;

  if( move_cnt > TurnLimit || cnt[0]==0 || cnt[1]==0 )
    return cnt[0] > cnt[1] ? "Выиграл игрок N1 (зелёные)!" : cnt[0] < cnt[1] ? "Выиграл игрок N2 (синие)!" : "Ничья"; */

  ClearMove();
  player1.Move1( ship_owners, ships_x, ships_y, ships_vx, ships_vy, ships_angle, ships_energy, ships_health, chg_angle, Acceler, shoot_energy, give_energy );
  player2.Move3( ship_owners, ships_x, ships_y, ships_vx, ships_vy, ships_angle, ships_energy, ships_health, chg_angle2, Acceler2, shoot_energy2, give_energy2 );

  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] == pBLACK )
    {
      chg_angle[i] = chg_angle2[i];
      Acceler[i] = Acceler2[i];
      shoot_energy[i] = shoot_energy2[i];
      for( int j = 0; j < ShipTotal; j++ )
        give_energy[i][j] = give_energy2[i][j];
    }

  // Move ships
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE )
    {
      MoveShip( ships_x[i], ships_y[i], ships_vx[i], ships_vy[i], ships_angle[i], chg_angle[i], ships_energy[i], Acceler[i] );

      // Check if the ship has fallen on the star
      if( sqr(ships_x[i]) + sqr(ships_y[i]) < sqr(StarRadius + ShipRadius) )
        ship_owners[i] = pNONE;
    }

  // Check if the ships collided
  double dx, dy, dist, len;
  for( int i = 0, j; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE )
      for( j = i+1; j < ShipTotal; j++ )
        if( ship_owners[j] != pNONE )
        {
          dx = ships_x[i] - ships_x[j];
          dy = ships_y[i] - ships_y[j];
          dist = sqrt( dx*dx + dy*dy );

          if( dist <= 2*ShipRadius ) 
          {
            // Firstly, move the ships away from each other
            if( dist > NULL_DOUBLE )
            {
              len = (2*ShipRadius - dist)/2;
              dx *= len/dist;
              dy *= len/dist;
            }
            else
              dx = ShipRadius, dy = 0;
            ships_x[i] += dx;   ships_y[i] += dy;
            ships_x[j] -= dx;   ships_y[j] -= dy;

            // And now collide them!
            CollideShips( ships_x[i], ships_y[i], ships_vx[i], ships_vy[i],
                          ships_x[j], ships_y[j], ships_vx[j], ships_vy[j] );
          }
        }

  // Стрельба
  double a, b, c; // уравнение прямой ax + by = c
  double d, dc; // Квадрат растояния до поражённого объекта
  int target_num; // Номер поражённого объекта (0-(ShipTotal-1) - корабли, ShipTotal - звезда, -1 - никто)
  double x, y, x_t, y_t, xc, yc; // x_t, y_t - координаты поражённой цели
  ShootsCount = 0;
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE && shoot_energy[i] >= NULL_DOUBLE )
    {
      // Уменьшаем энергию корабля
      shoot_energy[i] = LimShootEnergy(shoot_energy[i]);
      if( shoot_energy[i] > ships_energy[i] )
        if( (shoot_energy[i] = ships_energy[i]) <= 0 )
          continue; // А энергии то и вовсе нет
      ships_energy[i] -= shoot_energy[i];

      // Составляем уравнение прямой, по которой был произведён выстрел
      x = ships_x[i], y = ships_y[i];
      a = -sin(ships_angle[i]);
      b = cos(ships_angle[i]);
      c = a*x + b*y;

      // Ищем поражённый объект
      target_num = -1;
      // Далёкая точка на линии огня
      x_t = x + b*100;
      y_t = y - a*100;

      // Проверяем на попадание в звезду
      if( (d = Intersect( a, b, c, x, y, 0, 0, StarRadius, &x_t, &y_t )) > 0 )
        target_num = ShipTotal;

      // Проверяем на попадание в корабли
      for( int j = 0; j < ShipTotal; j++ )
        if( i != j && ship_owners[j] != pNONE )
        {
          dc = Intersect( a, b, c, x, y, ships_x[j], ships_y[j], ShipRadius, &xc, &yc );
          if( dc >= NULL_DOUBLE && (dc < d || target_num < 0) )
          {
            target_num = j;
            d = dc;
            x_t = xc;
            y_t = yc;                        
          }
        }

      // "Поражаем" объект, и, заодно, запоминаем выстрел для прорисовки
      if( target_num >= 0 && target_num < ShipTotal && d > NULL_DOUBLE )
        ships_health[target_num] -= DamageFromShoot*shoot_energy[i]/sqrt(d);

      Shoots[ShootsCount][0] = x;
      Shoots[ShootsCount][1] = y;
      Shoots[ShootsCount][2] = x_t;
      Shoots[ShootsCount][3] = y_t;
      Shoots[ShootsCount][4] = shoot_energy[i];
      ShootsCount++;
    }

  // Оцениваем жизни
  for( int i = 0; i < ShipTotal; i++ )
    if( ships_health[i] <= NULL_DOUBLE )
    {
      ships_health[i] = 0;
      ship_owners[i] = pNONE;
    }

  // Осуществляем передачу энергии
  double chg_energy[ShipTotal];
  memset( chg_energy, 0, ShipTotal*sizeof(double) );
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE )
      for( int j = 0; j < ShipTotal; j++ )
        if( ship_owners[j] != pNONE )
          if( give_energy[i][j] > NULL_DOUBLE )
            if( ships_energy[i] >= give_energy[i][j] )
            {
              chg_energy[j] += ship_owners[i]==ship_owners[j] ? give_energy[i][j]/EnergyToAlly :
                -give_energy[i][j]/EnergyToEnemy;
              chg_energy[i] -= give_energy[i][j];
            }
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE )
    {
      ships_energy[i] += chg_energy[i];
      if( ships_energy[i] < 0 )
      {
        ship_owners[i] = (ship_owners[i] == pBLACK ? pWHITE : pBLACK);
        ships_energy[i] = -ships_energy[i];
      }
    }

  // Прирост энергии
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE )
    {
      ships_energy[i] += StarEnergy( sqrt(sqr(ships_x[i]) + sqr(ships_y[i])) ) - EnergyPerTurn;
      if( ships_energy[i] > 1 )
        ships_energy[i] = 1;
      else if( ships_energy[i] < 0 )
        ship_owners[i] = pNONE;
    }

  // Если изменились владельцы, запоминаем их
  for( int i = 0; i < ShipTotal; i++ )
    if( ship_owners[i] != pNONE )
      Owners[i] = ship_owners[i];

  return 0;
}
