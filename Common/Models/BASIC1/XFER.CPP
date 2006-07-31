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
    MAKEUP2AREA("Makeups", IOId_Makeup2Area),
    BLEED2AREA("Bleeds", IOId_Bleed2Area),
   {NULL}};

double Drw_X_Tie[] = { DD_Poly, -1,1, 1,1, 1,-1, -1,-1, -1,1,
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
  m_PBPCtrl0.BuildDataDefn(DDB, this, PBPC_Basic);
  DDB.Visibility();

  if (NetDynamicMethod())
    {
    DDB.Text("");
    DDB.CheckBoxBtn("NetworkIsolator",  "",  DC_, "", xidNetworkIsolator,   this, isParm|SetOnChange);
    }
  if (!m_NetworkIsolator)
    {
#if (WITHBLOCKEVALUATOR)
    m_BlkEval.Add_OnOff(DDB);  
    if (PrjFileVerNo()<97 && (DDB.DoingPutData() && DDB.ForFileSnpScn() || 0))
#else
    if (1)
#endif
      {                      
      DDB.Text("");
      m_RB.Add_OnOff(DDB);
      EHX.Add_OnOff(DDB);
      }

    DDB.Visibility(SM_Buffered|HM_All);
    m_Leak.Add_OnOff(DDB,0,100);
    DDB.Visibility();
    GSM.Add_OnOff(DDB);
    DDB.CheckBox("ShowQFeed",             "",  DC_, "", &m_bShowQFeed,         this, isParm|SetOnChange);
    }

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  if (m_NetworkIsolator)
    {
    DDB.Visibility();
    DDB.Object(m_Accumulator, this, NULL, NULL, DDB_RqdPage);
    if (m_bShowQFeed)
    if (m_bShowQFeed)
      DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_QProd, this, NULL, NULL, DDB_RqdPage);
    }
  else //if (1)//NetProbalMethod())
    {
    DDB.Text("");
#if (WITHBLOCKEVALUATOR)
    m_BlkEval.BuildDataDefn(DDB);
#else
    m_RB.BuildDataDefn(DDB);
    EHX.BuildDataDefn(DDB);
#endif
    GSM.BuildDataDefn(DDB);
    m_QFeed(); // ensure exists
    if (m_QFeed.Exists())
      {
      DDB.Visibility(NSHM_All, m_bShowQFeed);
      DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);
      DDB.Visibility();
      }
    }


  if (m_Leak.Enabled)
    {
    DDB.Page("AreaIO", DDB_RqdPage);
    m_Leak.BuildDataDefn(DDB, NULL, DDB_NoPage, 100, DFIO_ShowQm);
    }

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag X_Tie::DataXchg(DataChangeBlk & DCB)
  {
//  for (int i=0; i<NoFlwIOs(); i++)
//    if (DCB.dwUserInfo==i+1 && IOFB(i,0)->DataXchg(DCB))
//      return 1;

#if (WITHBLOCKEVALUATOR)
  if (m_BlkEval.DataXchg(DCB))
    return 1;
#endif

  if (MN_Xfer::DataXchg(DCB))
    return 1;

  return 0;
  }

//==========================================================================
//
//
//
//==========================================================================
