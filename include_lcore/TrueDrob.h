// ������������ ����� ����� ��������

#pragma once

#include <TrueLong.h>

class DLL_OBJ TrueDrob
{
   public:
               TrueLong p;
               TrueLong q;
                   char sign; // 1 - �������������, 0 - �������������

               // !!! sgn == 0 ��� "+", sgn == 1 ��� "-"
               TrueDrob( TrueLong& pp, TrueLong& qq, char sgn = 0 ):
                  p(pp), q(qq), sign(sgn?1:0){}
               TrueDrob( const TrueDrob& val ) { *this = val; }
               TrueDrob( TrueLong& init ) { *this = init; }
               TrueDrob( long pp = 0, long qq = 1 );

     TrueDrob& Init( long pp, long qq );
     TrueDrob& Init( double init );
        double Double();
        //double GetDouble(); // ������
               // !!! sgn == 0 ��� "+", sgn == 1 ��� "-"
     TrueDrob& Init( TrueLong& pp, TrueLong& qq, char sign = 0 );
     TrueDrob& operator =( long init );
     TrueDrob& operator =( TrueLong& init );
     TrueDrob& operator =( const TrueDrob& init );

     TrueDrob& operator *=( long mnoj );
      TrueDrob operator *( long mnoj );
     TrueDrob& operator *=( TrueDrob& d );
      TrueDrob operator *( TrueDrob& d );

     TrueDrob& operator /=( long val );
      TrueDrob operator /( long val );
     TrueDrob& operator /=( TrueDrob& d );
      TrueDrob operator /( TrueDrob& d );

     TrueDrob& operator +=( long val );
      TrueDrob operator +( long val );
     TrueDrob& operator +=( TrueDrob& d );
      TrueDrob operator +( TrueDrob& d );

     TrueDrob& operator -=( long val );
      TrueDrob operator -( long val );
     TrueDrob& operator -=( TrueDrob& d );
      TrueDrob operator -( TrueDrob& d );
      TrueDrob operator -() { return TrueDrob(p,q,1 - sign); }

          char operator <( TrueDrob d );
          char operator >( TrueDrob d );
          char operator <=( TrueDrob d );
          char operator >=( TrueDrob d );
          char operator ==( TrueDrob& d );
          char operator !=( TrueDrob& d );

          char operator <( long d );
          char operator >( long d );
          char operator <=( long d );
          char operator >=( long d );
          char operator ==( long d );
          char operator !=( long d );

          char IsLess( double d );
          char IsGreat( double d );

          // ���������� �����
     TrueDrob& Optimize();
          char IsZero();
         char* ToString( char* buf, ulong size );
               // ��������� �� ���������� ��������: ������ �� �����, ��� eps (1, ����� 0)
          char IsAbsLess( double eps );
               // ���������� ��������
      //TrueDrob Abs() { return sign ? TrueDrob(p,q) : *this; }
};