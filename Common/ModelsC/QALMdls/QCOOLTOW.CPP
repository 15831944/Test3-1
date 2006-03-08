//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __QCOOLTOW_CPP
#include "qcooltow.h"
#include "alspmdlb.h"
//#include "qbspmdl.h"

//==========================================================================
/*#D:#T:Cooling Tower
#X:#u<#l<#b<General Description>>>#n#n
The cooling tower mixes perfectly all the incoming feed streams and then carries out any chemical reaction specified.
#n#n
The model then calculates the temperature and flowrate of the liquid stream based on the approach temperature of the air,
the wet bulb temperature, the average cell age and the time since screen descale.
The thermal energy removed from the feed vaporises some of the water in the feed.
The model calculates the amount of water vapourised so that an energy balance is maintained.
#n#n
#n#m<cooltow1.bmp>#n
#n#u<#l<#b<References>>>#n#n
cooltowe.doc - Cooling Tower Model Specification 1.0
#n#n
#n#u<#l<#b<Inputs and Outputs>>>#n#w
Label   Input/Output  Minimum number  Maximum number#n
feed       Input           1              20#n
liquid     Output          1              1#n
vapours    Output          0              1#n
#a
To function the cooling tower must have at least one feed, and the liquid output connected.
The vapour link must be connected if evaporation occurs.
#n#n
#n#u<#l<#b<Model variables>>>#n#n
#i<Air wet bulb temperature> : #n
#i<Approach> : #n
#i<No cells on line> : #n
#i<Avg cell age> : #n
#i<Time since screen descale> : #n
#i<Water vaporised> : 
#n#n
#n#u<#l<#b<Chemical Equations>>>#n#n
By default, no chemical reactions occur in the cooling towers.
Chemical reactions can be specified in a reaction file.
Any reactions take place before evapouration occurs and the outlet temperature set.
#n#n
#n#u<#l<#b<Heat Transfer>>>#n#n
Predictive model for Baro Cooling Water Temperature.#n
#n
Cooling water temperature = (a) + (b) + (c) + (d)#n
#n
where:#n
(a) = Wet bulb temperature#n
(b) = Approach temperature for a cell in good condition#n
(c) = Approach elevation (packing scale)#n
(d) = Approach elevation (entrainment screen scale)#n
#n
(a)  Annual average wet bulb temperature = 18.8C.
Hottest in January 23.5C.
Coldest in July 14.0C.#n
(b)  Found by trial to be 9.5C#n
(c)  Packing scale increases approach by 1.1C per year from when the packing is replaced.
Packing is replaced on an irregular basis and cells 2, 4, and 7 are expected to be repacked in winter 1996.
The current birthdates for the packings are: 1 - 1990, 2 - 1986, 3 - 1995, 4 - 1990, 5 - 1991, 6 - 1991, 7 - 1989.#n
(d)  Entrainment screen scale increases approach by 4.0C per year but is cleaned off each year in winter.#n
#n
Therefore:#n
Cooling water Temp = Wet bulb temp + 9.5 + Average cell age (years)*1.1 + months since screen descale / 12 * 4.0
*/
//==========================================================================

const byte ioid_Feed         = 0;
const byte ioid_Liquid       = 1;
const byte ioid_Vap          = 2;

static IOAreaRec QCoolingTowerIOAreaList[] =
  {{"Feed",   "Feed"  ,   ioid_Feed ,   LIO_In ,  nc_MLnk, 1,  20,     0, 1.0f},
   {"Liquid", "Liquid"  , ioid_Liquid , LIO_Out,  nc_MLnk, 1,  1,     0, (float)0.5},
   {"Vapour", "Vapour" ,  ioid_Vap   ,  LIO_Out,  nc_MLnk, 0,  1,     0, (float)0.5},
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_QCoolingTower[] = { DD_Poly, -10,-3, -10,3, 10,3, 10,-3, -10,-3,
                                    DD_Poly, -10, 3, -11,15, 8,15, 8,3, 
                                    DD_Poly, 3,3, 2,15,
                                    DD_Poly, -8,15, -8,16, 0,16, 0,15,
                                    DD_Poly,  3,15, 2,18, 6,18, 5,15,
                                    DD_End }; 
 //--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(QCoolingTower, "QCoolTow", "", Drw_QCoolingTower, "HeatTransfer", "CT", TOC_ALL|TOC_GRP_ALUMINA|TOC_QAL,
                    "Process:Unit:QAL Cooling Tower",
                    "The cooling tower mixes perfectly all the incoming feed " 
                    "streams and then carries out any chemical reaction specified. "
                    "The model then calculates the temperature and flowrate of the " 
                    "liquid stream based on the approach temperature of the air, the wet " 
                    "bulb temperature, the average cell age and the time since screen"
                    "descale. The thermal energy removed from the feed vaporises some " 
                    "of the water in the feed.  The model calculates the amount of water " 
                    "vapourised so that an energy balance is maintained. ")

QCoolingTower::QCoolingTower(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(QCoolingTowerIOAreaList);
  Contents.SetClosed(False);

  AirWetBulbT = C_2_K(18.8) ; //   
  Approach = C_2_K(9.5) ;//        
  NoCellinLine = 7; //   
  AvgCellAge = 365.25 * 86400.0 * 6; //  6 years   
  TimeSinceDescale = 365.25 * 86400.0 / 12.0 * 6;    // 6 months
  
  EvapFraction=0.0;
  QmWaterEvap=0.0;

  EHX.Open(&CEHX_LossPerQmClass);
  VLE.Open(NULL, true);
  };

// -------------------------------------------------------------------------

void QCoolingTower::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility();
  
  DDB.Double  ("AirWetBulbT",       "",  DC_T,    "C",     &AirWetBulbT,  this, isParm);
  DDB.Double  ("Approach",          "",  DC_dT,   "C",     &Approach,     this, isParm);
  DDB.Long    ("NoCellinLine",      "",  DC_ ,    "",      &NoCellinLine, this, isParm|InitHidden);
  DDB.Double  ("AvgCellAge",        "",  DC_Time, "year",  &AvgCellAge,   this, isParm);
  DDB.Double  ("TimeSinceDescale",  "",  DC_Time, "month", &TimeSinceDescale, this, isParm);
  DDB.Double  ("EvapFraction",      "",  DC_Frac, "%",     &EvapFraction, this, isResult);
  DDB.Double  ("QmWaterEvap",       "",  DC_Qm,   "kg/s",  &QmWaterEvap,  this, isResult);
  
  RB.Add_OnOff(DDB, False);
  
  BuildDataDefnShowIOs(DDB);

  RB.BuildDataDefn(DDB);
  DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag QCoolingTower::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  switch (DCB.lHandle)
    {
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB); 
      DCB.B=Contents.Closed(); 
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag QCoolingTower::ValidateData(ValidateDataBlk & VDB)
  {
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void QCoolingTower::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NJoins(); j++)
        SetPBJoinPressure(j, Std_P, true, true);
      break;
      }
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  };
 
//--------------------------------------------------------------------------

void QCoolingTower::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  };
 
//--------------------------------------------------------------------------

void QCoolingTower::EvalSteadyState()
  {
  };


//===========================================================================

class EvapFnd : public MRootFinderBase
  {
  public:
    pSpModel pMdl;
    double T, P;
    static CToleranceBlock s_Tol;
    CVLEBase & m_VLE;
    EvapFnd(pSpModel pMdl_, double t, double p, CVLEBase & VLE) :
      MRootFinderBase("QALCoolTower:EvapFnd", s_Tol),//1.0e-9)
      m_VLE(VLE)
      {
      pMdl=pMdl_;
      T=t;
      P=p;
      };
    LPCTSTR ObjTag() { return (LPCTSTR)pMdl->FullObjTag(); };
    double Function(double x)
      {
      m_VLE.SetFlashVapFrac(*pMdl, x, 0);
      pMdl->ClrStatesOK();
      pMdl->SetTemp(T);
      pMdl->SetPress(P);
      double h=pMdl->totHf();
      return h;
      };
  };

CToleranceBlock EvapFnd::s_Tol(TBF_BothSys, "QALCoolTower:EvapFnd", 0, 1.0e-9);

//--------------------------------------------------------------------------

void QCoolingTower::EvalProducts(long JoinMask)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      int ioLiq = IOWithId_Self(ioid_Liquid);
      ASSERT(ioLiq>=0 );
      SpConduit & Ql=*IOConduit(ioLiq);

      Ql.QZero();
      SigmaQInPMin(Ql, som_ALL, Id_2_Mask(ioid_Feed));

      RB.EvalProducts(Ql);

//   NoCellinLine  ??
      double AvgCellAgeY=AvgCellAge/(365.25*24.0*60.0*60.0);   
      double TimeSinceDescaleM=TimeSinceDescale/(365.25*24.0*60.0*60.0/12.0);   
      
      double ReqdLiqTemp = AirWetBulbT + Approach + AvgCellAgeY*1.1 + TimeSinceDescaleM/12.0*4.0;
      
      double TDrop=Max(0.0, Ql.Temp()-ReqdLiqTemp);
      if (TDrop>0.0)
        {
        EvapFnd EF(Ql.Model(), ReqdLiqTemp, Ql.Press(), VLE);
        EF.SetTarget(Ql.totHf());
        if (Valid(EvapFraction))
          EF.SetEstimate(EvapFraction, 1.0);
        flag Ok = false;
        int iRet=EF.Start(0.0, 1.0);
        if (iRet==RF_EstimateOK) //estimate is good, solve not required
          {
          EvapFraction=EF.Result();
          Ok = true;
          }
        else
          {
          if (iRet==RF_BadEstimate)
            iRet=EF.Start(0.0, 1.0); // Restart
          if (iRet==RF_OK)
            if (EF.Solve_Brent()==RF_OK)
              {
              EvapFraction=EF.Result();
              Ok = true;
              }
          }
        }
      QmWaterEvap=Ql.QMass(som_Gas);

      int ioVap = IOWithId_Self(ioid_Vap);
      if (ioVap>=0)
        {
        SpConduit & Qv=*IOConduit(ioVap);
        Qv.QZero();
        Qv.QAddF(Ql, som_Gas, 1.0);
        Qv.SetPress(Std_P);
        Qv.SetTemp(ReqdLiqTemp);

        /*//Ql.Model()->MArray().ZeroPhase(som_Gas);
        double Qsl=Ql.QMass(som_SL);
        Ql.QSetM(Ql, som_SL, Qsl, Std_P);
        Ql.SetTemp(ReqdLiqTemp);*/
        StkSpConduit Liquor("Liquor", chLINEID(), this);
        Liquor().QSetF(Ql, som_SL, 1.0, Std_P);
        Liquor().SetTemp(ReqdLiqTemp);
        Ql.QCopy(Liquor());
        }
      else
        {//error, no vapour stream connected!
        Ql.SetTemp(ReqdLiqTemp);
        }
      }
      break;
    default:
      MN_Surge::EvalProducts(JoinMask);
    }
  }
    
//--------------------------------------------------------------------------

void QCoolingTower::ConvergeStates(CConvergeStateBlk &CSB)
  {
  MN_Surge::ConvergeStates(CSB);
  ASSERT(SolveDynamicMethod());
  double Duty=0.0;
  }

//==========================================================================
//
//
//
//==========================================================================
