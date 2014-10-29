// ����� ������� ������������ ����������� ����� (�������� 0xFFFFFFFF*sizeof(ushort) ����)

#pragma once
#include <funcs.h>
#include <strings.h>

const ulong INIT_LEN = 0x20; // ��������� ���-�� ��������� � value (��� ����������� ������� malloc)

class DLL_OBJ TrueLong
{
   public:
                TrueLong(): value(0), length(0), meenl(0) { Stretch( INIT_LEN ); }
                TrueLong( long init );
                TrueLong( const TrueLong& init );
                ~TrueLong() { Clear(); }

      TrueLong& operator = ( long init );
      TrueLong& operator = ( const TrueLong& init );
      TrueLong& Init( int64_t init );

      TrueLong& operator *= ( long val );
       TrueLong operator * ( long val );
      TrueLong& operator *= ( TrueLong& val );
       TrueLong operator * ( TrueLong& val );
       TrueLong Pow( ulong deg );     // ���������� � ������� (��� �� ����������, ����� ���������)
      TrueLong& PowSelf( ulong deg ); // ���������� � ������� (��� � ����� �����������)

      TrueLong& operator -= ( TrueLong& val );
       TrueLong operator - ( TrueLong& val );
      TrueLong& operator -= ( long val );
       TrueLong operator - ( long val );

      TrueLong& operator += ( TrueLong& val );
       TrueLong operator + ( TrueLong& val );
      TrueLong& operator += ( long val );
       TrueLong operator + ( long val );

      TrueLong& operator /= ( TrueLong& val );
       TrueLong operator / ( TrueLong& val );
       TrueLong operator % ( TrueLong& val );
      TrueLong& operator /= ( long val );
       TrueLong operator / ( long val );
         ushort Divide( ushort val ); // ��������� �������, � ���� ���������� �� val
       TrueLong Divide( TrueLong val, TrueLong& rest ); // ��������� ���-�, ���� �� ����������, rest - �������

      TrueLong& operator >>= ( ulong num );    // �������� �� LIMIT^num
      TrueLong& operator <<= ( ulong num );    // �������� �� LIMIT^num

           bool operator > ( TrueLong& val );
           bool operator < ( TrueLong& val );
           bool operator >= ( TrueLong& val );
           bool operator <= ( TrueLong& val );
           bool operator == ( TrueLong& val );
           bool operator != ( TrueLong& val );

           bool operator > ( long val );
           bool operator < ( long val );
           bool operator >= ( long val );
           bool operator <= ( long val );
           bool operator == ( long val );
           bool operator != ( long val );

      TrueLong& FromString( const char* init ); // "0xNNN" ��� "NNNN"
          char* ToString( char* buf, ulong size );
        String& GetString( String& str );

           void Clear();
           void Optimize();
           bool IsZero( bool optimize=true );
           // �������� elem, ��� ����� ������� ������
           void AddHighElem( ushort elem );
           // ��������� ������ value �� len (�������������, ��� length ����� >= len)
           void Stretch( ulong len );
 
          ulong ULong();
        int64_t Int64();

                // ����� ����� � ������� �������������
         int64_t BitsLength();
                // ��������� �� 2^cnt
           void ShiftUp( ulong cnt );
                // ������� �� 2^cnt
           void ShiftDown( ulong cnt );

   protected:
      ushort*  value;
      ulong  length; // ����� ������� value 
      ulong  meenl;  // �������� ����� ������� (��, ��� �� ���, ��������� ����)

      TrueLong& FromHexStr( const char* init ); // "NNNNNNNNNN" ��� �������� "0x"
};

// ���������� ����� ��������
TrueLong NOD( TrueLong& a, TrueLong& b );
