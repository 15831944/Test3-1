#include "StdAfx.h"
#include "Edge.h"
#include <math.h>

matrix CEdge::A(6,6);
matrix CEdge::inv(6,6);
matrix CEdge::B(6,1);

int CEdge::nCount = 0;

CEdge::CEdge(void)
{
	Verticies.Init(6,1);
}

CEdge::CEdge(CEdge & e1)
{
	Verticies = e1.Verticies;
}

CEdge::~CEdge(void)
{
}

double TestCalc(double x, double y, double z, CEdge & e)
{
	double dTest;
	if(fabs(e.xdiff()) > 0.0)
	   dTest = (x - e.x1())/ e.xdiff() - 0.5;
	else if (fabs(e.ydiff()) > 0.0)
	   dTest = (y - e.y1())/ e.ydiff() - 0.5;
	else if (fabs(e.zdiff()) > 0.0)
	   dTest = (z - e.z1())/ e.zdiff() - 0.5;
	else
		dTest = 1.0;

	return dTest;
}

void ClosestVertex(double& x, double& y, double& z, CEdge & e)
{
	double Test = TestCalc(x,y,z,e);
	if(Test > 0.5) // Closest Vertex on e is x2 
	{
		 x = e.x2();
		 y = e.y2();
		 z = e.z2();
	}
	else if(Test < -0.5) // Closest Vertex is x1
	{
		 x = e.x1();
		 y = e.y1();
		 z = e.z1();
	}
// else leave point alone
}

void EdgeMinDistance( CEdge & e1, CEdge & e2, CEdge & result, double & dist)
{
	// This routine determines the minimum distance between two edges 
	// Edges are defined as a pair of verticies. 

	double e1x1 = e1.x1();
	double e1y1 = e1.y1();
	double e1z1 = e1.z1();
	double e1x2 = e1.x2();
	double e1y2 = e1.y2();
	double e1z2 = e1.z2();

	double e2x1 = e2.x1();
	double e2y1 = e2.y1();
	double e2z1 = e2.z1();
	double e2x2 = e2.x2();
	double e2y2 = e2.y2();
	double e2z2 = e2.z2();

	double& resx1 = result.x1();
	double& resy1 = result.y1();
	double& resz1 = result.z1();
	double& resx2 = result.x2();
	double& resy2 = result.y2();
	double& resz2 = result.z2();

	// If first starts by determining the minimum distance between a pair of infinite lines 
	// along the edges

	CEdge::nCount++;
	if(e1.Length() < 1.0e-20 || e2.Length() < 1.0e-20)
	{
		double a = 1 + 2;
	}
	
	if(e1.Length() < 1.0e-20 && e2.Length() < 1.0e-20)
	{
		result.x1() = e1.x1();
		result.y1() = e1.y1();
		result.z1() = e1.z1();
		result.x2() = e2.x1();
		result.y2() = e2.y1();
		result.z2() = e2.z1();
		dist = result.Length();
		return;
	}
	//matrix B(6,1); //,inv(6,6);
  
    // 
	for(int ii = 1; ii <2; ii++)
	{
	if(fabs(e1.xdiff()) > 0.0)
	{
		CEdge::A(1, 1) = 1.0;
		CEdge::A(1, 2) = e1.ydiff() / e1.xdiff();
		CEdge::A(1, 3) = e1.zdiff() / e1.xdiff();
		CEdge::A(1, 4) = -1.0;
		CEdge::A(1, 5) = -e1.ydiff() / e1.xdiff();
		CEdge::A(1, 6) = -e1.zdiff() / e1.xdiff();

		CEdge::A(2, 1) = -e1.ydiff() / e1.xdiff();
		CEdge::A(2, 2) = 1.0;
		CEdge::A(2, 3) = 0.0;
		CEdge::A(2, 4) = 0.0;
		CEdge::A(2, 5) = 0.0;
		CEdge::A(2, 6) = 0.0;

		CEdge::A(3, 1) = -e1.zdiff() / e1.xdiff();
		CEdge::A(3, 2) = 0.0;
		CEdge::A(3, 3) = 1.0;
		CEdge::A(3, 4) = 0.0;
		CEdge::A(3, 5) = 0.0;
		CEdge::A(3, 6) = 0.0;

		CEdge::B(1, 1) = 0.0;
		CEdge::B(2, 1) = e1.y1() - e1.x1() * e1.ydiff() / e1.xdiff();
		CEdge::B(3, 1) = e1.z1() - e1.x1() * e1.zdiff() / e1.xdiff();
	}
	else if(fabs(e1.ydiff()) > 0)
	{
		CEdge::A(1, 1) = 1.0;
		CEdge::A(1, 2) = 0.0;
		CEdge::A(1, 3) = 0.0;
		CEdge::A(1, 4) = 0.0;
		CEdge::A(1, 5) = 0.0;
		CEdge::A(1, 6) = 0.0;

		CEdge::A(2, 1) = 0.0;
		CEdge::A(2, 2) = 1.0;
		CEdge::A(2, 3) = e1.zdiff() / e1.ydiff();
		CEdge::A(2, 4) = 0.0;
		CEdge::A(2, 5) = -1.0;
		CEdge::A(2, 6) = -e1.zdiff() / e1.ydiff();

		CEdge::A(3, 1) = 0.0;
		CEdge::A(3, 2) = -e1.zdiff() / e1.ydiff();
		CEdge::A(3, 3) = 1.0;
		CEdge::A(3, 4) = 0.0;
		CEdge::A(3, 5) = 0.0;
		CEdge::A(3, 6) = 0.0;

		CEdge::B(1, 1) = e1.x1();
		CEdge::B(2, 1) = 0.0;
		CEdge::B(3, 1) = e1.z1() - e1.y1() * e1.zdiff() / e1.ydiff();
	}
	else if (fabs(e1.zdiff()) > 0)
	{
		CEdge::A(1, 1) = 1.0;
		CEdge::A(1, 2) = 0.0;
		CEdge::A(1, 3) = 0.0;
		CEdge::A(1, 4) = 0.0;
		CEdge::A(1, 5) = 0.0;
		CEdge::A(1, 6) = 0.0;

		CEdge::A(2, 1) = 0.0;
		CEdge::A(2, 2) = 1.0;
		CEdge::A(2, 3) = 0.0;
		CEdge::A(2, 4) = 0.0;
		CEdge::A(2, 5) = 0.0;
		CEdge::A(2, 6) = 0.0;

		CEdge::A(3, 1) = 0.0;
		CEdge::A(3, 2) = 0.0;
		CEdge::A(3, 3) = 1.0;
		CEdge::A(3, 4) = 0.0;
		CEdge::A(3, 5) = 0.0;
		CEdge::A(3, 6) = -1.0;

		CEdge::B(1, 1) = e1.x1();
		CEdge::B(2, 1) = e1.y1();
		CEdge::B(3, 1) = 0.0;
	}
	else
	{
		CEdge::A(1, 1) = 1.0;
		CEdge::A(1, 2) = 0.0;
		CEdge::A(1, 3) = 0.0;
		CEdge::A(1, 4) = 0.0;
		CEdge::A(1, 5) = 0.0;
		CEdge::A(1, 6) = 0.0;

		CEdge::A(2, 1) = 0.0;
		CEdge::A(2, 2) = 1.0;
		CEdge::A(2, 3) = 0.0;
		CEdge::A(2, 4) = 0.0;
		CEdge::A(2, 5) = 0.0;
		CEdge::A(2, 6) = 0.0;

		CEdge::A(3, 1) = 0.0;
		CEdge::A(3, 2) = 0.0;
		CEdge::A(3, 3) = 1.0;
		CEdge::A(3, 4) = 0.0;
		CEdge::A(3, 5) = 0.0;
		CEdge::A(3, 6) = 0.0;

		CEdge::B(1, 1) = e1.x1();
		CEdge::B(2, 1) = e1.y1();
		CEdge::B(3, 1) = e1.z1();
	}

    
	if(fabs(e2.xdiff()) > 0.0)
	{    
		CEdge::A(4, 1) = 1.0;
		CEdge::A(4, 2) = e2.ydiff() / e2.xdiff();
		CEdge::A(4, 3) = e2.zdiff() / e2.xdiff();
		CEdge::A(4, 4) = -1.0;
		CEdge::A(4, 5) = -e2.ydiff() / e2.xdiff();
		CEdge::A(4, 6) = -e2.zdiff() / e2.xdiff();
	   
	    
		CEdge::A(5, 1) = 0.0;
		CEdge::A(5, 2) = 0.0;
		CEdge::A(5, 3) = 0.0;
		CEdge::A(5, 4) = -e2.ydiff() / e2.xdiff();
		CEdge::A(5, 5) = 1.0;
		CEdge::A(5, 6) = 0.0;
	    
	  
		CEdge::A(6, 1) = 0.0;
		CEdge::A(6, 2) = 0.0;
		CEdge::A(6, 3) = 0.0;
		CEdge::A(6, 4) = -e2.zdiff() / e2.xdiff();
		CEdge::A(6, 5) = 0.0;
		CEdge::A(6, 6) = 1.0;
	    
		CEdge::B(4, 1) = 0.0;
		CEdge::B(5, 1) = e2.y1() - e2.x1() * e2.ydiff() / e2.xdiff();
		CEdge::B(6, 1) = e2.z1() - e2.x1() * e2.zdiff() / e2.xdiff();
	}

	else if (fabs(e2.ydiff()) > 0.0)
	{    
		CEdge::A(4, 1) = 0.0;
		CEdge::A(4, 2) = 0.0;
		CEdge::A(4, 3) = 0.0;
		CEdge::A(4, 4) = 1.0;
		CEdge::A(4, 5) = 0.0;
		CEdge::A(4, 6) = 0.0;
	    
		CEdge::A(5, 1) = 0.0;
		CEdge::A(5, 2) = 1.0;
		CEdge::A(5, 3) = e2.zdiff() / e2.ydiff();
		CEdge::A(5, 4) = 0.0;
		CEdge::A(5, 5) = -1.0;
		CEdge::A(5, 6) = -e2.zdiff() / e2.ydiff();
	   
		CEdge::A(6, 1) = 0.0;
		CEdge::A(6, 2) = 0.0;
		CEdge::A(6, 3) = 0.0;
		CEdge::A(6, 4) = 0.0;
		CEdge::A(6, 5) = -e2.zdiff() / e2.ydiff();
		CEdge::A(6, 6) = 1.0;
	    
		CEdge::B(4, 1) = e2.x1();
		CEdge::B(5, 1) = 0.0;
		CEdge::B(6, 1) = e2.z1() - e2.y1() * e2.zdiff() / e2.ydiff();
	}
	else if (fabs(e2.zdiff()) > 0.0)
	{    
		CEdge::A(4, 1) = 0.0;
		CEdge::A(4, 2) = 0.0;
		CEdge::A(4, 3) = 0.0;
		CEdge::A(4, 4) = 1.0;
		CEdge::A(4, 5) = 0.0;
		CEdge::A(4, 6) = 0.0;
	    
		CEdge::A(5, 1) = 0.0;
		CEdge::A(5, 2) = 0.0;
		CEdge::A(5, 3) = 0.0;
		CEdge::A(5, 4) = 0.0;
		CEdge::A(5, 5) = 1.0;
		CEdge::A(5, 6) = 0.0;
	   
		CEdge::A(6, 1) = 0.0;
		CEdge::A(6, 2) = 0.0;
		CEdge::A(6, 3) = 1.0;
		CEdge::A(6, 4) = 0.0;
		CEdge::A(6, 5) = 0.0;
		CEdge::A(6, 6) = -1.0;
	    
		CEdge::B(4, 1) = e2.x1();
		CEdge::B(5, 1) = e2.y1();
		CEdge::B(6, 1) = 0.0;
	}
	else
	{
		CEdge::A(4, 1) = 0.0;
		CEdge::A(4, 2) = 0.0;
		CEdge::A(4, 3) = 0.0;
		CEdge::A(4, 4) = 1.0;
		CEdge::A(4, 5) = 0.0;
		CEdge::A(4, 6) = 0.0;
	    
		CEdge::A(5, 1) = 0.0;
		CEdge::A(5, 2) = 0.0;
		CEdge::A(5, 3) = 0.0;
		CEdge::A(5, 4) = 0.0;
		CEdge::A(5, 5) = 1.0;
		CEdge::A(5, 6) = 0.0;
	   
		CEdge::A(6, 1) = 0.0;
		CEdge::A(6, 2) = 0.0;
		CEdge::A(6, 3) = 0.0;
		CEdge::A(6, 4) = 0.0;
		CEdge::A(6, 5) = 0.0;
		CEdge::A(6, 6) = 1.0;
	    
		CEdge::B(4, 1) = e2.x1();
		CEdge::B(5, 1) = e2.y1();
		CEdge::B(6, 1) = e2.z1();
	}
        
	// Test for parrallel
	if(    (fabs(e1.XDirCosine()) - fabs(e2.XDirCosine()) < 1.0e-8) 
		&& (fabs(e1.YDirCosine()) - fabs(e2.YDirCosine()) < 1.0e-8)  
		&& (fabs(e1.ZDirCosine()) - fabs(e2.ZDirCosine()) < 1.0e-8) )
	{

	// Lines are parrallel. This gives us infinite number of solutions
	// In this case we will select the closest vertex of e1 as one point 
	// and compute the closest point on edge e2

	// 1st select e1.v1  and compute distance to edge e2.v1 
	double d1 = pow(e1.x1() - e2.x1(),2) + pow(e1.y1() - e2.y1(),2) + pow(e1.z1() - e2.z1(),2);
    double xc = e1.x1();
    double yc = e1.y1();
    double zc = e1.z1();
    double Min = d1;
    
	// See if distance to e2.v2 is closer
    d1 = pow(e1.x1() - e2.x2(),2) + pow(e1.y1() - e2.y2(),2) + pow(e1.z1() - e2.z2(),2);
	if (d1 < Min) Min = d1;
    
	// Now check for second vertex e1.v2 
    d1 = pow(e1.x2() - e2.x2(),2) + pow(e1.y2() - e2.y2(),2) + pow(e1.z2() - e2.z2(),2);
	if (d1 < Min)
	{
		xc = e1.x2();
		yc = e1.y2();
		zc = e1.z2();
		Min = d1;
	}
 
    d1 = pow(e1.x2() - e2.x1(),2) + pow(e1.y2() - e2.y1(),2) + pow(e1.z2() - e2.z1(),2);
	if (d1 > Min)
	{
		xc = e1.x2();
		yc = e1.y2();
		zc = e1.z2();
		Min = d1;
	}

     CEdge::A(1, 1) = 1.0;
     CEdge::A(1, 2) = 0.0;
     CEdge::A(1, 3) = 0.0;
     CEdge::A(1, 4) = 0.0;
     CEdge::A(1, 5) = 0.0;
     CEdge::A(1, 6) = 0.0;
     
     CEdge::A(2, 1) = 0.0;
     CEdge::A(2, 2) = 1.0;
     CEdge::A(2, 3) = 0.0;
     CEdge::A(2, 4) = 0.0;
     CEdge::A(2, 5) = 0.0;
     CEdge::A(2, 6) = 0.0;
    
     CEdge::A(3, 1) = 0.0;
     CEdge::A(3, 2) = 0.0;
     CEdge::A(3, 3) = 1.0;
     CEdge::A(3, 4) = 0.0;
     CEdge::A(3, 5) = 0.0;
     CEdge::A(3, 6) = 0.0;
     
     CEdge::B(1, 1) = xc;
     CEdge::B(2, 1) = yc;
     CEdge::B(3, 1) = zc;
 	}
   
    //double det = A.
	try
	{
		CEdge::inv = CEdge::A.inverse();
		result.Verticies = CEdge::inv*CEdge::B;
	}
	catch(CMatrixException e)
	{
		CString szMessage(e.m_szErrorString);
		AfxMessageBox(szMessage);
	}
    
	// Having determined the closest point along infinite lines we now check to see if the result 
	// is outside the edge
	double TestA = TestCalc(result.x1(),result.y1(), result.z1(),e1); // Test intercept on Edge A
	double TestB = TestCalc(result.x2(),result.y2(), result.z2(),e2); // Test intercept on Edge B

	// if only one point is outside the edge 
	if(fabs(TestA) > 0.5 && fabs(TestB) <= 0.5) // Only Point A is outside range
	{
		ClosestVertex(result.x1(),result.y1(), result.z1(),e1);
	}

	if(fabs(TestA) <= 0.5 && fabs(TestB) > 0.5) // Only Point B is outside range
	{
		ClosestVertex(result.x2(),result.y2(), result.z2(),e2);
	}

	if(fabs(TestA) > 0.5 && fabs(TestB) > 0.5) // Both points are outside range
	{
		if(fabs(TestA) > fabs(TestB)) // Point A is furthest from edge recalculate intercept on Edge B
		{
			CEdge::A(1, 1) = 1.0;
			CEdge::A(1, 2) = 0.0;
			CEdge::A(1, 3) = 0.0;
			CEdge::A(1, 4) = 0.0;
			CEdge::A(1, 5) = 0.0;
			CEdge::A(1, 6) = 0.0;
		     
			CEdge::A(2, 1) = 0.0;
			CEdge::A(2, 2) = 1.0;
			CEdge::A(2, 3) = 0.0;
			CEdge::A(2, 4) = 0.0;
			CEdge::A(2, 5) = 0.0;
			CEdge::A(2, 6) = 0.0;
		    
			CEdge::A(3, 1) = 0.0;
			CEdge::A(3, 2) = 0.0;
			CEdge::A(3, 3) = 1.0;
			CEdge::A(3, 4) = 0.0;
			CEdge::A(3, 5) = 0.0;
			CEdge::A(3, 6) = 0.0;

			double x,y,z;
			ClosestVertex(x,y,z,e1);
			CEdge::B(1, 1) = x;
			CEdge::B(2, 1) = y;
			CEdge::B(3, 1) = z;

			try
			{
				CEdge::inv = CEdge::A.inverse();
				result.Verticies = CEdge::inv*CEdge::B;
			}
			catch(CMatrixException e)
			{
				CString szMessage(e.m_szErrorString);
				AfxMessageBox(szMessage);
			}

			//Check Intercept on Edge B
			ClosestVertex(result.x2(),result.y2(), result.z2(),e2); 

		}
		else  // // Point B is furthest from edge recalculate intercept on Edge A
		{
			CEdge::A(4, 1) = 0.0;
			CEdge::A(4, 2) = 0.0;
			CEdge::A(4, 3) = 0.0;
			CEdge::A(4, 4) = 1.0;
			CEdge::A(4, 5) = 0.0;
			CEdge::A(4, 6) = 0.0;
		     
			CEdge::A(5, 1) = 0.0;
			CEdge::A(5, 2) = 0.0;
			CEdge::A(5, 3) = 0.0;
			CEdge::A(5, 4) = 0.0;
			CEdge::A(5, 5) = 1.0;
			CEdge::A(5, 6) = 0.0;
		    
			CEdge::A(6, 1) = 0.0;
			CEdge::A(6, 2) = 0.0;
			CEdge::A(6, 3) = 0.0;
			CEdge::A(6, 4) = 0.0;
			CEdge::A(6, 5) = 0.0;
			CEdge::A(6, 6) = 1.0;

			double x,y,z;
			ClosestVertex(x,y,z,e2);
			CEdge::B(4, 1) = x;
			CEdge::B(5, 1) = y;
			CEdge::B(6, 1) = z;

			try
			{
				CEdge::inv = CEdge::A.inverse();
				result.Verticies = CEdge::inv*CEdge::B;
			}
			catch(CMatrixException e)
			{
				CString szMessage(e.m_szErrorString);
				AfxMessageBox(szMessage);
			}

			//Check Intercept on Edge A
			ClosestVertex(result.x1(),result.y1(), result.z1(),e1);	
		}
	}

	dist = result.Length();
	double a = 1 + 2;

	}

};

double CEdge::Length(void)
{
	return sqrt(pow(xdiff(),2) + pow(ydiff(),2) + pow(zdiff(),2));
}

double CEdge::XDirCosine(void)
{
	double l = Length();
	if(l == 0.0) return 0.0;
	return xdiff()/Length();
}
double CEdge::YDirCosine(void)
{
	double l = Length();
	if(l == 0.0) return 0.0;
	return ydiff()/Length();
}
double CEdge::ZDirCosine(void)
{
	double l = Length();
	if(l == 0.0) return 0.0;
	return zdiff()/Length();
}

void CEdge::glDraw()
{
	/*glBegin(GL_LINES);
	glVertex3f(x1(),y1(),z1());
	glVertex3f(x2(),y2(),z2());
	glEnd();*/
}