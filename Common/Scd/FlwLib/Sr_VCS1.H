//========== SysCAD - Copyright Kenwalt (Pty) Ltd / Bruce Botes =============
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SR_VCS1_H
#define  __SR_VCS1_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
//
#ifndef __VECTORS_H
  #include "vectors.h"
#endif
#ifndef __SP_MODEL_H
  #include "sp_model.h"
#endif

#ifdef __SR_VCS1_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif



class CIndexXlate : public CArray<int, int>
  {
  public:
    CIndexXlate & operator=(const CIndexXlate & A)
      {
      SetSize(A.GetSize());
      for (int i=0; i<A.GetSize(); i++)
        SetAt(i, A[i]);
      return *this;
      }
  };

const long MAXSPECIE   = 100;
const long MAXELEMENT  = 30;
const long MAXPHASE    = 16;

const long VCS_NO_ERR     = 0;
const long VCS_NO_CONV    = 1;
const long VCS_NO_TCONV   = 2;
const long VCS_PHASE_ERR  = 3;

class CVCSException
  {
  public:
    char Msg[256];
    CVCSException(LPCTSTR pMsg){strcpy(Msg, pMsg);};
    ~CVCSException(){};
    const char *ShowReason() const { return Msg; }
  };

class CVCSSolver
  {
  public:
    enum VCSPhase   { SinglePhase = 0, GasPhase, LiquidPhase, SolidPhase};// CNM , SLAG1, SLAG2, SLAG3, SLAG4 };
    enum VCSPath    { LInitData, LReInitData, L1955,L429,L6714,L430,L093,L090,L096,L441,L212,L227,L24192,
                      L1366,L857,LXferResult, LAdjTemp, LFinished};
    enum VCSMinMaj  { NMINOR = -1, MINOR, MAJOR };  // -1, 0, 1

  protected:

    long        m_nMaxPhase;
    long        m_nMaxSpecie;
    long        m_nMaxElement;

    double      m_TempKStart,           // temperature
                m_TempKFinal,           // temperature
                m_Press,                // pressure
                m_TEST;
    double      m_GasConst;             // kJ/kmole or J/mole
    double      m_Alpha;

    CLVector    m_SI;                   // VCSPhase m_Type of species i
    CLVector    m_IC;                   // m_IC[reaction_no]
    VCSPath     m_Route;

    CDVector    m_TMole,                // total moles for each Phase before the iteration
                m_TInert,               // inert moles for each Phase
                m_DeltaMole,            // change in moles for each Phase per iteration
                m_TMole1,               // total mole for each Phase after the iteration
                                        // temp. total moles per Phase
                m_W,                    // moles of species
                m_Wt,
                m_FEL,                  // previous iterations chem. pot (m_FF[i]*molefrac i)
                m_FE,                   // this iterations chem. pot (m_FF[i]*molefrac i)
                m_FF,                   // standard chem. pot (per mole)
                m_InitialChemPot,       // orginal chemical potential for activity porporses
                m_DS,                   // extent of reaction i
                m_DG,                   // delta g for the reaction i
                                        // there are less reactions than species
                                        // m_DG[reaction_no]
                m_GA,                   // current elemental abundance value
                m_GAI,                  // element abundance input value
                m_DA,                   // starting free energy storage
                m_AW,                   // temporary variable used by basopt() and InEst()
                m_SA,                   // temporary variable used by basopt() and elcor()
                m_SS;                   // temporary variable used by basopt() and elcor()

    CDMatrix    m_SC,                   // stoichiometric coef. vector for species
                                        // and temporary matrix for InEst/LinProg
                                        // m_SC[component_no][reaction_no]
                m_BM,                   // species formula vector
                m_SM,                   // temporary variable used by basopt() and elcor()
                m_DMole;                // Phase variable used in formula 6.4-16 in book
                                        // is calculated in BASOPT()

                // specie ordering
                //  |<--components-->|<--reactions--->|
                //  (1,2,.........,m_NC,m_NC1,.........,m_MR)
                //  m_MR = m_M if no species was deleted
                //  m_NC = m_NE (number of elements)
                // number of reactions => m_NR = m_MR - m_NC
                // number of orginal reactions => m_N = m_M - m_NE
                //                                    = m_NR + no of deleted species
                // deleted species is from m_MR+1 to m_M

    int         m_M,                    // number of species in input
                m_NE,                   // number of elements
                m_NoPhases,             // number of phases
                m_N,                    // equals m_M - m_NC
                m_NC1,                  // equals m_NC + 1
                m_TLoop,                // Temp Loops Count
                m_Iter,                 // iteration number
                m_Iti,                  // iteration variable
                m_It1,                  // iteration variable
                m_NR,                   // number of starting reactions
                m_NC,                   // number of components
                m_MR,                   // number of current reactions
                m_Minor,                // number of minors
                m_Type,                 // m_Type of chemical pot
                                        // relative to the orginal position
                m_NOpt,                 // no off basis optimisation
                m_IM,                   // flag - if true all reactions are MINORS
                m_LBO,                  // flag
                m_LEC,                  // flag
                m_ILT,                  // flag - if true no conver between minors
                                        //      - if false conver between majors (see equi_chk_min/maj)
                m_FORCED,               // flag
                m_CONV,                 // convergence flag
                m_ICONV,                // convergence flag
                m_ICONV1,               // convergence flag
                m_IEst,                 // user/machine estimate
                m_ITL,                  // m_DS[] step too large do convergence forcing
                m_SC_Max_I,             // maxinum i dimension of array m_SC
                m_SC_Max_J,             // maxinum j dimension of array m_SC
                m_nMaxIt,               // Maximum number of iterations
                m_nNotDeleteItCnt,      // Number of successive DeleteIt's that failed
                m_nMaxNotDeleteItCnt,   // Max Number of successive DeleteIt's that failed

                m_bFindTemp;            // Set if Final Temp must be found

    CLVector    m_NoSpeciePhase,        // number of species per Phase
                m_IR,                   // index to reaction species
                m_Ind,                  // index to orginal specie input array
                m_Index2;               // second index to show new position

    CLVector    m_ExistsInSrc;


    SpModel   * m_pModel;
    CDMatrix    m_SpElDef;
    CLVector    m_SpPhDef;
    CIndexXlate m_SpId2L;
    CIndexXlate m_L2SpId;
    CIndexXlate m_ElId2L;
    CIndexXlate m_L2ElId;
    SpVector    m_SpMass;               // Initial Mass for each specie
    SpVector    m_SpMoles;              // Initial Mass for each specie

    double      m_HOR;
    double      m_HORDiff;
    double      m_Hf_FdT;
    double      m_Hf_PrT;
    double      m_Hf_RefT;
    double      m_Hf_ZeroT;


    //char        m_sError[1024];

  public:

    CVCSSolver()
      {
      m_nMaxPhase=0;
      m_nMaxSpecie=0;
      m_nMaxElement=0;
      m_M=0;
      m_NE=0;
      m_NoPhases=0;
      m_N=0;
      m_NC1=0;
      m_TLoop=0;
      m_Iter=0;
      m_Iti=0;
      m_It1=0;
      m_NR=0;
      m_NC=0;
      m_MR=0;
      m_Minor=0;
      m_Type=0;
      m_NOpt=0;
      m_IM=0;
      m_LBO=0;
      m_LEC=0;
      m_ILT=0;
      m_FORCED=0;
      m_CONV=0;
      m_ICONV=0;
      m_IEst=0;
      m_ITL=0;
      m_SC_Max_I=0;
      m_SC_Max_J=0;
      m_nMaxIt=0;
      m_nNotDeleteItCnt=0;
      m_nMaxNotDeleteItCnt=20;

      m_TempKStart=0.0;
      m_TempKFinal=0.0;
      m_Press=0.0;
      m_TEST=0.0;
      m_GasConst=0.0;
      m_Alpha=0.0;

      m_bFindTemp=0;

      m_HOR=0;
      m_HORDiff=0;
      m_Hf_FdT=0;
      m_Hf_PrT=0;
      m_Hf_RefT=0;
      m_Hf_ZeroT=0;
      };

    long            Initialise(CDMatrix & SpElDef, CLVector & SpPhDef,
                          CIndexXlate & SpId2L, CIndexXlate & L2SpId,
                          CIndexXlate & ElId2L, CIndexXlate & L2ElId);
                    
    void            InitialiseDataPhase(void );
    void            InitialiseDataMoles(bool Complete);
    void            InitialiseDataInternal(void );
    void            InitialiseData(void);
    void            ReInitialiseData(void);
    long            Solve(SpModel * pModel, SpVector & Moles, double TempStart, double TempFinal, double Press, flag FindTemp);
    long            AdjustTemp(void);
    double          DeltaMoles(int SpId) { return m_pModel->VMole[SpId]-m_SpMoles[SpId]; };
    double          FinalMoles(int SpId) { return m_pModel->VMole[SpId]; };
    double          DHfFdT() { return m_Hf_FdT; };
    double          DHfPrT() { return m_Hf_PrT; };
    double          DHfRefT() { return m_Hf_RefT; };
    double          DHfZeroT() { return m_Hf_ZeroT; };
                    
    long            Fidelity() { return m_pModel->Fidelity(); };
  protected:
    enum VCSPath    Init(void);
    enum VCSPath    ChkOptBasis(void);
    enum VCSPath    EvalInitVect(void);
    enum VCSPath    Calculate(void);
    enum VCSPath    EquiChkMaj(void);
    enum VCSPath    EquiChkMin(void);
    enum VCSPath    ElemAbund(void);
    enum VCSPath    ChkDelSpecie(void);
    enum VCSPath    DumpChanges(void);
    void            CalcFinalInfo(void);
    void            prin_immed(void);

    // --------- routines in solve3-1.c ----------
    int             DeleteIt(int L,int mm);
    void            Force(void);
    int             ST2(void);
    int             ElCorr(void);
    void            DeltaG(int L, int j, int kp);
    void            BasOpt(int IFIRST);
    void            ELAB(void);
    void            DFE(CDVector & z, int kk, int ll);
    void            MLEQU(CDMatrix & c, CDMatrix & b, int m_M);
    void            Swap2(CLVector & x, int j1, int j2);
    void            Rotate(CLVector & x, int j1, int j2);
    int             AMax(CDVector & x, int j, int m_N);
    void            DSW(CDVector & x, int j1, int j2);
    void            DSW2(CDMatrix & x, int j1, int j2);
    void            InEst(void);
    void            LinProg(CDVector &psol, CDMatrix & ax);
                    
    void            GetActivityCoef(void);

    // Local function decalarations
    int             ClearDataArrays(void);
    void            CheckPhaseDist( void );
    //double          parse_opcode(INDEX i);
    //void            calc_sen(void);
    void            TransferResult(bool WithDebug);
    //int             calc_coef_mat(void);
    //void            ludcmp(double a[][MAXELEMENT+MAXSPECIE], int m_N, int indx[], float *d);
    //void            lubksb( double a[][MAXELEMENT+MAXSPECIE], int m_N, int indx[], double b[]);

  };

#undef DllImportExport

#endif