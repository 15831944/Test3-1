//-- TSMath.h: Math Definitions for the RioTintoTS namespace ----------------

#if !defined(TSMATH_H__7D19A5F6_17B5_4A41_8E62_8BEA3AF78740)
#define      TSMATH_H__7D19A5F6_17B5_4A41_8E62_8BEA3AF78740

#pragma once

/****************************************************************************
*
*        Forward declare RioTintoTS namespace contents
*
****************************************************************************/
namespace RioTintoTS
{

    // Various math constants - note: *insane* precision

    const double EPSILON            = 1e-10;
    const double BIG_NUMBER         = 1e100;

    const double ROOT2_TWO          = 1.414213562373095048801688724209698079;
    const double ROOT4_TWO          = 1.189207115002721066717499970560475915;
    const double PI                 = 3.141592653589793238462643383279502884;

    const double HALF_PI            = ( PI * 0.5 );
    const double TWO_PI             = ( PI * 2.0 );

    const double RADIAN_PER_DEGREE  = ( PI / 180.0 );
    const double DEGREE_PER_RADIAN  = ( 180.0 / PI );

    // Forward declare classes that are inside this namespace

    class VectorView;
    class Vector;
    class TableVector;
    class MatrixView;
    class Matrix;
    class PointVec;
    class Curve;
    class CoordSystem;

    // Hide templated min and max inside this namespace

    template<typename T>
    T Minimum( T x, T y )   { return ( x < y ) ? x : y; }

    template<typename T>
    T Maximum( T x, T y )   { return ( x > y ) ? x : y; }

}       // RioTintoTS namespace

#endif  // TSMATH_H__7D19A5F6_17B5_4A41_8E62_8BEA3AF78740
