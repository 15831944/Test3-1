//---------------------------------------------------------------------------
#ifndef EISPACK_H
#define EISPACK_H
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// All the code in this module was converted from fortran from the eispack
// libraries.

typedef long 	integer;
typedef char 	*address;
typedef short 	shortint;
typedef float 	real;
typedef double doublereal;
typedef long 	logical;

#ifndef UNAC_INT
#define UNAC_LONG_INT  long int
#define UNAC_INT       int
#define UNAC_INTEGER   int
#define UNAC_SHORT_INT short int
#define UNAC_FLOAT     float
#define UNAC_DOUBLE    double
#define UNAC_CHAR      char
#endif UNAC_INT


#define TRUE_ (1)
#define FALSE_ (0)

//---------------------------------------------------------------------------
UNAC_INT balanc_(
integer *nm, integer *n ,
UNAC_DOUBLE *a,
integer *low, integer *igh,
UNAC_DOUBLE *scale );
         
//---------------------------------------------------------------------------
UNAC_INT balbak_(
integer *nm, integer *n, integer *low, integer *igh,
UNAC_DOUBLE *scale,
integer *m ,
UNAC_DOUBLE *z );

//---------------------------------------------------------------------------
UNAC_INT cdiv_(
UNAC_DOUBLE *ar,UNAC_DOUBLE *ai, UNAC_DOUBLE *br, 
UNAC_DOUBLE *bi, UNAC_DOUBLE *cr, UNAC_DOUBLE *ci );

//---------------------------------------------------------------------------
UNAC_INT elmhes_(
integer *nm,integer *n, integer *low, integer *igh,
UNAC_DOUBLE *a,
integer *int_ );

//---------------------------------------------------------------------------
UNAC_INT eltran_(
integer *nm,integer *n,integer *low,integer *igh,
UNAC_DOUBLE *a,
integer *int_,
UNAC_DOUBLE *z);

//---------------------------------------------------------------------------
UNAC_INT hqr_(
integer *nm, integer *n, integer *low, integer *igh,
UNAC_DOUBLE *h, UNAC_DOUBLE *wr, UNAC_DOUBLE *wi,
integer *ierr );

//---------------------------------------------------------------------------
UNAC_INT hqr2_(
integer *nm, integer *n, integer *low, integer *igh,
UNAC_DOUBLE *h, UNAC_DOUBLE *wr, UNAC_DOUBLE *wi, UNAC_DOUBLE *z,
integer *ierr );

//---------------------------------------------------------------------------
UNAC_INT rg_(
integer *nm, integer *n ,
UNAC_DOUBLE *a, UNAC_DOUBLE *wr, UNAC_DOUBLE *wi,
integer *matz,
UNAC_DOUBLE *z,
integer *iv1,
UNAC_DOUBLE *fv1,
integer *ierr );

//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif __cplusplus
//---------------------------------------------------------------------------
#endif EISPACK_H
