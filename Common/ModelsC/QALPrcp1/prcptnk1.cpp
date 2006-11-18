//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __PRCPTNK1_CPP
#include "prcptnk1.h"

//==========================================================================

const byte ioid_Feed         = 0;
const byte ioid_Product      = 1;

static IOAreaRec QPrecipMainIOAreaList[] =
  {{"Feed",     "Feed",      ioid_Feed    , LIO_In ,    nc_MLnk, 1, 20,     0, 1.0f},
   {"Overflow", "Overflow",  ioid_Product , LIO_Out,    nc_MLnk, 1,  1,     0, (float)0.5},
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_QPrecipMain[] = {  DD_Poly, -5,14, -5,-14, 0,-16, 5,-14, 5,14, -5,14,
                                   DD_End }; 
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(QPrecipMain1, "QALPrecip", "1", Drw_QPrecipMain, "Tank", "PRCP", TOC_ALL|TOC_GRP_ALUMINA|TOC_QALEXTRA,
                    "Process:Unit:QAL Precipitator(1)",
                    "The precipitator models a continuous gibbsite crystallisation "
                    "precipitation tank.")

QPrecipMain1::QPrecipMain1(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(QPrecipMainIOAreaList);
  Contents.SetClosed(false);
  
  }

// -------------------------------------------------------------------------

void QPrecipMain1::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility();
  DDB.Text    ("xxxx");
  RB.Add_OnOff(DDB, False);
  
  DDB.Visibility();
  BuildDataDefnShowIOs(DDB);

  RB.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage);
  DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag QPrecipMain1::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag QPrecipMain1::ValidateData(ValidateDataBlk & VDB)
  {
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void QPrecipMain1::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        SetJoinPressure(j, Std_P, true, true);
      break;
      }
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  };
 
//--------------------------------------------------------------------------

void QPrecipMain1::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  }
 
//--------------------------------------------------------------------------

void QPrecipMain1::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      int ioProd = IOWithId_Self(ioid_Product);
      ASSERT(ioProd>=0);
      SpConduit & Qp=*IOConduit(ioProd);
      Qp.QZero();
      SigmaQInPMin(Qp, som_ALL, Id_2_Mask(ioid_Feed));//, &QSMBayerClass);
      //RB.EvalProducts(Qp);

      //Qp.SetModel(&QSMBayerClass);
      //QSMBayer & Bm=*((QSMBayer*)Qp.Model());

//CNM      if (Qp.QMass()>UsableMass)
//CNM        qp_main();
      break;
      }
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }
    
//--------------------------------------------------------------------------

void QPrecipMain1::ClosureInfo() 
  { 
  if (NoFlwIOs()>0)
    {
    MN_Surge::ClosureInfo();
    if (m_Closure.DoFlows())
      {
      CClosureInfo &CI=m_Closure[0];
      //HtOut+=dThermalLoss - (bWithHOR ? dReactionHeat : 0.0);
      }
    }
  };

//--------------------------------------------------------------------------

flag QPrecipMain1::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  int xxx=(No-CBContext());
  switch (No-CBContext())
    {
    case 1: pS="W\tTemperature out of Range";          return 1;
    case 2: pS="W\tCaustic Conc out of Range";         return 1;
    case 3: pS="W\tCarbon to Sulphur out of Range";    return 1;
    case 4: pS="W\tChlorine to Carbon out of Range";   return 1;
    case 5: pS="W\tTOOC to Carbon out of Range";       return 1;
    case 6: pS="W\tA/C Convergence not Starting";      return 1;
    case 7: pS="W\tA/C not Converging";                return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================
