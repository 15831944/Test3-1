#ifndef DMT_OSM_TYPE_H
#define DMT_OSM_TYPE_H

//-- OSM_type.h ------------------------------------------------------------------
//
// Classes Defined:
//
//   OSM_Type
//   OSM_HasType
//


//---------------------------------------------------------------------------
//
// Class OSM_Type DEFINITION
//

class OSM_Type
{
  public:

    // Uninitialized StreamType
    OSM_Type( );

    // Deallocate
    ~OSM_Type( );

  //-- Access ---------------------------------------------------------------

    // Set number of sizes + array of sieve sizes
    void setSizes( int initSizes, double* bufferSieveSizes );

    // Set a vector of sieve sizes
    void setSizes( OSM_Vector& sizes );

    // Set the number of ore components
    void setComponents( int initComps )
      { numComp = initComps; }

    // Read the number of size fractions
    int nSize( )
      { return numSize; }

    // Read the number of ore components
    int nComp( )
      { return numComp; }

    // Access Vector of sieve sizes (read-only)
    OSM_Vector& sieve( )
      { return sieveVec; }

    // Access Vector of nominal sizes (read-only)
    OSM_Vector& nominal( )
      { return nominalVec; }

  private:

  //-- Helper Functions -----------------------------------------------------

    // Calculate nominal sizes from sieve sizes
    void makeNominalSizes( );

  //-- Implementation -------------------------------------------------------

    int numSize;            // Number of sizes in OSM_Type
    int numComp;            // Number of components in OSM_Type

    OSM_Vector sieveVec;    // Size fraction sieve sizes
    OSM_Vector nominalVec;  // Size fraction nominal particle sizes
    OSM_Vector geoMeanVec;  // Size fraction
};


//---------------------------------------------------------------------------
//
// Class OSM_HasType DEFINITION
//

class OSM_HasType
{
  public:

    // Initialize with OSM_Type
    OSM_HasType( OSM_Type& sType );

    // Deallocate
    ~OSM_HasType( );

  //-- Access (proxy calls to OSM_Type) -------------------------------------

    // Read the number of size fractions
    int nSize( )
      { return myType.nSize(); }

    // Read the number of ore components
    int nComp( )
      { return myType.nComp(); }

    // Access Vector of sieve sizes
    OSM_Vector& sieve( )
      { return myType.sieve(); }

    // Access Vector of nominal sizes
    OSM_Vector& nominal( )
      { return myType.nominal(); }

    // Check if another object has the same OSM_Type
    int isSameType( OSM_Type& streamTypeObj )
      { return ( &myType == &streamTypeObj ); }

    operator OSM_Type&( ) 
      { return myType; }

  private:
  
  //-- Implementation -------------------------------------------------------

    // Reference to OSM_Type information
    OSM_Type& myType;
};


//-- end OSM_type.h ---------------------------------------------------------
#endif

