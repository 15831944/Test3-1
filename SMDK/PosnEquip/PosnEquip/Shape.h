#pragma once
#include "MatrixLib.h"
#include "Edge.h"
#include <atlsafe.h>

#include <DrawingDefs.h>

class CShape
{
public:
	CShape(void);
public:
	~CShape(void);
	matrix Verticies,RawVerticies;
	void Rotate(matrix & matRot);
	void Translate(double dx, double dy, double dz);
	void Init(int nVerticies);
	void InitVerticies(void);
	void AddVertex(int nIndex, double dx, double dy, double dz);
	virtual void glDraw(void);
	void DrawVertex(int nVertex);
	int GetEdges(void);
	virtual void GetEdge(int nEdge, CEdge & edge);
	void GetEdgeFromVerticies(int nVertex1, int nVertex2, CEdge & edge);
	friend void ShapeMinDistance(CShape & s1, CShape & s2, CEdge & result, double & dist);
	void SetOrigin(double x, double y, double z);
	void SetSAVertex(CComSafeArray<double>& sa,int saVertex, int ShapeVertex);
	void SetSAColour(CComSafeArray<double>& sa);

	// Drawing Functions
    virtual void GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& saVerticies);
	int GetNoDrawingPackets(void) {return m_nDrawPackets;};
	COLORREF colour;
	void SetColour(int nRed, int nGreen, int nBlue);
	DRAWING_MODE m_nMode;

public:
	int m_nVerticies;
protected :
	int m_nEdges;
	int m_nDrawPackets;
	double m_pOrigin[3];

public:
	bool m_bMinDistTest;
};

	inline double & SAVertX(CComSafeArray<double>& sa, unsigned int i) { return sa.GetAt(i*3+sa.GetLowerBound());};
	inline double & SAVertY(CComSafeArray<double>& sa, unsigned int i) { return sa.GetAt(i*3+sa.GetLowerBound()+1);};
	inline double & SAVertZ(CComSafeArray<double>& sa, unsigned int i) { return sa.GetAt(i*3+sa.GetLowerBound()+2);};
	inline double & SARed(CComSafeArray<double>& sa) { return sa.GetAt(sa.GetLowerBound());};
	inline double & SAGreen(CComSafeArray<double>& sa) { return sa.GetAt(sa.GetLowerBound()+1);};
	inline double & SABlue(CComSafeArray<double>& sa) { return sa.GetAt(sa.GetLowerBound()+2);};

