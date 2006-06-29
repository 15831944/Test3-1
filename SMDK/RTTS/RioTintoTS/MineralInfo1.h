//-- MineralInfo1.h: Declaration of MineralInfo1 class ----------------------

#if !defined(MINERALINFO1_H_44F28FA0_40A3_422D_A809_C776D589F509)
#define      MINERALINFO1_H_44F28FA0_40A3_422D_A809_C776D589F509

#pragma once
#include "TS.h"
#include "ObjectPtr.h"
#include "Vector.h"
#include "Matrix.h"

namespace RioTintoTS
{

// smart pointer to MineralInfo1 class 

class MineralInfo1;
typedef ObjectPtr<MineralInfo1> PMineralInfo1;


/****************************************************************************
*                                             
*   class MineralInfo1
*
*   Class defines the properties of a single type of mineral.
*
****************************************************************************/
class TS_API MineralInfo1
{

public:

    // Empty MineralInfo1
    MineralInfo1( );

    // Set mineral properties
    bool SetMineralInfo
    (
        double SG, double A, double b, double ta, double m, double c
    );

    // Set mineral appearance information
    bool SetAppearance
    (
        const VectorView& t10Values,
        const VectorView& tValues,
        const MatrixView& appMatrix
    );

	// Set default appearance matrix
	bool SetDefaultAppearance( );

    // Access mineral properties
    double SG( )    { return SG_; }
    double A( )     { return A_;  }
    double b( )     { return b_;  }
    double ta( )    { return ta_; }
    double m( )     { return m_;  }
    double c( )     { return c_;  }

    // Determine breakage distribution of a given quantity
    //  of a single sized mineral specifying the reduction ratio

    double BreakRockRR
    (
        double&         RockSize,   // The size of the rocks to break
        double&         Quantity,   // The mass/volume of the rocks to break
        double&         RR,         // Reduction ratio to apply to the rocks
        VectorView&     Sieves,     // Vector of sieve sizes
        VectorView&     Product     // Fractional size distribution to add
    );                              //   breakage products to

    // Determine breakage distribution of a given quantity
    //  of a single sized mineral specifying the specific energy

    double BreakRockEcs
    (
        double&         RockSize,   // The size of the rocks to break
        double&         Quantity,   // The mass/volume of the rocks to break
        double&         Ecs,        // Specific energe to apply to the rocks
        VectorView&     Sieves,     // Vector of sieve sizes
        VectorView&     Product     // Fractional size distribution to add
    );                              //   breakage products to

    // Determine breakage distribution of a given quantity
    //  of a single sized mineral specifying T10 of the distribution

    void BreakRockT10
    (
        double&         RockSize,   // The size of the rocks to break
        double&         Quantity,   // The mass/volume of the rocks to break
        double&         T10,        // T10 of the resultant breakage
        VectorView&     Sieves,     // Vector of sieve sizes
        VectorView&     Product     // Fractional size distribution to add
    );                              //   breakage products to

    // Calculate Ecs to obtain specified reduction-ratio
    double CalcEcs( const double& RR ) const
      { return m_ * exp( RR * c_ ); }

    // Calculate T10 obtained when a specified Ecs is applied
    double CalcT10( const double& Ecs ) const
      { return A_ * ( 1 - exp( -b_ * Ecs ) ); }

private:

    // Helper - Pre-calculate appearance splines
    void AppearancePreCalc( );

    // Mineral information values
    double SG_;
    double A_;
    double b_;
    double ta_;
    double m_;
    double c_;

    // Appearance matrix
    RioTintoTS::Vector t10Values_;
    RioTintoTS::Matrix tValues_;
    RioTintoTS::Matrix appMatrix_;

    // Spline pre-computation

    Vector X;
    Vector Y2;
    Vector Y4;
    Vector Y25;
    Vector Y50;
    Vector Y75;
    Vector S2;
    Vector S4;
    Vector S25;
    Vector S50;
    Vector S75;

    Vector XX;
    Vector YY;
    Vector SS;
    Vector PARAMT;

    // Default values for the MineralInfo1 parameters

    static const double DEFAULT_SG;
    static const double DEFAULT_A;
    static const double DEFAULT_B;
    static const double DEFAULT_TA;
    static const double DEFAULT_M;
    static const double DEFAULT_C;
};


}       // RioTintoTS namespace

#endif  // MINERALINFO1_H_44F28FA0_40A3_422D_A809_C776D589F509
