#include "stdafx.h"
#include "LimnStreamMatrix.h"

CLimnOreStreamMatrix::CLimnOreStreamMatrix(void) : Matrix()
{
}

CLimnOreStreamMatrix::CLimnOreStreamMatrix( CDiamondWizardConfiguration* thisConfig ) : Matrix( thisConfig->nOreSizes(), thisConfig->nSGs() )
{
	m_nOSz = thisConfig->nOreSizes() ;
	m_nSG = thisConfig->nSGs() ;
	m_thisConfig = thisConfig ;
}

CLimnOreStreamMatrix::~CLimnOreStreamMatrix(void)
{
}

Real& CLimnOreStreamMatrix::element(int m, int n)
{
	return Matrix::element(m,n) ;
}
Real CLimnOreStreamMatrix::element(int m, int n) const
{
	return Matrix::element(m,n) ;
}
Real CLimnOreStreamMatrix::operator()(int m, int n) const
{
	return Matrix::element(m-1,n-1) ;
}
Real& CLimnOreStreamMatrix::operator()(int m, int n)
{
	return Matrix::element(m-1,n-1) ;
}

CLimnOreStreamMatrix& CLimnOreStreamMatrix::operator=(const Matrix& m)
{
    if (this != &m) 
	{
        this->Matrix::operator=(m);
    }
    return *this;
}

void CLimnOreStreamMatrix::LoadFromLimnStream( double* pStream )
{
	for ( int iOSz = 0 ; iOSz < nOSz() ; iOSz++ )
	{
		for ( int iSG = 0 ; iSG < nSG() ; iSG++ )
		{
			int idx = thisConfig()->iODLimnStreamIndex(iOSz, iSG) ;
			this->element(iOSz,iSG) = pStream[idx] ;
		}
	}
}

void CLimnOreStreamMatrix::SaveToLimnStream( double* pStream )
{
	for ( int iOSz = 0 ; iOSz < nOSz(); iOSz++ )
	{
		for ( int iSG = 0 ; iSG < nSG() ; iSG++ )
		{
			int idx = thisConfig()->iODLimnStreamIndex(iOSz, iSG) ;
			pStream[idx] = this->element(iOSz,iSG) ;
		}
	}
}

double CLimnOreStreamMatrix::ToSizeDistributionForm( ColumnVector& meanSizeDistribution, ColumnVector& meanSGDistribution )
{
    
    ColumnVector SGFractionFlow( nSG() ) ;
    ColumnVector sizeFractionFlow( nOSz() ) ;

	int iSG ;
	int iOSz ;

	double flow ;
	double totalMass = 0.0 ;

	for ( iSG = 1 ; iSG <= nSG() ; iSG++ )
	{
		flow = this->Column(iSG).Sum() ;
		if ( flow < 0.0000001 ) flow = 0.0 ;
		SGFractionFlow(iSG) = flow ;
		totalMass += flow ;
	}

	if ( totalMass > 0.0 )
	{

		for ( iOSz = 1 ; iOSz <= nOSz() ; iOSz++ )
		{
			flow = this->Row(iOSz).Sum() ;
			if ( flow < 0.0000001 ) flow = 0.0 ;
			sizeFractionFlow(iOSz) = flow ;
		}

		for ( iSG = 1 ; iSG <= nSG() ; iSG++ )
		{
			for ( iOSz = 1 ; iOSz <= nOSz() ; iOSz++ )
			{
				flow = SGFractionFlow(iSG) ;
				if( flow > 0 )
				{
					this->element(iOSz-1, iSG-1) = 100.0 * this->element(iOSz-1, iSG-1) / flow ;
				}
				else
				{
					this->element(iOSz-1, iSG-1) = 0.0 ;
				}
			}
		}

		for ( iSG = 1 ; iSG <= nSG() ; iSG++ )											// SG Distribution
		{
			meanSGDistribution(iSG) = 100.0 * SGFractionFlow(iSG) / totalMass ;
		}

		for ( iOSz = 1 ; iOSz <= nOSz() ; iOSz++ )										// Size Distribution
		{	
			meanSizeDistribution(iOSz) = 100.0 * sizeFractionFlow(iOSz) / totalMass ;
		}
	}
	else
	{
		for ( iSG = 0 ; iSG < nSG() ; iSG++ )
		{
			for ( iOSz = 0 ; iOSz < nOSz() ; iOSz++ )
			{
				this->element(iOSz, iSG) = 0.0 ;
			}
		}
		meanSGDistribution = 0.0 ;
		meanSizeDistribution = 0.0 ;
	}

	return totalMass ;

}

void CLimnOreStreamMatrix::ToAbsoluteMassForm( double totalMass, ColumnVector& meanSGDistribution )
{
	for ( int iSG = 0 ; iSG < nSG() ; iSG++ )
	{
		double SGFractionFlow = totalMass * meanSGDistribution(iSG+1) ;

		for ( int iOSz = 0 ; iOSz < nOSz() ; iOSz++ )
		{
			this->element(iOSz, iSG) = ( SGFractionFlow * this->element(iOSz, iSG) ) /  10000.0 ;
		}
	}
}
