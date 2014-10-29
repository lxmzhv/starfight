// Common things: should be included everywhere

#pragma once 

#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)

#if defined(BUILDDLL)         // �������� ���������� � DLL
   #define DLL_OBJ DLL_EXPORT
#elif defined(USEDLL)         // ����� �������������� ���������� �� DLL
   #define DLL_OBJ DLL_IMPORT
#else                         // DOS ��� ���������� ������
   #define DLL_OBJ
#endif

#include <stdlib.h>
#include <math.h>

#pragma warning (disable: 4251)
#pragma warning (disable: 4996)
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

typedef unsigned long   ulong;
typedef unsigned int    uint;
typedef unsigned short  ushort;
typedef unsigned char   uchar;
typedef char Buf[0x100];

const double PI = 3.14159265358979323846;
const double TWO_PI     = 2*PI;
const double HALF_PI    = PI/2;
const double QUARTER_PI = PI/4;

const double NULL_DOUBLE  = 1e-12;
const double LARGE_DOUBLE = 1e+20;
const float  LARGE_FLOAT  = 1e+20f;

template< class T > char Sign( T a ) { return a > 0 ? 1 : a < 0 ? -1 : 0; }
template< class T > T Max( T a, T b ) { return a > b ? a : b; }
template< class T > T Min( T a, T b ) { return a < b ? a : b; }
template< class T > T sqr( T a ) { return a*a; }
template< class T > T& Maximize( T& a, const T& b ) { if( b > a ) a = b; return a; }
template< class T > T& Minimize( T& a, const T& b ) { if( b < a ) a = b; return a; }
template<class T> inline int sign( T a ) { return a > 0 ? 1 : a < 0 ? -1 : 0; }
template<class T> inline T Abs( T a ) { return a > 0 ? a : -a; }
template<class T> inline void Swap( T& a, T& b ) { T c = a; a = b; b = c; }

template< class T >
T& Validate( T& a, const T& min, const T& max )
{
   Maximize( a, min );
   Minimize( a, max );
   return a;
}

inline bool IsNullNum( double a ) { return fabs(a) < NULL_DOUBLE; }

// ��������� ����������� �������
inline bool CheckProb( const double& prob )
{
   return rand()%RAND_MAX < prob*(RAND_MAX-1);
}

DLL_OBJ int random_event( const double* events, int cnt );

template< int p, int q >
inline double pow_func( double x ) { return pow(x,static_cast<double>(p)/q); }

#define COMPILE_CHECK( cond, err_msg ) { char err_msg[cond?1:0]; err_msg[0]; }
