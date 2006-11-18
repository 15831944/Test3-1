//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_SHARE2_H__35A13D43_76B6_4580_9346_73722745E1B7__INCLUDED_)
#define MD_SHARE2_H__35A13D43_76B6_4580_9346_73722745E1B7__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

// ======================================================================
//
//
//
// ======================================================================

typedef ULONG64 DDEF_Flags;

// First Octet
// Network solution modes
const long NM_Probal       = eScdNet_Probal;
const long NM_Dynamic      = eScdNet_Dynamic;
const long NM_All          = eScdNet_Probal|eScdNet_Dynamic;

// Second Octet
const long SM_Direct       = eScdSolve_Direct;
const long SM_Inline       = eScdSolve_Inline;
const long SM_Buffered     = eScdSolve_Buffered;
//const long SM_Inline       = 0x00000008;              

// Third Octet
const long HM_None         = eScdHeat_None;
const long HM_Reduced       = eScdHeat_Simple;
const long HM_Full         = eScdHeat_Full;

// Fourth & Fifth Octets
// Link Flw Modes
const long LFM_Xfer         = eScdFlow_Xfer;   
const long LFM_Simple       = eScdFlow_Simple;
const long LFM_Linear       = eScdFlow_Linear;
const long LFM_Full         = eScdFlow_Full;
const long LFM_All          = eScdFlow_Xfer | eScdFlow_Simple | eScdFlow_Linear | eScdFlow_Full;
const long LFM_Propagate    = eScdFlow_Propagate;
const long LFM_Propagated   = eScdFlow_Propagated;
const long LFM_AllPg        = LFM_AllPg | eScdFlow_Propagate | eScdFlow_Propagated;

//const long LFM_HasFullFlow  = eScdFlow_HasFullFlow;

const long SM_DynSurge      = SM_Inline | SM_Buffered;
const long SM_DynBoth       = SM_Inline | SM_Buffered;
const long SM_All           = SM_Direct | SM_Inline | SM_Buffered;
const long HM_All           = HM_None | HM_Reduced | HM_Full;

const long NSHM_All         = NM_All | SM_All | HM_All;       

const long MODEVISIBLE      = eScdFlow_ModeVisible;       

const DDEF_Flags DDEF_NOCOMPARE           = 0x0000000000020000;// Maybe masked out ??
const DDEF_Flags DDEF_NAN_OK              = 0x0000000000040000;

const DDEF_Flags DDEF_NONDB               = 0x0000000000080000;
const DDEF_Flags DDEF_NOFILE              = 0x0000000000100000;
const DDEF_Flags DDEF_NOVIEW              = 0x0000000000200000;
const DDEF_Flags DDEF_NOSNAPSHOT          = 0x0000000000400000;
const DDEF_Flags DDEF_NOSCENARIO          = 0x0000000000800000;
const DDEF_Flags DDEF_CHECKBOX            = 0x0000000001000000;
const DDEF_Flags DDEF_BUTTON              = 0x0000000002000000;
const DDEF_Flags DDEF_PARAMSTOPPED        = 0x0000000004000000;
const DDEF_Flags DDEF_PARAM               = 0x0000000008000000;
const DDEF_Flags DDEF_RESULT              = 0x0000000010000000;
const DDEF_Flags DDEF_SETONCHANGE         = 0x0000000020000000;
const DDEF_Flags DDEF_HIDDEN              = 0x0000000040000000;
const DDEF_Flags DDEF_HIDEIFZERO          = 0x0000000080000000;

const DDEF_Flags DDEF_PARAMCONSTRUCT      = 0x0000000100000000;
const DDEF_Flags DDEF_WRITEPROTECT        = 0x0000000200000000;
const DDEF_Flags DDEF_TAGPARM             = 0x0000000400000000; 
const DDEF_Flags DDEF_FUNCTPARM           = 0x0000000800000000;
const DDEF_Flags DDEF_AFFECTSSTRUCT       = 0x0000001000000000;
const DDEF_Flags DDEF_KEEPHISTORY         = 0x0000002000000000;
const DDEF_Flags DDEF_DUPHANDLES_OK       = 0x0000004000000000;
const DDEF_Flags DDEF_MULTILINESTR        = 0x0000008000000000;
const DDEF_Flags DDEF_INERROR             = 0x0000010000000000;
const DDEF_Flags DDEF_DRIVERTAG           = 0x0000020000000000;
const DDEF_Flags DDEF_ARCHIVETAG          = 0x0000040000000000;
const DDEF_Flags DDEF_NOPUTSHORTCUT       = 0x0000080000000000; // Facilty Unused
const DDEF_Flags DDEF_DUALVALUE           = 0x0000100000000000;
const DDEF_Flags DDEF_DUALVALUEOVR        = 0x0000200000000000;
const DDEF_Flags DDEF_TREENODE            = 0x0000400000000000;
const DDEF_Flags DDEF_TREECLOSED          = 0x0000800000000000;
const DDEF_Flags DDEF_CHILDMASK           = 0x0003000000000000;
const DDEF_Flags DDEF_ISSPECIE            = 0x0004000000000000;
const DDEF_Flags DDEF_ISSPECIEATT         = 0x0008000000000000;
const DDEF_Flags DDEF_ISSPECIECALC        = 0x0010000000000000;
const DDEF_Flags DDEF_STARTROW            = 0x0020000000000000;
const DDEF_Flags DDEF_ENDROWS             = 0x0040000000000000;
//const DDEF_Flags DDEF_NDBIMPORT           = 0x0020000000000000;

//                                            6 5  4 3  2 1
//                                            0628406284062840
const int     DDEF_CHILDSHIFT             = 40;

inline DDEF_Flags ChildLevel(int i) { return (DDEF_CHILDMASK) & (((ULONG64)i)<<DDEF_CHILDSHIFT); }

//===============================

typedef DDEF_Flags MD_Flags;


// Unit Model Field MF_xxx : unit model field flags information/options/etc for adding tags to the system...
const MD_Flags MF_RESULT        = DDEF_RESULT;         //result field in access window
const MD_Flags MF_PARAMETER     = DDEF_PARAM;          //input field in access window
const MD_Flags MF_PARAM_STOPPED = (DDEF_PARAMSTOPPED|DDEF_PARAM); //input field in access window when solver is stopped
const MD_Flags MF_SET_ON_CHANGE = DDEF_SETONCHANGE;    //apply change on the input field immediatly
const MD_Flags MF_NO_FILING     = (DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO);                          //field does not need to be saved/recovered to database
const MD_Flags MF_NO_VIEW       = DDEF_NOVIEW;         //field not visible to user in access window
const MD_Flags MF_INIT_HIDDEN   = DDEF_HIDDEN;         //field initialiy hidden
const MD_Flags MF_NAN_OK        = DDEF_NAN_OK;         //for floating point fields, NAN (shown as * in access window) is allowed
const MD_Flags MF_BUTTON        = DDEF_BUTTON;         //for bool fields, Present a button
//const MD_Flags MF_ISRESULT      = DDEF_RESULT;       // this is a calculated field which goes invalid on any change
const MD_Flags MF_HIDEIFZERO    = DDEF_HIDEIFZERO;     // hide this field if it is zero
const MD_Flags MF_STARTROW      = DDEF_STARTROW;       // Put this and subsequent values on the same row until MF_StartRow or MF_EndRows
const MD_Flags MF_ENDROWS       = DDEF_ENDROWS;        // Marks end of row mode

const MD_Flags MF_Result        = MF_RESULT;           //result field in access window
const MD_Flags MF_Parameter     = MF_PARAMETER;        //input field in access window
const MD_Flags MF_ParamStopped  = MF_PARAM_STOPPED;    //input field in access window when solver is stopped
const MD_Flags MF_SetOnChange   = MF_SET_ON_CHANGE;    //apply change on the input field immediatly
const MD_Flags MF_NoFiling      = MF_NO_FILING;        //field does not need to be saved/recovered to database
const MD_Flags MF_NoView        = MF_NO_VIEW;          //field not visible to user in access window
const MD_Flags MF_InitHidden    = MF_INIT_HIDDEN;      //by default field initialiy hidden
const MD_Flags MF_NanOK         = MF_NAN_OK;           //for floating point fields, NAN (shown as * in access window) is allowed
const MD_Flags MF_Button        = MF_BUTTON;           //for bool fields, Present a button
//const MD_Flags MF_ISRESULT      = MF_ISRESULT;       // this is a calculated field which goes invalid on any change
const MD_Flags MF_HideIfZero    = MF_HIDEIFZERO;       // hide this field if it is zero
const MD_Flags MF_StartRow      = MF_STARTROW;         // Put this and subsequent values on the same row until MF_StartRow or MF_EndRows
const MD_Flags MF_EndRows       = MF_ENDROWS;          // Marks end of row mode

// ======================================================================
// Data Definitions for Text equivalents of Integral Values
const DWORD MDD_Hidden    = 0x00000001;
const DWORD MDD_Default   = 0x00000002;
const DWORD MDD_NoSelect  = 0x00000004;
struct MDDValueLst { long m_lVal; LPTSTR m_pStr; DWORD m_dwFlags; };

// ======================================================================
// Generalised Property infrastructure

// Property Model Field MP_xxx : properies model field flags information/options/etc for adding tags to the system...
const DWORD MP_Null             = 0x00000000; 
const DWORD MP_Result           = 0x00000000; //result field
const DWORD MP_Parameter        = 0x00000001; //input field
const DWORD MP_ParamStopped     = 0x00000002; //input field when solver is stopped
const DWORD MP_NoFiling         = 0x00000004; //field does not need to be saved/recovered to database
const DWORD MP_NoView           = 0x00000008; //field not visible to user in access window
const DWORD MP_InitHidden       = 0x00000010; //by default field initialiy hidden
const DWORD MP_NanOK            = 0x00000020; //for floating point fields, NAN (shown as * in access window) is allowed
const DWORD MP_CheckBox         = 0x00000040; //for bool fields, Present a checkbox

const DWORD MP_NoAccess         = 0x00008000; //not a field

const DWORD MP_UserProp         = 0x00010000; //belongs to user properties group
const DWORD MP_ConfigProp       = 0x00020000; //belongs to configuration group
const DWORD MP_GlobalProp       = 0x00040000; //belongs to global configuration group

const DWORD MP_SoftMin          = 0x10000000;
const DWORD MP_SoftMax          = 0x20000000;
const DWORD MP_HardMin          = 0x40000000;
const DWORD MP_HardMax          = 0x80000000;

// ======================================================================
//
//
//
// ======================================================================

const long AH_Active = 0x01;
const long AH_Hold   = 0x02;

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__PROPERTYBASE_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

const long MPI_EndOfProps = -1;    
const long MPI_MoreProps  = -2;

class DllImportExport MPropertyInfo
  {
  public:

    class MStringValueP
      {
      public:
        LPCSTR    m_sID;
        long      m_iValue;
        DWORD     m_dwFlags;
      };

    class MStringValueS
      {
      public:
        CString    m_sID;
        long      m_iValue;
        DWORD     m_dwFlags;
      };

  public:
    MPropertyInfo();
    MPropertyInfo(ePropertyTypes iType, LPCTSTR sTag, LPCTSTR sSym, short iCnv, LPCTSTR sCnv, double dMin, double dMax, ULONG dwFlags, LPCTSTR sDesc );

    void            Set(ePropertyTypes iType, LPCTSTR sTag, LPCTSTR sSym, short iCnv, LPCTSTR sCnv, double dMin, double dMax, ULONG dwFlags, LPCTSTR sDesc );
    void            Set(ePropertyTypes iType, LPCTSTR sTag, LPCTSTR sSym, MStringValueP * StrValues, ULONG dwFlags, LPCTSTR sDesc );

    void            SetStructName(LPCTSTR Nm, ULONG dwFlags = MP_ConfigProp, LPCTSTR TableSuffix=NULL);
    void            SetText(LPCTSTR Txt, ULONG dwFlags = 0);
    void            SetPage(LPCTSTR Txt, ePropertyPageTypes Pg, ULONG dwFlags = 0);

    bool            Settable()                { return (m_dwFlags & (MP_Parameter|MP_ParamStopped))!=0; };
    bool            UserGrp()                 { return (m_dwFlags & MP_UserProp)!=0; };
    bool            ConfigGrp()               { return (m_dwFlags & MP_ConfigProp)!=0; };
    bool            GlobalGrp()               { return (m_dwFlags & MP_GlobalProp)!=0; };

    bool            HasSoftMin()              { return (m_dwFlags & MP_SoftMin )!=0; };
    bool            HasSoftMax()              { return (m_dwFlags & MP_SoftMax )!=0; };
    bool            HasHardMin()              { return (m_dwFlags & MP_HardMin )!=0; };
    bool            HasHardMax()              { return (m_dwFlags & MP_HardMax )!=0; };
    double          HardMin()                 { return m_dMin; };
    double          HardMax()                 { return m_dMax; };

  public:

    // Variables particular to the Property
    ePropertyTypes  m_iType;
    CString         m_sTag;
    CString         m_sSym;
    short           m_iCnv;
    CString         m_sCnv;
    double          m_dMin;
    double          m_dMax;
    ULONG           m_dwFlags;
    CString         m_sDesc;

    CArray <MStringValueS, MStringValueS&> m_StrValues;

    // Variables for Extra's Prior to the Text
    CString         m_sStruct;
    CString         m_sTableSuffix;
    CString         m_sText;
    byte            m_ePgOpt;
    bool            m_bIsObject;
  };

// ----------------------------------------------------------------------

class DllImportExport MPropertyValue
  {
  public:
    MPropertyValue(void)          { m_eType=ePT_Null; };
    MPropertyValue(bool V)        { *this=V; };
    MPropertyValue(int V)         { *this=V; };
    MPropertyValue(long V)        { *this=V; };
    MPropertyValue(ULONG V)       { *this=V; };
    MPropertyValue(double V)      { *this=V; };
    MPropertyValue(LPCTSTR V)     { *this=V; };
    MPropertyValue(LPTSTR V)      { *this=V; };

    ~MPropertyValue(void) {};

    ePropertyTypes Type() { return m_eType; };

    operator bool();     
    operator int();     
    operator long();     
    //operator ULONG();     
    operator double();     
    operator LPCTSTR();     
    operator LPTSTR();     

    MPropertyValue & operator=(bool V);      
    MPropertyValue & operator=(int V);      
    MPropertyValue & operator=(long V);      
    MPropertyValue & operator=(ULONG V);      
    MPropertyValue & operator=(double V);      
    MPropertyValue & operator=(LPCTSTR V);      
    MPropertyValue & operator=(LPTSTR V);      

  protected:
    ePropertyTypes  m_eType;
    long            m_Long;
    ULONG           m_ULong;
    double          m_Double;
    CString         m_String;
  };


// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__MD_DEFN_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

class DllImportExport MCnv
  {
  public:
    MCnv()                          { m_Index=0; m_Txt=""; }
    MCnv & operator()(LPCTSTR Txt)  { m_Txt=Txt; return *this; }
    operator short()                { return m_Index; };
    operator long()                 { return m_Index; };

  public://protected:
    short    m_Index;
    CString  m_Txt;
  };

// Model Conversions MC_xxx : engineering conversion unit types...
extern DllImportExport MCnv MC_         ; //dimensionless
extern DllImportExport MCnv MC_None     ; //dimensionless
extern DllImportExport MCnv MC_Ang      ; //Angle                   : rad 
extern DllImportExport MCnv MC_DP       ; //Pressure Differcence    : kPa
extern DllImportExport MCnv MC_PpS      ; //Pressure Slew Rate      : kPa/s
extern DllImportExport MCnv MC_PpM      ; //Pressure Slew Rate      : kPa/kg
extern DllImportExport MCnv MC_PpVol    ; //Pressure Slew Rate      : kPa/m^3
extern DllImportExport MCnv MC_E        ; //Energy                  : kJ
extern DllImportExport MCnv MC_Frac     ; //Fraction                :
extern DllImportExport MCnv MC_FracRate ; //Fraction Rate           : %/s 
extern DllImportExport MCnv MC_Assay    ; //Assay                :
extern DllImportExport MCnv MC_HCap     ; //Heat Capacity           : kJ/dC
extern DllImportExport MCnv MC_HCapF    ; //                        : kJ/dC/s
extern DllImportExport MCnv MC_L        ; //Length                  : m 
extern DllImportExport MCnv MC_Ldt      ; //Velocity                : m/s  
extern DllImportExport MCnv MC_M        ; //Mass                    : kg 
extern DllImportExport MCnv MC_Qm       ; //Mass Flow               : kg/s 
extern DllImportExport MCnv MC_QmSlew   ; //Mass Flow Slewrate      : kg/s/s
extern DllImportExport MCnv MC_Qv       ; //Volumetric Flow         : m^3/s 
extern DllImportExport MCnv MC_QvSlew   ; //Volumetric Flow Slewrate: m^3/s/s 
extern DllImportExport MCnv MC_Qe       ; //Enerygy Flow            : kJ/s
extern DllImportExport MCnv MC_NQv      ; //Normal Volumetric Flow  : Nm^3/s
extern DllImportExport MCnv MC_NVol     ; //Normal Volume           : Nm^3
extern DllImportExport MCnv MC_P        ; //Pressure                : kPa
extern DllImportExport MCnv MC_Pwr      ; //Power                   : kJ/s
extern DllImportExport MCnv MC_pS       ; //Per unit time           : /s
extern DllImportExport MCnv MC_Rho      ; //Density                 : kg/m^3
extern DllImportExport MCnv MC_T        ; //Temperature             : K
extern DllImportExport MCnv MC_Time     ; //Time                    : s
extern DllImportExport MCnv MC_Vol      ; //Volume                  : m^3
extern DllImportExport MCnv MC_WI       ; //Work Index              : kWs/kg
extern DllImportExport MCnv MC_HMs      ; //Enthalpy                : kJ/kg
extern DllImportExport MCnv MC_HMl      ; //Molar Entalpy           : kJ/kmol  
extern DllImportExport MCnv MC_HV       ; //Volume Enthalpy         : kJ/m^3
extern DllImportExport MCnv MC_HNV      ; //Norm Volume Enthalpy    : kJ/Nm^3
extern DllImportExport MCnv MC_SMs      ; //Entrophy                : kJ/kh.K
extern DllImportExport MCnv MC_SMl      ; //Molar Entropy           : kJ/kmol.K
extern DllImportExport MCnv MC_CpMs     ; //Cp                      : kJ/kg.C 
extern DllImportExport MCnv MC_CpMl     ; //Molar Cp                : kJ/kmol.C 
extern DllImportExport MCnv MC_Conc     ; //Concetration            : kg/m^3
extern DllImportExport MCnv MC_Area     ; //Area                    : m^2
extern DllImportExport MCnv MC_HTC      ; //Heat Transfer Coeff     : kW/m^2.K 
extern DllImportExport MCnv MC_HCond    ; //Heat Conductivity       : kW/m.K
extern DllImportExport MCnv MC_UA       ; //Heat Transfer           : kW/K
extern DllImportExport MCnv MC_dT       ; //Change in Temperature   : K
extern DllImportExport MCnv MC_dTdt     ; //Rate of change in temp  : C/s
extern DllImportExport MCnv MC_I        ; //Current                 : A
extern DllImportExport MCnv MC_V        ; //Potential Diff          : V
extern DllImportExport MCnv MC_Epm      ; //Energy per Mass         : kW/t 
extern DllImportExport MCnv MC_Visc     ; //Viscosity               : Ns/m^2
extern DllImportExport MCnv MC_MpL      ; //Mass Loading            : kg/m
extern DllImportExport MCnv MC_KgMl     ; //Moles                   : kmol
extern DllImportExport MCnv MC_QKgMl    ; //Molar Flow              : kmol/s
extern DllImportExport MCnv MC_Cond     ; //Conductivity            : S/m 
extern DllImportExport MCnv MC_SpCnt    ; //SpecificCOunt           : #/kg
extern DllImportExport MCnv MC_Cnt      ; //Count                   : #
extern DllImportExport MCnv MC_CntRate  ; //CountRate               : #/s
extern DllImportExport MCnv MC_SurfTens ; //Surface Tension         : N/m
extern DllImportExport MCnv MC_EntRate  ; //EntrainRate             : %.s/kg
extern DllImportExport MCnv MC_DPperVol ; //PressDropPerVolFlow     : kPa/m^3/s
extern DllImportExport MCnv MC_SurfAreaM; //Surface Area(Mass)      : m^2/g
extern DllImportExport MCnv MC_SurfAreaL; //Surface Area(Volume)    : m^2/L
extern DllImportExport MCnv MC_FracSol  ; //Fraction(Solids)        : 
extern DllImportExport MCnv MC_FracLiq  ; //Fraction(Liquids)       :
extern DllImportExport MCnv MC_FracVap  ; //Fraction(Vapour)        :
/*extern DllImportExport MC_FracPh00      ; //Fraction(Phase00)
extern DllImportExport MC_FracPh01      ; //Fraction(Phase01)
extern DllImportExport MC_FracPh02      ; //Fraction(Phase02)
extern DllImportExport MC_FracPh03      ; //Fraction(Phase03)
extern DllImportExport MC_FracPh04      ; //Fraction(Phase04)
extern DllImportExport MC_FracPh05      ; //Fraction(Phase05)
extern DllImportExport MC_FracPh06      ; //Fraction(Phase06)
extern DllImportExport MC_FracPh07      ; //Fraction(Phase07)
extern DllImportExport MC_FracPh08      ; //Fraction(Phase08)
extern DllImportExport MC_FracPh09      ; //Fraction(Phase09)
extern DllImportExport MC_FracPh10      ; //Fraction(Phase10)
extern DllImportExport MC_FracPh11      ; //Fraction(Phase11)
extern DllImportExport MC_FracPh12      ; //Fraction(Phase12)
extern DllImportExport MC_FracPh13      ; //Fraction(Phase13)
extern DllImportExport MC_FracPh14      ; //Fraction(Phase14)
extern DllImportExport MC_FracPh15      ; //Fraction(Phase15)*/
extern DllImportExport MCnv MC_ThermTau ; //Thermal TimeConst       : dC/s.C
extern DllImportExport MCnv MC_CosPhi   ; //Power Factor            : Cos(<)
extern DllImportExport MCnv MC_Ohms     ; //Electrical Resistance   : Ohms
extern DllImportExport MCnv MC_Bytes    ; //Memory Size             : bytes
extern DllImportExport MCnv MC_ConcRate ; //Concentration Change Rate: kg/m^3/s
extern DllImportExport MCnv MC_KinVisc  ; //Kinematic Viscosity (Stoke) : m^2/s
extern DllImportExport MCnv MC_ElectVA  ; //Electrical VA (V*A)     : kVA
extern DllImportExport MCnv MC_Humidity ; //Specific Humidity       : kg(l)/kg(v)

extern DllImportExport MCnv MC_Money    ; //Money                   : US$
extern DllImportExport MCnv MC_MoneyFlow; //Money Flow              : US$/s


class DllImportExport MCnvs
  {
  public:
    MCnvs();    
    int     Count();
    int     FindPrimary(LPCTSTR Name);
    //int FindSecondary(LPCTSTR Name);
    bool    Create(LPCTSTR NameCnv, MCnv & Cnv);
    double  Scale(const MCnv & Cnv);
    double  Offset(const MCnv & Cnv);
  };

extern DllImportExport MCnvs gs_Cnvs;

//---------------------------------------------------------------------------

// Forward declarations...
class TaggedObject; 
class CToleranceBlock;
class SpPropOveride;
class SpMArray;

class DllImportExport MToleranceBlock
  {
  public:
    MToleranceBlock(DWORD Use, LPCTSTR Name, double Abs, double Rel, long MaxIters=100, 
      DWORD Flags=(TBF_UseMax|TBF_UseAbs|TBF_UseRel),
      CCnvIndex AbsIndex=MC_Frac, LPCTSTR AbsTxt="%");

    virtual ~MToleranceBlock();

    void      SetMaxIters(double MaxIters);
    void      SetAbs(double Abs);
    void      SetRel(double Rel);
    void      SetAbsMult(double Abs);
    void      SetRelMult(double Rel);
    void      SetFlags(DWORD Flags);

    LPCTSTR   GetName();         
    long      GetMaxIters();     
    double    GetAbs();          
    double    GetRel();          
    double    GetAbsMult();      
    double    GetRelMult();      

    double    GetNormalError();        
    double    GetRelativeError();        
    DWORD     GetFlags();        

    bool      UseMax();          
    bool      UseAbs();          
    bool      UseRel();          
    bool      Hide();            
    bool      UseMaxParent();    
    bool      UseAbsParent();    
    bool      UseRelParent();    
    bool      HasParent();       

    bool ConvergedDV(double D, double V);
    bool ConvergedVV(double V1, double V2);

    operator CToleranceBlock&() { return *m_TolBlk; }

  protected:
    CToleranceBlock * m_TolBlk;

  };

//---------------------------------------------------------------------------

#undef DllImportExport
#if defined(__MATHLIBTO_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

const int RF_OK                 = 0;
const int RF_LoLimit            = 1;
const int RF_HiLimit            = 2;
const int RF_BadEstimate        = 3;
const int RF_BadStartValues     = 4;
const int RF_NotStarted         = 5;
const int RF_NotConverged       = 6;
const int RF_EstimateOK         = 7;
const int RF_EstimateLoLimit    = 8;
const int RF_EstimateHiLimit    = 9;
const int RF_Independant        =10;
// Root Finder Class
class DllImportExport MRootFinderBase
  {
  public:
    MRootFinderBase(char *DescStr, CToleranceBlock & Tol);
    virtual int     Start(double X1, double X2, double Fn1=dNAN, double Fn2=dNAN, bool RetIndependant=false);
    virtual double  Function(double x) = 0;
    virtual int     Solve_Brent();
    virtual int     SolveFromEst(double Est, double Sign, double dChgFrac=0.01, double dChgInit=dNAN);
    virtual bool    Converged(double X1, double X2, double Fn1, double Fn2);

    virtual LPCTSTR ObjTag()                                { return ""; };
    virtual void    Limits(double X1, double X2)            { xmin=X1; xmax=X2; bLimits=1; };

    void            SetTarget(double FunctionTarget)        { dTarget=FunctionTarget; };
    void            SetEstimate(double Est, double EstSlp)  { dEst=Est; dEstSlp=EstSlp; bEstSet=1; };
    double          EstimatedSlope()                        { return dEstSlp; };
    double          Result()                                { return dResult; };
    double          Target()                                { return dTarget; };
    double          FuncValue()                             { return m_FnVal; };
    double          FuncError()                             { return m_FnErr; };
    int             Error()                                 { return iError; };
    int             Iterations()                            { return iter; };
    bool            Converging()                            { return iter>=0; };
    bool            TestingLimits()                         { return iter==-1; };
    bool            TestingEstimate()                       { return iter==-2; };
    void            SetLimits(double Min, double Max)       { dMin=Min; dMax=Max; };

    void            SetErrorHandling(bool LogErrors, bool * MRootFinderBusy);
    static LPCTSTR  ResultString(int ErrorRet);

  protected:
    LPCTSTR        pDescStr;
    bool           bLogErrors;
    bool         * pMRootFinderBusy;

    int            iError;
    double         dTarget;
    double         dResult;
    double         m_FnVal;
    double         m_FnErr;

  private:  
    double         x1, x2, xmin, xmax;
    double         m_Err1, m_Err2, m_Val1, m_Val2;
    bool           bStarted, bLimits, bConverged;
    bool           bEstSet;
    double         dEst, dEstSlp;
    int            iter;
    double         dMin, dMax;

    CToleranceBlock & m_Tol;

  };


class DllImportExport MRootFinder: public MRootFinderBase
  {
  public:
    MRootFinder(char *DescStr, CToleranceBlock & Tol) : MRootFinderBase(DescStr, Tol)
      {
      };
    int FindRoot(double FnTarget, double XLoBnd, double XHiBnd, double XEst=dNAN, double XEstSlope=1.0, bool RetIndependant=false);
    int FindRootEst(double FnTarget, double XLoBnd, double XHiBnd, double XEst, double XEstSlope=1.0, double ChgMax=0.0, double ChgInit=0.0);
  };

// -----------------------------------------------------------

#undef DllImportExport 

// ======================================================================
//
//
//
// ======================================================================

#endif // !defined(MD_SHARE2_H__35A13D43_76B6_4580_9346_73722745E1B7__INCLUDED_)
