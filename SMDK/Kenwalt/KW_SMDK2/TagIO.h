//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TAGIO_H
#define  __TAGIO_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <vector>

#ifdef __TAGIO_CPP
#define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

//---------------------------------------------------------------------------


class CTagIO : public MBaseMethod
  {
  public:
    CTagIO(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual bool	  ValidateDataFields();

    virtual bool    PreStartCheck();

    //virtual void    EvalProducts();
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All)  ; // each Iteration - To calculate required control actions
    virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All) ; // each End of Run
    
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              {}; // each Iteration End to calculate Stats relevant to model

  protected:
    bool            AddSubsTag(CString & Tag, LPCSTR NewTag, bool TestParam = false);

    bool            m_bOn;

    //variables for direct tag set and get
    double          m_dGetValue;
    double          m_dSetValue;
    CString         m_sGetTag;
    CString         m_sSetTag;
    CString         m_sGetStatus;
    CString         m_sSetStatus;


    //variables for tag subscription
    bool            m_bSubsActive;
    bool            m_bBuildListRqd;
    CString         m_sGetTagSubs1;
    CString         m_sGetTagSubs2;
    CString         m_sSetTagSubs1;
    double          m_dGetValueSubs1;
    double          m_dGetValueSubs2;
    double          m_dSetValueSubs1;
    int             m_iGet1;
    int             m_iGet2;
    int             m_iSet1;
  };

#endif