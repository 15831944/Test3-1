//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __XFER_CPP
#include "xfer.h"
#include "models.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Tie(1)
#X:#h<General Description>#nThis enables pipes to be split eg doubles as 
a T-piece or cross piece, depending upon its connections.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
IO       Input/Output       2              20#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Z_Reqd> : Datum.#n
#i<General_Model> : This can be used to switch the #j<general split model>#d<General Model>
on. If it is on then the associated variables must be configured.#n
#n#n
#h<Associated variables>#n
#i<P> : The pressure of the material within the unit.#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#n
#n#h<Other>#n
Default model prefix:X#n
Short name:Tie#n
Model type:Unit#n
#G:Units
*/
//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec X_TieIOAreaList[] =
  {{"IOPts",    "IO",   0,              LIO_InOut,     nc_MLnk, 2, 50, IOOptsHide|IOPipeJoin|IOGRP(1)},
  {"Inputs",   "In",   0,              LIO_In0,       nc_MLnk, 2, 25, IOOptsHide|IOPipeJoin|IOGRP(1)},
  {"Outputs",  "Out",  0,              LIO_Out0,      nc_MLnk, 2, 25, IOOptsHide|IOPipeJoin|IOGRP(1)},
  //"Leak",     "Leak", IOId_XferLeak,  LIO_Out,       nc_MLnk, 0,  1, IOOptsHide|IOEqnsHide|IOPipeJoin|IOHidden},
  LEAKS2AREA("Leak", IOId_XferLeak),
  MAKEUP2AREA(),
  BLEED2AREA(),
  {NULL}};

double Drw_X_Tie[] = { DD_Poly, -1.5,1.5, 1.5,1.5, 1.5,-1.5, -1.5,-1.5, -1.5,1.5,
                       DD_TagPos, 0, -5,
                       DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(X_Tie, "Tie", "1", Drw_X_Tie, "MixerSplitter", "X", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT, 
                    "Process:Piping:Tie(1)",
                    "General Purpose Tie Piece used to Simulate Tee & Cross pieces") 

                    X_Tie::X_Tie(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MN_Xfer(pClass_, TagIn, pAttach, eAttach)//,
  {
  AttachIOAreas(X_TieIOAreaList, &TeeEntryGroup);
  //Initialise();
  Joins.SetSize(1); // PreInit to Allow access to Leak Information
  //Joins[0].Leak.Qm=0.0;
  m_bShowQFeed = 1;
  m_bShowQProd = 1;

#if WITHNODEPROCS
  SetProcsAllowed(1);
#endif

#if XREFTEST 
  m_bTestTagIO = 0;
  m_iQm2=-1;
  m_iQm1=-1;
#endif

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(X_Tie), 0, mmio_CONNECT, &typeid(CFT_Mixer));
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model.*/
void X_Tie::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text   ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Double  ("PressureEst",  "PEst",  DC_P,    "kPag",   xidPEstMean,    this, isResult|0);
  DDB.Text   ("");
  BuildDataDefnElevation(DDB);

  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Text   ("");
  m_PCtrl0.BuildDataDefn(DDB, this, NetProbalMethod(), PC_BasicPB, PC_BasicDyn);//PC_Atmos|PC_Reqd);
  DDB.Visibility();

  if (NetDynamicMethod())
    {
    DDB.Text("");
    DDB.CheckBoxBtn("NetworkIsolator",  "",  DC_, "", xidNetworkIsolator,   this, isParmStopped|SetOnChange|AffectsStruct);
    }

  if (!m_NetworkIsolator)
    {
    m_BlkEval.Add_OnOff(DDB);  
    if (PrjFileVerNo()<97 && (DDB.DoingPutData() && DDB.ForFileSnpScn() || 0))
      {                      
      DDB.Text("");
      m_RB.Add_OnOff(DDB);
      m_EHX.Add_OnOff(DDB);
      }

    DDB.Visibility(SM_Buffered|HM_All);
    m_Leak.Add_OnOff(DDB,0,100);
    DDB.Visibility();
    GSM.Add_OnOff(DDB);
    DDB.CheckBox("ShowQFeed",             "",  DC_, "", &m_bShowQFeed,         this, isParm|SetOnChange);
    DDB.CheckBox("ShowQProd",             "",  DC_, "", &m_bShowQProd,         this, isParm|SetOnChange);
    }

#if XREFTEST
  DDB.CheckBox("TagIOTest",             "",  DC_, "", &m_bTestTagIO,         this, isParm|SetOnChange);
#endif

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  if (m_NetworkIsolator)
    {
    DDB.Visibility();
    DDB.Object(m_Accumulator, this, NULL, NULL, DDB_RqdPage);
    if (m_bShowQFeed)
      DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);
    m_QProd(); // ensure exists
    if (m_QProd.Exists())
      {
      DDB.Visibility(NSHM_All, m_bShowQProd);
      DDB.Object(&m_QProd, this, NULL, NULL, DDB_RqdPage);
      DDB.Visibility();
      }
    }
  else //if (1)//NetProbalMethod())
    {
    DDB.Text("");
    m_BlkEval.BuildDataDefn(DDB);
    GSM.BuildDataDefn(DDB);
    m_QFeed(); // ensure exists
    if (m_QFeed.Exists())
      {
      DDB.Visibility(NSHM_All, m_bShowQFeed);
      DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);
      DDB.Visibility();
      }
    m_QProd(); // ensure exists
    if (m_QProd.Exists())
      {
      DDB.Visibility(NSHM_All, m_bShowQProd);
      DDB.Object(&m_QProd, this, NULL, NULL, DDB_RqdPage);
      DDB.Visibility();
      }
    }


  //BuildDataDefnAreaIOs(DDB);

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag X_Tie::DataXchg(DataChangeBlk & DCB)
  {
  if (m_BlkEval.DataXchg(DCB))
    return 1;

  if (MN_Xfer::DataXchg(DCB))
    return 1;

  return 0;
  }

// -------------------------------------------------------------------------
#if XREFTEST
// -------------------------------------------------------------------------

flag X_Tie::ValidateData(ValidateDataBlk & VDB)
  {
  bool OK=true;
  if (m_bTestTagIO)
    {
    if (!GetTagIOReqd())
      {
      SetTagIOReqd(true, 100); 

      m_pTagIO->Add("P_1.Qm (kg/s)",                        "AName",  TIO_Get);
      m_pTagIO->Add("P_1.Qv (L/s)",                         "BName",  TIO_Get);
      m_pTagIO->Add("P_1.NQv (NL/s)",                       "",       TIO_Get);
      m_pTagIO->Add("P_1.Ti (C)",                           "",       TIO_Get);
      m_pTagIO->Add("P_1.To (K)",                           "",       TIO_Get);
      m_pTagIO->Add("P_1.To (C)",                           "",       TIO_Get);
      m_pTagIO->Add("XXXXX",                                "",       TIO_Get);

      m_iQm2 = m_pTagIO->Add("P_2.Qm (kg/s)",               "",       TIO_Get);
      m_iQm1 = m_pTagIO->Add("Cold_Water.Qm_Rqd (t/h)",     "",       TIO_Set);

      m_pTagIO->Remove("XXXXX");

      OK = (m_iQm2>=0 && m_iQm1>=0);
      }
    }
  else
    SetTagIOReqd(false, 100);

  return OK && MN_Xfer::ValidateData(VDB);
  }

// -------------------------------------------------------------------------

void X_Tie::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (TagIOOn())
    {
   // =Some simple control !!
    m_pTagIO->SetDValue(m_iQm1, 2.0*m_pTagIO->GetDValue(m_iQm2));
    }

  MN_Xfer::EvalProducts(NEI);
  };

// -------------------------------------------------------------------------
#endif
//==========================================================================
//
//
//
//==========================================================================
