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
    bool            m_bOn;
    bool            m_OnFly;

    CString         m_GetTempTag;
    CString         m_PutTempTag;

    double          m_GetTemp;
    double          m_PutTemp;
    double          m_DeltaTemp;

    int             m_iGet;
    int             m_iPut;

    //   double dCurrentTime;
    //   std::vector<FailureVariables> tasks;

    //   void SetSize(long size);
    //void Reset();
    //void RevalidateDataFields();

    //void FailItem(FailureVariables* task);
    //void RepairItem(FailureVariables* task);
    //double CalculateEvent(PDFType ePDF, double dAverage, double dStdDev);
  };

#endif