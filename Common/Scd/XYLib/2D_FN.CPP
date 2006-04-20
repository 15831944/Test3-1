//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>
#define __2D_FN_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_fnedt.h"

//#include "optoff.h"

// =========================================================================

const pchar C2DModel::GroupName="2DModel";

// -------------------------------------------------------------------------

pC2DModel C2DModel::Create(pC2DFn pFn_, pchar Name_)
  { 
  pC2DModel rp=(pC2DModel)TagObjClass::Construct(C2DModel::GroupName, Name_, NULL, pFn_->Tag(), pFn_, TOA_Embedded);
  if (rp==NULL)
    rp=(pC2DModel)TagObjClass::Construct(C2DModel::GroupName, "2D_Data", NULL, pFn_->Tag(), pFn_, TOA_Embedded);
  
  rp->AttachMeToObj(pFn_, TOA_Embedded);
  rp->pFn = pFn_;
  return rp;
  }

// -------------------------------------------------------------------------

C2DModel::C2DModel(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CBaseMdl(pClass_, pTag, pAttach, eAttach)
  {
  pFn = NULL;
  }

// -------------------------------------------------------------------------

C2DModel::~C2DModel()
  {
  }

// -------------------------------------------------------------------------

flag C2DModel::ReFit()
  {
  if (NPts()>=NParms())
    {
    CDVector DP;
    if (!pFn->LBEst(DP, dSa, iIter, iMaxIter))
      iIter = -1;   //singularity
    }
  else
    iIter = -2;  //insufficient points
  return True;
  }

// =========================================================================

IMPLEMENT_TAGOBJEDT(C2DFn, "2DFn", "2DFn", "", "", TOC_SYSTEM, C2DFnEdt, "", ""); 
//IMPLEMENT_SPARES(C2DFn, 100); 

C2DFn::C2DFn(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FxdEdtBookRef(NULL),
  TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  pM = C2DModel::Create(this, "2D_Data");
  bReDoFrac = True;
  pYFrac=NULL;
  pCollectDataFn=NULL;
  pCollectDataPtr=NULL;
  Clear();
  }

// -------------------------------------------------------------------------

C2DFn::C2DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FxdEdtBookRef(NULL),
  TaggedObject(&C2DFnClass, Tag_, pAttach, eAttach)
  {
  pM = C2DModel::Create(this, pModelId);
  bReDoFrac = True;
  pYFrac=NULL;
  pCollectDataFn=NULL;
  pCollectDataPtr=NULL;
  Clear();
  }

// -------------------------------------------------------------------------

C2DFn::C2DFn(pchar pModelId, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, pchar pXName, pchar pYName) :
  FxdEdtBookRef(NULL),
  TaggedObject(&C2DFnClass, Tag_, pAttach, eAttach)
  {
  pM = C2DModel::Create(this, pModelId);
  bReDoFrac = True;
  pYFrac=NULL;
  pCollectDataFn=NULL;
  pCollectDataPtr=NULL;
  Clear();
  SetAxisNames(pXName, pYName);
  }

// -------------------------------------------------------------------------

C2DFn::C2DFn(C2DFn& Fn) :
  FxdEdtBookRef(NULL),
  TaggedObject(Fn.m_pClass, "", Fn.pAttachedTo, Fn.eAttachment)
  {
  bReDoFrac = True;
  pYFrac=NULL;
  pCollectDataFn=NULL;
  pCollectDataPtr=NULL;
  Strng s = Fn.Tag();
  TaggedObject::Tag(s());
  CopyFunction(Fn);
  pM = C2DModel::Create(this, Fn.pM->ClassId());
  }

// -------------------------------------------------------------------------
 
C2DFn::~C2DFn()
  {
  if (pYFrac)
    delete pYFrac;
  pM->Destroy();
  pM = NULL;
  }
 
// -------------------------------------------------------------------------
 
void C2DFn::Clear()
  {
  PtsFName = "";
  sXName = "";
  sYName = "";
  X.SetSize(0);
  Y.SetSize(0);
  dXDispMin = 0.0; 
  dXDispMax = 1.0;
  dYDispMin = 0.0;
  dYDispMax = 1.0;
  dXLoExt = 0.0;//DBL_MAX*-1.0;
  dXUpExt = 1.0;//DBL_MAX;
  dXLoLim = 0.0;//DBL_MAX*-1.0;
  dXUpLim = 1.0;//DBL_MAX;
  dYLoLim = 0.0;//DBL_MAX*-1.0;
  dYUpLim = 1.0;//DBL_MAX;
  dXGRatio = 1.0;
  dYGRatio = 1.0;
  iRqdLength = 2;
  iNoElimIter = 3;
  iGraphWidth = 38;
  iGraphHeight = 18;
  bShowEqn = False;
  bShowOther = False;
  bShowData = False;
  bXLog = False;
  bYLog = False;
  bYCumulative = False;
  StructureChanged(this);

  bAllowCumulative = False;
  bAllowRedist = False;
  bAllowYtoCurve = False;
  bAllowRefit = False;

  bRedist = False;
  bAutoScale = True;
  bReDoFrac = True;
  pM->Clear();
  }

// -------------------------------------------------------------------------

void C2DFn::CopyFunction(C2DFn& Fn)
  {
  if (&Fn == this)
    return;
  X = Fn.X;
  Y = Fn.Y;
  dXDispMin = Fn.dXDispMin;
  dXDispMax = Fn.dXDispMax;
  dYDispMin = Fn.dYDispMin;
  dYDispMax = Fn.dYDispMax;
  dXGRatio = Max(Fn.dXGRatio, MinLogVal);
  dYGRatio = Max(Fn.dYGRatio, MinLogVal);
  dXLoExt  = Fn.dXLoExt;
  dXUpExt  = Fn.dXUpExt;
  dXLoLim  = Fn.dXLoLim;
  dXUpLim  = Fn.dXUpLim;
  dYLoLim  = Fn.dYLoLim;
  dYUpLim  = Fn.dYUpLim;
  PtsFName  = Fn.PtsFName;
  sXName = Fn.sXName;
  sYName = Fn.sYName;
  iNoElimIter = Fn.iNoElimIter;
  iRqdLength = Fn.iRqdLength;
  iGraphWidth = Range(10L, Fn.iGraphWidth, 60L);
  iGraphHeight = Range(5L, Fn.iGraphHeight, 30L);
  bShowEqn = Fn.bShowEqn;
  bShowOther = Fn.bShowOther;
  bShowData  = Fn.bShowData;
  bXLog = Fn.bXLog;
  bYLog = Fn.bYLog;
  bYCumulative = Fn.bYCumulative;
  bRedist = Fn.bRedist;
  bAutoScale = Fn.bAutoScale;
  bReDoFrac = True;
  if (_stricmp(Fn.ModelName(), ModelName())!=0)
    {
    pM->Destroy();
    pM = C2DModel::Create(this, Fn.ModelName());
    }
  pM->CopyModel(Fn.pM);
  }

// -------------------------------------------------------------------------
/*
void C2DFn::Get2DModelDesc(pTagObjClass pC, rStrng S)
  {
  S = pC->ShortDesc();
  S.SetLength(Min(20, S.Length()));
  S.Trim();
  }
*/
// -------------------------------------------------------------------------

void C2DFn::SetLength(long n)
  {
  if (Y.GetLen() != n)
    {
    bReDoFrac = True;
    if (n != 0)
      {
      n = Min(Max2DFnPts, Max(2L,n));
      Y.SetSize(n);
      X.SetSize(n);
      StructureChanged(this);
      }
    }
  }

// -------------------------------------------------------------------------

/*double C2DFn::GeometricRatio(double *ReqdRatio)
  { 
  if (ReqdRatio!=NULL)
    Distribute(this, Length(), ScanXMin(), ScanXMax(), *ReqdRatio);
  return XGRatio[0];
  }*/

// -------------------------------------------------------------------------

pchar C2DFn::ModelName(pchar pReqdModel) 
  {
  if (pReqdModel)
    ChangeModel(FindModelByClassId(pReqdModel));
  return pM->ClassId();
  }

// -------------------------------------------------------------------------

pchar C2DFn::FindModelByClassId(pchar pReqdModelClassId)
  {
  if (pReqdModelClassId==NULL)
    return NULL;
  pchar pReqdModel=NULL;
  Strng S;
  for (int i=0; ;i++)
    {
    pTagObjClass pC = TagObjClass::FindClassIndexed(C2DModel::GroupName, i);
    if (pC==NULL)
      break;
    else 
      {
      if (_stricmp(pC->ClassId(), pReqdModelClassId)==0)
        {
        pReqdModel = pC->ClassId();
        break;
        }
      }
    }
  return pReqdModel;
  }

// -------------------------------------------------------------------------

pchar C2DFn::FindModelByDesc(pchar pReqdModelDesc) 
  { 
  if (pReqdModelDesc==NULL)
    return NULL;
  pchar pReqdModel=NULL;
  Strng S;
  for (int i=0; ;i++)
    {
    pTagObjClass pC = TagObjClass::FindClassIndexed(C2DModel::GroupName, i);
    if (pC==NULL)
      break;
    else 
      {
      pC->GetShortDesc(S);
      if (S.XStrICmp(pReqdModelDesc)==0)
        {
        pReqdModel = pC->ClassId();
        break;
        }
      }
    }
  return pReqdModel;
  }

// -------------------------------------------------------------------------

flag C2DFn::ChangeModel(pchar pReqdModelClassId)
  {
  if (pReqdModelClassId)
    {
    StructureChanged(this);
    pC2DModel p = C2DModel::Create(this, pReqdModelClassId);
    if (p==NULL)
      return FALSE;

    pM->Destroy();
    pM = p;
    }
  return TRUE; 
  }

// -------------------------------------------------------------------------

flag C2DFn::ReFit() 
  {
  flag r = pM->ReFit();
  
  if (dXUpLim - dXLoLim<=1.0e-6)
    dXUpLim = dXLoLim + 0.1;
  if (dYUpLim - dYLoLim<=1.0e-6)
    dYUpLim = dYLoLim + 0.1;

  // is this the correct place for these checks ??? efficiency ?
  long N = Y.GetLen();
  for (long i=0; i<N; i++)
    if (!Valid(Y[i])) 
      AfxDebugBreak();
  if (N>=2)
    {
    double Xr = Max(ZeroLimit, X[N-1] - X[0]); 
    for (i=1; i<N; i++)
    if (X[i] - X[i-1] < 1.0e-6 * Xr) 
      X[i] = X[i-1] + 1.0e-6 * Xr; 
    }
  bReDoFrac = True;
  return r;
  }

// -------------------------------------------------------------------------

double C2DFn::GetNormX(double x)
  {
  return (x - dXLoLim) / NZ(dXUpLim - dXLoLim);
  }

// -------------------------------------------------------------------------

void C2DFn::GetNormX(CDVector &Xs)
  {
  for (long i=0; i<Xs.GetLen(); i++)
    Xs[i] = (Xs[i] - dXLoLim) / NZ(dXUpLim - dXLoLim);
  }

// -------------------------------------------------------------------------

double C2DFn::GetActualX(double x)
  {
  return x * (dXUpLim - dXLoLim) + dXLoLim;
  }

// -------------------------------------------------------------------------

void C2DFn::GetActualX(CDVector &Xs)
  {
  for (long i=0; i<Xs.GetLen(); i++)
    Xs[i] = Xs[i] * (dXUpLim - dXLoLim) + dXLoLim;
  }

// -------------------------------------------------------------------------

double C2DFn::GetNormY(double y)
  {
  return (y - dYLoLim) / NZ(dYUpLim - dYLoLim);
  }

// -------------------------------------------------------------------------

void C2DFn::GetNormY(CDVector &Ys)
  {
  for (long i=0; i<Ys.GetLen(); i++)
    Ys[i] = (Ys[i] - dYLoLim) / NZ(dYUpLim - dYLoLim);
  }

// -------------------------------------------------------------------------

double C2DFn::GetActualY(double y)
  {
  return y * (dYUpLim - dYLoLim) + dYLoLim;
  }

// -------------------------------------------------------------------------

void C2DFn::GetActualY(CDVector &Ys)
  {
  for (long i=0; i<Ys.GetLen(); i++)
    Ys[i] = Ys[i] * (dYUpLim - dYLoLim) + dYLoLim;
  }
        
// -------------------------------------------------------------------------

void C2DFn::MovePt(long i, double x, double y)
  { 
  if (Y.GetLen()==1)
    X[i] = x; 
  else if (i==0)
    X[i] = Min(X[i+1]-1.0e-6, x); 
  else if (i==Y.GetLen()-1)
    X[i] = Max(X[i-1]+1.0e-6, x); 
  else
    X[i] = Range(X[i-1]+1.0e-6, x, X[i+1]-1.0e-6); 
  Y[i] = y;
  bReDoFrac = True;
  }

// -------------------------------------------------------------------------

void C2DFn::Sort(flag Ascending)
  {
  if (Y.GetLen()>1)
    {
    CDMatrix Temp(2,Y.GetLen());
    Temp.SetRow(0,X);
    Temp.SetRow(1,Y);
    Temp.SortRows(0, Ascending);
    Temp.GetRow(0,X);
    Temp.GetRow(1,Y);
    bReDoFrac = True;
    }
  }

// -------------------------------------------------------------------------

flag C2DFn::GeoDistribute(long NReqd, double GeomRatio_)
  {//cnm code...
  dXGRatio=Max(GeomRatio_, MinLogVal);
  flag HasYs=(Length()>0);
  NReqd = Max(NReqd,2L);

  double nX[Max2DFnPts], nY[Max2DFnPts];

  double LoX = dXLoLim;
  double HiX = dXUpLim;
  if (GeomRatio_>0.999 && GeomRatio_<1.001)
    {
    for (long i=0; i<NReqd; i++)
      nX[i] = LoX+((HiX-LoX)*i)/(NReqd-1);
    GeomRatio_ = 1.0;
    }
  else
    {
    // see Maths Hand Book pg 107
    double a = (HiX-LoX)*(1.0-GeomRatio_)/(1.0-Pow(GeomRatio_,(NReqd-1)));
    double x = LoX;
    for (long i=0; i<NReqd; i++)
      {
      nX[i] = x;
      x += a;
      a *= GeomRatio_;
      }
    }
  if (HasYs)
    {
    for (long i=0; i<NReqd; i++)
      nY[i] = Yx(nX[i]);
    }
  else
    {
    for (long i=0; i<NReqd; i++)
      nY[i] = 0.0;
    }
  // Now Change the Length
  NReqd = Length(&NReqd);
  for (long i=0; i<NReqd; i++)
    SetPt(i, nX[i], nY[i]);
  dXGRatio=GeomRatio_;
  bReDoFrac = True;
  return True;
  }

// -------------------------------------------------------------------------

flag C2DFn::DoRedistribute()
  {
  flag b = GeoDistribute(iRqdLength, dXGRatio);
  /*
  flag b = False;
  if (Y.GetLen()<=iRqdLength)
    b = CreatePts(iRqdLength);
  else
    b = EliminatePts(iRqdLength, iNoElimIter);
  bReDoFrac = True;
  */
  return b;
  }

// -------------------------------------------------------------------------
/* bga
   This method will eliminate superfulous points from a XY function.
   It drops the points that have the least effect on the gradient and area under the curve.
   lNoPtsRqd is the number of x points required
*/
// -------------------------------------------------------------------------

flag C2DFn::EliminatePts(long lNoPtsRqd, long NoIterations)
  {
  long NoPts = Y.GetLen();
  if (NoPts<=lNoPtsRqd) 
    return False;//There are no points or there are already less points than required
  if (lNoPtsRqd<=2)
    return False;//you can't have less than two points 
  if (NoIterations<1)
    return False;//You cant get rid of points with so few iterations
  
  CDMatrix Weight(NoPts, 2); //sets up a matrix which represents the effect each point has on
                             //the graph coloumn 1 is the index and coloumn 2 the weight

  GetNormX(X); //normalise x points
  GetNormY(Y); //normalise y points
  long RefNoPtsRqd = lNoPtsRqd;
  long DeltaPts = Min(1L, (long)ceil(double((NoPts-lNoPtsRqd)/NoIterations)));
  long PtsRqd = NoPts;
  while (PtsRqd>RefNoPtsRqd)
    {
    PtsRqd -= DeltaPts;
    if (PtsRqd<RefNoPtsRqd)
      PtsRqd=RefNoPtsRqd;
    Sort();
    long N = X.GetLen();
    for (long i=0; i<(N-2); i++)
      {
  	  double x1 = (Xp(i));
  	  double x2 = (Xp(i+1));
  	  double x3 = (Xp(i+2));
  	  double y1 = (Yp(i));
  	  double y2 = (Yp(i+1));
  	  double y3 = (Yp(i+2));
  	  Weight(i,0) = i;//set index
  	  double dX1 = x2-x1;
  	  double dX2 = x3-x2;
  	  double temp = ((y2-y1)/dX1-(y3-y2)/dX2)/(dX1+dX2)*((dX1*(y1+y2)+dX2*(y2+y3)));
  	    //temp = Change in gradients*effective area under point
  	  Weight(i,1)	= fabs(temp);//>=0?temp:(temp*(-1)));//set weight       
  	  }

    Weight.SortCols(1,FALSE);//sort weight by colum2 descending

    CDMatrix temp(PtsRqd,2);//create a temporary matrix to restore the required points
    temp(0,0) = Xp(0);//first points
    temp(0,1) = Yp(0);

    long ltmp = min(N,(PtsRqd-1));
    for (long j=1; j<ltmp; j++)
    	{					
    	temp(j,0) = Xp((long)Weight((j-1),0)+1);
      temp(j,1) = Yp((long)Weight((j-1),0)+1);
  	  }
 
    N--;
    temp((PtsRqd-1),0) = Xp(N);//last points
    temp((PtsRqd-1),1) = Yp(N);

    SetLength(PtsRqd);
    for (j=1; j<lNoPtsRqd; j++)//set points
    	SetPt(j, temp(j,0), temp(j,1));
    }//while
  GetActualX(X);//denormalize
  GetActualY(Y);
  Sort();//sort back into order
  View().DoRebuild();//length has changed
  bReDoFrac = True;
  return True;
  }

// -------------------------------------------------------------------------
/* bga
This function is for creating points in the most crucial sections of a XY function
*/
// -------------------------------------------------------------------------

flag C2DFn::CreatePts(long lNoPtsRqd)
  {
  if (lNoPtsRqd<3L) //useless trying to create with so few points
    return False;
  double xmax = GetNormX(dXUpLim);
  double xmin = GetNormX(dXLoLim);
  TRACE("Replace with an Inline Array\n");
  CDVector Points(lNoPtsRqd);
  CLVector Flags(lNoPtsRqd);
  CDVector TmpPts(lNoPtsRqd*10);
  CDVector ImportantPts(lNoPtsRqd*10);

  //Distribute equally...
  double dx = (xmax-xmin)/(lNoPtsRqd*10-1);
  for (long i=0; i<lNoPtsRqd*10; i++)
    TmpPts[i] = GetActualX(i*dx+xmin);
  
  //get local min, max and inflections...
  long ImpCnt = 0;
  double P0 = 0.0;
  double P1 = 0.0;
  double P2 = GetNormY(Yx(TmpPts[0]));
  double P3 = GetNormY(Yx(TmpPts[1]));
  double P4 = GetNormY(Yx(TmpPts[2]));
  double m1,m2,m3;

  for (i=1; i<(lNoPtsRqd*10-1); i++)
    {
    P0 = P1;
    P1 = P2;
    P2 = P3;
    P3 = P4;
    if (i<lNoPtsRqd*10-2)
      P4 = GetNormY(Yx(TmpPts[i+2]));
    if ((P2-P1>ZeroLimit && P2-P3>ZeroLimit) || (P1-P2>ZeroLimit && P3-P2>ZeroLimit))
      ImportantPts[ImpCnt++] = TmpPts[i];//local max or min
    else if (i>1 && i<(lNoPtsRqd*10-2))
      {
      m1 = (P2-P0);
      m2 = (P3-P1);
      m3 = (P4-P2);
      //if ((m2-m1>ZeroLimit && m2-m3>ZeroLimit) || (m1-m2>ZeroLimit && m3-m2>ZeroLimit))
      //if ( (m2>m1 && m2>m3) || (m2<m1 && m2<m3))
      if ( (m2-m1>1.0e6 && m2-m3>1.0e6) || (m2-m1<-1.0e6 && m2-m3<-1.0e6))
        ImportantPts[ImpCnt++] = TmpPts[i];//local point of inflection
      }
    }

  if (ImpCnt>lNoPtsRqd) 
    {            
    SetLength(ImpCnt);
    for (i=0; i<ImpCnt; i++)
      SetPt(i, ImportantPts[i], Yx(ImportantPts[i]));
    Sort();
    return EliminatePts(lNoPtsRqd, iNoElimIter);
    }

  //create even distribution
  dx = (xmax-xmin)/(lNoPtsRqd-1);
  for (i=0;i<lNoPtsRqd;i++)
    Points[i] = GetActualX(i*dx+xmin);
  
  //insert all the important points over the closest point...
  long count1 = 0;
  long count2 = 1;
  Flags.SetAll(0);
  Flags[0] = 1;//store edge point
  Flags[lNoPtsRqd-1] = 1;//store edge point
  while (count1<ImpCnt)
    {
    if (ImportantPts[count1]<Points[count2])
      {
      if ( ( (ImportantPts[count1] - Points[count2-1]) < 
             (Points[count2] - ImportantPts[count1]) ) &&
           (Flags[count2-1]==0))
        {
        Points[count2-1] = ImportantPts[count1];
        Flags[count2-1] = 1;
        }
      else
        {
        Points[count2] = ImportantPts[count1];
        Flags[count2] = 1;
        }
      count1++;
      }
    count2++;
    }
  
  //shift non-important points left and right to get a better distribution...
  count1 = 0;
  flag end = False;
  CLVector LastMove(2);
  LastMove[0] = 0;
  LastMove[1] = 0;
  TmpPts.SetSize(lNoPtsRqd);
  CDMatrix TempSort(2,lNoPtsRqd-2);
  while ((count1<iNoElimIter) && (!end))
    {
    //calculate a weight for each point...
    P1 = 0.0;
    P2 = Yx(Points[0]);
    P3 = Yx(Points[1]);
    m1 = 0.0;
    m2 = (P2-P3)/(Points[0]-Points[1]);
    for (i=1; i<(lNoPtsRqd-1); i++)
      {
      P1 = P2;
      P2 = P3;
      P3 = Yx(Points[i+1]);
      m1 = m2;
      m2 = (P2-P3)/(Points[i]-Points[i+1]);
      TempSort[0][i-1] = i;
      TempSort[1][i-1] = fabs((m1-m2)); //weight = change in gradient arround the point i
      }
    TempSort.SortRows(1);//sort by weight
    
    long line = 0;
    count2 = 0;
    while ((count2<2) && (line<TempSort.GetRows()))//move most useless points near most usefull
      {
      if (Flags[(long)TempSort[0][line]]==0)
        {
        if ((TempSort[0][line]!=LastMove[0]) && (TempSort[0][line]!=LastMove[1]))
          {
          long k = (long)TempSort[0][lNoPtsRqd-count2-3];
          if (TempSort[0][line]<k) 
            TmpPts[(long)TempSort[0][line]] = (Points[k+1]-Points[k])/2+Points[k];
          else
            TmpPts[(long)TempSort[0][line]] = (Points[k]-Points[k-1])/2+Points[k-1];
          LastMove[count2] = (long)TempSort[0][line];
          count2++;
          }
        else
          end = True;//moving a point back
        }
      else  
        TmpPts[(long)TempSort[0][line]] = Points[(long)TempSort[0][line]];
      line++;
      }  
    for (i=line; i<(lNoPtsRqd-2); i++)
      TmpPts[(long)TempSort[0][i]] = Points[(long)TempSort[0][i]];
        
    if (!end)
      {
      for (i=1; i<lNoPtsRqd-1; i++)
        Points[i] = TmpPts[i];
      }
    Points.Sort();
    TmpPts.SetAll(0.0);
    count1++;
    }//while   

  //Set all the points in the function...
  //Note: Must calculate all the points before setting them. If some points are set and then Yx is
  //      called, the function behaves differently because the points allready set may have 
  //      changed the properties of the function. (eg for a data only curve)
  for (i=0; i<lNoPtsRqd; i++)
    TmpPts[i] = Yx(Points[i]);
  SetLength(lNoPtsRqd);
  for (i=0; i<lNoPtsRqd; i++)
    SetPt(i, Points[i], TmpPts[i]);
  Sort();
  bReDoFrac = True;
  return True;
  }

// -------------------------------------------------------------------------

void C2DFn::SetXPts(double * RqdXs, int NXs)
  {
  //inserts all x points from RqdXs 
  double y[Max2DFnPts];
  
  for (long j=0; j<NXs; j++)
    y[j]=Yx(RqdXs[j]);

  SetLength(NXs);
  for (j=0; j<NXs; j++)
    {
    X[j] = RqdXs[j];
    Y[j] = y[j];
    }
  bReDoFrac = True;
  };

// -------------------------------------------------------------------------

void C2DFn::SetXPts(CDVector &RqdXs)
  {
  //inserts all x points from RqdXs 
  double y[Max2DFnPts];
  const long n = RqdXs.GetLen();
  
  for (long j=0; j<n; j++)
    y[j]=Yx(RqdXs[j]);

  SetLength(n);
  for (j=0; j<n; j++)
    {
    X[j] = RqdXs[j];
    Y[j] = y[j];
    }
  bReDoFrac = True;
  };

// -------------------------------------------------------------------------

flag C2DFn::SetXPts(C2DFn &Fn)
  {
  //inserts all x points from RqdXs 
  double y[Max2DFnPts];
  const long n = Fn.Y.GetLen();
  
  for (long j=0; j<n; j++)
    y[j]=Yx(Fn.X[j]);

  SetLength(n);
  for (j=0; j<n; j++)
    {
    X[j] = Fn.Y[j];
    Y[j] = y[j];
    }
  bReDoFrac = True;
  return True;
  };

// -------------------------------------------------------------------------
    
void C2DFn::CombineXs(C2DFn &Fn)
  {//inserts all x points from Fn that are not present
  double y[2*Max2DFnPts];
  double x[2*Max2DFnPts];
  const long n1 = Y.GetLen();
  const long n2 = Fn.Y.GetLen();
  double Range = Max(fabs(X[n1-1]), fabs(Fn.X[n2-1]));
  long i1=0, i2=0, i=0;
  while (i1<n1 || i2<n2)
    {
    if (i1<n1 && i2<n2)
      {
      double x1 = X[i1];
      double x2 = Fn.X[i2];
      if (fabs(x1-x2) <= (1.0e-5*Range +1.0e-12)) // Same Point - Merge
        {
        x[i]=(x1+x2)*0.5;
        //y[i]=3;
        ++i1;
        ++i2;
        }
      else if (x1<x2)
        {
        x[i]=x1;
        //y[i]=1;
        ++i1;
        }
      else // (x1<x2)
        {
        x[i]=x2;
        //y[i]=2;
        ++i2;
        }
      }
    else if (i1<n1)
      {
      x[i]=X[i1];
      //y[i]=1;
      ++i1;
      }
    else //(i2<n2)
      {
      x[i]=Fn.X[i2];
      //y[i]=2;
      ++i2;
      }
    y[i]=Yx(x[i]);
    i++;
    }

  SetLength(i);
  for (long j=0; j<i; j++)
    {
    X[j] = x[j];
    Y[j] = y[j];
    }
  bReDoFrac = True;
  //if (bRedist)
  //  DoRedistribute();
  }

// -------------------------------------------------------------------------

flag C2DFn::InsertXPts(C2DFn &Fn)
  {
  if (Y.GetLen()==0 && Fn.Y.GetLen()==0)
    return False;

  CombineXs(Fn);
  /** CNM
  const long N = Y.GetLen();
  for (long i=0; i<N; i++)
    Y[i] += Fn.Yx(X[i]);
  **/
  if (bRedist)
    DoRedistribute();
  ReFit();
  
  return True;
  }

// -------------------------------------------------------------------------

flag C2DFn::Add(double Offset)
  {
  if (Y.GetLen()==0)
    return False;

  Y.Add(Offset);

  //if (bRedist)
  //  DoRedistribute();
  ReFit();

  return True;
  }

// -------------------------------------------------------------------------

flag C2DFn::Add(C2DFn &Fn, double Scalar, double Offset)
  {
  long Ylen =   Y.GetLen();
  long FnYlen = Fn.Y.GetLen();                      // kcg crash
  if (Ylen==0 && FnYlen==0)
    return False;
  if (Ylen == 0)
    {
    SetLength(Fn.Y.GetLen());
    for (long i=0; i<FnYlen; i++)
      {
      Y[i] = Fn.Yp(i) * Scalar + Offset;
      X[i] = Fn.Xp(i);
      }
    }
  else
    {
    CombineXs(Fn);
    const long N = Y.GetLen();
    for (long i=0; i<N; i++)
      Y[i] = Yx(X[i]) + (Fn.Yx(X[i]) * Scalar + Offset);
    }
  
  /*
  if(Y.GetLen()==0 && Fn.Y.GetLen()==0)
    return False;

  CombineXs(Fn);
  const long N = Y.GetLen();
  for (long i=0; i<N; i++)
    Y[i] = Yx(X[i]) + (Fn.Yx(X[i]) * Scalar + Offset);
  */
  if (bRedist)
    DoRedistribute();
  ReFit();

  return True;
  }

// -------------------------------------------------------------------------

flag C2DFn::Multiply(double Scalar)
  {
  if (Y.GetLen()==0)
    return False;
  
  Y.Mult(Scalar);

  //if (bRedist)
  //  DoRedistribute();
  ReFit();

  return True;
  }

// -------------------------------------------------------------------------

flag C2DFn::Multiply(C2DFn& Fn, double Scalar, double Offset)
  {
  if (Y.GetLen()==0 && Fn.Y.GetLen()==0)
    return False;

  CombineXs(Fn);
  const long N = Y.GetLen();
  for (long i=0; i<N; i++)
    {
    double y1=Yx(X[i]);
    double y2=Fn.Yx(X[i]);
    Y[i] = Yx(X[i]) * (Fn.Yx(X[i]) * Scalar + Offset);
    }

  if (bRedist)
    DoRedistribute();
  ReFit();

  return True;
  }

// -------------------------------------------------------------------------

const double Extension=1.0e-9;

void C2DFn::ReCalcFrac()
  {
  if (Y.GetLen()==0)
    return;
  
  if (pYFrac==NULL)
    pYFrac=new CDVector;
  CDVector & YF=(*pYFrac);
  YF.SetSize(Y.GetLen());
  double P[Max2DFnPts];
  const long N = Y.GetLen();
  //double x0=Min(X[0]-Extension, dXLoExt);
  //double xn=Max(X[N-1]+Extension, dXUpExt);

  //P[0] = (Y[0]-0.0);//  /GTZ(X[0]-x0);
  P[0] = Y[0];//  /GTZ(X[0]-x0);
  double Sig=P[0];
  for (long i=1; i<N; i++)
    {
    P[i]=(Y[i]-Y[i-1]);//  /GTZ(X[i]-X[i-1]);
    Sig+=P[i];
    }
  //P[N]=GTZ(1.0-Y[N-1]);//  /GTZ(xn-X[N-1]);
  //Sig+=P[N];
  double Fact=GTZ(1.0/GTZ(Sig));

  YF[0] = Fact*P[0];
  for (i=1; i<N; i++)
    YF[i] = Fact*P[i];
  //YF[N]=Fact*P[N];

  bReDoFrac = False;
  }

// -------------------------------------------------------------------------

double C2DFn::ToFractional(double Area)
  {
  if (Y.GetLen()==0)
    return False;
  double P[Max2DFnPts];
  bReDoFrac=1;
  const long N = Y.GetLen();
  //double x0=Min(X[0]-Extension, dXLoExt);
  //double xn=Max(X[N-1]+Extension, dXUpExt);
  //P[0] = (Y[0]-0.0);//  /GTZ(X[0]-x0);
  double Sig=P[0]=Y[0];
  for (long i=1; i<N; i++)
    {
    P[i]=(Y[i]-Y[i-1]);//  /GTZ(X[i]-X[i-1]);
    Sig+=P[i];
    }
  //P[N]=GTZ(Area-Y[N-1]);//  /GTZ(xn-X[N-1]);
  //Sig+=P[N];

  /*
  P[0] = (Y[0]-0.0);//  /GTZ(X[0]-x0);
  double Sig=P[0];
  for (long i=1; i<N; i++)
    {
    P[i]=(Y[i]-Y[i-1]);//  /GTZ(X[i]-X[i-1]);
    Sig+=P[i];
    }
  P[N]=GTZ(Area-Y[N-1]);//  /GTZ(xn-X[N-1]);
  Sig+=P[N];
  */
  double Fact=GTZ(Area/GTZ(Sig));

  //SetLength(N+1);

  Y[0] = Fact*P[0];
  for (i=1; i<N; i++)
    Y[i] = Fact*P[i];
  //X[N]=xn;
  //Y[N]=Fact*P[N];
  
  if (bRedist)
    DoRedistribute();

  return Fact;
  }

//--------------------------------------------------------------------------

double C2DFn::ToCumulative(double ConstInteg, double Total)
  {
  if (Y.GetLen()==0)
    return False;
  const long N = Y.GetLen();
  for (long i=1; i<N; i++)
    Y[i] = Y[i-1] + Y[i];//*(X[i]-X[i-1]);
  double Yt = Y[N-1];
  /*
  double Yp[Max2DFnPts];
  Yp[0] = Y[0];
  Y[0] = (Yp[0]);//*(X[0]-Min(X[0]-Extension, dXLoLim));
  for (long i=1; i<N; i++)
    {
    Yp[i] = Y[i];
    Y[i] = Y[i-1] + Yp[i];//*(X[i]-X[i-1]);
    }
  //double Yt = Y[N-2] + Yp[N-1]*(X[N-1]-X[N-2]);
  double Yt = Y[N-1];
  */
  //SetLength(N-1);

  double Scale=Total/GTZ(Yt); 
  if (Scale >= 1.0e-10 && Scale <=1.0e10)
    Y.Mult(Scale); 

  if (bRedist)
    DoRedistribute();

  return Scale;
  }

// -------------------------------------------------------------------------

double C2DFn::SetCumulativeArea(double ConstInteg, double RqdArea)
  {
  if (Y.GetLen()==0)
    return 1.0;
  const long N = Y.GetLen();
  double Yt = ConstInteg;
  for (long i=1; i<N; i++)
    Yt+= 0.5*(Y[i-1] + Y[i]);//*(X[i]-X[i-1]);
  double Fact=Yt/GTZ(RqdArea);
  Y.Mult(Fact);
  return Fact;
  }

// -------------------------------------------------------------------------

double C2DFn::ScaleEndPoint(double RqdEndPt)
  {
  if (Y.GetLen()==0)
    return 1.0;

  const long N = Y.GetLen();
  double Fact=RqdEndPt/NZ(Y[N-1]); 
  Y.Mult(Fact); 
  return Fact;
  }

// -------------------------------------------------------------------------

flag C2DFn::Convolve(C2DFn &Fn, double Frac, double Offset)
  {
  if (Y.GetLen()==0 && Fn.Y.GetLen()==0)
    return False;

  CombineXs(Fn);
  const double Frac2 = 1.0 - Frac;
  const long N = Y.GetLen();
  for (long i=0; i<N; i++)
    {
    double y = Yx(X[i]);
    //Y[i] = y * Frac + Frac2 * y * Fn.Yx(X[i]);
    if (Offset > 1e-6)
      Y[i] = y * Frac + Frac2 * y * (Offset - Fn.Yx(X[i]));
    else
      Y[i] = y * Frac + Frac2 * y * Fn.Yx(X[i]);
    }

  if (bRedist)
    DoRedistribute();
  ReFit();

  return True;
  }

// -------------------------------------------------------------------------

flag C2DFn::GetYFrac(C2DFn &Fn)
  {
  SetLength(Fn.Length()+1);
  
  if (Length()==0)
    return False;

  const long N = Y.GetLen();
  for (long i=0; i<N; i++)
    {
    X[i]=Fn.XpFrac(i);
    Y[i]=Fn.YpFrac(i);
    }

  return True;
  }

// -------------------------------------------------------------------------

//double C2DFn::Integrate(double IntConst)
//  {
//  if (Y.GetLen()==0)
//    return IntConst;
//  
//  VERIFY(False); //this function needs to be checked (kga)
//
//  double Yt = IntConst; // Constant of Integration
//  double Xi = X[0];
//  double Yi = Y[0];
//  const long N = Y.GetLen();
//  for (long i=1; i<N; i++)
//    {
//    double LoX = X[i-1];
//    double HiX = X[i];
//    const long Ni = 1; //(Fit.Type==NoFit ? 1 : 15);
//    for (long j=0; j<Ni; j++)
//      {
//      double x = LoX + ((HiX - LoX) * (j + 1)) / Ni;
//      double y = Yx(x);
//      Yt += (x - Xi) * 0.5 * (Yi + y);
//      Yi = y;
//      Xi = x;
//      }
//    Y[i] = Yt;
//    }
//
//  if (bRedist)
//    DoRedistribute();
//  ReFit();
//  return Yt;
//  /*
//  pC2DFn pS;
//  flag IntOnly=(pSrc==NULL);
//  if (!IntOnly)
//    {
//    if (pSrc->NPts()==0)
//      return IntConst;
//    pS=(pSrc==this ? new C2DFn(*((pC2DFn)pSrc)): (pC2DFn)pSrc);
//    SetLength(pS->NPts());
//    }
//  else
//    pS=this;
//  if (pS->NPts()==0)
//    return IntConst;
//  ASSERT(X.GetRows() <= 1); //only meaningful for one independent X variable
//  
//  double Yt=IntConst; // Constant of Integration
//  double Xi=pS->Xp(0);
//  double Yi=pS->Yp(0);
//  if (!IntOnly)
//    SetPt(0,Xi,Yt);
//  long N = pS->NPts();
//  for (long i=1; i<N; i++)
//    {
//    double LoX = pS->Xp(i-1);
//    double HiX = pS->Xp(i);
//    long Ni=1; //(Fit.Type==NoFit ? 1 : 15);
//    for (long j=0; j<Ni; j++)
//      {
//      double X=LoX+((HiX-LoX)*(j+1))/(Ni);
//      double Y=pS->Yx(X);
//      Yt+=(X-Xi)*0.5*(Yi+Y);
//      Yi=Y;
//      Xi=X;
//      }
//  
//    if (!IntOnly)
//      SetPt(i,Xi,Yt);
//    }
//
//  if (bRedist)
//    DoRedistribute();
//  ReFit();
//  if (!IntOnly)
//    if (pSrc==this) delete pS;
//  return Yt;
//  */
//  }
//
//// -------------------------------------------------------------------------
//
//flag C2DFn::Differentiate()
//  {
//  if (Y.GetLen()==0)
//    return False;
//  
//  VERIFY(False); //this function needs to be checked (kga)
//
//  const long N = Y.GetLen();
//  const long Pts = N - 1;
//  for (long i=0; i<N; i++)
//    {
//    double Xi = X[i];
//    double dxl = (i>0 ? Xi - X[i-1] : X[i+1] - Xi) * 0.000001;
//    double dxh = (i<Pts ? X[i+1] - Xi : Xi - X[i-1]) * 0.000001;
//    Y[i] = (Yx(Xi+dxh) - Yx(Xi-dxl)) / GTZ(dxl+dxh);
//    }
//  double Temp = Y[Pts];
//  Y[Pts] = 0.0;
//  Y.Add(Temp * -1.0);
//  
//  if (bRedist)
//    DoRedistribute();
//  ReFit();
//
//  return True;
//
//  /*
//  double nX[MaxFnPts], nY[MaxFnPts], Temp;
//  pC2DFn pS=(pSrc==this ? new C2DFn(*((pC2DFn)pSrc)) : (pC2DFn)pSrc);
//  SetLength(pS->NPts());
//  nX[0] = pS->Xp(0);
//  nY[0] = 0.0;
//  long Pts = pS->NPts() - 1;
//  long N = Y.GetLen();
//  for (long i=0; i<=Pts; i++)
//    {
//    double dxl=(i>0 ? pS->Xp(i)-pS->Xp(i-1) : pS->Xp(i+1)-pS->Xp(i))*0.000001;
//    double dxh=(i<N-1 ? pS->Xp(i+1)-pS->Xp(i) : pS->Xp(i)-pS->Xp(i-1))*0.000001;
//    double Xi=pS->Xp(i);
//    nX[i] = pS->Xp(i);
//    nY[i] = (pS->Yx(Xi+dxh)-pS->Yx(Xi-dxl))/GTZ(dxl+dxh);
//    }
//  Temp = nY[Pts];
//  SetPt(0,nX[0],nY[0]);
////  SetPt(1,X[1],Temp,FALSE);
//  SetPt(Pts,nX[Pts],0.0);
////  Temp = Y[Pts] - 0.9 * Y[1];
//  for (i=0; i<=Pts; i++)
//    {
//    SetPt(i,nX[i],nY[i]-Temp);
//    }
//  
//  if (bRedist)
//    DoRedistribute();
//  ReFit();
//  if (pSrc==this) delete pS;
//  return True;*/
//  }
//
/// -------------------------------------------------------------------------

//double C2DFn::NormaliseCumulative(pC2DFn pFn, double IntConst)
//  {
//  if (Y.GetLen()==0)
//    return 1.0;
//  
//  ASSERT(pFn!=this); //does it make sense if it is ???
//  VERIFY(False); //this function needs to be checked (kga)
//
//  const long N = Y.GetLen();
//  if (pFn)
//    pFn->SetLength(N);
//  
//  double Yt = IntConst; // Constant of Integration
//  double Xi = X[0];
//  double Yi = Y[0];
//  if (pFn)
//    pFn->SetPt(0, Xi, Yt);
//  for (long i=1; i<N; i++)
//    {
//    double LoX = X[i-1];
//    double HiX = X[i];
//    const long Ni = 1; //(Fit.Type==NoFit ? 1 : 15);
//    for (long j=0; j<Ni; j++)
//      {
//      double x = LoX + ((HiX - LoX) * (j + 1)) / Ni;
//      double y = Yx(x);
//      Yt += (x - Xi) * 0.5 * (Yi + y);
//      Yi = y;
//      Xi = x;
//      }
//  
//    if (pFn)
//      pFn->SetPt(i, Xi, Yt);
//    }
//  
//  double NormFactor = 1.0 / GTZ(Yt);
//  Y.Mult(NormFactor);
//
//  if (pFn)
//    {
//    pFn->Y.Mult(NormFactor);
//    if (pFn->bRedist)
//      pFn->DoRedistribute();
//    pFn->ReFit();
//    }
//
//
//  if (bRedist)
//    DoRedistribute();
//  ReFit();
//
//  if (pFn)
//    {
//    pFn->Y.Mult(NormFactor);
//    pFn->ReFit();
//    }
//
//  return NormFactor;
//  
//  /*if (pSrc1->NPts()==0)// && pSrc2->NPts()==0)
//    return 1.0;
//  ASSERT(X.GetRows() <= 1); //only meaningful for one independent X variable
//
//  pC2DFn pS1=(pSrc1==this ? new C2DFn(*((pC2DFn)pSrc1)) : (pC2DFn)pSrc1);
//  pC2DFn pS2=(pC2DFn)pSrc2;
//
//  SetLength(pS1->NPts());
//  if (pS2)
//    pS2->SetLength(pS1->NPts());
//  
//  double Yt=IntConst; // Constant of Integration
//  double Xi=pS1->Xp(0);
//  double Yi=pS1->Yp(0);
//  if (pS2)
//    pS2->SetPt(0,Xi,Yt);
//  long N = Y.GetLen();
//  for (long i=1; i<N; i++)
//    {
//    double LoX=pS1->Xp(i-1);
//    double HiX=pS1->Xp(i);
//    long Ni=1; //(Fit.Type==NoFit ? 1 : 15);
//    for (long j=0; j<Ni; j++)
//      {
//      double X=LoX+((HiX-LoX)*(j+1))/(Ni);
//      double Y=pS1->Yx(X);
//      Yt+=(X-Xi)*0.5*(Yi+Y);
//      Yi=Y;
//      Xi=X;
//      }
//  
//    if (pS2)
//      pS2->SetPt(i,Xi,Yt);
//    }
//  double NormFactor=1.0/GTZ(Yt);
//
//  N = Y.GetLen();
//  for (i=0; i<N; i++)
//    SetPt(i,Xp(i),NormFactor*Yp(i));
//
//  if (bRedist)
//    DoRedistribute();
//  ReFit();
//
//  if (pS2)
//    {
//    for (i=0; i<pS2->NPts(); i++)
//      pS2->SetPt(i,pS2->Xp(i),NormFactor*(pS2->Yp(i)));
//    pS2->ReFit();
//    }
//
//  if (pSrc1==this) delete pS1;
//  return NormFactor;*/
//  }
//
// -------------------------------------------------------------------------

flag C2DFn::LoadPts(char* FName, CCnvIndex XCnvDC, CCnvIndex YCnvDC)
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
    }
  Y.SetSize(X.GetLen());

  //make sure upper and lower limits are reasonably meaningful...
  if (X.GetLen()>0)
    {
    for (int i=0; i<X.GetLen(); i++)
      {
      dXLoLim = Min(dXLoLim, X[i]);
      dXUpLim = Max(dXUpLim, X[i]);
      dYLoLim = Min(dYLoLim, Y[i]);
      dYUpLim = Max(dYUpLim, Y[i]);
      }
    if (bXLog)
      dXLoLim = Max(dXLoLim, MinLogVal);
    if (bYLog)
      dYLoLim = Max(dYLoLim, MinLogVal);
    if (dXUpLim - dXLoLim<=1.0e-6)
      dXUpLim = dXLoLim + 0.1;
    if (dYUpLim - dYLoLim<=1.0e-6)
      dYUpLim = dYLoLim + 0.1;
    }

  StructureChanged(this);
  bReDoFrac = True;
  return b;
  }

// -------------------------------------------------------------------------

flag C2DFn::SavePts(char* FName, char * XLabel, char * YLabel)
  {
  FILE* f = fopen(FName, "wt");
  if (f)
    {
    char Buff[512];
    sprintf(Buff, "%s,%s\n", XLabel ? XLabel : sXName(), YLabel ? YLabel : sYName());
    fwrite(Buff, sizeof(char), strlen(Buff), f);
    
    if (X.sCnvTxt.Length()>0 || Y.sCnvTxt.Length()>0)
      {
      sprintf(Buff, "%s,%s\n", X.sCnvTxt(), Y.sCnvTxt());
      fwrite(Buff, sizeof(char), strlen(Buff), f);
      }

    for (int i=0; i<Y.GetLen(); i++)
      {
      sprintf(Buff, "%g,%g\n", X.Human(i), Y.Human(i));
      fwrite(Buff, sizeof(char), strlen(Buff), f);
      }
    fclose(f);
    return True;
    }
  return False;
  }

// -------------------------------------------------------------------------

XID xidXYFnLen     = XyXID(0);
XID xidXYFnName    = XyXID(1);
XID xidXYPtsFName  = XyXID(2);
XID xid2DBlob      = XyXID(3);
XID xidXYFnLen_    = XyXID(4);

const int Id_Name        =   1;
const int Id_Xf          =   3;
const int Id_Yf          =   5;
const int Id_PtsFName    =   6;
const int Id_YdMn        =   7;
const int Id_XfMn        =   8;
const int Id_YfMn        =  10;
const int Id_XfMx        =  11;
const int Id_YfMx        =  13;
const int Id_Tag         =  14;
const int Id_Len         =  15;
const int Id_RqdLen      =  16;
const int Id_NoElimIter  =  17;
const int Id_ShowData    =  24;
const int Id_XxLo        =  30;
const int Id_XfLo        =  31;
const int Id_YfLo        =  32;
const int Id_XxUp        =  33;
const int Id_XfUp        =  34;
const int Id_YfUp        =  35;
const int Id_XfGR        =  36;
const int Id_YfGR        =  37;
const int Id_GrWidth     =  38;
const int Id_GrHeight    =  39;

const int Id_SaveBtn       = 150;
const int Id_CancelBtn     = 151;
const int Id_SortBtn       = 152;
const int Id_RefitBtn      = 153;
const int Id_ShowEqnBtn    = 154;
const int Id_ShowOtherBtn  = 155;
const int Id_ShowDataBtn   = 156;
const int Id_AutoScaleBtn  = 157;
const int Id_InitParmsBtn  = 158;
const int Id_YToCurveBtn   = 159;
const int Id_LoadPtsBtn    = 160;
const int Id_SavePtsBtn    = 161;
const int Id_DistributeBtn = 162;
const int Id_LogXBtn       = 163;
const int Id_LogYBtn       = 164;
const int Id_CumYBtn       = 165;

void C2DFn::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  Strng S;
  DDBValueLstMem DDB0;
  TagObjClass::GetSDescValueLst(C2DModel::GroupName, DDB0);
  DDB.String("Eqn",         "", DC_,         "",     xidXYFnName,  this, isParm, DDB0());

  DDB.Blob("C2DInfoBlob", xid2DBlob, this);

  DDB.Long  ("Len",         "", DC_,         "", xidXYFnLen,   this, isParm);
  DDB.Long  ("RqdLen",      "", DC_,         "", &iRqdLength,  this, isParm);
  DDB.Long  ("NoElimIter",  "", DC_,         "", &iNoElimIter, this, isParm);
  DDB.Long  ("GraphWidth",  "", DC_,         "", &iGraphWidth, this, isParm);
  DDB.Long  ("GraphHeight", "", DC_,         "", &iGraphHeight,this, isParm);
  DDB.Double("XDispMin",    "", DC_,         "", &dXDispMin,   this, isParm);
  DDB.Double("XDispMax",    "", DC_,         "", &dXDispMax,   this, isParm);
  DDB.Double("YDispMin",    "", DC_,         "", &dYDispMin,   this, isParm);
  DDB.Double("YDispMax",    "", DC_,         "", &dYDispMax,   this, isParm);
  DDB.Double("XLoExt",      "", DC_,         "", &dXLoExt,     this, isParm);
  DDB.Double("XUpExt",      "", DC_,         "", &dXUpExt,     this, isParm);
  DDB.Double("XLower",      "", DC_,         "", &dXLoLim,     this, isParm);
  DDB.Double("XUpper",      "", DC_,         "", &dXUpLim,     this, isParm);
  DDB.Double("YLower",      "", DC_,         "", &dYLoLim,     this, isParm);
  DDB.Double("YUpper",      "", DC_,         "", &dYUpLim,     this, isParm);
  DDB.Double("XGRatio",     "", DC_,         "", &dXGRatio,    this, isParm);
  DDB.Double("YGRatio",     "", DC_,         "", &dYGRatio,    this, isParm);
  DDB.String("PtsFile",     "", DC_,         "", xidXYPtsFName,this, isParm);
  DDB.String("XName",       "", DC_,         "", &sXName,      this, isParm|noFile);
  DDB.String("YName",       "", DC_,         "", &sYName,      this, isParm|noFile);
  DDB.Bool  ("ShowEqn",     "", DC_,         "", &bShowEqn,    this, isParm);
  DDB.Bool  ("ShowOther",   "", DC_,         "", &bShowOther,  this, isParm);
  DDB.Bool  ("ShowData",    "", DC_,         "", &bShowData,   this, isParm);
  DDB.Bool  ("XLog",        "", DC_,         "", &bXLog,       this, isParm);
  DDB.Bool  ("YLog",        "", DC_,         "", &bYLog,       this, isParm);
  DDB.Bool  ("YCumulative", "", DC_,         "", &bYCumulative,this, isParm);
  DDB.Bool  ("Redist",      "", DC_,         "", &bRedist,     this, isParm);
  DDB.Bool  ("AutoScale",   "", DC_,         "", &bAutoScale,  this, isParm);
  DDB.BeginArray(this, "2D", "2DPts", Length(), 0/*xidXYFnLen_*/);
  for (int i=0; i<Length(); i++)
    {
    DDB.BeginElement(this, i); 
    DDB.Double("X",     "",     DC_,    "",   &X.m_d[i],      this, isParm); 
    DDB.Double("Y",     "",     DC_,    "",   &Y.m_d[i],      this, isParm); 
    }
  DDB.EndArray();
  pM->BuildDataDefn(DDB);
  DDB.EndStruct();
  }

// -------------------------------------------------------------------------

flag C2DFn::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB)) 
    return 1;

  if (pCollectDataFn!=NULL)
    {
    if (pCollectDataFn(pCollectDataPtr))
      {
      }
    }

  switch (DCB.lHandle)
    {
    case xidXYFnName:
      DCB.pC = ModelName(DCB.rpC);
      return 1;
    case xidXYFnLen: 
      DCB.L = Length(DCB.rL);
      return 1;
    case xidXYFnLen_: 
      DCB.L = Length(DCB.rL);
      return 1;
    case xidXYPtsFName: 
      if (DCB.rpC)
        PtsFName = DCB.rpC;
      DCB.pC = PtsFName();
      return 1;
    case xid2DBlob: 
      {
      if (DCB.rpBlob)
        {
        char*p=(char*)DCB.rpBlob;
        X.iCnvDC =*((byte*)p);
        p++;
        Y.iCnvDC =*((byte*)p);
        p++;
        X.sCnvTxt="";
        if ((X.iCnvDC)>0)
          {
          X.sCnvTxt=p;
          p+=X.sCnvTxt.Length()+1;
          }
        Y.sCnvTxt="";
        if ((Y.iCnvDC)>0)
          {
          Y.sCnvTxt="";
          p+=Y.sCnvTxt.Length()+1;
          }
        }
      char*p=(char*)DCB.pBlob;
      *((CCnvIndex*)p)=X.iCnvDC;
      p++;
      *((CCnvIndex*)p)=Y.iCnvDC;
      p++;
      if ((X.iCnvDC)>0 && X.sCnvTxt.Length()>0)
        {
        strcpy(p, X.sCnvTxt());
        p+=X.sCnvTxt.Length()+1;
        }
      if ((Y.iCnvDC)>0 && Y.sCnvTxt.Length()>0)
        {
        strcpy(p, Y.sCnvTxt());
        p+=Y.sCnvTxt.Length()+1;
        }
      DCB.wBlobLen=p-(char*)DCB.pBlob;
      }
      return 1;
    }

  if (pM->DataXchg(DCB))
    return 1;
  return False;
  }

// -------------------------------------------------------------------------

void C2DFn::Build()
  {
  pchar p=Tag();
  static Strng sx;
  static Strng sy;
  sx = sXName();
  sx = sx.Left(9);
  sy = sYName();
  sy = sy.Left(8);

  StartPage((strlen(p)>0) ? p : "Fn");
  StartBlk(2, 0, NULL);
  int L=0;
  SetDParm(L,"Equation", 9, "", Id_Name, 30, 0, "");
  Strng S;
  pTagObjClass pC;
  for (int i=0; (pC=TagObjClass::FindClassIndexed(C2DModel::GroupName, i))!=NULL;i++)
    {
    pC->GetShortDesc(S);
    FldHasFixedStrValue(i, S());
    }

  StartBlk(bShowEqn ? (pM->NParms()>0 && pM->InitParms(False) ? 2 : 1) : 1, 0, NULL);
  L=0;
  SetButton(L,bShowEqn? "-":"+", Id_ShowEqnBtn, 3, 1, " ");
  SetDesc(L, "Equation -------------------------------", -1, 24, 2, "");

  if (bShowEqn)
    {
    if (pM->NParms()>0 && pM->InitParms(False))
      {
      L++;
      SetSpace(L, 1);
      SetButton(L,"Init Parms",  Id_InitParmsBtn, 12, 0, "");
      }
    }
  pM->SetView(pView);
  pM->Build(bShowEqn);

  flag bAllow1=bAllowRedist||bAllowCumulative;
  flag bAllow2=bAllowYtoCurve||bAllowRefit;
  
  int L1=bAllowRedist ? 2:0;
  int L2=bAllow1 ? 1:0;
  int L3=bAllow2 ? 1:0;
  StartBlk(bShowOther ? 9+L1+L2+L3 : 1, 0, NULL);
  L=0;
  //if (bShowOther)
  //  SetButton(L,"Show>Hide Info", Id_ShowOtherBtn, 16, 0, "");
  //else
  //  SetButton(L,"Hide>Show Info", Id_ShowOtherBtn, 16, 0, "");
  //SetDesc(L, "----------------------------------------", -1, 24, 2, "");
  SetButton(L,bShowOther? "-":"+", Id_ShowOtherBtn, 3, 1, " ");
  SetDesc(L, "Information ----------------------------", -1, 24, 2, "");
  
  if (bShowOther)
    {
    L++;
    SetSpace(L, 1);
    if (bXLog)
      SetButton(L,"Log>Lin X", Id_LogXBtn, 14, 0, "");
    else
      SetButton(L,"Lin>Log X", Id_LogXBtn, 14, 0, "");
    SetSpace(L, 1);
    if (bYLog)
      SetButton(L,"Log>Lin Y", Id_LogYBtn, 14, 0, "");
    else
      SetButton(L,"Lin>Log Y", Id_LogYBtn, 14, 0, "");
    
    L++;
    SetSpace(L, 1);
    if (bAutoScale)
      SetButton(L,"Auto>Manual Scale Display",  Id_AutoScaleBtn, 29, 0, "");
    else
      SetButton(L,"Manual>Auto Scale Display",  Id_AutoScaleBtn, 29, 0, "");
    if (bAllow1)
      {
      if (bAllowCumulative)
        {
        SetSpace(L, 1);
        if (bYCumulative)
          SetButton(L,"Cum>Norm Y", Id_CumYBtn, 14, 0, "");
        else
          SetButton(L,"Norm>Cum Y", Id_CumYBtn, 14, 0, "");
        }
      else
        SetSpace(15);
      if (bAllowRedist)
        {
        SetSpace(L, 1);
        SetButton(L,"Distribute", Id_DistributeBtn, 14, 0, "");
        }
      }
  
    if (bAllow2)
      {
      L++;
      if (bAllowYtoCurve)
        {
        SetSpace(L, 1);
        SetButton(L,"Y To Curve", Id_YToCurveBtn, 14, 0, "");
        }
      else
        SetSpace(15);
      if (bAllowRefit)
        {
        SetSpace(L, 1);
        SetButton(L,"ReFit",  Id_RefitBtn, 14, 0, "");
        }
      }

    L++;
    SetSpace(L, 1);
    SetDParm(L,"Graph: Width", 13, "", Id_GrWidth, 2, 2, "");
    SetSpace(L,1);
    SetDParm(L,"Height", 7, "", Id_GrHeight, 2, 2, "");
    L++;
    SetSpace(L,14);
    SetDesc(L, sx(), -1, 9, 2, "");
    SetDesc(L,"(X)", -1, 3, 2, "");
    SetSpace(L,1);
    SetDesc(L, sy(), -1, 8, 2, "");
    SetDesc(L,"(Y)", -1, 3, 2, "");
    L++;
    SetSpace(L, 1);
    SetDesc(L,"Display Min", -1, 11, 0, "");
    SetSpace(L,2);
    SetParm(L,"", Id_XfMn, 12, 2, "", !bAutoScale);
    SetParm(L,"", Id_YfMn, 12, 2, "", !bAutoScale);
    L++;
    SetSpace(L, 1);
    SetDesc(L,"Display Max", -1, 11, 0, "");
    SetSpace(L,2);
    SetParm(L,"", Id_XfMx, 12, 2, "", !bAutoScale);
    SetParm(L,"", Id_YfMx, 12, 2, "", !bAutoScale);
    L++;
    SetSpace(L, 1);
    SetDesc(L,"Lower Boundry", -1, 13, 0, "");
    SetParm(L,"", Id_XfLo, 12, 2, "");
    SetParm(L,"", Id_YfLo, 12, 2, "");
    L++;
    SetSpace(L, 1);
    SetDesc(L,"Upper Boundry", -1, 13, 0, "");
    SetParm(L,"", Id_XfUp, 12, 2, "");
    SetParm(L,"", Id_YfUp, 12, 2, "");
    if (bAllowRedist)
      {
      L++;
      SetSpace(L, 1);
      SetDesc(L,"Geom Ratio", -1, 11, 0, "");
      SetSpace(L,3);
      SetParm(L,"", Id_XfGR, 6, 2, "");
      SetSpace(L,6);
      SetParm(L,"", Id_YfGR, 6, 2, "");
      L++;
      SetSpace(L, 1);
      SetDParm(L,"Required Length", 4, "", Id_RqdLen,  6, 2, "");
      SetSpace(L,1);
      SetDParm(L,"Eliminate Factor", 3, "", Id_NoElimIter,  6, 2, "");
      }
    }

  StartBlk(bShowData ? 5+(int)Length() : 1, 5, NULL);
  L=0;
//  if (bShowData)
//    SetButton(L,"Show>Hide Pts", Id_ShowDataBtn, 16, 6, "");
//  else
//    SetButton(L,"Hide>Show Pts", Id_ShowDataBtn, 16, 6, "");
//  SetDesc(L, "----------------------------------------", -1, 24, 2, "");
  SetButton(L,bShowData ? "-":"+", Id_ShowDataBtn, 3, 1, " ");
  SetDesc(L, "Data Points ----------------------------", -1, 24, 2, "");
  if (bShowData)
    {
    L++;
    SetSpace(L, 1);
    SetButton(L,"Load Pts", Id_LoadPtsBtn, 14, 0, "");
    SetSpace(L, 1);
    SetButton(L,"Save Pts", Id_SavePtsBtn, 14, 0, "");
    L++;
    SetSpace(L, 1);
    SetDParm(L,"File", 7, "", Id_PtsFName, 50, 0, "");
    L++;
    SetSpace(L, 1);
    SetDParm(L,"Length", 7, "", Id_Len,  7, 2, "");
    SetSpace(L, 1);
    SetButton(L,"Sort", Id_SortBtn, 14, 0, "");
    if (Length()>0)
      {
      L++;
      SetSpace(L,6);
      SetDesc(L, sx(), -1, 9, 2, "");
      SetDesc(L,"(X)", -1, 3, 2, "");
      SetSpace(L,1);
      SetDesc(L, sy(), -1, 8, 2, "");
      SetDesc(L,"(Y)", -1, 3, 2, "");
      for (long r=0; r<Length(); r++)
        {
        L++;
        char tmp[5];
        sprintf(tmp, "%i", r);
        SetDesc(L, tmp, -1 , 4, 2, "");
        SetSpace(L,1);
        SetParm(L,"", Id_Xf, 12, 2, "");
        SetParm(L,"", Id_Yf, 12, 2, "");
        }
    	}
    }
  }

//---------------------------------------------------------------------------

void C2DFn::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//header
    //int p=EI.PageNo;
    //int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Name:      Str=pM->ShortDesc();  break;
      }
    }
  
  if (CurrentBlk(EI))
    {//eqn
    }
  pM->SetView(pView);
  pM->Load(EI, Str);

  if (CurrentBlk(EI))
    {//other
    //int p=EI.PageNo;
    //int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XfLo:      Str.Set("%g", dXLoLim); break;
      case Id_XfUp:      Str.Set("%g", dXUpLim); break;
      case Id_YfLo:      Str.Set("%g", dYLoLim); break;
      case Id_YfUp:      Str.Set("%g", dYUpLim); break;
      case Id_XfMn:      Str.Set("%g", dXDispMin); break;
      case Id_YfMn:      Str.Set("%g", dYDispMin); break;
      case Id_XfMx:      Str.Set("%g", dXDispMax); break;
      case Id_YfMx:      Str.Set("%g", dYDispMax); break;
      case Id_XxLo:      Str.Set("%g", dXLoExt); break;
      case Id_XxUp:      Str.Set("%g", dXUpExt); break;
      case Id_RqdLen:    Str.Set("%i", iRqdLength); break;
      case Id_NoElimIter:Str.Set("%i", iNoElimIter); break;
      case Id_GrWidth:   Str.Set("%i", iGraphWidth); break;
      case Id_GrHeight:  Str.Set("%i", iGraphHeight); break;
      case Id_XfGR:      Str.Set("%g", dXGRatio); break;
      case Id_YfGR:      Str.Set("%g", dYGRatio); break;
      }
    }

  if (CurrentBlk(EI))
    {//data
    //int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Xf:        Str.Set("%g", X[i]); break;
      case Id_Yf:        Str.Set("%g", Y[i]); break;
      case Id_Len:       Str.Set("%i", Length()); break;
      case Id_PtsFName:  Str=(PtsFName.Length()>0) ? PtsFName() : ""; break;
      }
    }
  }

//---------------------------------------------------------------------------

long C2DFn::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header
    bObjModified=1;
    //int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Name:    
        ChangeModel(FindModelByDesc(Str()));
        View().DoRebuild();
        Fix=1;
        break;
      }
    }

  if (CurrentBlk(EI))
    {//eqn
    }
  pM->SetView(pView);
  Fix = Max(pM->Parse(EI, Str), Fix);
  bObjModified |= pM->bObjModified;
  
  if (CurrentBlk(EI))
    {//other
    bObjModified = 1;
    //int p=EI.PageNo;
    //int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_RqdLen:
        iRqdLength = Range(2L, Str.SafeAtoL(), (long)Max2DFnPts);
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
      case Id_GrWidth:
        iGraphWidth = Range(10L, Str.SafeAtoL(), 60L);
        Fix=1; 
        break;
      case Id_GrHeight:  
        iGraphHeight = Range(5L, Str.SafeAtoL(), 30L);
        View().DoRebuild();
        Fix=1; 
        break;
      case Id_XfMn:    
        if (bXLog)
          dXLoLim = Max(dXLoLim, MinLogVal);
        dXDispMin = max(Str.SafeAtoF(), dXLoLim);
        Fix=1; 
        break;
      case Id_YfMn:    
        if (bYLog)
          dYLoLim = Max(dYLoLim, MinLogVal);
        dYDispMin = max(Str.SafeAtoF(), dYLoLim);
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
      case Id_XxLo:    
        dXLoExt = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_XfLo:    
        dXLoLim = Str.SafeAtoF(); 
        if (bXLog)
          {
          dXLoLim = Max(dXLoLim, MinLogVal);
          dXUpLim = Max(dXLoLim, dXUpLim);
          }
        if (dXUpLim - dXLoLim<=1.0e-6)
          dXUpLim = dXLoLim + 0.1;
        Fix=1; 
        break;
      case Id_YfLo:    
        dYLoLim = Str.SafeAtoF(); 
        if (bYLog)
          {
          dYLoLim = Max(dYLoLim, MinLogVal);
          dYUpLim = Max(dYLoLim, dYUpLim);
          }
        if (dYUpLim - dYLoLim<=1.0e-6)
          dYUpLim = dYLoLim + 0.1;
        Fix=1; 
        break;
      case Id_XxUp:    
        dXUpExt = Str.SafeAtoF(); 
        Fix=1; 
        break;
      case Id_XfUp:    
        dXUpLim = Str.SafeAtoF(); 
        if (dXUpLim - dXLoLim<=1.0e-6)
          dXUpLim = dXLoLim + 0.1;
        Fix=1; 
        break;
      case Id_YfUp:    
        dYUpLim = Str.SafeAtoF(); 
        if (dYUpLim - dYLoLim<=1.0e-6)
          dYUpLim = dYLoLim + 0.1;
        Fix=1; 
        break;
      case Id_XfGR:
        {
        dXGRatio = Max(Str.SafeAtoF(),MinLogVal);
        Fix=1; 
        break;
        }
      case Id_YfGR:    
        {
        dYGRatio = Max(Str.SafeAtoF(), MinLogVal);
        Fix=1; 
        break;
        }
      }
    }

  if (CurrentBlk(EI))
    {//data
    bObjModified = 1;
    //int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Xf: 
        SetLength(Max((long)(i+1), Length()));
        bReDoFrac = True;
        if (bXLog)
          {
          dXLoLim = Max(dXLoLim, MinLogVal);
          X[i] = Max(Str.SafeAtoF(), dXLoLim);
          }
        else
          X[i] = Str.SafeAtoF();
        Fix=1;
        break;
      case Id_Yf:
        SetLength(Max((long)(i+1), Length()));
        bReDoFrac = True;
        if (bYLog)
          {
          dYLoLim = Max(dYLoLim, MinLogVal);
          Y[i] = Max(Str.SafeAtoF(), dYLoLim);
          }
        else
          Y[i] = Str.SafeAtoF();
        Fix=1;
        break;
      case Id_Len: 
        SetLength(Range(0L, Str.SafeAtoL(), (long)Max2DFnPts));
        View().DoRebuild();
        break;
      case Id_PtsFName:
        PtsFName = Str;
        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long C2DFn::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header
    int p=EI.PageNo;
    //int i=(int)(EI.BlkRowNo-EI.Index);
    //switch (EI.FieldId)
    //  {
    //  case Id_ShowEqnBtn:
    //    bShowEqn = !bShowEqn;
    //    View().DoRebuild();
    //    break;
    //  case Id_ShowOtherBtn:
    //    bShowOther = !bShowOther;
    //    View().DoRebuild();
    //    break;
    //  case Id_ShowDataBtn:
    //    bShowData = !bShowData;
    //    View().DoRebuild();
    //    break;
    //  }
    }

  if (CurrentBlk(EI))
    {//eqn
    switch (EI.FieldId)
      {
      case Id_ShowEqnBtn:
        bShowEqn = !bShowEqn;
        View().DoRebuild();
        break;
      case Id_InitParmsBtn:
        {
        pM->InitParms(True);
        View().DoReload();
        break;
        }
      }
    }
  pM->SetView(pView);
  pM->ButtonPushed(EI, Str);

  flag bDoRefit = True;
  if (CurrentBlk(EI))
    {//other
    switch (EI.FieldId)
      {
      case Id_ShowOtherBtn:
        bShowOther = !bShowOther;
        View().DoRebuild();
        break;
      case Id_RefitBtn:
        Fix=1;
        View().DoReload();
        break;
      /*case Id_YMnMxBtn:
        {
        dYDispMin = DBL_MAX;
        dYDispMax = DBL_MAX*-1.0;
        for (long j=0; j<Y.GetLen(); j++)
          {
          dYDispMin = Min(dYDispMin, Y[j]);
          dYDispMax = Max(dYDispMax, Y[j]);
          }

        double xmin = dXDispMin-0.05*(dXDispMax-dXDispMin);
        double xmax = dXDispMax+0.05*(dXDispMax-dXDispMin);
        int CrvPts = C2DFnEdt::CrvPts;
        for (j=0; j<CrvPts; j++)
          {
          double Xv = (xmin+(j*(xmax-xmin))/(CrvPts-1));
          double Yv = Yx(Xv);
          dYDispMin = Min(dYDispMin, Yv);
          dYDispMax = Max(dYDispMax, Yv);
          }
        View().DoReload();
        break;        
        }*/
      case Id_AutoScaleBtn:
        bAutoScale = !bAutoScale;
        View().DoRebuild();
        break;
      case Id_YToCurveBtn:
        {
        for (long j=0; j<Y.GetLen(); j++)
          Y[j] = Yx(X[j]);
        bReDoFrac = True;
        Fix=1;
        bDoRefit = False;
        View().DoReload();
        break;
        }
      case Id_DistributeBtn:
        {
        if (DoRedistribute())
          {
          Fix=1;
          View().DoRebuild();
          }
        break;
        }
      case Id_LogXBtn:
        {
        bXLog = !bXLog;
        Fix=1;
        if (bXLog)
          {
          dXLoLim = Max(dXLoLim, MinLogVal);
          dXDispMin = Max(dXLoLim, dXDispMin);
          dXUpLim = Max(dXLoLim, dXUpLim);
          dXDispMax = Max(dXDispMin, dXDispMax);
          for (long j=0; j<Length(); j++)
            X[j] = Max(dXLoLim, X[j]);
          }
        View().DoRebuild();
        break;
        }
      case Id_LogYBtn:
        {
        bYLog = !bYLog;
        Fix=1;
        if (bYLog)
          {
          dYLoLim = Max(dYLoLim, MinLogVal);
          dYDispMin = Max(dYLoLim, dYDispMin);
          dYUpLim = Max(dYLoLim, dYUpLim);
          dYDispMax = Max(dYDispMin, dYDispMax);
          for (long j=0; j<Length(); j++)
            Y[j] = Max(dYLoLim, Y[j]);
          }
        View().DoRebuild();
        break;
        }
      case Id_CumYBtn:
        {
        bYCumulative = !bYCumulative;
        Fix=1;
        if (bYCumulative)
          ToCumulative();
        else
          ToFractional();
        View().DoRebuild();
        }
        break;
      }
    }
  if (CurrentBlk(EI))
    {//data
    switch (EI.FieldId)
      {
      case Id_ShowDataBtn:
        bShowData = !bShowData;
        View().DoRebuild();
        break;
      case Id_LoadPtsBtn:
        {
        if (PtsFName.Length()>0)
          {
          Strng ss;
          if (PtsFName.XStrRChr('.'))
            PtsFName.SetLength(PtsFName.XStrRChr('.')-PtsFName.Buffer());
          //if (PtsFName.Find('.')>=0)
          //  PtsFName = PtsFName.Left(PtsFName.Find('.'));
          PtsFName += ".csv";
//          PtsFName = AdaptFilename2(ss, PtsFName());
          PtsFName.FnExpand();

          LoadPts(PtsFName());
          Fix=1;
          View().DoRebuild();
          }
        break;
        }
      case Id_SavePtsBtn:
        {
        if (PtsFName.Length()>0)
          SavePts(PtsFName());
        break;
        }
      case Id_SortBtn:
        {
        Sort();
        View().DoReload();
        break;
        }
      }
    }

  if (bDoRefit && Fix)
    ReFit();
  return Fix;
  }

//---------------------------------------------------------------------------

const double ar_Perturb = 1.0e-5;

double C2DFn::RegSumSSQ(C2DRegStuff &R, CDVector &Parms)
  {
  pM->SetParms(Parms);
  double Sum = 0.0;
  for (long i=0; i<R.N; i++)
    {
    R.YC[i] = Yx(X[i]);
    Sum += Sqr((Y[i] - R.YC[i]) / R.StdD[i]);
    }
  return Sum;
  }

// -------------------------------------------------------------------------
// EvalDeriv evaluates the derivative of the isotherm response with respect to 
// parameter k at data point i.
double C2DFn::RegEvalDeriv(C2DRegStuff &R, long i, long k)
  {
//  double h = ar_Perturb * R.Parms[k];
  double h = R.ParmsTol[k] * 10.0;
  if (fabs(h) < fabs(ar_Perturb))
    h = ar_Perturb;
  //double h = ar_Perturb * (R.MaxParms[k] - R.MinParms[k]);
  R.Parms[k] += h;
  //double PerturbFp = pM->Equation(Xs, Y[i], R.Parms) / R.StdD[i];
  pM->SetParms(R.Parms);
  double PerturbFp = Yx(X[i]) / R.StdD[i];
  R.Parms[k] -= (2.0 * h);
  pM->SetParms(R.Parms);
  //double PerturbFm = pM->Equation(Xs, Y[i], R.Parms) / R.StdD[i];
  double PerturbFm = Yx(X[i]) / R.StdD[i];
  R.Parms[k] += h;
  pM->SetParms(R.Parms);
  return (PerturbFp - PerturbFm) / (2.0 * h);
 }

// -------------------------------------------------------------------------
//ConstructMatrix constructs the Z matrix and the C vector required by the Law 
//and Bailey regression algorithm.                                             
void C2DFn::RegConstructMatrix(C2DRegStuff &R, CDMatrix &Z, CDVector &C)
  {
  CDVector Derivs(R.NParms);
  C.SetSize(R.NParms);
  C.SetAll(0.0);
  Z.SetSize(R.NParms, R.NParms);
  Z.SetAll(0.0);
  RegSumSSQ(R, R.Parms);
  //Find the residuals
  double dd;
  for (long i=0; i<R.N; i++)
    {//Loop through all data points
    for (long k=0; k<R.NParms; k++)
      {//Get all derivs at data point i
      Derivs[k] = RegEvalDeriv(R, i, k);
      C[k] += ((Y[i] - R.YC[i]) *  Derivs[k] / R.StdD[i]);
	    dd = C[k];
      }
    //Construct the matrix
    for (long NRow=0; NRow<R.NParms; NRow++)
      for (long NCol=0; NCol<R.NParms; NCol++)
        Z[NRow][NCol] += (Derivs[NRow] * Derivs[NCol]);
    }
  StructureChanged(this);
  }

// -------------------------------------------------------------------------
// LawAndBailey is the Law and Bailey regression routine.                      
// On entry, B contains initial guesses for the parameters, BTol contains the  
// required tolerances, F is the equation to be used, S is the sum of squares  
// routine to be used, NParm is the number of parameters to estimate, Np is the
// number of data points in the data set.                                      
// On exit, B contains the estimated parameter values, Sa contains the sum of  
// squares at the minimu, and Iter contains the number of iterations performed 
flag C2DFn::LBEst( CDVector &DeltaParms, //returned
                   double &Sa, //returned
                   long &Iter, //returned
                   const long ItMax) 
  {
  C2DRegStuff R;
  R.NParms = pM->NParms();
  ASSERT(R.NParms>0); //this is meaningless unless the equation has parameters
  pM->GetParms(R.Parms);
  pM->GetParmRanges(R.MinParms, R.MaxParms, R.ParmsTol);
  R.N = Y.GetLen();   //==X.GetCols()
  R.YC.SetSize(R.N);
  R.StdD.SetSize(R.N);
  R.StdD.SetAll(1.0);
  CDVector NextParms(R.NParms);
  CDVector RestrParms(R.NParms);
  CDMatrix Z(R.NParms, R.NParms);
  CDVector C(R.NParms);
  double SaPrv1 = 1.0e+30;
  double SaPrv2 = 1.0e+30;
  double SaPrv3 = 1.0e+30;
  double SaInit = 0.0;
  //DeltaB.SetSize(R.NParm, 0.0);
  flag Converged = False;
  CLVector Index(R.NParms);
  int D;
  Iter = 0;
  #ifdef dbgEst
  dbgpln("------------** START LBEST **---------------------------------------------");
  dbgDump();
  R.MinParms.dbgDump("MinParms");
  R.MaxParms.dbgDump("MaxParms");
  R.ParmsTol.dbgDump("ParmsTol");
  R.StdD.dbgDump("StdD    ");
  dbgpln("------------");
  #endif
  SaInit = RegSumSSQ(R, R.Parms);
  //double SaPrv1 = Sa;
  
  //Calculate initial SSQ
  do
    {
    dbgpln("-+- %d -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+", Iter);
    #ifdef dbgEst
    R.Parms.dbgDump("Parms");
    dbgpln("Sa:%10g", Sa);
    #endif
    double Alpha = 1.0;
    double Beta = 0.25;
    RegConstructMatrix(R, Z, C);
    #ifdef dbgEst
    Z.dbgDump("Z");
    C.dbgDump("C");

//kcg hardwired
    for (long k=0; k<R.NParms; k++)
      dbgpln("C:%10g , k" , C[k],k);
    for (k=0; k<R.NParms ; k++)
      for (long kk=0; kk<R.NParms; kk++)
        dbgpln("Z:%10g , k , kk" , Z[k][kk], k , kk);
    #endif
    DeltaParms = C;
    if (Z.LUDecompose(Index, D))
      Z.LUBackSub(Index, DeltaParms);
    else 
      return False; //probably singular !!! (Z.GetErr() == VMErrSingular)
    //Invert, solution was returned in DeltaParms
    Iter++;
    double Dt = 0.0;
    for (long i=0; i<R.NParms; i++)
      Dt += (C[i] * DeltaParms[i]);
    if (Dt<0.0)
      {
      Dt = -Dt;
      DeltaParms.Mult(-1.0);
      }

    for (i=0; i<R.NParms; i++)
      {
      NextParms[i] = R.Parms[i] + DeltaParms[i];
      if (NextParms[i] > R.MaxParms[i])
        {
        RestrParms[i] = R.MaxParms[i];
        }
      else if (NextParms[i] < R.MinParms[i])
        {
        RestrParms[i] = R.MinParms[i];
        }          
      }

//    for (i=0; i<R.NParms; i++)
//      NextParms[i] = R.Parms[i] + DeltaParms[i];

    Converged = True;
    //Be optimistic !
    #ifdef dbgEst
    DeltaParms.dbgDump("DeltaParms");
    NextParms.dbgDump("NextParms");
    dbgpln("Dt:%10g", Dt);
    #endif

    //Check if tolerances are met
    //for (i=0; ((i<R.NParms) && Converged); i++)
    //  if (fabs(DeltaParms[i])>R.ParmsTol[i]) 
    //    Converged = False;
    Sa = RegSumSSQ(R, NextParms);
    if (Sa > 1e-5)	  // kcg the 1e-5 must become converge limit parm
      {
      Converged = False;
      #ifdef dbgEst
      dbgpln("Tolerances are NOT met !!!");
      #endif
      }
    else
      {//skip straight out...
      R.Parms = NextParms;
      pM->SetParms(R.Parms);
      }
    
    if (!Converged)
      {//Check if changes are significant
      flag AllConv = True;
      for (i=0; ((i<R.NParms) && AllConv); i++)
        //AllConv = (Parms[i] == (Parms[i] + DeltaParms[i]));
        //AllConv = (fabs(DeltaParms[i]) < ZeroLimit); //?????kga
        //AllConv = (fabs(DeltaParms[i]) < (R.MaxParms[i]-R.MinParms[i])*1.0e-6); //?????cnm
        //AllConv = ((fabs(DeltaParms[i])*1.0e6) < (R.Parms[i]+1.0e-6)); //?????cnm
//        AllConv = (fabs(DeltaParms[i]) < (R.Parms[i] * R.ParmsTol[i]));// + 1.0e-14)); //?????kcg
        AllConv = (fabs(DeltaParms[i]) < (R.ParmsTol[i]));// + 1.0e-14)); //?????kcg
      Converged = AllConv;
      }
    //if ((SaPrv3 - Sa) < 0.0001 * Sa)
    if (fabs(SaPrv3 - Sa) < (0.00001 * Sa))
      {//Check if significant changes in SSQ
      Converged = True;
      }
  
    #ifdef dbgEst
    if (!Converged)
      dbgpln("Tolerances are still NOT met !!!");
    #endif
    if (!Converged)
      {
      double RestrS,DeltaS,Test;
      flag AlphaConv = False;
      do
        {
        //SaNext:=SumSSq(Np,NParm,Model,NextB);  //Calculate SSQ at next parameter values
        //Calculate SSQ at restricted values
        for (i=0; i<R.NParms; i++)
          {
          RestrParms[i] = R.Parms[i] + Alpha * DeltaParms[i];
          if (RestrParms[i] > R.MaxParms[i])
            {
            RestrParms[i] = R.MaxParms[i];
//            if (fabs(Alpha) > 1e-14)
//              DeltaParms[i] = (R.MaxParms[i] - R.Parms[i]) / Alpha;
            }
          else if (RestrParms[i] < R.MinParms[i])
            {
            RestrParms[i] = R.MinParms[i];
//            if (fabs(Alpha) > 1e-14)
//              DeltaParms[i] = (R.MinParms[i] - R.Parms[i]) / Alpha;
            }          
          }
        Test = 0.0;
        RestrS = RegSumSSQ(R, RestrParms);
        DeltaS = SaPrv1 - RestrS;

        if ((SaPrv1 < 0.9e+30) && (!AlphaConv))       //Only after first iteration
          {
          Test = DeltaS - Beta * Dt * Alpha * (2.0 - Alpha);
          NextParms = RestrParms;
          if (Test < 0.0)
            {
            Alpha = Alpha / 2.0;
            //Remember and adjust up and down
// kcg must fix this
/*
           for (i=0; i<R.NParms; i++)
              {//Calculate restricted values
              NextParms[i] = R.Parms[i] + Alpha * DeltaParms[i];
              if (fabs(Alpha * DeltaParms[i]) < (R.ParmsTol[i] * fabs(R.Parms[i])))
                AlphaConv = True; //No improvement in alpha loop
              }
*/		     
            AlphaConv = True;
            for (i=0; i<R.NParms; i++)
              {//Calculate restricted values
              if (fabs(Alpha * DeltaParms[i]) > (R.ParmsTol[i]))
                {
                AlphaConv = False; //No improvement in alpha loop
                break;
                }
              }
            }
          }
        else                          //First time is direct substitution
          {
		      if (SaPrv1 > 0.9e30)
		        {
//            NextParms = RestrParms;
	          for (i=0; i<R.NParms; i++)
	            RestrParms[i] = R.Parms[i];
	          NextParms = RestrParms;
	          RestrS = SaInit;
            Test = 1.0;
			      }
		      else
	          {
	          for (i=0; i<R.NParms; i++)
	            RestrParms[i] = R.Parms[i];
	          NextParms = RestrParms;
	          RestrS = SaPrv1;
	          if (fabs(SaPrv3 - RestrS) < (0.00001 * RestrS))
	            {//Check if significant changes in SSQ
	            Converged = True;
  	          }
	          Test = 1.0;
	          }
          }
        }
      while ((Test < 0.0) && (!AlphaConv));
      
      //test > 0 AND AlpaConv = True
      #ifdef dbgEst
      DeltaParms.dbgDump("DeltaParms");
      NextParms.dbgDump("NextParms");
      dbgpln("Test  :%10g", Dt);
      dbgpln("RestrS:%10g", RestrS);
      dbgpln("DeltaS:%10g", DeltaS);
      #endif
      Sa = RestrS;
      R.Parms = NextParms;
      pM->SetParms(R.Parms);
      }
    SaPrv3 = SaPrv2;
    SaPrv2 = SaPrv1;
    SaPrv1 = Sa;
    }
  while ((!Converged) && (Iter <= ItMax));
  #ifdef dbgEst
  dbgpln("------------");
  dbgpln("Converged:%d", Converged);
  dbgDump();
  dbgpln("------------** END LBEST **---------------------------------------------");
  #endif
  return True;
  }

//---------------------------------------------------------------------------

void C2DFn::dbgDump( pchar Desc, flag Horizontal)
  {
  dbgpln("%s:[%d]", Desc, X.GetLen());
  if (Horizontal)
    {
    const long MxDmp=30;
    for (long i=0; i<Min(MxDmp,X.GetLen()); i++)
	    dbgp("%14.6g ", X[i]);
  	if (X.GetLen()>MxDmp)
  	  dbgp("...");
    dbgpln("");
    for (i=0; i<Min(MxDmp,X.GetLen()); i++)
	    dbgp("%14.6g ", Y[i]);
  	if (X.GetLen()>MxDmp)
  	  dbgp("...");
    dbgpln("");
    }
  else
    {
    for (long i=0; i<X.GetLen(); i++)
      dbgpln("%4d: %14.6g  %14.6g", i, X[i], Y[i]);
    }
  }

// -------------------------------------------------------------------------
//==========================================================================
// -------------------------------------------------------------------------

class C2DFnEntryExit
  {
  public:
    C2DFnEntryExit();
    ~C2DFnEntryExit();
  };

C2DFnEntryExit::C2DFnEntryExit()
  {
/********************************************** Testing Testing
  C2DFn A(&C2DFnClass, "A", NULL, TOA_Free);
  C2DFn B(&C2DFnClass, "B", NULL, TOA_Free);
  C2DXAxis Xla(0.0, 1.0, 9.0, 20.0, 9, 1.4);

  A.SetXLimits(Xla);

  A.SetYPt(0, 0.1);
  A.SetYPt(1, 0.2);
  A.SetYPt(2, 0.3);
  A.SetYPt(3, 0.4);
  A.SetYPt(4, 0.5);
  A.SetYPt(5, 0.6);
  A.SetYPt(6, 0.7);
  A.SetYPt(7, 0.8);
  A.SetYPt(8, 0.9);
  A.dbgDump("A 1");
  A.ToFractional();
  A.dbgDump("A 2");
  A.ToCumulative(0.0, 1.0);
  A.dbgDump("A 3");
  dbgpln("AFrac");
  for (long i=0; i<A.GetLen(); i++)
    dbgpln("%4d: %14.6g  %14.6g  %14.6g", i, A.Xp(i), A.Yp(i), A.YpFrac(i));
  dbgpln("%4d: %14.14s  %14.14s  %14.6g", i, "", "", A.YpFrac(i));

  C2DXAxis Xlb(0.0, 1.0, 9.0, 20.0, 2, 1);
  B.SetXLimits(Xlb);
  B.SetYPt(0,0.0);
  B.SetYPt(1,1.0);
  B.dbgDump("B 1");
  B.SetXLimits(Xla);
  B.dbgDump("B 2");
*/
//  for (int i=0; i<MaxSpareXYPts; i++)
//    C2DFn::SparePts[i]=NULL;
  };

C2DFnEntryExit::~C2DFnEntryExit()
  {
/*
  for (int i=0; i<MaxSpareXYPts; i++)
    while (C2DFn::SparePts[i]) 
      {
      pXYPt p=C2DFn::SparePts[i];
      C2DFn::SparePts[i]=C2DFn::SparePts[i]->NxtSpare;
      ::delete []p;
      };
*/
  };

C2DFnEntryExit C2DFnEntryExitInstance; // destructor will get called at exit;

//==========================================================================
