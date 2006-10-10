#include "stdafx.h"
#include "Octagon.h"
#include <math.h>

const double PI = 3.14159;

COctagon::COctagon(void)
{
	m_nVerticies = 16;
	RawVerticies.Init(3,m_nVerticies);
	Verticies.Init(3,m_nVerticies);
	m_pOrigin[0] = 0.0;
	m_pOrigin[1] = 0.0;
	m_pOrigin[2] = 0.0;
	m_nEdges = 24;
	m_nDrawPackets = 1;
	m_dWidth = 0.0;
	m_dRadius = 0.0;
}

COctagon::~COctagon(void)
{
}

void COctagon::SetVerticies(void)
{

	double dl = m_dRadius/cos(PI/8.0);

	RawVerticies(1,1) = m_dRadius + m_pOrigin[0];
	RawVerticies(2,1) =  dl*sin(PI/8.0) + m_pOrigin[1];
	RawVerticies(3,1) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,2) = dl*cos(3*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,2) =  dl*sin(3*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,2) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,3) = dl*cos(5*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,3) =  dl*sin(5*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,3) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,4) = dl*cos(7*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,4) =  dl*sin(7*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,4) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,5) = dl*cos(9*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,5) = dl*sin(9*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,5) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,6) = dl*cos(11*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,6) = dl*sin(11*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,6) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,7) = dl*cos(13*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,7) = dl*sin(13*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,7) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,8) = dl*cos(15*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,8) = dl*sin(15*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,8) = m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,9) = m_dRadius + m_pOrigin[0];
	RawVerticies(2,9) =  dl*sin(PI/8.0) + m_pOrigin[1];
	RawVerticies(3,9) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,10) = dl*cos(3*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,10) =  dl*sin(3*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,10) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,11) = dl*cos(5*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,11) =  dl*sin(5*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,11) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,12) = dl*cos(7*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,12) =  dl*sin(7*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,12) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,13) = dl*cos(9*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,13) = dl*sin(9*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,13) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,14) = dl*cos(11*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,14) = dl*sin(11*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,14) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,15) = dl*cos(13*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,15) = dl*sin(13*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,15) = -m_dWidth/2.0 + m_pOrigin[2];

	RawVerticies(1,16) = dl*cos(15*PI/8.0) + m_pOrigin[0];
	RawVerticies(2,16) = dl*sin(15*PI/8.0) + m_pOrigin[1];
	RawVerticies(3,16) = -m_dWidth/2.0 + m_pOrigin[2];


	//RawVerticies(1,1) = -dl + m_pOrigin[0];
	//RawVerticies(2,1) =  m_pOrigin[1];
	//RawVerticies(3,1) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,2) = -dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,2) =  dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,2) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,3) = m_pOrigin[0];
	//RawVerticies(2,3) =  dl + m_pOrigin[1];
	//RawVerticies(3,3) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,4) = dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,4) =  dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,4) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,5) = dl + m_pOrigin[0];
	//RawVerticies(2,5) = m_pOrigin[1];
	//RawVerticies(3,5) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,6) = dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,6) =  -dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,6) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,7) = m_pOrigin[0];
	//RawVerticies(2,7) =  -dl + m_pOrigin[1];
	//RawVerticies(3,7) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,8) = -dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,8) =  -dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,8) = m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,9) = -dl + m_pOrigin[0];
	//RawVerticies(2,9) =  m_pOrigin[1];
	//RawVerticies(3,9) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,10) = -dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,10) =  dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,10) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,11) = m_pOrigin[0];
	//RawVerticies(2,11) =  dl + m_pOrigin[1];
	//RawVerticies(3,11) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,12) = dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,12) =  dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,12) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,13) = dl + m_pOrigin[0];
	//RawVerticies(2,13) = m_pOrigin[1];
	//RawVerticies(3,13) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,14) = dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,14) =  -dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,14) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,15) = m_pOrigin[0];
	//RawVerticies(2,15) =  -dl + m_pOrigin[1];
	//RawVerticies(3,15) = -m_dWidth/2.0 + m_pOrigin[2];

	//RawVerticies(1,16) = -dl*cos(PI/4.0) + m_pOrigin[0];
	//RawVerticies(2,16) =  -dl*sin(PI/4.0) + m_pOrigin[1];
	//RawVerticies(3,16) = -m_dWidth/2.0 + m_pOrigin[2];

}


void COctagon::SetDims(double dRadius, double dWidth)
{
	m_dWidth = dWidth;
	m_dRadius = dRadius;
}

void COctagon::GetEdge(int nEdge,CEdge & edge)
{
	switch(nEdge)
	{
	case 1 : GetEdgeFromVerticies(1,2,edge); break;
	case 2 : GetEdgeFromVerticies(2,3,edge); break;
	case 3 : GetEdgeFromVerticies(3,4,edge); break;
	case 4 : GetEdgeFromVerticies(4,5,edge); break;
	case 5 : GetEdgeFromVerticies(5,6,edge); break;
	case 6 : GetEdgeFromVerticies(6,7,edge); break;
	case 7 : GetEdgeFromVerticies(7,8,edge); break;
	case 8 : GetEdgeFromVerticies(8,1,edge); break;

	case 9 : GetEdgeFromVerticies(9,10,edge); break;
	case 10 : GetEdgeFromVerticies(10,11,edge); break;
	case 11 : GetEdgeFromVerticies(11,12,edge); break;
	case 12 : GetEdgeFromVerticies(12,13,edge); break;
	case 13 : GetEdgeFromVerticies(13,14,edge); break;
	case 14 : GetEdgeFromVerticies(14,15,edge); break;
	case 15 : GetEdgeFromVerticies(15,16,edge); break;
	case 16 : GetEdgeFromVerticies(16,9,edge); break;

	case 17 : GetEdgeFromVerticies(1,9,edge); break;
	case 18 : GetEdgeFromVerticies(2,10,edge); break;
	case 19 : GetEdgeFromVerticies(3,11,edge); break;
	case 20 : GetEdgeFromVerticies(4,12,edge); break;
	case 21 : GetEdgeFromVerticies(5,13,edge); break;
	case 22 : GetEdgeFromVerticies(6,14,edge); break;
	case 23 : GetEdgeFromVerticies(7,15,edge); break;
	case 24 : GetEdgeFromVerticies(8,16,edge); break;

	default : ;
	}
}

void COctagon::GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& sa)
{
	switch(nPacket)
	{
	case 1: 
		nMode = COMBO_BLOCK;//POLY_COMBO_BLOCK;
		sa.Resize(51,1);

		SetSAColour(sa);
		SetSAVertex(sa,1,1);
		SetSAVertex(sa,2,2);
		SetSAVertex(sa,3,3);
		SetSAVertex(sa,4,4);
		SetSAVertex(sa,5,5);
		SetSAVertex(sa,6,6);
		SetSAVertex(sa,7,7);
		SetSAVertex(sa,8,8);
		SetSAVertex(sa,9,9);
		SetSAVertex(sa,10,10);
		SetSAVertex(sa,11,11);
		SetSAVertex(sa,12,12);
		SetSAVertex(sa,13,13);
		SetSAVertex(sa,14,14);
		SetSAVertex(sa,15,15);
		SetSAVertex(sa,16,16);
	default : ;
	}


	return;
}