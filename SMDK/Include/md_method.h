//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __MD_METHOD_H
#define  __MD_METHOD_H

#include "md_vector.h"
#include "md_share2.h"
#include "md_share3.h"
#include "md_share4.h"
#include "md_data.h"

#ifdef __MD_METHOD_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

// local forward's
class MBaseMethodImpl;
class MSurgeUnitDefBase;
class MReactionBlk;
class MMethodUtility;
class MProbalPCtrl;
class MUnitDefBase;
class MTagIO;

// remote forward's
class MdlNode;
class PkDataUnion; 
class DataDefnBlk;
class DataChangeBlk;
class ValidateDataBlk;
class CSetDatumsData;
class NodeDrawEntry;
class TagObjClass;
class CReactionBase;
class CFilesUsedArray;
class CClosureInfoArray;
class CPropagateNetInfoCtrl;
class SpModel;

class CEnvironHXBase;
class CVLEBase;
class CFT_Flash;
class CFT_Condenser;
class CFT_Mixer;
class CMacroMdl;
class CMacroMdlBase;
class CRqdPressCtrl;

class CNodeTagIOItem; 
class CNodeTagIOList;

//---------------------------------------------------------------------------
/* Class MInOutDefStruct : Used to Setup definition of IO (input/output) ports to a model.
Defined as a static array of MInOutDefStruct. The last entry must be NULL to indicate end of array.
NOTE: The IO Port name must be unique and valid tag (no illegal characters or spaces in name).
      Once used in projects, this name CAN NOT be changed as it forms part of tags for defining
      model connections, otherwise projects will not load! (If desparate an edit in some of
      the tables in the scd_mdls.mdb database can overcome the problem.)*/

//List of possible connection types
const DWORD MIO_In              = 0x00000001; //input only
const DWORD MIO_Out             = 0x00000002; //output only
const DWORD MIO_InOut           = MIO_In|MIO_Out; //input or output

const DWORD MIO_Material        = 0x00000010; //pipe link
const DWORD MIO_Control         = 0x00000020; //control link
const DWORD MIO_Electrical      = 0x00000040; //electrical link

const DWORD MIO_Transfer        = 0x00000100; //only allow pipes of type transfer link to be connected
const DWORD MIO_Overflow        = 0x00000200; //overflow pipe
const DWORD MIO_GasVent         = 0x00000400; //gas vent pipe
const DWORD MIO_PipeEntry       = 0x00000800; //
const DWORD MIO_PipeJoin        = 0x00001000; //
const DWORD MIO_ApertureHoriz   = 0x00002000; //pipe is connected "horizontal" (eg base of tank)

const long MCN_Join             = 0x00100000; //pipe connected to the others and balance is enforced (default)
const long MCN_Open             = 0x00200000; //pipe is connected to a surge
const long MCN_Closed           = 0x00400000; //pipe is connected to a 'blank'
const long MCN_IOMask           = 0x0000ffff; //pipe is connected to a 'blank'

// Tear Type
enum MTearTypes { MTT_NoTear = 0, MTT_SystemTear = 1, MTT_ManualTear = 2, MTT_Break = 3 };
// Tear Priority
enum MTearPriorities { MTP_First = 0, MTP_Normal = 1, MTP_Last = 2 };

struct DllImportExport MInOutDefStruct
  {
    LPTSTR        m_sDesc;      //connection port description (eg "Steam Vent"). Can be changed
    LPTSTR        m_sName;      //unique connection port Name. NOTE: This forms part of tag for pipe connections. (eg "Steam")
    long          m_lId;        //unique port ID
    long          m_nRqd;       //minimum number of connections required to this IO port
    long          m_nMax;       //maximum number of connections allowed to this IO port
    long          m_lCnId;      //used to group a number different port IDs (eg shell and tube sides of heatexchanger)
    float         m_dFracHgt;   //default connection height as a fraction between 0.0f and 1.0f (only relevant in dynamic to models with surge)
    DWORD         m_dwOptions;  //IO connection port options and type. MIO_xxx Multiple flags can be set using | (eg MIO_In|MIO_Material)
  };


//---------------------------------------------------------------------------
// Method Options

const DWORD MO_ShowIOs          = 0x00000004;
const DWORD MO_ShowIOOpts       = 0x00000008;
const DWORD MO_ShowContents     = 0x00000010;

//---------------------------------------------------------------------------

enum MStatesToSet { MSS_ZeroFlows=1 , MSS_Empty=2, MSS_Preset=3, MSS_PBInitialise=5, MSS_DynStatsRunInit=6, MMS_EmptySpillTargets=7 };

class MUtilArray : public CArray <MMethodUtility*, MMethodUtility*> {};

//---------------------------------------------------------------------------

class DllImportExport MBaseMethodCommon : public MBaseDataCommon, public MSubConstructBase
  {
  public:
    MBaseMethodCommon(MUnitDefBase *pUnitDef, TaggedObject * pNd);
  public:
    MUnitDefBase     *m_pUnitDef;
    MBaseMethodImpl  *m_pImpl;
    MdlNode          *m_pNd;

    MUtilArray        m_Utils;
    MInOutDefStruct  *m_pIODefs;
    long              m_nJoins;
  };

//---------------------------------------------------------------------------

class DllImportExport MBaseMethodCommonRef
  {
  public:
    MBaseMethodCommonRef(MBaseMethodCommon *pCom) :
        //m_pImpl(pCom->m_pImpl),
        m_pCommon(pCom),
        m_pNd(pCom->m_pNd)
        //,
        //m_pDDB(pCom->m_pDDB),
        //m_pDCB(pCom->m_pDCB),
        //m_pVDB(pCom->m_pVDB)
        {
        }

  public:
      MBaseMethodCommon * m_pCommon;
      //MBaseMethodImpl   * &m_pImpl;
      MdlNode           * &m_pNd;
      //DataDefnBlk       * &m_pDDB;
      //DataChangeBlk     * &m_pDCB;
      //ValidateDataBlk   * &m_pVDB;
  };

//===========================================================================
/* Class MJoin: Helper class for managing a join. A join is a number of streams 
   from one or more port Id connections.*/
class DllImportExport MJoin
  {
  public:
    MJoin (MdlNode*p, long iJoin) { m_pNd=p; m_iJoin=iJoin; };
    MJoin (const MJoin & J) { m_pNd=J.m_pNd; m_iJoin=J.m_iJoin; };

    // Properties
    double        getP();
    void          putP(double P);
    __declspec(property(get=getP,put=putP))             double P;


    // Operators
    double         GetProbalPIn(MProbalPCtrl *pPC=NULL);
    void           SetProbalP(MProbalPCtrl *pPC=NULL);
    void           SetProbalP(double P, bool DoInputs=false, bool DoOutputs=true);
    void           EvalProbalP(MProbalPCtrl *pPC=NULL, SpModel * pVec=NULL);

  protected:
    MdlNode     * m_pNd;
    long          m_iJoin;
  };

//---------------------------------------------------------------------------
/* Class MJoins: Array of Join classes.*/
class DllImportExport MJoins : public MBaseMethodCommonRef
  {
  public:
    MJoins(MBaseMethodCommon *pCom) : MBaseMethodCommonRef(pCom) {};

    // Properties
    long         getCount();
    void         putCount(long N);
    __declspec(property(get=getCount,put=putCount))   long Count;

    MJoin        operator[](int iJoin) { return MJoin(m_pNd, iJoin); }
  private:
  };

//===========================================================================

class CPressBlk;
class DllImportExport MPressureBlk
  {
  public:
    MPressureBlk(CPressBlk*p) { m_pPB=p; };
    MPressureBlk(const MPressureBlk & PB) { m_pPB=PB.m_pPB; };

    double getPEst();

    __declspec(property(get=getPEst)) double PEst;

  protected:
    CPressBlk * m_pPB;

  };

//---------------------------------------------------------------------------

class DllImportExport MFlowBlk
  {
  public:
    MFlowBlk(MdlNode*p, long IONo) { m_pNd=p; m_iIONo=IONo; };
    MFlowBlk(const MFlowBlk & FB) { m_pNd=FB.m_pNd; m_iIONo=FB.m_iIONo; };

    double          getQm();
    double          getQmEst();
    double          getDPb();
    void            putDPb(double DP);
    void            SetDPb(double DP, double dDP);
    double          getDPq();
    void            putDPq(double DP);
    void            SetDPq(double DP, double dDP);
    double          getDPz();
    void            putDPz(double DP);
    void            SetDPz(double DP, double dDP);

    void            SetQmFree();
    void            SetXferMode();
    void            SetXferCapacity(double QmCap);
    void            SetQmReqd(double QmReqd, bool IsDegFree=true, float MaxNode=fNAN, float MaxFlng=fNAN);

    void            SetDerivsBad(bool On=true);
    void            SetFunctOfPress(bool On=true);

    double          MeanPress();
    double          MeanFlngPress();
    double          MeanNodePress();
    double          MeanFBPress();

    double         getVelocity();

    double         getRegulation();
    void           putRegulation(double A);

    int            QmSign();
    double         QmMeas(double Scl=1.0);
    double         QvMeas(double Scl=1.0);
    double         VelMeas(double Scl=1.0);
    double         DQmMeas(double Scl=1.0);
    static double  RangeFlow(double Qm, double LoLimit);
    double         SetQmMeasRange(double Rho, double LoQm);
    double         SetQvMeasRange(double Rho, double LoVol);
    double         SetVelMeasRange(double Rho, double Area, double LoVel);

    MPressureBlk  getNodePBlk();
    MPressureBlk  getFlngPBlk();

    __declspec(property(get=getQm))                           double Qm;
    __declspec(property(get=getQmEst))                        double QmEst;
    __declspec(property(get=getDPb,put=putDPb))               double DPb;
    __declspec(property(get=getDPq,put=putDPq))               double DPq;
    __declspec(property(get=getVelocity))                     double Velocity;
    __declspec(property(get=getRegulation,put=putRegulation)) double Regulation;

    __declspec(property(get=getNodePB))           MPressureBlk NodePBlk;
    __declspec(property(get=getFlngPB))           MPressureBlk FlngPBlk;

  protected:
    MdlNode  *m_pNd;
    long      m_iIONo;
  };

//---------------------------------------------------------------------------

class DllImportExport MFlow
  {
  public:
    MFlow (MdlNode*p, long IONo)    { m_pNd=p; m_iIONo=IONo; };
    MFlow (const MFlow & F)         { m_pNd=F.m_pNd; m_iIONo=F.m_iIONo; };

    bool          getValid()        { return m_pNd && m_iIONo>=0; };
    long          getId();
    long          getIdRmt();
    bool          getIsFlwIn();
    bool          getIsFlwOut();
    double        getEstFlwIn();
    double        getEstFlwOut();
    double        getFlwIn();
    double        getFlwOut();
    MStream       getStream();
    MFlowBlk      getFlowBlk()      { return MFlowBlk(m_pNd, m_iIONo); };
    double        getAbsHeight();
    void          putAbsHeight(double H);
    double        getRelHeight();
    void          putRelHeight(double H);
    double        getDatum();
    double        getIOPJoin();
    double        getIOPFlange();

    bool          getIsMakeUpReqd();
    bool          getIsMakeUpReqdLcl();
    bool          getIsMakeUpAvail();

    double        getMakeUpReqd();
    void          putMakeUpReqd(double Rqd);
    double        getMakeUpReqdLcl();
    void          putMakeUpReqdLcl(double Rqd);
    double        getMakeUpAvail();
    void          putMakeUpAvail(double Avl);


    MTearTypes        getTearTypeRequired();
    void              putTearTypeRequired(MTearTypes Type);
    MTearPriorities   getTearPriority();
    void              putTearPriority(MTearPriorities Priority);

    __declspec(property(get=getValid))                                      bool              Valid;
    __declspec(property(get=getId))                                         long              Id;
    __declspec(property(get=getIdRmt))                                      long              IdRmt;
    __declspec(property(get=getIsFlwIn))                                    bool              IsFlwIn;
    __declspec(property(get=getIsFlwOut))                                   bool              IsFlwOut;
    __declspec(property(get=getEstFlwIn))                                   double            EstFlwIn;
    __declspec(property(get=getEstFlwOut))                                  double            EstFlwOut;
    __declspec(property(get=getFlwIn))                                      double            FlwIn;
    __declspec(property(get=getFlwOut))                                     double            FlwOut;
    __declspec(property(get=getFlowBlk))                                    MFlowBlk          FlowBlk;
    __declspec(property(get=getStream))                                     MStream           Stream;
    __declspec(property(get=getAbsHeight,put=putAbsHeight))                 double            AbsHeight;
    __declspec(property(get=getRelHeight,put=putRelHeight))                 double            RelHeight;
    __declspec(property(get=getDatum))                                      double            Datum;
    __declspec(property(get=getIOPJoin))                                    double            IOPJoin;
    __declspec(property(get=getIOPFlange))                                  double            IOPFlange;
    __declspec(property(get=getIsMakeUpReqd))                               bool              IsMakeUpReqd;
    __declspec(property(get=getIsMakeUpReqdLcl))                            bool              IsMakeUpReqdLcl;
    __declspec(property(get=getIsMakeUpAvail))                              bool              IsMakeUpAvail;
    __declspec(property(get=getMakeUpReqd,put=putMakeUpReqd))               double            MakeUpReqd;
    __declspec(property(get=getMakeUpReqdLcl,put=putMakeUpReqdLcl))         double            MakeUpReqdLcl;
    __declspec(property(get=getMakeUpAvail,put=putMakeUpAvail))             double            MakeUpAvail;

    __declspec(property(get=getTearTypeRequired,put=putTearTypeRequired))   MTearTypes        TearTypeRequired;
    __declspec(property(get=getTearPriority,put=putTearPriority))           MTearPriorities   TearPriority;

    //// Methods
    void          SetOpen(long iJoinId, bool Damped=false);
    void          SetClosed(long iJoinId);
    void          SetJoin(long iJoinId);

    void          SetP(double P);
    void          SetP(double P, double SLDensity, double Head);

  protected:
    MdlNode  *m_pNd;
    long      m_iIONo;

  };

//---------------------------------------------------------------------------

//class DllImportExport MSurgeIF : public MBaseMethodCommonRef
//  {
//  friend class BlackBoxBase;
//  friend class CBBBase;
//  public:
//    MSurgeIF (MBaseMethodCommon *pCom) : MBaseMethodCommonRef(pCom) { m_nSrg=0; m_pSDD=NULL; };

//    long     getCount();
//    void     putCount(long n);
//    double   getHeight(long iSurge);
//    void     putHeight(long iSurge, double H);
//    double   getAbsHeight(long iSurge, long iIO);
//    void     putAbsHeight(long iSurge, long iIO, double H);
//    double   getRelHeight(long iSurge, long iIO);
//    void     putRelHeight(long iSurge, long iIO, double H);

//    __declspec(property(get=getCount,put=putCount))           long Count;
//    __declspec(property(get=getHeight,put=putHeight))         double Height[/*iSurge*/];
//    __declspec(property(get=getAbsHeight,put=putAbsHeight))   double AbsHeight[/*iSurge*/][/*iIO*/];
//    __declspec(property(get=getRelHeight,put=putRelHeight))   double RelHeight[/*iSurge*/][/*iIO*/];

//  private:

//    CSetDatumsData * m_pSDD;
//    long   m_nSrg;
//  } Surge;

//---------------------------------------------------------------------------
/* Class MFlowIOs: Helper class for accessing streams connected to input and output connections (ie ports). 
   There may be zero, one or more streams connected to a given port Id. A stream connected to a port Id may
   be an input or output (different streams for the same port Id could be input or output).
*/
class DllImportExport MFlowIOs : public MBaseMethodCommonRef
  {
  public:
    MFlowIOs (MBaseMethodCommon *pCom) : MBaseMethodCommonRef(pCom) {};

    // Return number of connected streams for the specified Port Id (-1 for all)
    long         getCount(long Id=-1);

    // Return index of first connected stream for the specified Port Id (-1 for all)
    long         getFirst(long Id=-1);
    // Return index of next connected stream after the specified index for the specified Port Id (-1 for all)
    long         getNext(long CurIndex, long Id=-1);
    // Return index of first input connected stream for the specified Port Id (-1 for all)
    long         getFirst_In(long Id=-1);
    // Return index of next input connected stream after the specified index for the specified Port Id (-1 for all)
    long         getNext_In(long CurIndex, long Id=-1);
    // Return index of first output connected stream for the specified Port Id (-1 for all)
    long         getFirst_Out(long Id=-1);
    // Return index of next output connected stream after the specified index for the specified Port Id (-1 for all)
    long         getNext_Out(long CurIndex, long Id=-1);

    // Return total mass flow for the stream of the specified index
    MFlow        operator[](int Index) { return MFlow(m_pNd, Index); }

    // Sum of all connected streams, filtered by the specified phases, to the specified Port Id (-1 for all) into Cd. 
    // MStream will be at min P of all streams. Return total mass flow.
    double   AddMixtureIn_Id(MStream & Cd, long Id=-1, DWORD PhaseM=MP_All);

  public:
    __declspec(property(get=getCount))          long Count[/*Id*/];
    __declspec(property(get=getFirst))          long First[/*Id*/];
    __declspec(property(get=getFirst_In))       long First_In[/*Id*/];
    __declspec(property(get=getNext_In))        long Next_In[/*CurIndex*/][/*Id*/];
    __declspec(property(get=getFirst_Out))      long First_Out[/*Id*/];
    __declspec(property(get=getNext_Out))       long Next_Out[/*CurIndex*/][/*Id*/];

  private:
    //void          CheckMIOStreams();
  };


//===========================================================================
// Control IO Interface
class DllImportExport MCtrlIOs : public MBaseMethodCommonRef
  {
  public:
    MCtrlIOs(MBaseMethodCommon *pCom) : MBaseMethodCommonRef(pCom) {};

    long         getNoCtrlIOs();

    __declspec(property(get=getNoCtrlIOs))      long NoCtrlIOs;

  };

//===========================================================================
/* Class MTagIO: Helper class for geting or setting external tags. 
   This can be with once off get and set commands; or through a subscription 
   list where tags are continuously updated.
*/

class DllImportExport MTagIOItem
  {
  protected:
    MTagIOItem(MTagIO & TagIO, bool DoAllocate);// cannot instantiate standalone

  public:
    virtual ~MTagIOItem();

  public:
    MTagIOResult    CheckTag();

    long            getSubsIndex();
    LPCSTR          getTag();
    void            putTag(LPCSTR Tag);

    bool            getNumDataType();
    bool            getIntDataType();
    bool            getFloatDataType();
    bool            getStrngDataType();
            
    short           getDataType(); 
    MD_Flags        getIOFlags();  
    MCnv            getCnv();      


    //return true if tag is valid (Tag Exists & CnvTxt is acceptable)
    bool            getIsValid();
    //return value IO tag
    long            getLong();
    //set value of IO tag
    void            putLong(long Value);
    //return value in SI units of IO tag
    double          getDoubleSI();
    //set value SI units of IO tag
    void            putDoubleSI(double Value);
    //return value in SI units of IO tag
    double          getDoubleCnv(LPCSTR Cnv=NULL);
    //set value SI units of IO tag
    void            putDoubleCnv(double Value, LPCSTR Cnv=NULL);
    //return value IO tag
    LPCSTR          getString();
    //set value of IO tag
    void            putString(LPCSTR Value);
    
    //return conversion index of IO tag
    short           getCnvIndex();
    //return conversion text of IO tag
    LPCSTR          getCnvText();

    //return conversion index of IO tag
    long            getUserHandle();
    //return conversion text of IO tag
    void            putUserHandle(long Handle);

    __declspec(property(get=getSubsIndex))                    long          SubsIndex;
    
    __declspec(property(get=getNumDataType))                  bool          NumDataType;
    __declspec(property(get=getIntDataType))                  bool          IntDataType;
    __declspec(property(get=getFloatDataType))                bool          FloatDataType;
    __declspec(property(get=getStrngDataType))                bool          StrngDataType;
    __declspec(property(get=getIsParam))                      bool          IsParam; 
    __declspec(property(get=getDataType))                     short         DataType;
    __declspec(property(get=getFlags))                        MD_Flags      Flags;    
    __declspec(property(get=getCnv))                          MCnv          Cnv;      
    __declspec(property(get=getTag,put=putTag))               LPCSTR        Tag;      


    __declspec(property(get=getLong,put=putLong))             long          Long;
    __declspec(property(get=getDoubleSI,put=putDoubleSI))     double        DoubleSI;
    __declspec(property(get=getDoubleCnv,put=putDoubleCnv))   double        DoubleCnv[];
    __declspec(property(get=getString,put=putString))         LPCSTR        String;

    __declspec(property(get=getCnvIndex))                     short         CnvIndex;
    __declspec(property(get=getCnvText))                      LPCSTR        CnvText;

    __declspec(property(get=getUserHandle,put=putUserHandle)) long          UserHandle;

    __declspec(property(get=getIOFlags))                      MD_Flags      IOFlags;


  public:
    MTagIO          * m_pTagIO;
    CNodeTagIOItem  * m_pItem;
    bool              m_bOwnsItem;

  };

//===========================================================================

class DllImportExport MTagIODirect : public MTagIOItem
  {
  public:
    MTagIODirect(MTagIO & TagIO);
    virtual ~MTagIODirect();
    
    MTagIOResult    ReadValue();
    MTagIOResult    WriteValue();
  };

//===========================================================================

class DllImportExport MTagIOSubscription : public MTagIOItem
  {
  friend class MTagIO;

  public:
    MTagIOSubscription(MTagIO & TagIO);
    virtual ~MTagIOSubscription();

  public:
    long            Configure(long UserHandle, LPCSTR ItemTag, LPCSTR Name, long Options);

    //return true if tag is valid and active
    bool            getIsActive();
    __declspec(property(get=getIsActive))                     bool          IsActive;
    //return true IO tag is a Get 
    bool            getIsGet();
    //return true IO tag is a Set 
    bool            getIsSet();
    //return true IO tag is a Parameter 
    bool            getIsParm();
    __declspec(property(get=getIsGet))                        bool          IsGet;
    __declspec(property(get=getIsSet))                        bool          IsSet;
    __declspec(property(get=getIsParm))                       bool          IsParm;
  };

class DllImportExport MTagIO : public MBaseMethodCommonRef
  {
  friend class MTagIOItem;
  friend class MTagIOSubscription;
  public:
    MTagIO(MBaseMethodCommon *pCom) : MBaseMethodCommonRef(pCom) {};

    static CString FormatAsTagOnly(const CString & Tag);
    static CString FormatAsTagAndCnv(const CString & Tag);
    static LPCSTR ResultString(MTagIOResult Res);

    //Enable and activate TagIO subscription
    void            Open();
    //Disable and stop TagIO subscription
    void            Close();
    //return count of IO tags
    long            getCount();
    //sets/return Active state of IO tags
    bool            getActive();
    //return count of IO tags
    void            putActive(bool On);
    
    // must used in ValidateDataFields - returns false when TagIOValidation must NOT occur
    bool            ValidateReqd();
    // must used to bracket Config's etc
    bool            StartValidateDataFields();
    bool            EndValidateDataFields();


    __declspec(property(get=getCount))                          long       Count;
    __declspec(property(get=getActive,put=putActive))           bool       Active;

  protected:
    void            Add(MTagIOSubscription * pItem);
    void            Remove(MTagIOSubscription * pItem);
    void            RemoveAll();
  };

//===========================================================================

class DllImportExport MClosureInfo
  {
  public:
    MClosureInfo(CClosureInfoArray & CIA, MVector & RefMdl);

    bool DoFlows();
    bool DoContent();
    //int  JoinID();
    void AddHeatFlwIn(int Join, double HfFlow, double HsFlow);
    void AddHeatFlwOut(int Join, double HfFlow, double HsFlow);
    void AddPowerIn(int Join, double PowerIn);
    void AddMassFlwIn(int Join, MVector & MassFlow);
    void AddMassFlwOut(int Join, MVector & MassFlow);
    void AddContentHeat(int Join, double Heat);
    void AddContentMass(int Join, MVector & Mass);

  protected:
    CClosureInfoArray & m_CIA;
  };

//===========================================================================

/* Class MBaseMethod: Base method class from which custom user unit models are derived.*/
class DllImportExport MBaseMethod : public MBaseMethodCommon
  {
  friend class BlackBoxBase;
  friend class CBBBase;
  public:
	  MBaseMethod(MUnitDefBase * pUnitDef, TaggedObject * pNd);       //constructor
    virtual ~MBaseMethod();   //destructor
    void            Init(MdlNode * pNd); //Basic initialisation
    virtual void    Init() {}; //initialisation, used to setup UnitDef
    virtual void    BuildDataFields() = 0;                   // pure virtual method for defining data fields
    virtual bool    ExchangeDataFields() { return false; };  // virtual method for manipulating data
    virtual bool    ValidateDataFields() { return true; };   // virtual method for validating data

    //interface (used by Init):
    void            SetIODefinition(MInOutDefStruct * pDefs);
    void            RegisterFlashTrainNode(int IOId, bool fModel, const type_info * pNdVarTypeId);

    //interface (used by all):
  public://data members and helper classes
    CString         m_sErrorMsg;

    MLog            Log;
    MDebug          Dbg;
    MDataDefn       DD;
    MDataChange     DX;
    MDataValidate   DV;
    MJoins          Joins;
    MFlowIOs        FlwIOs;
    MCtrlIOs        CtrlIOs;
    MTagIO          TagIO;

  private:
    long            m_lJoinMask;

  public://--- Properties ---------
    //Model tag
    LPCTSTR         getTag();
    //Model class ID
    LPCTSTR         getClassId();

    //Network solver modes
    bool            getIsNetProbal();
    bool            getIsNetDynamic();
    //EvalProduct solve Methods
    bool            getIsSolveDirect();
    bool            getIsSolveInline();
    bool            getIsSolveBuffered();

    //Solver time in seconds from 1970
    double          getTime();
    //Iteration time increment in seconds
    double          getDeltaTime();
    //Joinmask Used in EvalProducts
    long            getJoinMask();
    //Pause solver (go to Idle mode) at end of iteration
    void            SetIdleRequired(char* pNote = NULL);
    //Stop solver at end of iteration
    void            SetStopRequired(char* pNote = NULL);



  public://--- Runtime Virtual methods ---------
    virtual bool    PreStartCheck()               { m_sErrorMsg = ""; return true; }; // to check immediatly before a solve or run

    virtual void    PostConnect(long IONo)        {};
    virtual void    PreDisConnect(long IONo)      {};
    virtual void    SetDatums()                   {};
    virtual void    SetSizes()                    {};

    virtual void    InitialiseSolution()          {}; // each

    virtual bool    PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);

    virtual void    StartSolution()               {}; // each Start of run
    virtual void    StartStep()                   {}; // each Iteration - At Start of Step
    virtual bool    ConfigureJoins();                 // each Iteration - Configure IO Connections

    virtual void    EvalMakeUpReqd()              {};
    virtual void    EvalMakeUpAvail()             {};

    virtual bool    EvalJoinPressures()           { return false; }; // each Dyn Iteration - Setup Pressures at input to FlwIO's
    virtual bool    EvalJoinFlows(long JoinNo)    { return false; }; // each Dyn Iteration - Set up Flow relationships
    virtual bool    EvalFlowEquations(eScdFlwEqnTasks Task, long IONo, long FE, long LnkNo) { return false; }; // iterations with each Dyn Iteration - Converge Pressure-Flow relationships

    virtual void    EvalProducts()                {}; // each Iteration
    virtual void    EvalIntegral(double StepSize) {}; // each Dyn Iteration - Advance the model StepSize

    virtual void    EvalDiscrete()                {}; // each Dyn Iteration -
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All)          {}; // each Start of Run
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)             {}; // each Iteration - To apply result of control
    virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All)            {}; // each Iteration - To calculate required control actions
    virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              {}; // each Iteration End to calculate Stats relevant to model

    virtual void    EvalPowerAvailable()          {};
    virtual void    EvalPowerRequired()           {};
    virtual void    EvalState()                   {}; // each Dyn Iteration - Finalise the state of the model

    virtual void    ClosureInfo(MClosureInfo & CI) {};

    virtual void    SetState(MStatesToSet SS)     {}; // on demand to initialise the state

    virtual bool    GetModelAction(CMdlActionArray & Acts)   { return false; };
    virtual bool    SetModelAction(CMdlAction & Act)         { return false; };

    virtual bool    GetModelGraphic(CMdlGraphicArray & Grfs) { return false; };
    virtual bool    OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf) { return false; };

  public:// MacroModels (eg Flash Train)
    virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval)  {};
    virtual bool    MacroMdlValidNd(int iIONo)                { return true; };
    virtual void    MacroMdlAddNd(int iIONo)                  {};
    virtual void    MacroMdlRemoveNd(int iIONo)               {};
    virtual CMacroMdlBase* MacroMdlActivate()                 { return NULL; };
    virtual void    MacroMdlDeactivate()                      {};

  public:// Utilities
    void            MakeUpNodeTransferAvail(int JoinId); // Transfers Makeup Availabilities to Destinations
    void            MakeUpNodeTransferReqd(int JoinId);  // Transfers Makeup Requirements to Source

    bool            getDbgBrk();

    //Model tag
    __declspec(property(get=getTag))            LPCTSTR Tag;
    //Model class ID
    __declspec(property(get=getClassId))        LPCTSTR ClassId;
    //Network solver modes
    __declspec(property(get=getIsNetProbal))      bool      IsNetProbal;
    __declspec(property(get=getIsNetDynamic))     bool      IsNetDynamic;
    //EvalProduct solve Methods
    __declspec(property(get=getIsSolveDirect))    bool      IsSolveDirect;
    __declspec(property(get=getIsSolveInline))    bool      IsSolveInline;
    __declspec(property(get=getIsSolveBuffered))  bool      IsSolveBuffered;

    //Solver time in seconds from 1970
    __declspec(property(get=getTime))          double   Time;
    //Iteration time increment in seconds
    __declspec(property(get=getDeltaTime))     double   DeltaTime;
    __declspec(property(get=getDbgBrk))        bool     DbgBrk;
    __declspec(property(get=getJoinMask))      long     JoinMask;

  };

//---------------------------------------------------------------------------
/*class DllImportExport MBaseMethodEx : public MBaseMethod
  {
  // This class is designed to be an 'Extended' vesion of MBaseMethod which would
  //  'open up' more complex capabilities'
  };*/

//===========================================================================
//
//===========================================================================

//Model solve mode options... (any combination; but at least ONE of the following)
const DWORD MSolveMode_Probal       = 0x00000001; //is probal model
const DWORD MSolveMode_DynamicFlow  = 0x00000002; //is dynamic model (flow / material transfer)
const DWORD MSolveMode_DynamicFull  = 0x00000004; //is dynamic model (pressure / full hydrolic)
//const DWORD MSolveMode_Electrical   = 0x00000008; //is dynamic model (electrical)
const DWORD MSolveMode_All          = MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull;

//Model group option... (should use ONE of the following)
const DWORD MGroup_General          = 0x00000001; //general group
const DWORD MGroup_Energy           = 0x00000002; //energy transfer group
const DWORD MGroup_Separation       = 0x00000004; //mass separation group
const DWORD MGroup_PSD              = 0x00000008; //size distribution group
const DWORD MGroup_Transfer         = 0x00000010; //mass transfer group
const DWORD MGroup_Alumina          = 0x00000020; //alumina group
const DWORD MGroup_Electrical       = 0x00000040; //Power Reticulation/Elect/Pneumatic

//Model Licensing add-on options... (any combination; none or any number of the following)
const DWORD MLicense_Standard           = 0x00000001; //Part of standard models (eg tank, tie, etc)
const DWORD MLicense_BlackBox           = 0x00000002; //"BlackBox" models developed using SMDK
const DWORD MLicense_HeatExchange       = 0x00000004; //Heat Exchange models (eg flash-tank, shell&tube, etc)
const DWORD MLicense_ExtraHeatExchange  = 0x00000008; //Extra Heat Exchange models (eg turbine, boiler, etc)
const DWORD MLicense_PSD                = 0x00000010; //Particle size-distribution/communition models (eg screen, crusher, etc)
const DWORD MLicense_Alumina            = 0x00000020; //Alumina models (eg Precipitator, etc)
const DWORD MLicense_Electrical         = 0x00000040; //Electrical/Power Distribution models (eg transformer, switch, etc)
const DWORD MLicense_User               = 0x00000080; //custom/client models

//---------------------------------------------------------------------------
/*Class MUnitDefBase: Helper class for class definition for the unit "class factory".
DEFINE_TRANSFER_UNIT(Object, Class, DLL) or DEFINE_SURGE_UNIT(Object, Class, DLL) are
used to add your SMDK unit class to the system. This definition class requires that you
provide the override for the virtual function GetOptions(). This is used to set options
by overriding defaults. Defaults are:
  DefaultTag: "X" or "S" (Transfer or Surge)
  Drawing: a box
  TreeDescription: "User:Class"
  Description: ""
  ModelSolveMode: MSolveMode_Probal
  ModelGroup: MGroup_General
  ModelLicense: MLicense_Standard
*/
class DllImportExport MUnitDefBase : public MSubConstructBaseDef
  {
  friend class CBBBase;
  friend class CBBSurge;
  friend class CBBTransfer;
  public:
    MUnitDefBase(bool WithDevelopementChecks);
    virtual           ~MUnitDefBase();
    virtual           MSubConstructBase * Construct(TaggedObject * pNd) = 0;

  public:
    // Model Definition Construct. Use this to set the model options
    virtual void      GetOptions() = 0;
    //return the full class name, DLL*Class (eg "Demo*Reactor")
    LPCSTR            ClassName()             { return (LPCSTR)m_sClassName; };

  public:
    // The default prefix characters when a tag is generated eg "TG" will produce TG_1, TG_2, TG_3
    // AddUsrPrefix defines whether all fields are prefixed by 'Usr.' or not
    void              SetDefaultTag(LPCTSTR DefTag, bool AddUsrPrefix=false);
    // Set graphics group of symbols (eg "Tank", "HeatTransfer") and provide a default drawing symbol
    void              SetDrawing(LPCTSTR DrwGroup, double Drawing[]);
    // A Description in the form of TreeNode:[TreeNode:...]UnitDescription eg. "Process:Unit:Tank"
    void              SetTreeDescription(LPCTSTR TreeDesc);
    // A Long Description
    void              SetDescription(LPCTSTR Desc);
    // Set Model Solve Mode that this Unit model supports (One or more of the MSolveMode_xxx flags)
    void              SetModelSolveMode(DWORD Mode);
    // Set Model Groups (One of the MGroup_xxx flags)
    void              SetModelGroup(DWORD Group);
    // Set License Options... (should use ONE of the following)
    void              SetModelLicense(DWORD License);

  protected:
    CString           m_sClassName;    // Class name

    TagObjClass     * m_pClassDef;
    NodeDrawEntry   * m_pDrawEntry;
    CString           m_sDllName;

  private:
    bool              m_WithDevelopementChecks;
    bool              m_AddUsrPrefix;
    //void              DevelopementChecksOn();
    //void              DevelopementChecksOff();
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
//Class MTransferUnitDefBase: Unit Definition base class for Transfer unit models.
class DllImportExport MTransferUnitDefBase : public MUnitDefBase
  {
  public:
    MTransferUnitDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks);
    virtual ~MTransferUnitDefBase() {};
  };

//---------------------------------------------------------------------------

#ifdef SMDK_Debug
#define DEVCHECKSON   1
#else
#define DEVCHECKSON   0
#endif

//---------------------------------------------------------------------------
/*Macro for adding a SMDK transfer unit to the system. Chose the 'Class' and 'DLL'
names carefully as once these are released and used in projects they cannot easily
be changed, if they are changed old projects will not load (unless the scd_models.mdb
is carefully edited to make the equivalent changes!)
The parameters are:
  Obj   : The C++ class object name (ie your class as declared in the source code)
          (eg SimpleReactor)
  Class : A short string name for your class. Must follow valid tag naming rules
          (ie no spaces, etc) (eg "Reactor")
  DLL   : A short string name for the group this belongs to. This is usually the DLL group and is
          as a #define DLL_GroupName in a shared header file such as stdafx.h
          (eg #define DLL_GroupName "DemoUsr")
  */
#define DEFINE_TRANSFER_UNIT(Obj, Class, DLL) \
class Obj##_UnitDef :  public MTransferUnitDefBase \
  { \
  public: \
    Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL); \
    ~Obj##_UnitDef(); \
    virtual MSubConstructBase* Construct(TaggedObject * pNd); \
    virtual void GetOptions(); /* to be supplied by the user*/ \
  }; \
extern Obj##_UnitDef Obj##_UnitD; \
Obj##_UnitDef::Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL) : \
  MTransferUnitDefBase(pClass, pSubClass, pShortDesc, pDLL, DEVCHECKSON) { GetOptions(); }; \
Obj##_UnitDef::~Obj##_UnitDef() { }; \
MSubConstructBase* Obj##_UnitDef::Construct(TaggedObject * pNd) { return new Obj(this, pNd); }; \
Obj##_UnitDef Obj##_UnitD(DLL##"*"##Class, NULL, "User:"##Class, DLL);

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//Class MSurgeUnitDefBase: Unit Definition base class for unit models with surge (contents).
class DllImportExport MSurgeUnitDefBase : public MUnitDefBase
  {
  public:
    MSurgeUnitDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks);
    virtual ~MSurgeUnitDefBase() {};
  };

//---------------------------------------------------------------------------

/*Macro for adding a SMDK surge unit to the system. See description for DEFINE_TRANSFER_UNIT.*/
#define DEFINE_SURGE_UNIT(Obj, Class, DLL) \
class Obj##_UnitDef :  public MSurgeUnitDefBase \
  { \
  public: \
    Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL); \
    ~Obj##_UnitDef(); \
    virtual MSubConstructBase* Construct(TaggedObject * pNd); \
    virtual void GetOptions(); /* to be supplied by the user*/ \
  }; \
extern Obj##_UnitDef Obj##_UnitD; \
Obj##_UnitDef::Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL) : \
  MSurgeUnitDefBase(pClass, pSubClass, pShortDesc, pDLL, DEVCHECKSON) { GetOptions(); }; \
Obj##_UnitDef::~Obj##_UnitDef() { }; \
MSubConstructBase* Obj##_UnitDef::Construct(TaggedObject * pNd) { return new Obj(this, pNd); }; \
Obj##_UnitDef Obj##_UnitD(DLL##"*"##Class, NULL, "User:"##Class, DLL);

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//Class MControlUnitDefBase: Unit Definition base class for unit models with surge (contents).
class DllImportExport MControlUnitDefBase : public MUnitDefBase
  {
  public:
    MControlUnitDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks);
    virtual ~MControlUnitDefBase() {};
  };

//---------------------------------------------------------------------------

/*Macro for adding a SMDK surge unit to the system. See description for DEFINE_TRANSFER_UNIT.*/
#define DEFINE_CONTROL_UNIT(Obj, Class, DLL) \
class Obj##_UnitDef :  public MControlUnitDefBase \
  { \
  public: \
    Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL); \
    ~Obj##_UnitDef(); \
    virtual MSubConstructBase* Construct(TaggedObject * pNd); \
    virtual void GetOptions(); /* to be supplied by the user*/ \
  }; \
extern Obj##_UnitDef Obj##_UnitD; \
Obj##_UnitDef::Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL) : \
  MControlUnitDefBase(pClass, pSubClass, pShortDesc, pDLL, DEVCHECKSON) { GetOptions(); }; \
Obj##_UnitDef::~Obj##_UnitDef() { }; \
MSubConstructBase* Obj##_UnitDef::Construct(TaggedObject * pNd) { return new Obj(this, pNd); }; \
Obj##_UnitDef Obj##_UnitD(DLL##"*"##Class, NULL, "User:"##Class, DLL);

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
/*Class MMethodUtility: Base class for "Utility Helper" classes to provide
access to "utility" classes in SysCAD (eg reaction block, VLE, etc)*/
class DllImportExport MMethodUtility : public MBaseMethodCommonRef
  {
  public:
    MMethodUtility(MBaseMethodCommon *pCom, LPCTSTR Tag);
    virtual           ~MMethodUtility();
    virtual void      BuildDataFields()                  { };
    virtual bool      ExchangeDataFields()               { return false; };
    virtual bool      ValidateDataFields()               { return true; };

    virtual void      OnAppActivate(bool bActive)        {};
    virtual int       FilesUsed(CFilesUsedArray & Files)  { return 0; }
    virtual void      SetTag(LPCTSTR Tag)              { m_sTag=Tag; };
    virtual LPCTSTR   Tag()                            { return m_sTag; };

  protected:
    CString           m_sTag;
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
/*Class MProbalPCtrl: Helper class for managing input or outpu stream pressures.*/
class DllImportExport MProbalPCtrl : public MMethodUtility
  {
  friend class MJoin;

  public:
    MProbalPCtrl(MBaseMethodCommon *pCom, LPCTSTR Tag="PCtrl", long Method=PC_SatP, double PRqd=StdP+100, bool MdlAvail=true);
    ~MProbalPCtrl();

    void    BuildDataFields(long Options);
    void    CalculateResult();

    long              getMethod();
    void              putMethod(long Method);
    double            getPRqd();
    void              putPRqd(double PRqd);
    double            getTRqd();
    void              putTRqd(double TRqd);
    double            getPMeas();
    void              putPMeas(double PMeas);
    double            getP2Set();
    void              putP2Set(double P2Set);

    _declspec(property(get=getMethod,put=putMethod))    long   Method;
    _declspec(property(get=getPRqd,  put=putPRqd))      double PRqd;
    _declspec(property(get=getTRqd,  put=putTRqd))      double TRqd;
    _declspec(property(get=getPMeas, put=putPMeas))     double PMeas;
    _declspec(property(get=getP2Set, put=putP2Set))     double P2Set;

    void SetP2Set(double P2Set, bool DoInputs, bool DoOutputs);

  protected:
    CRqdPressCtrl * m_pPCtrl;

  };

//---------------------------------------------------------------------------
/* Class MReactionBlk: Helper class to provide access to the SysCAD reaction block functionality.*/
class DllImportExport MReactionBlk : public MMethodUtility
  {
  public:
    MReactionBlk(MBaseMethodCommon *pCom, bool IsSurgeType, LPCTSTR Tag="RB");
    ~MReactionBlk();

  public:
    void              OnOffCheckBox(DWORD Flags=MF_PARAM_STOPPED);
    void              BuildDataFields();
    void              EvalProducts(MStream & S, double EstFinalT=dNAN);

  public:
    double            HfSumTot(bool RefToProd=false);
    double            HfSumSrc(bool RefToProd=false);
    double            HfSumSnk(bool RefToProd=false);
    double            HfSumRct(bool RefToProd=false);
    double            HfErr(bool RefToProd=false);
    double            HsSumHX();
    double            HsSumPwr();
    double            EstFinalT();

  public:
    void              Enable();
    void              Disable();
    bool              Enabled();
    bool              OnAndOK();

  protected:
    bool              ExchangeDataFields();
    bool              ValidateDataFields();

  private:
    bool              Open();
    bool              Load(LPCTSTR pRCTFn);
    void              Close();
    void              OnAppActivate(bool bActive);
    int               FilesUsed(CFilesUsedArray & Files);

  protected:
    CReactionBase   * m_pRB;
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
/* Class MEnvironHXBlk: Helper class to provide access to the SysCAD EHX block functionality.*/
class DllImportExport MEnvironHXBlk : public MMethodUtility
  {
  public:
    MEnvironHXBlk(MBaseMethodCommon *pCom, DWORD Options, LPCTSTR Tag="EHX");
    ~MEnvironHXBlk();

  public:
    void            OnOffCheckBox(DWORD Flags=MF_PARAM_STOPPED);
    void            BuildDataFields();
    void            EvalProducts(MStream & S, double FinalTEst=dNAN);
    void            EvalProducts(MStream & S, double Len, double Diam, double FinalTEst=dNAN);

    double          HeatFlow();

  public:
    void            Enable();
    void            Disable();
    bool            Enabled();

  protected:
    bool            ExchangeDataFields();
    bool            ValidateDataFields();

  protected:
    CEnvironHXBase * m_pEHX;
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
/* Class MVLEBlk: Helper class to provide access to the SysCAD VLE block functionality.*/
class DllImportExport MVLEBlk : public MMethodUtility
  {
  public:
    MVLEBlk(MBaseMethodCommon *pCom, DWORD SelectMask, LPCTSTR Tag="VLE");
    ~MVLEBlk();

  public:
    void            OnOffCheckBox(DWORD Flags=MF_PARAM_STOPPED);
    void            BuildDataFields();

  public:
    // Flash the contents
    double          FlashVapFrac(MVector &Mdl);
    // Flash the contents such that the mass fraction of vapour is VapFrac
    void            SetFlashVapFrac(MVector &Mdl, double Temp, double Press, double VapFrac, DWORD Flags);
    // Flash the contents such that the mass fraction of vapour is VapFrac
    void            SetFlashVapFrac(MVector &Mdl, double VapFrac, DWORD Flags);
    // Flash the contents such that the mass fraction of vapour is VapFrac
    void            SetFlashVapFrac(MStream &Q, double Temp, double Press, double VapFrac, DWORD Flags);
    // Flash the contents such that the mass fraction of vapour is VapFrac
    void            SetFlashVapFrac(MStream &Q, double VapFrac, DWORD Flags);

    // Flash the contents given Temperature & Pressure (adjust VapMassFrac)
    void            TPFlash(MVector &Mdl, double Temp, double Press, DWORD Flags);
    // Flash the contents given Duty & Pressure (adjust VapMassFrac & T) 
    // ie T will be the Saturation Temp at the final conditions
    // InjectedDuty is that added due to mechanical inefficiencies is machinery like pumps 
    void            PFlash(MStream &Q, double Press, double InjectedDuty=0.0, DWORD Flags=0);
    // Flash the contents given Duty & Pressure (adjust VapMassFrac & T) 
    // ie T will be the Saturation Temp at the final conditions
    // InjectedDuty is that added due to mechanical inefficiencies is machinery like pumps 
    void            PFlash(MStream &Ql, MStream &Qv, double Press, double InjectedDuty=0.0, DWORD Flags=0);
    // Flash the contents to equilibrium within the Fixed Container Volume
    void            VFlash(MContainer &Cn, double InjectedDuty=0.0, DWORD Flags=0);

    // Flash the contents Such that mass of Vapour is VapMass (adjust T & P)  
    void            MVapFlash(MStream &Q, double VapMass, double InjectedDuty=0.0, DWORD Flags=0);
    // Flash the contents Such that mass of Vapour is VapMass (adjust T & P)
    void            MVapFlash(MStream &Ql, MStream &Qv, double VapMass, double InjectedDuty=0.0, DWORD Flags=0);

    // returns the Saturation Pressure At T igonres the Partial pressure of other gasses
    double          SaturationP(MVector & V, double T);
    // returns the Total Pressure At T ie SaturationP + Partial Pressure of other gasses
    double          SaturationTotalP(MVector & V, double T);

  public:
    void            Enable();
    void            Disable();
    bool            Enabled();

  protected:
    bool            ExchangeDataFields();
    bool            ValidateDataFields();

  protected:
    CVLEBase      * m_pVLE;
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
/* Class MFT_Flash: Helper class for managing a steam source in a flash train macro model.*/
class DllImportExport MFT_Flash : public MMethodUtility
  {
  public:
    MFT_Flash(MBaseMethodCommon *pCom, LPCTSTR Tag="FSrc");
    ~MFT_Flash();

    operator          CMacroMdlBase *();
    const type_info * TypeInfo();

    void              BuildDataFields();
    bool              ExchangeDataFields();
    bool              ValidateDataFields();

    void              SetState(eScdMdlStateActs RqdState);

    bool              getActive();

    double            getFeedP();
    void              putFeedP(double FeedP);
    double            getFeedSaturationP();
    void              putFeedSaturationP(double FeedSaturationP);
    double            getFlashP();
    void              putFlashP(double FlashP);
    double            getFlashT();
    void              putFlashT(double FlashT);
    double            getEstFlashP();
    void              putEstFlashP(double EstFlashP);
    double            getVapourFlow();
    void              putVapourFlow(double VapourFlow);
    double            getMinFlshTrainP();
    double            getSuctionP();

    double            getPEval();
    void              putPEval(double PEval);
    double            getVapourFlowEval();
    void              putVapourFlowEval(double VapourFlowEval);
    double            getVapourFlowSensEval();
    void              putVapourFlowSensEval(double VapourFlowSensEval);
    bool              getVapourFlowSensGood();
    void              putVapourFlowSensGood(bool VapourFlowSensGood);

  public:
    _declspec(property(get=getActive))                                        bool   Active;
    _declspec(property(get=getFeedP,put=putFeedP))                            double FeedP;
    _declspec(property(get=getFeedSaturationP,put=putFeedSaturationP))        double FeedSaturationP;
    _declspec(property(get=getFlashP,put=putFlashP))                          double FlashP;
    _declspec(property(get=getFlashT,put=putFlashT))                          double FlashT;
    _declspec(property(get=getEstFlashP,put=putEstFlashP))                    double EstFlashP;
    _declspec(property(get=getVapourFlow,put=putVapourFlow))                  double VapourFlow;
    _declspec(property(get=getMinFlshTrainP))                                 double MinFlshTrainP;
    _declspec(property(get=getSuctionP))                                      double SuctionP;

    _declspec(property(get=getPEval,put=putPEval))                            double PEval;
    _declspec(property(get=getVapourFlowEval,put=putVapourFlowEval))          double VapourFlowEval;
    _declspec(property(get=getVapourFlowSensEval,put=putVapourFlowSensEval))  double VapourFlowSensEval;
    _declspec(property(get=getVapourFlowSensGood,put=putVapourFlowSensGood))  bool   VapourFlowSensGood;

  protected:
    CFT_Flash       * m_pFlash;
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
/* Class MFT_Condenser: Helper class for managing a steam sink in a flash train macro model.*/
class DllImportExport MFT_Condenser : public MMethodUtility
  {
  public:
    MFT_Condenser(MBaseMethodCommon *pCom, LPCTSTR Tag="FSnk");
    ~MFT_Condenser();

    operator          CMacroMdlBase *();
    const type_info * TypeInfo();

    void              BuildDataFields();
    bool              ExchangeDataFields();
    bool              ValidateDataFields();

    void              SetState(eScdMdlStateActs RqdState);

    bool              getActive();

    double            getSuctionP();
    void              putSuctionP(double SuctionP);
    double            getVapourFlow();
    void              putVapourFlow(double VapourFlow);
    double            getVapourFlowReqd();
    void              putVapourFlowReqd(double VapourFlowReqd);
    double            getMinSaturationP();
    void              putMinSaturationP(double MinSaturationP);

  public:
    _declspec(property(get=getActive))                            bool   Active;
    _declspec(property(get=getSuctionP,put=putSuctionP))          double SuctionP;
    _declspec(property(get=getVapourFlow,put=putVapourFlow))                      double VapourFlow;
    _declspec(property(get=getVapourFlowReqd,put=putVapourFlowReqd))              double VapourFlowReqd;
    _declspec(property(get=getMinSaturationP,put=putMinSaturationP))          double MinSaturationP;

  protected:
    CFT_Condenser *   m_pCondenser;
  };

// --------------------------------------------------------------------------

//class DllImportExport MFT_Mixer : public MMethodUtility
//  {
//  public:
//
//    MFT_Mixer(MBaseMethodCommon *pCom, LPCTSTR Tag="FMix");
//    ~MFT_Mixer();
//    void            BuildDataFields();
//    bool            ExchangeDataFields();
//    bool            ValidateDataFields();
//    void            Activate(CMacroMdl * pMMNd, pFlwNode pNd);
//    void            Deactivate();
//
//  protected:
//    CFT_Mixer * m_pMix;
//
//  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

#undef DllImportExport

#endif
