//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __GRFCMDS_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "grfcmds.h"
#include "project.h"
#include "dlgbusy.h"
#include "grfdlgs.h"
#include "scd_text.h"
#include "accnode.h"
#include "scd_wm.h"
#include "toolbars.h"
#include "dynmseg.h"
#include "licbase.h"
#include "chngtag.h"
#include "mdlvalue.h"
#include "tagobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgdxfmem 0x0
#define dbgdrw    0x0

#define WithTees 0

#if dbgdxfmem
    extern "C" 
    {
    extern int malloc_dxf_entry(void *addr,char *name,int size);
    extern int realloc_dxf_entry(void *newaddr,void *oldaddr,char *name,int size);
    extern int free_dxf_entry(void *addr);
    extern int total_dxf_entry();
    extern int print_dxf_entry(char *comment,char *nm);
    extern int clear_dxf_entry();
    }
#endif
    extern "C" 
    {
    extern int zero_malloc_total();
    extern int get_malloc_total();
    }

/////////////////////////////////////////////////////////////////////////////

inline SFEFlwLib * FE() { return gs_pPrj->FlwLib(); };

/////////////////////////////////////////////////////////////////////////////
// CPdsImpOpts dialog

class CPdsImpOpts : public CDialog
{
// Construction
public:
	CPdsImpOpts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPdsImpOpts)
	enum { IDD = IDD_PDSIMPORT_OPTIONS };
	CString	m_ItemsFound;
	BOOL	m_Iso;
	int		m_Comp;
	int		m_iEndPt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPdsImpOpts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPdsImpOpts)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/////////////////////////////////////////////////////////////////////////////
// CPdsImpOpts dialog


CPdsImpOpts::CPdsImpOpts(CWnd* pParent /*=NULL*/)
	: CDialog(CPdsImpOpts::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CPdsImpOpts)
	m_ItemsFound = _T("");
	m_Iso = FALSE;
	m_Comp = -1;
	m_iEndPt = -1;
	//}}AFX_DATA_INIT
  }


void CPdsImpOpts::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPdsImpOpts)
	DDX_Text(pDX, IDC_ITEMSFOUND, m_ItemsFound);
	DDX_Check(pDX, IDC_PDSIMP_ISO, m_Iso);
	DDX_Radio(pDX, IDC_PDSIMP_MINCOMP, m_Comp);
	DDX_Radio(pDX, IDC_PDSIMP_SRCSNK, m_iEndPt);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CPdsImpOpts, CDialog)
	//{{AFX_MSG_MAP(CPdsImpOpts)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPdsImpOpts message handlers

//===========================================================================

enum enumType 
  {
    CNFT_Null, 
    CNFT_3W, 
    CNFT_4W,
    CNFT_AV,
    CNFT_CR,
    CNFT_EL,
    CNFT_ER,
    CNFT_FL,
    CNFT_HA,
    CNFT_MT,
    CNFT_PI,
    CNFT_RB,
    CNFT_RD,
    CNFT_RI,
    CNFT_TE,
    CNFT_TF,
    CNFT_VA,
    CNFT_Missing
  };

struct CmpInfoStruct { char * Str; int Cat; int ConnNodes; int Cp; char* FBType; };
const CmpInfoStruct CmpInfo[]=
  {
    {"00", 0, 0, 0, ""},
    {"3W", 2, 4, 4, "Join"},
    {"4W", 2, 5, 5, "Join"},
    {"AV", 2, 3, 3, "Valve"},
    {"CR", 1, 5, 5, "InLine"},
    {"EL", 1, 3, 3, "InLine"},
    {"ER", 1, 2, 0, "ExpCon"},
    {"FL", 1, 2, 0, "InLine"},
    {"HA", 3, 2, 0, "InLine"},
    {"MT", 1, 3, 3, "InLine"},
    {"PI", 1, 2, 0, "Pipe"},
    {"RB", 1, 2, 0, "InLine"},
    {"RD", 1, 2, 0, "ExpCon"},
    {"RI", 1, 3, 3, "InLine"},
    {"TE", 1, 4, 4, "Join"},
    {"TF", 1, 2, 0, "InLine"},
    {"VA", 2, 3, 3, "Valve"},
    {"??", 0, 0, 0, ""},
  };

// To be tweaked according to Options Block
const long FirstConnNdNo  =    1;
const long LastConnNdNo   = 900;//1000;
const long FirstEndNdNo   = 901;//1001;
const long LastEndNdNo    = 950;//1500;
const long FirstIntNdNo   = 951;//1501;
const long LastIntNdNo    = 9999;

class CNF_Node
  {
  public:
    long          m_lNo;
    Pt_SLW        m_Pt;
    long          m_lflag;
    double        m_dIntens;
    long          m_nConns;
    long          m_nPasses;
    flag          m_ConnectRqd;
    flag          m_Connected;
    
    CNF_Node()
      {
      m_lNo=0;
      m_nConns=0;
      m_nPasses=0;
      m_ConnectRqd=false;
      m_Connected=false;
      };

    flag IsConnect()  { return m_lNo>=FirstConnNdNo  && m_lNo<=LastConnNdNo; };
    flag IsEndPt()    { return m_lNo>=FirstEndNdNo  && m_lNo<=LastEndNdNo; };
    flag IsInternal() { return m_lNo>=FirstIntNdNo  && m_lNo<=LastIntNdNo; };
  };

const int MaxConns=5;
class CNF_Comp
  {
  public:
    byte          m_bType;
    Strng         m_sName;
    Strng         m_sId;
    Strng         m_sCode;
    Strng         m_sData1;
    Strng         m_sData2;
    
    int           m_nConns;
    int           m_nMaxConns;
    long          m_lNdConn[MaxConns];
    byte          m_bUsed[MaxConns];
    double        m_dDiam[MaxConns];
    long          m_lNdOrigin;
    long          m_lNdCOfG;
    flag          m_bIsThinFlange;
    flag          m_bIs2Conn;
    flag          m_bIs2Hanger;
    flag          m_bInSeq;

    Strng         m_sTag;

    CNF_Comp()
      {
      m_bType=CNFT_Null;
      for (int i=0; i<MaxConns; i++)
        {
        m_lNdConn[i]=-1;
        m_bUsed[i]=false;
        m_dDiam[i]=0.0;
        }
      m_lNdOrigin=-1;
      m_lNdCOfG=-1;
      m_bIsThinFlange=false;
      m_bIs2Conn=false;
      m_bIs2Hanger=false;
      m_bInSeq=false;
      }
  };

class CNF_Seq
  {
  public:
    CNF_Node    * m_pN;
    CNF_Comp    * m_pC;
    CNF_Node    * m_pN0;
    CNF_Node    * m_pN1;
    long          m_iInx0;
    long          m_iInx1;
    long          m_lSeqNo;
    flag          m_bSeqStart;
    flag          m_bSeqEnd;
    flag          m_bReverse;
    Strng         m_Tag;

    double        m_L; // Length
    double        m_Z; // Change in Elevation

    CNF_Seq()
      {
      m_pC        = NULL;
      m_pN0       = NULL;
      m_pN1       = NULL;
      m_iInx0     = -1;
      m_iInx1     = -1;
      m_lSeqNo    = -1;
      m_bSeqStart = false;
      m_bSeqEnd   = false;
      m_bReverse  = false;
      m_L=0;
      m_Z=0;
      }
  };

typedef CSCDList <CNF_Node, CNF_Node&> CNF_NodeList;
typedef CSCDListIterWithPos<CNF_NodeList, CNF_Node*> CNF_NodeListIter;
typedef CSCDList <CNF_Comp, CNF_Comp&> CNF_CompList;
typedef CSCDListIterWithPos<CNF_CompList, CNF_Comp*> CNF_CompListIter;
typedef CSCDList <CNF_Seq, CNF_Seq&> CNF_SeqList;
typedef CSCDListIterWithPos<CNF_SeqList, CNF_Seq*> CNF_SeqListIter;

const byte CNF_Nd1   = 0x01;
const byte CNF_Nd2   = 0x02;

// Maps onto Dialog
const byte CNFC_MinUnits  =  0;
const byte CNFC_ValvesSep =  1;
const byte CNFC_MaxUnits  =  2;

const byte CNFE_SrcSnk =  0;
const byte CNFE_Tanks  =  1;

class CNF_Contents
  {
  public:
    CNF_NodeList    m_Nodes;
    CNF_CompList    m_Comps;
    CNF_SeqList     m_Pipes, m_Tees, m_EndPts;
    int             m_Counts[CNFT_Missing+1];

    flag            m_bIso;
    int             m_iComplexity;
    int             m_iEndPt;

    C3_BOX_S        m_Bnd;
    double          m_Extent;

    long            m_iLevel;
    
    CNF_Contents();

    int             Parse(char *Fn);
    int             Options();
    int             NextInSeq(CNF_Node * pN, flag SeqStart);
    int             NextInSeq(CNF_Comp* pC, long Inx);
    int             Sequence();
    int             FindNdTags(CNF_Node & N0, char * EIOTag, char * CIOTag,  Strng & Tag, Strng & IO);
    DXF_ENTITY      AddSymbol(GrfCmdBlk & GCB, pDXF_Drawing pDrw, 
                              int * NVerts, Pt_SLW * Verts, 
                              char * KernalName, 
                              char * Tag, Attr_Settings &Tag_Attr_Set);
    //int             BuildUnits(GrfCmdBlk & GCB, pDXF_Drawing pDrw, Attr_Settings &Tag_Attr_Set);
    int             Build(GrfCmdBlk & GCB, pDXF_Drawing pDrw, Attr_Settings &Tag_Attr_Set);
    int             SetValue(GrfCmdBlk & GCB, char * Tag, char *Var, long Value);
    int             SetValue(GrfCmdBlk & GCB, char * Tag, char *Var, double Value, CCnvIndex DC_What, char * Cnv);
    int             SetValue(GrfCmdBlk & GCB, char * Tag, char *Var, char * Value);
    int             SetValueG(GrfCmdBlk & GCB, char * Tag, char *Var, char * Value);
    
    int             Configure(GrfCmdBlk & GCB);

    CNF_Node      * GetNd(long No);
    CNF_Comp      * GetComp(long NdNo, long & NdInx);
  };

//---------------------------------------------------------------------------
    
inline CNF_Node * CNF_Contents::GetNd(long No)
  {
  if (No<0)
    return NULL;
  CNF_NodeListIter Ni(m_Nodes);
  for (CNF_Node * pN=Ni.First() ; pN; pN=Ni.Next())
    if (pN->m_lNo==No)
      return pN;
  return NULL;
  };

//---------------------------------------------------------------------------

inline CNF_Comp * CNF_Contents::GetComp(long NdNo, long & NdInx)
  {
  CNF_CompListIter Ci(m_Comps);
  for (CNF_Comp * pC=Ci.First() ; pC; pC=Ci.Next())
    for (int i=0; i<3; i++)
      if (!pC->m_bUsed[i] && pC->m_lNdConn[i]==NdNo && !pC->m_bIsThinFlange)
        {
        NdInx=i;
        return pC;
        }

  return NULL;
  };

//---------------------------------------------------------------------------

CNF_Contents::CNF_Contents()
  {
  for (int i=0; i<=CNFT_Missing; i++)
    m_Counts[i]=0;

  }

//---------------------------------------------------------------------------

int CNF_Contents::Parse(char *Fn)
  {
  flag Ret=0;
  FILE *f=fopen(Fn, "rt");
  if (f)
    {
    CSVColArray C;
    char Buff[4096];//, PrevComponent[256];
    char *Pos=&Buff[0];
    fgets(Buff, sizeof(Buff)-1, f);

    flag FirstNd=true;
    int LnNo=1;
    while (!feof(f))
      {
      int Quote;
      if (Buff[0]!='!')
        {
        int nFlds = ParseCSVTokens(Buff, C, Quote);

        if (C[0])
          {
          double DimScl=(*(C[0])=='M') ? 0.001 : 0.0254;
          if (_stricmp(C[0], "DRAW")==0)
            {
            }
          else if (_stricmp(C[0], "LOAD")==0 || _stricmp(C[0], "MLOAD")==0)
            {
            }
          else if (_stricmp(C[0], "LSET")==0)
            {
            }
          else if (_stricmp(C[0], "CODE")==0)
            {
            }
          else if (_stricmp(C[0], "PROP")==0 || _stricmp(C[0], "MPROP")==0)
            {
            CNF_Comp & Cmp=m_Comps.GetTail();
            // Check Values 0,1,2
            int i=SafeAtoL(C[3],-1);
            switch (i)
              {
              case 3: 
              case 4: 
              case 5: 
                Cmp.m_dDiam[i-3]=DimScl*SafeAtoF(C[4],1);
                break;
              default:;
              }
            }
          else if (_stricmp(C[0], "LNOD")==0 || _stricmp(C[0], "MLNOD")==0)
            {
            }
          else if (_stricmp(C[0], "NODE")==0 || _stricmp(C[0], "MNODE")==0)
            {
            CNF_Node N;
            N.m_lNo=SafeAtoL(C[1],-1);
            N.m_Pt.World.X=DimScl*SafeAtoF(C[2],0);
            N.m_Pt.World.Y=DimScl*SafeAtoF(C[3],0);
            N.m_Pt.World.Z=DimScl*SafeAtoF(C[4],0);
            N.m_lflag=SafeAtoL(C[5],0);
            N.m_dIntens=SafeAtoF(C[6],0);
            m_Nodes.AddTail(N);
            if (FirstNd)
              c3a_box_init_pt(&m_Bnd, &N.m_Pt.World[0]);
            else
              c3a_box_append_pt(&m_Bnd, &N.m_Pt.World[0]);
            FirstNd=false;
            }
          else  // Component
            {
            CNF_Comp Cmp;

            Cmp.m_sName=C[0];
            Cmp.m_sId=C[1];
            Cmp.m_sCode=C[2]?C[2]:"";
            Cmp.m_sData1=C[3]?C[3]:"";
            Cmp.m_sData2=C[4]?C[4]:"";

            for (int i=CNFT_Null+1; i<CNFT_Missing; i++)
              if (_stricmp(C[0], CmpInfo[i].Str)==0)
                break;
            const CmpInfoStruct & CIS=CmpInfo[i];
            Cmp.m_bType=i;
            m_Counts[i]++;
            if (Cmp.m_bType!=CNFT_Missing)
              {
              Cmp.m_nConns=0;
              Cmp.m_nMaxConns=(CIS.ConnNodes-(CIS.Cp>0 ? 1:0));
              for (int i=0; i<Cmp.m_nMaxConns; i++)
                {
                Cmp.m_lNdConn[i]=SafeAtoL(C[5+i],-1);
                if (Cmp.m_lNdConn[i]>=0)
                  Cmp.m_nConns++;
                }
              Cmp.m_lNdOrigin=(CIS.Cp>0 ? SafeAtoL(C[5+i],-1) : -1);
              Cmp.m_bIs2Hanger=(CIS.Cat==3);
              Cmp.m_bIs2Conn=((CIS.ConnNodes-(CIS.Cp>0 ? 1:0))==2);
              m_Comps.AddTail(Cmp);
              }
            else
              {
              int xxx=0;
              }
            }
          }
        }

      fgets(Buff, sizeof(Buff)-1, f);
      LnNo++;
      }

    fclose(f);
    }
  return Ret;
  }

//---------------------------------------------------------------------------

int CNF_Contents::Options()
  {
  CPdsImpOpts Dlg;
  Dlg.m_Comp=CNFC_MinUnits;
  Dlg.m_iEndPt=CNFE_SrcSnk;
  Dlg.m_Iso=false;

  Strng S;
  for (int i=0; i<=CNFT_Missing; i++)
    {
    if (m_Counts[i])
      {
      S.Set("%s : %i\n", CmpInfo[i].Str, m_Counts[i]);
      Dlg.m_ItemsFound+=S();
      }
    dbgpln("%s : %i", CmpInfo[i].Str, m_Counts[i]);
    }

  if (Dlg.DoModal()!=IDOK)
    return false;

  m_iComplexity=Dlg.m_Comp;
  m_iEndPt=Dlg.m_iEndPt;
  m_bIso=Dlg.m_Iso;
  return true;
  }

//---------------------------------------------------------------------------

inline double Length(Pt_3f & P, Pt_3f & Q)
  {
  return Sqrt(Sqr(P.X-Q.X)+Sqr(P.Y-Q.Y)+Sqr(P.Z-Q.Z));
  }

//---------------------------------------------------------------------------

//int CNF_Contents::NextInSeq_ZeroLen(CNF_Node * pN)//, long SeqNo)
//  {
//  }

//---------------------------------------------------------------------------

int CNF_Contents::NextInSeq(CNF_Node * pN, flag SeqStart)//, long SeqNo)
  {
  pN->m_nPasses++;
  long Inx;
  CNF_Comp * pC=GetComp(pN->m_lNo, Inx); 
#if WithTees
  if (pC)
#else
  if (pC && (pC->m_bType!=CNFT_TE || (pC->m_bType==CNFT_TE && !pN->IsEndPt())))
#endif
    {
    pC->m_bUsed[Inx]=true;
    if (pC->m_bIsThinFlange)
      {
      dbgp("%.*s%-5s%.*s", m_iLevel,". . . . . . . . . . . . . . ",pC->m_sName(), 20-m_iLevel,"                            ");
      dbgpln("%-20s  %2i[%5i] %2s %5s  -----------", pC->m_sId(), Inx, pC->m_lNdConn[Inx], "", "");
      }
    else if (pC->m_bIs2Hanger)
      {
      pC->m_bInSeq=true;
      dbgp("%.*s%-5s%.*s", m_iLevel,". . . . . . . . . . . . . . ",pC->m_sName(), 20-m_iLevel,"                            ");
      dbgp("%-20s  %2i[%5i] %2s %5s  ", pC->m_sId(), Inx, pC->m_lNdConn[Inx], "", "");
      dbgp("  D:%10.2f %10s   %10s   %10s", 1000*pC->m_dDiam[Inx],"", "", "");
      dbgpln("");
      NextInSeq(GetNd(pC->m_lNdConn[Inx]), SeqStart);//, SeqNo);
      }
#if WithTees
    else if (pC->m_nMaxConns>2)
      { // is a tee or multi way;
      pC->m_bInSeq=true;
      NextInSeq(pC, Inx);
      }
#endif
    else 
      { // is not a tee or multi way;
      long MxConns=pC->m_nMaxConns;
      for (int i=(Inx+1)%MxConns; i!=Inx; i=(i+1)%MxConns)
        {
#if WithTees
        if (pC->m_lNdConn[i]>=0)
#else
        if (pC->m_lNdConn[i]>=0 && (pC->m_bType!=CNFT_TE || (pC->m_bType==CNFT_TE && pC->m_lNdConn[i]<=LastConnNdNo)))
#endif
          {
          CNF_Node * pN1=GetNd(pC->m_lNdConn[i]);
          CNF_Seq Seq;
          Seq.m_pC=pC;
          Seq.m_pN0=pN;
          Seq.m_pN1=pN1;
          Seq.m_iInx0=Inx;
          Seq.m_iInx1=i;
          Seq.m_bReverse=Inx>i;
          Seq.m_lSeqNo=-1;
          Seq.m_bSeqStart=SeqStart;
          m_Pipes.AddTail(Seq);
          pC->m_bInSeq=true;
          SeqStart=false;

          if(1)
            {
            double L=Length(pN1->m_Pt.World, pN->m_Pt.World);
            double Z=pN1->m_Pt.World.Z-pN->m_Pt.World.Z;
            dbgp("%.*s%-5s%.*s", m_iLevel,". . . . . . . . . . . . . . ",pC->m_sName(), 20-m_iLevel,"                            ");
            dbgp("%-20s  %2i[%5i] %2i[%5i] ", pC->m_sId(), Inx, pC->m_lNdConn[Inx], i, pC->m_lNdConn[i]);
            dbgp("  D:%10.2f>%10.2f L:%10.2f Z:%10.2f", 1000*pC->m_dDiam[Inx], 1000*pC->m_dDiam[i], L*1000, Z*1000);
            dbgpln("");
            }

          if (!pC->m_bUsed[i]) 
            {
            pC->m_bUsed[i]=true;
            NextInSeq(GetNd(pC->m_lNdConn[i]), false);//, SeqNo);
            }
          }
        }
      }
    }
  else
    {
    int xxx=0;
    }
  return 0;
  }

//---------------------------------------------------------------------------

int CNF_Contents::NextInSeq(CNF_Comp* pC, long Inx)
  {
  dbgp("%.*s%-5s%.*s", m_iLevel,". . . . . . . . . . . . . . ",pC->m_sName(), 20-m_iLevel,"                            ");
  dbgpln("%-20s  %2i[%5i] %2s %5s  +++++++++++", pC->m_sId(), Inx, pC->m_lNdConn[Inx], "", "");
  m_iLevel++;
  
  CNF_Seq Seq;
  Seq.m_pC=pC;
  Seq.m_lSeqNo=-1;
  Seq.m_bSeqStart=true;
  m_Tees.AddTail(Seq);
  pC->m_bInSeq=true;

  long MxConns=pC->m_nMaxConns;
  for (int i=(Inx+1)%MxConns; i!=Inx; i=(i+1)%MxConns)
    {
#if WithTees
    if (pC->m_lNdConn[i]>=0)
#else
    if (pC->m_lNdConn[i]>=0 && pC->m_lNdConn[i]<=LastConnNdNo)
#endif
      {
      if (!pC->m_bUsed[i])
        {
        pC->m_bUsed[i]=true;
        NextInSeq(GetNd(pC->m_lNdConn[i]), true);
        }
      }
    }
  m_iLevel--;
  return 0;
  }

//---------------------------------------------------------------------------

int CNF_Contents::Sequence()
  {
  CNF_NodeListIter Ni(m_Nodes);
  CNF_CompListIter Ci(m_Comps);
  CNF_Comp * pC;
  CNF_Node * pN;

  for (pC=Ci.First() ; pC; pC=Ci.Next())
    {
    pC->m_bIsThinFlange=(pC->m_lNdConn[0]>=0 && pC->m_lNdConn[0]==pC->m_lNdConn[1] && pC->m_lNdConn[2]<0);
    if (!pC->m_bIsThinFlange)// && pC->m_bIs2Conn)
      for (int i=0; i<3; i++)
        {
        CNF_Node *pN=GetNd(pC->m_lNdConn[i]);
        if (pN)
          pN->m_nConns++;
        }
    }
  
  for (pN=Ni.First() ; pN; pN=Ni.Next())
    {
    if (pN->IsEndPt())
      {
      CNF_Seq Seq;
      Seq.m_pN=pN;
      Seq.m_lSeqNo=-1;
      Seq.m_bSeqStart=true;
      m_EndPts.AddTail(Seq);
//      pC->m_bInSeq=true;
      }
    }
  
  if (1)
    {
    for (pC=Ci.First() ; pC; pC=Ci.Next())
      {
      CNF_Node *pN0=GetNd(pC->m_lNdConn[0]);
      CNF_Node *pN1=GetNd(pC->m_lNdConn[1]);
      double L=(pN0 && pN1) ? Length(pN0->m_Pt.World, pN1->m_Pt.World) : 0.0;
      
      dbgp("%3s  %-20s    %3i: %5i%5i%5i", pC->m_sName(), pC->m_sId(), pC->m_bType, pC->m_lNdConn[0], pC->m_lNdConn[1], pC->m_lNdConn[2]);
      dbgp(" [%5i] [%5i]", pN0->m_lNo, pN1->m_lNo);
      dbgp("  D:%10.2f %10.2f %10.2f", 1000*pC->m_dDiam[0], 1000*pC->m_dDiam[1], 1000*pC->m_dDiam[2]);
      dbgp("  L:%10.2f", L*1000);
      dbgp(" %s", pC->m_bIs2Conn ? "2Conn":"     ");
      if (pC->m_bIsThinFlange)
        dbgp(" Thin==");
      dbgpln("");
      }
    for (pN=Ni.First() ; pN; pN=Ni.Next())
      dbgpln("%5i :  %12.7f %12.7f %12.7f  C:%3i", pN->m_lNo, pN->m_Pt.World.X, pN->m_Pt.World.Y, pN->m_Pt.World.Z, pN->m_nConns);
    }


  m_iLevel=0;
  for (pN=Ni.First() ; pN; pN=Ni.Next())
    {
    //if (m_nConns==1 && pN->m_nPasses==0)
    if (pN->IsEndPt() && pN->m_nPasses==0)
      {
      dbgpln("--------");
      NextInSeq(pN, true);
      }
    }
  dbgpln("--------");

  int SeqNo=1;
  CNF_SeqListIter Ti(m_Tees);
  for (CNF_Seq *pT=Ti.First(); pT; pT=Ti.Next())
    pT->m_lSeqNo=SeqNo++;

  SeqNo=1;
  CNF_SeqListIter Ei(m_EndPts);
  for (CNF_Seq *pE=Ei.First(); pE; pE=Ei.Next())
    pE->m_lSeqNo=SeqNo++;

  SeqNo=0;
  CNF_SeqListIter Pi(m_Pipes);
  CNF_Seq *pPPrev=NULL;
  for (CNF_Seq *pP=Pi.First(); pP; pP=Pi.Next())
    {
    if (pP->m_bSeqStart)
      {
      if (pPPrev)
        pPPrev->m_bSeqEnd=true;
      SeqNo++;
      }
    pPPrev=pP;
    pP->m_lSeqNo=SeqNo;
    }
  if (pPPrev)
    pPPrev->m_bSeqEnd=true;

  for (pC=Ci.First() ; pC; pC=Ci.Next())
    if (!pC->m_bInSeq)
      {
      CNF_Node *pN0=GetNd(pC->m_lNdConn[0]);
      CNF_Node *pN1=GetNd(pC->m_lNdConn[1]);
      double L=(pN0 && pN1) ? Length(pN0->m_Pt.World, pN1->m_Pt.World) : 0.0;
      if (L>1.0e-10)
        LogError("Import", LF_DoAfxMsgBox|LF_Exclamation, "Component %s:%s missed : Length=%g", pC->m_sName(), pC->m_sId(),L);
      else
        LogWarning("Import", 0, "Component %s:%s missed : Length=%g", pC->m_sName(), pC->m_sId(),L);
      }

  if (1)
    {
    dbgpln("-------- IN SEQUENCE");
    for (pE=Ei.First(); pE; pE=Ei.Next())
      {
      //dbgpln(" %3s  %-20s", pC->m_sName(), pC->m_sId());
      pN=pE->m_pN;
      dbgp("End:%4i ",pE->m_lSeqNo);
      dbgp(" [%5i]", pN->m_lNo);
      dbgpln("");
      }
    for (pT=Ti.First(); pT; pT=Ti.Next())
      {
      pC=pT->m_pC;
      dbgp("Tee:%4i ",pT->m_lSeqNo);
      dbgp(" %3s  %-20s", pC->m_sName(), pC->m_sId());
      for (int i=0; i<pC->m_nMaxConns; i++)
        if (pC->m_lNdConn[i]>=0)
          dbgp(" [%5i]", pC->m_lNdConn[i]);
      dbgpln("");
      }

    CNF_SeqListIter Pi(m_Pipes);
    CNF_Seq *pP=Pi.First();
    for ( ; pP; pP=Pi.Next())
      {
      if (pP->m_bSeqStart)
        dbgpln("Seq:%4i",pP->m_lSeqNo);
      pC=pP->m_pC;
      CNF_Node *pN0=pP->m_pN0;
      CNF_Node *pN1=pP->m_pN1;
      int i0=pP->m_iInx0;
      int i1=pP->m_iInx1;
      pP->m_L=Length(pN0->m_Pt.World, pN1->m_Pt.World);
      pP->m_Z=pN1->m_Pt.World.Z-pN0->m_Pt.World.Z;

      dbgp("         %3s  %-20s", pC->m_sName(), pC->m_sId());
      dbgp(" [%5i] [%5i]", pN0->m_lNo, pN1->m_lNo);
      dbgp("  D:%10.2f>%10.2f L:%10.2f Z:%10.2f", 1000*pC->m_dDiam[i0], 1000*pC->m_dDiam[i1], pP->m_L*1000, pP->m_Z*1000);
      dbgpln("");
      //dbgpln(" #:%i", pP->m_lSeqNo);
      }
    dbgpln("--------");

    dbgpln("-------- NOT IN SEQUENCE");
    for (pC=Ci.First() ; pC; pC=Ci.Next())
      if (!pC->m_bInSeq)
        {
        CNF_Node *pN0=GetNd(pC->m_lNdConn[0]);
        CNF_Node *pN1=GetNd(pC->m_lNdConn[1]);
        //
        ////double L=(pN0 && pN1) ? pN0->m_Pt.World.Dist(pN1->m_Pt.World) : 0.0;
        double L=(pN0 && pN1) ? Length(pN0->m_Pt.World, pN1->m_Pt.World) : 0.0;
      
        dbgp("%3s  %-20s    %3i: %5i%5i%5i", pC->m_sName(), pC->m_sId(), pC->m_bType, pC->m_lNdConn[0], pC->m_lNdConn[1], pC->m_lNdConn[2]);
        //dbgp(" [%5i] [%5i]", pN0?pN0->m_lNo:-1, pN1?pN1->m_lNo:-1);
        dbgp("  D:%10.2f %10.2f %10.2f", 1000*pC->m_dDiam[0], 1000*pC->m_dDiam[1], 1000*pC->m_dDiam[2]);
        dbgp("  L:%10.2f", L*1000);
        dbgp(" %s", pC->m_bIs2Conn ? "2Conn":"     ");
        dbgp(" %s", pC->m_bIsThinFlange ? "Thin":"    ");
        dbgpln("");
        }
//    for (pN=Ni.First() ; pN; pN=Ni.Next())
//      dbgpln("%5i :  %12.7f %12.7f %12.7f  C:%3i", pN->m_lNo, pN->m_Pt.World.X, pN->m_Pt.World.Y, pN->m_Pt.World.Z, pN->m_nConns);

    dbgpln("--------");
    }  
  return 0;
  };

//---------------------------------------------------------------------------
#define MaxVerts 100

//---------------------------------------------------------------------------

DXF_ENTITY CNF_Contents::AddSymbol(GrfCmdBlk & GCB, pDXF_Drawing pDrw, 
                                   int * NVerts, Pt_SLW * Verts, 
                                   char * KernalName, 
                                   char * Tag, Attr_Settings &Tag_Attr_Set)
  {
  DXF_ENTITY newinsert;
  Pt_3f sc(1.0, 1.0, 1.0);
  Pt_3f ptt(0., 0., 0.);
  Pt_3f pti=Verts[0].World;//N1.m_Pt.World;
  Pt_3f basept, vpt;
  basept.Zero();

  char nm[1024];
  //pchar KernalName = "LNKBLK_";
  int unum = pDrw->Blocks.FindUnique(KernalName, -1);
  sprintf(nm, "%s_%d", KernalName, Max(1, unum));

  Tag_Attr_Set.Flags=1/*HideTag*/ ? DXF_ATTRIB_INVIS : 0; 

  pBlock b = pDrw->Add_Block(nm, basept.p());
  DXF_ENTITY a = b->Add_Attrib_Defn("Tag", "Tag ", "*", basept.p(), Tag_Attr_Set);
  vpt.Zero();

  // now add the line to the block
  int iv=0;
  for (int j=0; NVerts[j]>0; j++)
    {
    C3_CURVE Link = NULL;
    for (int i=0; i<NVerts[j]; i++)
      {
      Pt_3f & ptmp = Verts[iv].World;
      ptmp.X -= pti.X;
      ptmp.Y -= pti.Y;
      ptmp.Z -= pti.Z;
      if (Link)
        b->Add_PLine_Vertex(Link, ptmp.p());
      else
        Link = b->Add_PLine_Start(ptmp.p());
      iv++;
      }
    }
  newinsert = pDrw->Create_Insert(nm, pti, GR_WHITE, sc, 0.0, Tag, ptt, Tag_Attr_Set);

  return newinsert;
  }

//---------------------------------------------------------------------------

int CNF_Contents::FindNdTags(CNF_Node & N0, char * EIOTag, char * CIOTag,  Strng & Tag, Strng & IO)
  {
  if (N0.IsEndPt())
    {
    CNF_SeqListIter Ei(m_EndPts);
    for (CNF_Seq * pE=Ei.First(); pE; pE=Ei.Next())
      if (pE->m_pN->m_lNo==N0.m_lNo)
        break;
    //Tag.Set("E_%i", pE->m_lSeqNo);
    Tag=pE->m_Tag();
    IO=EIOTag;
    }
  else if (N0.IsConnect())
    {
    CNF_Seq * pSrc=NULL;
    CNF_SeqListIter Ti(m_Tees);
    for (CNF_Seq * pT=Ti.First(); pT && (pSrc==NULL); pT=Ti.Next())
      {
      CNF_Comp * pC=pT->m_pC;
      for (int i=0; i<pC->m_nMaxConns; i++)
        if (pC->m_lNdConn[i]==N0.m_lNo)
          pSrc=pT;
      }
    if (pSrc)
      {
      //Tag.Set("T_%i", pSrc->m_lSeqNo);
      Tag=pSrc->m_Tag();
      IO=CIOTag;
      }
    }
  else
    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Bad Start Point");
  return 0;
  }

//---------------------------------------------------------------------------

static struct tMoves { int x,y,z,flag;} Moves[] =
  {
    { 1, 1, 1, 1},
    {-1, 1, 1, 0},
    {-1,-1, 1, 0},
    { 1,-1, 1, 0},
    { 1, 1, 1, 0},
    { 1, 1,-1, 1},
    {-1, 1,-1, 0},
    {-1,-1,-1, 0},
    { 1,-1,-1, 0},
    { 1, 1,-1, 0},
    { 1, 1, 1, 1},
    { 1, 1,-1, 0},
    {-1, 1, 1, 1},
    {-1, 1,-1, 0},
    { 1,-1, 1, 1},
    { 1,-1,-1, 0},
    {-1,-1, 1, 1},
    {-1,-1,-1, 0},
    // The End
    { 0, 0, 0,-1},
  };

int CNF_Contents::Build(GrfCmdBlk & GCB, pDXF_Drawing pDrw, Attr_Settings &Tag_Attr_Set)
  {
  Strng T;
  int iTag;
  int MdlInsertErr = 0;
  DXF_ENTITY newinsert = NULL;
  m_Extent = Max(Max(C3_MAX_X(&m_Bnd)-C3_MIN_X(&m_Bnd), C3_MAX_Y(&m_Bnd)-C3_MIN_Y(&m_Bnd)), C3_MAX_Z(&m_Bnd)-C3_MIN_Z(&m_Bnd));

  iTag=0;
  CNF_SeqListIter Ti(m_Tees);
  for (CNF_Seq *pT=Ti.First(); pT; pT=Ti.Next())
    {
    CNF_Comp * pC=pT->m_pC;
    CNF_Node &N0=*GetNd(pC->m_lNdConn[0]);
    CNF_Node &N1=*GetNd(pC->m_lNdConn[1]);
    CNF_Node &N2=*GetNd(pC->m_lNdConn[2]);

    N0.m_ConnectRqd=true;
    N1.m_ConnectRqd=true;
    N2.m_ConnectRqd=true;

    int NVerts[]={4,0};
    Pt_SLW Verts[MaxVerts];
    Verts[0]=N0.m_Pt;
    Verts[1]=N1.m_Pt;
    Verts[2]=N2.m_Pt;
    Verts[3]=N0.m_Pt;

    do
      T.Set("T_%i", ++iTag);
    while (!FE()->FE_TestModelTagUnique(T()));
    pT->m_Tag=T();

    newinsert=AddSymbol(GCB, pDrw, NVerts, Verts, "TEEBLK_", T(), Tag_Attr_Set);
    MdlInsertErr = GCB.AddUnitModel("Tie-1", T());
    }

  iTag=0;
  double ESz=0.002*m_Extent;
  CNF_SeqListIter Ei(m_EndPts);
  for (CNF_Seq *pE=Ei.First(); pE; pE=Ei.Next())
    {
    CNF_Node &N=*pE->m_pN;
    N.m_ConnectRqd=true;
    int iv=-1;
    int NVerts[10];
    Pt_SLW Verts[MaxVerts];
    for (int i=0; Moves[i].flag>=0; i++)
      {
      if (Moves[i].flag==1)
        NVerts[++iv]=0;
      NVerts[iv]++;
      Verts[i]=N.m_Pt;
      Verts[i].World.X+=Moves[i].x*ESz;
      Verts[i].World.Y+=Moves[i].y*ESz;
      Verts[i].World.Z+=Moves[i].z*ESz;
      }
    NVerts[iv++]=0;

    do
      T.Set("E_%i", ++iTag);
    while (!FE()->FE_TestModelTagUnique(T()));
    pE->m_Tag=T();
    
    newinsert=AddSymbol(GCB, pDrw, NVerts, Verts, "ENDBLK_", T(), Tag_Attr_Set);
    MdlInsertErr = GCB.AddUnitModel("Feed_XPG-1", T());
    }

  iTag=0;
  int NVerts[2]={0,0};
  Pt_SLW Verts[MaxVerts];
  CNF_SeqListIter Pi(m_Pipes);
  Strng SrcTag, SrcIO, DstTag, DstIO;
  for (CNF_Seq *pP=Pi.First(); pP; pP=Pi.Next())
    {
    CNF_Comp * pC=pP->m_pC;
    CNF_Node &N0=*pP->m_pN0;
    CNF_Node &N1=*pP->m_pN1;
    
    if (pP->m_bSeqStart)
      {
      do
        T.Set("L_%i", ++iTag);
      while (!FE()->FE_TestModelTagUnique(T()));
      
      NVerts[0]=0;
      Verts[NVerts[0]++]=N0.m_Pt;
      FindNdTags(N0, "Src", "IO", SrcTag, SrcIO);
        // Bad
      N0.m_Connected=true;
      }

    pP->m_Tag=T();
    
    Verts[NVerts[0]++]=N1.m_Pt;
    if (pP->m_bSeqEnd && NVerts[0])
      {
      FindNdTags(N1, "Src", "IO", DstTag, DstIO);
      N1.m_Connected=true;
      
      newinsert=AddSymbol(GCB, pDrw, NVerts, Verts, "LNKBLK_", T(), Tag_Attr_Set);
      if (newinsert)
        {
        CMdlValueSet::Clear();
        int err = gs_pPrj->AddNodeConnect("Pipe-1", T(), SrcTag(), SrcIO(), DstTag(), DstIO());
        if (err)
          LogError("Import", LF_DoAfxMsgBox|LF_Exclamation, "Connection not Made:\n%s\n%s.%s->%s.%s", T(), SrcTag(), SrcIO(), DstTag(), DstIO());
        //else
        //  gs_LicenseCnt.xCalcUnits();
        }
      SrcTag="";
      DstTag="";
      }
    }
  
  
#define DoExtraConns 0
#if DoExtraConns
  CNF_NodeListIter Ni(m_Nodes);
  for (CNF_Node * pN=Ni.First() ; pN; pN=Ni.Next())
    if (pN->m_ConnectRqd && !pN->m_Connected)
      {
      int NdNo=pN->m_lNo;
      dbgpln("Extra Connect %i", NdNo);

      int iTag=0;

      CNF_SeqListIter Ei(m_EndPts);
      for (CNF_Seq * pE=Ei.First(); pE; pE=Ei.Next())
        if (pE->m_pN->m_lNo==NdNo)
          {
          switch (iTag++)
            {
            case 0:
              SrcTag=pE->m_Tag();
              SrcIO="Src";//EIOTag;
              break;
            case 1:
              DstTag=pE->m_Tag();
              DstIO="Src";//EIOTag;
              break;
            }
          }

      CNF_SeqListIter Ti(m_Tees);
      for (CNF_Seq * pT=Ti.First(); pT; pT=Ti.Next())
        {
        CNF_Comp * pC=pT->m_pC;
        for (int i=0; i<pC->m_nMaxConns; i++)
          if (pC->m_lNdConn[i]==NdNo)
            {
            switch (iTag++)
              {
              case 0:
                SrcTag=pT->m_Tag();
                SrcIO="IO";//EIOTag;
                break;
              case 1:
                DstTag=pT->m_Tag();
                DstIO="IO";//EIOTag;
                break;
              }
            }
        }
      
      int cerr=-1;
      if (iTag==2)
        {
        dbgpln("Connect %s.%s >> %s.%s", SrcTag(), SrcIO(), DstTag(), DstIO());
        cerr = GCB.pMdl->AddConnect(SrcTag(), SrcIO(), DstTag(), DstIO());
        }
      if (cerr)
        LogError("Import", LF_DoAfxMsgBox|LF_Exclamation, "Connection not Made for Node %i", NdNo);
      }
#endif
  
  return 0;
  }

//---------------------------------------------------------------------------

int CNF_Contents::SetValue(GrfCmdBlk & GCB, char * Tag, char *Var, double Value, CCnvIndex DC_What, char * Cnv)
  {
  Strng WrkTag;
  WrkTag.Set("%s.%s", Tag, Var);
  flag UseCnv = (DC_What)>0 && (strlen(Cnv)>0);
  CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0));
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (GCB.XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem = ObjData.FirstItem();
    PkDataUnion DU;
    if (UseCnv)
      DU.SetTypeDouble(pItem->Type(), Value, pItem->CnvIndex(), Cnv);
    else
      DU.SetTypeDouble(pItem->Type(), Value);
    CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
    if (GCB.XWriteTaggedItem(ObjData, Route)!=TOData_OK)
      LogError("Import", 0, "Write failed for %s", WrkTag());
    }
  else
    {
    LogError("Import", 0, "Valid tag expected (%s)", WrkTag());
    return 1;
    }
  return 0; 
  }
  
//---------------------------------------------------------------------------

int CNF_Contents::SetValue(GrfCmdBlk & GCB, char * Tag, char *Var, long Value)
  {
  Strng WrkTag;
  WrkTag.Set("%s.%s", Tag, Var);
  CXM_ObjectTag ObjTag(WrkTag(), 0);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (GCB.XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem = ObjData.FirstItem();
    PkDataUnion DU;
    DU.SetTypeLong(pItem->Type(), Value);
    CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
    if (GCB.XWriteTaggedItem(ObjData, Route)!=TOData_OK)
      LogError("Import", 0, "Write failed for %s", WrkTag());
    }
  else
    {
    LogError("Import", 0, "Valid tag expected (%s)", WrkTag());
    return 1;
    }
  return 0; 
  }
  
//---------------------------------------------------------------------------

int CNF_Contents::SetValue(GrfCmdBlk & GCB, char * Tag, char *Var, char * Value)
  {
  Strng WrkTag;
  WrkTag.Set("%s.%s", Tag, Var);
  CXM_ObjectTag ObjTag(WrkTag(), 0);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (GCB.XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem = ObjData.FirstItem();
    PkDataUnion DU;
    DU.SetTypeString(pItem->Type(), Value);
    CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
    if (GCB.XWriteTaggedItem(ObjData, Route)!=TOData_OK)
      LogError("Import", 0, "Write failed for %s", WrkTag());
    }
  else
    {
    LogError("Import", 0, "Valid tag expected (%s)", WrkTag());
    return 1;
    }
  return 0; 
  }
//---------------------------------------------------------------------------

int CNF_Contents::SetValueG(GrfCmdBlk & GCB, char * Tag, char *Var, char * Value)
  {
  Strng WrkTag;
  WrkTag.Set("%s.%s", Tag, Var);
  CXM_ObjectTag ObjTag(WrkTag(), 0);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (GCB.XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem = ObjData.FirstItem();
    PkDataUnion DU;
    DU.SetTypeString(tt_Generic, Value);
    CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
    if (GCB.XWriteTaggedItem(ObjData, Route)!=TOData_OK)
      LogError("Import", 0, "Write failed for %s", WrkTag());
    }
  else
    {
    LogError("Import", 0, "Valid tag expected (%s)", WrkTag());
    return 1;
    }
  return 0; 
  }

//---------------------------------------------------------------------------

int CNF_Contents::Configure(GrfCmdBlk & GCB)
  {
  //Strng Tag;
  CNF_SeqListIter Ti(m_Tees);
  for (CNF_Seq *pT=Ti.First(); pT; pT=Ti.Next())
    {
    CNF_Comp * pC=pT->m_pC;
    double Z=GetNd(pC->m_lNdOrigin)->m_Pt.World.Z;
    //Tag.Set("T_%i", pT->m_lSeqNo);
    SetValue(GCB, pT->m_Tag(), "Z_Rqd", Z, DC_L, "m");
    }

  CNF_SeqListIter Ei(m_EndPts);
  for (CNF_Seq *pE=Ei.First(); pE; pE=Ei.Next())
    {
    CNF_Node &N=*pE->m_pN;
    double Z=N.m_Pt.World.Z;
    //Tag.Set("E_%i", pE->m_lSeqNo);
    SetValue(GCB, pE->m_Tag(), "Z_Rqd", Z, DC_L, "m");
    SetValueG(GCB, pE->m_Tag(), "Mode", "Source_ConstP");
    }

  CNF_SeqListIter Pi(m_Pipes);
  //Strng SrcTag, SrcIO, DstTag, DstIO;
  CArray <CNF_Seq*, CNF_Seq*> pSeq;
  int nSegs=0;
  for (CNF_Seq *pP=Pi.First(); pP; pP=Pi.Next())
    {
    //CNF_Comp * pC=pP->m_pC;
    //CNF_Node &N0=*pP->m_pN0;
    //CNF_Node &N1=*pP->m_pN1;
    //Tag.Set("L_%i", pP->m_lSeqNo);
    if (pP->m_bSeqStart)
      {
      nSegs=0;
      }
    pSeq.SetAtGrow(nSegs++, pP);
    if (pP->m_bSeqEnd)
      {
      //nSegs++;
      if (nSegs<2)
        LogError("Import", LF_DoAfxMsgBox|LF_Exclamation, "Too few Segments in %s", pP->m_Tag());

      SetValue(GCB, pP->m_Tag(), "MultiFlwBlk", 1L);
      SetValue(GCB, pP->m_Tag(), "TwoPhaseFlw", 1L, DC_, "");
      SetValue(GCB, pP->m_Tag(), "VLEquilibrium", 1L, DC_, "");
      long Cnt1=Max(1, (nSegs/2));
      long Cnt2=Max(1, (nSegs-nSegs/2));
      long Cnt=(1<<0 | Cnt1<<8 | Cnt2<<16 | 1<<24);
      SetValue(GCB, pP->m_Tag(), "MFB.FBCntMsk", Cnt);

      Strng T;
      int iSeq=0;
      CNF_Seq *p;
      p=pSeq[iSeq];
      double D=p->m_pC->m_dDiam[p->m_bReverse ? 1:0];
      T.Set("%s.MFB.FB.[Entry-%i]", pP->m_Tag(), 0);
      //SetValue(GCB, T(), "FB.EqpIdStr", p->m_pC->m_sId());
      SetValue(GCB, T(), "RqdLen", 0, DC_L, "m");
      SetValue(GCB, T(), "DZ", 0, DC_L, "m");
      SetValue(GCB, T(), "Diam", D, DC_L, "m");
      SetValue(GCB, T(), "FB.K", 1.0, DC_, "");
      for (int i=0; i<Cnt1; i++)
        {
        p=pSeq[iSeq];
        D=p->m_pC->m_dDiam[p->m_bReverse ? 0:1];
        T.Set("%s.MFB.FB.[Src-%i]", pP->m_Tag(), i);
        SetValue(GCB, T(), "FB.EqpIdStr", p->m_pC->m_sId());
        SetValue(GCB, T(), "RqdLen",  p->m_L, DC_L, "m");
        SetValue(GCB, T(), "DZ",      p->m_Z, DC_L, "m");
        SetValue(GCB, T(), "Diam",    D, DC_L, "m");
        SetValue(GCB, T(), "FB.Name", CmpInfo[p->m_pC->m_bType].FBType);
        SetValue(GCB, T(), "FB.K", 1., DC_, "");
        iSeq++;
        }
      for (i=Cnt2-1; i>=0; i--)
        {
        p=pSeq[iSeq];
        D=p->m_pC->m_dDiam[p->m_bReverse ? 0:1];
        T.Set("%s.MFB.FB.[Dst-%i]", pP->m_Tag(), i);
        SetValue(GCB, T(), "FB.EqpIdStr", p->m_pC->m_sId());
        SetValue(GCB, T(), "RqdLen",   p->m_L, DC_L, "m");
        SetValue(GCB, T(), "DZ",      -p->m_Z, DC_L, "m");
        SetValue(GCB, T(), "Diam",    D, DC_L, "m");
        SetValue(GCB, T(), "FB.Name", CmpInfo[p->m_pC->m_bType].FBType);
        SetValue(GCB, T(), "FB.K", 2., DC_, "");
        iSeq++;
        }
      p=NULL;//pSeq[iSeq];
      T.Set("%s.MFB.FB.[Exit-%i]", pP->m_Tag(), 0);
      //SetValue(GCB, T(), "EqpIdStr", p->m_pC->m_sId());
      SetValue(GCB, T(), "RqdLen", 0, DC_L, "m");
      SetValue(GCB, T(), "DZ", 0, DC_L, "m");
      SetValue(GCB, T(), "Diam", D, DC_L, "m");
      SetValue(GCB, T(), "FB.K", 1.0, DC_, "");
 
/*
  (       : <        > '[Entry-0]' = 'FlwBlk'
  Strng   : <Pfvsc   > 'Desc' = ''
  Double  : <Pfvscp  > 'RqdLen' = '0' Cnv:'m()'[2]
  Double  : <Pfvscp  > 'ActLen' = '0' Cnv:'m()'[2]
  Double  : <Pfvsc   > 'Diam' = '0.02' Cnv:'mm()'[2]
  Double  : <Pfvsc   > 'DZ' ('') = '0' Cnv:'mm()'[2]
  Double  : <Pfvsc   > 'Qm' ('') = '0' Cnv:'kg/s()'[6]
  Double  : <Pfvsc   > 'QmEst' ('') = '0' Cnv:'kg/s()'[6]
  Double  : <Pfvsc   > 'VFrac' ('') = '0' Cnv:'%()'[1]
  Double  : <Pfvsc   > 'LFrac' ('') = '1' Cnv:'%()'[1]
  Double  : <Pfvsc   > 'dPq' ('') = '0' Cnv:'kPa()'[23]
  Double  : <Pfvsc   > 'dPa' ('') = '0' Cnv:'kPa()'[23]
  Double  : <Pfvsc   > 'dPz' ('') = '0' Cnv:'kPa()'[23]
  Double  : <Pfvsc   > 'dPzX' ('') = '0' Cnv:'kPa()'[23]
  Double  : <Pfvsc   > 'dPb' ('') = '0' Cnv:'kPa()'[23]
  Double  : <Pfvsc   > 'Vel' ('') = '0' Cnv:'m/s()'[26]
  Double  : <Pfvsc   > 'Fill' ('') = '0'
  Double  : <Pfvsc   > 'PFlng' = '101.31' Cnv:'kPa()'[4]
  Double  : <Pfvsc   > 'PNode' = '101.31' Cnv:'kPa()'[4]
  Double  : <Pfvsc   > 'DatumFlng' = '-0.938987' Cnv:'m()'[2]
  Double  : <Pfvsc   > 'DatumNode' = '-0.938987' Cnv:'m()'[2]
  Double  : <Pfvsc   > 'RhoL' = '1000' Cnv:'kg/m^3()'[10]
  Double  : <Pfvsc   > 'NRhoV' = '1.22481' Cnv:'kg/m^3()'[10]
  Double  : <Pfvsc   > 'Temp' = '293.16' Cnv:'dC()'[5]
  Double  : <Pfvsc   > 'Fill' = '0' Cnv:'%()'[1]
  Double  : <PfvscN  > 'SlipRatio' = '*'
  Double  : <PfvscN  > 'VoidFraction' = '*' Cnv:'%()'[1]
  Double  : <PfvscN  > 'OnePhPart' = '1' Cnv:'%()'[1]
  Double  : <PfvscN  > 'TempOut' = '293.16' Cnv:'C()'[5]
  {       : <        > 'FB' = '$blk'
  Strng   : <vH      > 'Group' = 'MFBJoin'
  Strng   : <Pfvsc   > 'Name' = 'Join' StrList[ None, Join]
  Double  : <Pfvsc   > 'K' ('') = '-6.27744e+066'
  {       : <        > 'Overide' = '$blk'
  Double  : <PfvscN  > 'Tau' ('TimeConst') = '*' Cnv:'s()'[11]
  Double  : <PfvscHN > 'Rho' ('Density') = '*' Cnv:'kg/m^3()'[10]
  Double  : <PfvscHN > 'Temp' ('Temperature') = '*' Cnv:'dC()'[5]
  }       : <        > 'Overide' = ''
  }       : <        > 'FB' = ''
  )       : <        > '[Entry-0]' = ''
  (       : <        > '[Src-0]' = 'FlwBlk'
*/
      }
    }

  return 0;
  }

//---------------------------------------------------------------------------
 
void GrfCmdBlk::ImportPDS(pchar Fn, flag DoMerge)
  {
  flag UseBusyDlg = !CDlgBusy::IsOpen();
  if (UseBusyDlg)
    CDlgBusy::Open("");
  char Buff[256];
  sprintf(Buff, "Importing neutral file %s", Fn ? Fn : "");
  CStatusBarMsg BarMsg(Buff);

  CNF_Contents NFC;
  NFC.Parse(Fn);
  if (NFC.Options())
    {
    INCOMPLETECODE()
    //gs_Exec.SetGlblRunModes(SM_Buffered|HM_Full, NSHM_All);

    NFC.Sequence();

    __time64_t t;

    pDoc->SetModifiedFlag(TRUE);

    zero_malloc_total();
    if (pDrw && !DoMerge)
      {
      t = _time64(NULL);
      if (UseBusyDlg)
        CDlgBusy::SetLine(1, "Deleting Drawing");
      pDsp->SetViewDrawing(NULL);
      delete pDrw;
      pDrw = NULL;
      dbgpln("Delete Drawing %ld secs", time(NULL) - t);

      // If drawing hase been deleted should be nothing left in the ma
      #if dbgdxfmem
      char nmbuf[1024];
      sprintf(nmbuf,"%sSYSCAD.DX1",RootDirectory()); 
      print_dxf_entry("DXF_FILE debug - Unfreed Mallocs after drawing is deleted",nmbuf);
      clear_dxf_entry();
      #endif
      }

    if (Fn)
      {
      t = time(NULL);
      if (UseBusyDlg)
        CDlgBusy::SetLine(1, "Importing neutral file :\n\n %s", Fn);
      if (pDrw == NULL)
        {
        pDrw = new DXF_Drawing(pDsp, "", True);
        pDsp->SetViewDrawing(pDrw);
        }

      int test_mode = 1;
      //int err1 = NFC.BuildUnits(*this, pDrw, Tag_Attr_Set);
      int err1 = NFC.Build(*this, pDrw, Tag_Attr_Set);
      int err2 = NFC.Configure(*this);
      if (err1==1 && err2==1)  // indicates a load error followed by a ok cleanup
        {
        delete pDrw;
        pDrw = new DXF_Drawing(pDsp, "", True);
        pDsp->SetViewDrawing(pDrw);

        //int test_mode = 1;
        //if (pDrw->Load(Fn, DoMerge ? Load_Merge : Load_All,test_mode))
        //  {
        //  AfxMessageBox("Cannot recover drawing - Aborting");
        //  if (UseBusyDlg)
        //    CDlgBusy::Close();
        //  return;
        //  }
        }
      else if (err1==-1 || err2==-1)
        {
        AfxMessageBox("Cannot recover drawing - Aborting");
        if (UseBusyDlg)
          CDlgBusy::Close();
        return;
        }

      pDsp->Open();
      pDsp->SetZoom(Zoom_All, 0.0, 0.0, 280.0, 198.0); // ????????????????????????????? BUG ??
      pDsp->Close();

      #if dbgdrw
      dbgpln("Load Drawing %ld secs", time(NULL) - t);
      dbgpln("!!!!!!!!!!!!!!!!!!dxf.dbg file created!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      pDrw->PrintEntities(pDsp->Vp1,"dxf.dbg");
      #endif
      }

    pDsp->SetViewDrawing(pDrw);

    if (Fn)
      pWnd->Invalidate(True);
    }

  if (UseBusyDlg)
    CDlgBusy::Close();
  }


