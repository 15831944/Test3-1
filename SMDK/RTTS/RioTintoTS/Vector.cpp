//-- Vector.cpp: implementation of the Vector class. ------------------------

// externals
#include <iostream>
#include <assert.h>
#include "Vector.h"

// import RioTintoTS declarations
using namespace RioTintoTS;




//-- Create a vector from an STL vector -------------------------------------

Vector::Vector( const std::vector<double>& V )
{
    first_ = NULL;
    size_  = 0;
    step_  = 0;

    long i, N = static_cast<long>( V.size() );

    resize( N );

    for( i=0; i<N; i++ )
    {
        operator[]( i ) = V[i];
    }
}









//-- Vector.cpp -------------------------------------------------------------
