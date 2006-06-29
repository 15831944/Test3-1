//-- Matrix.h: interface for the Matrix class. ------------------------------

#if !defined(MATRIX_H_C0F933ED_7BC6_49C6_99CC_C075D8EA40CD)
#define      MATRIX_H_C0F933ED_7BC6_49C6_99CC_C075D8EA40CD

#pragma once

#include <iosfwd>
#include <cstdlib>
#include <memory>
#include <cmath>
#include <assert.h>
#include "TSMath.h"
#include "Vector.h"

// declare in namespace RioTintoTS
namespace RioTintoTS
{




    /************************************************************************
    *
    *    forward declarations
    *
    ************************************************************************/
    class MatrixView;
    class Matrix;




    class TableVector : public VectorView
    {

    public:

        TableVector( )
        {
            vectorStep_ = 0;
        }

        /*explicit*/ TableVector( const TableVector& T ) : VectorView( T )
        {
            vectorStep_ = T.vectorStep_;
        }

        TableVector& operator=( const VectorView& T )
        {
            VectorView::operator=( T );
            return *this;
        }

        void next( )
        {
            moveView( vectorStep_ );
        }

        void previous( )
        {
            moveView( -vectorStep_ );
        }

        void set( const TableVector& T )
        {
            VectorView::reset( T );
            vectorStep_ = T.vectorStep_;
        }

        friend class MatrixView;

    private:

        TableVector( double* first, long size, long step, long vectorStep )
            : VectorView( first, size, step )
            , vectorStep_( vectorStep )
        {
            /*void*/
        }

        long vectorStep_;
    };





    /************************************************************************
    *
    *   class MatrixView
    *
    ************************************************************************/
    class MatrixView
    {

    public:

        //// LIFECYCLE //////////////////////////////////////////////////////

        //-- Construct Empty ------------------------------------------------

        MatrixView( )
        {
            first_    = NULL;
            rowCount_ = 0;
            colCount_ = 0;
            rowStep_  = 1;
            colStep_  = 1;
        }

        //-- Copy another MatrixView ----------------------------------------

        //// AUTOMATIC COPY CONSTRUCTOR INTENDED
        //MatrixView( const MatrixView& M )
        //{
        //    setView( M.first(), M.rowCount(), M.colCount_(),
        //        M.rowStep(), M.colStep() )
        //}

        //-- Select elements in a another MatrixView ------------------------

        MatrixView( const MatrixView& M,
                    long  startRow,  long  startCol,
                    long  rowCount,  long  colCount,
                    long  rowSkip=1, long  colSkip=1 )
        {
            first_ = M.first()              // locate first element
                   + startRow * M.rowStep()
                   + startCol * M.colStep();

            rowCount_ = rowCount;           // establish Matrix size
            colCount_ = colCount;

            rowStep_  = M.rowStep() * rowSkip;  // determine row and
            colStep_  = M.colStep() * colSkip;  //   column step sizes
        }

        //-- Destroy --------------------------------------------------------

        ~MatrixView( )
        {
            /* void */
        }


        //// ACCESSORS //////////////////////////////////////////////////

        //-- Determine number of rows ---------------------------------------

        long rowCount( ) const     { return rowCount_; }

        //-- Determine number of columns ------------------------------------

        long columnCount( ) const  { return colCount_; }

        //-- Access a row of a MatrixView -----------------------------------

        TableVector row( long rowNum ) const
        {
            assert( rowNum < rowCount_ );
            double* ptr = first_ + ( rowNum * rowStep_ );
            return TableVector( ptr, colCount_, colStep_, rowStep_ );
        }

        //-- Access a row of a MatrixView -----------------------------------

        TableVector operator[]( long rowNum ) const
        {
            return row( rowNum );
        }

        //-- Access a column of a MatrixView --------------------------------

        TableVector column( long colNum ) const
        {
            assert( colNum < colCount_ );
            double* ptr = first_ + ( colNum * colStep_ );
            return  TableVector( ptr, rowCount_, rowStep_, colStep_ );
        }

        //-- Determine the sum of all elements ------------------------------

        double sum( ) const
        {
            double total = 0;
            for( int i=0; i<rowCount_; i++ )
            {
                TableVector& thisRow = row(i);
                total += thisRow.sum();
            }
            return total;
        }

        //// MANIPULATORS ///////////////////////////////////////////////////

        //-- Set all elements to a single value -----------------------------

        void clear( double S = 0.0 )
        {
            for( long i=0; i<rowCount_; i++ )
                row(i).clear( S );
        }

        //-- Assign elements from another MatrixView ------------------------

        MatrixView& operator=( const MatrixView& M )
        {
            assert( rowCount() == M.rowCount() );
            assert( columnCount() == M.columnCount() );
            for( long i=0; i<rowCount_; i++ )
                operator[]( i ) = M[i];
            return *this;
        }

        //-- Scale matrix elements by a scalar ------------------------------

        MatrixView& operator*=( const double& scalar )
        {
            for( int i=0; i<rowCount_; i++ )
                operator[]( i ) *= scalar;
            return *this;
        }

    protected:


        //// HELPERS ////////////////////////////////////////////////////////

        //-- Construct referring to raw memory ------------------------------

        MatrixView( double* first, long rowCount, long colCount,
                    long rowStep, long colStep )
        {
            setView( first, rowCount, colCount, rowStep, colStep );
        }

        //-- Reseat the view being referred to ------------------------------

        void setView( double* first, long rowCount, long colCount,
                      long rowStep, long colStep )
        {
            first_    = first;
            rowCount_ = rowCount;
            colCount_ = colCount;
            rowStep_  = rowStep;
            colStep_  = colStep;
        }

        //-- Get pointer to the first element -------------------------------

        double* first() const      { return first_; }

        //-- Get rowStep size -----------------------------------------------

        long rowStep( ) const      { return rowStep_; }

        //-- Get colStep size -----------------------------------------------

        long colStep( ) const      { return colStep_; }


    private:


        //// IMPLEMENTATION /////////////////////////////////////////////////

        double* first_;
        long    rowCount_;
        long    colCount_;
        long    rowStep_;
        long    colStep_;

    };


    /************************************************************************
    *
    *   class Matrix
    *
    ************************************************************************/
    class Matrix : public MatrixView
    {

    public:


        //// LIFECYCLE //////////////////////////////////////////////////////

        //-- Construct empty ------------------------------------------------

        Matrix( )
        {
            /* void */
        }

        //-- Construct with specified size ----------------------------------

        Matrix( long nRows, long nCols )
        {
            resize( nRows, nCols );
        }

        //-- Construct with specified size and data -------------------------

        Matrix( long nRows, long nCols, double* initBuf )
        {
            resize( nRows, nCols );

            const int nElem = nRows * nCols;
            double* pBuf1 = buffer_.get();
            double* pBuf2 = initBuf;

            for( int i=0; i<nElem; i++ )
                *pBuf1++ = *pBuf2++;
        }

        //-- Construct from elements in a Matrix ----------------------------

        Matrix( const Matrix& M )
        {
            operator=( M );
        }

        //-- Construct from elements in a MatrixView ------------------------

        Matrix( const MatrixView& M )
        {
            operator=( M );
        }

        //-- Assign from elements in a Matrix -------------------------------

        Matrix& operator=( const Matrix& M )
        {
            return operator=( (MatrixView&) M );
        }

        //-- Assign from elements in a MatrixView ---------------------------

        Matrix& operator=( const MatrixView& M )
        {
            long nRow = M.rowCount();
            long nCol = M.columnCount();

            if( (nRow!=rowCount()) || (nCol!=columnCount()) )
                resize( nRow, nCol );

            double *ptr = buffer_.get();
            for( long i=0; i<nRow; i++ )
            {
                VectorView V = M[i];

                for( long j=0; j<nCol; j++ )
                    *ptr++ = V[j];
            }
            return *this;
        }

        //-- Clear all elements in a Matrix ---------------------------------

        void clear( double S=0.0 )
        {
            long elemCount = rowCount() * columnCount();
            double *ptr = buffer_.get();
            while( elemCount-- )
                *ptr++ = S;
        }


        //-- Resize a Matrix (preserving elements) --------------------------

        //void resize( long newRowCount, long newColCount )
        //{
        //    // TO DO
        //}

        //-- Resize a Matrix (discard elements) -----------------------------

        void resize( long newRowCount, long newColCount )
        {
            long elemCount = newRowCount * newColCount;

            if( elemCount > 0 )
            {
                // fill buffer with zeros
                std::auto_ptr<double> newBuffer( new double[elemCount] );
                double *ptr = newBuffer.get();
                for( long i=0; i<elemCount; i++ )
                    *ptr++ = 0.0;

                // set implementation to newBuffer_
                buffer_ = newBuffer;
            }
            else
            {
                // release old implementation
                // buffer_.reset();
                // CANNOT AGREE ON HOW TO RELEASE AN AUTO_PTR
                // SUSPECT MICROSOFT NON-CONFORMANCE WITH ISO C++
            }

            // set MatrixView to use the new buffer
            setView( buffer_.get(),
                newRowCount, newColCount, newColCount, 1 );
        }

        //-- Swap the implementations of two Matricies ----------------------

        friend void swap( Matrix& M1, Matrix& M2 )
        {
            M1.swap( M2 );
        }

    protected:

        //-- Swap implementation with another Matrix ------------------------

        void swap( Matrix& M )
        {
            // Save our implementation

            std::auto_ptr<double> tBuffer = buffer_;
            double* tFirst   = first();
            long    tRowStep = rowStep();
            long    tColStep = colStep();
            long    tNRow    = rowCount();
            long    tNCol    = columnCount();

            // Take implementation from M

            buffer_ = M.buffer_;
            setView
            (
                M.first(),
                M.rowCount(), M.columnCount(),
                M.rowStep(), M.colStep()
            );

            // Give M our implementation

            M.buffer_ = tBuffer;;
            M.setView
            (
                tFirst,
                tNRow, tNCol,
                tRowStep, tColStep
            );
        }

    private:


        //// IMPLEMENTATION /////////////////////////////////////////////////

        // buffer to hold matrix elements
        std::auto_ptr<double> buffer_;


    };




    /************************************************************************
    *
    *   MatrixView free functions
    *
    ************************************************************************/

    inline bool operator==( const MatrixView& M1, const MatrixView& M2 )
    {
        if( M1.rowCount() != M2.rowCount() )
            return false;

        if( M1.columnCount() != M2.columnCount() )
            return false;

        for( int i=0; i<M1.rowCount(); i++ )
        {
            const TableVector& R1 = M1[i];
            const TableVector& R2 = M2[i];

            for( int j=0; j<M1.columnCount(); j++ )
                if( fabs(R1[j]-R2[j]) > EPSILON )
                    return false;
        }
        return true;
    }

    inline bool operator!=( const MatrixView& M1, const MatrixView& M2 )
    {
        return !( M1==M2 );
    }




}       // namespace RioTintoTS

#endif  // MATRIX_H_C0F933ED_7BC6_49C6_99CC_C075D8EA40CD
