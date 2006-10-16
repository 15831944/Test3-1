#pragma once

class CSpline
{
public:
	CSpline(void);
	virtual ~CSpline(void);

	void Make(double* xVals, double* yVals, int nVals) ;

	double Evaluate(double v) ;

protected:
	int N ;
	double X[100] ;
	double Y[100] ;
	double d[100] ;
	double u[100] ;
	double w[100] ;
	double p[100] ;
};
