#include "StdAfx.h"
#include "Block.h"

CBlock::CBlock(void)
: m_dLength(0)
{
	m_nVerticies = 8;
	RawVerticies.Init(3,m_nVerticies);
	Verticies.Init(3,m_nVerticies);
	m_pOrigin[0] = 0.0;
	m_pOrigin[1] = 0.0;
	m_pOrigin[2] = 0.0;
	m_nEdges = 12;
	m_nDrawPackets = 1;

}

CBlock::~CBlock(void)
{
}

void CBlock::SetDims(double dLength, double dHeight, double dWidth)
{
	m_dLength = dLength;
	m_dWidth = dWidth;
	m_dHeight = dHeight;
}

void CBlock::SetVerticies(void)
{
	RawVerticies(1,1) = m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,1) = m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,1) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,2) = m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,2) = m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,2) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,3) = m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,3) = -m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,3) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,4) = m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,4) = -m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,4) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,5) = -m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,5) = m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,5) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,6) = -m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,6) = m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,6) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,7) = -m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,7) = -m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,7) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,8) = -m_dLength/2.0 + m_pOrigin[0];
	RawVerticies(2,8) = -m_dHeight/2.0 + m_pOrigin[1];
	RawVerticies(3,8) = -m_dWidth/2.0 + m_pOrigin[2];

}

void CBlock::GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& sa)
{
	nMode = COMBO_BLOCK;
	sa.Resize(27,1);
	SetSAColour(sa);
	SetSAVertex(sa,1,1);
	SetSAVertex(sa,2,2);
	SetSAVertex(sa,3,4);
	SetSAVertex(sa,4,3);
	SetSAVertex(sa,5,5);
	SetSAVertex(sa,6,6);
	SetSAVertex(sa,7,8);
	SetSAVertex(sa,8,7);
	return;


};

void CBlock::glDraw(void)
{
	/*glBegin(GL_LINE_STRIP);							
		glVertex3f(Verticies(1,1),Verticies(2,1),Verticies(3,1));
		glVertex3f(Verticies(1,2),Verticies(2,2),Verticies(3,2));
		glVertex3f(Verticies(1,4),Verticies(2,4),Verticies(3,4));
		glVertex3f(Verticies(1,3),Verticies(2,3),Verticies(3,3));
		glVertex3f(Verticies(1,1),Verticies(2,1),Verticies(3,1));
	glEnd();

	glBegin(GL_LINE_STRIP);							
		glVertex3f(Verticies(1,5),Verticies(2,5),Verticies(3,5));
		glVertex3f(Verticies(1,6),Verticies(2,6),Verticies(3,6));
		glVertex3f(Verticies(1,8),Verticies(2,8),Verticies(3,8));
		glVertex3f(Verticies(1,7),Verticies(2,7),Verticies(3,7));
		glVertex3f(Verticies(1,5),Verticies(2,5),Verticies(3,5));
	glEnd();

 	glBegin(GL_LINES);							
		glVertex3f(Verticies(1,1),Verticies(2,1),Verticies(3,1));
		glVertex3f(Verticies(1,5),Verticies(2,5),Verticies(3,5));
		glVertex3f(Verticies(1,2),Verticies(2,2),Verticies(3,2));
		glVertex3f(Verticies(1,6),Verticies(2,6),Verticies(3,6));
		glVertex3f(Verticies(1,3),Verticies(2,3),Verticies(3,3));
		glVertex3f(Verticies(1,7),Verticies(2,7),Verticies(3,7));
		glVertex3f(Verticies(1,4),Verticies(2,4),Verticies(3,4));
		glVertex3f(Verticies(1,8),Verticies(2,8),Verticies(3,8));
	glEnd();*/

}

void CBlock::GetEdge(int nEdge,CEdge & edge)
{

	switch(nEdge)
	{
	case 1 : GetEdgeFromVerticies(1,2,edge); break;
	case 2 : GetEdgeFromVerticies(2,4,edge); break;
	case 3 : GetEdgeFromVerticies(4,3,edge); break;
	case 4 : GetEdgeFromVerticies(3,1,edge); break;
	case 5 : GetEdgeFromVerticies(5,6,edge); break;
	case 6 : GetEdgeFromVerticies(6,8,edge); break;
	case 7 : GetEdgeFromVerticies(8,7,edge); break;
	case 8 : GetEdgeFromVerticies(7,5,edge); break;
	case 9 : GetEdgeFromVerticies(1,5,edge); break;
	case 10 : GetEdgeFromVerticies(2,6,edge); break;
	case 11 : GetEdgeFromVerticies(3,7,edge); break;
	case 12 : GetEdgeFromVerticies(4,8,edge); break;
	default : ;
	}
};

