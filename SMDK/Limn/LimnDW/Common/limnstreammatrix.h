#pragma once

#ifdef LIMNDW
	#include "limnglbl.h"
#endif

class CLimnOreStreamMatrix:public Matrix
{
public:
	CLimnOreStreamMatrix(void);
	CLimnOreStreamMatrix( CDiamondWizardConfiguration* thisConfig ) ;
	virtual ~CLimnOreStreamMatrix(void);

    void operator=(Real f) { Matrix::operator=(f); }

	CLimnOreStreamMatrix& CLimnOreStreamMatrix::operator=(const Matrix& m) ;


    Real& operator()(int, int);                  // access element
	Real& element(int, int);                     // access element
    Real operator()(int, int) const;             // access element
    Real element(int m, int n) const;            // access element


	void LoadFromLimnStream(  double* pStream ) ;
	void SaveToLimnStream( double* pStream ) ;

	double ToSizeDistributionForm( ColumnVector& meanSizeDistribution, ColumnVector& meanSGDistribution ) ;
	void ToAbsoluteMassForm( double totalMass, ColumnVector& meanSGDistribution ) ;

	int nSG(void) { return m_nSG ; }
	int nOSz(void) { return m_nOSz ; }
	CDiamondWizardConfiguration* thisConfig() { return m_thisConfig ; }

protected:
	int m_nOSz ;
	int m_nSG ;
	CDiamondWizardConfiguration* m_thisConfig ;

};
