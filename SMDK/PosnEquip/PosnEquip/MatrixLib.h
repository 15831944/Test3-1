// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MATRIXLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MATRIXLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef _MATRIXLIB
#define _MATRIXLIB

#include <iostream>
using namespace std;

#ifdef MATRIXLIB_EXPORTS
#define MATRIXLIB_API __declspec(dllexport)
#else
#define MATRIXLIB_API __declspec(dllimport)
#endif

class MATRIXLIB_API matrix {
	protected :
	  double *dataPtr;
	  double m_dMinTol;
	  int *indx;
	public :
		void SetMinTol(double dMinTol);
		bool m_bAttached;
	  unsigned int rows, cols;
		matrix();
		matrix(unsigned int i,unsigned int j);
		matrix(unsigned int i);
		matrix(const matrix& m);
		void Attach(double * pdData, unsigned int i, unsigned int j);
		void Detach();
		void Init(unsigned int i,unsigned int j = 1);
	    ~matrix(); 
		double& operator ()(unsigned int i, unsigned int j)	
			{return *(dataPtr + (i - 1)*cols + j - 1); };
		double& elem(unsigned int i, unsigned int j = 1)
			{return *(dataPtr + (i - 1)*cols + j - 1); };
		double * GetDat() { return dataPtr; };
		double GetVal(unsigned int i) const {return dataPtr[i-1];};
		double GetVal(unsigned int i, unsigned int j) const 
			{return *(dataPtr + (i - 1)*cols + j - 1); };
		double& operator [](unsigned int i) {return dataPtr[i-1];};
		MATRIXLIB_API friend ostream& operator <<(ostream& os, matrix& mat);
		MATRIXLIB_API friend istream& operator >>(istream& is, matrix& mat);
		matrix operator +(matrix& m1);
		matrix operator -(matrix& m1);
		MATRIXLIB_API friend matrix operator -(const matrix& m);
		MATRIXLIB_API friend matrix operator *(double x, matrix& m1) {return m1*x;};
		matrix operator *(matrix& m1);
		matrix operator *(double x);
		matrix operator /(double x);
		matrix ludcmp();
		MATRIXLIB_API friend void lubksb(const matrix& m, double b[]);
		matrix inverse();
		matrix transpose();
		matrix& operator =( const matrix& m);
		MATRIXLIB_API friend matrix id(unsigned int n);
        MATRIXLIB_API friend double scalar_p(const matrix& x1, const matrix& x2);
        void set(double x) {for(unsigned i = 0; i< rows*cols; i++) *(dataPtr + i) = x;};
		void dump(const char * szFilename);
		double GetMin(bool Absolute);
		double GetMax(bool Absolute);
};

class MATRIXLIB_API CMatrixException
{
public:
	char m_szErrorString[256];
	CMatrixException(void);
	CMatrixException(const char * szErrorString);
	~CMatrixException(void);
};

#endif