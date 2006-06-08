//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#include "sc_defs.h"
#define  __M_AREA_CPP
#include "m_area.h"
#include "scd_wm.h"
#include "dbgmngr.h"
#include "nrecipe.h"

#define dbgAreas       (1 || WITHDEBUG)

#if dbgAreas
static CDbgMngr dbgCalculations    ("Areas", "Calculations");
static CDbgMngr dbgCalculationsDbg ("Areas", "CalculationsDbg");
static CDbgMngr dbgDumpReports     ("Areas", "DumpReports");
#endif

//==========================================================================
//
//
//
//==========================================================================

const int ARSign[AR_Size] = 
  { 
  1,   // AR_Balance, 
  1,   // AR_BalanceRel, 
  +1,   // AR_LnksI, 
  -1,   // AR_LnksO, 
  -1,   // AR_Spills, 
  -1,   // AR_Vents, 
  +1,   // AR_LeaksI, 
  -1,   // AR_LeaksO, 
  +1,   // AR_Srcs, 
  -1,   // AR_Snks, 
  +1,   // AR_Makeups, 
  -1,   // AR_Bleeds, 
  -1,   // AR_AccumI, 
  +1,   // AR_AccumO, 
  };

const LPCTSTR ARName[AR_Size] = 
  { 
  "Balance",        // AR_Balance, 
  "Balance_Rel",    // AR_BalanceRel, 
  "Links In",       // AR_LnksI, 
  "Links Out",      // AR_LnksO, 
  "Spills",         // AR_Spills, 
  "Vents",          // AR_Vents, 
  "Leaks In",       // AR_LeaksI, 
  "Leaks Out",      // AR_LeaksO, 
  "Srcs",           // AR_Srcs, 
  "Snks",           // AR_Snks, 
  "Makeups",        // AR_Makeups, 
  "Bleeds",         // AR_Bleeds, 
  "Accumulation",   // AR_AccumI, 
  "Depletion",      // AR_AccumO, 
  };

//==========================================================================
//
//
//
//==========================================================================

//TODO What to do about Tearing of LeaksIn

MN_Area::MN_Area (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
MdlNode(pClass_, TagIn, pAttach, eAttach),
m_Content("Content", this, TOA_Embedded)
  {
  AttachClassInfo(nc_MSrcSnk|nc_Audit,NULL);
  m_PFeed=Std_P;

  m_Content.SetSQFlags(SQF_Editable|SQF_Source|SQF_CreateOK|SQF_CreateDsp, true);

  m_dwAuditOptions = 0;
  m_bDoAudit       = true;
  m_bForceAudit    = false;

  m_OvResults.ZeroFlows();

  for (int j=0; j<AR_Size; j++)
    m_nCounts[j]=0;

  m_iEvalPosn=200;
  };

// -------------------------------------------------------------------------

MN_Area::~MN_Area ()
  {
  };

// -------------------------------------------------------------------------

void MN_Area::ResetData(flag Complete)
  {
  for (int i=0; i<NoFlwIOs(); i++)
    for (int j=0; j<NIOFBs(i); j++)
      IOFB(i,j)->ResetData(Complete);
  }

// -------------------------------------------------------------------------

void MN_Area::BuildDataDefn(DataDefnBlk & DDB, byte What)
  {
  Strng S;
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  if (!m_bForceAudit)
    {
    DDB.Text  ("");
    DDB.CheckBoxBtn("AuditOn",   "",    DC_,    "",  &m_bDoAudit,       this, isParm);
    }

  DDB.Text  ("");
  DDB.Double ("Temperature",  "T",    DC_T,    "C",      xidTemp,       this, isResult|noFile|noSnap);
  DDB.Double ("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,        this, isResult|noFile|noSnap);
  DDB.Double ("Pressure",     "P",    DC_P,    "kPag",   xidPMean,      this, isResult|noFile|noSnap);

  DDB.Visibility(SHM_All);
  DDB.Text  ("");
  DDB.Double("P_Rqd",         "",    DC_P   , "kPag",   xidEnvPRqd,     this, isParm|NAN_OK);

  DDB.Visibility(SHM_All);
  DDB.Double("T_Rqd",         "",    DC_T   , "C",      xidEnvTRqd,     this, isParm|NAN_OK);

  BuildDataDefnElevation(DDB);

  //CDirectFlwIO::sm_
  if (m_bDoAudit && gs_EvalAudits || m_bForceAudit)
    {
    if (What & BDDAO_Flows)
      {
      if (DDB.BeginStruct(this, "Flows", NULL, DDB_RqdPage))
        {
        if (DDB.BeginStruct(this, "Mass", NULL, DDB_NoPage))
          {
          DDB.Double ("",   "Spills",       DC_Qm,   "kg/s",  xidSpillsInQm,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Vents",        DC_Qm,   "kg/s",  xidVentsInQm,    this, isResult|noFile|noSnap); 
          DDB.Double ("",   "LeaksIn",      DC_Qm,   "kg/s",  xidLeaksInQm,    this, isResult|noFile|noSnap); 
          DDB.Double ("",   "LeaksOut",     DC_Qm,   "kg/s",  xidLeaksOutQm,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Srcs",         DC_Qm,   "kg/s",  xidSrcsInQm,     this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Snks",         DC_Qm,   "kg/s",  xidSnksInQm,     this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Makeups",      DC_Qm,   "kg/s",  xidMakeupsInQm,  this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Bleeds",       DC_Qm,   "kg/s",  xidBleedsInQm,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Accumulation", DC_Qm,   "kg/s",  xidAccumsInQm,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Depletion",    DC_Qm,   "kg/s",  xidAccumsOutQm,  this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Balance",      DC_Qm,   "kg/s",  xidNettInQm,     this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Balance.Rel",  DC_Frac, "%",     xidNettInQmRel,  this, isResult|noFile|noSnap); 
          }
        DDB.EndStruct();

        DDB.Text(" ");
        if (DDB.BeginStruct(this, "Heat", NULL, DDB_NoPage))
          {
          DDB.Double ("",   "Spills",       DC_Pwr,   "kJ/s", xidSpillsInHz,    this, isResult|noFile|noSnap);
          DDB.Double ("",   "Vents",        DC_Pwr,   "kJ/s", xidVentsInHz,     this, isResult|noFile|noSnap);
          DDB.Double ("",   "LeaksIn",      DC_Pwr,   "kJ/s", xidLeaksInHz,     this, isResult|noFile|noSnap);
          DDB.Double ("",   "LeaksOut",     DC_Pwr,   "kJ/s", xidLeaksOutHz,    this, isResult|noFile|noSnap);
          DDB.Double ("",   "Srcs",         DC_Pwr,   "kJ/s", xidSrcsInHz,      this, isResult|noFile|noSnap);
          DDB.Double ("",   "Snks",         DC_Pwr,   "kJ/s", xidSnksInHz,      this, isResult|noFile|noSnap);
          DDB.Double ("",   "Makeups",      DC_Pwr,   "kJ/s", xidMakeupsInHz,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Bleeds",       DC_Pwr,   "kJ/s", xidBleedsInHz,    this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Accumulation", DC_Pwr,   "kJ/s", xidAccumsInHz,    this, isResult|noFile|noSnap);
          DDB.Double ("",   "Depletion",    DC_Pwr,   "kJ/s", xidAccumsOutHz,   this, isResult|noFile|noSnap);
          DDB.Double ("",   "Balance",      DC_Pwr,   "kJ/s", xidNettInHz,      this, isResult|noFile|noSnap);
          DDB.Double ("",   "Balance.Rel",  DC_Frac,  "%",    xidNettInHzRel,   this, isResult|noFile|noSnap);
          }
        DDB.EndStruct();
        }
      DDB.EndStruct();
      }

    if (What & BDDAO_Totals)
      {
      DDB.Text(" ");
      if (DDB.BeginStruct(this, "Totals", NULL, DDB_RqdPage))
        {
        if (DDB.BeginStruct(this, "Mass", NULL, DDB_NoPage))
          {
          DDB.Double ("",   "Spills",       DC_M,     "kg", xidSpillsInQmSum,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Vents",        DC_M,     "kg", xidVentsInQmSum,    this, isResult|noFile|noSnap); 
          DDB.Double ("",   "LeaksIn",      DC_M,     "kg", xidLeaksInQmSum,    this, isResult|noFile|noSnap); 
          DDB.Double ("",   "LeaksOut",     DC_M,     "kg", xidLeaksOutQmSum,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Srcs",         DC_M,     "kg", xidSrcsInQmSum,     this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Snks",         DC_M,     "kg", xidSnksInQmSum,     this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Makeups",      DC_M,     "kg", xidMakeupsInQmSum,  this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Bleeds",       DC_M,     "kg", xidBleedsInQmSum,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Accumulation", DC_M,     "kg", xidAccumsInQmSum,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Depletion",    DC_M,     "kg", xidAccumsOutQmSum,  this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Balance",      DC_M,     "kg", xidNettInQmSum,     this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Balance.Rel",  DC_Frac,  "%",  xidNettInQmSumRel,  this, isResult|noFile|noSnap); 
          }
        DDB.EndStruct();

        DDB.Text(" ");
        if (DDB.BeginStruct(this, "Heat", NULL, DDB_NoPage))
          {
          DDB.Double ("",   "Spills",       DC_E,     "kJ", xidSpillsInHzSum,   this, isResult|noFile|noSnap);
          DDB.Double ("",   "Vents",        DC_E,     "kJ", xidVentsInHzSum,    this, isResult|noFile|noSnap);
          DDB.Double ("",   "LeaksIn",      DC_E,     "kJ", xidLeaksInHzSum,    this, isResult|noFile|noSnap);
          DDB.Double ("",   "LeaksOut",     DC_E,     "kJ", xidLeaksOutHzSum,   this, isResult|noFile|noSnap);
          DDB.Double ("",   "Srcs",         DC_E,     "kJ", xidSrcsInHzSum,     this, isResult|noFile|noSnap);
          DDB.Double ("",   "Snks",         DC_E,     "kJ", xidSnksInHzSum,     this, isResult|noFile|noSnap);
          DDB.Double ("",   "Makeups",      DC_E,     "kJ", xidMakeupsInHzSum,  this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Bleeds",       DC_E,     "kJ", xidBleedsInHzSum,   this, isResult|noFile|noSnap); 
          DDB.Double ("",   "Accumulation", DC_E,     "kJ", xidAccumsInHzSum,   this, isResult|noFile|noSnap);
          DDB.Double ("",   "Depletion",    DC_E,     "kJ", xidAccumsOutHzSum,  this, isResult|noFile|noSnap);
          DDB.Double ("",   "Balance",      DC_E,     "kJ", xidNettInHzSum,     this, isResult|noFile|noSnap);
          DDB.Double ("",   "Balance.Rel",  DC_Frac,  "%",  xidNettInHzSumRel,  this, isResult|noFile|noSnap);
          }
        DDB.EndStruct();
        }
      DDB.EndStruct();
      }
    }

  if (What & (BDDAO_Flows|BDDAO_Totals))
    {

    DDB.Text(" ");
    if (DDB.BeginStruct(this, "Counts", NULL, DDB_RqdPage))
      {
      DDB.Long   ("",   "Spills",   DC_,     "",       xidSpillsInCnt,  this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "Vents",    DC_,     "",       xidVentsInCnt,   this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "LeaksIn",  DC_,     "",       xidLeaksInCnt,   this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "LeaksOut", DC_,     "",       xidLeaksOutCnt,  this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "Srcs",     DC_,     "",       xidSrcsInCnt,    this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "Snks",     DC_,     "",       xidSnksInCnt,    this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "Makeups",  DC_,     "",       xidMakeupsInCnt, this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "Bleeds",   DC_,     "",       xidBleedsInCnt,  this, isResult|noFile|noSnap); 
      DDB.Long   ("",   "Accums",   DC_,     "",       xidAccumsInCnt,  this, isResult|noFile|noSnap); 
      //DDB.Long   ("",   "Nett",     DC_,     "",       xidNettInCnt,    this, isResult|noFile|noSnap); 
      }
    DDB.EndStruct();
    }
  DDB.Text(" ");
  DDB.Text("Audit Clipboard options:");
  DDB.Button("Copy_All",       "", DC_, "", xidIO_CopyAdtAll, this, isParm);
  DDB.Button("Copy_Mass",      "", DC_, "", xidIO_CopyAdtMass, this, isParm);
  DDB.Button("Copy_MassTotal", "", DC_, "", xidIO_CopyAdtMassTotal, this, isParm);
  DDB.Button("Copy_Heat",      "", DC_, "", xidIO_CopyAdtHeat, this, isParm);
  DDB.Button("Copy_HeatTotal", "", DC_, "", xidIO_CopyAdtHeatTotal, this, isParm);

  if (What & BDDAO_Contents)
    {
    DDB.Visibility();
    DDB.Object(&m_Content,  this, NULL, NULL, DDB_RqdPage);
    if (0)
      {
      //DDB.Object(&m_SrcsI,   this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_SnksO,   this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_MakeupsI,this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_BleedsO, this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_SpillsO, this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_VentsO,  this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_LeaksI,  this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_OvResults.m_Sum[AR_LeaksO],  this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_AccI,    this, NULL, NULL, DDB_RqdPage, -1, isResult);
      //DDB.Object(&m_AccO,    this, NULL, NULL, DDB_RqdPage, -1, isResult);
      }
    }

  DDB.Text("");
  DDB.EndStruct();
  };  

// -------------------------------------------------------------------------


flag MN_Area::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMakeupsInCnt:         DCB.L = m_nCounts[AR_Makeups];            return 1;
    case xidBleedsInCnt :         DCB.L = m_nCounts[AR_Bleeds];             return 1;
    case xidSpillsInCnt :         DCB.L = m_nCounts[AR_Spills];             return 1;
    case xidVentsInCnt  :         DCB.L = m_nCounts[AR_Vents];              return 1;
    case xidLeaksInCnt  :         DCB.L = m_nCounts[AR_LeaksI];              return 1;
    case xidLeaksOutCnt :         DCB.L = m_nCounts[AR_LeaksO];              return 1;
    case xidSrcsInCnt   :         DCB.L = m_nCounts[AR_Srcs];               return 1;
    case xidSnksInCnt   :         DCB.L = m_nCounts[AR_Snks];               return 1;
    case xidAccumsInCnt :         DCB.L = m_nCounts[AR_AccumI];                return 1;

    case xidMakeupsInQm:          DCB.D = m_OvResults.m_Sum[AR_Makeups].FlowMass;    return 1;
    case xidMakeupsInHz:          DCB.D = m_OvResults.m_Sum[AR_Makeups].FlowHz;      return 1;
    case xidMakeupsInQmSum:       DCB.D = m_OvResults.m_Sum[AR_Makeups].TotalMass;   return 1;
    case xidMakeupsInHzSum:       DCB.D = m_OvResults.m_Sum[AR_Makeups].TotalHz;     return 1;

    case xidBleedsInQm:           DCB.D = m_OvResults.m_Sum[AR_Bleeds].FlowMass;     return 1;
    case xidBleedsInHz:           DCB.D = m_OvResults.m_Sum[AR_Bleeds].FlowHz;       return 1;
    case xidBleedsInQmSum:        DCB.D = m_OvResults.m_Sum[AR_Bleeds].TotalMass;    return 1;
    case xidBleedsInHzSum:        DCB.D = m_OvResults.m_Sum[AR_Bleeds].TotalHz;      return 1;

    case xidSpillsInQm:           DCB.D = -m_OvResults.m_Sum[AR_Spills].FlowMass;    return 1;
    case xidSpillsInHz:           DCB.D = -m_OvResults.m_Sum[AR_Spills].FlowHz;      return 1;
    case xidSpillsInQmSum:        DCB.D = -m_OvResults.m_Sum[AR_Spills].TotalMass;   return 1;
    case xidSpillsInHzSum:        DCB.D = -m_OvResults.m_Sum[AR_Spills].TotalHz;     return 1;

    case xidVentsInQm:            DCB.D = -m_OvResults.m_Sum[AR_Vents].FlowMass;     return 1;
    case xidVentsInHz:            DCB.D = -m_OvResults.m_Sum[AR_Vents].FlowHz;       return 1;
    case xidVentsInQmSum:         DCB.D = -m_OvResults.m_Sum[AR_Vents].TotalMass;    return 1;
    case xidVentsInHzSum:         DCB.D = -m_OvResults.m_Sum[AR_Vents].TotalHz;      return 1;

    case xidLeaksInQm:            DCB.D = m_OvResults.m_Sum[AR_LeaksI].FlowMass;      return 1;
    case xidLeaksInHz:            DCB.D = m_OvResults.m_Sum[AR_LeaksI].FlowHz;        return 1;
    case xidLeaksInQmSum:         DCB.D = m_OvResults.m_Sum[AR_LeaksI].TotalMass;     return 1;
    case xidLeaksInHzSum:         DCB.D = m_OvResults.m_Sum[AR_LeaksI].TotalHz;       return 1;

    case xidLeaksOutQm:           DCB.D = -m_OvResults.m_Sum[AR_LeaksO].FlowMass;     return 1;
    case xidLeaksOutHz:           DCB.D = -m_OvResults.m_Sum[AR_LeaksO].FlowHz;       return 1;
    case xidLeaksOutQmSum:        DCB.D = -m_OvResults.m_Sum[AR_LeaksO].TotalMass;    return 1;
    case xidLeaksOutHzSum:        DCB.D = -m_OvResults.m_Sum[AR_LeaksO].TotalHz;      return 1;

    case xidSrcsInQm:             DCB.D = m_OvResults.m_Sum[AR_Srcs].FlowMass;       return 1;
    case xidSrcsInHz:             DCB.D = m_OvResults.m_Sum[AR_Srcs].FlowHz;         return 1;
    case xidSrcsInQmSum:          DCB.D = m_OvResults.m_Sum[AR_Srcs].TotalMass;      return 1;
    case xidSrcsInHzSum:          DCB.D = m_OvResults.m_Sum[AR_Srcs].TotalHz;        return 1;

    case xidSnksInQm:             DCB.D = -m_OvResults.m_Sum[AR_Snks].FlowMass;      return 1;
    case xidSnksInHz:             DCB.D = -m_OvResults.m_Sum[AR_Snks].FlowHz;        return 1;
    case xidSnksInQmSum:          DCB.D = -m_OvResults.m_Sum[AR_Snks].TotalMass;     return 1;
    case xidSnksInHzSum:          DCB.D = -m_OvResults.m_Sum[AR_Snks].TotalHz;       return 1;

    case xidAccumsInQm:           DCB.D = -m_OvResults.m_Sum[AR_AccumI].FlowMass;       return 1;
    case xidAccumsInHz:           DCB.D = -m_OvResults.m_Sum[AR_AccumI].FlowHz;         return 1;
    case xidAccumsInQmSum:        DCB.D = -m_OvResults.m_Sum[AR_AccumI].TotalMass;      return 1;
    case xidAccumsInHzSum:        DCB.D = -m_OvResults.m_Sum[AR_AccumI].TotalHz;        return 1;

    case xidAccumsOutQm:          DCB.D = m_OvResults.m_Sum[AR_AccumO].FlowMass;       return 1;
    case xidAccumsOutHz:          DCB.D = m_OvResults.m_Sum[AR_AccumO].FlowHz;         return 1;
    case xidAccumsOutQmSum:       DCB.D = m_OvResults.m_Sum[AR_AccumO].TotalMass;      return 1;
    case xidAccumsOutHzSum:       DCB.D = m_OvResults.m_Sum[AR_AccumO].TotalHz;        return 1;

    case xidNettInQm:             DCB.D = m_OvResults.FlowMass();   return 1;
    case xidNettInHz:             DCB.D = m_OvResults.FlowHz();     return 1;   
    case xidNettInQmSum:          DCB.D = m_OvResults.TotalMass();  return 1;  
    case xidNettInHzSum:          DCB.D = m_OvResults.TotalHz();    return 1;  

    case xidNettInQmRel:          DCB.D = m_OvResults.FlowMass()  / GTZ(m_OvResults.FlowMassMax());   return 1;
    case xidNettInHzRel:          DCB.D = m_OvResults.FlowHz()    / GTZ(m_OvResults.FlowHzMax());     return 1;
    case xidNettInQmSumRel:       DCB.D = m_OvResults.TotalMass() / GTZ(m_OvResults.TotalMassMax());  return 1;
    case xidNettInHzSumRel:       DCB.D = m_OvResults.TotalHz()   / GTZ(m_OvResults.TotalHzMax());    return 1;

    case xidIO_CopyAdtAll         :
    case xidIO_CopyAdtMass        :
    case xidIO_CopyAdtMassTotal   :
    case xidIO_CopyAdtHeat        :
    case xidIO_CopyAdtHeatTotal   :
      {
      DWORD Options=0;
      switch (DCB.lHandle)
        {
        case xidIO_CopyAdtAll         :  Options =    SUB_NA_ADT_MASS      |
                                           SUB_NA_ADT_MASSTOTAL |
                                           SUB_NA_ADT_HEAT      |
                                           SUB_NA_ADT_HEATTOTAL ; break;
        case xidIO_CopyAdtMass        :  Options =    SUB_NA_ADT_MASS      ; break;
        case xidIO_CopyAdtMassTotal   :  Options =    SUB_NA_ADT_MASSTOTAL ; break;
        case xidIO_CopyAdtHeat        :  Options =    SUB_NA_ADT_HEAT      ; break;
        case xidIO_CopyAdtHeatTotal   :  Options =    SUB_NA_ADT_HEATTOTAL ; break;
        }
      if (DCB.ForView() && DCB.rB)
        {
        char* pTag = new char[strlen(FullObjTag())+1];
        strcpy(pTag, FullObjTag());
        AfxGetMainWnd()->PostMessage(WMU_NODEAUDIT, Options, (LPARAM)pTag);
        }
      DCB.B=0;
      return 1;
      }


    case xidEnvTRqd:
      if (DCB.rD && Finite(*DCB.rD))
        m_Content.SetTRqdTo(Range(CDB.MinT, *DCB.rD, CDB.MaxT));
      DCB.D=m_Content.TRqd();
      return 1;
    case xidEnvPRqd:
      if (DCB.rD && Finite(*DCB.rD))
        m_Content.SetPRqdTo(Range(1.0, *DCB.rD, 1.0e10));
      DCB.D=m_Content.PRqd();
      return 1;

    }

  if (MdlNode::DataXchg(DCB))
    return 1;

  return 0;
  }

// -------------------------------------------------------------------------

flag MN_Area::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);

  if (!Valid(m_Content.PRqd()))
    {
    LogError(FullObjTag(), 0, "Invalid Pressure");
    m_Content.SetPRqdTo(Std_P);
    }

  EvalState();

  return OK;
  };

// -------------------------------------------------------------------------

long MN_Area::CheckDataRanges(bool CheckIO)
  {
  return MdlNode::CheckDataRanges((m_bDoAudit && gs_EvalAudits || m_bForceAudit) && CheckIO);
  }

// -------------------------------------------------------------------------

flag MN_Area::PreStartCheck()
  {
  flag Ok=true;
  if (m_Content.Model())
    Ok=m_Content.Model()->Class()->LicOK();
  if (!Ok)
    LogError(FullObjTag(), 0, "Specie model '%s' not enabled by licensing", m_Content.Model()->Class()->ShortDesc());
  return MdlNode::PreStartCheck();
  };

//--------------------------------------------------------------------------

flag MN_Area::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  //MdlAction M0(0, MAT_Value, SSMode&SSM_ConstP && fabs(m_Content.PRqd()-Std_P)>0.1,
  //                "Pressure (kPa)", false,
  //                (m_Content.PRqd()-Std_P)*P_RegB.Output(), 0.0, m_Content.PRqd()-Std_P);

  Acts.SetSize(0);
  //Acts.SetAtGrow(0, M0);
  //Acts.SetAtGrow(1, M1);
  //Acts.SetAtGrow(2, M2);
  //if (sConnectedTag.Len()>0)
  //  Acts.SetAtGrow(3, M3);
  return true;
  };

//--------------------------------------------------------------------------

flag MN_Area::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  //switch (Act.iIndex)
  //  {
  //  case 0:
  //    P_RegB.SetOutput(Act.dValue/GTZ(m_Content.PRqd()-Std_P));
  //    break;
  //  }
  return true;
  };

//--------------------------------------------------------------------------

long MN_Area::NodeFlwTask(NodeFlwTasks Task)
  {
  return MdlNode::NodeFlwTask(Task);
  }

//---------------------------------------------------------------------------

void MN_Area::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);
  //IOFB(IONo,0)->AssignFlwEqnGroup(NullFlwGroup, NullFlwGroup.Default(), this);
  };

//---------------------------------------------------------------------------

void MN_Area::PreDisConnect(int IONo)
  {
  };

//--------------------------------------------------------------------------

Z_States MN_Area::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  return Z_Unknown;//SetDatums_Node(Pass, NULL, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

void MN_Area::StartSolution()
  {
  }

//--------------------------------------------------------------------------

void MN_Area::StartStep()
  {
  MdlNode::StartStep();
  }

//---------------------------------------------------------------------------

void MN_Area::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    {
    switch (IOId_Self(i))
      {
      case IOId_AreaSpillI:
      case IOId_AreaVentI:
      case IOId_AreaLeakI:
      case IOId_AreaLeakO:
      case IOId_AreaBleedI:
      case IOId_AreaMakeupO:
        SetIO_Open(i, 0, false, ESS_Allowed);
        break;
      default:
        SetIO_Closed(i, 0);
        break;
      }
    }
  };

//--------------------------------------------------------------------------

eEvalSeqStart MN_Area::IOEvalSeqStart_Self(int i)
  { 
  if (IOId_Self(i)==IOId_AreaLeakI)
    {
    if (IOQm_Out(i)>0.0)
      return ESS_Required;
    return ESS_Allowed;
    }
  return MdlNode::IOEvalSeqStart_Self(i);
  }

//--------------------------------------------------------------------------

void MN_Area::EvalJoinPressures(long JoinMask)
  {
  const int N=NoFlwIOs();
  if (Valid(m_Content.PRqd()))
    m_Content.SetPress(PRequired());

  m_PFeed=m_Content.Press();
  for (int i = 0; i<N; i++)
    Set_IOP_Self(i,m_PFeed);
  };

//--------------------------------------------------------------------------

flag MN_Area::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  FlwBlk *pFB = IOFB(IONo,0);

  if (Task==FET_GetMode)
    {
    pFB->SetXferMode();
    switch (IOId_Self(IONo))
      {
      case IOId_AreaMakeupO:
        pFB->SetXferMode();
        pFB->SetXferCapacity(-LargePosFlow);
        return true;
      case IOId_AreaBleedI:
      case IOId_AreaSpillI:
      case IOId_AreaVentI:
        pFB->SetXferMode();
        pFB->SetXferCapacity(LargePosFlow);
        return true;
      case IOId_AreaLeakI:
      case IOId_AreaLeakO:
        pFB->SetQmFree();
        return true;
      default:
        pFB->SetXferMode();
        return true;
      }
    //flag EqnOK=pFB->EvaluateFlwEqn(Task, pProps, true, false, 1.0, NULL, NULL);
    //return EqnOK || pFB->m_fFixed;
    }
  else
    {
    flag EqnOK=pFB->EvaluateFlwEqn(Task, pProps, true, false, 1.0, NULL, NULL);
    return EqnOK || pFB->m_fFixed;
    }
  };

//--------------------------------------------------------------------------

void MN_Area::EvalState()
  {
  };

//--------------------------------------------------------------------------

void MN_Area::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  //  InRange = InRange && m_OvResults.m_Sum[AR_Srcs].SMFnsInRange() && m_OvResults.m_Sum[AR_Srcs].SMFnsInRange();
  //  InRange = InRange && m_OvResults.m_Sum[AR_Snks].SMFnsInRange() && m_OvResults.m_Sum[AR_Snks].SMFnsInRange();
  //  InRange = InRange && m_OvResults.m_Sum[AR_Spills].SMFnsInRange() && m_OvResults.m_Sum[AR_Spills].SMFnsInRange();
  //  InRange = InRange && m_OvResults.m_Sum[AR_LeaksI].SMFnsInRange() && m_OvResults.m_Sum[AR_LeaksO].SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    for (int i=0;i<NoFlwIOs(); i++)
      {
      SpConduit &C=*IOConduit(i);
      if (IO_In(i))
        {
        CI.m_HfLoss   += C.totHf();
        CI.m_HsLoss   += C.totHs();
        CI.m_MassLoss += C.QMass();
        }
      else if (IO_Out(i))
        {
        CI.m_HfGain   += C.totHf();
        CI.m_HsGain   += C.totHs();
        CI.m_MassGain += C.QMass();
        }
      }
    }
  };

//--------------------------------------------------------------------------

void MN_Area::DoEvalProducts(flag Validating, double dTime)
  {
  }

// --------------------------------------------------------------------------

void MN_Area::EvalProductsInit(EvalProductsInitTasks Task)
  {
  //DoEvalProducts(false, ICGetTimeInc());
  for (int i=0;i<NoFlwIOs(); i++)
    {
    if (!IO_In(i))
      EvalProductsInit_Source(Task, i, IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas));
    }

  if (Valid(m_Content.TRqd()))
    m_Content.SetTemp(TRequired());


  switch (SolveMethod())
    {
    case SM_Inline:
    case SM_Buffered:
      {
      for (int i=0;i<NoFlwIOs(); i++)
        {
        switch (IOId_Self(i))
          {
          case IOId_AreaLeakO:
          case IOId_AreaMakeupO:
            {
            double Qo = IOQm_Out(i);
            SpConduit &Q = *IOConduit(i);
            if (Qo>=0.0)
              {
              Q.QSetM(m_Content, som_ALL, Qo, IOP_Flng(i));
              Q.SetTempPress(TRequired(), IOP_Flng(i));
              }
            break;
            }
          }
        }
      break;
      }
    }

  }

//--------------------------------------------------------------------------

void MN_Area::EvalProducts(long JoinMask)
  {
  DoEvalProducts(false, ICGetTimeInc());
  }

//NBNB The Area Tears and or EvalOrder needs thought

//--------------------------------------------------------------------------

void MN_Area::EvalDerivs(long JoinMask)
  {
  }

//--------------------------------------------------------------------------

//void MN_Area::PostEvalDerivs()
//  {
//  }

//--------------------------------------------------------------------------

void MN_Area::EvalDiscrete()
  {
  }

//--------------------------------------------------------------------------

void MN_Area::EvalAudit(int Pass)
  {
  MdlNode::EvalAudit(Pass);
  switch (Pass)
    {
    case 0:
      {
      //// Integrate All DirectIOs
      //POSITION Pos=m_NdsInArea.GetHeadPosition();
      //while (Pos)
      //  {
      //  FlwNode &Nd=*m_NdsInArea.GetNext(Pos);

      //  //for (int io=0; io<Nd.NoProcessIOs(); io++)
      //  //  {
      //  //  double Qm=Sign(Nd.IOQm_In(io))*Nd.IOConduit(io)->QMass();
      //  //  double Hz=Sign(Nd.IOQm_In(io))*Nd.IOConduit(io)->totHz();
      //  //  }
      //
      //  int i=0;
      //  POSITION DPos=Nd.m_DirectIOs.GetHeadPosition();
      //  while (DPos)
      //    {
      //    CDirectFlwIO &D=*m_DirectIOs.GetNext(DPos);
      //    FlwNode &DNd=D.Nd;

      //    if (D.Connected && (DNd.m_pArea==this))
      //      {
      //      if (D.IsIO)
      //        D.Sum.Set(D.Cd);
      //      D.Sum.Integrate(ICGetTimeInc());
      //      }
      //    }
      //  }

      //POSITION DPos=m_DirectIOsConnected.GetHeadPosition();
      //while (DPos)
      //  {
      //  CDirectFlwIO &D=*m_DirectIOsConnected.GetNext(DPos);
      //  FlwNode &DNd=D.Nd;
      //  if (D.Connected && (&DNd.Area!=this) && (&D.Area==this))
      //    D.Sum.Integrate(ICGetTimeInc());
      //  }

      break;
      }  
    case 1:
      {
      m_OvResults.ZeroFlows();

      if (!(m_bDoAudit && gs_EvalAudits || m_bForceAudit)) 
        break;

      for (int j=0; j<AR_Size; j++)
        m_nCounts[j]=0;

#if dbgAreas
      if (dbgCalculations() || dbgCalculationsDbg())
        dbgpln("Area : ========================================================================================= %s", FullObjTag());
#endif

      int iNd=0;
      POSITION Pos=m_NdsInArea.GetHeadPosition();
      while (Pos)
        {
        if (iNd<=m_NdResults.GetSize())
          m_NdResults.Add(new CSumResult);

        CSumResult &NdR=*m_NdResults[iNd];

        FlwNode &Nd=*m_NdsInArea.GetNext(Pos);

        NdR.m_pNd=&Nd;
        NdR.ZeroFlows();

#if dbgAreas
        if (dbgCalculations() || dbgCalculationsDbg() && Nd.DoDbgBrk())
          dbgpln("------------------------------------------------------------------------------------------------ %s [%s]", Nd.FullObjTag(), Nd.ClassId());
#endif

        for (int io=0; io<Nd.NoProcessIOs(); io++)
          {
          if (Nd.IOQm_In(io)>0)
            {
            m_OvResults.m_Sum[AR_LnksI].Add(*Nd.IOAreaSum(io));
            NdR.m_Sum[AR_LnksI].Add(*Nd.IOAreaSum(io));
            }
          else if (Nd.IOQm_Out(io)>0)
            {
            m_OvResults.m_Sum[AR_LnksO].Add(*Nd.IOAreaSum(io));
            NdR.m_Sum[AR_LnksO].Add(*Nd.IOAreaSum(io));
            }

#if dbgAreas
          if (dbgCalculations() || dbgCalculationsDbg() && Nd.DoDbgBrk())
            {
            //if (Nd.Nd_Rmt(io))
            dbgpln("FlowIO:%2i %2s %20s %20s %12s %12s     %-10s : %s", io, "", 
              DbgFltString(Nd.IOAreaSum(io)->FlowMass,7,10), 
              DbgFltString(Nd.IOAreaSum(io)->FlowHz),
              "","",Nd.IODesc_Self(io)->pName, Nd.Nd_Rmt(io)->FullObjTag());
            }
#endif
          }

        int i=0;
        for (int d=0; i<Nd.NoDirectIOs(); d++)
          {
          CDirectFlwIO &D=*Nd.m_DirectIOs[d];
          FlwNode &DNd=D.Nd;

          if (D.Connected && (DNd.m_pArea==this))
            {

#if dbgAreas
            if (dbgCalculations() || dbgCalculationsDbg() && DNd.DoDbgBrk())
              {
              //dbgpln("DrctIO:%2i %2i %20s %20s %15s.%s", i, D.Type, DbgFltString(D.Cd.QMass(),7,10), DbgFltString(D.Cd.FlowHz()), D.Tag, TheNd->FullObjTag());
              dbgpln("DrctIO:%2i %2i %20s %20s %12s %12s     %-10s : %s [%s]", 
                i, D.Type, DbgFltString(D.Sum.FlowMass,7,10), DbgFltString(D.Sum.FlowHz), 
                "", "", D.Tag, DNd.FullObjTag(), DNd.ClassId());
              }
#endif
            switch (D.Type)
              {
              case eDIO_Makeup:
                m_nCounts[AR_Makeups]++;
                m_OvResults.m_Sum[AR_Makeups].Add(D.Sum);
                NdR.m_Sum[AR_Makeups].Add(D.Sum);
                break;
              case eDIO_Bleed:
                m_nCounts[AR_Bleeds]++;
                m_OvResults.m_Sum[AR_Bleeds].Add(D.Sum);
                NdR.m_Sum[AR_Bleeds].Add(D.Sum);
                break;
              case eDIO_Spill:
                m_nCounts[AR_Spills]++;
                m_OvResults.m_Sum[AR_Spills].Add(D.Sum);
                NdR.m_Sum[AR_Spills].Add(D.Sum);
                break;
              case eDIO_Vent:
                m_nCounts[AR_Vents]++;
                m_OvResults.m_Sum[AR_Vents].Add(D.Sum);
                NdR.m_Sum[AR_Vents].Add(D.Sum);
                break;
              case eDIO_Leak:
                if (Sign(DNd.IOQm_In(D.MyConnectedIO()))>=0)
                  {
                  m_nCounts[AR_LeaksI]++;
                  m_OvResults.m_Sum[AR_LeaksI].Add(D.Sum);
                  NdR.m_Sum[AR_LeaksI].Add(D.Sum);
                  }   
                else
                  {
                  m_nCounts[AR_LeaksO]++;
                  m_OvResults.m_Sum[AR_LeaksO].Add(D.Sum);
                  NdR.m_Sum[AR_LeaksO].Add(D.Sum);
                  }
                break;
              case eDIO_Src:
                m_nCounts[AR_Srcs]++;
                m_OvResults.m_Sum[AR_Srcs].Add(D.Sum);
                NdR.m_Sum[AR_Srcs].Add(D.Sum);
                break;
              case eDIO_Snk:
                m_nCounts[AR_Snks]++;
                m_OvResults.m_Sum[AR_Snks].Add(D.Sum);
                NdR.m_Sum[AR_Snks].Add(D.Sum);
                break;
              case eDIO_Accum:  
                m_nCounts[AR_AccumI]++;
                m_OvResults.m_Sum[AR_AccumI].Add(D.Sum);
                NdR.m_Sum[AR_AccumI].Add(D.Sum);
                break;
              case eDIO_Deplete: 
                m_nCounts[AR_AccumO]++;
                m_OvResults.m_Sum[AR_AccumO].Add(D.Sum);
                NdR.m_Sum[AR_AccumO].Add(D.Sum);
                break;
              }
            }
          i++;
          }
        iNd++;        
        };


      bool DoErrorMsg=true;
      int i=0;
      POSITION DPos=m_DirectIOsConnected.GetHeadPosition();
      while (DPos)
        {
        CDirectFlwIO &D=*m_DirectIOsConnected.GetNext(DPos);
        FlwNode &DNd=D.Nd;

        int iNd;
        for (iNd=m_NdResults.GetSize()-1; iNd>=0; iNd--)
          if (m_NdResults[iNd]->m_pNd==&DNd)
            break;

        if (iNd<0)
          iNd=m_NdResults.Add(new CSumResult);

        CSumResult &NdR=*m_NdResults[iNd];

        if (D.Connected && (&DNd.Area!=this) && (&D.Area==this))
          {
          if (DoErrorMsg)
            LogError(FullObjTag(), 0, "Area : the following code is still to be checked");
          DoErrorMsg=false;
          //ASSERT_ALWAYS(false, "Area : the following code is still to be checked");
#if dbgAreas
          if (dbgCalculations() || dbgCalculationsDbg() && DNd.DoDbgBrk())
            //dbgpln("DrctIO:%2i %2i %20s %20s %15s.%s", i, D.Type, DbgFltString(D.Sum.QMass(),7,10), DbgFltString(D.Sum.FlowHz()), D.Tag, TheNd->FullObjTag());
            dbgpln("DrctIO:%2i %2i %20s %20s %12s %12s   %-10s : %s [%s]", 
            i, D.Type, DbgFltString(D.Sum.FlowMass,7,10), DbgFltString(D.Sum.FlowHz),
            "", "", D.Tag, DNd.FullObjTag(), DNd.ClassId());
#endif
          switch (D.Type)
            {
            case eDIO_Makeup:
              m_nCounts[AR_Makeups]++;
              m_OvResults.m_Sum[AR_Makeups].Add(D.Sum);
              NdR.m_Sum[AR_Makeups].Add(D.Sum);
              break;
            case eDIO_Bleed:
              m_nCounts[AR_Bleeds]++;
              m_OvResults.m_Sum[AR_Bleeds].Add(D.Sum);
              NdR.m_Sum[AR_Bleeds].Add(D.Sum);
              break;
            case eDIO_Spill:
              m_nCounts[AR_Spills]++;
              m_OvResults.m_Sum[AR_Spills].Add(D.Sum);
              NdR.m_Sum[AR_Spills].Add(D.Sum);
              break;
            case eDIO_Vent:
              m_nCounts[AR_Vents]++;
              m_OvResults.m_Sum[AR_Vents].Add(D.Sum);
              NdR.m_Sum[AR_Vents].Add(D.Sum);
              break;
            case eDIO_Leak:
              if (Sign(DNd.IOQm_In(D.MyConnectedIO()))>=0)
                {
                m_nCounts[AR_LeaksI]++;
                m_OvResults.m_Sum[AR_LeaksI].Add(D.Sum);
                NdR.m_Sum[AR_LeaksI].Add(D.Sum);
                }   
              else
                {
                m_nCounts[AR_LeaksO]++;
                m_OvResults.m_Sum[AR_LeaksO].Add(D.Sum);
                NdR.m_Sum[AR_LeaksO].Add(D.Sum);
                }
              break;
            case eDIO_Src:
              m_nCounts[AR_Srcs]++;
              m_OvResults.m_Sum[AR_Srcs].Add(D.Sum);
              NdR.m_Sum[AR_Srcs].Add(D.Sum);
              break;
            case eDIO_Snk:
              m_nCounts[AR_Snks]++;
              m_OvResults.m_Sum[AR_Snks].Add(D.Sum);
              NdR.m_Sum[AR_Snks].Add(D.Sum);
              break;
            case eDIO_Accum:  
              m_nCounts[AR_AccumI]++;
              m_OvResults.m_Sum[AR_AccumI].Add(D.Sum);
              NdR.m_Sum[AR_AccumI].Add(D.Sum);
              break;
            case eDIO_Deplete: 
              m_nCounts[AR_AccumO]++;
              m_OvResults.m_Sum[AR_AccumO].Add(D.Sum);
              NdR.m_Sum[AR_AccumO].Add(D.Sum);
              break;
            }

          //OvTotalM+=SUMALL(m_Nd, getFlowMass);
          //OvTotalH+=SUMALL(m_Nd, getFlowHz);
          }
        }

      for (int j=iNd; j<m_NdResults.GetSize(); j++)
        delete m_NdResults[j]; 
      m_NdResults.SetSize(iNd);

#if dbgAreas
      if (dbgDumpReports())
        {
        const LPCTSTR PassName[4]={"Mass Flows", "Heat Flows", "Mass Total Flows", "Heat Totals Flows"};
        for (int iPass=0; iPass<4; iPass++)
          {
          dbgpln("%s================================================================================================================", PassName[iPass]);
          dbgp("%s","No");
          dbgp("\t%s","Tag");
          for (int j=0; j<AR_Size; j++)
            dbgp("\t%s", ARName[j]);
          dbgpln("\t%s", "");
          int iNd;
          for (iNd=0; iNd<m_NdResults.GetSize(); iNd++)
            {
            CSumResult &R=*m_NdResults[iNd];
            dbgp("%i",iNd);
            dbgp("\t%s", R.m_pNd->FullObjTag());
            //Strng BalFrm("=Sum(RC[%i]:RC[%i])", 0-AR_Balance, AR_AccumO-AR_Balance);
            //Strng BalRelFrm("=RC[-1]/Max(abs(RC[%i]),abs(RC[%i]))",AR_LnksI-AR_Balance,AR_LnksO-AR_Balance);
            for (int j=0; j<AR_Size; j++)
              {
              double D=0;
              switch (iPass)            
                {
                case 0: D=R.m_Sum[j].FlowMass;  break;
                case 1: D=R.m_Sum[j].FlowHz;    break;
                case 2: D=R.m_Sum[j].TotalMass; break;
                case 3: D=R.m_Sum[j].TotalHz;   break;
                }
              dbgp(iPass<=AR_BalanceRel || fabs(D)>1e-30 ? "\t%.20g":"\t", ARSign[j]*D);
              }
            //dbgp("\t=Sum(RC[%i]:RC[%i])", 0-AR_Balance, AR_AccumO-AR_Balance);
            //dbgp("\t=RC[-1]/Max(1e-30,abs(RC[%i]),abs(RC[%i]))",AR_LnksI-AR_BalanceRel,AR_LnksO-AR_BalanceRel);
            dbgpln("");
            }

          dbgp("%s","No");
          dbgp("\t%s","Tag");
          for (int j=0; j<AR_Size; j++)
            dbgp("\t%s", ARName[j]);
          dbgpln("\t%s", "");

          dbgp("%s","");
          dbgp("\t%s","Totals");
          for (int j=0; j<AR_Size; j++)
            dbgp("\t=Sum(R[%i]C:R[%i]C)",-iNd-1,-1);
          //dbgp("\t=Sum(RC[%i]:RC[%i])", 0-AR_Balance, AR_AccumO-AR_Balance);
          //dbgp("\t=RC[-1]/Max(1e-30,abs(RC[%i]),abs(RC[%i]))",AR_LnksI-AR_BalanceRel,AR_LnksO-AR_BalanceRel);
          dbgpln("");

          dbgpln("================================================================================================================");
          }
        }
#endif
      break;
      }
    }
  }

//--------------------------------------------------------------------------


static int TestErrorM(void * p, void * q) 
  { 
  return (fabs(((CSumResult*)p)->m_Sum[AR_Balance].FlowMass)>fabs(((CSumResult*)q)->m_Sum[AR_Balance].FlowMass));
  }

static int TestErrorMT(void * p, void * q) 
  { 
  return (fabs(((CSumResult*)p)->m_Sum[AR_Balance].TotalMass)>fabs(((CSumResult*)q)->m_Sum[AR_Balance].TotalMass));
  }

static int TestErrorH(void * p, void * q) 
  { 
  return (fabs(((CSumResult*)p)->m_Sum[AR_Balance].FlowHz)>fabs(((CSumResult*)q)->m_Sum[AR_Balance].FlowHz));
  }

static int TestErrorHT(void * p, void * q) 
  { 
  return (fabs(((CSumResult*)p)->m_Sum[AR_Balance].TotalHz)>fabs(((CSumResult*)q)->m_Sum[AR_Balance].TotalHz));
  }

void MN_Area::GetBalanceReport(CNodeAuditInfo & Audit)
  {
  CString &S=Audit.m_Report;

  CArray<CSumResult*, CSumResult*> Results;
  Results.SetSize(0,256);
  for (int iNd=0; iNd<m_NdResults.GetSize(); iNd++)
    Results.Add(m_NdResults[iNd]);

  const dword RqdOptions[4] = { SUB_NA_ADT_MASS, SUB_NA_ADT_MASSTOTAL, SUB_NA_ADT_HEAT, SUB_NA_ADT_HEATTOTAL };
  const LPCTSTR PassName[4] = { "Mass Flows", "Mass Total Flows", "Heat Flows", "Heat Totals Flows"};
  for (int iPass=0; iPass<4; iPass++)
    {
    if ((Audit.m_Options & RqdOptions[iPass])==0)
      continue;

    S.AppendFormat("%s","====");
    S.AppendFormat("\t%s","==========");
    for (int j=0; j<AR_Size; j++)
      S.AppendFormat("\t%s", "==========");
    S.AppendFormat("\t%s\r\n", "");
    //S.AppendFormat("%s ================================================================================================================\r\n", PassName[iPass]);
    S.AppendFormat("Area:\t%s\r\n", FullObjTag());
    S.AppendFormat("Audit:\t%s\r\n", PassName[iPass]);
    S.AppendFormat("%s","No");
    S.AppendFormat("\t%s","Tag");
    //S.AppendFormat("\t%s", ARName[AR_Balance]);
    //S.AppendFormat("\t%s", ARName[AR_BalanceRel]);
    for (int j=0; j<AR_Size; j++)
      S.AppendFormat("\t%s", ARName[j]);
    //for (int j=0; j<AR_Size; j++)
    //  S.AppendFormat("\t%s", ARName[j]);
    S.AppendFormat("\t%s\r\n", "");
    int iNd;

    if (Results.GetSize())
      {
      for (iNd=0; iNd<Results.GetSize(); iNd++)
        {
        CSumResult &Res=*Results[iNd];
        switch (iPass)            
          {
          case 0: 
            {
            double &B = Res.m_Sum[AR_Balance].m_FlowMass;
            double &R = Res.m_Sum[AR_BalanceRel].m_FlowMass;
            double F = Res.FlowMass();  
            double I = Res.m_Sum[AR_LnksI].FlowMass;  
            double O = Res.m_Sum[AR_LnksO].FlowMass;  
            B = I-O+F;
            double Mx=Max(fabs(I), fabs(O));
            if (fabs(B)<1e-14*Mx)
              B=0;
            R = B/Max(1e-30, Mx);
            break;
            }
          case 1: 
            {
            double &B = Res.m_Sum[AR_Balance].m_TotalMass;
            double &R = Res.m_Sum[AR_BalanceRel].m_TotalMass;
            double F = Res.TotalMass();  
            double I = Res.m_Sum[AR_LnksI].TotalMass;  
            double O = Res.m_Sum[AR_LnksO].TotalMass;  
            B = I-O+F;
            double Mx=Max(fabs(I), fabs(O));
            if (fabs(B)<1e-14*Mx)
              B=0;
            R = B/Max(1e-30, Mx);
            break;
            }
          case 2: 
            {
            double &B = Res.m_Sum[AR_Balance].m_FlowHz;
            double &R = Res.m_Sum[AR_BalanceRel].m_FlowHz;
            double F = Res.FlowHz();  
            double I = Res.m_Sum[AR_LnksI].FlowHz;  
            double O = Res.m_Sum[AR_LnksO].FlowHz;  
            B = I-O+F;
            double Mx=Max(fabs(I), fabs(O));
            if (fabs(B)<1e-14*Mx)
              B=0;
            R = B/Max(1e-30, Mx);
            break;
            }
          case 3: 
            {
            double &B = Res.m_Sum[AR_Balance].m_TotalHz;
            double &R = Res.m_Sum[AR_BalanceRel].m_TotalHz;
            double F = Res.TotalHz();  
            double I = Res.m_Sum[AR_LnksI].TotalHz;  
            double O = Res.m_Sum[AR_LnksO].TotalHz;  
            B = I-O+F;
            double Mx=Max(fabs(I), fabs(O));
            if (fabs(B)<1e-14*Mx)
              B=0;
            R = B/Max(1e-30, Mx);
            break;
            }
          }
        }

      //for (iNd=0; iNd<Results.GetSize(); iNd++)
      //  dbgpln("B: %16.3f %16.3f %16.3f %16.3f %s", 
      //    Results[iNd]->m_Sum[AR_Balance].FlowMass,  
      //    Results[iNd]->m_Sum[AR_Balance].TotalMass, 
      //    Results[iNd]->m_Sum[AR_Balance].FlowHz,    
      //    Results[iNd]->m_Sum[AR_Balance].TotalHz,   
      //    Results[iNd]->m_pNd->FullObjTag());

      switch (iPass)            
        {
        case 0: HpSort(Results.GetSize(), (void**)&Results.ElementAt(0), TestErrorM);   break;
        case 1: HpSort(Results.GetSize(), (void**)&Results.ElementAt(0), TestErrorMT);  break;
        case 2: HpSort(Results.GetSize(), (void**)&Results.ElementAt(0), TestErrorH);   break;
        case 3: HpSort(Results.GetSize(), (void**)&Results.ElementAt(0), TestErrorHT);  break;
        }
      //for (iNd=0; iNd<Results.GetSize(); iNd++)
      //  dbgpln("A: %16.3f %16.3f %16.3f %16.3f %s", 
      //    Results[iNd]->m_Sum[AR_Balance].FlowMass,  
      //    Results[iNd]->m_Sum[AR_Balance].TotalMass, 
      //    Results[iNd]->m_Sum[AR_Balance].FlowHz,    
      //    Results[iNd]->m_Sum[AR_Balance].TotalHz,   
      //    Results[iNd]->m_pNd->FullObjTag());

      for (iNd=0; iNd<Results.GetSize(); iNd++)
        {
        CSumResult &Res=*Results[iNd];
        S.AppendFormat("%i",iNd);
        S.AppendFormat("\t%s", Res.m_pNd->FullObjTag());
        for (int j=0; j<AR_Size; j++)
          {
          double D=0;
          switch (iPass)            
            {
            case 0: D=Res.m_Sum[j].FlowMass;  break;
            case 1: D=Res.m_Sum[j].TotalMass; break;
            case 2: D=Res.m_Sum[j].FlowHz;    break;
            case 3: D=Res.m_Sum[j].TotalHz;   break;
            }
          S.AppendFormat(iPass<=AR_BalanceRel || fabs(D)>1e-30 ? "\t%.20g":"\t", ARSign[j]*D);
          }
        S.AppendFormat("\r\n");
        }
      }

    S.AppendFormat("%s","====");
    S.AppendFormat("\t%s","==========");
    for (int j=0; j<AR_Size; j++)
      S.AppendFormat("\t%s", "==========");
    S.AppendFormat("\t%s\r\n", "");

    S.AppendFormat("%s","");
    S.AppendFormat("\t%s","Totals");
    S.AppendFormat("\t=Sum(RC[%i]:RC[%i])", 2,2+(AR_Size-AR_LnksI)-1);
    S.AppendFormat("\t=RC[-1]/Max(1e-30,abs(RC[%i]),abs(RC[%i]))",1,2);
    if (Results.GetSize())
      {
      for (int j=AR_BalanceRel+1; j<AR_Size; j++)
        S.AppendFormat("\t=Sum(R[%i]C:R[%i]C)",-iNd-1,-1);
      }
    else
      {
      for (int j=0; j<AR_Size; j++)
        S.AppendFormat("\t0");
      }
    //S.AppendFormat("\t=Sum(RC[%i]:RC[%i])", 0-AR_Balance, AR_AccumO-AR_Balance);
    //S.AppendFormat("\t=RC[-1]/Max(1e-30,abs(RC[%i]),abs(RC[%i]))",AR_LnksI-AR_BalanceRel,AR_LnksO-AR_BalanceRel);
    S.AppendFormat("\r\n");

    S.AppendFormat("%s","====");
    S.AppendFormat("\t%s","==========");
    for (int j=0; j<AR_Size; j++)
      S.AppendFormat("\t%s", "==========");
    S.AppendFormat("\t%s\r\n", "");
    //S.AppendFormat("%.*s================================================================================================================\r\n", strlen(PassName[iPass])+1, "=========================");
    //S.AppendFormat("\r\n");
    S.AppendFormat("\r\n");
    S.AppendFormat("\r\n");
    }
  };

//--------------------------------------------------------------------------

flag MN_Area::InitialiseSolution()
  {
  MdlNode::InitialiseSolution();
  return 1;
  };

//--------------------------------------------------------------------------

flag MN_Area::TerminateSolution()
  {

  return 1;
  };

//--------------------------------------------------------------------------

void MN_Area::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_Empty:
    case MSA_PreSet:
    case MSA_ZeroFlows:
    case MSA_EmptySpillTargets:
      //case MSA_SteadyState:
      {
      m_OvResults.ZeroFlows();
      for (int i=0; i<m_NdResults.GetCount(); i++)
        m_NdResults[i]->ZeroFlows();
      }
    case MSA_DynStatsRunInit:
      {
      m_OvResults.ZeroTotals();
      for (int i=0; i<m_NdResults.GetCount(); i++)
        m_NdResults[i]->ZeroTotals();
      break;
      }
    }
  }

//--------------------------------------------------------------------------

double MN_Area::TRequired()
  {
  if (Valid(m_Content.TRqd()))
    {
    double T = m_Content.TRqd();
    m_Content.SetTemp(T);
    return T;
    }
  else
    return m_Content.Temp();
  };

//--------------------------------------------------------------------------

double MN_Area::PRequired()
  {
  if (Valid(m_Content.PRqd()))
    {
    double P = m_Content.PRqd();
    m_Content.SetPress(P);
    return P;
    }
  else
    return m_Content.Press();
  }

//--------------------------------------------------------------------------

dword  MN_Area::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();

  int HasFlw=0;
  double TFlw=0.0;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    TFlw+=IOQm_In(i);
    if (IOConduit(i)->QMass()>UsableMass)
      HasFlw=1;
    }
  Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
  if (NoFlwIOs()==1)
    {
    if (TFlw>1.0e-6)
      Status |= FNS_IsSnk;
    else if (TFlw<-1.0e-6)
      Status |= FNS_IsSrc;
    }

  return Status;
  };

//--------------------------------------------------------------------------

flag MN_Area::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="E\t???"; return 1;
    case 2: pS="W\t???"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

