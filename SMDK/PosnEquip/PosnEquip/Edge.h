#pragma once
#include "MatrixLib.h"
class CEdge
{
public:
	CEdge(void);
	CEdge(CEdge & e1);
public:
	~CEdge(void);
	matrix Verticies;
	static matrix A, inv, B;  // Used for min Distance Calculations
	static int nCount;
	inline double xdiff(){return Verticies(4,1) - Verticies(1,1);};
	inline double ydiff(){return Verticies(5,1) - Verticies(2,1);};
	inline double zdiff(){return Verticies(6,1) - Verticies(3,1);};
	inline double& x1(){return Verticies(1,1);};
	inline double& y1(){return Verticies(2,1);};
	inline double& z1(){return Verticies(3,1);};
	inline double& x2(){return Verticies(4,1);};
	inline double& y2(){return Verticies(5,1);};
	inline double& z2(){return Verticies(6,1);};
	friend void EdgeMinDistance( CEdge & e1, CEdge & e2, CEdge & result, double & dist);
    friend double TestCalc(double x, double y, double z, CEdge & e);
public:
	double Length(void);
public:
	double XDirCosine(void);
	double YDirCosine(void);
	double ZDirCosine(void);
	void glDraw();
};
