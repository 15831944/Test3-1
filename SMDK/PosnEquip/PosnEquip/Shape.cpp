#include "StdAfx.h"
#include "Shape.h"

CShape::CShape(void)
: m_nEdges(0)
, m_bMinDistTest(false)
{
	m_nVerticies = 0;
	m_nDrawPackets = 1;
	colour = RGB(255,255,255); // White
	m_nMode = LINE_LOOP;
}

CShape::~CShape(void)
{
}

void CShape::Translate(double dx, double dy, double dz)
{
	for(int i=1; i <= m_nVerticies; i++)
	{
		Verticies(1,i) += dx;
		Verticies(2,i) += dy;
		Verticies(3,i) += dz;
	}
}

void CShape::Rotate(matrix & matRot)
{
    Verticies = matRot*Verticies;
}

void CShape::Init(int nVerticies)
{
	RawVerticies.Init(3,nVerticies);
	Verticies.Init(3,nVerticies);
	m_nVerticies = nVerticies;
}

void CShape::InitVerticies(void)
{
	Verticies = RawVerticies;
}


void CShape::AddVertex(int nIndex, double dx, double dy, double dz)
{
	RawVerticies(1,nIndex) = dx;
	RawVerticies(2,nIndex) = dy;
	RawVerticies(3,nIndex) = dz;
}

void CShape::GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& sa)
{
	nMode = m_nMode;
	int count = m_nVerticies*3;
	sa.Resize(count+3,1); // Add 3 for color

	// Colour white
	SARed(sa) = GetRValue(colour)/255.0;
	SAGreen(sa) = GetGValue(colour)/255.0;
	SABlue(sa) = GetBValue(colour)/255.0;

	// Verticies
	for(int i = 1; i<= m_nVerticies; i++)
	{
		SAVertX(sa,i) = Verticies(1,i);
		SAVertY(sa,i) = Verticies(2,i);
		SAVertZ(sa,i) = Verticies(3,i);
	}

};

void CShape::glDraw(void)
{

	// Default behaviour joins the dots
/*	glBegin(mode);	
	    for(int i = 1; i<=m_nVerticies; i++)  glVertex3f(Verticies(1,i),Verticies(2,i),Verticies(3,i));
	glEnd();*/

}
void CShape::DrawVertex(int nVertex)
{
	//glVertex3f(Verticies(1,nVertex),Verticies(2,nVertex),Verticies(3,nVertex));
}

int CShape::GetEdges(void)
{
	return m_nEdges;
}

void CShape::GetEdge(int nEdge, CEdge & edge)
{
	if(nEdge > m_nEdges) return;
	if(m_nVerticies < nEdge +1) return;

	// Default behaviour : edges are between adjacent verticies
	GetEdgeFromVerticies(nEdge,nEdge+1,edge);

};

void CShape::GetEdgeFromVerticies(int nVertex1, int nVertex2, CEdge & edge)
{

	// Default behaviour : edges are between adjacent verticies
	for(int i = 1; i <= 3; i++)
	{
		edge.Verticies(i,1) = Verticies(i,nVertex1);
		edge.Verticies(i+3,1) = Verticies(i,nVertex2);
	}

};

void ShapeMinDistance(CShape & s1, CShape & s2, CEdge & result, double & MinDist)
{
	int nEdge1 = s1.GetEdges();
	int nEdge2 = s2.GetEdges();
	CEdge e1,e2,temp;
	double testdist;
	MinDist = 1e10;  // huge number
	for (int i = 1; i <= nEdge1; i++)
	{
		s1.GetEdge(i,e1);
		for (int j = 1; j <= nEdge2; j++)
		{
			s2.GetEdge(j,e2);
			EdgeMinDistance(e1,e2,temp,testdist);
			if(testdist < MinDist)
			{
				MinDist = testdist;
				result = temp;
			}
		}
	}
};


void CShape::SetColour(int nRed, int nGreen, int nBlue)
{
	colour = RGB(nRed,nGreen,nBlue);
}

void CShape::SetOrigin(double x, double y, double z)
{
	m_pOrigin[0] = x;
	m_pOrigin[1] = y;
	m_pOrigin[2] = z;
}

void CShape::SetSAVertex(CComSafeArray<double>& sa,int saVertex, int ShapeVertex)
{
	SAVertX(sa,saVertex) = Verticies(1,ShapeVertex); 
	SAVertY(sa,saVertex) = Verticies(2,ShapeVertex); 
	SAVertZ(sa,saVertex) = Verticies(3,ShapeVertex); 
}

void CShape::SetSAColour(CComSafeArray<double>& sa)
{
	SARed(sa) = GetRValue(colour)/255.0; 
	SAGreen(sa) = GetGValue(colour)/255.0; 
	SABlue(sa) = GetBValue(colour)/255.0; 
}
