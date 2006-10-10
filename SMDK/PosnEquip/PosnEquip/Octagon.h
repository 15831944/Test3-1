#pragma once
#include "shape.h"

class COctagon :
	public CShape
{
public:
	COctagon(void);
public:
	virtual ~COctagon(void);
public:
	void SetDims(double dRadius, double dWidth);
	void SetVerticies(void);
	virtual void GetEdge(int nEdge, CEdge & edge);
	virtual void GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& saVerticies);

protected :
	double m_dWidth;
	double m_dRadius;

};
