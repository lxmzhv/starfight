#pragma once

#include <math.h>
#include <funcs.h>

enum Player
{
  pNONE  = 0,
  pWHITE = 1,
  pBLACK = 2,
  pLAST  = 2
};

double DeathRadius();
double SafeRadius();
double AlarmRadius();

const double StarRadius = 3;//1e-20;
const double ShipRadius = 0.5;

const double G = 1;
const double CollideEnergyDispersion = 0; /* from 0 to 1 */

const long ShipCount = 10;
const long ShipTotal = 2*ShipCount;
const long TurnLimit = 20000;
const double InitShipDistance = Max( SafeRadius(), ShipTotal*ShipRadius*6/TWO_PI );

const double MaxChangeAngle = 0.1;
const double MaxAcceleration = 0.1; //0.03;
const double MaxShootEnergy = 0.025;
const double EnergyForAcceleration = 0.05;//0.3;
const double EnergyFromStar = 10*EnergyForAcceleration*G; // EnergyForAcceleration*G
const double EnergyPerTurn  = EnergyFromStar/5000; // EnergyFromStar/900
const double DamageFromShoot = 3;

const double EnergyTransferRadius = 3;
const double EnergyToAlly  = 2;
const double EnergyToEnemy = 3;

const double MaxAccEnergy = EnergyForAcceleration*MaxAcceleration;

// Область значений для косинуса и синуса
double CosSinVal( double val );

void Gravitation( double x, double y, double& gx, double& gy );
double StarEnergy( double distance );
double CorrectAngle( double angle );
double LimAngle( double chg_angle );
double LimAcceler( double& acc );
double LimShootEnergy( double& shoot_energy );

// Разница углов по модулю (результат от 0 до PI)
double AngleDiff( double ang1, double ang2 );
// Разница углов не по модулю (результат от -PI до PI)
double AngleDiffSign( double ang1, double ang2 );

// Определить угол наклона вектора
double VectorAngle( double x1, double y1, double x2, double y2 );
// Определить угол наклона вектора
// Если длина вектора уже известна можно её передать в dist
double VectorAngle( double x, double y, double* dist = 0 );

// Пересекаем луч (a*x + b*y = c) с кругом (центр: {x0,y0}, радиус: r)
// причём, a = -sin(deg), b = cos(deg), где deg - угол наклона прямой относительно оси Ох
// (x, y) - точка выстрела (нужна для однозначности ответа)
// вернёт квадрат расстояния до пересечения или -1
double Intersect( double a, double b, double c, double x, double y, double x0, double y0, double r, double* xx=0, double* yy=0 );
