//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HEATER_H
#define  __HEATER_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif
#ifndef __M_BSTRES_H
#include "m_bstres.h"
#endif
#ifndef __MDLXREF_H
#include "xrefs.h"
#endif

#ifdef __HEATER_CPP
  #define DllImportExport DllExport
#elif !defined(HeatXch1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define SKIPIT 0
#if SKIPIT 
#pragma message ("---------------------------------------SKIPPED")
#else

//===========================================================================

enum HSCM_HeaterSizeCalcMethod { HSCM_None, HSCM_Duty, HSCM_HTC_Des, HSCM_Area_Des, HSCM_LMTD_Des, HSCM_HTC_Mdl, HSCM_Area_Mdl, HSCM_LMTD_Mdl };

class HeaterSizeCalcs
  {
  public:
    HeaterSizeCalcs();
    void BuildDataDefn(DataDefnBlk & DDB, pTaggedObject This);
    flag DataXchg(DataChangeBlk & DCB);
    void SetMdlDuty(double MdlDuty);

    bool Enabled() { return bOn; };
    bool DutyMethod() { return (iSizCalcMethod==HSCM_Duty); };
    bool HTCMethod() { return (iSizCalcMethod==HSCM_HTC_Des || iSizCalcMethod==HSCM_HTC_Mdl); };
    bool AreaMethod() { return (iSizCalcMethod==HSCM_Area_Des || iSizCalcMethod==HSCM_Area_Mdl); };
    bool LMTDMethod() { return (iSizCalcMethod==HSCM_LMTD_Des || iSizCalcMethod==HSCM_LMTD_Mdl); };
    bool DesDutyMethod() { return (iSizCalcMethod==HSCM_HTC_Des || iSizCalcMethod==HSCM_Area_Des || iSizCalcMethod==HSCM_LMTD_Des); };
    bool MdlDutyMethod() { return (iSizCalcMethod==HSCM_HTC_Mdl || iSizCalcMethod==HSCM_Area_Mdl || iSizCalcMethod==HSCM_LMTD_Mdl); };
    double HTC() { return dHTC; };
    double Area() { return dArea; };

  private:
    bool           bOn;
    byte           iSizCalcMethod;
    double         dHTC;
    double         dArea;
    double         dLMTD;
    double         dDesignDuty;
    double         dModelDuty;
    double         dActualDuty;
  };

//===========================================================================

DEFINE_TAGOBJ(SimpleHeater)
class DllImportExport SimpleHeater : public MN_BstRes
  {
  public:
    byte           iDutyMethod;
    HeaterSizeCalcs HSC;
    CTgFnIoVar DutyVar;
    double         dDutyFactor;
    double         dFarDutyVal;
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1,      //flag set True for first iteration when run is pressed
                   bDutyVarValid:1;

    SimpleHeater(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SimpleHeater(){};

  public:
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual flag   PreStartCheck();
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);

    // CNodeXRefMngr Overides
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual dword  ModelStatus();
    DEFINE_CI(SimpleHeater, MN_BstRes, 4);

  };

//===========================================================================

#endif

#undef DllImportExport

#endif


