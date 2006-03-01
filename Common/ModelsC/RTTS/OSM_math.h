#ifndef DMT_OSM_MATH_H
#define DMT_OSM_MATH_H

//-- OSM_Math.h -------------------------------------------------------------
//
// Classes Defined:
//
//   OSM_Vector
//   OSM_Matrix
//


//---------------------------------------------------------------------------
//
// Class OSM_Vector DEFINITION
//

class OSM_Vector
{
  public:

    // Default Vector, optionally specify length and memory buffer
    OSM_Vector( int initLength = 0, double* buffer = NULL );

    // Reference-copy of another Vector
    OSM_Vector( OSM_Vector& v );

    // Free OSM_Vector resources
    ~OSM_Vector( );

  //-- Access ---------------------------------------------------------------

    // Dimension the Vector, optionally supply storage
    int dimension( int length, double* buffer = NULL );

    // Set Vector elements with a Scalar
    OSM_Vector& operator=( double x );

    // Set Vector elements with another Vector
    OSM_Vector& operator=( const OSM_Vector& );

    // Alias this Vector to another Vector
    OSM_Vector& refersTo( OSM_Vector& v1 );

    // Access element of Vector
    double& operator[]( int index )
      { return vecBuf[index]; }

    // Access element of a const Vector
    const double& operator[]( int index ) const
      { return vecBuf[index]; }

    // Access Vector as array of double
    operator double*( )
      { return vecBuf; }

    // Determine vector length
    int count() const
      { return length; }

  //-- Arithmetic -----------------------------------------------------------

    // Load vector values into this vector
    OSM_Vector& load( const OSM_Vector& v1 )
      { return operator=( v1 ); }

    // Add another vector to this one
    OSM_Vector& add( const OSM_Vector& v1 );

    // Load the result of memberwise multiplication of two vectors
    OSM_Vector& loadProduct( const OSM_Vector& v1, const OSM_Vector& v2 );

    // Load the result of mul;tiplying Vector elements by a Scalar
    OSM_Vector& loadProduct( const OSM_Vector& v1, const double& x );

    // Load the result of adding two Vectors
    OSM_Vector& loadSum( const OSM_Vector& v1, const OSM_Vector& v2 );

    // Load the result of adding two Vectors, testing against present values
    double loadSumWithCompare( const OSM_Vector& v1, const OSM_Vector& v2 );

    // Add the product of a Vector and a scalar
    OSM_Vector& addProduct( const OSM_Vector& v1, const double& x );

    // Load the result of subtracting a Vector from another Vector
    OSM_Vector& loadSubtraction( const OSM_Vector& v1, const OSM_Vector& v2 );

    // Get the sum of a Vector
    double sum( ) const;

    // Get the dot product of two Vectors
    friend double dot( const OSM_Vector&v1, const OSM_Vector& v2 );

  private:

  //-- Helper Functions -----------------------------------------------------

    // Restore implementation to default-empty Vector
    void deallocate( );

    // Test matching length with another vector
    int sameSize( const OSM_Vector& v1 ) const
      { return length==v1.length; }

    // Test matching length with two other vectors
    int sameSize( const OSM_Vector& v1, const OSM_Vector& v2 ) const
      { return sameSize(v1) && sameSize(v2); }

  //-- Implementation -------------------------------------------------------

    friend class OSM_Matrix; // Matrix can use private helper functions

    int     memAlloc;        // Set TRUE if memory allcoated to vecBuf
    int     length;          // Number of items in the Vector
    double* vecBuf;          // Data elements of the Vector
};


//---------------------------------------------------------------------------
//
// Class OSM_Matrix DEFINITION
//

class OSM_Matrix : public OSM_Vector
{
  public:

    // Default, empty matrix
    OSM_Matrix( );

    // Dimensioned matrix, optionally specify data area
    OSM_Matrix( int nRows, int nColumns, double* buffer = NULL );

    // Release Matrix resources
    ~OSM_Matrix( );

  //-- Access ---------------------------------------------------------------

    // Dimension Matrix, optionally specify storage
    int dimension( int nRows, int nCols, double* buffer = NULL );

    // Assign scalar value to all Matrix elements
    OSM_Matrix& operator=( const double& x );

    // Assign values from a OSM_Vector
    OSM_Matrix& operator=( const OSM_Vector V1 );

    // Use the implementation of another Matrix
    OSM_Matrix& refersTo( OSM_Matrix& M1 );

    // Determine number of rows;
    int rows()
      { return numRows; }

    // Determnine number of columns
    int columns()
      { return numCols; }

    // Access row vector in matrix
    OSM_Vector& operator[]( int index )
      { return rowVec[index]; }

  private:

  //-- Helper Functions -----------------------------------------------------

    // Restore default Matrix
    void deallocate( );

  //-- Implementation -------------------------------------------------------

    int         numRows;        // Number of rows in Matrix
    int         numCols;        // Number of columns in Matrix
    OSM_Vector* rowVec;         // Array of row-vectors
};

//-- end OSM_Math.h ----------------------------------------------------------
#endif

