#pragma once
#include "Shape.h"

class CBlock : public CShape
{
public:
	CBlock(void);
public:
	~CBlock(void);
public:
	double m_dLength,m_dWidth,m_dHeight;

	void SetDims(double dLength, double dHeight, double dWidth);
	void SetVerticies(void);
	virtual void glDraw(void);
	virtual void GetEdge(int nEdge, CEdge & edge);
	virtual void GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& saVerticies);
};
