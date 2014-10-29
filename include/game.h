#include <ai.h>
#include <starfight.h>
#include <points.h>

extern Player ship_owners[ShipTotal];
extern double ships_x[ShipTotal];
extern double ships_y[ShipTotal];
extern double ships_vx[ShipTotal];
extern double ships_vy[ShipTotal];
extern double ships_angle[ShipTotal];
extern double chg_angle[ShipTotal];
extern double ships_energy[ShipTotal];
extern double ships_health[ShipTotal];
extern double Shoots[ShipTotal][5];
extern int    ShootsCount;
extern double Acceler[ShipTotal];
extern Player Owners[ShipTotal];

void InitGame();
const char* Step();
void MoveShip( DPoint2D& r, DPoint2D& v, double& angle, double chg_angle, double& energy, double& acc );
int FindShip( double x, double y );
int NextAliveShip(int idx, bool direction);
