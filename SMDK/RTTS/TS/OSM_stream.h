#ifndef DMT_OSM_STREAM_H
#define DMT_OSM_STREAM_H

//-- OSM_stream.h -----------------------------------------------------------
//
// Classes Defined:
//
//   OSM_Stream
//   OSM_cSizeDist
//


//---------------------------------------------------------------------------
//
// Class OSM_Stream DEFINITION
//

class TS_API OSM_Stream : public OSM_Matrix, public OSM_HasType
{
  public:

    // Create typed Stream, specifying matrix of components (optional)
    OSM_Stream( OSM_Type& sType, double* compMatrix=NULL );

    // Free stream resources
    ~OSM_Stream( );

  //-- Access ---------------------------------------------------------------

    // Link a specified buffer as the component matrix of this OSM_Stream
    void linkTo( double* data )
      { dimension( nComp(), nSize(), data ); }

    // Fill the component matrix with zero elements
    void clear( );

    // Assign stream values from another stream
    OSM_Stream& operator=( OSM_Stream& S1 )
      { return operator=( (OSM_Vector&)S1 ); }

    // Assign stream values from a Vector
    OSM_Stream& operator=( OSM_Vector& V1 );

    // Create text representation of the OSM_Stream
    friend ostream& operator<<( ostream& out,  OSM_Stream& outS );
};


//---------------------------------------------------------------------------
//
// Class OSM_cSizeDist DEFINITION
//

class OSM_cSizeDist : public OSM_HasType
{
  public:

    // Build distribution from a multi-component Stream
    OSM_cSizeDist( OSM_Stream& S );

    // Deallocate
    ~OSM_cSizeDist( );

  //-- Access ---------------------------------------------------------------

    // Determine the size at which a given percentage passes
    double sizePassing( double perCent );

    // Determine the percentage passing a given size
    double percentPassing( double size );

    // Access value in cumulative size distribution
    double& operator[]( int index )
      { return cDist[index]; }

  private:

  //-- Helper Funcitons( RR-interpolation ) ---------------------------------

    // Set line segment in RR-space
    void setRR( double s1, double s2, double p1, double p2 );

    // Interpolate percent passing a specified size on RR line segment
    double percentRR( double size );

    // Interpolate size passing a specifed percentage on RR line segment
    double sizeRR( double percent );

    // Filter percentage in RR space to open interval (0,100)
    void filterP( double& P );

    // Filter size in RR space to open interval (0,inf)
    void filterS( double& S );

  //-- Implementation -------------------------------------------------------

    OSM_Vector cDist;       // Vector containing %cumulative distribution
    double tph;             // tph of material in origninal stream

    double s0;              // Size of first point in RR line-segment
    double p0;              // Percentage of first point in RR line-segment
    double x0;              // x coord of first point in RR-space
    double y0;              // y coord of first point in RR-space
    double dx;              // width of line segment in RR-space
    double dy;              // height of line segment in RR-space
};

//-- end OSM_stream.h -------------------------------------------------------
#endif

