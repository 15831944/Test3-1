//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __VLV4PORT_H
#define  __VLV4PORT_H

#include <sc_defs.h>
#include "m_base.h"
#include "flshtrn.h"
#include "sp_react.h"

#ifdef __VLV4PORT_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ==========================================================================
// Basic Flow Object
             
const long Max4WayIOPorts=4;
const long Max4WayJoins=2;

class C4PortMode
  { 
  public:
  C4PortMode()
    {
    m_iMd         = -1; 
    //m_Jn[MaxIOPorts][MaxJoins]; 
    memset(m_Jn, -1,  sizeof(m_Jn)); 
    memset(m_PortsJoin, -1, sizeof(m_PortsJoin)); 

    for (int j=0; j<Max4WayJoins; j++)
      for (int i=0; i<Max4WayIOPorts; i++)
        m_Jn[j].m_Port[i] = -1; 

    for (int i=0; i<Max4WayIOPorts; i++)
      m_PortsJoin[i] = -1; 

    m_InTime  = 0;
    m_OutTime = 0;
    m_iChgMd  = -1;
    //m_Open    = 0; 
    }
  public:
    int         m_iMd; 
    CString     m_sName; 
    struct      { signed char m_Port[Max4WayIOPorts]; } m_Jn[Max4WayJoins]; 
    //signed char m_Jn[Max4WayJoins][Max4WayIOPorts]; 

    signed char m_PortsJoin[Max4WayIOPorts];
    double      m_InTime;
    double      m_OutTime;
    int         m_iChgMd;
  }; 

class C4PortPort
  { 
  public:
    C4PortPort()
      {
      m_ResMult   = 1.0;
      m_Open      = 0.0;
      };

    double      m_ResMult;
    double      m_Open;
  };

class C4PortTrans
  { 
  public:
    double      m_TimeInc;
    double      m_OpenS;
    double      m_OpenE;
    long        m_Mode;
  };

class C4PortProfPt
  { 
  public:
    C4PortProfPt() { };

    //long        m_Mode;
    double      m_Posn;
    struct      { double m_Open[Max4WayIOPorts]; } m_Jn[Max4WayJoins]; 
  };

class C4PortProfile 
  {
  public:
    C4PortProfile & operator=(C4PortProfile & P)
      {
      m_sName=P.m_sName;
      m_Pts.SetSize(P.m_Pts.GetSize());
      for (int i=0; i<m_Pts.GetSize(); i++)
        m_Pts[i]=P.m_Pts[i];
      return *this;
      };

  public:
    CString                             m_sName;
    CArray<C4PortProfPt, C4PortProfPt&> m_Pts;
  };

DEFINE_TAGOBJ(C4PortValve);
class DllImportExport C4PortValve  : public MdlNode
  {
  public:
    C4PortValve (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C4PortValve (){};

    virtual void      BuildDataDefn(DataDefnBlk & DDB);
    virtual flag      DataXchg(DataChangeBlk & DCB);
    virtual flag      ValidateData(ValidateDataBlk & VDB);
    virtual flag      GetModelAction(CMdlActionArray & Acts);
    virtual flag      SetModelAction(CMdlAction & Act);

    //virtual CFlange * GetFlange(int IOId);
    void              CreateConfigFile(LPCTSTR Fn);
    bool              TokenIs(CTokenParser &Tkns, LPCTSTR pRqdToken);
    bool              CheckToken(CTokenParser &Tkns, LPCTSTR pRqdToken);
    void              LoadVlvConfigurations();//LPCTSTR Fn);
    void              LoadVlvModes();//LPCTSTR Fn, LPCTSTR Config);
    void              SetVlvMode(long Md, bool Immediate, double RqdPosn=-1.2345);
    //void              SetConnStrs();

    LPTSTR            GetJoinStr(long iJoin);
    LPTSTR            GetJoinPipeStr(long iJoin, long iPipe);

    void              FixModeStrs(int IONo);
    virtual void      PostConnect(int IONo);
    virtual void      PreDisConnect(int IONo);
    virtual void      SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag      Set_Sizes();
    virtual void      SetState(eScdMdlStateActs RqdState);
    virtual bool      PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual void      ConfigureJoins();
    virtual void      StartStep();

    virtual double    GetMaxTimeInc();

    virtual void      EvalJoinPressures(long JoinMask);
    virtual flag      EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void      EvalProductsInit(EvalProductsInitTasks Task);
    virtual bool      EvalProductClusters(int Index, long & JoinMask);
    virtual void      EvalProducts(CNodeEvalIndex & NEI);

    virtual void      EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void      EvalDiscrete();

    virtual void      DumpDerivs();
    virtual flag      ChangeMode(dword NewMode, dword OldMode);

    virtual void      EvalPBMakeUpReqd(long JoinMask);
    virtual void      EvalPBMakeUpAvail(long JoinMask);

    virtual void      OnAppActivate(BOOL bActive);   
    virtual int       FilesUsed(CFilesUsedArray & Files);

    virtual void      ClosureInfo();

    virtual dword     ModelStatus();

    LPTSTR            ConfigFileX() { m_sConfigFileXTmp.FnExpand(m_sConfigFileC()); return m_sConfigFileXTmp(); };

  public:

    CArray<C4PortMode, C4PortMode&>     m_Modes;
    CArray<C4PortProfile, C4PortProfile&> m_Profiles;
    C4PortPort                          m_Ports[Max4WayIOPorts];

    DDBValueLstMem    m_ConfigNames;
    Strng             m_sConfigFileXTmp;
    Strng             m_sConfigFileC;
    FILETIME          m_CfgFileTime;
    Strng             m_sConfig;
    bool              m_bErrorFound;

    HANDLE            m_hProcess;      //process handle for the editor
    DWORD             m_dwProcessId;   //process ID for the editor

    long              m_iVlvModeAct;
    long              m_iVlvModeRqd;
    //long              m_iVlvModeMidPt;

    struct  
      {
      byte              m_iMethod;  
      bool              m_bOn;           
      long              m_iOffMode;      
      long              m_iOnMode;     
      double            m_ActualPosn;
      double            m_ReqdPosn;   

      }               m_State;

    long              m_nInterSteps;     // The number of steps to transition over for each part of change
    long              m_iTrans; 
    double            m_TmInStep; 
    double            m_OpnWRT50;
    double            m_ClosedTime;
    CArray<C4PortTrans, C4PortTrans&> m_Trans;

    long              m_iVlvModeStrs;
    DDBValueLstMem    m_ModeStrs;
    DDBValueLstMem    m_ProfileNames;
    long              m_iProfile;
    //bool              m_AllowManualPortPosn;

    Strng             m_sConnStrs[Max4WayJoins];
    Strng             m_sConnPipeStrs[Max4WayJoins][Max4WayIOPorts];

    CFlwEqnPtr         m_FEP;

    DEFINE_CI(C4PortValve, MdlNode, 4);

  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif


