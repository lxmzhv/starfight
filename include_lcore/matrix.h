
#pragma once

#include <TrueDrob.h>
#include <pointers.h>

typedef TrueDrob Elem;
typedef const Elem CElem;

class DLL_OBJ Matrix
{
   public:
                Matrix( ulong n_str = 1, ulong n_stlb = 1, Elem* arr = 0 );
                Matrix( Matrix& m );
                // arr == 0     - ��������� ������
                // n_str (n_stlb) == 0 - �������� ��� ��������� nStr (nStlb)
        Matrix& Init( Elem* arr = 0, ulong n_str = 0, ulong n_stlb = 0 );
                // ������������������� ������
        Matrix& InitSize( ulong n_str = 0, ulong n_stlb = 0 );
                // ������������������� ���������
        Matrix& InitEd( ulong n_str = 0, ulong n_stlb = 0 );
        Matrix& operator =( Matrix& m );
         Matrix operator *( Matrix& m );
 virtual  Elem& operator[]( ulong offset ) { return data[offset]; }
 virtual CElem& operator[]( ulong offset ) const { return data[offset]; }
 virtual  Elem& GetElem( ulong n_str, ulong n_stlb );
 virtual CElem& GetElem( ulong n_str, ulong n_stlb ) const;
           void SwapStr( int s1, int s2, ulong beg_stlb = 0 );
          ulong NumStr() const { return nStr; }
          ulong NumStlb() const { return nStlb; }

                // ������� ��� ������� (������� m1 ����������� � m2, � ��������� � this)
        Matrix& Merge( const Matrix& m1, const Matrix& m2 );
                // ��������� ������� �� m1 � m2, beg_stlb - ������ ������� ������ �������
           void Split( Matrix& m1, Matrix& m2, ulong beg_stlb ) const;

                // �������� ������� ������� � res; ����� false, ���� �������� ����������
           bool GetInverse( Matrix& res );

   protected:
      ulong nStr, nStlb; // ����������� �������
   VP<Elem> data;

            ulong DataSize() const { return nStr*nStlb*sizeof(Elem); }
};

// ��������� ����� ������ ��� ������� m1m2 (�.�. ��� �� �����������)
// ����������� | 1 2 3 | � | 2 4 | ��� | 1 2 3 2 4 |
//             | 4 5 6 |   | 4 5 |     | 4 5 6 4 5 |
// ��������� ����� � m1, m2; ����� ����������� ��������� m1, m2
DLL_OBJ Matrix DoGauss( Matrix& m1, Matrix& m2 );

// ��������� ����� ������ ��� ������� m, ��������� ����� � m
DLL_OBJ Matrix& DoGauss( Matrix& m );

DLL_OBJ Matrix Merge( const Matrix& m1, const Matrix& m2 );

