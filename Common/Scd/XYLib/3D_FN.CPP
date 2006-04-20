//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>
#define __3D_FN_CPP
#include "sc_defs.h"
#include "3d_fn.h"
#include "3d_fnedt.h"

// =========================================================================

const pchar C3DModel::GroupName="3DModel";

// -------------------------------------------------------------------------

pC3DModel C3DModel::Create(pC3DFn pFn_, pchar Name_)
  { 
  pC3DModel rp=(pC3DModel)TagObjClass::Construct(C3DModel::GroupName, Name_, NULL, pFn_->Tag(), pFn_, TOA_Embedded);
  if (rp==NULL)
    rp=(pC3DModel)TagObjClass::Construct(C3DModel::GroupName, "3D_Data", NULL, pFn_->Tag(), pFn_, TOA_Embedded);
  
  rp->AttachMeToObj(pFn_, TOA_Embedded);
  rp->pFn = pFn_;
  return rp;
  }

// -------------------------------------------------------------------------

C3DModel::C3DModel(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CBaseMdl(pClass_, pTag, pAttach, eAttach)
  {
  pFn = NULL;
  }

// -------------------------------------------------------------------------

C3DModel::~C3DModel()
  {
  }

// =========================================================================

IMPLEMENT_TAGOBJEDT(C3DFn, "3DFn", "3DFn", "", "", TOC_SYSTEM, C3DFnEdt, "", ""); 
//IMPLEMENT_SPARES(C3DFn, 100); 
 
C3DFn::C3DFn(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FxdEdtBookRef(NULL),
  TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  pM = C3DModel::Create(this, "3D_Data");
  Clear();
  }
 
// -------------------------------------------------------------------------


C3DFn::C3DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FxdEdtBookRef(NULL),
  TaggedObject(&C3DFnClass, Tag_, pAttach, eAttach)
  {
  pM = C3DModel::Create(this, pModelId);
  Clear();
  }

// -------------------------------------------------------------------------

C3DFn::C3DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, pchar pXName, pchar pYName, pchar pZName) :
  FxdEdtBookRef(NULL),
  TaggedObject(&C3DFnClass, Tag_, pAttach, eAttach)
  {
  pM = C3DModel::Create(this, pModelId);
  Clear();
  SetAxisNames(pXName, pYName, pZName);
  }

// -------------------------------------------------------------------------
  
C3DFn::C3DFn(C3DFn& Fn) :
  FxdEdtBookRef(NULL),
  TaggedObject(Fn.m_pClass, "", Fn.pAttachedTo, Fn.eAttachment)
  {
  Strng s = Fn.m_sTag;
  TaggedObject::Tag(s());
  CopyFunction(Fn);
  pM = C3DModel::Create(this, Fn.pM->ClassId());
  }   

// -------------------------------------------------------------------------
 
C3DFn::~C3DFn()
  {
  pM->Destroy();
  pM = NULL;
  }
 
// -------------------------------------------------------------------------
 
void C3DFn::Clear()
  {
  PtsFName = "";
  sXName = "";
  sYName = "";
  sZName = "";
  X.SetSize(0);
  Y.SetSize(0);
  Z.SetSize(0);
  dXDispMin = 0.0; 
  dXDispMax = 1.0;
  dYDispMin = 0.0;
  dYDispMax = 1.0;
  dZDispMin = 0.0;
  dZDispMax = 1.0;
  dXLoLim = 0.0;//DBL_MAX*-1.0;
  dXUpLim = 1.0;//DBL_MAX;
  dYLoLim = 0.0;//DBL_MAX*-1.0;
  dYUpLim = 1.0;//DBL_MAX;
  dZLoLim = 0.0;//DBL_MAX*-1.0;
  dZUpLim = 1.0;//DBL_MAX;
  dXGRatio = 1.0;
  dYGRatio = 1.0;
  dZGRatio = 1.0;
  iRqdLength = 2;
  iNoElimIter = 3;
  StructureChanged(this);
  bShowData = True;
  bRedist = False;
  bIsBuilt = False;
  iNoCrsPts = 0;
  CrsSection.SetSize(0,0);
  iXDivs = 20;
  iYDivs = 20;
  iContourIntervals = 10;
  pM->Clear();
  }

// -------------------------------------------------------------------------

void C3DFn::CopyFunction(C3DFn& Fn)
  {
  if (&Fn == this)
    return;
  dXDispMin = Fn.dXDispMin;
  dXDispMax = Fn.dXDispMax;
  dYDispMin = Fn.dYDispMin;
  dYDispMax = Fn.dYDispMax;
  dZDispMin = Fn.dZDispMin;
  dZDispMax = Fn.dZDispMax;
  dXGRatio = Fn.dXGRatio;
  dYGRatio = Fn.dYGRatio;
  dZGRatio = Fn.dZGRatio;
  dXLoLim  = Fn.dXLoLim;
  dXUpLim  = Fn.dXUpLim;
  dYLoLim  = Fn.dYLoLim;
  dYUpLim  = Fn.dYUpLim;
  dZLoLim  = Fn.dZLoLim;
  dZUpLim  = Fn.dZUpLim;
  PtsFName  = Fn.PtsFName;
  sXName = Fn.sXName;
  sYName = Fn.sYName;
  sZName = Fn.sZName;
  iNoElimIter = Fn.iNoElimIter;
  iRqdLength = Fn.iRqdLength;
  bShowData  = Fn.bShowData;
  bRedist = Fn.bRedist;
  iXDivs = Fn.iXDivs;
  iYDivs = Fn.iYDivs;
  iContourIntervals = Fn.iContourIntervals;
  iNoCrsPts = Fn.iNoCrsPts;
  CrsSection = Fn.CrsSection;
  if (_stricmp(Fn.ModelName(), ModelName())!=0)
    {
    pM->Destroy();
    pM = C3DModel::Create(this, Fn.ModelName());
    }
  pM->CopyModel(Fn.pM);
  }

// -------------------------------------------------------------------------

pchar C3DFn::ModelName(pchar pReqdModelDesc) 
  {
  if (pReqdModelDesc)
    ChangeModel(pReqdModelDesc);
  return pM->Class()->ShortDesc();
  }

// -------------------------------------------------------------------------

flag C3DFn::ChangeModel(pchar pReqdModelDesc) 
  { 
  pchar pReqdModel=NULL;
  Strng S;
  for (int i=0; ;i++)
    {
    pTagObjClass pC=TagObjClass::FindClassIndexed(C3DModel::GroupName, i);
    if (pC==NULL)
      break;
    else 
      {
      pC->GetShortDesc(S);
      if (S.XStrICmp(pReqdModelDesc)==0)
        {
        pReqdModel=pC->ClassId();
        break;
        }
      }
    }

  if (pReqdModel)
    {
    StructureChanged(this);
    pC3DModel p = C3DModel::Create(this, pReqdModel);
    if (p==NULL)
      return FALSE;
    pM->Destroy();
    pM = p;
    }
  return TRUE; 
  }

// -------------------------------------------------------------------------

void C3DFn::SetLength(long n)
  {
  if (Z.GetLen() != n)
    {
    if (n != 0)
      n = Min(Max3DFnPts, Max(2L,n));
    Z.SetSize(n);
    Y.SetSize(n);
    X.SetSize(n);
    StructureChanged(this);
    }
  }

// -------------------------------------------------------------------------

/*flag C3DFn::InRegion(double x, double y)
  {
  double XUp = GetNormX(dXUpLim);
  double XLo = GetNormX(dXLoLim);
  double YUp = GetNormY(dYUpLim);
  double YLo = GetNormY(dYLoLim);
  return (x>=XLo && x<=XUp && y>=YLo && y<=YUp);
  }*/

// -------------------------------------------------------------------------

flag C3DFn::ReFit() 
  {
  if (!bIsBuilt)
    {
    bIsBuilt = True;
    /*double xmin = GetNormX(ScanXMin());
    double xmax = GetNormX(ScanXMax());
    double ymin = GetNormY(ScanYMin());
    double ymax = GetNormY(ScanYMax());
    GetNormX(X);
    GetNormY(Y);
    GetNormZ(Z);*/
    //Trinet.InitTriNet(xmin,ymin,xmax,ymax);
    Trinet.InitTriNet(ScanXMin(), ScanYMin(), ScanXMax(), ScanYMax());
    for (int i=0; i<Z.GetLen(); i++ )
      Trinet.AddVertex(X[i], Y[i], Z[i], 0);


    //i=0;
    //double x1,y1,z1, x2,y2,z2, x3,y3,z3;
    //for (flag ok=Trinet.first_triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3); ok ; ok=Trinet.next_triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3))
    //  dbgpln("%3i  %10g,%10g  %10g,%10g  %10g,%10g",++i, x1,y1, x2,y2, x3,y3);

    /*GetActualX(X);
    GetActualY(Y);
    GetActualY(Z);*/
    }
  return pM->ReFit();
  }

// -------------------------------------------------------------------------

double C3DFn::GetNormX(double x)
  {
  if (fabs(dXGRatio-1.0)<1.0e-6)
    return (x - dXLoLim) / (dXUpLim - dXLoLim);
  return log(x / dXLoLim) / log(dXGRatio);
  }

// -------------------------------------------------------------------------

void C3DFn::GetNormX(CDVector &Xs)
  {
  flag b = (fabs(dXGRatio-1.0)<1.0e-6);
  for (long i=0; i<Xs.GetLen(); i++)
    {
    if (b)
      Xs[i] = (Xs[i] - dXLoLim) / (dXUpLim - dXLoLim);
    else
      Xs[i] = (log(Xs[i] / dXLoLim) / log(dXGRatio));
    }
  }

// -------------------------------------------------------------------------

double C3DFn::GetActualX(double x)
  {
  if (fabs(dXGRatio-1.0)<1.0e-6)
    return x * (dXUpLim - dXLoLim) + dXLoLim;
  return dXLoLim * pow(dXGRatio, x);
  }

// -------------------------------------------------------------------------

void C3DFn::GetActualX(CDVector &Xs)
  {
  flag b = (fabs(dXGRatio-1.0)<1.0e-6);
  for (long i=0; i<Xs.GetLen(); i++)
    {
    if (b)
      Xs[i] = Xs[i] * (dXUpLim - dXLoLim) + dXLoLim;
    else
      Xs[i] = dXLoLim * pow(dXGRatio, Xs[i]);
    }
  }

// -------------------------------------------------------------------------

double C3DFn::GetNormY(double y)
  {
  if (fabs(dYGRatio-1.0)<1.0e-6)
    return (y - dYLoLim) / (dYUpLim - dYLoLim);
  return log(y / dYLoLim) / log(dYGRatio);
  }

// -------------------------------------------------------------------------

void C3DFn::GetNormY(CDVector &Ys)
  {
  flag b = (fabs(dYGRatio-1.0)<1.0e-6);
  for (long i=0; i<Ys.GetLen(); i++)
    {
    if (b)
      Ys[i] = (Ys[i] - dYLoLim) / (dYUpLim - dYLoLim);
    else
      Ys[i] = (log(Ys[i] / dYLoLim) / log(dYGRatio));
    }
  }

// -------------------------------------------------------------------------

double C3DFn::GetActualY(double y)
  {
  if (fabs(dYGRatio-1.0)<1.0e-6)
    return y * (dYUpLim - dYLoLim) + dYLoLim;
  return dYLoLim * pow(dYGRatio, y);
  }

// -------------------------------------------------------------------------

void C3DFn::GetActualY(CDVector &Ys)
  {
  flag b = (fabs(dYGRatio-1.0)<1.0e-6);
  for (long i=0; i<Ys.GetLen(); i++)
    if (b)
      Ys[i] = Ys[i] * (dYUpLim - dYLoLim) + dYLoLim;
    else
      Ys[i] = dYLoLim * pow(dYGRatio, Ys[i]);
  }
        
// -------------------------------------------------------------------------

double C3DFn::GetNormZ(double z)
  {
  if (fabs(dZGRatio-1.0)<1.0e-6)
    {
    if (_isnan(z))
      return dZLoLim;
    return (z - dZLoLim) / (dZUpLim - dZLoLim);
    }
  return log(z / dZLoLim) / log(dZGRatio);
  }

// -------------------------------------------------------------------------

void C3DFn::GetNormZ(CDVector &Zs)
  {
  flag b = (fabs(dZGRatio-1.0)<1.0e-6);
  for (long i=0; i<Zs.GetLen(); i++)
    {
    if (b)
      {
      if (_isnan(Zs[i]))
        Zs[i] = dZLoLim;
      else
        Zs[i] = (Zs[i] - dZLoLim) / (dZUpLim - dZLoLim);
      }
    else
      Zs[i] = (log(Zs[i] / dZLoLim) / log(dZGRatio));
    }
  }

// -------------------------------------------------------------------------

double C3DFn::GetActualZ(double z)
  {
  if (fabs(dZGRatio-1.0)<1.0e-6)
    return z * (dZUpLim - dZLoLim) + dZLoLim;
  return dZLoLim * pow(dZGRatio, z);
  }

// -------------------------------------------------------------------------

void C3DFn::GetActualZ(CDVector &Zs)
  {
  flag b = (fabs(dZGRatio-1.0)<1.0e-6);
  for (long i=0; i<Zs.GetLen(); i++)
    if (b)
      Zs[i] = Zs[i] * (dZUpLim - dZLoLim) + dZLoLim;
    else
      Zs[i] = dZLoLim * pow(dZGRatio, Zs[i]);
  }
        
// -------------------------------------------------------------------------

void C3DFn::SortByX(flag Ascending)
  {
  if (Z.GetLen()>1)
    {
    CDMatrix Temp(3, Z.GetLen());
    Temp.SetRow(0,X);
    Temp.SetRow(1,Y);
    Temp.SetRow(2,Z);
    Temp.SortRows(0, Ascending, 1, Ascending);
    Temp.GetRow(0,X);
    Temp.GetRow(1,Y);
    Temp.GetRow(2,Z);
    }
  }

// -------------------------------------------------------------------------

void C3DFn::SortByY(flag Ascending)
  {
  if (Z.GetLen()>1)
    {
    CDMatrix Temp(3, Z.GetLen());
    Temp.SetRow(0,X);
    Temp.SetRow(1,Y);
    Temp.SetRow(2,Z);
    Temp.SortRows(1, Ascending, 0, Ascending);
    Temp.GetRow(0,X);
    Temp.GetRow(1,Y);
    Temp.GetRow(2,Z);
    }
  }

// -------------------------------------------------------------------------

flag C3DFn::DoRedistribute()
  {
  return True;
  flag b = False;
  if (Z.GetLen()<=iRqdLength)
    b = CreatePts(iRqdLength);
  else
    b = EliminatePts(iRqdLength, iNoElimIter);
  return b;
  }

// -------------------------------------------------------------------------
/* bga
   This method will eliminate superfulous edge points from a 3-D function.
   It drops the points that have the least effect on the direction, gradient and area 
   under the curve with the edge unwound.
   lNoPtsRqd is the number of x points required
*/
// -------------------------------------------------------------------------

flag C3DFn::EliminateEdgePts(long lNoPtsRqd,CLMatrix &Edge)
  {
  ASSERT(FALSE); //NOT CHECKED !!!
  long N = Edge.GetRows();
  ASSERT(N>=lNoPtsRqd);//Whos who the program should not be here
  if (N<=lNoPtsRqd) 
    return False;//There are no points or there are already less points than the number required
  
  CDMatrix Weight(N,2);	 //sets up a matrix which represents the effect each point has on
                         //the graph coloumn 1 is the index and coloumn 2 the weight
  for (long i=0; i<N; i++)
    {
	  long lFstNo = Edge[i][1];
    long lSndNo = Edge[i][0];
    long lTrdNo = Edge[i][2];
    
	  double x1 = X[lFstNo];
	  double x2 = X[lSndNo];
	  double x3 = X[lTrdNo];
	  double y1 = Y[lFstNo];
	  double y2 = Y[lSndNo];
	  double y3 = Y[lTrdNo];
	  double z1 = Z[lFstNo];
	  double z2 = Z[lSndNo];
	  double z3 = Z[lTrdNo];
	  Weight[i][0] = Edge[i][0];//set index
    double dX1 = sqrt((x2-x1) * (x2-x1)+(y2-y1) * (y2-y1));
    double dX2 = sqrt((x2-x3) * (x2-x3)+(y2-y3) * (y2-y3));
    double dX3 = sqrt((x1-x3) * (x1-x3)+(y1-y3) * (y1-y3));
    double temp;
    if (x1==x2 || x3==x2 || x1==x3)
      temp = 1.0;
    else
      temp = ((y2-y1)/(x2-x1) - (y3-y2)/(x3-x2)) / (x3-x1);
	  temp = Max(temp, 1.0);
	  temp *= ((z2-z1)/dX1-(z3-z2)/dX2)/(dX3)*((dX1*(z1+z2)+dX2*(z2+z3)));//Change in gradients*
	                                                                                //effective area under point
	  Weight(i,1)	= fabs(temp);//>=0?temp:(temp*(-1)));//set weight       
	  }
  
  Weight.SortCols(1,FALSE);//sort weight by colum2 descending
  
  for (long j=0; j<lNoPtsRqd; j++)
  	Edge[j][0] = (long)Weight[j][0];
  return True;
  }

// -------------------------------------------------------------------------
/* bga
   This method will eliminate superfulous points from a 3d function.
   It drops the points that have the least effect on the gradient and area under the curve.
   lNoPtsRqd is the number of x points required
*/
// -------------------------------------------------------------------------

flag C3DFn::EliminatePts(long lNoPtsRqd,long NoIterations)
  {
  const long NPts = Z.GetLen();
  if (NPts<=lNoPtsRqd || lNoPtsRqd<3) 
    return False;
 
  if (NoIterations<1)
    return False; //You cant get rid of points with so few iterations

  ASSERT((NoIterations*2) < (NPts-lNoPtsRqd));//posibility of losing ratio of middle to edge points
 
  GetNormX(X);
  GetNormY(Y);
  GetNormY(Z);
  long RefNoPtsRqd = lNoPtsRqd;
  long DeltaPts = (long)ceil(double((NPts-lNoPtsRqd)/NoIterations));
  lNoPtsRqd = NPts;
  while (lNoPtsRqd>RefNoPtsRqd)
    {
    long N = X.GetLen();
    lNoPtsRqd -= DeltaPts;
    if (lNoPtsRqd<RefNoPtsRqd)
      lNoPtsRqd = RefNoPtsRqd;
    if (!bIsBuilt) 
      {
      GetActualX(X);
      GetActualY(Y);
      GetActualZ(Z);
      ReFit();//builds the tri network
      GetNormX(X);
      GetNormY(Y);
      GetNormZ(Z);
      }
    long M = Trinet.TriangleCount();
    ASSERT(M>2);//a tri network is present

    CDMatrix Key(N*M, 5);//sort matrix to extract important data
    CLMatrix Edge(N*M, 3); //the edge points

    //counters...
    long lTmpLineNo=0;
    long lLineNo=0;
    long lTriNo=0;
    long lNoCorectPts=0;
    long lNoEdge=0;
   
    long valid;
    double x0,x1,x2,y0,y1,y2,z0,z1,z2;
 
    for (long i=0; i<N; i++)//No of points
      {
      valid=0;
      Trinet.first_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2);//get first set of verti
      lTriNo = lTmpLineNo=0;
      while ((valid<2)&&(lTriNo<=M))
        {
        flag Do = (((X[i]==x0) && (Y[i]==y0)) ||
                  ((X[i]==x1) && (Y[i]==y1)) ||
                  ((X[i]==x2) && (Y[i]==y2)));
        if (Do)//point on triangle
          {
          if ((z0==1.0e30) || (z1==1.0e30) || (z2==1.0e30))//edgepoint
            {
            if (!(((z0==1.0e30)&&(z1==1.0e30))||((z2==1.0e30)&&(z1==1.0e30))||((z0==1.0e30)&&(z2==1.0e30))))
              {
              double tmpx,tmpy,tmpz;
              if ((X[i]==x0) && (Y[i]==y0))
                {
                if (z1==1.0e30)
                  {
                  tmpx=x2;
                  tmpy=y2;
                  tmpz=z2;
                  }
                else
                  {
                  tmpx=x1;
                  tmpy=y1;
                  tmpz=z1;
                  }
                }
              if ((X[i]==x1) && (Y[i]==y1))
                {
                if (z0==1.0e30)
                  {
                  tmpx=x2;
                  tmpy=y2;
                  tmpz=z2;
                  }
                else
                  {
                  tmpx=x0;
                  tmpy=y0;
                  tmpz=z0;
                  }
                }
              if ((X[i]==x2) && (Y[i]==y2))
                {
                if (z1==1.0e30)
                  {
                  tmpx=x0;
                  tmpy=y0;
                  tmpz=z0;
                  }
                else
                  {
                  tmpx=x1;
                  tmpy=y1;
                  tmpz=z1;
                  }
                }
              long lPntCnt = 0;
              while ((X[lPntCnt]!=tmpx) || (Y[lPntCnt]!=tmpy)) 
                lPntCnt++;
              Edge[lNoEdge][0] = i;
              Edge[lNoEdge][++valid] = lPntCnt;
              if (valid==2)//find other triangle verticies
                {
                lNoEdge++;
                lLineNo -= lTmpLineNo;
                }
              }
            }
          else if (valid ==0) //not an edge point
            {
            double D1,D2,dHt,dX,dHt1,dHt2,dX1,dX2,Direction;
            if ((X[i]==x0) && (Y[i]==y0))
              {
              D1 = atan2((y0-y1),(x0-x1));
              D2 = atan2((y0-y2),(x0-x2));
              dHt1 = z1;
              dHt2 = z2;
              dX1 = sqrt((y0-y1)*(y0-y1)+(x0-x1)*(x0-x1));
              dX2 = sqrt((y0-y2)*(y0-y2)+(x0-x2)*(x0-x2));
              }
            if ((X[i]==x1) && (X[i]==y1))
              {
              D1=atan2((y1-y0),(x1-x0));
              D2=atan2((y1-y2),(x1-x2));
              dHt1=z0;
              dHt2=z2;
              dX1=sqrt((y1-y0)*(y1-y0)+(x1-x0)*(x1-x0));
              dX2=sqrt((y1-y2)*(y1-y2)+(x1-x2)*(x1-x2));
              }
            if ((X[i]==x2) && (X[i]==y2))
              {
              D1=atan2((y2-y0),(x2-x0));
              D2=atan2((y2-y1),(x2-x1));
              dHt1=z0;
              dHt2=z1;
              dX1=sqrt((y2-y0)*(y2-y0)+(x2-x0)*(x2-x0));
              dX2=sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
              }
    
            if (((D1>0)&&(D2>0)) || ((D1<0)&&(D2<0)))// 
              {
              if (D1<D2)
                {
                Direction=D1;
                dHt=dHt1;
                dX=dX1;
                }
              else
                {
                Direction=D2;
                dHt=dHt2;
                dX=dX2;
                }
              }
            else
              {
              if (fabs(D1-D2)<PI)
                {
                if (D1<D2)
                  {
                  Direction=D1;
                  dHt=dHt1;
                  dX=dX1;
                  }
                else
                  {
                  Direction=D2;
                  dHt=dHt2;
                  dX=dX2;
                  }
                }
              else
                {
                if (D1>D2)
                  {
                  Direction=D1;
                  dHt=dHt1;
                  dX=dX1;
                  }
                else
                  {
                  Direction=D2;
                  dHt=dHt2;
                  dX=dX2;
                  }
                }
              }
            Key[lLineNo][0] = (double)i;         //point no
            ASSERT((Key[lLineNo][0] - (long)Key[lLineNo][0])==0.0);//huh can't the computer count
            Key[lLineNo][1] = lTriNo;            //triangle number
            Key[lLineNo][2] = Direction+PI;      //direction made positive (0-2PI)
            Key[lLineNo][3] = dHt;               //hieght at direction point
            Key[lLineNo++][4] = dX;              //distance from direction point
            lTmpLineNo++;
            }
          }//if do
        Trinet.next_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2);
        lTriNo++;
        }//while
      if (valid==0)
        lNoCorectPts++;
      }//i
    Edge.SetSize(lNoEdge, 3);

    double MidToEdg = (float)lNoCorectPts/lNoEdge;
    if (lNoPtsRqd/(MidToEdg+1)<lNoEdge)
      {
      lNoEdge=(long)ceil(lNoPtsRqd/(MidToEdg+1));
      EliminateEdgePts(lNoEdge,Edge);
      }

    Key.SetSize(lLineNo,5);//right size matrix
    Key.SortCols(0,True,2,True);//by PntNo then Direction
 
    CDMatrix Weight((lNoCorectPts),2);//matrix for point importance
    long lWeightLine=0;
    long C = lLineNo;
    for (i=0; i<C; i++)
      {
      ASSERT((Key[i][0] - (long)Key[i][0])==0.0);//huh can't the computer count
      long Count = 1;
      Weight[lWeightLine][0]=Key[i][0];//Set point index
      flag bEnd = False;
      while (((long)Key[i][0]==(long)Key[i+Count][0]) && !bEnd)//same point
        {
        Count++;
        if (lLineNo==Count+i+1) 
          bEnd = True;
        }
      if (bEnd) 
        Count++;
      ASSERT(Count>2);//need at least three triangles else should have been edge
      long TmpCount;
  //  if ((YMin()==3.0||YMin()==4.0||YMin()==5.0))
  //    {
      for (long j=0; j<10; j++) //bga 10 diferent directions could be anything
        {
        flag FoundTri = False;
        flag FoundTri2 = False;
        long lTriNo2;
        TmpCount = 0;
        double RefDirec = PI/10*j;
        while (!FoundTri || !FoundTri2)//find the tri opposite to current direction
          {
          if (++TmpCount==Count) 
            TmpCount=0;
          double dVal=(Key[i+TmpCount][2]-RefDirec);
          if (dVal>0.0)
            {
            if ((dVal>PI) && !FoundTri) 
              {
              FoundTri=True;//further away(gone past)
              lTriNo = (long)Key[i+TmpCount-1][1];
              }
            }//if
          else
            {
            dVal += 2.0*PI;
            if ((dVal>PI)&&!FoundTri) 
              {
              FoundTri=True;
              lTriNo = (long)Key[i+TmpCount-1][1];
              }
            }//else
          dVal=(Key[i+TmpCount][2]-RefDirec+PI);
          if ((dVal>0)&&!FoundTri2)
            {
            if (dVal>PI) 
              {
              FoundTri2=True;//further away(gone past)
              lTriNo2 = (long)Key[i+TmpCount-1][1];
              }
            }//if
          else
            {
            dVal += 2*PI;
            if ((dVal>PI)&&!FoundTri2) 
              {
              FoundTri2=True;
              lTriNo2 = (long)Key[i+TmpCount-1][1];
              }
            }//else
          }//while found triangle
        if (--TmpCount<0) TmpCount=Count-1;//previous triangle was the right one
        long lPtNo   = (long)Key[i+TmpCount][0];
        double dDirec  = RefDirec;
        double dX;
        VERIFY(FALSE); //KGA what should dX be set to ???

        //get details of point from tri 2
        VERIFY(Trinet.Getr(lTriNo2,dX));//r of tri net is radius of circum circle
        double dDist   = dX;
        double dTmpx   = (X[lPtNo])+sin(dDirec+PI)*dDist;
        double dTmpy   = (Y[lPtNo])+cos(dDirec+PI)*dDist;
        double dTmpht;
        VERIFY(Trinet.GetZ(lTriNo2,dTmpx,dTmpy,dTmpht));
        dTmpht = (dTmpht);
        double dNormht = (Y.m_d[lPtNo]);
        double  r;
        //VERIFY(FALSE); //KGA what should r be set to ???

        //get details of point from tri 1
        VERIFY(Trinet.Getr(lTriNo,r));
        dDist   = r;
        dTmpx   = (X[lPtNo])+sin(dDirec)*dDist;
        dTmpy   = (Y[lPtNo])+cos(dDirec)*dDist;
        double dTmpz;
        VERIFY(Trinet.GetZ(lTriNo,dTmpx,dTmpy,dTmpz));

        // make the dips as important as the peaks
        if (dNormht<0.5) 
          dNormht = 1.0-dNormht;
        if (dTmpz<0.5) 
          dTmpz = 1.0-dTmpz;
        if (dTmpht<0.5) 
          dTmpht = 1.0-dTmpht;

        double temp = ((dNormht-dTmpht)/dX-(dTmpz-dNormht)/dDist)/(dX+dDist)*
                      ((dX*(dTmpht+dNormht)+dDist*(dNormht+dTmpz)));//Change in gradients*effective area under point

        Weight[lWeightLine][1] += fabs(temp);
        }
      lWeightLine++;
      i += Count-1;
  //bga development mothod that uses trinet and not 10 directions
//      }
  /*  else
     {
      for(long j=0;j<Count;j++)
        {
        long lMarker=i+j;
        TmpCount=j;
        flag FoundTri=False;
        while (!FoundTri)//find the tri opposite to current direction
          {
          if (++TmpCount==Count) TmpCount=0;
          double dVal=(Key[i+TmpCount][2]-Key[lMarker][2]);
          if ((dVal)>0)
            {
            if (dVal>PI) FoundTri=True;//further away(gone past)
            }//if
          else
            {
            dVal += 2*PI;
            if (dVal>PI) FoundTri=True;
            }//else
          }//while
        if (--TmpCount<0) TmpCount=Count-1;//previous triangle was the right one
        long lPtNo   = (long)Key.m_d[i+TmpCount][0];
             lTriNo  = (long)Key.m_d[i+TmpCount][1];
        double dDirec  = Key.m_d[lMarker][2];
        double dTmpht  = Key.m_d[i+TmpCount][3];
        double dNormht = (Y.m_d[lPtNo]-zmin)/(zmax-zmin);
        double dX      = Key.m_d[lMarker][4];
        double  r;
        ASSERT(Trinet.Getr(lTriNo,r));
        double dDist   = r;
        double dTmpx   = (X.m_d[0][lPtNo]-xmin)/(xmax-xmin)+sin(dDirec)*dDist;
        double dTmpy   = (X.m_d[1][lPtNo]-ymin)/(ymax-ymin)+cos(dDirec)*dDist;
        double dTmpz;
        ASSERT(Trinet.GetZ(lTriNo,dTmpx,dTmpy,dTmpz));
        dTmpz = (dTmpz-zmin)/(zmax-zmin);
        //double dX      = hypot(X.m_d[0][lPtNo],X.m_d[1][lPtNo]);
  
        if (XMax(1)==2.0)
          {
          if (dNormht<0.5) dNormht = 1-dNormht;
          if (dTmpz<0.5) dTmpz = 1-dTmpz;
          if (dTmpht<0.5) dTmpht = 1-dTmpht;
          }
        double temp = ((dNormht-dTmpht)/dX-(dTmpz-dNormht)/dDist)/(dX+dDist)*
                      ((dX*(dTmpht+dNormht)+dDist*(dNormht+dTmpz)));//Change in gradients*effective area under point

        double temp1 = ((dNormht-dTmpht)/dX-(dTmpz-dNormht)/dDist)/(dX+dDist);//Change in gradients*

        double temp2 = ((dX*(dTmpht+dNormht)+dDist*(dNormht+dTmpz)));//effective area under point
  
        if (YMin()== 1) 
           Weight.m_d[lWeightLine][1]+=fabs(temp1);
        else 
          if (YMin()== 2) 
            Weight.m_d[lWeightLine][1]+=fabs(temp2);
          else 
            Weight.m_d[lWeightLine][1]+=fabs(temp);

        }//j
      Weight.m_d[lWeightLine][1]=Weight.m_d[lWeightLine][1]/Count;
      lWeightLine++;
      i += Count-1;
      }*/
    }
    Weight.SortCols(1,False);//sort weight

    CDMatrix temp(lNoPtsRqd,3);//create a temporary matrix to restore the required points

    for (long j=0; j<lNoEdge; j++)
      {					
      temp[j][0] = X[Edge[j][0]];//edge points
      temp[j][1] = Y[Edge[j][0]];
      temp[j][2] = Z[Edge[j][0]];
  	  }
    for (j = lNoEdge; j<lNoPtsRqd;j++)
      {					
      temp[j][0] = X[(long)Weight[j-lNoEdge][0]];//middle points
      temp[j][1] = Y[(long)Weight[j-lNoEdge][0]];
      temp[j][2] = Z[(long)Weight[j-lNoEdge][0]];
  	  }

    SetLength(lNoPtsRqd);
    for (j=0; j<lNoPtsRqd; j++)//set points
      SetPt(j, temp[j][0], temp[j][1], temp[j][2]);
    bIsBuilt = False;
    }
  GetActualX(X);
  GetActualY(Y);
  GetActualY(Z);
  View().DoRebuild();
  return True;
  }

// -------------------------------------------------------------------------
//bga work in progress will be finished
// -------------------------------------------------------------------------
flag C3DFn::CreatePts(long lNoPtsRqd)
  {
/*  double xmax = XUpLim[0];//get outer boundry limits
  double xmin = XLoLim[0];
  double ymax = XUpLim[1];
  double ymin = XLoLim[1];
  GetNormX(xmax,0);
  GetNormX(xmin,0);
  GetNormX(ymax,1);
  GetNormX(ymin,1);

  long MSize = (long)ceil(sqrt(lNoPtsRqd*10)); //divide into a square matrix
                                        //it is imposible to say which axes should have more points
  CDMatrix Points(3,lNoPtsRqd);
  CDMatrix TempSort(2,lNoPtsRqd);//weighting of points
  CDMatrix TmpPts(3,(long)pow(MSize+1,2));//working matrix
  CDMatrix ImportantPts(2,(long)pow(MSize+1,2));//Turning Points and points of inflection

  double dx = (xmax-xmin)/(MSize-1);//even grid
  double dy = (ymax-ymin)/(MSize-1);

  for (long i=0;i<(MSize);i++)
    for (long j=0;j<(MSize);j++)
      {
//      TmpPts(0,i,j)=i*dx+xmin;
//      TmpPts(1,i,j)=j*dy+ymin;
      }
/*====================================================
grid point reference numbers

            P16     P9     P10

                 P8 P1 P2
                 
            P15  P7 P0 P3  P11

                 P6 P5 P4
                    
            P14     P13    P12

=====================================================*/
  /*long ImpCnt = 0;
  for (i=1;i<(MSize-1);i++)
    for (long j=1;j<(MSize-1);j++)
      {
//      double P0=Yx(TmpPts(0,i  ,j  ),TmpPts(1,i  ,j  ));
//      double P1=Yx(TmpPts(0,i-1,j  ),TmpPts(1,i-1,j  ));
//      double P2=Yx(TmpPts(0,i-1,j+1),TmpPts(1,i-1,j+1));
//      double P3=Yx(TmpPts(0,i  ,j+1),TmpPts(1,i  ,j+1));
//      double P4=Yx(TmpPts(0,i+1,j+1),TmpPts(1,i+1,j+1));
//      double P5=Yx(TmpPts(0,i+1,j  ),TmpPts(1,i+1,j  ));
//      double P6=Yx(TmpPts(0,i+1,j-1),TmpPts(1,i+1,j-1));
//      double P7=Yx(TmpPts(0,i  ,j-1),TmpPts(1,i  ,j-1));
//      double P8=Yx(TmpPts(0,i-1,j-1),TmpPts(1,i-1,j-1));
    
//      if ((((P0>P1)&&(P0>P2)&&(P0>P3)&&(P0>P4)&&(P0>P5)&&(P0>P6)&&(P0>P7)&&(P0>P8))
  //      ||((P0<P1)&&(P0<P3)&&(P0<P1)&&(P0<P3)&&(P0<P1)&&(P0<P3)&&(P0<P1)&&(P0<P3)))
    //    &&InRegion(TmpPts(0,i  ,j  ),TmpPts(1,i  ,j  )))
        {
        ImportantPts(0,ImpCnt)=i;//local max or min
        ImportantPts(1,ImpCnt)=j;//local max or min
        ImpCnt++;
        }
//      if ((i>1)&&(i<(MSize-2))&&(j>1)&&(j<(MSize-2))
//         &&InRegion(TmpPts(0,i  ,j  ),TmpPts(1,i  ,j  )))
        {
//        double P9 =Yx(TmpPts(0,i-2,j  ),TmpPts(1,i-2,j  ));
//        double P10=Yx(TmpPts(0,i-2,j+2),TmpPts(1,i-2,j+2));
//        double P11=Yx(TmpPts(0,i  ,j+2),TmpPts(1,i  ,j+2));
//        double P12=Yx(TmpPts(0,i+2,j+2),TmpPts(1,i+2,j+2));
//        double P13=Yx(TmpPts(0,i+2,j  ),TmpPts(1,i+2,j  ));
//        double P14=Yx(TmpPts(0,i+2,j-2),TmpPts(1,i+2,j-2));
//        double P15=Yx(TmpPts(0,i  ,j-2),TmpPts(1,i  ,j-2));
//        double P16=Yx(TmpPts(0,i-1,j-2),TmpPts(1,i-1,j-2));
/*        
        double m1 =(P0-P9);//vert
        double m2 =(P5-P1);
        double m3 =(P13-P0);

        double m4 =(P0-P14);//45
        double m5 =(P2-P6);
        double m6 =(P10-P0);

        double m7 =(P0-P15);//horiz
        double m8 =(P3-P7);
        double m9 =(P11-P0);

        double m10=(P0-P16);//135
        double m11=(P4-P8);
        double m12=(P12-P0);
        if((((m2>m1)&&(m2>m3))||((m2<m1)&&(m2<m3)))||
           (((m5>m4)&&(m5>m6))||((m5<m4)&&(m5<m6)))||
           (((m8>m7)&&(m8>m9))||((m8<m7)&&(m8<m9)))||
           (((m11>m10)&&(m11>m12))||((m11<m10)&&(m11<m12))))*/
    /*      {
          ImportantPts(0,ImpCnt)=i;//local point of inflection
          ImportantPts(1,ImpCnt)=j;//local point of inflection
          ImpCnt++;
          }
        }
      }

/*
  if (ImpCnt>lNoPtsRqd) 
    {            
    SetLength(ImpCnt);
    for (i=0;i<ImpCnt;i++)
      {
      double tempy = Yx(ImportantPts[i]);
      GetNormY(tempy);
      SetPt(i,ImportantPts[i],tempy);
      }
    SortByX;
    EliminatePts(lNoPtsRqd);
    return 0;  
    }
  dx = (xmax-xmin)/(lNoPtsRqd-1);//create even distribution
  for (i=0;i<lNoPtsRqd;i++)
    {
    Points(0,i)=i*dx+xmin;
    }
  long count1=0;
  long count2=1;
  Points(1,0)=1.0;//store edge point
  Points(1,lNoPtsRqd-1)=1.0;//store edge point
  
  while (count1<ImpCnt)
    {
    if (ImportantPts[count1]<Points(0,count2))
      {
      if (((ImportantPts[count1] - Points(0,count2-1))<
           (Points(0,count2)-ImportantPts[count1]))&&(Points(1,count2-1)==0))
        {
        Points(0,count2-1)=ImportantPts[count1];
        Points(1,count2-1)=1.0;
        }
      else
        {
        Points(0,count2)=ImportantPts[count1];
        Points(1,count2)=1.0;
        }
      count1++;
      }
    count2++;
    }
  count1=0;
  flag end=False;
  CLVector LastMove(2);
  LastMove[0]=0;
  LastMove[1]=0;
  TmpPts.SetSize(lNoPtsRqd);
  //flag FirstTime=True;
  while ((count1<MaxIter)&&(!end))
    {
    for (i=1;i<(lNoPtsRqd-1);i++)
      {
      double P1 = Yx(Points(0,i-1));
    //  GetNormY(P1);
      double P2 = Yx(Points(0,i));
  //    GetNormY(P2);
      double P3 = Yx(Points(0,i+1));
//      GetNormY(P3);
      double m1=(P1-P2)/(Points(0,i-1)-Points(0,i));
      double m2=(P2-P3)/(Points(0,i)-Points(0,i+1));
      TempSort(0,i-1) = i;
      TempSort(1,i-1) = fabs((m1-m2));
      }
    TempSort.SortRows(1);//sort by weight
    long line=0;
    count2 = 0;
    while (count2 < 2)
      {
      if (Points(1,(long)TempSort(0,line))==0)
        {
        if ((TempSort(0,line)!=LastMove[0])&&(TempSort(0,line)!=LastMove[1]))
          {
          TmpPts[(long)TempSort(0,line)] =(Points(0,(long)TempSort(0,lNoPtsRqd-count2-3)+1)-
                                         Points(0,(long)TempSort(0,lNoPtsRqd-count2-3)))*1/2+
                                         Points(0,(long)TempSort(0,lNoPtsRqd-count2-3));
          LastMove[count2]=(long)TempSort(0,line);
          count2++;
          }
        else
          {
          end=True;
          }
        }
      else  
        TmpPts[(long)TempSort(0,line)]=Points(0,(long)TempSort(0,line));
      line++;
      }  
    //  }
    for (i=line;i<(lNoPtsRqd-2);i++)
      {
      TmpPts[(long)TempSort(0,i)]=Points(0,(long)TempSort(0,i));
      }
        
      if(!end)
        {
        for (i=1;i<lNoPtsRqd-1;i++)
          {
          Points(0,i)=TmpPts[i];
          }
        }
    Points.SortRows(0,1);
    for (i=1;i<lNoPtsRqd;i++)
      {
      TmpPts[i]=0;
      }
    count1++;
    //ASSERT(TempSort(0,lNoPtsRqd-3)>1);
    }//while   
        SetLength(lNoPtsRqd);
        for (i=0;i<lNoPtsRqd;i++)
          {
          double tempy=Yx(Points(0,i));
          GetNormY(tempy);
          SetPt(i,Points(0,i),tempy);
          }
        
    SortByX();    */
    GetActualX(X);
    GetActualY(Y);
  return 1;
  }

// -------------------------------------------------------------------------
//####### NNB Eliminate points should be run after your application using this function.###############
// -------------------------------------------------------------------------

void C3DFn::CombineSurfaces(C3DFn& Fn)
  {
  ASSERT(FALSE); //not checked
  /*char y[2*MaxFnPts];
  //double x[2*MaxFnPts];
  CDMatrix x(2,2*MaxFnPts);
  long n1 = pS1->NPts();
  long n2 = pS2->NPts();
  pS1->SortByX(0,1,1,1);//to give the arrays corresponding orders
  pS2->SortByX(0,1,1,1);
  long i1=0, i2=0, i=0;
  while (i1<n1 || i2<n2)
    {
    if (i1<n1 && i2<n2)
      {
      double x1 = pS1->Xp(i1,0);
      double x2 = pS1->Xp(i1,0);
      double y1 = pS2->Xp(i2,1);
      double y2 = pS2->Xp(i2,1);
      
      if ((fabs(x1-x2) <= (1.0e-3*Max(fabs(x1), fabs(x2))+1.0e-12))&&
         (fabs(y1-y2) <= (1.0e-3*Max(fabs(x1), fabs(x2))+1.0e-12)))   // Same Point - Merge
        {
        x.m_d[0][i]=(x1+x2)*0.5;
        x.m_d[1][i]=(y1+y2)*0.5;
        y[i]=3;
        ++i1;
        ++i2;
        }
      else if (x1<x2)
        {
        x.m_d[0][i]=x1;
        x.m_d[1][i]=y1;
        y[i]=1;
        ++i1;
        }
      else // (x1<x2)
        {
        x.m_d[0][i]=x2;
        x.m_d[1][i]=y2;
        y[i]=2;
        ++i2;
        }
      }
    else if (i1<n1)
      {
      x.m_d[0][i]=pS1->Xp(i1,0);
      x.m_d[1][i]=pS1->Xp(i1,1);
      y[i]=1;
      ++i1;
      }
    else //(i2<n2)
      {
      x.m_d[0][i]=pS2->Xp(i2,0);
      x.m_d[1][i]=pS2->Xp(i2,1);
      y[i]=2;
      ++i2;
      }
    i++;
    }

  SetLength(i);
  for (long j=0; j<i; j++)
    SetPt(j, x.m_d[0][j], x.m_d[1][j], y[j]);*/
  } 
// -------------------------------------------------------------------------

flag C3DFn::Add(double Offset)
  {
  if (Z.GetLen()==0)
    return False;

  Z.Add(Offset);

  ReFit();

  return True;  
  }

// -------------------------------------------------------------------------

flag C3DFn::Add(C3DFn& Fn)
  {
  if (Z.GetLen()==0 && Fn.Z.GetLen()==0)
    return False;

  CombineSurfaces(Fn);

  const long N = Z.GetLen();
  for (long i=0; i<N; i++)
    Z[i] = Zxy(X[i], Y[i]) + Fn.Zxy(X[i], Y[i]);
  
  if (bRedist)
    DoRedistribute();
  ReFit();

  return True;   
  }

// -------------------------------------------------------------------------

flag C3DFn::Multiply(double Scalar)
  {
  if (Z.GetLen()==0)
    return False;

  Z.Mult(Scalar);

  ReFit();

  return True;
  }

// -------------------------------------------------------------------------

flag C3DFn::Multiply(C3DFn& Fn)
  {
  if (Z.GetLen()==0 && Fn.Z.GetLen()==0)
    return False;

  CombineSurfaces(Fn);

  const long N = Z.GetLen();
  for (long i=0; i<N; i++)
    Z[i] = Zxy(X[i], Y[i]) * Fn.Zxy(X[i], Y[i]);
  
  if (bRedist)
    DoRedistribute();
  ReFit();

  return True;   
  }

// -------------------------------------------------------------------------

XID xidXYFnLen     = XyXID(10000);
XID xid3DFnName    = XyXID(10001);
XID xidXYPtsFName  = XyXID(10002);

const int Id_Name        =   1;
const int Id_Xf          =   3;
const int Id_Yf          =   4;
const int Id_Zf          =   5;
const int Id_PtsFName    =   6;
const int Id_XfMn        =   8;
const int Id_YfMn        =   9;
const int Id_ZfMn        =  10;
const int Id_XfMx        =  11;
const int Id_YfMx        =  12;
const int Id_ZfMx        =  13;
const int Id_Tag         =  14;
const int Id_Len         =  15;
const int Id_RqdLen      =  16;
const int Id_NoElimIter  =  17;
const int Id_ShowData    =  24;
const int Id_XDivs       =  26;
const int Id_YDivs       =  27;
const int Id_ContourInt  =  28;
const int Id_XfLo        =  30;
const int Id_YfLo        =  31;
const int Id_ZfLo        =  32;
const int Id_XfUp        =  33;
const int Id_YfUp        =  34;
const int Id_ZfUp        =  35;
const int Id_XfGR        =  36;
const int Id_YfGR        =  37;
const int Id_ZfGR        =  38;

const int Id_SaveBtn       = 150;
const int Id_CancelBtn     = 151;
const int Id_SortXBtn      = 152;
const int Id_SortYBtn      = 153;
const int Id_RefitBtn      = 154;
const int Id_ShowBtn       = 155;
const int Id_ZMnMxBtn      = 156;
const int Id_InitParmsBtn  = 157;
const int Id_ZToCurveBtn   = 158;
const int Id_LoadPtsBtn    = 159;
const int Id_SavePtsBtn    = 160;
const int Id_DistributeBtn = 161;

// -------------------------------------------------------------------------

void C3DFn::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  Strng S;
  DDBValueLstMem DDB0;
  TagObjClass::GetSDescValueLst(C3DModel::GroupName, DDB0);
  DDB.String("Eqn",         "", DC_,         "",     xid3DFnName,  this, isParm, DDB0());
  DDB.Long  ("XDivs",        "", DC_,         "", &iXDivs,      this, isParm);
  DDB.Long  ("YDivs",        "", DC_,         "", &iYDivs,      this, isParm);
  DDB.Long  ("ContourInter", "", DC_,         "", &iContourIntervals,this, isParm);
  DDB.Long  ("Len",          "", DC_,         "", xidXYFnLen,   this, isParm);
  DDB.Long  ("RqdLen",       "", DC_,         "", &iRqdLength,  this, isParm);
  DDB.Long  ("NoElimIter",   "", DC_,         "", &iNoElimIter, this, isParm);
  DDB.Double("XDispMin",     "", DC_,         "", &dXDispMin,   this, isParm);
  DDB.Double("XDispMax",     "", DC_,         "", &dXDispMax,   this, isParm);
  DDB.Double("YDispMin",     "", DC_,         "", &dYDispMin,   this, isParm);
  DDB.Double("YDispMax",     "", DC_,         "", &dYDispMax,   this, isParm);
  DDB.Double("ZDispMin",     "", DC_,         "", &dZDispMin,   this, isParm);
  DDB.Double("ZDispMax",     "", DC_,         "", &dZDispMax,   this, isParm);
  DDB.Double("XLower",       "", DC_,         "", &dXLoLim,     this, isParm);
  DDB.Double("XUpper",       "", DC_,         "", &dXUpLim,     this, isParm);
  DDB.Double("YLower",       "", DC_,         "", &dYLoLim,     this, isParm);
  DDB.Double("YUpper",       "", DC_,         "", &dYUpLim,     this, isParm);
  DDB.Double("ZLower",       "", DC_,         "", &dZLoLim,     this, isParm);
  DDB.Double("ZUpper",       "", DC_,         "", &dZUpLim,     this, isParm);
  DDB.Double("XGRatio",      "", DC_,         "", &dXGRatio,    this, isParm);
  DDB.Double("YGRatio",      "", DC_,         "", &dYGRatio,    this, isParm);
  DDB.Double("ZGRatio",      "", DC_,         "", &dZGRatio,    this, isParm);
  DDB.String("PtsFile",      "", DC_,         "", xidXYPtsFName,this, isParm);
  DDB.String("XName",        "", DC_,         "", &sXName,      this, isParm);
  DDB.String("YName",        "", DC_,         "", &sYName,      this, isParm);
  DDB.String("ZName",        "", DC_,         "", &sZName,      this, isParm);
  DDB.Bool  ("ShowData",     "", DC_,         "", &bShowData,   this, isParm);
  DDB.Bool  ("Redist",       "", DC_,         "", &bRedist,     this, isParm);
  DDB.BeginArray(this, "3D", "3DPts", Length(), xidXYFnLen);
  for (int i=0; i<Length(); i++)
    {
    DDB.BeginElement(this, i); 
    DDB.Double("X",     "",     DC_,    "",   &X.m_d[i],      this, isParm); 
    DDB.Double("Y",     "",     DC_,    "",   &Y.m_d[i],      this, isParm); 
    DDB.Double("Z",     "",     DC_,    "",   &Z.m_d[i],      this, isParm); 
    }
  DDB.EndArray();
  pM->BuildDataDefn(DDB);
  DDB.EndStruct();
  }

// -------------------------------------------------------------------------

flag C3DFn::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB)) 
    return 1;
  switch (DCB.lHandle)
    {
    case xid3DFnName: DCB.pC = ModelName(DCB.rpC);
      return 1;
    case xidXYFnLen: DCB.L = Length(DCB.rL);      //DCB.StructureChanged();
            return 1;
    case xidXYPtsFName: 
      if (DCB.rpC)
        PtsFName = DCB.rpC;
      DCB.pC = PtsFName(); //DCB.StructureChanged();
      return 1;
    }
  return False;
  }

//---------------------------------------------------------------------------

void C3DFn::Build()
  {
  pchar p=Tag();
  StartPage((strlen(p)>0) ? p : "Fn");
  StartBlk(1, 0, NULL);

  int L=0;
  SetDParm(L,"Eqn", 4, "", Id_Name  ,  30, 0, "");
  Strng S;
  pTagObjClass pC;
  for (int i=0; (pC=TagObjClass::FindClassIndexed(C3DModel::GroupName, i))!=NULL; i++)
    {
    pC->GetShortDesc(S);
    FldHasFixedStrValue(i, S());
    }

  pM->SetView(pView);
  pM->Build();

  long Ln = Max(8L, Length());
  const long RowsHdr = 15L;
  long TotLen = Max(RowsHdr, RowsHdr+Ln);
  StartBlk((int)TotLen, RowsHdr, NULL);

  L=0;
  SetDParm(L,"Pts File", 12, "", Id_PtsFName, 40, 0, "");
  L++;
  SetDParm(L,"X Divisions", 12, "", Id_XDivs,  4, 2, "");
  SetDParm(L,"Y Divisions", 12, "", Id_YDivs,  4, 2, "");
  L++;
  SetDParm(L,"No of Contour Intervals", 24, "", Id_ContourInt,  6, 0, "");
  L++;
  SetSpace(L, 1);
  SetButton(L,"Sort X",  Id_SortXBtn,  8, 0, "");
  SetSpace(L, 1);
  SetButton(L,"Sort Y",  Id_SortYBtn,  8, 0, "");
  SetSpace(L, 1);
  if (bShowData)
    SetButton(L,"Hide Pts",  Id_ShowBtn,  10, 0, "");
  else
    SetButton(L,"Show Pts",  Id_ShowBtn,  10, 0, "");
  //if (PtsFName.Length()>0)
    {
    SetSpace(L, 1);
    SetButton(L,"Load Pts",  Id_LoadPtsBtn,  10, 0, "");
    SetSpace(L, 1);
    SetButton(L,"Save Pts",  Id_SavePtsBtn,  10, 0, "");
    }
  L++;
  SetSpace(L, 1);
  SetButton(L,"Z To Curve",  Id_ZToCurveBtn,  12, 0, "");
  SetSpace(L, 1);
  SetButton(L,"Distribute", Id_DistributeBtn, 12, 0, "");
  SetSpace(L, 1);
  if (pM->NParms()>0)
    {
    SetSpace(L, 1);
    SetButton(L,"Init Parms",  Id_InitParmsBtn, 12, 0, "");
    }
  L++;
  SetSpace(L, 1);
  SetButton(L,"ReFit",  Id_RefitBtn,  7, 0, "");
// To GoBack
//  SetSpace(L, 1);
//  SetButton(L,"Z Mn/Mx",  Id_ZMnMxBtn,  9, 0, "");
  L++;
  static Strng sx;
  static Strng sy;
  static Strng sz;
  sx = sXName();
  sx = sx.Left(9);
  sy = sYName();
  sy = sy.Left(8);
  sz = sZName();
  sz = sz.Left(8);
  SetSpace(L,13);
  SetDesc(L, sx(), -1, 9, 2, "");
  SetDesc(L,"(X)", -1, 3, 2, "");
  SetSpace(L,1);
  SetDesc(L, sy(), -1, 8, 2, "");
  SetDesc(L,"(Y)", -1, 3, 2, "");
  SetSpace(L,1);
  SetDesc(L, sz(), -1, 8, 2, "");
  SetDesc(L,"(Z)", -1, 3, 2, "");
  L++;
  SetDesc(L,"Display Min", -1, 14, 0, "");
  //SetSpace(L,2);
  SetParm(L,"", Id_XfMn, 12, 2, "");
  SetParm(L,"", Id_YfMn, 12, 2, "");
  SetParm(L,"", Id_ZfMn, 12, 2, "");
  L++;
  SetDesc(L,"Display Max", -1, 14, 0, "");
  //SetSpace(L,2);
  SetParm(L,"", Id_XfMx, 12, 2, "");
  SetParm(L,"", Id_YfMx, 12, 2, "");
  SetParm(L,"", Id_ZfMx, 12, 2, "");
  L++;
  SetDesc(L,"Lower Boundry", -1, 14, 0, "");
  SetParm(L,"", Id_XfLo, 12, 2, "");
  SetParm(L,"", Id_YfLo, 12, 2, "");
  SetParm(L,"", Id_ZfLo, 12, 2, "");
  L++;
  SetDesc(L,"Upper Boundry", -1, 14, 0, "");
  SetParm(L,"", Id_XfUp, 12, 2, "");
  SetParm(L,"", Id_YfUp, 12, 2, "");
  SetParm(L,"", Id_ZfUp, 12, 2, "");
  L++;
  SetDesc(L,"Geom Ratio", -1, 14, 0, "");
  //SetSpace(L,3);
  SetParm(L,"", Id_XfGR, 6, 2, "");
  SetSpace(L,6);
  SetParm(L,"", Id_YfGR, 6, 2, "");
  SetSpace(L,6);
  SetParm(L,"", Id_ZfGR, 6, 2, "");
  L++;
  SetDParm(L,"Length", 14,"", Id_Len,  6, 0, "");
  SetSpace(L,2);
  SetDParm(L,"Required ", 14, "", Id_RqdLen,  6, 2, "");
//  L++;                          
//  SetDParm(L,"Eliminate Factor", 14, "", Id_NoElimIter,  11, 0, "");
  if (bShowData)
    {
    L++;
    SetSpace(L,5);
    SetDesc(L, sx(), -1, 9, 2, "");
    SetDesc(L,"(X)", -1, 3, 2, "");
    SetSpace(L,1);
    SetDesc(L, sy(), -1, 8, 2, "");
    SetDesc(L,"(Y)", -1, 3, 2, "");
    SetSpace(L,1);
    SetDesc(L, sz(), -1, 8, 2, "");
    SetDesc(L,"(Z)", -1, 3, 2, "");
    L = RowsHdr-1;
    for (long r=0; r<Length(); r++)
      {
      L++;
      char tmp[5];
      sprintf(tmp, "%i", r);
      SetDesc(L, tmp, -1 , 4, 2, "");
      SetSpace(L,1);
      SetParm(L,"", Id_Xf, 12, 2, "");
      SetParm(L,"", Id_Yf, 12, 2, "");
      SetParm(L,"", Id_Zf, 12, 2, "");
      }
  	}
  }

//---------------------------------------------------------------------------

void C3DFn::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Name:       Str=pM->ShortDesc();  break;
      }
    }

  pM->SetView(pView);
  pM->Load(EI, Str);

  if (CurrentBlk(EI))
    {
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_PtsFName:  
        if (PtsFName.Length()>0)
          Str=PtsFName();
        else
          Str="";
        break;
      case Id_XDivs:     Str.Set("%i", iXDivs); break;
      case Id_YDivs:     Str.Set("%i", iYDivs); break;
      case Id_ContourInt: Str.Set("%i", iContourIntervals); break;
      case Id_XfMn:      Str.Set("%g", dXDispMin); break;
      case Id_YfMn:      Str.Set("%g", dYDispMin); break;
      case Id_ZfMn:      Str.Set("%g", dZDispMin); break;
      case Id_XfMx:      Str.Set("%g", dXDispMax); break;
      case Id_YfMx:      Str.Set("%g", dYDispMax); break;
      case Id_ZfMx:      Str.Set("%g", dZDispMax); break;
      case Id_XfLo:      Str.Set("%g", dXLoLim); break;
      case Id_YfLo:      Str.Set("%g", dYLoLim); break;
      case Id_ZfLo:      Str.Set("%g", dZLoLim); break;
      case Id_XfUp:      Str.Set("%g", dXUpLim); break;
      case Id_YfUp:      Str.Set("%g", dYUpLim); break;
      case Id_ZfUp:      Str.Set("%g", dZUpLim); break;
      case Id_XfGR:      Str.Set("%g", dXGRatio); break;
      case Id_YfGR:      Str.Set("%g", dYGRatio); break;
      case Id_ZfGR:      Str.Set("%g", dZGRatio); break;
      case Id_Xf:        Str.Set("%g", X[i]); break;
      case Id_Yf:        Str.Set("%g", Y[i]); break;
      case Id_Zf:        Str.Set("%g", Z[i]); break;
      case Id_Len:       Str.Set("%i", Length()); break;
      case Id_RqdLen:    Str.Set("%i", iRqdLength); break;
      case Id_NoElimIter:Str.Set("%i", iNoElimIter); break;
      }
    }
  }

//---------------------------------------------------------------------------

long C3DFn::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {
    bObjModified=1;
    //int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Name:    
        ChangeModel(Str());
        View().DoRebuild();
        Fix=1;
        break;
      }
    }

  pM->SetView(pView);
  Fix = Max(pM->Parse(EI, Str), Fix);
  bObjModified |= pM->bObjModified;

  if (CurrentBlk(EI))
    {
    bObjModified = 1;
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_PtsFName:
        if (Str.Length()!=PtsFName.Length() && (PtsFName.Length()==0 || Str.Length()==0))
          View().DoRebuild();
        PtsFName = Str;
        break;
      case Id_XDivs:
        iXDivs = Max(2L, Str.SafeAtoL());
        Fix=1;
        break;
      case Id_YDivs:
        iYDivs = Max(2L, Str.SafeAtoL());
        Fix=1;
        break;
      case Id_ContourInt:
        iContourIntervals = Min(MaxContourIntervals, Max(4L, Str.SafeAtoL()));
        Fix=1;
        break;
      case Id_Len: 
        SetLength(Range(2L, Str.SafeAtoL(), (long)Max3DFnPts));
        View().DoRebuild();
        break;
      case Id_RqdLen:
        iRqdLength = Min(Max(Str.SafeAtoL(),2L),(long)Max3DFnPts);
        if (bRedist)
          {
          DoRedistribute();
          View().DoRebuild();
          Fix=1; 
          }
        break;
      case Id_NoElimIter:
        iNoElimIter = Max(3L, Str.SafeAtoL());
        break;
      case Id_XfMn:    
        dXDispMin = max(Str.SafeAtoF(), dXLoLim);
        Fix=1; 
        break;
      case Id_YfMn:    
        dYDispMin = max(Str.SafeAtoF(), dYLoLim);
        Fix=1; 
        break;
      case Id_ZfMn:
        dZDispMin = max(Str.SafeAtoF(), dZLoLim);
        Fix=1; 
        break;
      case Id_XfMx:    
        dXDispMax = min(Str.SafeAtoF(), dXUpLim);
        Fix=1; 
        break;
      case Id_YfMx:    
        dYDispMax = min(Str.SafeAtoF(), dYUpLim);
        Fix=1; 
        break;
      case Id_ZfMx:    
        dZDispMax = min(Str.SafeAtoF(), dZUpLim);
        Fix=1; 
        break;
      case Id_XfLo:    
        dXLoLim = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_YfLo:    
        dYLoLim = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_ZfLo:    
        dZLoLim = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_XfUp:    
        dXUpLim = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_YfUp:    
        dYUpLim = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_ZfUp:    
        dZUpLim = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_XfGR:    
        dXGRatio = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_YfGR:    
        dYGRatio = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_ZfGR:    
        dZGRatio = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_Xf: 
        {
        SetLength(Max((long)(i+1), Length()));
        double d = X[i];
        X[i] = Str.SafeAtoF();
        if (d!=X[i])
          bIsBuilt = 0;
        Fix=1;
        break;
        }
      case Id_Yf: 
        {
        SetLength(Max((long)(i+1), Length()));
        double d = Y[i];
        Y[i] = Str.SafeAtoF();
        if (d!=Y[i])
          bIsBuilt = 0;
        Fix=1;
        break;
        }
      case Id_Zf:
        {
        SetLength(Max((long)(i+1), Length()));
        double d = Z[i];
        Z[i] = Str.SafeAtoF();
        if (d!=Y[i])
          bIsBuilt = 0;
        Fix=1;
        break;
        }
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long C3DFn::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_SaveBtn:
        View().DoRebuild();
        break;
      case Id_CancelBtn:
        View().DoRebuild();
        break;
      }
    }

  pM->SetView(pView);
  pM->ButtonPushed(EI, Str);
      
  flag bDoRefit = True;
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_SortXBtn:
        SortByX();
        View().DoReload();
        break;
      case Id_SortYBtn:
        SortByY();
        View().DoReload();
        break;
      case Id_RefitBtn:
        bIsBuilt = 0;
        Fix=1;
        View().DoReload();
        break;
      case Id_ShowBtn:
        if (bShowData)
          bShowData = 0;
        else
          bShowData = 1;
        View().DoRebuild();
        break;
      case Id_ZMnMxBtn:
        {
        double CrsDist=0;
        dZDispMin = DBL_MAX;
        dZDispMax = DBL_MAX*-1.0;
        double XMin = GetNormX(dXDispMin);
        double XMax = GetNormX(dXDispMax);
        double YMin = GetNormY(dYDispMin);
        double YMax = GetNormY(dYDispMax);
        double dX = XMax - XMin;
        double dY = YMax - YMin;
        int CrvPts = C3DFnEdt::CrvPts;
        CDVector Fst(2);
        CDVector Snd(2);
        long TmpCount =0;
        double TmpDist = 0.0;
        double TtlDist = 0.0;
        for (long j=0; j<C3DFnEdt::CrvPts; j++)
          {
          double X1=GetCrsSection(0,j);
          double X2=GetCrsSection(0,j+1);
          double Y1=GetCrsSection(1,j);
          double Y2=GetCrsSection(1,j+1);
          CrsDist += (sqrt(((X1-XMin)/dX-(X2-XMin)/dX)*
                          ((X1-XMin)/dX-(X2-XMin)/dX)
                         +((Y1-YMin)/dY-(Y2-YMin)/dY)*
                          ((Y1-YMin)/dY-(Y2-YMin)/dY)));
          }
        Fst[0] = GetCrsSection(0,TmpCount);
        Fst[1] = GetCrsSection(1,TmpCount);
        Snd[0] = GetCrsSection(0,(TmpCount+1));
        Snd[1] = GetCrsSection(1,(TmpCount+1));
        double DistPts = sqrt(((Fst[0]-XMin)/dX-(Snd[0]-XMin)/dX)*
                              ((Fst[0]-XMin)/dX-(Snd[0]-XMin)/dX)
                             +((Fst[1]-YMin)/dY-(Snd[1]-YMin)/dY)*
                              ((Fst[1]-YMin)/dY-(Snd[1]-YMin)/dY));
        DistPts=GTZ(DistPts);
        double Dx = CrsDist/CrvPts;

        double LastX0 = (TmpDist*(Snd[0]-Fst[0])/DistPts)+Fst[0];
        double LastY0 = (TmpDist*(Snd[1]-Fst[1])/DistPts)+Fst[1];
        for (j=0; j<CrvPts; j++)
          {
          TmpDist += Dx;
          TtlDist += Dx;
          if ((TmpDist>(DistPts+Dx)) && (TmpCount<(GetNoCrsPts()-2)))
            {                                  
            TmpCount++;
            Fst[0]=GetCrsSection(0,TmpCount);
            Fst[1]=GetCrsSection(1,TmpCount);
            Snd[0]=GetCrsSection(0,TmpCount+1);
            Snd[1]=GetCrsSection(1,TmpCount+1);
            DistPts = sqrt(((Fst[0]-XMin)/dX-(Snd[0]-XMin)/dX)*
                           ((Fst[0]-XMin)/dX-(Snd[0]-XMin)/dX)
                          +((Fst[1]-YMin)/dY-(Snd[1]-YMin)/dY)*
                           ((Fst[1]-YMin)/dY-(Snd[1]-YMin)/dY));
            TmpDist=0.0;
            }
          if (fabs(DistPts)>1.0e-20)
            {
            double X0 = (TmpDist*(Snd[0]-Fst[0])/DistPts)+Fst[0];
            double Y0 = (TmpDist*(Snd[1]-Fst[1])/DistPts)+Fst[1];
            double Zv = Zxy(X0, Y0);
            GetActualZ(Zv);
            dZDispMax = Max(dZDispMax, Zv);
            dZDispMin = Min(dZDispMin, Zv);
            }
          }
        View().DoReload();
        break;        
        }
      case Id_InitParmsBtn:
        {
        //InitParms(); 
        ASSERT(False);//todo
        View().DoReload();
        break;
        }
      case Id_ZToCurveBtn:
        {
        for (long j=0; j<Z.GetLen(); j++)
          Z[j] = Zxy(X[j], Y[j]);
        Fix=1;
        bIsBuilt = 0;
        bDoRefit = False;
        View().DoReload();
        break;
        }
      case Id_LoadPtsBtn:
        {
        if (PtsFName.Length()>0)
          {
          Strng ss;
          bIsBuilt = 0;
//          if (PtsFName.StrRChr('.'))
//            PtsFName.SetLength(PtsFName.StrRChr('.')-PtsFName.Buffer());
//          PtsFName += ".csv";
          PtsFName.FnCheckExtension("csv");
//          PtsFName = AdaptFilename2(ss, PtsFName());
          PtsFName.FnSearchContract();
          ss.FnExpand(PtsFName());

//          ss.FnExpand();
          //PtsFName.Lower();
          LoadPts(ss());
          }
        Fix=1;
        View().DoRebuild();
        break;
        }
      case Id_SavePtsBtn:
        {
        if (PtsFName.Length()>0)
          SavePts(PtsFName());
        break;
        }
      }
    }
  if (bDoRefit && Fix)
    ReFit();
  return Fix;
  }

// -------------------------------------------------------------------------

flag C3DFn::LoadPts(char* FName, CCnvIndex XCnvDC, CCnvIndex YCnvDC, CCnvIndex ZCnvDC)
  {
  CVMLoadHelper H;
  H.SetFindName();
  H.SetFindCnv();
  H.StartRow = 1;
  strcpy(H.FName, FName);
  flag b = X.Load(H, X.iCnvDC>DC_ ? X.iCnvDC : XCnvDC);
  if (b && H.GotName())
    sXName=H.VectorName();
  if (b)
    {
    H.Rows = X.GetLen();
    H.Cols = 1;
    H.StartCol = 2;
    b = Y.Load(H, Y.iCnvDC>DC_ ? Y.iCnvDC : YCnvDC);
    if (b && H.GotName())
      sYName=H.VectorName();
    if (b)
      {
      H.Rows = X.GetLen();
      H.Cols = 1;
      H.StartCol = 3;
      b = Z.Load(H, Z.iCnvDC>DC_ ? Z.iCnvDC : ZCnvDC);
      if (b && H.GotName())
        sZName=H.VectorName();
      }
	  StructureChanged(this);
    }
  Y.SetSize(X.GetLen());
  Z.SetSize(X.GetLen());
  return b;
  }

// -------------------------------------------------------------------------

flag C3DFn::SavePts(char* FName, char * XLabel, char * YLabel, char * ZLabel)
  {
  FILE* f = fopen(FName, "wt");
  if (f)
    {
    char Buff[512];
    sprintf(Buff, "%s,%s,%s\n", XLabel ? XLabel : sXName(), 
                                YLabel ? YLabel : sYName(), 
                                ZLabel ? ZLabel : sZName());
    fwrite(Buff, sizeof(char), strlen(Buff), f);
    
    if (X.sCnvTxt.Length()>0 || 
        Y.sCnvTxt.Length()>0 || 
        Z.sCnvTxt.Length()>0)
      {
      sprintf(Buff, "%s,%s,%s\n", X.sCnvTxt(), Y.sCnvTxt(), Z.sCnvTxt());
      fwrite(Buff, sizeof(char), strlen(Buff), f);
      }

    for (int i=0; i<Y.GetLen(); i++)
      {
      sprintf(Buff, "%g,%g,%g\n", X.Human(i), Y.Human(i), Z.Human(i));
      fwrite(Buff, sizeof(char), strlen(Buff), f);
      }
    fclose(f);
    return True;
    }
  return False;
  }

// -------------------------------------------------------------------------


//==========================================================================
// -------------------------------------------------------------------------

