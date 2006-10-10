#include "StdAfx.h"
#include "Stacker.h"
#include "StackerManager.h"
#include <atlsafe.h>
#include <math.h>

CStackerManager CStacker::StackerManager;

CStacker::CStacker(void)
: m_szName(_T("Stacker"))
{
	m_dLuftPos = 0.0;
	m_dBoomLength =60.0;
	m_dBoomHeight = 5.0;
	m_dBoomWidth = 5.0;
	m_dBoomLuftPivotHeight = 5.0;
	m_dBoomLuftPivotXOffset = 5.0;
	m_dBoomLuftPivotCentreOffset = 5.0;
	m_dBoomAnchor = 60.0;
	m_dCWLength = 20.0;
	m_dCWWidth = 5.0;
	m_dCWLuftPivotHeight = 10.0;
	m_dCWLuftPivotXOffset = 5.0;
	m_dCWLuftPivotCentreOffset = 5.0;
	m_dCWAngle = 0.0;
	m_dCWHeight = 5.0;
	m_dCWAnchor = 20.0;
	m_dxOffset = 0.0;
	m_dTravelPos = -5.0;
	m_dTowerHeight = 20.0;
	m_dTowerOffset = 0.0;
	m_dRailHeight = 0.0;
	m_dLongTravelSlope = 0.0;
	YRotation.Init(3,3);
	ZRotation.Init(3,3);
	m_bStaticTower = false;

	m_bBucketWheel = false;
	m_dBucketWheelDiameter = 0.0;  
	m_dBucketWheelYOffset = 0.0; 

	m_dSlewHighLim = 2.0*3.141592654;
	m_dSlewLowLim = 0.0;
	m_dLuffHighLim = 3.141592654/2.0;
	m_dSlewLowLim = 0.0;

	m_bCabin = false;
	m_dCabinLength = 0.0;
	m_dCabinHeight = 0.0;
	m_dCabinXOffset = 0.0;

	m_bBase = false;
	m_dBaseLength = 0.0;
	m_dBaseHeight = 0.0;
	m_dBaseWidth = 0.0;

	ShapeList.push_back(&Boom); // Boom is used in minimum distance calcs
	ShapeList.push_back(&CW);  // Counter weight is used in minimum distance calcs
	ShapeList.push_back(&Tower);  // Counter weight is used in minimum distance calcs
	Initialise();
}

CStacker::~CStacker(void)
{
}

void CStacker::Initialise(void)
{
	// When the stacker is rotated it is first rotated about the z axis to give it luff. 
	// It is then rotated about the y axis to give it slew.
	// Prior to rotation about the zaxis we must position the luff pivot point for each element at the origin
	Boom.SetDims(m_dBoomLength,m_dBoomHeight,m_dBoomWidth);
	// Blocks are initially created with the origin at the centre. For the Boom the pivot point is at the end. 
	// Therefore we simply shift the boom half its length in the x direction.  
	Boom.SetOrigin(
		m_dBoomLength/2.0, // By convention we shift it in the positive direction
		m_dBoomLuftPivotCentreOffset,// The boom is also shifted in the y direction based on the pivot offset from the centre.
		0.0); // Since we are rotating about the z axis there is no need to offset any elements in the z direction
	Boom.SetVerticies();
	Boom.InitVerticies();
	Boom.SetColour(0,127,63);  // Green Boom

	CW.SetDims(-m_dCWLength,m_dCWHeight,m_dCWWidth);
	// The counterweight has two options when being luffed. It can either rotate about the boom pivot point or it can pivot on
	// its own axis
	double dCWXOffset,dCWYOffset;

	if(m_bStaticTower) // CW pivots on own axis. Set the counterwieght pivot point at the origin
	{
		// Counter weight is shifted in opposite directionto Boom
		dCWXOffset = 0.0;
		// Note Pivot Y offset accounts for pivot not being in centre
	    dCWYOffset = m_dCWLuftPivotCentreOffset;
	}
	else  // CW rotates with boom. The Boom pivot is set at the origin and the counterwieght is offset from Boom pivot
	{
		dCWXOffset = m_dCWLuftPivotXOffset-m_dBoomLuftPivotXOffset; // X offset from Boom pivot point
		dCWYOffset = m_dCWLuftPivotHeight-m_dBoomLuftPivotHeight + m_dCWLuftPivotCentreOffset; // Y offset from Boom pivot point
	}

	CW.SetOrigin(-m_dCWLength/2.0+dCWXOffset,dCWYOffset,0.0);
	CW.SetVerticies();
	CW.InitVerticies();
	CW.SetColour(127,63,63); // Green counter weight

	Tower.Init(4);
	// Tower is positioned relative to Boom.
	Tower.AddVertex(1,m_dTowerOffset-m_dBoomLuftPivotXOffset,m_dBoomHeight/2+m_dBoomLuftPivotCentreOffset,m_dBoomWidth/2.0);
	Tower.AddVertex(2,m_dTowerOffset-m_dBoomLuftPivotXOffset,m_dBoomHeight/2+m_dBoomLuftPivotCentreOffset,-m_dBoomWidth/2.0);
	Tower.AddVertex(3,m_dTowerOffset-m_dBoomLuftPivotXOffset,m_dTowerHeight-m_dBoomLuftPivotHeight,-m_dBoomWidth/2.0);
	Tower.AddVertex(4,m_dTowerOffset-m_dBoomLuftPivotXOffset,m_dTowerHeight-m_dBoomLuftPivotHeight,m_dBoomWidth/2.0);
	Tower.InitVerticies();
	Tower.SetColour(191,63,0); // Orange Tower

	BoomAnchor.Init(2);
	BoomAnchor.AddVertex(1,m_dBoomAnchor,m_dBoomHeight/2.0+m_dBoomLuftPivotCentreOffset,m_dBoomWidth/2.0);
	BoomAnchor.AddVertex(2,m_dBoomAnchor,m_dBoomHeight/2.0+m_dBoomLuftPivotCentreOffset,-m_dBoomWidth/2.0);
	BoomAnchor.InitVerticies();

	CWAnchor.Init(2);
	// The counterwieght anchor is offset the same way the counter weight is
	CWAnchor.AddVertex(1,dCWXOffset-m_dCWAnchor,m_dCWHeight/2.0+dCWYOffset,m_dCWWidth/2.0);
	CWAnchor.AddVertex(2,dCWXOffset-m_dCWAnchor,m_dCWHeight/2.0+dCWYOffset,-m_dCWWidth/2.0);
	CWAnchor.InitVerticies();

	if(m_bBucketWheel)
	{
		BucketWheel.SetDims(m_dBucketWheelDiameter/2,m_dBoomWidth);
		BucketWheel.SetOrigin(
			m_dBoomLength+m_dBucketWheelDiameter/2,// Bucket wheel is shifted to the end of the Boom
			m_dBucketWheelYOffset,  // Y Offset by definition is difference between centre of Bucket Wheel and luft pivot point
			0.0);
		BucketWheel.SetVerticies();
		BucketWheel.InitVerticies();
		BucketWheel.SetColour(0,123,123);  // Red Boom
		AddtoShapeList(&BucketWheel);
	}
	else
		RemoveFromShapeList(&BucketWheel);

	if(m_bCabin)
	{
		Cabin.SetDims(m_dCabinLength,m_dCabinHeight,m_dBoomWidth);
		Cabin.SetOrigin(m_dCabinLength/2+m_dCabinXOffset,m_dBoomLuftPivotCentreOffset+m_dBoomHeight/2.0+m_dCabinHeight/2.0,0.0);
		Cabin.SetVerticies();
		Cabin.InitVerticies();
		Cabin.SetColour(0,31,21);
		AddtoShapeList(&Cabin);
	}
	else
		RemoveFromShapeList(&Cabin);

	if(m_bBase)
	{
		Base.SetDims(m_dBaseLength,m_dBaseHeight,m_dBaseWidth);
		Base.SetOrigin(0,m_dBaseHeight/2.0 + m_dRailHeight,0.0);
		Base.SetVerticies();
		Base.InitVerticies();
		Base.SetColour(255,255,255);
		AddtoShapeList(&Base);
	}
	else
		RemoveFromShapeList(&Base);

}

void CStacker::Update(void)
{
	double BoomSlew = m_dSlewPos+3.14159/2.0; // convert to radians
	double BoomLuft = m_dLuftPos; // convert to radians

	YRotation.set(0.0);
	YRotation(1,1) = cos(BoomSlew);
	YRotation(1,3) = -sin(BoomSlew);
	YRotation(2,2) = 1.0;
	YRotation(3,1) = sin(BoomSlew);
	YRotation(3,3) = cos(BoomSlew);

	ZRotation.set(0.0);
	ZRotation(1,1) = cos(BoomLuft);
	ZRotation(1,2) = -sin(BoomLuft);
	ZRotation(3,3) = 1.0;
	ZRotation(2,1) = sin(BoomLuft);
	ZRotation(2,2) = cos(BoomLuft);

	Boom.InitVerticies();
	Boom.Rotate(ZRotation);
	// Boom has already been shifted to position the luft pivot point at the origin
	// After the rotation about trhe z axis (luft) we must reposition to pace the slew axis at the origin
	double dBoomSlewXOffset,dBoomSlewYOffset;

	// X offset is defined as distance of luft pivot from slew axis
	dBoomSlewXOffset = m_dBoomLuftPivotXOffset; 
	// Position boom at correct Height. Note Y Pivot offset correction has already been made
	dBoomSlewYOffset = m_dBoomLuftPivotHeight + m_dRailHeight+m_dTravelPos*m_dLongTravelSlope;

	Boom.Translate(
		dBoomSlewXOffset,
		dBoomSlewYOffset, // Not need to make this now but doesn't hurt
		0.0);
	Boom.Rotate(YRotation);
	Boom.Translate(m_dxOffset,0.0,-m_dTravelPos);

	Tower.InitVerticies();
	// Tower rotates with boom if no static
	if(!m_bStaticTower) Tower.Rotate(ZRotation);
	// Tower is already correctly positioned relative to Boom
	Tower.Translate(dBoomSlewXOffset,dBoomSlewYOffset,0.0);
	Tower.Rotate(YRotation);
	Tower.Translate(m_dxOffset,0.0,-m_dTravelPos);

	// Bucket wheel is correct relative to boom so use Boom adjustments
	if(m_bBucketWheel)
	{
		BucketWheel.InitVerticies();
		BucketWheel.Rotate(ZRotation);
		BucketWheel.Translate(dBoomSlewXOffset, dBoomSlewYOffset, 0.0);
		BucketWheel.Rotate(YRotation);
		BucketWheel.Translate(m_dxOffset,0.0,-m_dTravelPos);
	}

	// Cabin is correct relative to boom so use Boom adjustments
	if(m_bCabin)
	{
		Cabin.InitVerticies();
		Cabin.Rotate(ZRotation);
		Cabin.Translate(dBoomSlewXOffset, dBoomSlewYOffset, 0.0);
		Cabin.Rotate(YRotation);
		Cabin.Translate(m_dxOffset,0.0,-m_dTravelPos);
	}

	BoomAnchor.InitVerticies();
	BoomAnchor.Rotate(ZRotation);
	BoomAnchor.Translate(dBoomSlewXOffset, dBoomSlewYOffset, 0.0);  
	BoomAnchor.Rotate(YRotation);
	BoomAnchor.Translate(m_dxOffset,0.0,-m_dTravelPos);

    double cwSlew =BoomSlew; 
    double cwLuft =BoomLuft-m_dCWAngle; 

	YRotation(1,1) = cos(cwSlew);
	YRotation(1,3) = -sin(cwSlew);
	YRotation(2,2) = 1.0;
	YRotation(3,1) = sin(cwSlew);
	YRotation(3,3) = cos(cwSlew);

	ZRotation(1,1) = cos(cwLuft);
	ZRotation(1,2) = -sin(cwLuft);
	ZRotation(3,3) = 1.0;
	ZRotation(2,1) = sin(cwLuft);
	ZRotation(2,2) = cos(cwLuft);

	// Boom has already been shifted to position the luft pivot point at the origin
	// After the rotation about trhe z axis (luft) we must reposition to place the slew axis at the origin
	double dCWSlewXOffset,dCWSlewYOffset;

	if(m_bStaticTower)
	{
	// If the CW luffs independantly of the Boom, the counter weight pivot point has been positioned at the origin
	// We must shift the counter weight appropriately prior to slewing 
		// X offset is defined as distance of luft pivot from slew axis
		dCWSlewXOffset = m_dCWLuftPivotXOffset; 
		// Counter weight has already been shifted by m_dCWPivotOffset, shift Pivot to correct height
		dCWSlewYOffset = m_dCWLuftPivotHeight + m_dRailHeight + m_dTravelPos*m_dLongTravelSlope;
	}
	else
	{
		// Counterweight has already been shifted to be correct relative to boom pivot.
		// Therefore must use Boom adjustments
		dCWSlewXOffset = dBoomSlewXOffset; 
		dCWSlewYOffset = dBoomSlewYOffset; 
	}

	CW.InitVerticies();
	CW.Rotate(ZRotation);
	CW.Translate(dCWSlewXOffset, dCWSlewYOffset, 0.0);  
	CW.Rotate(YRotation);
	CW.Translate(m_dxOffset,0.0,-m_dTravelPos);

	CWAnchor.InitVerticies();
	CWAnchor.Rotate(ZRotation);
	CWAnchor.Translate(dCWSlewXOffset, dCWSlewYOffset, 0.0);  
	CWAnchor.Rotate(YRotation);
	CWAnchor.Translate(m_dxOffset,0.0,-m_dTravelPos);

	// No Rotation For Base
	if(m_bBase)
	{
		Base.InitVerticies();
		Base.Translate(m_dxOffset,m_dTravelPos*m_dLongTravelSlope,-m_dTravelPos);
	}
 
}


void CStacker::glDraw(void)
{
	/*glColor3f(0.0f,0.5f,0.25f);	
	Boom.glDraw();
	glColor3f(0.5f,0.25f,0.25f);	
	CW.glDraw();
	glColor3f(0.0f,0.75f,1.0f);	
	Tower.glDraw();

	glColor3f(0.75f,0.25f,0.0f);	
	glBegin(GL_LINES);
		Tower.DrawVertex(4);
		BoomAnchor.DrawVertex(1);
		Tower.DrawVertex(3);
		BoomAnchor.DrawVertex(2);
		Tower.DrawVertex(4);
		CWAnchor.DrawVertex(1);
		Tower.DrawVertex(3);
		CWAnchor.DrawVertex(2);
	glEnd();*/
}

void CStacker::SetPos(double dSlew, double dLuft, double dTravel)
{
	m_dSlewPos = dSlew;
	m_dLuftPos = dLuft;
	m_dTravelPos = dTravel;
	Update();
}

void CStacker::SetXOffset(double dxOffset)
{
	m_dxOffset = dxOffset;
}

void StackerMinDistance(CStacker & st1, CStacker & st2, CEdge& result, double & MinDistance)
{
	CEdge testEdge;
	double testDistance; 
	int nShape1, nShape2;
	nShape1 = st1.ShapeList.size();
	nShape2 = st2.ShapeList.size();

	if(nShape1 > 0 && nShape2 > 0)
		ShapeMinDistance(*(st1.ShapeList[0]),*(st2.ShapeList[0]),result,MinDistance);

	for(int i = 0; i < nShape1; i++)
	{
		for(int j = 0; j < nShape2; j++)
		{
			if(i + j > 0) // already done first one
			{
				ShapeMinDistance(*(st1.ShapeList[i]),*(st2.ShapeList[j]),testEdge,testDistance);
				if(testDistance < MinDistance)
				{
					MinDistance = testDistance;
					result = testEdge;
				}
			}
		}
	}
}

void CStacker::GetDrawingPacket(int nPacket, DRAWING_MODE & nMode, CComSafeArray<double>& sa)
{

	int nIndex = 0;
	int nDrawingPackets;
	for(int i = 0; i< ShapeList.size(); i++)
	{
		CShape * pShape = ShapeList[i];
		nDrawingPackets = pShape->GetNoDrawingPackets();
		if(nPacket <= nIndex + nDrawingPackets)
		{
			ShapeList[i]->GetDrawingPacket(nPacket - nIndex, nMode, sa);
			return;
		}
		nIndex += nDrawingPackets;

	}

	nMode = LINES;
	sa.Resize(27,1);
	SARed(sa) = 0.5; SAGreen(sa) = 0.5; SABlue(sa) = 0.0; 
	SAVertX(sa,1) = Tower.Verticies(1,3);		SAVertY(sa,1) = Tower.Verticies(2,3);		SAVertZ(sa,1) = Tower.Verticies(3,3); 
	SAVertX(sa,2) = BoomAnchor.Verticies(1,2);	SAVertY(sa,2) = BoomAnchor.Verticies(2,2);	SAVertZ(sa,2) = BoomAnchor.Verticies(3,2); 
	SAVertX(sa,3) = Tower.Verticies(1,4);		SAVertY(sa,3) = Tower.Verticies(2,4);		SAVertZ(sa,3) = Tower.Verticies(3,4); 
	SAVertX(sa,4) = BoomAnchor.Verticies(1,1);	SAVertY(sa,4) = BoomAnchor.Verticies(2,1);	SAVertZ(sa,4) = BoomAnchor.Verticies(3,1); 
	SAVertX(sa,5) = Tower.Verticies(1,3);		SAVertY(sa,5) = Tower.Verticies(2,3);		SAVertZ(sa,5) = Tower.Verticies(3,3); 
	SAVertX(sa,6) = CWAnchor.Verticies(1,2);	SAVertY(sa,6) = CWAnchor.Verticies(2,2);	SAVertZ(sa,6) = CWAnchor.Verticies(3,2); 
	SAVertX(sa,7) = Tower.Verticies(1,4);		SAVertY(sa,7) = Tower.Verticies(2,4);		SAVertZ(sa,7) = Tower.Verticies(3,4); 
	SAVertX(sa,8) = CWAnchor.Verticies(1,1);	SAVertY(sa,8) = CWAnchor.Verticies(2,1);	SAVertZ(sa,8) = CWAnchor.Verticies(3,1); 

}

int CStacker::GetNoDrawingPackets(void)
{
	int nPackets = 0;
	for(int i = 0; i < ShapeList.size(); i++)
		nPackets += ShapeList[i]->GetNoDrawingPackets();

	nPackets += 1; // Add 1 for cables
	return nPackets;
};

void CStacker::AddtoShapeList(CShape * pShape)
{
	for(int i = 0; i < ShapeList.size(); i++)
		if(pShape == ShapeList[i]) return;

	ShapeList.push_back(pShape);
}

void CStacker::RemoveFromShapeList(CShape * pShape)
{
	vector<CShape *>::iterator i;
	for(i = ShapeList.begin(); i!= ShapeList.end(); i++)
		if(*i == pShape) 
		{
			ShapeList.erase(i);
			break;
		}
}
