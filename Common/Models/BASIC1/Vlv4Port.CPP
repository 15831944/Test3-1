//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __VLV4PORT_CPP
#include "vlv4port.h"
#include "scd_wm.h"
#include "dbgmngr.h"

//#include "optoff.h"

#define  dbg4Port   (WITHDEBUG)

#if (dbg4Port)
static CDbgMngr dbgLoad     ("4PortValve", "Load");
static CDbgMngr dbgOperate  ("4PortValve", "Operate");
#endif

static LPCTSTR ConfigFileName = "$Prj\\Connections.s4p";
static LPCTSTR ConfigFileContents =
"[Names]                                                                            \r\n"
"Name0=Standard                                                                     \r\n"
"                                                                                   \r\n"
";       Mode,  Name,             { Join1Conns },{ Join2Conns}, Into,From Time(s)   \r\n"
"[Standard]                                                                         \r\n"
"Mode0  =   0,  \"Off\",          {            },{           },  0.0, 0.0           \r\n"
"Mode1  =   1,  \"On:1-2-3-4\",   { 1,2,3,4    },{           },  0.0, 0.0           \r\n"
"Mode2  =   2,  \"On:1-2, 3-4\",  { 1,2        },{ 3,4       },  0.0, 0.0           \r\n"
"Mode3  =   3,  \"On:1-3, 2-4\",  { 1,3        },{ 2,4       },  0.0, 0.0           \r\n"
"Mode4  =   4,  \"On:1-4, 2-3\",  { 1,4        },{ 2,3       },  0.0, 0.0           \r\n"
"                                                                                   \r\n"
";          DPMult                                                                  \r\n"
"Port1  =   1.0                                                                     \r\n"
"Port2  =   1.0                                                                     \r\n"
"Port3  =   1.0                                                                     \r\n"
"Port4  =   1.0                                                                     \r\n"
"                                                                                   \r\n"
"; >= 0 implies all closed for this time                                            \r\n"
"ClosedTime = 0                                                                     \r\n"
"                                                                                   \r\n"
"; Example of Profile (Remove Comments)                                             \r\n"
";                          	/   Port > Jn1   \\     /   Port > Jn2   \\             \r\n"
";                 Position        1    2    3    4       1    2    3    4          \r\n"
";Profile0.Name =   FCV-03-43-01                                                    \r\n"
";Profile0.Pt0  =   0.0,        { 1.0, 1.0, 0.0, 0.0 },{ 0.0, 0.0, 0.0, 0.0  }      \r\n"
";Profile0.Pt1  =   0.5,        { 1.0, 1.0, 0.0, 1.0 },{ 0.0, 0.0, 0.0, 0.0  }      \r\n"
";Profile0.Pt2  =   1.0,        { 0.0, 1.0, 1.0, 1.0 },{ 0.0, 0.0, 0.0, 0.0  }      \r\n"
";Profile1.Name =   FCV-XX-XX-XX                                                    \r\n"
";Profile1.Pt0  =   0.0,        { 1.0, 1.0, 1.0, 0.0 },{ 0.0, 0.0, 0.0, 0.0  }      \r\n"
";Profile1.Pt1  =   0.5,        { 1.0, 1.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 0.0  }      \r\n"
";Profile1.Pt2  =   1.0,        { 0.0, 1.0, 1.0, 1.0 },{ 0.0, 0.0, 0.0, 0.0  }      \r\n"
"                                                                                   \r\n";


XID xidValveConfig        = ModelXID(1);
XID xidValveModeAct       = ModelXID(2);
XID xidValveModeRqd       = ModelXID(3);
XID xidValveModeForce     = ModelXID(4);
XID xid4WayName           = ModelXID(5);
XID xidEdit4WayBtn        = ModelXID(6);
XID xidReload4WayBtn      = ModelXID(7);
XID xidBrowse4WayBtn      = ModelXID(8);
XID xidPress1             = ModelXID(9);
XID xidPress2             = ModelXID(10);
XID xidPortStr1           = ModelXID(11);
XID xidPortStr2           = ModelXID(12);
XID xidPortStr3           = ModelXID(13);
XID xidPortStr4           = ModelXID(14);
XID xidPortOpn1           = ModelXID(15);
XID xidPortOpn2           = ModelXID(16);
XID xidPortOpn3           = ModelXID(17);
XID xidPortOpn4           = ModelXID(18);
XID xidConnStr1           = ModelXID(20);
XID xidConnStr10          = ModelXID(21);
XID xidConnStr11          = ModelXID(22);
XID xidConnStr12          = ModelXID(23);
XID xidConnStr13          = ModelXID(24);
XID xidConnStr2           = ModelXID(25);
XID xidConnStr20          = ModelXID(26);
XID xidConnStr21          = ModelXID(27);
XID xidConnStr22          = ModelXID(28);
XID xidConnStr23          = ModelXID(29);
XID xidStateMethod        = ModelXID(40);
XID xidStateEnable        = ModelXID(41);
XID xidStateOn            = ModelXID(42);
XID xidStateOffMode       = ModelXID(43);
XID xidStateOnMode        = ModelXID(44);
XID xidStateActualPosn    = ModelXID(45);
XID xidStateReqdPosn      = ModelXID(46);
XID xidStateProfile       = ModelXID(47);
//XID xidStateMidPtMode     = ModelXID(44);

const byte  Meth_None     = 0;
const byte  Meth_OnOff    = 1;
const byte  Meth_Profile  = 2;

static IOAreaRec Vlv4PortIOAreaList[] =
  {
    {"Port1",  "Port1",  0,              LIO_InOut,     nc_MLnk, 0, 1, IOOptsHide|IOPipeJoin, 0.0f, { -5,  2, -2, -2}},
    {"Port2",  "Port2",  1,              LIO_InOut,     nc_MLnk, 0, 1, IOOptsHide|IOPipeJoin, 0.0f, {  2,  2,  5, -2}},
    {"Port3",  "Port3",  2,              LIO_InOut,     nc_MLnk, 0, 1, IOOptsHide|IOPipeJoin, 0.0f, { -2,  5,  2,  2}},
    {"Port4",  "Port4",  3,              LIO_InOut,     nc_MLnk, 0, 1, IOOptsHide|IOPipeJoin, 0.0f, { -2, -2,  2, -5}},
    //{"Leak",     "Leak", IOId_XferLeak,  LIO_Out,       nc_MLnk, 0,  1, IOOptsHide|IOEqnsHide|IOPipeJoin|IOHidden},
    {NULL}
  };

double Drw_Vlv4Port[] = { DD_Poly, -3,-1, -3,1, -1,1, -1,3, 1,3, 1,1, 3,1, 3,-1, 1,-1, 1,-3, -1,-3, -1,-1, -3,-1,
DD_End };

//const long Max4WayIOPorts=4;

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(C4PortValve , "Vlv4Port", "1", Drw_Vlv4Port, "Valve4Port", "X", TOC_ALL|TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Piping:Vlv4Port(1)",
                    "Four Port Valve");

//===========================================================================

C4PortValve ::C4PortValve (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MdlNode(pClass_, TagIn, pAttach, eAttach),
m_FEP(NULL, FBOpt_AsValve, 1.0)
  {
  AttachClassInfo(nc_Process, Vlv4PortIOAreaList);
  m_FEP.AssignFlwEqnGroup(Vlv4PortGroup, Vlv4PortGroup.Default()/*"VE_Line"*/, this);
  m_FEP.SetOptions(FBDDOpt_WithDPSpec, 1.0);

  Joins.SetSize(1);

  m_iVlvModeAct         = 0;
  m_iVlvModeRqd         = 0;
  //m_iVlvModeMidPt       = -1;// -1;  should be -1
  m_iVlvModeStrs        = -1;
  m_OpnWRT50            = 1.0;

  m_nInterSteps         = 0;
  m_iTrans              = 0;
  m_TmInStep            = 0;

  m_State.m_iMethod     = 0;
  m_State.m_bOn         = 0;
  m_State.m_iOffMode    = 0;
  m_State.m_iOnMode     = 1;
  m_State.m_ActualPosn  = 0.0;
  m_State.m_ReqdPosn    = dNAN;

  m_iProfile            = -1;

  m_CfgFileTime.dwHighDateTime=0;
  m_CfgFileTime.dwLowDateTime=0;
  m_sConfigFileC=ConfigFileName;
  //m_sConfigFileX.FnExpand(m_sConfigFileC());
  if (!FileExists(ConfigFileX()))
    CreateConfigFile(ConfigFileX());
  m_sConfig="Standard";
  LoadVlvConfigurations();

  m_bErrorFound=false;

  // Initialise Mode Masks
  //if (Modes[1].m_lMask==0)
  //  {
  //  for (int m=0; m<MaxModes; m++)
  //    {
  //    long Mm=0;
  //    for (int i=0; i<Max4WayIOPorts; i++)
  //      if (Modes[m].m_PortsJoin[i]>=0)
  //        Mm|=(1<<(i+4*Modes[m].m_PortsJoin[i]));
  //    Modes[m].m_lMask=Mm;
  //    //dbgpln("%4i %4i", m, Mm);
  //    }
  //  }
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model.*/
void C4PortValve ::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  //DDB.Text   ("");
  //DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
  //DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  //DDB.Double  ("PressureEst",  "PEst",  DC_P,    "kPag",   xidPEstMean,    this, isResult|0);
  //DDB.Text   ("");
  BuildDataDefnElevation(DDB);

  DDB.Text(" ");
  DDBValueLst EmptyLst[] = { {0} };
  DDBValueLst * CfgList = m_ConfigNames.Length()>0?m_ConfigNames.List():EmptyLst;
  DDBValueLst * ModeList = m_ModeStrs.Length()>0?m_ModeStrs.List():EmptyLst;
  DDBValueLst * ProfileList = m_ProfileNames.Length()>0?m_ProfileNames.List():EmptyLst;

  //DDBValueLst * ModeListX = m_ModeStrsX.Length()>0?m_ModeStrsX.List():EmptyLst;

  // These buttons should follow the 4WayName
  DDB.String("CfgFile",     "", DC_,    "",   xid4WayName,               this, isParm);
  DDB.Button("EditCfg",     "", DC_,    "",   xidEdit4WayBtn,            this, isParm);
  DDB.Button("ReloadCfg",   "", DC_,    "",   xidReload4WayBtn,          this, isParm);
  DDB.Button("BrowseCfg",   "", DC_,    "",   xidBrowse4WayBtn,          this, isParm);

  DDB.Page("Connects", DDB_RqdPage);
  //DDB.Text(" ");
  DDB.String("Configuration", "", DC_,  "",   xidValveConfig,            this, isParm|SM_DynBoth|SetOnChange, CfgList);
  DDB.Long  ("TransSteps",  "", DC_,    "",   &m_nInterSteps,            this, SM_DynBoth|isParm);
  //DDB.Button("Mode.Force",  "", DC_,    "",   xidValveModeForce,         this, isParm|SM_DynBoth|SetOnChange);
  DDB.Long  ("TransCnt",    "", DC_,    "",   &m_iTrans,                 this, SM_DynBoth|InitHidden);
  DDB.Double("TmInStep",    "", DC_Time, "s", &m_TmInStep,               this, SM_DynBoth|InitHidden);
  DDB.Double("TransPosn",   "", DC_Frac, "%", &m_OpnWRT50,               this, SM_DynBoth|InitHidden);
  DDB.Text("");
  if (OrigPrjFileVerNo()<=90 &&  DDB.ForFiling())
    {
    DDB.BeginStruct(this, "State");
    DDB.CheckBoxBtn("Enable", "", DC_,    "",   xidStateEnable,            this, isParm|SM_DynBoth);
    DDB.Visibility(NSHM_All, m_State.m_iMethod==Meth_OnOff);
    DDB.CheckBoxBtn("On",     "", DC_,    "",   xidStateOn,                this, isParm|SM_DynBoth);
    DDB.Long  ("Off.Mode",    "", DC_,    "",   xidStateOffMode,           this, isParm|SM_DynBoth|SetOnChange, ModeList);
    DDB.Long  ("On.Mode",     "", DC_,    "",   xidStateOnMode,            this, isParm|SM_DynBoth|SetOnChange, ModeList);
    //DDB.Long  ("MidPt.Mode",  "", DC_,    "",   xidStateMidPtMode,         this, isParm|SM_DynBoth|SetOnChange, ModeListX);
    DDB.EndStruct();
    }


  DDB.BeginStruct(this, "Posn");
  if (OrigPrjFileVerNo()<=93 && DDB.ForFiling() && DDB.DoingPutData()) // to go after Coors
    DDB.CheckBoxBtn("Enable", "", DC_,    "",   xidStateEnable,            this, isParm|SM_DynBoth);

  DDBValueLstMem PosnMeth;
  PosnMeth.Add(Meth_None, "Mode");
  PosnMeth.Add(Meth_OnOff, "On/Off");
  if (m_Profiles.GetCount())
    PosnMeth.Add(Meth_Profile, "Profile");
  DDB.Byte("Method", "", DC_,    "",   xidStateMethod,                   this, isParm|SM_DynBoth|SetOnChange, PosnMeth());

  DDB.Visibility(NSHM_All, m_State.m_iMethod==Meth_None);
  DDB.Long  ("Mode.Rqd",    "", DC_,    "",   xidValveModeRqd,           this, (m_State.m_iMethod==Meth_None?isParm:0)|SM_DynBoth|SetOnChange, ModeList);
  DDB.Long  ("Mode.Act",    "", DC_,    "",   xidValveModeAct,           this, SM_DynBoth, ModeList);
  DDB.Visibility(NSHM_All, m_State.m_iMethod!=Meth_None);
  DDB.Double("Actual",      "", DC_Frac,  "%", xidStateActualPosn,       this, SM_DynBoth);
  DDB.Double("Reqd",        "", DC_Frac,  "%", xidStateReqdPosn,         this, isParm|SM_DynBoth|NAN_OK);
  DDB.Visibility(NSHM_All, m_State.m_iMethod==Meth_OnOff);
  DDB.Long  ("Off.Mode",    "", DC_,    "",   xidStateOffMode,           this, isParm|SM_DynBoth|SetOnChange, ModeList);
  DDB.Long  ("On.Mode",     "", DC_,    "",   xidStateOnMode,            this, isParm|SM_DynBoth|SetOnChange, ModeList);
  DDB.CheckBoxBtn("On",     "", DC_,    "",   xidStateOn,                this, isParm|SM_DynBoth);
  DDB.Visibility(NSHM_All, m_State.m_iMethod==Meth_Profile);

  DDB.Long  ("Profile",     "", DC_,    "",   xidStateProfile,           this, isParm|SM_DynBoth|SetOnChange, ProfileList);

  DDB.Visibility();
  DDB.EndStruct();

  DDB.Text("");

  DDB.Text("Ports");
  DDB.String("Port1",       "", DC_, "",    xidPortStr1,      this, SM_DynBoth|isTag);
  DDB.String("Port2",       "", DC_, "",    xidPortStr2,      this, SM_DynBoth|isTag);
  DDB.String("Port3",       "", DC_, "",    xidPortStr3,      this, SM_DynBoth|isTag);
  DDB.String("Port4",       "", DC_, "",    xidPortStr4,      this, SM_DynBoth|isTag);

  DDB.Double("Port1Open",   "", DC_Frac, "%",    xidPortOpn1,  this, SM_DynBoth);
  DDB.Double("Port2Open",   "", DC_Frac, "%",    xidPortOpn2,  this, SM_DynBoth);
  DDB.Double("Port3Open",   "", DC_Frac, "%",    xidPortOpn3,  this, SM_DynBoth);
  DDB.Double("Port4Open",   "", DC_Frac, "%",    xidPortOpn4,  this, SM_DynBoth);

  DDB.Text("");
  DDB.BeginStruct(this, "Join1", NULL, DDB_NoPage);
  DDB.Double("Pressure", "", DC_P,  "kPag", xidPress1,        this, SM_DynBoth);
  DDB.String("Pipes",    "", DC_,   "",     xidConnStr1,      this, SM_DynBoth);
  DDB.String("Pipe1",    "", DC_,   "",     xidConnStr10,     this, SM_DynBoth|isTag);
  DDB.String("Pipe2",    "", DC_,   "",     xidConnStr11,     this, SM_DynBoth|isTag);
  DDB.String("Pipe3",    "", DC_,   "",     xidConnStr12,     this, SM_DynBoth|isTag);
  DDB.String("Pipe4",    "", DC_,   "",     xidConnStr13,     this, SM_DynBoth|isTag);
  DDB.EndStruct();                          
  DDB.BeginStruct(this, "Join2", NULL, DDB_NoPage);
  DDB.Double("Pressure", "", DC_P,  "kPag", xidPress2,        this, SM_DynBoth);
  DDB.String("Pipes",    "", DC_,   "",     xidConnStr2,      this, SM_DynBoth);
  DDB.String("Pipe1",    "", DC_,   "",     xidConnStr20,     this, SM_DynBoth|isTag);
  DDB.String("Pipe2",    "", DC_,   "",     xidConnStr21,     this, SM_DynBoth|isTag);
  DDB.String("Pipe3",    "", DC_,   "",     xidConnStr22,     this, SM_DynBoth|isTag);
  DDB.String("Pipe4",    "", DC_,   "",     xidConnStr23,     this, SM_DynBoth|isTag);
  DDB.EndStruct();

  DDB.Page  ("..");
  m_FEP.BuildDataDefn(FBDDOpt_WithEqn, DDB, this, NULL);

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag C4PortValve::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return true;

  if (m_FEP.DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xid4WayName:
      if (DCB.rpC)
        {
        m_sConfigFileC.FnContract(DCB.rpC);
        LoadVlvConfigurations();
        }
      DCB.pC=m_sConfigFileC();
      return 1;
    case xidBrowse4WayBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Ext("*.s4p");
        CSCDFileDialog Dlg(true, NULL, Ext(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "4 Port Valve Files (*.s4p)|*.s4p||");
        Dlg.m_ofn.lpstrInitialDir = PrjFiles();
        Dlg.m_ofn.lpstrTitle = "4 Port Valve Cfg File";
        HWND H=::GetActiveWindow();
        if (Dlg.DoModal()==IDOK)
          {
          m_sConfigFileC=(LPCTSTR)Dlg.GetPathName();
          m_sConfigFileC.FnContract();
          }
        ::SetActiveWindow(H);
        DCB.B=0;
        LoadVlvConfigurations();
        }
      DCB.B=0;
      return 1;
    case xidReload4WayBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        LoadVlvConfigurations();
        //Load4Way(s4WayName(), False);
        }
      DCB.B=0;
      return 1;
    case xidEdit4WayBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        m_CfgFileTime.dwLowDateTime=0;
        m_CfgFileTime.dwHighDateTime=0;
        if (m_sConfigFileC.Length()==0)
          {
          m_sConfigFileC=ConfigFileName;
          if (!FileExists(ConfigFileX()))
            CreateConfigFile(ConfigFileX());
          LoadVlvConfigurations();
          }
        Strng FileName(ConfigFileX());
        Strng FullFilename;
        int RetCode = DCB.DoEditBtn(m_hProcess, m_dwProcessId, "", FileName(), "s4p", &FullFilename, true);
        if (RetCode==-1)
          {
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            fprintf(f, "%s", ConfigFileContents);
            fclose(f);
            }
          else
            {
            LogError("4WayValve", 0, "Default Configuration file not written");
            }
          RetCode = DCB.DoEditBtn(m_hProcess, m_dwProcessId, "", FileName(), "s4p");
          }
        if (RetCode==0)
          {
          if (!FnModifyTime(ConfigFileX(), m_CfgFileTime))
            {
            m_CfgFileTime.dwHighDateTime=0;
            m_CfgFileTime.dwLowDateTime=0;
            }
          }
        }
      DCB.B=0;
      return 1;
    case xidValveConfig:
      if (DCB.rpC)
        {
        m_sConfig=DCB.rpC;
        LoadVlvConfigurations();
        }
      DCB.pC=m_sConfig();
      return 1;
    case xidValveModeRqd:
      if (DCB.rL && m_State.m_iMethod==Meth_None)
        SetVlvMode(*DCB.rL, false);
      DCB.L=m_iVlvModeRqd;
      return 1;
    case xidValveModeAct:
      if (DCB.rL && DCB.ForFileSnpScn())
        m_iVlvModeAct=*DCB.rL;
      DCB.L=m_iVlvModeAct;
      return 1;
    case xidValveModeForce:
      if (DCB.rB && m_State.m_iMethod==Meth_None && *DCB.rB!=0)
        SetVlvMode(m_iVlvModeRqd, true);
      DCB.B=0;
      return 1;
    case xidStateEnable:
      if (DCB.rB)
        {
        m_State.m_iMethod=(*DCB.rB?Meth_OnOff:Meth_None);
        if (m_State.m_iMethod==Meth_OnOff)
          SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
        }
      DCB.B=0;//m_State.m_bEnable;
      return 1;
    case xidStateMethod:
      if (DCB.rB)
        {
        m_State.m_iMethod=*DCB.rB;
        switch (m_State.m_iMethod)
          {
          case Meth_OnOff:
            SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
            break;
          case Meth_Profile:
            {
            int xxx=0;
            //_asm int 3;
            //SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
            break;
            }
          }
        }
      DCB.B=m_State.m_iMethod;
      return 1;
    case xidStateProfile:
      if (DCB.rL)
        {
        m_iProfile=*DCB.rL;
        m_iProfile=Max(m_iProfile, 0L);
        m_iProfile=Min(m_iProfile, m_Profiles.GetCount()-1L);
        }
      DCB.L=m_iProfile;
      return 1;
    case xidStateOn:
      if (DCB.rB)
        {
        m_State.m_bOn=*DCB.rB!=0;
        if (m_State.m_iMethod==Meth_OnOff)
          SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
        }
      DCB.B=m_State.m_bOn;
      return 1;
    case xidStateOffMode:
      if (DCB.rL)
        {
        m_State.m_iOffMode=*DCB.rL;
        if (m_State.m_iMethod==Meth_OnOff)
          SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
        }
      DCB.L=m_State.m_iOffMode;
      return 1;
    case xidStateOnMode:
      if (DCB.rL)
        {
        m_State.m_iOnMode=*DCB.rL;
        if (m_State.m_iMethod==Meth_OnOff)
          SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
        }
      DCB.L=m_State.m_iOnMode;
      return 1;
    case xidStateActualPosn:
      DCB.D=m_State.m_ActualPosn;
      return 1;
    case xidStateReqdPosn:
      if (DCB.rD)
        {
        if (Valid(*DCB.rD))
          {
          //m_State.m_ReqdPosn=Range(0.0, *DCB.rD, 1.0);
          if (m_State.m_iMethod==Meth_OnOff)
            SetVlvMode(*DCB.rD >= 0.5 ? m_State.m_iOnMode : m_State.m_iOffMode, false, *DCB.rD);
          }
        else
          {
          m_State.m_ReqdPosn=*DCB.rD;
          if (m_State.m_iMethod==Meth_OnOff)
            SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
          }
        }
      DCB.D=m_State.m_ReqdPosn;
      return 1;
    case xidPress1:
      if (Joins.GetSize()>=1)
        DCB.D=Joins[0].PB()->P;
      else
        DCB.D=dNAN;
      return 1;
    case xidPress2:
      if (Joins.GetSize()>=2)
        DCB.D=Joins[1].PB()->P;
      else
        DCB.D=dNAN;
      return 1;

    case xidPortStr1:    { int i=IOWithId_Self(0);  DCB.pC= (i>=0 && Nd_Rmt(i) ? Nd_Rmt(i)->FullObjTag() : "" );}   return 1;
    case xidPortStr2:    { int i=IOWithId_Self(1);  DCB.pC= (i>=0 && Nd_Rmt(i) ? Nd_Rmt(i)->FullObjTag() : "" );}   return 1;
    case xidPortStr3:    { int i=IOWithId_Self(2);  DCB.pC= (i>=0 && Nd_Rmt(i) ? Nd_Rmt(i)->FullObjTag() : "" );}   return 1;
    case xidPortStr4:    { int i=IOWithId_Self(3);  DCB.pC= (i>=0 && Nd_Rmt(i) ? Nd_Rmt(i)->FullObjTag() : "" );}   return 1;

    case xidPortOpn1:    if (DCB.rD) m_Ports[0].m_Open=Range(0.0, *DCB.rD, 1.0); DCB.D=m_Ports[0].m_Open;  return 1;
    case xidPortOpn2:    if (DCB.rD) m_Ports[1].m_Open=Range(0.0, *DCB.rD, 1.0); DCB.D=m_Ports[1].m_Open;  return 1;
    case xidPortOpn3:    if (DCB.rD) m_Ports[2].m_Open=Range(0.0, *DCB.rD, 1.0); DCB.D=m_Ports[2].m_Open;  return 1;
    case xidPortOpn4:    if (DCB.rD) m_Ports[3].m_Open=Range(0.0, *DCB.rD, 1.0); DCB.D=m_Ports[3].m_Open;  return 1;

    case xidConnStr1:    DCB.pC=GetJoinStr(0);   return 1;
    case xidConnStr10:   DCB.pC=GetJoinPipeStr(0,0);  return 1;
    case xidConnStr11:   DCB.pC=GetJoinPipeStr(0,1);  return 1;
    case xidConnStr12:   DCB.pC=GetJoinPipeStr(0,2);  return 1;
    case xidConnStr13:   DCB.pC=GetJoinPipeStr(0,3);  return 1;
    case xidConnStr2:    DCB.pC=GetJoinStr(1);   return 1;
    case xidConnStr20:   DCB.pC=GetJoinPipeStr(1,0);  return 1;
    case xidConnStr21:   DCB.pC=GetJoinPipeStr(1,1);  return 1;
    case xidConnStr22:   DCB.pC=GetJoinPipeStr(1,2);  return 1;
    case xidConnStr23:   DCB.pC=GetJoinPipeStr(1,3);  return 1;

      //case xidValveMask:
      //  {
      //  if (DCB.rL)
      //    {
      //    long Mi=*DCB.rL;
      //    for (int m=0; m<MaxModes; m++)
      //      {
      //      //long Mm=0;
      //      //for (int i=0; i<Max4WayIOPorts; i++)
      //      //  if (Modes[m].m_PortsJoin[i]>=0)
      //      //    Mm|=(1<<(i+4*Modes[m].m_PortsJoin[i]));
      //      if (Mi==Modes[m].m_lMask)
      //        {
      //        SetVlvMode(m);
      //        break;
      //        }
      //      }
      //    }

      //  //long Mm=0;
      //  //for (int i=0; i<Max4WayIOPorts; i++)
      //  //  {
      //  //  if (Modes[m_iVlvModeAct].m_PortsJoin[i]>=0)
      //  //    Mm|=(1<<(i+4*Modes[m_iVlvModeAct].m_PortsJoin[i]));
      //  //  }

      //  DCB.L=Modes[m_iVlvModeAct].m_lMask;
      //  return 1;
      //  }
    }
  return MdlNode::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

void C4PortValve::CreateConfigFile(LPCTSTR Fn)
  {
  if (!FileExists((LPTSTR)Fn))
    {
    FILE * f=fopen(Fn, "wt");
    if (f)
      {
      fprintf(f, "%s", ConfigFileContents);
      fclose(f);
      }
    else
      {
      LogError("4WayValve", 0, "Configuration file %s not written", Fn);
      }
    }
  }

//--------------------------------------------------------------------------

void C4PortValve::LoadVlvConfigurations()//LPCTSTR Fn)
  {
  m_ConfigNames.Empty();

  bool FoundOld=false;
  if (!FnModifyTime(ConfigFileX(), m_CfgFileTime))
    {
    m_CfgFileTime.dwHighDateTime=0;
    m_CfgFileTime.dwLowDateTime=0;
    }

  CProfINIFile PF(ConfigFileX());
  for (int i=0; ; i++)
    {
    Strng Entry, Name;
    Entry.Set("Name%i", i);
    Name=PF.RdStr("Names", Entry(), "");
    if (Name.Length()>0)
      {
      m_ConfigNames.Add(i,Name());
      FoundOld=FoundOld || m_sConfig.XStrICmp(Name)==0;
      }
    else
      break;
    }

  if (!FoundOld)
    m_sConfig="Standard";

  LoadVlvModes();
  };

//---------------------------------------------------------------------------

bool C4PortValve::TokenIs(CTokenParser &Tkns, LPCTSTR pRqdToken)
  {
  const char * p=Tkns.NextToken();
  if (_stricmp(p, pRqdToken)==0)
    return True;
  Tkns.SetReturnSameToken();
  return False;
  }

//---------------------------------------------------------------------------

bool C4PortValve::CheckToken(CTokenParser &Tkns, LPCTSTR pRqdToken)
  {
  const char * p=Tkns.NextToken();
  if (_stricmp(p, pRqdToken)!=0)
    {
    if (!m_bErrorFound)
      LogError(FullObjTag(), 0, "'%s' Unexpected while parsing %s", p, Tkns.GetWholeLine());
    m_bErrorFound=true;
    return false;
    }
  return true;
  }

//--------------------------------------------------------------------------

void C4PortValve::LoadVlvModes()//LPCTSTR Fn, LPCTSTR ConfigName)
  {
  m_Modes.RemoveAll();

  #if dbg4Port
  if (dbgLoad())
    {
    dbgpln("%s", "----------------");
    dbgpln("%s", ConfigFileX());
    dbgpln("%s", m_sConfig());
    }
  #endif

  m_bErrorFound=true;

  CProfINIFile PF(ConfigFileX());
  for (int iMd=0; ; iMd++)
    {
    Strng Entry, Mode;
    Entry.Set("Mode%i", iMd);
    Mode=PF.RdStr(m_sConfig(), Entry(), "");
    if (Mode.Length()>0)
      {
      C4PortMode Md;
      //CTkn
      //int iMd=Mode.XStrPBrk(",{}");

      //enum eWhat {eMd, eName, /*eWait0,*/ eJ0, /*eWait1,*/ eJ1, /*eWait2,*/ eInTime, eOutTime } What=eMd;

      int nJ0=0;
      int nJ1=0;
      LPTSTR StartChar=NULL;
      LPTSTR EndChar=NULL;

      if (iMd==0)
        m_bErrorFound=false;

      Strng Buff(Mode);
      CTokenParser Tkns(true, true, Buff());
      Tkns.SetSeperators(",{}");
      Md.m_iMd=SafeAtoI(Tkns.NextToken());
      CheckToken(Tkns, ",");
      Md.m_sName=Tkns.NextToken();
      CheckToken(Tkns, ",");
      CheckToken(Tkns, "{");
      while (!TokenIs(Tkns, "}"))
        {
        Md.m_Jn[0].m_Port[nJ0]=SafeAtoI(Tkns.NextToken())-1;
        if (TokenIs(Tkns, ","))
          nJ0++;
        }
      CheckToken(Tkns, ",");
      CheckToken(Tkns, "{");
      while (!TokenIs(Tkns, "}"))
        {
        Md.m_Jn[1].m_Port[nJ1]=SafeAtoI(Tkns.NextToken())-1;
        if (TokenIs(Tkns, ","))
          nJ1++;
        }
      CheckToken(Tkns, ",");
      Md.m_InTime=Range(0.0, SafeAtoF(Tkns.NextToken()), 100.0);
      CheckToken(Tkns, ",");
      Md.m_OutTime=Range(0.0, SafeAtoF(Tkns.NextToken()), 100.0);
      if (TokenIs(Tkns, ","))
        Md.m_iChgMd = SafeAtoI(Tkns.NextToken());
      else
        Md.m_iChgMd = -1;

      for (int j=0; j<Max4WayJoins; j++)
        {
        for (int i=0; i<Max4WayIOPorts; i++)
          {
          if (Md.m_Jn[j].m_Port[i]>=0)
            Md.m_PortsJoin[Md.m_Jn[j].m_Port[i]]=j;
          }
        }

      #if dbg4Port
      if (dbgLoad())
        {
        dbgp("%3i %-20s", Md.m_iMd, Md.m_sName);
        for (int j=0; j<Max4WayJoins; j++)
          {
          dbgp(" {");
          for (int i=0; i<Max4WayIOPorts; i++)
            dbgp(Md.m_Jn[j].m_Port[i]>=0 ?"%3i":"   ", Md.m_Jn[j].m_Port[i]);
          dbgp("}");
          }
        dbgp(" %10.2f,%10.2f", Md.m_InTime, Md.m_OutTime);
        dbgp(" [%3i%3i%3i%3i]", Md.m_PortsJoin[0], Md.m_PortsJoin[1], Md.m_PortsJoin[2], Md.m_PortsJoin[3]);
        dbgpln("");
        }
      #endif

      m_Modes.Add(Md);
      //m_ConfigNames.Add(Name());
      int xxx=0;
      }
    else
      break;
    };

  for (int io=0; io<Max4WayIOPorts; io++)
    {
    Strng Entry, Port;
    Entry.Set("Port%i", io+1);
    Port=PF.RdStr(m_sConfig(), Entry(), "");
    m_Ports[io].m_ResMult = 0.5; // 0.5 to account for 50% drop in and 50% drop out;
    m_Ports[io].m_Open    = 0.0;
    if (Port.Length()>0)
      m_Ports[io].m_ResMult = Range(0.0, 0.5*SafeAtoF(Port()), 1000.0);
    }

  m_Profiles.SetSize(0,4);
  m_ProfileNames.Empty();
  for (int pr=0; ; pr++)
    {
    Strng Entry, Name;
    Entry.Set("Profile%i.Name", pr);
    Name=PF.RdStr(m_sConfig(), Entry(), "");
    Name.Trim();
    if (Name.GetLength()==0)
      break;

    C4PortProfile Prf;
    Prf.m_sName=Name();
    m_ProfileNames.Add(pr, Name());

    for (int ipt=0; ipt<100; ipt++)
      {
      Strng Entry, Data;
      Entry.Set("Profile%i.Pt%i", pr, ipt);
      Data=PF.RdStr(m_sConfig(), Entry(), "");
      Data.Trim();
      if (Data.GetLength()==0)
        break;

      C4PortProfPt Pt;
      CTokenParser Tkns(true, true, Data());
      Tkns.SetSeperators(",{}");
      Pt.m_Posn=Range(0.0, SafeAtoF(Tkns.NextToken()), 1.0);
      CheckToken(Tkns, ",");
      //Pt.m_Mode=SafeAtoI(Tkns.NextToken());
      for (int j=0; j<Max4WayJoins; j++)
        {
        if (j>0)
          CheckToken(Tkns, ",");
        CheckToken(Tkns, "{");
        for (int i=0; i<Max4WayIOPorts; i++)
          {
          if (i>0)
            CheckToken(Tkns, ",");
          Pt.m_Jn[j].m_Open[i]=Range(0.0, SafeAtoF(Tkns.NextToken()), 1.0);
          }
        CheckToken(Tkns, "}");
        }

      Prf.m_Pts.Add(Pt);
      }
    m_Profiles.Add(Prf);
    }
  if (m_Profiles.GetSize()==0)
    m_ProfileNames.Add(-1,"None");

  m_ClosedTime=Range(0.0, SafeAtoF(PF.RdStr(m_sConfig(), "ClosedTime", "0")), 1000.0);

  #if dbg4Port
  if (dbgLoad())
    {
    dbgpln("Closed: %10.2f", m_ClosedTime);
    dbgpln("%s", "----------------");
    }
  #endif;

  SetCI(1, m_bErrorFound);

  //for (int io=0; io<NoFlwIOs(); io++)
  FixModeStrs(-1);

  SetVlvMode(Max(0L, m_iVlvModeAct), true);
  };

//--------------------------------------------------------------------------

void C4PortValve::SetVlvMode(long Md, bool Immediate, double RqdPosn)
  {
  if (m_bErrorFound)
    return;
  int i=Range(0L, Md, m_Modes.GetSize()-1L);
  bool UseRqdPosn=(RqdPosn!=-1.2345);
  if (Valid(RqdPosn) && UseRqdPosn)
    RqdPosn=Range(0.0, RqdPosn, 1.0);
  if (i!=m_iVlvModeRqd || !Valid(m_State.m_ReqdPosn) || RqdPosn!=m_State.m_ReqdPosn)
    {
    m_iVlvModeRqd=i;
    //m_State.m_ReqdPosn=RqdPosn;
    //if (m_Modes[m_iVlvModeRqd].m_iChgMd>=0 && (UseRqdPosn && RqdPosn>0.0 && RqdPosn<1.0)) // ie changing
    //  m_iVlvModeRqd=m_Modes[m_iVlvModeRqd].m_iChgMd;
    if (Immediate || (m_nInterSteps==0))
      {
      m_iVlvModeAct = m_iVlvModeRqd;
      m_iTrans   = 0;
      m_TmInStep = 0;
      if (Valid(RqdPosn))
        m_OpnWRT50 = 2.0*fabs(RqdPosn-0.5);
      else
        m_OpnWRT50 = 1.0;
      m_Trans.SetSize(0);
      if (UseRqdPosn)
        {
        m_State.m_ReqdPosn=RqdPosn;
        m_State.m_ActualPosn=RqdPosn;
        }
      }
    else
      {
      m_Trans.SetSize(0);
      C4PortTrans T;
      if (m_iVlvModeAct>=0)
        {
        for (int i=0; i<m_nInterSteps; i++)
          {
          T.m_Mode=m_iVlvModeAct;
          T.m_TimeInc=m_Modes[m_iVlvModeAct].m_OutTime/m_nInterSteps;
          T.m_OpenS=double(m_nInterSteps-i)/(m_nInterSteps);
          T.m_OpenE=double(m_nInterSteps-(i+1))/(m_nInterSteps);
          m_Trans.Add(T);
          }
        }
      if (m_ClosedTime>0)
        {
        T.m_Mode=-1;
        T.m_TimeInc=m_ClosedTime;
        T.m_OpenS=0;
        T.m_OpenE=0;
        m_Trans.Add(T);
        }
      for (int i=0; i<m_nInterSteps; i++)
        {
        T.m_Mode=m_iVlvModeRqd;
        T.m_TimeInc=m_Modes[m_iVlvModeRqd].m_InTime/m_nInterSteps;
        T.m_OpenS=double(i)/(m_nInterSteps);
        T.m_OpenE=double(i+1)/(m_nInterSteps);
        m_Trans.Add(T);
        }

      #if dbg4Port
      if (dbgOperate())
        {
        dbgpln("----------------------------------");
        for (int i=0; i<m_Trans.GetSize(); i++)
          {
          C4PortTrans &T=m_Trans[i];
          dbgpln("%3i %3i S:%5.2f E:%5.2f %10.3f", i, T.m_Mode, T.m_OpenS, T.m_OpenE, T.m_TimeInc);
          }
        dbgpln("----------------------------------");
        }
      #endif

      m_iTrans    = 0;
      m_TmInStep  = 0;
      m_OpnWRT50  = 1.0;
      if (m_Trans.GetSize()==0)
        m_iVlvModeAct=m_iVlvModeRqd;
      }
    }
  }

//--------------------------------------------------------------------------

void C4PortValve ::StartStep()
  {
  }

//--------------------------------------------------------------------------

double C4PortValve::GetMaxTimeInc()
  {
  if (!m_bErrorFound)
    {
    if /*while*/ (m_iTrans < m_Trans.GetSize())
      {
      C4PortTrans &T=m_Trans[m_iTrans];
      //m_iVlvModeAct=T.m_Mode;
      //m_Open=T.m_Open;
      //dbgpln("4Port:GetMaxTimeInc   %3i %10.2f %3i %10.2f %10.3f", m_iTrans, m_TmInStep, m_iVlvModeAct, m_Open, T.m_TimeInc);
      if (T.m_TimeInc>0)
        {
        return T.m_TimeInc;
        }
      //m_iTrans++;
      }
    }
  //m_Trans.SetSize(0);
  return 1e10;//0.1+(60.0*rand())/RAND_MAX;
  };

//--------------------------------------------------------------------------

void C4PortValve::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  if (!m_bErrorFound)
    {
    if (m_iTrans < m_Trans.GetSize())
      {
      C4PortTrans &T=m_Trans[m_iTrans];
      m_iVlvModeAct=T.m_Mode;
      m_OpnWRT50=T.m_OpenS+(T.m_OpenE-T.m_OpenS)*m_TmInStep/GTZ(T.m_TimeInc);
      #if dbg4Port
      if (dbgOperate())
        dbgpln("4Port:EvalCtrlActions iT:%3i Tm:%10.2f Md:%3i Opn:%10.2f %10.3f", m_iTrans, m_TmInStep, m_iVlvModeAct, m_OpnWRT50, T.m_TimeInc);
      #endif
      m_TmInStep+=ICGetTimeInc();
      if (m_TmInStep>=T.m_TimeInc)
        {
        m_TmInStep-=T.m_TimeInc;
        m_iTrans++;
        }
      }
    else
      {
      //for (int i=0; i<Max4WayIOPorts; i++)
      //  m_Ports[i].m_Open=0.0;

      if (Valid(m_State.m_ReqdPosn))
        m_OpnWRT50=2.0*fabs(m_State.m_ActualPosn-0.5);
      else
        m_OpnWRT50=1.0;
      #if dbg4Port
      if (dbgOperate())
        dbgpln("4Port:EvalCtrlActions    %3s    %10s    %3i     %10.2f %10s", "","", m_iVlvModeAct, m_OpnWRT50, "");
      #endif
      }

    if (Valid(m_State.m_ReqdPosn))
      m_State.m_bOn=(m_State.m_ActualPosn>=0.5);
    }
  };

//--------------------------------------------------------------------------

LPTSTR C4PortValve::GetJoinStr(long iJoin)
  {
  Strng &S=m_sConnStrs[iJoin];
  S="";
  int k=0;
  for (int i=0; i<Max4WayIOPorts; i++)
    {
    if (m_iVlvModeAct>=0 && m_iVlvModeAct<m_Modes.GetSize() && m_Modes[m_iVlvModeAct].m_PortsJoin[i] == iJoin)
      {
      int io=IOWithId_Self(i);
      if (io>=0)
        {
        if (Nd_Rmt(io))
          {
          if (S.Len()>0)
            S+=",";
          S+=Nd_Rmt(io)->FullObjTag();
          }
        else
          S+="?";
        }
      }
    }
  return S();
  }

//--------------------------------------------------------------------------

LPTSTR C4PortValve::GetJoinPipeStr(long iJoin, long iPipe)
  {
  int k=0;
  Strng &S=m_sConnPipeStrs[iJoin][iPipe];
  S="";
  int iojoin=0;
  if (m_iVlvModeAct>=0 && m_iVlvModeAct<m_Modes.GetSize())
    {
    for (int io=0; io<NoFlwIOs(); io++)
      {
      int port=IOId_Self(io);
      if (m_Modes[m_iVlvModeAct].m_PortsJoin[port] == iJoin)
        {
        if (iojoin==iPipe)
          {
          S=Nd_Rmt(io)->FullObjTag();
          return S();
          }
        iojoin++;
        }
      }
    }
  return S();
  }
//--------------------------------------------------------------------------

flag C4PortValve::ValidateData(ValidateDataBlk & VDB)
  {
  m_iProfile=Max(m_iProfile, 0L);
  m_iProfile=Min(m_iProfile, m_Profiles.GetCount()-1L);

  flag   OK=m_FEP.ValidateData(VDB);
  return MdlNode::ValidateData(VDB) && OK;
  };

//--------------------------------------------------------------------------

flag C4PortValve::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  switch (m_State.m_iMethod)
    {
    case Meth_None:
      for (int i=0; i<m_ModeStrs.Length(); i++)
        {
        DDBValueLst * L=m_ModeStrs.Item(i);
        Acts.SetAtGrow(i, MdlAction(i, MAT_State, m_iVlvModeAct!=L->m_lVal, L->m_pStr, false, L->m_lVal));
        }
      break;
    case Meth_OnOff:
      Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_State.m_bOn, "On", 1));
      Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_State.m_bOn, "Off", 0));
      Acts.SetAtGrow(2, MdlAction(2, MAT_Switch));
      break;
    }
  return True;
  }

//--------------------------------------------------------------------------

flag C4PortValve::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (m_State.m_iMethod)
    {
    case Meth_None:
      {
      DDBValueLst * L=m_ModeStrs.Item(Act.iIndex);
      SetVlvMode(L->m_lVal, false);
      break;
      }
    case Meth_OnOff:
      switch (Act.iIndex)
        {
        case 0:
        case 1:
          m_State.m_bOn=Act.iValue!=0;
          break;
        case 2:
          m_State.m_bOn=!m_State.m_bOn;
          break;
        }
      SetVlvMode(m_State.m_bOn ? m_State.m_iOnMode : m_State.m_iOffMode, false);
      break;
    }
  return true;
  }

//--------------------------------------------------------------------------

CFlange * C4PortValve ::GetFlange(int IOId)
  {
  return NULL;
  };

//--------------------------------------------------------------------------

void C4PortValve::FixModeStrs(int IONo)
  {
  m_ModeStrs.Empty();
  for (int iMd=0; iMd<m_Modes.GetSize(); iMd++)
    m_ModeStrs.Add(m_Modes[iMd].m_iMd, (LPTSTR)(LPCTSTR)m_Modes[iMd].m_sName);//, 0Modetrs[iMd].m_dwFlags);
  };

//---------------------------------------------------------------------------

void C4PortValve ::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);
  IOFB(IONo,0)->AssignFlwEqnGroup(Vlv4PortGroup, Vlv4PortGroup.Default(), this);
  IOFB(IONo,0)->SetParentFlwEqn(&m_FEP);
  FixModeStrs(-1);
  };

//---------------------------------------------------------------------------

void C4PortValve ::PreDisConnect(int IONo)
  {
  FixModeStrs(IONo);
  MdlNode::PreDisConnect(IONo);
  }

//--------------------------------------------------------------------------

void C4PortValve ::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  SetDatums_Node(Pass, List, IOIn, NULL);
  };

//--------------------------------------------------------------------------

flag C4PortValve ::Set_Sizes()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    {
    double A=IOFB_Rmt(i,0)->Area();
    IOFB(i,0)->SetArea(A);
    IOFB(i,0)->SetActLength(0.0);
    IOFB(i,0)->SetFitLength(0.0);
    }
  return True;
  };

//--------------------------------------------------------------------------

void   C4PortValve ::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
      {
      ResetData(false);

      double P0 = NoFlwIOs() ? AtmosPress(IODatum_Term(0)) : Std_P;
      for (int j=0; j<2; j++)
        {
        Set_JoinP(j, P0);
        Set_JoinP_Est(j, P0);
        Set_JoinP_Max(j, dNAN);
        Set_JoinP_MaxSet(j, dNAN);
        }
      for (int i=0; i<NoFlwIOs(); i++)
        {
        Set_IOP_Flng(i, P0);
        Set_IOP_Self(i, P0);
        Set_IOP_Est_Self(i, P0);
        Set_IOP_Est_Flng(i, P0);
        IOConduit(i)->SetState(RqdState);
        if (IOConduitIO(i))
          IOConduitIO(i)->SetState(RqdState);
        if (IOConduitIn(i))
          IOConduitIn(i)->SetState(RqdState);
        SetIOQm_In(i, 0.0);
        SetIOQmEst_In(i, 0.0);
        SetIOQmSpace_Self(i, 0.0);
        SetIOQmAvail_Self(i, 0.0);
        }
      }
    break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//---------------------------------------------------------------------------

bool C4PortValve::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!FlwNode::DoPropagateNetInfo(Ctrl, IONo, false))
    return false;

  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (i!=IONo)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//---------------------------------------------------------------------------

void C4PortValve::ConfigureJoins()
  {
  Set_NoJoins(3);
  if (m_State.m_iMethod==Meth_Profile && m_iProfile>=0)
    {
    C4PortProfile & Prf= m_Profiles[m_iProfile];
    for (int i=0; i+1<Prf.m_Pts.GetSize() && Prf.m_Pts[i+1].m_Posn<m_State.m_ActualPosn ; i++)
      {}

    C4PortProfPt & Pt0=Prf.m_Pts[i];
    C4PortProfPt & Pt1=Prf.m_Pts[i+1];
    double Frac=(m_State.m_ActualPosn-Pt0.m_Posn)/(Pt1.m_Posn-Pt0.m_Posn);

    int OpenTo[Max4WayIOPorts];
    for (int i=0; i<Max4WayIOPorts; i++)
      {
      OpenTo[i]=-1;
      m_Ports[i].m_Open=0.0;
      }
    for (int j=0; j<Max4WayJoins; j++)
      {
      for (int i=0; i<Max4WayIOPorts; i++)
        {
        double PortOpen= Pt0.m_Jn[j].m_Open[i]+Frac*(Pt1.m_Jn[j].m_Open[i]-Pt0.m_Jn[j].m_Open[i]);
        if (PortOpen>1e-6)
          {
          if (OpenTo[i]<0)
            OpenTo[i]=j;
          else
            LogWarning(Tag(), 0, "Port %i connected to multiple joins", i+1);
          m_Ports[i].m_Open=PortOpen;
          }
        }
      }               

    for (int i=0; i<Max4WayIOPorts; i++)
      {
      if (OpenTo[i]>=0)
        SetIO_Join(i, OpenTo[i]);
      else
        SetIO_Closed(i, 2);
      }

    }
  else if (!m_bErrorFound && InRange(0L, m_iVlvModeAct, m_Modes.GetSize()-1L))
    {
    long ModeReqd=m_iVlvModeAct;
    long ModeMidPt=-1;
    if (m_OpnWRT50<1.0 && m_Modes[ModeReqd].m_iChgMd>=0)
      ModeMidPt=m_Modes[ModeReqd].m_iChgMd;

    for (int i=0; i<Max4WayIOPorts; i++)
      m_Ports[i].m_Open=0.0;

    int JCnt[Max4WayJoins];
    for (int i=0; i<Max4WayJoins; i++)
      JCnt[i]=0;
    for (int i=0; i<NoProcessIOs(); i++)
      {
      int port=IOId_Self(i);
      int Jn=m_Modes[ModeReqd].m_PortsJoin[port];
      if (Jn>=0)
        JCnt[Jn]++;
      int JnMidPt = (ModeMidPt>=0) ? m_Modes[ModeMidPt].m_PortsJoin[port] : -1;
      if (JnMidPt>=0)
        JCnt[JnMidPt]++;
      }
    for (int i=0; i<NoProcessIOs(); i++)
      {
      int IsClosed=true;
      int port=IOId_Self(i);

      int Jn  = m_Modes[ModeReqd].m_PortsJoin[port];
      if (Jn>=0 && JCnt[Jn]>1)
        {
        if (fDoDbgBrk)
          dbgpln("SetIO_Join(%3i %3i) A %s", i, Jn, FullObjTag());
        SetIO_Join(i, Jn);
        m_Ports[port].m_Open+=m_OpnWRT50;
        IsClosed=false;
        }

      int JnMidPt = (ModeMidPt>=0) ? m_Modes[ModeMidPt].m_PortsJoin[port] : -1;
      if (JnMidPt>=0 && JCnt[JnMidPt]>1)
        {
        if (fDoDbgBrk)
          dbgpln("SetIO_Join(%3i %3i) B %s", i, JnMidPt, FullObjTag());
        SetIO_Join(i, JnMidPt);
        m_Ports[port].m_Open+=(1.0-m_OpnWRT50);
        IsClosed=false;
        }

      if (IsClosed)
        {
        if (fDoDbgBrk)
          dbgpln("SetIO_Closed(%3i %3i) %s", i, 2, FullObjTag());
        SetIO_Closed(i, 2);
        }
      }
    for (int i=0; i<Max4WayIOPorts; i++)
      m_Ports[i].m_Open=Min(1.0, m_Ports[i].m_Open);

    //if ()

    if (0)
      {
      dbgp("Md: %3i %3i %6.1f  ", ModeReqd, ModeMidPt, m_OpnWRT50*100);
      for (int i=0; i<Max4WayIOPorts; i++)
        dbgp(" %6.1f", m_Ports[i].m_Open*100);
      dbgpln("");
      }
    }
  else
    {
    for (int i=0; i<NoProcessIOs(); i++)
      {
      if (fDoDbgBrk)
        dbgpln("SetIO_Closed(%3i %3i) %s", i, 2, FullObjTag());
      SetIO_Closed(i, 2);
      }
    }
  };

//--------------------------------------------------------------------------

//void C4PortValve ::SetJoinPressure(int iSolnTyp, int iJoinId, double P)
//  {
//  if (iJoinId==0)
//    MdlNode::SetJoinPressure(iSolnTyp, PB, P);
//  FlwNode::SetJoinPressure(iSolnTyp, iJoinId, P);
//  };
//
////--------------------------------------------------------------------------
//
//double C4PortValve ::GetJoinPressure(int iSolnTyp, int iJoinId)
//  {
//  if (iJoinId==0)
//    return MdlNode::GetJoinPressure(iSolnTyp, PB);
//  return FlwNode::GetJoinPressure(iSolnTyp, iJoinId);
//  };

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/
void C4PortValve ::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      if (NoProcessJoins()>=1)
        EvalPBJoinPressure(0, NULL, NULL);//&QProd());
      //if (NoProcessJoins()>=2)
      //  EvalPBJoinPressure(1);

      break;
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      if (fDoDbgBrk)
        {
        for (int i=0; i<NoFlwIOs(); i++)
          {
          dbgpln("IOP %3i %10.3f %10.3f %s", i, IOP_Est_Self(i), IOP_Self(i), FullObjTag());
          }
        }
      break;
    }
  };

// -------------------------------------------------------------------------

flag C4PortValve ::ChangeMode(dword NewMode, dword OldMode)
  {
  if (NewMode==SM_Direct && !GSM.Enabled())
    GSM.Open();

  return True;
  };

// --------------------------------------------------------------------------

flag C4PortValve ::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  int port=IOId_Self(IONo);
  double xx[]={m_Ports[port].m_ResMult};
  IOFB(IONo, FE)->SetUserValues(xx, 1);
  double Reg=Sqr(m_Ports[port].m_Open);
  if (IO_Closed_Self(IONo) || IO_Closed_Rmt(IONo))
    Reg=0.0;
  //dbgpln("IONo:%3i port %3i  %6.1f %s", IONo, port, m_Ports[port].m_Open*100, FullObjTag());
  return IOFB(IONo, FE)->EvaluateFlwEqn(Task, pProps, !IO_Closed_Self(IONo), false, Reg, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
  };

// --------------------------------------------------------------------------

void C4PortValve ::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Node(Task, 0x0ffffff);
  }

// --------------------------------------------------------------------------

bool C4PortValve::EvalProductClusters(int Index, long & JoinMask)
  {
  //JoinMask=-1;
  //return Index==0;

  switch (Index)
    {
    case 0: JoinMask=0x00000001;          return true;
    case 1: JoinMask=0x00000002;          return true;
    case 3: JoinMask=(0xffffffff & ~0x3); return true;
    }
  return false;
  };

// --------------------------------------------------------------------------

void C4PortValve ::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoProcessJoins()>0)
    {
    switch (SolveMethod())
      {
      case SM_Direct:
        if (NoProcessJoins()>0)
          Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, NULL, NULL, NULL);
        break;
      case SM_Inline:
      case SM_Buffered:
        for (int j=0; j<NoProcessJoins(); j++)
          Xfer_EvalProducts(j, Joins[j].Pressure(), NULL, NULL, NULL, NULL, NULL);
        break;
      }
    }
  };

//--------------------------------------------------------------------------

void C4PortValve ::EvalDiscrete()
  {
  MdlNode::EvalDiscrete();
  };

//--------------------------------------------------------------------------

void C4PortValve ::EvalPBMakeUpReqd(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferReqd(0);
  MakeUpNodeTransferReqd(0);
  };

//--------------------------------------------------------------------------

void C4PortValve ::EvalPBMakeUpAvail(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferAvail(0);
  MakeUpNodeTransferAvail(0);
  };

//--------------------------------------------------------------------------

void C4PortValve ::OnAppActivate(BOOL bActive)
  {
  if (/*fOn &&*/ bActive /*&& fFileLoadAllowed*/)
    {

    FILETIME Ft;
    if (FnModifyTime(ConfigFileX(), Ft))
      {
      CTime TNew(Ft), TOld(m_CfgFileTime);
      if (TNew>TOld)
        {
        LoadVlvConfigurations();

        ValidateDataBlk VDB;
        ValidateData(VDB);

        char* pTag = new char[strlen(Tag())+1];
        strcpy(pTag, Tag());
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_ACCREFRESHSAVE, (LPARAM)pTag);
        };

      //if (!Reload && sRCTName() && (EditTime.dwLowDateTime || EditTime.dwHighDateTime))
      //  {
      //  if (sFn.FnExpandQueryReload(sRCTName(), EditTime))
      //    {
      //    Reload=true;
      //    EditTime.dwLowDateTime=0;
      //    EditTime.dwHighDateTime=0;
      //    }
      //  }

      //if (!Reload && pTkns)
      //  {
      //  for (int i=0; i<pTkns->GetFilesUsedCount(); i++)
      //    if (sFn.FnExpandQueryReload((char*)pTkns->GetIncFileName(i), pTkns->GetIncFileTime(i)))
      //      Reload=true;
      //  }

      //if (Reload)
      //  {
      //  bReloadReqd=true;
      //  if (bAutoReload)
      //    {
      //    Load(sRCTName(), true);
      //    char* pTag = new char[strlen(BaseTag())+1];
      //    strcpy(pTag, BaseTag());
      //    ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_ACCREFRESHSAVE, (LPARAM)pTag);
      //    }
      //  ValidateDataBlk VDB;
      //  ValidateData(VDB);
      //  }
      }
    }
  }

//---------------------------------------------------------------------------

int C4PortValve ::FilesUsed(CFilesUsedArray & Files)
  {
  Strng S=ConfigFileX();
  if (S.Len()>0)
    {
    Files.AddFile(S(), FU_CopyFile|FU_EditTxt);
    return 1;
    }
  return 0;
  };

//--------------------------------------------------------------------------

void C4PortValve ::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI= m_Closure[0];
    }
  };

//--------------------------------------------------------------------------

void C4PortValve ::DumpDerivs()
  {
  dbgpln("--Xfr %-12s", FullObjTag());
  for (int i = 0; i < NoFlwIOs(); i++)
    if (IO_In(i))
      dbgpln("            In  >> :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC %s",
      IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas) ,
      IOConduit(i)->msHz(),
      IOConduit(i)->totHf()/GTZ(IOConduit(i)->QMass(som_ALL)),
      K2C(IOConduit(i)->Temp()), Nd_Rmt(i)->FullObjTag());
  for (i = 0; i < NoFlwIOs(); i++)
    if (IO_Out(i))
      dbgpln("            Out << :[%14.6g][%14.6g]|[%14.6g] %14.6g %14.3fC %s",
      IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas) ,
      IOConduit(i)->msHz(),
      IOConduit(i)->totHf()/GTZ(IOConduit(i)->QMass(som_ALL)),
      K2C(IOConduit(i)->Temp()), Nd_Rmt(i)->FullObjTag());
  }

//--------------------------------------------------------------------------

dword C4PortValve ::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (GetStatusAsStopped())
    {
    Status |= (/*m_FEP.FlwRqdActive() ? FNS_UFlw :*/ FNS_UNoFlw);
    }
  else
    {
    if (NoFlwIOs())
      {
      int HasFlw=0;
      double TFlw=0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        TFlw+=IOQm_In(i);
        if (IOConduit(i)->QMass()>UsableMass)
          HasFlw=1;
        }
      Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
      if (TFlw>1.0e-6)
        Status |= FNS_NettQmP;
      else if (TFlw<-1.0e-6)
        Status |= FNS_NettQmN;

      switch (m_State.m_iMethod)
        {
        case Meth_None:
          Status |= (m_iVlvModeAct ? FNS_On : FNS_Off);
          break;
        case Meth_OnOff:
          Status |= (m_State.m_bOn ? FNS_On : FNS_Off);
          break;
        case Meth_Profile:
          Status |= (m_State.m_ActualPosn>0.5 ? FNS_On : FNS_Off);
          break;
        }
      }
    }
  return Status;
  };

//==========================================================================
//
//
//
//==========================================================================

#define DllImportExport

DEFINE_TAGOBJ(C4E_PDrop);
class C4E_PDrop : public CFlwEqn
  {
  enum TeeTypes { TT_Straight, TT_Branch};
  static DDBValueLst DDBTT[];
  public:
    double            m_KDp;
    double            m_KQv;
    CFrictionFactor   m_FF;

    C4E_PDrop(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~C4E_PDrop();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

DDBValueLst C4E_PDrop::DDBTT[]=
  {
    {TT_Straight, "Straight"},
    {TT_Branch,   "Branch"},
    {0}
  };

//==========================================================================
//
//
//
//==========================================================================

XID xidKConst = MdlBsXID(17001);
XID xidKFact  = MdlBsXID(17000);
XID xidPDiam  = MdlBsXID(17002);
XID xidVDiam  = MdlBsXID(17003);

IMPLEMENT_FLWEQN(C4E_PDrop, Vlv4PortGroup.Name(), "V4E_PDrop", "", TOC_SYSTEM,
                 "Valve 4 Port DP",
                 "Valve 4 Port DP");

C4E_PDrop::C4E_PDrop(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_KDp=0;
  m_KQv=0.001;
  };
//--------------------------------------------------------------------------

C4E_PDrop::~C4E_PDrop()
  {
  }

//--------------------------------------------------------------------------

void C4E_PDrop::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("K_DP",        "", DC_DP, "kPa", &m_KDp,          this, SM_DynBoth|isParm);
  DDB.Double("K_Qv",        "", DC_Qv, "L/s", &m_KQv,          this, SM_DynBoth|isParm);
  m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "", xidKFact, NULL, "Calculated", "Required");
  BuildDataDefnOveride(DDB);
  };

// --------------------------------------------------------------------------

flag C4E_PDrop::DataXchg(DataChangeBlk & DCB)
  {
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  return CFlwEqn::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag C4E_PDrop::ValidateData(ValidateDataBlk & VDB)
  {
  m_KDp=ValidateRange(VDB, "KDp", 0.0,  m_KDp, 1000.0);
  m_KQv=ValidateRange(VDB, "KQv", 1e-6, m_KQv, 1000.0);
  return True;
  }

//--------------------------------------------------------------------------

//void C4E_PDrop::StartSolution(rFlwEqnBlk FE)
//  {
//  };

//--------------------------------------------------------------------------

flag C4E_PDrop::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double dDensMeas=Max(0.001, FE.MeanRho(pProps));
  //double Qv=
  double dViscMeas=FE.MeanViscosity(pProps);
  double dPipeArea;
  if (pPhD0 && pPhD0->IsPipe())
    dPipeArea=pPhD0->Area();
  else if (pPhD1 && pPhD1->IsPipe())
    dPipeArea=pPhD1->Area();
  else
    {};

  //dbgpln("             %6.1f", Regulation*100);
  m_PhD.SetArea(dPipeArea);
  if (Regulation>0.0)
    {
    FE.SetQmFree();
    double KVel=m_KQv/FE.Area();
    FE.SetVelMeasRange(m_dDensMeas, dPipeArea, KVel*0.001);
    double K=m_KDp*FE.UserValue(0)/(5e-4*dDensMeas*KVel*KVel);

    m_PhD.KFact.SetVal(K*Regulation, this);

    //FE.SetVelMeasRange(dDensMeas, KVel*0.001);

    double Vel0 = FE.VelMeas();
    double DQm  = FE.DQmMeas(1.001);
    double Vel1 = FE.VelMeas(1.001);

    double DP0=-FE.QmSign()*5e-4*m_PhD.KFact()*m_dDensMeas*Vel0*Vel0;
    double DP1=-FE.QmSign()*5e-4*m_PhD.KFact()*m_dDensMeas*Vel1*Vel1;
    //FE.SetVelocity(FE.QmSign()*FE.VelMeas());
    FE.SetDPq(DP0, -FE.QmSign()*(DP0-DP1)/GTZ(DQm));
    }
  else
    {
    FE.SetQmReqd(0.0);
    }
  return True;
  };

//--------------------------------------------------------------------------

flag C4PortValve::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tBad Configuration"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
