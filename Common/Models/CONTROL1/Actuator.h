//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ACTUATOR_H
#define  __ACTUATOR_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
#ifndef __MODELS_H
#include "xrefs.h"
#endif  
#ifndef __MODELS_H
#include "models.h"
#endif  

#ifdef __ACTUATOR_CPP
  #define DllImportExport 
#elif !defined(Control1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define SKIPIT 0
#if SKIPIT 
#pragma message ("---------------------------------------SKIPPED")
#else

//============================================================================

_FWDDEF(CActuatorBlk)
class CActuatorBlk : public CXRefStatus
  {
  public:
    Strng      m_sID;
    Strng      m_sIDInput;
    Strng      m_sTagInput;
    Strng      m_sIDOutput;
    Strng      m_sTagOutput;
    //Strng      m_sIORaw;

    CTgFnIoVar m_OutputVar;
    CTgFnIoVar m_InputVar;
    flag       m_bValid:1;
    long       m_iPriority;

    double     m_dReqdValue;

    struct 
      {
      byte     m_iType;
      float    m_dFactor;
      float    m_dBias;
      }        m_Xform;
    
    struct 
      {
      byte     m_iType;
      float    m_dTau;
      double   m_dPrevValue;
      }        m_Flt;

    struct 
      {
      byte     m_iType;
      float    m_dSlewTime;
      float    m_dLoValue;
      float    m_dHiValue;
      float    m_dNoiseStdDevFrac;
      double   m_dPrevValue;
      }        m_Fail;

    float      m_dMinValue; 
    float      m_dMaxValue; 

    double     m_dValue;

    float      m_dLoLimit;  
    float      m_dHiLimit;  
    flag       m_bLoInvert; 
    flag       m_bHiInvert; 
    flag       m_bLo;       
    flag       m_bHi;       

    struct 
      {
      byte     m_iType;
      float    m_dMinValue;
      float    m_dMaxValue;
      }        m_RawXform;

    double     m_dOutputValue;       

    Strng      m_sPLCAddress;

    CActuatorBlk();
    virtual ~CActuatorBlk();
    void Init(CNodeXRefMngr * pXRM, int iNo);
    void ExecIns(double dT);

    //CXRefStatus Override
    bool IsXRefActive() const    { return m_bValid;  };
  
  };

typedef CSmartPtrAllocate<CActuatorBlk> CSPActuatorBlk;
typedef CArray<CSPActuatorBlk, CSPActuatorBlk&> CActuatorArray;

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(CActuator);
class CActuator : public FlwNode
  {
  friend class CTgFnIoVar;
  public:
    CActuatorArray m_DataBlk;      
    CArray <IOAreaRec, IOAreaRec> m_IOAreas;

    flag           bOn;                  //
    static flag    bWithCnvComment;
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CActuator(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CActuator();
    virtual void   ResetData(flag Complete);
  private:
    void           SetCount(long NewSize);
    flag           SortRqd();
    void           Sort();
    void           FixIOTags();
  public:
    virtual pchar  Tag(pchar ReqdTag=NULL);
    
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);

    // CNodeXRefMngr Overides
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalDiscrete();
    virtual dword  ModelStatus();

    DEFINE_CI(CActuator, FlwNode, 4);

  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif
