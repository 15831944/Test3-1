//-- Vector.h: interface for the Vector class. ------------------------------

#if !defined(VECTOR_H_71A1EA83_B40B_42AF_8D0E_9BCDA1ED327A)
#define      VECTOR_H_71A1EA83_B40B_42AF_8D0E_9BCDA1ED327A

#pragma once

#include <cstdlib>
#include <vector>
#include <memory>
#include <cmath>
#include <assert.h>

#include "TS.h"
#include "TSMath.h"



namespace RioTintoTS
{


    /************************************************************************
    *
    *    forward declarations
    *
    ************************************************************************/
    class VectorView;
    class Vector;
    class Curve;




    /************************************************************************
    *
    *   class VectorView
    *
    ************************************************************************/
    class TS_API VectorView
    {

    public:


        //// LIFECYCLE //////////////////////////////////////////////////////

        //-- Construct empty ------------------------------------------------

        VectorView( )
        {
            first_ = NULL;
            size_  = 0;
            step_  = 0;
        }

        //-- Copy another VectorView ----------------------------------------

        VectorView( const VectorView& V )
        {
            first_ = V.first();
            size_  = V.size();
            step_  = V.step();
        }

        //-- Select elements in another VectorView --------------------------

        VectorView( VectorView& V, long start, long count, long skip=1 )
        {
            first_ = V.first() + start * V.step();
            size_  = count;
            step_  = skip * V.step();
        }

        //-- Destroy --------------------------------------------------------

        ~VectorView( )
        {
            /* void */
        }

        //// ACCESSORS //////////////////////////////////////////////////////

        //-- Get vector size ------------------------------------------------

        long size( ) const
        {
            return size_;
        }

        //-- Access element in a VectorView ---------------------------------

        double& operator[]( long I )
        {
            assert( I < size_ );
            return first_[ I * step_ ];
        }

        //-- Access const element in a const VectorView ---------------------

        const double& operator[]( long I ) const
        {
            assert( I < size_ );
            return first_[ I * step_ ];
        }

        //-- Pointer to the memory holding the vector data ------------------

        double* buffer( )
        {
            return first_;
        }


        //// MANIPULATORS ///////////////////////////////////////////////////

        void reset( const VectorView& V )
        {
            first_ = V.first_;
            size_  = V.size_;
            step_  = V.step_;
        }

        //-- Set all elements to a single value -----------------------------

        VectorView& clear( double S = 0.0 )
        {
            for( long i=0; i<size_; i++ )
                operator[]( i ) = S;

            return *this;
        }

        //-- Assign elements from another VectorView ------------------------

        VectorView& operator=( const VectorView& V )
        {
            assert( size() >= V.size() );
            for( long i=0; i<V.size_; i++ )
                operator[]( i ) = V[i];
            return *this;
        }

        //-- Add elements in another VectorView to this one -----------------

        VectorView& operator+=( const VectorView& V )
        {
            assert( size() == V.size() );
            for( long i=0; i<size_; i++ )
                operator[]( i ) += V[i];
            return *this;
        }

        //-- Subtract elements in another VectorView from this one ----------

        VectorView& operator-=( const VectorView& V )
        {
            assert( size() == V.size() );
            for( long i=0; i<size_; i++ )
                operator[]( i ) -= V[i];
            return *this;
        }

        //-- Memberwise multiply by elements in another VectorView ----------

        VectorView& operator*=( const VectorView& V )
        {
            assert( size() == V.size() );
            for( long i=0; i<size_; i++ )
                operator[]( i ) *= V[i];
            return *this;
        }

        //-- Multiply the VectorView elements by a scalar -------------------

        VectorView& operator*=( const double& S )
        {
            for( long i=0; i<size_; i++ )
                operator[]( i ) *= S;
            return *this;
        }

        //-- Add a scalar to each element inw a VectorView ------------------

        VectorView& operator+=( const double& S )
        {
            for( long i=0; i<size_; i++ )
                operator[]( i ) += S;
            return *this;
        }

        //-- Get the sum of all VectorView elements -------------------------

        double sum( ) const
        {
            double total = 0;
            for( long i=0; i<size_; i++ )
                total += operator[]( i );
            return total;
        }

        //-- Get the sum of squared values ----------------------------------

        double sumSquares( ) const
        {
            double total = 0;
            for( long i=0; i<size_; i++ )
            {
                double temp = operator[]( i );
                total += (temp*temp);
            }
            return total;
        }

        //-- Get the vector magnitude ---------------------------------------

        double length( ) const
        {
            return sqrt( sumSquares() );
        }

        //-- Normalize a VectorView's elements ------------------------------

        double normalize( )
        {
            double len = length( );
            if( len > 0 )
                operator*=( 1/len );
            else
                clear( );
            return len;
        }

        //-- Reverse a VectorView's elements --------------------------------

        VectorView reverse( )
        {
            double* pLast = & operator[]( size_-1 );
            return VectorView( pLast, size_, -step_ );
        }

        //-- Set values of a VectorView to their natural logs ---------------

        bool setLog( )
        {
            int     i;
            double* pElem;
            double  elem;

            for( i=0, pElem=first_; i<size_; i++, pElem += step_ )
            {
                elem = *pElem;
                if( elem <= 0 )
                    goto logFailed;
                else
                    *pElem = log( elem );
            }
            return true;

        logFailed:
            return false;
        }


        //-- Create normalized copy - implemented after Vector --------------

        const Vector normalized( );


        //// FREE FUNCTIONS /////////////////////////////////////////////////

        // Memberwise addition of two VectorViews into temporary result
        // const Vector operator+(const VectorView&, const VectorView&);

        // Memberwise subtact of two VectorViews into temporary result
        // const Vector operator-(const VectorView&, const VectorView&);

        // Memberwise multiply of two VectorViews into temporary result
        // const Vector operator*(const VectorView&, const VectorView&);

        // Scalar multiply of VectorView elements into temporary result
        // const Vector operator*(const VectorView&, const double&);
        // const Vector operator*(const double&, const VectorView&);

        // Get negation of all VectorView elements into a temporary result
        // const Vector operator-(const VectorView&);

        // Inner product of two VectorView's elements
        // double dotProduct(const VectorView&, const VectorView&);

        // Compare two vectors
        // bool operator==( const VectorView& V1, const VectorView& V2 );
        // bool operator!=( const VectorView& V1, const VectorView& V2 );


        //// FRIEND CLASSES /////////////////////////////////////////////////

        //-- Classes requiring access to protected members ------------------

        friend class Vector;
        friend class Matrix;
        friend class MatrixView;

// Remove protected for now as using VectorView to overlay SysCAD data
//    protected:


        //// HIDDEN CONSTRUCTORS ////////////////////////////////////////////

        //-- Construct referring to raw memory ------------------------------

        VectorView( double* first, long size, long step=1 )
        {
            first_ = first;
            size_  = size;
            step_  = step;
        }


        //// HELPERS ////////////////////////////////////////////////////////

        //-- Reset the view being referred to -------------------------------

        void reset( double* first, long size, long step )
        {
            first_ = first;
            size_  = size;
            step_  = step;
        }

        //-- Move the view by some displacement -----------------------------

        void moveView( long i )
        {
            first_ += i;
        }

        //-- Get pointer to the first element -------------------------------

        double* first() const
        {
            return first_;
        }

        //-- Get step size --------------------------------------------------

        long step( ) const
        {
            return step_;
        }

    private:


        //// IMPLEMENTATION /////////////////////////////////////////////////

        double* first_;       // location of first vector element
        long    size_;        // count of elements in the vector
        long    step_;        // offset from one element to the next

    };



    /************************************************************************
    *
    *   class Vector
    *
    ************************************************************************/
    class Vector : public VectorView
    {

    public:

        //// LIFECYCLE //////////////////////////////////////////////////////

        //-- Construct empty ------------------------------------------------

        Vector( )
        {
            /* void */
        }

        //-- Construct of specified size ------------------------------------

        Vector( const long& initSize )
        {
            resize( initSize );
        }

        //-- Construct of specified size and initial data -------------------

        Vector( const long& initSize, double* initBuf )
        {
            resize( initSize );

            const int nElem = initSize;
            double* pBuf1 = buffer_.get();
            double* pBuf2 = initBuf;

            for( int i=0; i<nElem; i++ )
                *pBuf1++ = *pBuf2++;
        }

        //-- Construct from elements in a VectorView ------------------------

        Vector( const VectorView& V )
        {
            operator=( V );
        }

        //-- Copy construct from elements in a Vector -----------------------

        Vector( const Vector& V )
        {
            operator=( V );
        }

        // Construct from elements in an STL vector -------------------------

        Vector( const std::vector<double>& V );

        //-- Destroy --------------------------------------------------------

        ~Vector( )
        {
            /* void */
        }


        //// MANIPULATORS ///////////////////////////////////////////////////

        //-- Assign from elements in a VectorView ---------------------------

        Vector& operator=( const VectorView& V )
        {
            long vecLen = V.size();
            if( size() != vecLen )          // need to alloc buffer_ ?
                resize( vecLen );

            double *ptr = buffer_.get();    // initialize element values
            for( long i=0; i<vecLen; i++ )  //   with VectorView elements
                *ptr++ = V[i];

            return *this;
        }

        //-- Assign from elements in a Vector -------------------------------

        Vector& operator=( const Vector& V )
        {
            return operator=( (const VectorView&) V );
        }

        //-- Resize a vector (discarding elements) --------------------------

        void resize( long newSize )
        {
            if( size() != newSize )
            {
                // allocate memory to a new buffer
                std::auto_ptr<double> newBuffer( new double[newSize] );
                assert( newBuffer.get() != NULL );

                // fill buffer with zeros
                double *ptr = newBuffer.get();
                for( long i=0; i<newSize; i++ )
                    *ptr++ = 0.0;

                // set implementation to newBuffer_ and size
                buffer_ = newBuffer;
                reset( buffer_.get(), newSize, 1 );
            }
        }

        //-- Resize a vector (preserving elements) --------------------------

        // void resize( long newSize );


        //-- Swap the implementations of two vectors ------------------------

        friend void swap( Vector& V1, Vector& V2 );


    private:

        //// IMPLEMENTATION /////////////////////////////////////////////////

        // buffer to hold vector elements
        std::auto_ptr<double> buffer_;

    };




    /************************************************************************
    *
    *   Implementation of remaining member and free functions
    *
    ************************************************************************/

    //-- Normalized copy of a VectorView ------------------------------------
    inline const Vector VectorView::normalized( )
    {
        Vector retVal( *this );         // copy elements into temp Vector
        retVal.normalize();             // normalize this Vector
        return retVal;                  // send copy to caller
    }

    //-- Memberwise addition of two VectorViews -----------------------------
    inline
     const Vector operator+( const VectorView& V1, const VectorView& V2 )
    {
        Vector retVal( V1 );            // copy V1 into temp Vector
        retVal += V2;                   // add V2 elements to V1
        return retVal;                  // send copy to caller
    }

    //-- Memberwise subtraction of two VectorViews --------------------------
    inline
     const Vector operator-( const VectorView& V1, const VectorView& V2 )
    {
        Vector retVal( V1 );            // copy V1 into temp Vector
        retVal -= V2;                   // subtract V2 elements from V1
        return retVal;                  // send copy to caller
    }

    //-- Memberwise multiply of two VectorViews -----------------------------
    inline
     const Vector operator*( const VectorView& V1, const VectorView& V2 )
    {
        Vector retVal( V1 );            // copy V1 into temp Vector
        retVal *= V2;                   // multiply V1 elements by V2
        return retVal;                  // send copy to caller
    }

    //-- Scalar multiple of a VectorView's elements -------------------------
    inline
      const Vector operator*( const VectorView& V1, const double& S )
    {
        Vector retVal( V1 );            // copy V1 into temp Vector
        retVal *= S;                    // scale V1 elements by S
        return retVal;                  // send copy to caller
    }
    inline
      const Vector operator*( const double& S, const VectorView& V1 )
    {
        Vector retVal( V1 );            // copy V1 into temp Vector
        retVal *= S;                    // scale V1 elements by S
        return retVal;                  // send copy to caller
    }

    //-- Negation of a VectorView's elements --------------------------------

    inline const Vector operator-( const VectorView& V1 )
    {
        Vector retVal( V1 );            // copy V1 into temp Vector
        retVal *= (-1);                 // scale V1 elements by -1
        return retVal;                  // send copy to caller
    }

    //-- Inner product of two VectorView's elements -------------------------

    inline double dotProduct( const VectorView& V1, const VectorView& V2 )
    {
        assert(V1.size()==V2.size());   // debug build check

        double total = 0;               // accumulate total
        for( long i=0; i<V1.size(); i++ )
            total += ( V1[i] * V2[i] );

        return total;
    }

    //-- Swap two Vector implementations ------------------------------------

    inline void swap( Vector& V1, Vector& V2 )
    {
        std::auto_ptr<double> buf1 = V1.buffer_;
        std::auto_ptr<double> buf2 = V2.buffer_;
        long size1 = V1.size();
        long size2 = V2.size();

        V1.buffer_ = buf2;
        V1.reset( buf2.get(), size2, 1 );

        V2.buffer_ = buf1;
        V2.reset( buf1.get(), size1, 1 );
    }

    //-- Test that two vectors are (more or less) equal ---------------------

    inline bool operator==( const VectorView& V1, const VectorView& V2 )
    {
        if( V1.size() != V2.size() )
            return false;

        for( long i=0; i<V1.size(); i++ )
            if( fabs( V1[i]-V2[i] ) > EPSILON )
                return false;

        return true;
    }

    //-- Test vector inequality ---------------------------------------------

    inline bool operator!=( const VectorView& V1, const VectorView& V2 )
    {
        return !( V1==V2 );
    }


}      // namespace RioTintoTS

#endif // VECTOR_H_71A1EA83_B40B_42AF_8D0E_9BCDA1ED327A
