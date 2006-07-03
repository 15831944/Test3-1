//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __VLVIPRV_H
#define  __VLVIPRV_H

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
             

DEFINE_TAGOBJ(CIsoPRV);
class DllImportExport CIsoPRV : public MdlNode
  {
  public:
    CIsoPRV(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CIsoPRV(){};

    virtual void      BuildDataDefn(DataDefnBlk & DDB);
    virtual flag      DataXchg(DataChangeBlk & DCB);
    virtual flag      ValidateData(ValidateDataBlk & VDB);
    virtual flag      GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag      SetModelAction(Strng & Tag, MdlAction & Act);

    //virtual CFlange * GetFlange(int IOId);
    //void              CreateConfigFile(LPCTSTR Fn);
    //bool              TokenIs(CTokenParser &Tkns, LPCTSTR pRqdToken);
    //bool              CheckToken(CTokenParser &Tkns, LPCTSTR pRqdToken);
    //void              LoadVlvConfigurations();//LPCTSTR Fn);
    //void              LoadVlvModes();//LPCTSTR Fn, LPCTSTR Config);
    //void              SetVlvMode(long Md, bool Immediate, double RqdPosn=-1.2345);
    ////void              SetConnStrs();

    //LPTSTR            GetJoinStr(long iJoin);
    //LPTSTR            GetJoinPipeStr(long iJoin, long iPipe);

    //void              FixModeStrs(int IONo);
    virtual void      PostConnect(int IONo);
    virtual void      PreDisConnect(int IONo);
    virtual Z_States  SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src);
    virtual flag      Set_Sizes();
    virtual void      SetState(eScdMdlStateActs RqdState);
    virtual bool      PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual void      ConfigureJoins();
    virtual void      StartStep();

    //virtual double    GetMaxTimeInc();

    virtual void      EvalJoinPressures(long JoinMask);
    virtual flag      EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void      EvalProductsInit(EvalProductsInitTasks Task);
    //virtual bool      EvalProductClusters(int Index, long & JoinMask);
    virtual void      EvalProducts(CNodeEvalIndex & NEI);

    virtual void      EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void      EvalDiscrete();

    virtual void      DumpDerivs();
    //virtual flag      ChangeMode(dword NewMode, dword OldMode);

    virtual void      EvalPBMakeUpReqd(long JoinMask);
    virtual void      EvalPBMakeUpAvail(long JoinMask);

    //virtual void      OnAppActivate(BOOL bActive);   
    //virtual int       FilesUsed(CFilesUsedArray & Files);

    virtual void      ClosureInfo();

    virtual dword     ModelStatus();

    //LPTSTR            ConfigFileX() { m_sConfigFileXTmp.FnExpand(m_sConfigFileC()); return m_sConfigFileXTmp(); };

  public:

    //CArray<C4PortMode, C4PortMode&>     m_Modes;
    //CArray<C4PortProfile, C4PortProfile&> m_Profiles;
    //C4PortPort                          m_Ports[Max4WayIOPorts];

    double              m_PFeed;

    //DDBValueLstMem    m_ConfigNames;
    //Strng             m_sConfigFileXTmp;
    //Strng             m_sConfigFileC;
    //FILETIME          m_CfgFileTime;
    //Strng             m_sConfig;
    //bool              m_bErrorFound;

    //HANDLE            m_hProcess;      //process handle for the editor
    //DWORD             m_dwProcessId;   //process ID for the editor

    //long              m_iVlvModeAct;
    //long              m_iVlvModeRqd;
    //long              m_iVlvModeMidPt;

    //struct  
    //  {
    //  byte              m_iMethod;  
    //  bool              m_bOn;           
    //  long              m_iOffMode;      
    //  long              m_iOnMode;     
    //  double            m_ActualPosn;
    //  double            m_ReqdPosn;   

    //  }               m_State;

    //long              m_nInterSteps;     // The number of steps to transition over for each part of change
    //long              m_iTrans; 
    //double            m_TmInStep; 
    //double            m_OpnWRT50;
    //double            m_ClosedTime;
    //CArray<C4PortTrans, C4PortTrans&> m_Trans;

    //long              m_iVlvModeStrs;
    //DDBValueLstMem    m_ModeStrs;
    //DDBValueLstMem    m_ProfileNames;
    //long              m_iProfile;
    ////bool              m_AllowManualPortPosn;

    //Strng             m_sConnStrs[Max4WayJoins];
    //Strng             m_sConnPipeStrs[Max4WayJoins][Max4WayIOPorts];

    CFlwEqnPtr         m_FEP;

    DEFINE_CI(CIsoPRV, MdlNode, 4);

  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif


