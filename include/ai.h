#pragma once

#include <starfight.h>

const double ORBIT_V_ERROR = 0.5;
const double ORBIT_R_ERROR = 0.5;
const double ORBIT_ANG_ERROR = 2*MaxChangeAngle;
const double MIN_ENERGY = 0.5;

class AI
{
   public:

         AI( Player pl ): player(pl), MoveCount(0) {}

  void Move1( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], 
             double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal], double health[ShipTotal], 
             double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] );

  void Move2( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], 
             double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal], double health[ShipTotal], 
             double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] );

  void Move3( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], 
             double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal], double health[ShipTotal], 
             double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] );

   private:

   Player player;
     long MoveCount;
   double PrevX[ShipTotal], PrevY[ShipTotal], PrevVx[ShipTotal], PrevVy[ShipTotal], PrevAngle[ShipTotal];
   double PrevChgAngle[ShipTotal], PrevAcc[ShipTotal];
   double NewX[ShipTotal], NewY[ShipTotal]; // Ожидаемые новые координаты

   private:

  void RestorePrevMove( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal],
                        double vy[ShipTotal], double angle[ShipTotal] );
  void CalcNewCoordinates( Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal],
                           double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal] );
  bool TestShoot( long our, long enemy, double x, double y, double angle, Player owner[ShipTotal] );

  bool FindShoot( long our, long enemy, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                  double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal],
                  double& chg_ang, double& acc );
  bool Shoot( long num, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
              double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal], double energy[ShipTotal],
              double& chg_ang, double& acc );

  bool FindCapture( long our, long enemy, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                  double vx[ShipTotal], double vy[ShipTotal], double energy[ShipTotal],
                  double& give_energy );
  bool Capture( long num, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                double vx[ShipTotal], double vy[ShipTotal], double energy[ShipTotal], double give_energy[ShipTotal][ShipTotal] );

  bool TurnToShoot( int num, Player owner[ShipTotal], double x[ShipTotal], double y[ShipTotal], 
                    double vx[ShipTotal], double vy[ShipTotal], double angle[ShipTotal], double& chg_ang, double& shoot_e, double* distance=0, long max_num=0, long* cur=0 );

  void CalcMove( int num, double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal], 
                 double angle[ShipTotal], double energy[ShipTotal], double& chg_ang, double& acc );

  bool MoveToOrbit( int num, double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal],
                    double angle[ShipTotal], double energy[ShipTotal], double& chg_ang, double& acc );

  bool MoveToOrbit2( int num, double x[ShipTotal], double y[ShipTotal], double vx[ShipTotal], double vy[ShipTotal],
                     double angle[ShipTotal], double energy[ShipTotal], double& chg_ang, double& acc );

  bool IsCriticalState( double x, double y );

  void ClearMove( double chg_angle[ShipTotal], double acceler[ShipTotal], double shoot_sector[ShipTotal], double give_energy[ShipTotal][ShipTotal] );
};
