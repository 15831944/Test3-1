//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __ELECTRICS_H
#define  __ELECTRICS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif
#ifndef __SFE_BASE_H
  #include "sfe_base.h"
#endif
#ifndef __ELECBASE_H
  #include "elecbase.h"
#endif

#ifdef __POWER_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

class FlwNode;        // forward decl
class CPwrUserSocket; // forward decl

// ===========================================================================
//
//
//
// ===========================================================================

enum CPwrTypes {CPS_None, CPS_Elec, CPS_Air};
extern DllImportExport DDBValueLst DDBCTTypes[];

// ---------------------------------------------------------------------------

class DllImportExport CPwrLoad 
  {
  protected:
    double m_dShaftPower;

  public:
    CPwrLoad()        { m_dShaftPower=0; };

    virtual CPwrTypes Type() { return CPS_None; };
    virtual void      BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, LPCSTR Tag, bool AsParm) {};
    virtual flag      DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj) { return 0;};
    virtual flag      ValidateData(ValidateDataBlk & VDB, TaggedObject * pTagObj) { return true; };

    double    ShaftPower()            { return m_dShaftPower; }
    void      SetShaftPower(double P) { m_dShaftPower=P; }
  };

class DllImportExport CPwrSupply
  {
  protected:
    bool    m_bOn;

  public:

    CPwrSupply()     { };

    virtual CPwrTypes Type() { return CPS_None; };
    virtual void      BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, LPCSTR Tag, bool AsParm) {};
    virtual bool      On()            { return m_bOn; };
    virtual void      SetOn(bool On)  { m_bOn=On; };
    virtual bool      Present()       { return false; };
    virtual bool      SetPower(CPwrLoad * pLoad)                                  { return true; };
    //virtual bool      SetPower(CPwrUserSocket * pSocket)                          { return true; };
    virtual flag      DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj)       { return 0;};
    virtual flag      ValidateData(ValidateDataBlk & VDB, TaggedObject * pTagObj) { return true; };
  };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CElecLoad : public CPwrLoad  
  {
  public:
    double          m_dPhaseAngle; 

    CElecLoad();
    CPwrTypes       Type() { return CPS_Elec; };
    void            BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, LPCSTR Tag, bool AsParm);
    flag            DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj) { return 0;};
    flag            ValidateData(ValidateDataBlk & VDB, TaggedObject * pTagObj) { return true; };
  };

// ---------------------------------------------------------------------------

class DllImportExport CElecSupply : public CPwrSupply
  {
  public:
    CETermStrip    *m_pTermStrip;

    double          m_dPower;
    double          m_dPhaseAngle;

  public:
    CElecSupply(CETermStrip * pTS=NULL)   
      { 
      m_pTermStrip=pTS; 
      m_dPower=0;
      m_dPhaseAngle=0;
      };

    CPwrTypes       Type() { return CPS_Elec; };
    void            BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, LPCSTR Tag, bool AsParm);
    flag            DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj);
    flag            ValidateData(ValidateDataBlk & VDB, TaggedObject * pTagObj);
    bool            Present();// { return m_bOn; };
    bool            SetPower(CPwrLoad * pLoad);
                    
  };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CAirLoad : public CPwrLoad 
  {
  public:
    double m_dShaftPower;

    CAirLoad();

    CPwrTypes       Type() { return CPS_Air; };
    void            BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, LPCSTR Tag, bool AsParm);
    flag            DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj) { return 0;};
    flag            ValidateData(ValidateDataBlk & VDB, TaggedObject * pTagObj) { return true; };
  };

// ---------------------------------------------------------------------------

class DllImportExport CAirSupply: public CPwrSupply
  {
  public:
    double          m_dXXXX;

    CAirSupply();

    CPwrTypes       Type() { return CPS_Air; };
    void            BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, LPCSTR Tag, bool AsParm);
    flag            DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj) { return 0;};
    flag            ValidateData(ValidateDataBlk & VDB, TaggedObject * pTagObj) { return true; };
    bool            Present() { return false; };
    bool            SetPower(CPwrLoad * pLoad);
  };

// ===========================================================================
//
//
//
// ===========================================================================

class CPwrUserSocket;
 
class DllImportExport CPwrUser : public TaggedObject 
  {
  friend class CPwrUserSocket;
  friend class CPwrSupplyItem;
  friend class CPwrSupplies;

  public:
    enum CPwrUserConns {CPU_None, CPU_ByName, CPU_Wired, CPU_Socket};
    
  protected:
    FlwNode         * m_pNd; 
    Strng             m_sPwrSupply;
    Strng             m_sRemoteSrc;
    long              m_iInx;
    double            m_dPowerDraw;
    
    CPwrUserConns     m_eConnectRqd;
    CPwrUserConns     m_eConnectAct;
    CPwrSupply      * m_pSupply;
    CPwrLoad        * m_pLoad;
    CETermStrip     * m_pTermStrip;

    CPwrUserSocket  * m_pSocket;

  public:
    CPwrUser(TagObjClass *pClass_, char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach);
    CPwrUser(char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach);
    
    ~CPwrUser();

    void            Initialise(LPCTSTR SupplyName);
    void            BuildDataDefn(DataDefnBlk & DDB);
    void            BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwnObj, LPTSTR pName=NULL, dword dwUserInfo=-1);
    flag            DataXchg(DataChangeBlk & DCB);
    CPwrUserConns   SetConnectRqd(LPCTSTR pName);
    flag            ValidateData(ValidateDataBlk & VDB);
    CPwrSupply    * Supply();
    CPwrLoad      * Load();
    void            SetConnect(CPwrUserConns Connect, CPwrSupply *pPwrSupply);
    static bool     ChangeLoadToMatch(CPwrLoad ** pLoad, CPwrSupply *pSupply);

    flag            GlobalConnect(FlwNode * pNd, CPwrSupply * pSupply); 
    flag            GlobalDisConnect();

    LPCTSTR         State();
    flag            On();
    CPwrTypes       Type();
    double          SetPower(double ShaftPower);
    void            ConnectPower();
    void            Zero();

    // Elec
    CETermStrip    *TermStrip();

  };
DEFINE_TAGOBJ(CPwrUser);

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CPwrUserSocket : public TaggedObject 
  {
  public:

    //flag            m_bOn; 

    CPwrSupply    * m_pSupply;
    CPwrLoad      * m_pLclLoad;
    //CPwrLoad      * m_pRmtLoad;
    CPwrUser      * m_pUser;

    CPwrUserSocket(char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach);//, CPwrLoad * pLd, CPwrSupply * pCd);
    CPwrUserSocket(TagObjClass *pClass_, char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach);
    
    ~CPwrUserSocket();

    void            BuildDataDefn(DataDefnBlk & DDB);
    flag            DataXchg(DataChangeBlk & DCB);
    flag            ValidateData(ValidateDataBlk & VDB);

    bool            ConnectUser(CPwrUser * pUser);
    bool            DisConnectUser();

    void            SetSupply(CPwrSupply * pSupply);
    CPwrSupply   *  Supply();

    void            EvalCtrlActions();
    void            EvalPowerRequired();

    CPwrLoad      * Load() { return m_pUser ? m_pUser->m_pLoad : m_pLclLoad; };
    
    flag            On()  { return Supply() ? Supply()->On() : false; };
    void            SetOn(bool On) { if (Supply()) Supply()->SetOn(On); };
    double          ShaftPower() { return Load()? Load()->ShaftPower() : 0.0; };

  };
DEFINE_TAGOBJ(CPwrUserSocket);

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CPwrSupplyItem
  {
  public:
    Strng           m_Tag;
//    flag           m_On;
    //CPwrTypes      m_Type;
    CPwrSupply     *m_pSupply;

    CPwrSupplyItem();
    CPwrSupplyItem & operator=(CPwrSupplyItem & I);
    CPwrTypes       Type();
    CPwrSupply     *Supply() { return m_pSupply; };
    bool            On() { return m_pSupply->On(); };
    void            SetOn(bool On) { m_pSupply->SetOn(On); };

  };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CPwrSupplies
  {
  protected:
    CArray <CPwrSupplyItem, CPwrSupplyItem&> m_Supplies;
    CMap <LPCTSTR, LPCTSTR, long, long > m_Map;
  public:

    CPwrSupplies();

    void            AddSupply(LPCTSTR Tag, CPwrTypes Type, double Voltage);//, eTSConfigurations Phases);
    void            RemSupply(LPCTSTR Tag);
    long            NSupplies();
    void            SetNSupplies(long n);
    LPCTSTR         GetTag(long n);
    void            SetTag(long n, LPCTSTR Tag);

    CPwrSupplyItem & operator[](long i) { return m_Supplies[i]; };

    void            FixMap();
                    
    void            Initialise(bool Full);
    void            Terminate();
    void            ZeroAll();

    inline long FindSupply(LPCTSTR Tag)
      {
      long i;
      if (Tag && m_Map.Lookup(Tag, i))
        return i;
      return -1;
      }

    inline CPwrSupplyItem * GetSupply(CPwrUser & User)
      {
      long i;
      if (m_Map.Lookup(User.m_sPwrSupply(), i))
        return &m_Supplies[i];
      return NULL;
      }

    // user;
    //long            BuildDataDefn(CPwrUser & User, DataDefnBlk & DDB, TaggedObject * pTagObj);
    //LPCTSTR         State(CPwrUser & User);
    //flag            On(CPwrUser & User);
    //CPwrTypes       Type(CPwrUser & User);
    CPwrSupply    * Supply(CPwrUser & User);
  };

// ===========================================================================
//
//
//
// ===========================================================================

extern DllImportExport CPwrSupplies gs_PwrSupplies;

// ===========================================================================

#undef DllImportExport

 #endif
