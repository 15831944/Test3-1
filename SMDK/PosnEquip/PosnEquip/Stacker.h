#pragma once
#include "Block.h"
#include "Octagon.h"
#include <map>
#include <atlsafe.h>
#include <vector>

class CStackerManager;

using namespace std;


struct StackerRec
{
	bool m_bCheck;
	double m_dMinDistance;
};

class CStacker
{
public:
	CStacker(void);
	// Boom Data
	double m_dBoomLength, m_dBoomWidth, m_dBoomHeight, m_dBoomLuftPivotHeight, m_dBoomLuftPivotXOffset, m_dBoomLuftPivotCentreOffset;
	// Counterweight Data
	double m_dCWLength, m_dCWHeight, m_dCWWidth, m_dCWLuftPivotHeight, m_dCWLuftPivotXOffset, m_dCWLuftPivotCentreOffset, m_dCWAngle;
	// Tower Data
    double m_dTowerHeight, m_dBoomAnchor, m_dCWAnchor, m_dTowerOffset;
	// Rail Data
	double m_dRailHeight, m_dLongTravelSlope;
	// Position Data
	double m_dSlewPos, m_dLuftPos, m_dxOffset, m_dTravelPos;
	double m_dSlewHighLim, m_dSlewLowLim, m_dLuffHighLim, m_dLuffLowLim;

	// Does the tower luft with the boom
	bool m_bStaticTower;

	// Optional BucketWheel
	bool m_bBucketWheel;
	double m_dBucketWheelDiameter,m_dBucketWheelYOffset;

	// Optional Cabin
	bool m_bCabin;
	double m_dCabinLength, m_dCabinHeight, m_dCabinXOffset;

	// Optional base
	bool m_bBase;
	double m_dBaseLength,m_dBaseHeight,m_dBaseWidth;

	static CStackerManager StackerManager;

	map<CStacker *,StackerRec> m_bTestMap; 
	vector<CShape *> ShapeList;

	CBlock Boom, CW, Cabin, Base;
	COctagon BucketWheel;
	CShape Tower, BoomAnchor, CWAnchor;
friend void StackerMinDistance(CStacker & st1, CStacker & st2, CEdge& result, double & MinDistance);
public:
	~CStacker(void);
public:
	void Initialise(void);
	void Update(void);
	void glDraw(void);
	void SetPos(double dSlew, double dLuft, double dTravel);
public:
	void SetXOffset(double dxOffset);
public:
	CString m_szName;
	matrix YRotation,ZRotation;
	long m_nArrayIndex;
public:
	void GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& psaVerticies);
	int GetNoDrawingPackets(void);
	void AddtoShapeList(CShape * pShape);
	void RemoveFromShapeList(CShape * pShape);
};
