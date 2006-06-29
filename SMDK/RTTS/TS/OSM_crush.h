#ifndef DMT_OSM_CRUSH_H
#define DMT_OSM_CRUSH_H

//-- OSM_Crush.h ------------------------------------------------------------
//
// Classes Defined:
//
//   OSM_Crusher
//   OSM_Model400
//


//---------------------------------------------------------------------------
//
// Class OSM_Crusher DEFINITION
//

class TS_API OSM_Crusher : public OSM_HasType
{
  public:

    // Create with StreamType
    OSM_Crusher( OSM_Type& sType );

    // Deallocate
    ~OSM_Crusher( );

  //-- Access ---------------------------------------------------------------

    // Connect a specified Stream as the model feed
    int setFeed( OSM_Stream& modelFeed );

    // Link the model product Stream to a user supplied buffer
    void linkProduct( double* productData );

    // Test if feed stream is present
    int hasFeed()
      { return feedStream!=NULL; }

    // Get reference to connected feed stream
    OSM_Stream& feed()
      { return *feedStream; }

    // Get reference to model's product stream
    OSM_Stream& product()
      { return productStream; }

  private:
  
  //-- Implementation -------------------------------------------------------

    OSM_Stream* feedStream;     // pointer to supplied feed Stream or NULL
    OSM_Stream  productStream;  // OSM_Crusher's product stream
};


//---------------------------------------------------------------------------
//
// Class OSM_Model400 DEFINITION
//

class OSM_Model400 : public OSM_Crusher
{
  public:

    // Initialize with OSM_Type
    OSM_Model400( OSM_Type& sType );

    // Deallocate( )
    ~OSM_Model400( );

  //-- Access ---------------------------------------------------------------

    // Set a T10 value to all components
    void setT10( double T10value )
      { T10.dimension( nComp() ); T10 = T10value; }

    // Set an array of T10 values
    void setT10( double* buffer )
      { T10.dimension( nComp(), buffer ); }

    // Set crusher machine parameters
    void setParams( double newK1, double newK2, double newK3 )
      { k1=newK1; k2=newK2; k3=newK3; }

    // Set crusher appearance function regression
    void setAppearance(
      double newM0, double newM1, double newM2, double newM3 )
      { m0=newM0; m1=newM1; m2=newM2; m3=newM3; }

    // Calculate the model
    int calculate( );

  private:
  
  //-- Calculations ---------------------------------------------------------

    // Calculate the classification function
    void calculateC( );

    // Calculate the appearance function for a given component
    void calculateA( int comp );

    // Calculate daughter products with appearance function
    void makeDaughters( OSM_Vector& feed, OSM_Vector& broken );

  //-- Implementation -------------------------------------------------------

    OSM_Vector C;           // Classification function
    OSM_Matrix A;           // Appearance function
    OSM_Vector T10;         // Vector of T10 values

    OSM_Matrix contents;    // Crusher contents

    double k1;              // k1 crusher parameter
    double k2;              // k2 crusher parameter
    double k3;              // k3 crusher parameter

    double m0;              // appearance function regression parameter
    double m1;              // appearance function regression parameter
    double m2;              // appearance function regression parameter
    double m3;              // appearance function regression parameter

    long   maxIteration;    // Maximum number of iterations
    double tolerance;       // Convergence tolerance
};


//-- end OSM_Crush.h --------------------------------------------------------
#endif

