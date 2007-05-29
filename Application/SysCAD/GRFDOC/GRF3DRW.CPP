//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __GRF3DRW_CPP
#include "sc_defs.h"
#include "grf3drw.h"
#include "scd_text.h"
#include "traceent.h"
#include "dynmseg.h"
#include "sfe_clnt.h"
#include "project.h"
#include "scdtempl.h"
#ifndef __FE_CONST_H
#include "..\flwlib\fe_const.h"
#endif
//#include "optoff.h"

extern "C"
  {
#include "c2cdefs.h"
#include "c2ddefs.h"
#include "grldefs.h"
  extern int get_line_error();
  }


#define dbgCNM            0
#define dbgcolor          0
#define dbgdxf            0
#define dbgdxfget         0
#define dbgdxfadd         0
#define dbgdxfdraw        0
#define dbgdxfgetb        0
#define dbgclosestc       0
#define dbgBlk            0
#define dbgexplode        0
#define dbgLineExport     0
#define dbgAssocGrf       0
#define dbgTagManagement  0


const double MaxBigDim = 1.0e4;

//===========================================================================

CString MakeValidBlockName(LPTSTR BlockName)
  {
  CString S=BlockName;
  const int len = S.GetLength();
  for (int i=0; i<len; i++)
    {
    if (S[i]==':' || S[i]=='*')
      S.SetAt(i, '$');
    else if (S[i]==' ' || S[i]=='.' || S[i]==',' || S[i]=='\t')
      S.SetAt(i, '_');
    }
  return S;
  }

//Strng MakeValidSymbolName(LPTSTR SymbolName)
//  {
//  Strng S=SymbolName;
//  char * pDol=S.XStrChr('$');
//  char * pUnd=S.XStrChr('_');
//  if (pDol==NULL && pUnd!=NULL)
//    *pUnd=':';
//  const int len = S.Len();
//  for (int i=0; i<len; i++)
//    {
//    if (S[i]=='$')
//      S[i] = ':';
//    else if (S[i]=='_')
//      S[i] = ' ';
//    }
//  return S;
//  }

//===========================================================================

CGrfHelper::CGrfHelper()
  {
  GR_BACKGROUND = GR_BLACK;
  BackGroundRGB = grl_get_win_color(GR_BACKGROUND);
  DigPenRGB = 0x0000ff00;
  pDigPen = NULL;
  pBrush = NULL;
  //Note: MSStyle_SDash difficult to see on short lines or small units
  iShowMdlStatusInActive    = MSStyle_Solid;
  iShowMdlStatusError       = MSStyle_LDash;
  iShowMdlStatusWarning     = MSStyle_Solid;
  iShowMdlStatusTear        = MSStyle_Solid;
  iShowMdlStatusMacMdl      = MSStyle_Solid;
  iShowMdlStatusOff         = MSStyle_Solid;
  iShowMdlStatusOn          = MSStyle_Solid;
  iShowMdlStatusLFlwVap     = MSStyle_Solid;
  iShowMdlStatusLFlwSol     = MSStyle_Solid;
  iShowMdlStatusLFlwP       = MSStyle_Solid;
  iShowMdlStatusLFlwN       = MSStyle_Solid;
  iShowMdlStatusLNoFlw      = MSStyle_Solid;
  iShowMdlStatusClear       = MSStyle_Solid;
  iShowMdlStatusUnKnown     = MSStyle_Solid;
  iShowMdlStatusNettQmP     = MSStyle_Off;
  iShowMdlStatusNettQmN     = MSStyle_Off;
  iShowMdlStatusUFlw        = MSStyle_Solid;
  iShowMdlStatusUNoFlw      = MSStyle_Solid;
  iShowMdlStatusIsSrc       = MSStyle_Solid;
  iShowMdlStatusIsSnk       = MSStyle_Solid;

  iShowMdlStatusXferLnk     = MSStyle_Solid;
  iShowMdlStatusSimpleLnk   = MSStyle_Solid;
  iShowMdlStatusLinearLnk   = MSStyle_Solid;
  iShowMdlStatusFullLnk     = MSStyle_Solid;
  }

//---------------------------------------------------------------------------

CGrfHelper::~CGrfHelper()
  {
  if (pBrush)
    delete pBrush;
  if (pDigPen)
    delete pDigPen;
  }

//---------------------------------------------------------------------------

void CGrfHelper::MdlStatusColor(dword Status, int &Color, int &OvrColor, int &OvrStyle)
  {
  OvrColor=-1;
  OvrStyle=-1;
  if (Color==-1 && gs_pPrj->ShowStatus())
    {
    if (Status!=0xFFFFFFFF)
      {
      //if ((Status & FNS_Marked))
      //  Color = GR_HIGHLIGHT; //don't overlay colours for this status!
      //else
      if (iShowMdlStatusInActive && (Status & FNS_InActive))
        Color = GR_MDLSTATUS_INACTIVE; //don't overlay colours for this status!
      else
        {
        if      (                                                  (Status & FNS_Marked   )) { OvrColor = GR_HIGHLIGHT;                                           }
        else if (                      iShowMdlStatusError      && (Status & FNS_Error    )) { OvrColor = GR_MDLSTATUS_ERROR;   OvrStyle = iShowMdlStatusError;   }
        else if (                      iShowMdlStatusWarning    && (Status & FNS_Warning  )) { OvrColor = GR_MDLSTATUS_WARNING; OvrStyle = iShowMdlStatusWarning; }
        else if (                      iShowMdlStatusTear       && (Status & FNS_Tear     )) { OvrColor = GR_MDLSTATUS_TEAR;    OvrStyle = iShowMdlStatusTear;    }
        else if (                      iShowMdlStatusMacMdl     && (Status & FNS_MacMdl   )) { OvrColor = GR_MDLSTATUS_MACMDL;  OvrStyle = iShowMdlStatusMacMdl;  }
        else if (!gs_Exec.Stopped() && iShowMdlStatusOff        && (Status & FNS_Off      )) { OvrColor = GR_MDLSTATUS_OFF;     OvrStyle = iShowMdlStatusOff;     }
        else if (!gs_Exec.Stopped() && iShowMdlStatusOn         && (Status & FNS_On       )) { OvrColor = GR_MDLSTATUS_ON;      OvrStyle = iShowMdlStatusOn;      }

        else if (                      iShowMdlStatusIsSrc      && (Status & FNS_IsSrc    )) { OvrColor = GR_MDLSTATUS_ISSRC;   OvrStyle = iShowMdlStatusIsSrc;   }
        else if (                      iShowMdlStatusIsSnk      && (Status & FNS_IsSnk    )) { OvrColor = GR_MDLSTATUS_ISSNK;   OvrStyle = iShowMdlStatusIsSnk;   }
        else if (                      iShowMdlStatusNettQmP    && (Status & FNS_NettQmP  )) { OvrColor = GR_MDLSTATUS_NETTQMP; OvrStyle = iShowMdlStatusNettQmP; }
        else if (                      iShowMdlStatusNettQmN    && (Status & FNS_NettQmN  )) { OvrColor = GR_MDLSTATUS_NETTQMN; OvrStyle = iShowMdlStatusNettQmN; }
        else if (                      iShowMdlStatusXferLnk    && (Status & FNS_LnkXfer  )) { OvrColor = GR_MDLSTATUS_LXFER;   OvrStyle = iShowMdlStatusXferLnk; }
        else if (                      iShowMdlStatusSimpleLnk  && (Status & FNS_LnkSimple)) { OvrColor = GR_MDLSTATUS_LSIMPLE; OvrStyle = iShowMdlStatusSimpleLnk; }
        else if (                      iShowMdlStatusLinearLnk  && (Status & FNS_LnkLinear)) { OvrColor = GR_MDLSTATUS_LLINEAR; OvrStyle = iShowMdlStatusLinearLnk; }
        else if (                      iShowMdlStatusFullLnk    && (Status & FNS_LnkFull  )) { OvrColor = GR_MDLSTATUS_LFULL;   OvrStyle = iShowMdlStatusFullLnk; }
        else {};

        if      (                      iShowMdlStatusInActive   && (Status & FNS_Hold     )) { Color    = GR_MDLSTATUS_INACTIVE;                        }
      //else if (                      1                        && (Status & FNS_IsCLnk   )) { Color    = GR_MDLSTATUS_BLACK; OvrStyle = MSStyle_LDash; }
      //else if (                      1                        && (Status & FNS_IsELnk   )) { Color    = GR_MDLSTATUS_BLACK; OvrStyle = MSStyle_LDash; }
      //else if (                      1                        && (Status & FNS_IsElec   )) { Color    = GR_MDLSTATUS_ELEC;  OvrStyle = MSStyle_SDash; }
      //else if (                      1                        && (Status & FNS_IsALnk   )) { Color    = GR_MDLSTATUS_BLACK; OvrStyle = MSStyle_LDash; }
        else if (                      iShowMdlStatusUFlw       && (Status & FNS_UFlw     )) { Color    = GR_MDLSTATUS_UFLW;                            }
        else if (                      iShowMdlStatusUNoFlw     && (Status & FNS_UNoFlw   )) { Color    = GR_MDLSTATUS_UNOFLW;                          }
        else if (                      iShowMdlStatusLFlwVap    && (Status & FNS_LFlwVap  )) { Color    = GR_MDLSTATUS_LFLWVAP;                         }
        else if (                      iShowMdlStatusLFlwSol    && (Status & FNS_LFlwSol  )) { Color    = GR_MDLSTATUS_LFLWSOL;                         }
        else if (                      iShowMdlStatusLFlwP      && (Status & FNS_LFlwP    )) { Color    = GR_MDLSTATUS_LFLWP;                           }
        else if (                      iShowMdlStatusLFlwN      && (Status & FNS_LFlwN    )) { Color    = GR_MDLSTATUS_LFLWN;                           }
        else if (                      iShowMdlStatusLNoFlw     && (Status & FNS_LNoFlw   )) { Color    = GR_MDLSTATUS_LNOFLW;                          }

        else Color=GR_MDLSTATUS_CLEAR;
        }

      if (!gs_pPrj->ShowStatus() && OvrColor>=0)
        {
        Color=OvrColor;
        OvrColor=-1;
        }
      }
    else
      Color=GR_OTHERGRAPHICS;
    }
  };

//---------------------------------------------------------------------------

CGrfHelper GrfHelper;

//===========================================================================

int dxf_current_line(FILE *fp)
  {
  int cnt = 0;
  char b[4096];
  fpos_t pos,p;
  if( fgetpos(fp,&pos) )
    {
    return -1;
    }
  rewind(fp);
  while( fgets(b,4096,fp) && !fgetpos(fp,&p) && p < pos )
    {
    cnt++;
    }
  fsetpos(fp,&pos);
  return cnt;
  }


void dump_vp(VP_VIEWPORT vp);

//void ConstructInsertMatrix(C3_TRANSFORM Imat,pBlock b,DXF_ENTITY e);

//===========================================================================

void Pt_3f::Unit()
  {
  double l = c3v_norm(p());
  if( fabs(l) < 1.0e-100 )
    {
    TRACE(" Normalising zero length vector");
    return;
    }
  X /= l;
  Y /= l;
  Z /= l;
  }

//---------------------------------------------------------------------------

void Pt_3f::Scale(double Scale)
  {
  X *= Scale;
  Y *= Scale;
  Z *= Scale;
  }

//---------------------------------------------------------------------------

Pt_3f Pt_3f::operator+(Pt_3f & Pt)
  {
  Pt_3f A(*this);
  A.X = X + Pt.X;
  A.Y = Y + Pt.Y;
  A.Z = Z + Pt.Z;
  return A;
  }

//---------------------------------------------------------------------------

Pt_3f Pt_3f::operator-(Pt_3f & Pt)
  {
  Pt_3f A(*this);
  A.X = X - Pt.X;
  A.Y = Y - Pt.Y;
  A.Z = Z - Pt.Z;
  return A;
  }

//---------------------------------------------------------------------------

Pt_3f Pt_3f::operator*(Pt_3f & Pt)
  {
  Pt_3f A(*this);
  A.X = X * Pt.X;
  A.Y = Y * Pt.Y;
  A.Z = Z * Pt.Z;
  return A;
  }

//===========================================================================

void Pt_SLW::Zero()
  {
  Screen.X  = 0.0;
  Screen.Y  = 0.0;
  Logical.X = 0;
  Logical.Y = 0;
  Logical.Z = 0;
  World.X   = 0.0;
  World.Y   = 0.0;
  World.Z   = 0.0;
  }

//---------------------------------------------------------------------------

void Pt_SLW::Get(const Pt_SLW &Pt)
  {
  Screen= Pt.Screen;
  Logical=Pt.Logical;
  World=Pt.World;
  }

//---------------------------------------------------------------------------

void Pt_SLW::GetX(const Pt_SLW &Pt)
  {
  Screen.X  = Pt.Screen.X;
  Logical.X = Pt.Logical.X;
  World.X   = Pt.World.X;
  }

//---------------------------------------------------------------------------

void Pt_SLW::GetY(const Pt_SLW &Pt)
  {
  Screen.Y  = Pt.Screen.Y;
  Logical.Y = Pt.Logical.Y;
  World.Y   = Pt.World.Y;
  }

//---------------------------------------------------------------------------

void Pt_SLW::GetZ(const Pt_SLW &Pt)
  {
  Logical.Z = Pt.Logical.Z;
  World.Z   = Pt.World.Z;
  }

//---------------------------------------------------------------------------

Pt_SLW Pt_SLW::operator+=(Pt_SLW &Pt)
  {
  Screen.X  = Screen.X  + Pt.Screen.X;
  Screen.Y  = Screen.Y  + Pt.Screen.Y;
  Logical.X = Logical.X + Pt.Logical.X;
  Logical.Y = Logical.Y + Pt.Logical.Y;
  Logical.Z = Logical.Z + Pt.Logical.Z;
  World.X   = World.X   + Pt.World.X;
  World.Y   = World.Y   + Pt.World.Y;
  World.Z   = World.Z   + Pt.World.Z;
  return *this;
  }

//---------------------------------------------------------------------------

Pt_SLW Pt_SLW::operator-=(Pt_SLW &Pt)
  {
  Screen.X  = Screen.X  - Pt.Screen.X;
  Screen.Y  = Screen.Y  - Pt.Screen.Y;
  Logical.X = Logical.X - Pt.Logical.X;
  Logical.Y = Logical.Y - Pt.Logical.Y;
  Logical.Z = Logical.Z - Pt.Logical.Z;
  World.X   = World.X   - Pt.World.X;
  World.Y   = World.Y   - Pt.World.Y;
  World.Z   = World.Z   - Pt.World.Z;
  return *this;
  }

//============================================================================

void Line_2f::Set(REAL x1,REAL y1,REAL x2,REAL y2)
  {
  start.Set(x1,y1);
  end.Set(x2,y2);
  }

//---------------------------------------------------------------------------

void Line_2f::Set(Pt_2f start,Pt_2f end)
  {
  Set(start.GetX(),start.GetY(),end.GetX(),end.GetY());
  }

//---------------------------------------------------------------------------

int Line_2f::LineSegmentCross(Line_2f l1,Line_2f l2,REAL &parm1,REAL &parm2)
  {
  double a = l1.start.Y          -            l1.end.Y;
  double b =            l1.end.X - l1.start.X;
  double c = l1.start.X*l1.end.Y - l1.start.Y*l1.end.X;
  double d = l2.start.Y          -            l2.end.Y;
  double e =            l2.end.X - l2.start.X;
  double f = l2.start.X*l2.end.Y - l2.start.Y*l2.end.X;
  double w = (e*a) - (d*b);

  if( fabs(w) < 1.0e-30 )
    return(0);

  double x = ((f*b) - (e*c))/w;
  double y = ((c*d) - (a*f))/w;

  if( fabs(l1.start.X-l1.end.X) > fabs(l1.start.Y-l1.end.Y))
    parm1 = (x-l1.start.X)/(l1.end.X - l1.start.X);
  else
    parm1 = (y-l1.start.Y)/(l1.end.Y - l1.start.Y);

  if( parm1 < 0.0 || parm1 > 1.0 )
    return 0;

  if( fabs(l2.start.X-l2.end.X) > fabs(l2.start.Y-l2.end.Y))
    parm2 = (x-l2.start.X)/(l2.end.X - l2.start.X);
  else
    parm2 = (y-l2.start.Y)/(l2.end.Y - l2.start.Y);

  if( parm2 < 0.0 || parm2 > 1.0 )
    return 0;

  return 1;
  }

//---------------------------------------------------------------------------

REAL Line_2f::PerpParm(Pt_2f pt,REAL &distance)
  {
  REAL x2mx1 = end.GetX() - start.GetX();
  REAL x3mx1 = pt.GetX()  - start.GetX();
  REAL y2my1 = end.GetY() - start.GetY();
  REAL y3my1 = pt.GetY()  - start.GetY();
  REAL len   = (x2mx1*x2mx1) + (y2my1*y2my1);

  if (len < 1.0e-30)
    {
    distance = _hypot((double)(start.GetX()-pt.GetX()),(double)(start.GetY()-pt.GetY()));
    return 0.0;
    }
  REAL l   = ((x2mx1*x3mx1) + (y2my1*y3my1))/len;
  REAL xp  = start.GetX()   + l*x2mx1;
  REAL yp  = start.GetY()   + l*y2my1;
  REAL aa  = xp-pt.GetX();
  REAL bb  = yp-pt.GetY();
  distance = sqrt(aa*aa + bb*bb); // CNM ???
  return l;
  }

//============================================================================

const float  A0_PageWidth = (float)1120.0;

static far char *HdrVarNames[N_HDR_VARS] =
  {
  "$ACADVER", "$INSBASE", "$EXTMIN", "$EXTMAX", "$LIMMIN",
    "$LIMMAX", "$ORTHOMODE", "$REGENMODE", "$FILLMODE", "$QTEXTMODE",
    "$MIRRTEXT", "$DRAGMODE", "$LTSCALE", "$OSMODE", "$ATTMODE",
    "$TEXTSIZE", "$TRACEWID", "$TEXTSTYLE", "$CLAYER", "$CELTYPE",
    "$CECOLOR", "$DIMSCALE", "$DIMASZ", "$DIMEXO", "$DIMDLI",
    "$DIMRND", "$DIMDLE", "$DIMEXE", "$DIMTP", "$DIMTM",
    "$DIMTXT", "$DIMCEN", "$DIMTSZ", "$DIMTOL", "$DIMLIM",
    "$DIMTIH", "$DIMTOH", "$DIMSE1", "$DIMSE2", "$DIMTAD",
    // "$DIMZIN", "$DIMBLK", "$DIMASO", "$DIMSHO", "$DIMPOST",
    "$DIMZIN", "", "$DIMASO", "$DIMSHO", "$DIMPOST",
    "$DIMAPOST", "$DIMALT", "$DIMALTD", "$DIMALTF", "$DIMLFAC",
    "$DIMTOFL", "$DIMTVP", "$DIMTIX", "$DIMSOXD", "$DIMSAH",
    "$DIMBLK1", "$DIMBLK2", "$LUNITS", "$LUPREC", "$AXISMODE",
    "$AXISUNIT", "$SKETCHINC", "$FILLETRAD", "$AUNITS", "$AUPREC",
    "$MENU", "$ELEVATION", "$THICKNESS", "$LIMCHECK", "$BLIPMODE",
    "$CHAMFERA", "$CHAMFERB", "$SKPOLY", "$TDCREATE", "$TDUPDATE",
    "$TDINDWG", "$TDUSRTIMER", "$USRTIMER", "$ANGBASE", "$ANGDIR",
    "$PDMODE", "$PDSIZE", "$PLINEWID", "$COORDS", "$SPLFRAME",
    "$SPLINETYPE", "$SPLINESEGS", "$ATTDIA", "$ATTREQ", "$HANDLING",
    // "$HANDSEED", "$SURFTAB1", "$SURFTAB2", "$SURFTYPE", "$SURFU",
    "", "$SURFTAB1", "$SURFTAB2", "$SURFTYPE", "$SURFU",
    "$SURFV", "$FLATLAND", "$UCSNAME", "$UCSORG", "$UCSXDIR",
    "$UCSYDIR", "$USERI1", "$USERI2", "$USERI3", "$USERI4",
    "$USERI5", "$USERR1", "$USERR2", "$USERR3", "$USERR4",
    "$USERR5", "$WORLDVIEW"
  };

static far int HdrVarSize[N_HDR_VARS];

//============================================================================

static int HdrVarIndex(pchar Nm)
  {
  int i = N_HDR_VARS;
  while ((--i >= 0) && (strcmp(HdrVarNames[i], Nm) == 0));
  return i;
  }

//----------------------------------------------------------------------------

static void SetIntHdrVar(pchar HdrInfo[N_HDR_VARS], pchar Nm, int v)
  {
  int i = HdrVarIndex(Nm);
  if (i >= 0 && HdrVarSize[i] == sizeof(v))
    *((int *)HdrInfo[i]) = v;
  }

//============================================================================

#if dbgdxf|dbgdxfadd|dbgdxfget

static char *DxfEntIdStrs[17] =
  {
  "POINT   ",   // 1L
    "VERTEX  ",   // 2L
    "LINE    ",   // 4L
    "CIRCLE  ",   // 8L
    "ARC     ",   // 16L
    "TRACE   ",   // 32L
    "SOLID   ",   // 64L
    "POLYLINE",   // 128L
    "LINE3D  ",   // 256L
    "3DFACE  ",   // 512L
    "TEXT    ",   // 1024L
    "SHAPE   ",   // 2048L
    "INSERT  ",   // 4096L
    "ATTDEF  ",   // 8192L
    "ATTRIB  ",   // 16384L
    "DIM     ",   // 32768L
    "VIEWPORT"    // 65536L
  };

// ---------------------------------------------------------------------------

static char *DxfEntIdStr(long id)
  {
  int i=0;
  while (id >>= 1) i++;
  return DxfEntIdStrs[i];
  }

#endif

// ---------------------------------------------------------------------------

void MakeLayerName(pchar FullNm, LayerTypes LayerType, pchar CGrfLayer)
  {
  strcpy(FullNm, LayerNames[LayerType]);
  strcat(FullNm, CGrfLayer);
  };


//============================================================================

CGrfLayer::CGrfLayer(pDXF_Drawing DrawingIn, pBlock BlockIn)
  {
  Nxt = NULL;
  Def = NULL;
  m_Color = GR_WHITE;
  m_DiscColor = -1;                     // Off
  m_Thickness = 0;
  Drawing = DrawingIn;
  Block = BlockIn;
  CurveLst  = dml_create_list();
  InsertLst = dml_create_list();
  OtherLst  = dml_create_list();
  };

//----------------------------------------------------------------------------

CGrfLayer::~CGrfLayer()
  {
  Clear();
  dml_free_list(CurveLst);
  dml_free_list(InsertLst);
  dml_free_list(OtherLst);
  //dxf_free_table_entry(Def); kga
  };

//----------------------------------------------------------------------------

void CGrfLayer::Clear()
  {
  DML_ITEM it;
  DXF_ENTITY e;
  C3_CURVE c;

  WALK_CURVE_LIST(CurveLst, it, c)
    {
    CEntInView* p = ((CEntInView*)(it->pKenwalta));
    if( p )
      p->VP()->DelDisplaySegment(p);
    c3d_free_curve(c);
    }
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    CEntInView* p = ((CEntInView*)(it->pKenwalta));
    if( p )
      p->VP()->DelDisplaySegment(p);
    dxf_free_entity(e);
    }
  WALK_ENTITY_LIST(OtherLst, it, e)
    {
    CEntInView* p = ((CEntInView*)(it->pKenwalta));
    if( p )
      p->VP()->DelDisplaySegment(p);
    dxf_free_entity(e);
    }
  dml_clear_list(CurveLst);
  dml_clear_list(InsertLst);
  dml_clear_list(OtherLst);
  }

#if dbgTagManagement  
void DumpInsertList(LPCSTR Name, DML_LIST InsertLst)
  {
  if (1)
    {
    DML_ITEM it;
    DXF_ENTITY e;
    WALK_ENTITY_LIST(InsertLst, it, e)
      {
      DML_LIST l1 = DXF_INSERT_ATTRS(e);
      if (l1)
        {
        DML_ITEM it1;
        DXF_ENTITY e1;
        WALK_ENTITY_LIST(l1, it1, e1)
          {
          dbgpln("%s: 0x%08x %-30s : %s ", Name, e1,
            DXF_ATTRIB_TAG_GET(e1) ? DXF_ATTRIB_TAG_GET(e1) : "---",
            DXF_ATTRIB_VALUE_GET(e1) ? DXF_ATTRIB_VALUE_GET(e1) : "");
          }
        }
      }
    }
  }
#endif

//----------------------------------------------------------------------------

flag CGrfLayer::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  flag TagChngd = 0;

#if dbgTagManagement  
  DumpInsertList("AAA", InsertLst);
#endif

  DML_ITEM it;
  DXF_ENTITY e;
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    DML_LIST l1 = DXF_INSERT_ATTRS(e);
    if (l1)
      {
      DML_ITEM it1;
      DXF_ENTITY e1;
      WALK_ENTITY_LIST(l1, it1, e1)
        {
#if dbgTagManagement
        dbgpln("ChangeTags: 0x%08x %-30s : %s ", e1,
               DXF_ATTRIB_TAG_GET(e1) ? DXF_ATTRIB_TAG_GET(e1) : "---",
               DXF_ATTRIB_VALUE_GET(e1) ? DXF_ATTRIB_VALUE_GET(e1) : "");
#endif
        if ( DXF_ATTRIB_TAG_GET(e1) && !_stricmp(DXF_ATTRIB_TAG_GET(e1),TagAttribStr) &&
          DXF_ATTRIB_VALUE_GET(e1) && !_stricmp(DXF_ATTRIB_VALUE_GET(e1),pOldTag) )
          {
#if dbgTagManagement
          dbgpln(" >> %s -> %s", pOldTag, pNewTag);
#endif
          DXF_ATTRIB_VALUE_SET(e1, pNewTag);
          TagChngd = 1;
          }
        }
      }
    }

#if dbgTagManagement  
  DumpInsertList("BBB", InsertLst);
#endif

  return TagChngd;
  }

//----------------------------------------------------------------------------

void CGrfLayer::CollectTags(Strng_List & TagList)
  {
#if dbgTagManagement  
  DumpInsertList("EEE", InsertLst);
#endif

  DML_ITEM it;
  DXF_ENTITY e;
  //dbgpln("=========");
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    DML_LIST l1 = DXF_INSERT_ATTRS(e);
    if (l1)
      {
      DML_ITEM it1;
      DXF_ENTITY e1;
      WALK_ENTITY_LIST(l1, it1, e1)
        {
#if dbgTagManagement
        dbgpln("CollectTags: 0x%08x %-30s : %s ", e1,
               DXF_ATTRIB_TAG_GET(e1) ? DXF_ATTRIB_TAG_GET(e1) : "---",
               DXF_ATTRIB_VALUE_GET(e1) ? DXF_ATTRIB_VALUE_GET(e1) : "");
#endif
        if( DXF_ATTRIB_TAG_GET(e1) && !_stricmp(DXF_ATTRIB_TAG_GET(e1),TagAttribStr))// &&
          //DXF_ATTRIB_VALUE_GET(e1) && !_stricmp(DXF_ATTRIB_VALUE_GET(e1),pOldTag) )
          {
          //DXF_ATTRIB_VALUE_SET(e1,pNewTag);
          if (strlen(DXF_ATTRIB_VALUE_GET(e1))==0)
            {
            int xxx=0;
            }
          TagList.Append(DXF_ATTRIB_VALUE_GET(e1));
          }
        }
      }
    }
  }

//----------------------------------------------------------------------------

flag CGrfLayer::FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr)
  {
  DML_ITEM it;
  DXF_ENTITY e;
  //dbgpln("=========");
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    DML_LIST l1 = DXF_INSERT_ATTRS(e);
    if (l1)
      {
      DML_ITEM it1;
      DXF_ENTITY e1;
      WALK_ENTITY_LIST(l1, it1, e1)
        {
        //dbgpln("%-30s : %s ",
        //       DXF_ATTRIB_TAG_GET(e1) ? DXF_ATTRIB_TAG_GET(e1) : "---",
        //       DXF_ATTRIB_VALUE_GET(e1) ? DXF_ATTRIB_VALUE_GET(e1) : "");

        if( DXF_ATTRIB_TAG_GET(e1) && !_stricmp(DXF_ATTRIB_TAG_GET(e1),TagAttribStr))// &&
          //DXF_ATTRIB_VALUE_GET(e1) && !_stricmp(DXF_ATTRIB_VALUE_GET(e1),pOldTag) )
          {
          if (stricmp(DXF_ATTRIB_VALUE_GET(e1), Tag)==0)
            {
            if (pLastMatchingInsert==e)
              pLastMatchingInsert=NULL;
            else
              {
              pLastMatchingInsert=e;
              pLastMatchingAttr=e1;
              return true;          
              }
            }
          }
        }
      }
    }
  return false;
  }

//---------------------------------------------------------------------------
 
flag CGrfLayer::UpdateModelAssocGraphics(CMdlAssocGraphicList & Grfs, CMdlAssocGraphicMap & Map, CList<CMdlAssocGraphicX,CMdlAssocGraphicX&> & GrfsX)
  {
#if dbgTagManagement  
  DumpInsertList("CCC", InsertLst);
#endif

  DML_ITEM it;
  DXF_ENTITY eInsert;
  flag WorkReqd = 0;
  WALK_ENTITY_LIST(InsertLst, it, eInsert)
    {
    DML_LIST l1 = DXF_INSERT_ATTRS(eInsert);
    if (l1)
      {
      //DML_ITEM it1;
      //DXF_ENTITY e1;

      //      WALK_ENTITY_LIST(l1, it1, e1)
      //        {
      //        if ( DXF_ATTRIB_TAG_GET(e1) && !_stricmp(DXF_ATTRIB_TAG_GET(e1),AssocTagAttribStr))
      //          {
      //#if dbgAssocGrf
      //          dbgpln("AssocTag 0x%08x = %s", e1, DXF_ATTRIB_VALUE_GET(e1));
      //#endif
      //          eIsAssoc=e;
      //          break;
      //          }
      //        }

      DXF_ENTITY eTag=Find_Attr(eInsert, TagAttribStr);
      if (eTag)
        {
        LPTSTR Tag=DXF_ATTRIB_VALUE_GET(eTag);
        DXF_ENTITY eAssoc=Find_Attr(eInsert, AssocTagAttribStr);
#if dbgAssocGrf
        dbgpln("ModelAssocGraphics Tag 0x%08x      = %s", eTag, Tag); 
#endif                               
        CMdlAssocGraphicMapItem GI;
        if (Map.Lookup(Tag, GI))
          {
#if dbgAssocGrf
          dbgpln("                   Found @ = %s [%i]", GI.m_pG->m_sNdTag, GI.m_Index);
#endif
          if (eAssoc)
            {
            CMdlAssocGraphic::CItem & I=GI.m_pG->m_AssocGrfs[GI.m_Index];
            WorkReqd= 1;
            I.m_bExists=true;
            if (!I.m_bShowIt)
              {
#if dbgAssocGrf
              dbgpln("                   Delete @ %s", Tag);
#endif
              GrfsX.AddHead(CMdlAssocGraphicX(CMdlAssocGraphicX::eDelete, NULL, 0, 0, 0, eInsert));
              }
            }
          else //if (I.m_bShowIt)
            {
            //ASSERT_ALWAYS(stricmp(Tag, GI.m_pG->m_sNdTag)==0, "Tag Should match", __FILE__, __LINE__); 
            WorkReqd= 1;
            GrfsX.AddHead(CMdlAssocGraphicX(CMdlAssocGraphicX::eAddNdAssocGrfs, GI.m_pG, -1, DXF_INSERT_PT(eInsert)[0], DXF_INSERT_PT(eInsert)[1], eInsert));
            }
          }
        else //if (eIsAssoc)
          {
          // Item not found - remove it if assoc grfs
#if dbgAssocGrf
          dbgpln("                   Missing ");
#endif
          if (eAssoc)
            {
#if dbgAssocGrf
            dbgpln("                   Delete @ %s", Tag);
#endif
            GrfsX.AddHead(CMdlAssocGraphicX(CMdlAssocGraphicX::eDelete, NULL, 0, 0, 0, eInsert));
            }
          }
        }
      }
    }

#if dbgTagManagement  
  DumpInsertList("DDD", InsertLst);
#endif
  return WorkReqd;
  };   

//----------------------------------------------------------------------------

void PopulateTagInfo(DXF_Drawing * pDrawing, DXF_ENTITY insert, DXF_ENTITY tag, CGrfTagInfo & GTI, DML_LIST &style)
  {
  GTI.m_sSymbol = DXF_INSERT_BLOCK_GET(insert);
  GTI.m_Node.m_X = DXF_INSERT_PT(insert)[0];
  GTI.m_Node.m_Y = DXF_INSERT_PT(insert)[1];
  GTI.m_Node.m_Z = DXF_INSERT_PT(insert)[2];
  GTI.m_Node.m_XScale = DXF_INSERT_X_SCALE(insert);
  GTI.m_Node.m_YScale = DXF_INSERT_Y_SCALE(insert);
  GTI.m_Node.m_ZScale = DXF_INSERT_Z_SCALE(insert);
  GTI.m_Node.m_Rotation = DXF_INSERT_ROT_ANG(insert);
  GTI.m_Node.m_Visible  = true;
  GTI.e = insert;
  GTI.m_LoBnd.m_X = GTI.m_Node.m_X;
  GTI.m_LoBnd.m_Y = GTI.m_Node.m_Y;
  GTI.m_LoBnd.m_Z = GTI.m_Node.m_Z;
  GTI.m_HiBnd.m_X = GTI.m_Node.m_X;
  GTI.m_HiBnd.m_Y = GTI.m_Node.m_Y;
  GTI.m_HiBnd.m_Z = GTI.m_Node.m_Z;

  pBlock b;
  b=pDrawing->Blocks.Find(DXF_INSERT_BLOCK_GET(insert));
  if (b)
    {
    C3_TRANSFORM c3t;
    c3t[0][0] = c3t[0][1] = c3t[0][2] = c3t[0][3] = 0.0;
    c3t[1][0] = c3t[1][1] = c3t[1][2] = c3t[1][3] = 0.0;
    c3t[2][0] = c3t[2][1] = c3t[2][2] = c3t[2][3] = 0.0;
    c3t[0][0] = c3t[1][1] = c3t[2][2] = 1.0;

    C3_BOX_S Bounds;
    C3_TRANSFORM c3t_mem;
    
    PT3 p;
    c3v_set(1.1e30, 1.1e30, 1.1e30, p);
    c3a_box_init_pt(&Bounds, p);


    ASSIGN_ARRAY(c3t_mem, c3t, c3t_mem);

    C3_TRANSFORM Imat;
    pDrawing->ConstructInsertMatrix(Imat,b,insert);
    c3t_mult(Imat,c3t_mem,c3t);
    b->GetBounds(c3t, &Bounds);
    ASSIGN_ARRAY(c3t, c3t_mem, c3t_mem);

    GTI.m_LoBnd.m_X = C3_MIN_X(&Bounds);
    GTI.m_LoBnd.m_Y = C3_MIN_Y(&Bounds);
    GTI.m_LoBnd.m_Z = C3_MIN_Z(&Bounds);
    GTI.m_HiBnd.m_X = C3_MAX_X(&Bounds);
    GTI.m_HiBnd.m_Y = C3_MAX_Y(&Bounds);
    GTI.m_HiBnd.m_Z = C3_MAX_Z(&Bounds);
    }

  if (tag)
    {
    text_info ti;
    double /*dx,dy,*/xmin,xmax,ymin,ymax;

    text_info_from_dxf_attrib(&ti, style, tag);
    scd_acad_text_corners(&ti, &xmin, &ymin, &xmax, &ymax);

    GTI.m_Tag.m_X = DXF_ATTRIB_PT(tag)[0]+0.5*(xmax-xmin);
    GTI.m_Tag.m_Y = DXF_ATTRIB_PT(tag)[1];//+0.5*(ymax-ymin);
    GTI.m_Tag.m_Z = DXF_ATTRIB_PT(tag)[2];
    GTI.m_Tag.m_XScale = ti.height/3.0; // CNM a Kludge  XF_ATTRIB_X_SCALE(tag);
    GTI.m_Tag.m_YScale = GTI.m_Tag.m_XScale;//DXF_ATTRIB_Y_SCALE(tag);
    GTI.m_Tag.m_ZScale = GTI.m_Tag.m_XScale;//DXF_ATTRIB_Z_SCALE(tag);
    GTI.m_Tag.m_Rotation = DXF_ATTRIB_ROT_ANG(tag);
    GTI.m_Tag.m_Visible = (DXF_ATTRIB_AFLAGS(tag)&DXF_ATTRIB_INVIS)==0;
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::CollectTagInfo(DXF_Drawing * pDrawing, bool AllInserts, CGrfTagInfoArray & GTIA)
  {
  DML_ITEM it;
  DXF_ENTITY e;
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    char* pv = Find_Attr_Value(e, TagAttribStr);
    if (pv || AllInserts)
      {
      CGrfTagInfo GTI;
      GTI.m_sTag = pv ? pv:"NULL";
      GTI.m_bHasTag = pv!=NULL;
      PopulateTagInfo(pDrawing, e, Find_Attr(e, TagAttribStr), GTI, Drawing->StyleLst);
      GTIA.Add(GTI);
      }
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::Add(C3_CURVE c)
  {
#if dbgdxfadd
  dbgp("      \nLayer::Add a Curve to CGrfLayer:%s",DXF_LAYER_NAME_GET(Def));
#endif

  int discard = 0;
  C3_BOX_S box;
  double BiggestDim;
  flag test = GetCurveBox(&box,c,&BiggestDim);

  dbgpln("GetCurveTest %12g %12g %12g %12g %12g %12g %12g",
    C3_MIN_X(&box),C3_MIN_Y(&box),C3_MIN_Z(&box),
    C3_MAX_X(&box),C3_MAX_Y(&box),C3_MAX_Z(&box),BiggestDim);

  if( test && BiggestDim > MaxBigDim)
    {
    char b[1024];
    (void)sprintf(b," A DXF entity contains large(%g) numeric values\n.May it be discarded ?",BiggestDim);
    if (AfxMessageBox(b, MB_YESNO|MB_ICONQUESTION)==IDYES)
      {
      discard = 1;
      }
    }

  if( !discard )
    DML_ITEM it = dml_append_data(CurveLst, c);
  }

//----------------------------------------------------------------------------

void CGrfLayer::Add(DXF_ENTITY e)
  {
#if dbgdxfadd
  dbgp("      \nLayer::Add a Entity to CGrfLayer:%s", DXF_LAYER_NAME_GET(Def));
#endif

  if ( DXF_ENTITY_IS_CURVE(e) )
    {
#if dbgdxfadd
    dbgp("   Add Entity is a curve");
#endif
    C3_CURVE c = dx3_entity_to_curve(e);
    if (c)
      {
      int discard = 0;
      C3_BOX_S box;
      double BiggestDim = 0.0;
      if( GetCurveBox(&box, c, &BiggestDim) && BiggestDim > MaxBigDim )
        {
        char b[1024];
        sprintf(b, "A DXF curve entity contains large(%g) numeric values\n.May it be discarded ?", BiggestDim);
        if (0)//AfxMessageBox(b, MB_YESNO|MB_ICONQUESTION)==IDYES)
          {
          discard = 1;
          }
        }
      if (!discard)
        {
        DML_ITEM it = dml_append_data(CurveLst, c);
        c->m_Color = DXF_ENTITY_COLOR(e);
        c->m_Thickness = DXF_ENTITY_THICKNESS(e);
        C3_CURVE_LAYER_SET(c,DXF_ENTITY_LAYER_GET(e)); // ensure the layers are same for change layer
        }
      }
    else
      LogError("GrfCmds", 0, "ERROR: Unable to add curve to layer!");

    dxf_free_entity(e);
    }
  else if (DXF_ENTITY_IS_INSERT(e))
    {
#if dbgdxfadd
    dbgp(" Entity is an insert");
#endif
    DXF_ENTITY_LAYER_SET(e,DXF_LAYER_NAME_GET(Def));
    DML_ITEM it = dml_append_data(InsertLst, e);
    DML_ITEM ait;
    DXF_ENTITY ae;
    if (DXF_INSERT_ATTRS(e))
      {
      WALK_ENTITY_LIST(DXF_INSERT_ATTRS(e), ait, ae)
        {
        DXF_ENTITY_LAYER_SET(ae,DXF_LAYER_NAME_GET(Def));
        }
      }
    }
  else
    {
#if dbgdxfadd
    dbgp(" Entity is other");
#endif

    C3_BOX_S box;
    int discard = 0;
    double BiggestDim = 0.0;
    if( GetEntityBox(&box, e, &BiggestDim) && BiggestDim > MaxBigDim )
      {
      char b[1024];
      sprintf(b, "A DXF entity contains large(%g) numeric values\n.May it be discarded ?", BiggestDim);
      if (AfxMessageBox(b, MB_YESNO|MB_ICONQUESTION)==IDYES)
        {
        discard = 1;
        }
      }
    if (!discard)
      {
      DXF_ENTITY_LAYER_SET(e,DXF_LAYER_NAME_GET(Def));
      if( !discard )
        DML_ITEM it = dml_append_data(OtherLst, e);
      }
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::Put(DXF_FILE f)
  {
#if dbgdxf
  dbgpln("  Put CGrfLayer %s ---------------------",DXF_LAYER_NAME_GET(Def));
#endif

  DML_ITEM it;
  DXF_ENTITY e;
  C3_CURVE c;
  PT3 extr;
  DXF_ENTITY_DESC desc;
  c3v_set_zero(extr);

  // The layer and linetype here should be entity specific
  // Do the color now and fix the linetype layer comeback

  WALK_CURVE_LIST(CurveLst, it, c)
    {
    desc = dxf_create_descriptor(DXF_LAYER_NAME_GET(Def), DXF_LAYER_LTYPE_GET(Def),
      c->m_Color, 0.0, extr);
    dx3_put_curve(f,c, desc);
    dxf_free_descriptor(desc);
    }


  // a fix has been put in library to prevent a curve of 1 vertex in the dxf file

  // start change check the text flags here and fix the alignment point
  if( Drawing->StyleLst->length )
    {
    WALK_ENTITY_LIST(InsertLst, it, e)
      {
      DML_ITEM ait;
      DXF_ENTITY ae;
      if (DXF_INSERT_ATTRS(e))
        {
        WALK_ENTITY_LIST(DXF_INSERT_ATTRS(e), ait, ae)
          {
          dxf_set_attrib_alignment_point(ae,Drawing->StyleLst);
          }
        }
      }

    WALK_ENTITY_LIST(OtherLst, it, e)
      {
      if( DXF_ENTITY_IS_TEXT(e))
        dxf_set_text_alignment_point(e,Drawing->StyleLst);
      }
    }
  // end change
  dxf_put_entities(f, InsertLst);
  dxf_put_entities(f, OtherLst);
  //dxf_free_descriptor(desc);
  }

//----------------------------------------------------------------------------

char* fetch_dxf_tag_value(DXF_ENTITY e,char *s)
  {
  DML_LIST l1 = DXF_INSERT_ATTRS(e);
  DML_ITEM it1;
  DXF_ENTITY e1;

  if( !l1 )
    return((char *)NULL);
  WALK_ENTITY_LIST(l1, it1, e1)
    {
    if (_stricmp(DXF_ATTRIB_TAG_GET(e1), s) == 0)
      {
      return DXF_ATTRIB_VALUE_GET(e1);
      }
    }
  return (char *)NULL;
  }

//----------------------------------------------------------------------------

void CGrfLayer::PrintEntities(pViewport vp, FILE *fp)
  {
  DML_ITEM it;
  C3_CURVE c;
  DXF_ENTITY e;
  CEntInView* ev;
  // General entity info
  fprintf(fp,"      CGrfLayer %s Flags:%d Color:%d LineType:%s\n",
    DXF_LAYER_NAME_GET (Def),
    DXF_LAYER_FLAGS(Def),
    DXF_LAYER_COLOR(Def),
    DXF_LAYER_LTYPE_GET(Def));


  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    ev = (CEntInView*)it->pKenwalta;
    fprintf(fp,"            Insert type:%d name:%-20s addr:%x Segment %x\n",e->entity_id,DXF_INSERT_BLOCK_GET(e),e,it->pKenwalta);
    if( ev )ev->Qprint(fp);
    }
  WALK_CURVE_LIST(CurveLst, it, c)
    {
    ev = (CEntInView*)it->pKenwalta;
    fprintf(fp,"            Curve addr:%x  Segment %x\n",c,it->pKenwalta);
    //     (void)fprintf(fp,"                  Box %12.3f %12.3f\n",C3_CURVE_X_MIN(c),C3_CURVE_X_MAX(c));
    //     (void)fprintf(fp,"                  Box %12.3f %12.3f\n",C3_CURVE_Y_MIN(c),C3_CURVE_Y_MAX(c));
    //     (void)fprintf(fp,"                  Box %12.3f %12.3f\n",C3_CURVE_Z_MIN(c),C3_CURVE_Z_MAX(c));
    if( ev )ev->Qprint(fp);
    }
  WALK_ENTITY_LIST(OtherLst, it, e)
    {
    ev = (CEntInView*)it->pKenwalta;
    fprintf(fp,"            Entity type:%d  addr:%x Segment %x\n",e->entity_id,e,it->pKenwalta);
    if ( ev )ev->Qprint(fp);
    }
  }

//----------------------------------------------------------------------------

DXF_ENTITY CGrfLayer::Find_Tagged_Insert(pchar Val)
  {
  DML_ITEM it;
  DXF_ENTITY e;

  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    pchar v;
    if( Val && (v = Find_Attr_Value(e,TagAttribStr)) && !strcmp(Val,v))
      return e;
    }
  return NULL;
  }

//----------------------------------------------------------------------------

void CGrfLayer::SetDiscColor(DML_LIST llist,int on)
  {
  DML_ITEM item;
  DXF_TABLE_ENTRY l;
  if( on )
    {
    m_DiscColor = GR_LIGHTGRAY;
    DML_WALK_LIST(llist,item)
      {
      l = (DXF_TABLE_ENTRY)DML_RECORD(item);
      if( _stricmp(DXF_LAYER_NAME_GET(l),DXF_LAYER_NAME_GET(Def)) == 0)
        {
        m_DiscColor = DXF_LAYER_COLOR(l);
        break;
        }
      }
    }
  else
    m_DiscColor = -1;
  }

//----------------------------------------------------------------------------

DML_LIST CGrfLayer::SelectNearbyCurves(DML_LIST l,PT3 tcog,REAL tol)
  {
  PT3        ecog;
  DML_ITEM   it;
  C3_CURVE   c;

  WALK_CURVE_LIST(CurveLst, it, c)
    {
    ecog[0] = (C3_CURVE_X_MIN(c) + C3_CURVE_X_MAX(c))/2.0;
    ecog[1] = (C3_CURVE_Y_MIN(c) + C3_CURVE_Y_MAX(c))/2.0;
    ecog[2] = 0.0;
    if (c3v_dist(tcog, ecog) < tol)
      dml_append_data(l,c);
    }
  return l;
  }

//----------------------------------------------------------------------------

DML_LIST CGrfLayer::SelectInsertsOnAttrCombo(DML_LIST l, pchar *AttrTags, pchar *AttrVals, IRegExpPtr *pRE)
  {
  pchar *pTag;
  pchar *pVal;
  DML_ITEM it;
  DXF_ENTITY ins;

  WALK_ENTITY_LIST(InsertLst, it, ins)
    {
    //if (!EntityIsSelectable(ins,NULL))
    //  continue;

    //mhmnew
    // for this insert test each tag and if any one is not True the break

    pTag = AttrTags;
    pVal = AttrVals;
    while (*pTag)
      {
      if (*pVal==NULL || strlen(*pVal)==0)
        {
        DXF_ENTITY ae = Find_Attr(ins, *pTag);
        if (ae==NULL)
          {
          break;
          }
        }
      else
        {
        char* pv = Find_Attr_Value(ins, *pTag);
        if (pv==NULL)
          {
          break;
          }
        else if (pRE)
          {
          int l=strlen(*pVal);
          // Check for end of line marker
          try
            {
            if (l && ((*pVal)[0]!='^' || (*pVal)[l-1]!='$'))
              {
              char Buff[256];
              strcpy(Buff, (*pVal)[0]!='^' ? "^" : "");
              strcat(Buff, *pVal);
              strcat(Buff, (*pVal)[l-1]!='$' ? "$" : "");
              (*pRE)->Pattern = Buff;
              }
            else
              {
              (*pRE)->Pattern = *pVal;
              }
            if (VARIANT_FALSE == (*pRE)->Test(pv))
              break;
            }
          catch (_com_error)
            {
            break;
            }
          }
        else if (_stricmp(pv, *pVal))
          {
          break;
          }
        }
      pTag++;
      pVal++;
      }
    if (!*pTag)
      {
      if (it->pKenwalta)
        ((CEntInView*)(it->pKenwalta))->SetMarkBit();
      if (l)
        dml_append_data(l, ins);
      }
    }
  return l;
  }

//----------------------------------------------------------------------------

void CGrfLayer::CountBlocksUsed(CWordArray& BlkCnt)
  {
  if (Def)
    {
    int index;
    pBlock b;
    DML_ITEM it;
    DXF_ENTITY e;
    WALK_ENTITY_LIST(InsertLst, it, e)
      {
      if (DXF_ENTITY_IS_INSERT(e))
        {
        b = Drawing->Blocks.Find(DXF_INSERT_BLOCK_GET(e), index);
        if (b)
          {
          if (BlkCnt[index]==0) //only need to try count embeded blocks once!
            b->CountBlocksUsed(Drawing, BlkCnt);
          BlkCnt[index]++;
          }
        }
      }
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::Draw(pViewport vp, C3_BOX box, int color, int Lvl,CGrfLayer * pLay,CGrfLayer * iLay)
  {
  static char *cur_tag;
  DML_ITEM it;
  C3_CURVE c;
  DXF_ENTITY e;

#if dbgCNM
  dbgpln("  %*s  Draw CGrfLayer %s ---------------------",Lvl," ",Def ? DXF_LAYER_NAME_GET(Def) : "NULL");
#endif

  WALK_CURVE_LIST(CurveLst,it, c)
    {
#if dbgCNM
    if (Def)
      dbgpln("  %*s  Draw Curve %x %x %i",Lvl," ",c, DXF_LAYER_FLAGS(Def), DXF_LAYER_COLOR(Def));
#endif
    if ( Def && ((DXF_LAYER_FLAGS(Def) & 0x01) == 0) && (DXF_LAYER_COLOR(Def) >= 0))
      {
      Drawing->Draw(c, vp, box, color, Lvl,pLay,iLay,it);
      }
    }

  WALK_ENTITY_LIST(InsertLst, it, e)
    {
#if dbgCNM
    if (Def)
      dbgpln("  %*s  Draw Insert %x %x %i",Lvl," ",e, DXF_LAYER_FLAGS(Def), DXF_LAYER_COLOR(Def));
#endif
    if ( Def && ((DXF_LAYER_FLAGS(Def) & 0x01) == 0) && (DXF_LAYER_COLOR(Def) >= 0))
      {
      Drawing->Draw(e, vp, box, color, Lvl,pLay,iLay,it);
      }
    }

  WALK_ENTITY_LIST(OtherLst, it, e)
    {
#if dbgCNM
    if (Def)
      dbgpln("  %*s  Draw Other %x %x %i",Lvl," ",e, DXF_LAYER_FLAGS(Def), DXF_LAYER_COLOR(Def));
#endif
    if ( Def && ((DXF_LAYER_FLAGS(Def) & 0x01) == 0) && (DXF_LAYER_COLOR(Def) >= 0))
      {
      Drawing->Draw(e, vp, box, color, Lvl,pLay,iLay,it);
      }
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::EntityInvalidate(DXF_ENTITY ein,C3_CURVE cin)
  {
  DML_ITEM it;
  C3_CURVE c;
  DXF_ENTITY e;

  if( ein )
    {
    WALK_ENTITY_LIST(InsertLst, it, e)
      {
      if (it->pKenwalta && ein == e )
        {
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }

    WALK_ENTITY_LIST(OtherLst, it, e)
      {
      if (it->pKenwalta && ein == e )
        {
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }
    }
  else if ( cin )
    {
    WALK_CURVE_LIST(CurveLst, it, c)
      {
      if (it->pKenwalta && cin == c)
        {
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }
    }
  else
    {
    WALK_CURVE_LIST(CurveLst, it, c)
      {
      if (it->pKenwalta )
        {
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }

    WALK_ENTITY_LIST(InsertLst, it, e)
      {
      if (it->pKenwalta)
        {
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }

    WALK_ENTITY_LIST(OtherLst, it, e)
      {
      if (it->pKenwalta)
        {
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }
    }
  }

//----------------------------------------------------------------------------

DXF_ENTITY CGrfLayer::ClosestInsert(PT3 Point, REAL &DistMin)
  {
  DML_ITEM it;
  DXF_ENTITY Cls = NULL;
  DXF_ENTITY e;
  DistMin = 1.0e38;
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    REAL d = c3v_dist(DXF_INSERT_PT(e), Point);
    if (d < DistMin)
      {
      DistMin = d;
      Cls = e;
      }
    }
  return Cls;
  }

//----------------------------------------------------------------------------
// start change
DXF_ENTITY CGrfLayer::ClosestEntity(PT3 Point, REAL &DistMin)
  {
  DML_ITEM it;
  DXF_ENTITY Cls = NULL;
  DXF_ENTITY e;
  DistMin = 1.0e38;

  // check the list of inserts
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    REAL d = c3v_dist(DXF_INSERT_PT(e), Point);
    //dbgpln(" On insert list %d\n",e->entity_id);
    if (d < DistMin)
      {
      DistMin = d;
      Cls = e;
      }
    }

  // check the other list
  WALK_ENTITY_LIST(OtherLst, it, e)
    {
    REAL d;
    if (DXF_ENTITY_IS_TEXT(e))
      {
      d = c3v_dist(DXF_TEXT_INS_PT(e),Point);
      }
    else
      {
      dbgpln(" Unimpleneted pic type %d\n",e->entity_id);
      }
    if (d < DistMin)
      {
      DistMin = d;
      Cls = e;
      }
    }


  return Cls;
  }
// end change
//-------------------------------------------------------------------------
C3_CURVE CGrfLayer::ClosestCurve(PT3 Point, REAL &DistMin)
  {
#ifdef dbgclosestc
  dbgpln("=====================ClosestCurve=================");
#endif
  DML_ITEM it;
  C3_CURVE Cls = NULL;
  C3_CURVE c;
  DistMin = 1.0e38;

  WALK_CURVE_LIST(CurveLst, it, c)
    {
    PT3 Ray;
    c3v_set(0.,0.,1.,Ray);
    REAL d=1.0e38;

    int seg;
    REAL rad, sweep;
    PT3 ctr, ept0, ept1, normal;
    flag more, is_arc;

    c3c_ept0(c,ept0);
#ifdef dbgclosestc
    dbgpln("  Point                       %8.2f,%8.2f",Point[0],Point[1]);
    dbgpln("  start   %8.2f,%8.2f ",ept0[0],ept0[1]);
#endif
    seg=0;
    more=c3c_get_pcurve_data(c,seg,&is_arc,ctr,ept1,&rad,normal,&sweep);
    while (more)
      {
      PT3 p;
      REAL t=c3v_project_line(ept0,ept1,Point,p);
      REAL dt;
      if (t < 0.0)
        dt=c3v_dist(ept0, Point);
      else if (t > 1.0)
        dt=c3v_dist(ept1, Point);
      else
        dt=c3v_dist(p, Point);

      d = Min(d,dt);

#ifdef dbgclosestc
      dbgpln("  seg %2i %8.2f,%8.2f   %8.2f,%8.2f   %8.4f  %8.2f",seg,ept1[0],ept1[1],p[0],p[1],t, dt);
#endif
      seg++;
      memmove(ept0,ept1,sizeof(ept0));
      more=c3c_get_pcurve_data(c,seg,&is_arc,ctr,ept1,&rad,normal,&sweep);
      }

    if (d < DistMin)
      {
      DistMin = d;
      Cls = c;
#ifdef dbgclosestc
      dbgpln("                                                           <<<<<<<<<");
#endif
      }
    }
#ifdef dbgclosestc
  dbgpln("==================================================");
#endif
  return Cls;
  }

//----------------------------------------------------------------------------

int isblanks(pchar s)
  {
  if( strlen(s) == 0)
    return 1;
  for( int i = 0; i < (int)strlen(s); i++ )
    {
    if( isspace(s[i]))
      continue;
    if( iscntrl(s[i]))
      continue;
    return 0;
    }
  return 1;
  }

//----------------------------------------------------------------------------

flag CGrfLayer::GetEntityBox(C3_BOX_S *box,DXF_ENTITY e,double *BiggestDim)
  {
  switch (DX_ENTITY_ID(e))
    {
    case DXF_POINT:
      c3a_box_init_pt(box,DXF_POINT_PT(e));
      break;
    case DXF_LINE:
      c3a_box_init_pt(box,DXF_LINE_PT0(e));
      c3a_box_append_pt(box,DXF_LINE_PT1(e));
      break;
    case DXF_CIRCLE:
      c3a_box_init_pt(box,DXF_CIRCLE_CTR(e));
      break;
    case DXF_ARC:
      c3a_box_init_pt(box,DXF_ARC_CTR(e));
      break;
    case DXF_TRACE:
      c3a_box_init_pt  (box,DXF_TRACE_PT0(e));
      c3a_box_append_pt(box,DXF_TRACE_PT1(e));
      c3a_box_append_pt(box,DXF_TRACE_PT2(e));
      c3a_box_append_pt(box,DXF_TRACE_PT3(e));
      break;
    case DXF_SOLID:
      c3a_box_init_pt  (box,DXF_SOLID_PT0(e));
      c3a_box_append_pt(box,DXF_SOLID_PT1(e));
      c3a_box_append_pt(box,DXF_SOLID_PT2(e));
      c3a_box_append_pt(box,DXF_SOLID_PT3(e));
      break;
    case DXF_TEXT:
      if( DXF_TEXT_VALUE_GET(e) && strlen(DXF_TEXT_VALUE_GET(e)) && !isblanks(DXF_TEXT_VALUE_GET(e)))
        {
        c3a_box_init_pt  (box,DXF_TEXT_INS_PT(e));
        c3a_box_append_pt(box,DXF_TEXT_AL_PT(e));
        }
      else
        {
        return False;
        }
      break;
    case DXF_INSERT:
      c3a_box_init_pt  (box,DXF_INSERT_PT(e));
      break;
    case DXF_ATTDEF:
      if( DXF_ATTDEF_VALUE_GET(e) && strlen(DXF_ATTDEF_VALUE_GET(e)) && !(DXF_ATTDEF_AFLAGS(e)|DXF_ATTDEF_INVIS) && !isblanks(DXF_ATTDEF_VALUE_GET(e)))
        c3a_box_init_pt  (box,DXF_ATTDEF_PT(e));
      else
        return False;
      break;
    case DXF_ATTRIB:
      if( DXF_ATTRIB_VALUE_GET(e) && strlen(DXF_ATTRIB_VALUE_GET(e)) && !(DXF_ATTRIB_AFLAGS(e)|DXF_ATTRIB_INVIS) && !isblanks(DXF_ATTRIB_VALUE_GET(e)))
        c3a_box_init_pt  (box,DXF_ATTRIB_PT(e));
      else
        return False;
      break;
    default:
#if dbgdxfgetb
      dbgpln(" GetEntityBox unimplemented entity type = %d\n",(int)DX_ENTITY_ID(e));
#endif
      return False;
      break;
    }

  if( BiggestDim )
    {
    *BiggestDim = 0.0;
    if( fabs(C3_MIN_X(box)) < *BiggestDim ) *BiggestDim = fabs(C3_MIN_X(box));
    if( fabs(C3_MIN_Y(box)) < *BiggestDim ) *BiggestDim = fabs(C3_MIN_Y(box));
    if( fabs(C3_MIN_Z(box)) < *BiggestDim ) *BiggestDim = fabs(C3_MIN_Z(box));
    if( fabs(C3_MAX_X(box)) < *BiggestDim ) *BiggestDim = fabs(C3_MAX_X(box));
    if( fabs(C3_MAX_Y(box)) < *BiggestDim ) *BiggestDim = fabs(C3_MAX_Y(box));
    if( fabs(C3_MAX_Z(box)) < *BiggestDim ) *BiggestDim = fabs(C3_MAX_Z(box));
    }
  return True;
  }

//----------------------------------------------------------------------------

flag CGrfLayer::GetCurveBox(C3_BOX_S *box,C3_CURVE c,C3_TRANSFORM c3t,double *BiggestDim)
  {
  PT3 pt0,pt1;
  c3t_eval_pt(C3_MIN_PT(C3_CURVE_BOX(c)), c3t, pt0);
  c3t_eval_pt(C3_MAX_PT(C3_CURVE_BOX(c)), c3t, pt1);
  c3a_box_init_pt(box, pt0);
  c3a_box_append_pt(box, pt1);

  if( BiggestDim )
    {
    *BiggestDim = 0.0;
    if( fabs(C3_MIN_X(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MIN_X(box));
    if( fabs(C3_MIN_Y(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MIN_Y(box));
    if( fabs(C3_MIN_Z(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MIN_Z(box));
    if( fabs(C3_MAX_X(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MAX_X(box));
    if( fabs(C3_MAX_Y(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MAX_Y(box));
    if( fabs(C3_MAX_Z(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MAX_Z(box));
    }

  return True;
  }

//----------------------------------------------------------------------------

flag CGrfLayer::GetCurveBox(C3_BOX_S *box,C3_CURVE c,double *BiggestDim)
  {
  PT3 pt0,pt1;
  c3v_copy(C3_MIN_PT(C3_CURVE_BOX(c)), pt0);
  c3v_copy(C3_MAX_PT(C3_CURVE_BOX(c)), pt1);
  c3a_box_init_pt(box, pt0);
  c3a_box_append_pt(box, pt1);

  if( BiggestDim )
    {
    *BiggestDim = 0.0;
    if( fabs(C3_MIN_X(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MIN_X(box));
    if( fabs(C3_MIN_Y(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MIN_Y(box));
    if( fabs(C3_MIN_Z(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MIN_Z(box));
    if( fabs(C3_MAX_X(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MAX_X(box));
    if( fabs(C3_MAX_Y(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MAX_Y(box));
    if( fabs(C3_MAX_Z(box)) > *BiggestDim ) *BiggestDim = fabs(C3_MAX_Z(box));
    }

  return True;
  }

//----------------------------------------------------------------------------

void CGrfLayer::Explode(C3_TRANSFORM xfrm, DXF_ENTITY insert,pBlock b)
  {
  DXF_ENTITY e;
  DML_ITEM it;
  C3_CURVE c;

  //Do the attributes
  DML_LIST alist = DXF_INSERT_ATTRS(insert);
  if( alist )
    {
    WALK_ENTITY_LIST(alist, it, e)
      {
      if( strlen(DXF_ATTRIB_VALUE_GET(e)) > 0 )
        {
        DXF_ENTITY text;

        text = dxf_create_text(
          NULL,
          DXF_ATTRIB_PT(e),           //insert pt
          DXF_ATTRIB_HEIGHT(e),       // height
          DXF_ATTRIB_VALUE_GET(e),        //text
          DXF_ATTRIB_ROT_ANG(e),      //angle
          DXF_ATTRIB_X_SCALE(e),      //scale
          DXF_ATTRIB_STYLE_GET(e)         //style_name
          );
        Drawing->Add_Entity(text);//add to the current layer
        Drawing->pDsp->Draw(text, -1);
        }
      }
    }

  // Curves must be transformed by the matrix supplied
  WALK_CURVE_LIST(CurveLst,it, c)
    {
    C3_CURVE cnew = c3d_copy(c);
    ASSERT(cnew);
    cnew->m_Color = c->m_Color;
    cnew->m_Thickness = c->m_Thickness;

    C3_CURVE_LAYER_SET(cnew, DXF_LAYER_NAME_GET(Def));

    Drawing->SetCurrentLayer(DXF_LAYER_NAME_GET(Def));
    c3c_transform(cnew, xfrm);
    Drawing->Add_Curve(cnew);
    Drawing->pDsp->Draw(cnew, -1);
    }

  // Insert must have the scale translation and rotation included
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    DXF_ENTITY enew;

    enew = dxf_create_insert(NULL,
      DXF_INSERT_BLOCK_GET(e),
      DXF_INSERT_PT(e),
      DXF_INSERT_X_SCALE(e),
      DXF_INSERT_ROT_ANG(e),
      DXF_INSERT_COLS_CNT(e),
      DXF_INSERT_ROWS_CNT(e),
      DXF_INSERT_COL_SPC(e),
      DXF_INSERT_ROW_SPC(e)
      );
    ASSERT(enew);
    DXF_ENTITY_LAYER_SET(enew,DXF_ENTITY_LAYER_GET(e));
    DXF_ENTITY_HANDLE_SET(enew,DXF_ENTITY_HANDLE_GET(e));
    DXF_ENTITY_LTYPE_SET(enew,DXF_ENTITY_LTYPE_GET(e));

    DXF_ENTITY_THICKNESS(enew)    = DXF_ENTITY_THICKNESS(e);
    DXF_ENTITY_COLOR(enew)        = DXF_ENTITY_COLOR(e);
    DXF_ENTITY_EXTRUSION(enew)[0] = DXF_ENTITY_EXTRUSION(e)[0];
    DXF_ENTITY_EXTRUSION(enew)[1] = DXF_ENTITY_EXTRUSION(e)[1];
    DXF_ENTITY_EXTRUSION(enew)[2] = DXF_ENTITY_EXTRUSION(e)[2];


    DXF_INSERT_PT(enew)[0]   += DXF_INSERT_PT(insert)[0];
    DXF_INSERT_PT(enew)[1]   += DXF_INSERT_PT(insert)[1];
    DXF_INSERT_PT(enew)[2]   += DXF_INSERT_PT(insert)[2];

    DXF_INSERT_X_SCALE(enew) *= DXF_INSERT_X_SCALE(insert);
    DXF_INSERT_Y_SCALE(enew) *= DXF_INSERT_Y_SCALE(insert);
    DXF_INSERT_Z_SCALE(enew) *= DXF_INSERT_Z_SCALE(insert);

    DXF_INSERT_ROT_ANG(enew) += DXF_INSERT_ROT_ANG(insert);

    Drawing->SetCurrentLayer(DXF_LAYER_NAME_GET(Def));
    Drawing->Add_Insert(enew);
    Drawing->pDsp->Draw(enew,-1);
    }

  // Text must be translated and rotated and scaled by the transform.
  // scaleing must be applied to the height
  // Attributes are done as above but are converted back to text
  WALK_ENTITY_LIST(OtherLst, it, e)
    {
    if( DXF_ENTITY_IS_TEXT(e) )
      {
      DXF_ENTITY enew;

      enew = dxf_create_text(
        NULL,
        DXF_TEXT_INS_PT(e),       //insert pt
        DXF_TEXT_HEIGHT(e),       // height
        DXF_TEXT_VALUE_GET(e),        //text
        DXF_TEXT_ROT_ANG(e),      //angle
        DXF_TEXT_X_SCALE(e),      //scale
        DXF_TEXT_STYLE_GET(e)         //style_name
        );



      ASSERT(enew);
      DXF_ENTITY_LAYER_SET(enew,DXF_ENTITY_LAYER_GET(e));
      DXF_ENTITY_HANDLE_SET(enew,DXF_ENTITY_HANDLE_GET(e));
      DXF_ENTITY_LTYPE_SET(enew,DXF_ENTITY_LTYPE_GET(e));

      DXF_ENTITY_THICKNESS(enew)    = DXF_ENTITY_THICKNESS(e);
      DXF_ENTITY_COLOR(enew)        = DXF_ENTITY_COLOR(e);
      DXF_ENTITY_EXTRUSION(enew)[0] = DXF_ENTITY_EXTRUSION(e)[0];
      DXF_ENTITY_EXTRUSION(enew)[1] = DXF_ENTITY_EXTRUSION(e)[1];
      DXF_ENTITY_EXTRUSION(enew)[2] = DXF_ENTITY_EXTRUSION(e)[2];


      DXF_TEXT_INS_PT(enew)[0] += DXF_INSERT_PT(insert)[0];
      DXF_TEXT_INS_PT(enew)[1] += DXF_INSERT_PT(insert)[1];
      DXF_TEXT_INS_PT(enew)[2] += DXF_INSERT_PT(insert)[2];

      DXF_TEXT_AL_PT(enew)[0]  += DXF_INSERT_PT(insert)[0];
      DXF_TEXT_AL_PT(enew)[1]  += DXF_INSERT_PT(insert)[1];
      DXF_TEXT_AL_PT(enew)[2]  += DXF_INSERT_PT(insert)[2];

      DXF_TEXT_X_SCALE(enew)   *= DXF_INSERT_X_SCALE(insert);
      DXF_TEXT_HEIGHT(enew)    *= DXF_INSERT_Y_SCALE(insert);

      Drawing->SetCurrentLayer(DXF_LAYER_NAME_GET(Def));
      Drawing->Add_Entity(enew);
      Drawing->pDsp->Draw(enew,-1);
      }
    else
      {
      TRACE("Unknown otherlst entity type in explode\n");
      }
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds)
  {
  DML_ITEM it;
  C3_CURVE c;
  C3_BOX_S box;

  WALK_CURVE_LIST(CurveLst,it, c)
    {
    if( GetCurveBox(&box,c,c3t))
      {
      if (C3_MAX_X(Bounds) < 1.0e30)
        {
        c3a_box_append(Bounds, &box);
        }
      else
        {
        c3a_box_copy(&box, Bounds);
        }

#if dbgdxfgetb
      dbgp("\nCurve Lst X:%6.0f %6.0f  Y:%6.0f %6.0f   Z:%6.0f %6.0f",C3_MIN_X(&box),C3_MAX_X(&box),C3_MIN_Y(&box), C3_MAX_Y(&box), C3_MIN_Z(&box), C3_MAX_Z(&box));
#endif
      }
    }

  DXF_ENTITY e;
  WALK_ENTITY_LIST(InsertLst, it, e)
    {
    pBlock b;
    b=Drawing->Blocks.Find(DXF_INSERT_BLOCK_GET(e));
    if (b)
      {
      C3_TRANSFORM c3t_mem;
      ASSIGN_ARRAY(c3t_mem, c3t, c3t_mem);

      C3_TRANSFORM Imat;
      Drawing->ConstructInsertMatrix(Imat,b,e);
      c3t_mult(Imat,c3t_mem,c3t);
      b->GetBounds(c3t, Bounds);
      ASSIGN_ARRAY(c3t, c3t_mem, c3t_mem);
      }

#if dbgdxfgetb
    dbgp("\nInsertLst X:%6.0f %6.0f  Y:%6.0f %6.0f   Z:%6.0f %6.0f",C3_MIN_X(&box),C3_MAX_X(&box),C3_MIN_Y(&box), C3_MAX_Y(&box), C3_MIN_Z(&box), C3_MAX_Z(&box));
    dbgp("\n : %s", DXF_INSERT_BLOCK_GET(e));
#endif
    }

  WALK_ENTITY_LIST(OtherLst, it, e)
    {
    if( GetEntityBox(&box,e))
      {
      /***
      CNM Removed because Transform is not taken into account

      if (C3_MAX_X(Bounds) < 1.0e30)
      c3a_box_append(Bounds, &box);
      else
      c3a_box_copy(&box, Bounds);
      */

#if dbgdxfgetb
      dbgp("\n OtherLst X:%6.0f %6.0f  Y:%6.0f %6.0f   Z:%6.0f %6.0f",C3_MIN_X(&box),C3_MAX_X(&box),C3_MIN_Y(&box), C3_MAX_Y(&box), C3_MIN_Z(&box), C3_MAX_Z(&box));
#endif
      }
    }
  }

//----------------------------------------------------------------------------

void CGrfLayer::GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds, DML_LIST TheCurveLst, DML_LIST TheInsertLst, DML_LIST TheOtherLst)
  {
  DML_ITEM it;
  C3_CURVE c;
  C3_BOX_S box;

  WALK_CURVE_LIST(TheCurveLst,it, c)
    {
    if( GetCurveBox(&box,c,c3t))
      {
      if (C3_MAX_X(Bounds) < 1.0e30)
        {
        c3a_box_append(Bounds, &box);
        }
      else
        {
        c3a_box_copy(&box, Bounds);
        }

#if dbgdxfgetb
      dbgp("\nCurve Lst X:%6.0f %6.0f  Y:%6.0f %6.0f   Z:%6.0f %6.0f",C3_MIN_X(&box),C3_MAX_X(&box),C3_MIN_Y(&box), C3_MAX_Y(&box), C3_MIN_Z(&box), C3_MAX_Z(&box));
#endif
      }
    }

  DXF_ENTITY e;
  WALK_ENTITY_LIST(TheInsertLst, it, e)
    {
    if (DX_ENTITY_ID(e)==DXF_INSERT)
      {
      pBlock b;
      b=Drawing->Blocks.Find(DXF_INSERT_BLOCK_GET(e));
      if (b)
        {
        C3_TRANSFORM c3t_mem;
        ASSIGN_ARRAY(c3t_mem, c3t, c3t_mem);

        C3_TRANSFORM Imat;
        Drawing->ConstructInsertMatrix(Imat,b,e);
        c3t_mult(Imat,c3t_mem,c3t);
        b->GetBounds(c3t, Bounds);
        ASSIGN_ARRAY(c3t, c3t_mem, c3t_mem);
        }
      }
    else
      {
      dbgp("GetBounds Missing code for ID %i",DX_ENTITY_ID(e));
      }

#if dbgdxfgetb
    dbgp("\nInsertLst X:%6.0f %6.0f  Y:%6.0f %6.0f   Z:%6.0f %6.0f",C3_MIN_X(&box),C3_MAX_X(&box),C3_MIN_Y(&box), C3_MAX_Y(&box), C3_MIN_Z(&box), C3_MAX_Z(&box));
    dbgp("\n : %s", DXF_INSERT_BLOCK_GET(e));
#endif
    }

  WALK_ENTITY_LIST(TheOtherLst, it, e)
    {
    if( GetEntityBox(&box,e))
      {
      /***
      CNM Removed because Transform is not taken into account

      if (C3_MAX_X(Bounds) < 1.0e30)
      c3a_box_append(Bounds, &box);
      else
      c3a_box_copy(&box, Bounds);
      */

#if dbgdxfgetb
      dbgp("\n OtherLst X:%6.0f %6.0f  Y:%6.0f %6.0f   Z:%6.0f %6.0f",C3_MIN_X(&box),C3_MAX_X(&box),C3_MIN_Y(&box), C3_MAX_Y(&box), C3_MIN_Z(&box), C3_MAX_Z(&box));
#endif
      }
    }
  }

//============================================================================

LayerCollection::LayerCollection()
  {
  Drawing = NULL;
  Block = NULL;
  Layers = NULL;

  MakeLayerName(CurLayer, Drawing_Layer, "");
  };

//----------------------------------------------------------------------------

LayerCollection::LayerCollection(pDXF_Drawing DrawingIn, pBlock BlockIn)
  {
  Drawing = DrawingIn;
  Block = BlockIn;
  Layers = NULL;

  MakeLayerName(CurLayer, Drawing_Layer, "");
  };

//----------------------------------------------------------------------------

LayerCollection::~LayerCollection()
  {
  while (Layers)
    {
    CGrfLayer * ln = Layers;
    Layers=Layers->Nxt;
    delete ln;
    }
  };

//----------------------------------------------------------------------------

BOOL LayerCollection::SetLayerState(pViewport vp, pchar LayName, BOOL set, int state)
  {
  if (!LayName)
    return FALSE;
  BOOL ret = FALSE;
  pchar ucstr = strupr(strdup(LayName));
  DML_LIST llist = Drawing->LayerLst;
  DML_ITEM item;
  DXF_TABLE_ENTRY layer;
  DML_WALK_LIST(llist, item)
    {
    layer = (DXF_TABLE_ENTRY)DML_RECORD(item);
    if (_stricmp(DXF_LAYER_NAME_GET(layer), ucstr) == 0)
      {
      if (set)
        DXF_LAYER_FLAGS(layer) |= state;
      else
        DXF_LAYER_FLAGS(layer) &= ~state;
      ret = TRUE;
      break;
      }
    }

  // find this layer
  CGrfLayer * l = Find(ucstr);
  if (l)
    l->EntityInvalidate();
  free(ucstr);
  return ret;
  }

//----------------------------------------------------------------------------

BOOL LayerCollection::SetLayerColor(pViewport vp, pchar LayName, int color)
  {
  if (!LayName)
    return FALSE;
  BOOL ret = FALSE;
  pchar ucstr = strupr(strdup(LayName));
  DML_LIST llist = Drawing->LayerLst;
  DML_ITEM item;
  DXF_TABLE_ENTRY layer;
  DML_WALK_LIST(llist, item)
    {
    layer = (DXF_TABLE_ENTRY)DML_RECORD(item);
    if (_stricmp(DXF_LAYER_NAME_GET(layer), ucstr) == 0)
      {
      DXF_LAYER_COLOR(layer) = color;
      ret = TRUE;
      break;
      }
    }

  // find this layer
  CGrfLayer * l = Find(ucstr);
  if (l)
    l->EntityInvalidate();
  free(ucstr);
  return ret;
  }

//----------------------------------------------------------------------------

void LayerCollection::SetCurrentLayer(LayerTypes Typ, pchar LayName)
  {
  if( !LayName )
    LayName = "";
  MakeLayerName(CurLayer, Typ, LayName);
  }

//----------------------------------------------------------------------------

void LayerCollection::SetCurrentLayer(pchar LayName)
  {
  if( !LayName )
    LayName = "";
  strcpy(CurLayer, LayName);
  }

//----------------------------------------------------------------------------

void LayerCollection::CountBlocksUsed(CWordArray& BlkCnt)
  {
  CGrfLayer * l = NULL;
  WALK_LAYERS(Layers,NULL, l)
    l->CountBlocksUsed(BlkCnt);
  }

//----------------------------------------------------------------------------

CGrfLayer * LayerCollection::Find(pchar LayerName)
  {
  if (LayerName == NULL || (strcmp(LayerName, "")==0))
    LayerName = CurLayer;
  CGrfLayer * l = NULL;
  WALK_LAYERS(Layers,NULL, l)
    if (l->Def)
      if (_stricmp(LayerName, DXF_LAYER_NAME_GET(l->Def))==0)
        break;
  return l;
  }

//---------------------------------------------------------------------------

void LayerCollection::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  CGrfLayer * l = NULL;
  WALK_LAYERS(Layers,NULL, l)
    l->ChangeTag(pOldTag,pNewTag);
  }

//----------------------------------------------------------------------------

void LayerCollection::CollectTags(Strng_List &TagList)
  {
  CGrfLayer * l = NULL;
  WALK_LAYERS(Layers,NULL, l)
    l->CollectTags(TagList);
  }

//----------------------------------------------------------------------------

flag LayerCollection::FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr)
  {
  CGrfLayer * l = NULL;
  WALK_LAYERS(Layers,NULL, l)
    {
    if (l->FindTag(Tag, pLastMatchingInsert, pLastMatchingAttr))
      return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag LayerCollection::UpdateModelAssocGraphics(CMdlAssocGraphicList & Grfs, CMdlAssocGraphicMap & Map, CList<CMdlAssocGraphicX,CMdlAssocGraphicX&> & GrfsX)
  {
  flag WorkReqd =0;
  CGrfLayer * l = NULL;
  WALK_LAYERS(Layers,NULL, l)
    WorkReqd |= l->UpdateModelAssocGraphics(Grfs, Map, GrfsX);
  return WorkReqd;
  }

//----------------------------------------------------------------------------

CGrfLayer * LayerCollection::Append(DXF_TABLE_ENTRY layer)
  {
  CGrfLayer * ln = new CGrfLayer(Drawing, Block);
  CGrfLayer * l;
  if (!Layers)
    Layers = ln;
  else
    {
    l = Layers;
    while (l->Nxt)
      l = l->Nxt;
    l->Nxt = ln;
    }
  ln->Def = layer;
  return ln;
  }

//----------------------------------------------------------------------------

int testdxstring(DX_STRING *s,int len)
  {
  if( !s ) return 0;
  if( s->size < 0 ) return 0;
  if( s->size > len ) return 0;
  if( !s->p )return 0;
  pchar ptr = s->p;
  while( ptr <= (s->p+len))
    {
    if( *ptr == NULL )break;
    ptr++;
    }
  if( ptr > (s->p+len))return 0;
  len = strlen(s->p );
  ptr = s->p;
  while( *ptr )
    {
    if( iscntrl(*ptr))return 0;
    if( !isprint(*ptr))return 0;
    ptr++;
    }
  return 1;
  }

//----------------------------------------------------------------------------

void LayerCollection::Free(CGrfLayer * layer)
  {
  CGrfLayer * l = Layers, * lp;
  if (l == layer)
    Layers = l->Nxt;
  else
    {
    while (l != layer)
      {
      lp = l;
      l = l->Nxt;
      }
    lp->Nxt = l->Nxt;
    }
  dxf_free_table_entry(l->Def);
  delete l;
  }

//----------------------------------------------------------------------------

void LayerCollection::Get(DXF_FILE file, int What, int test_mode)
  {
  CGrfLayer * l ;
  pchar  BlockName = (Block ? DXF_BLOCK_NAME_GET(Block->Def) : NULL);

  if( What == 99 )
    {
#if dbgdxfget
    dbgpln(" LayerCollection::Get - has 99 what val indicates load form disc\n");
#endif
    dxf_set_filepos(file, NULL, DXF_FIRST_ENTITY);
    }
  else
    {
#if dbgdxfget
    dbgpln(" LayerCollection::Get - has !99 what val indicates load from disc\n");
#endif
    dxf_set_filepos(file, BlockName, DXF_FIRST_ENTITY);
    }


  int nnn=0;
  DXF_ENTITY e = dxf_get_entity(file, NULL);
  while (e)
    {
    nnn++;
    // check out the layers, colors and styles
    if (What == 99)
      {
      dxf_set_entity_layer(e,"0");
      dxf_set_entity_color(e, -1);
      l = Find(DXF_ENTITY_LAYER_GET(e));
      if (!l)
        l = Append(Drawing->FindLayerDef(DXF_ENTITY_LAYER_GET(e)));
      ASSERT(l);
      }
    else
      {
      if( DXF_ENTITY_COLOR_STATUS(e))
        dxf_set_entity_color(e,-1);
      l = Find(DXF_ENTITY_LAYER_GET(e));
      if (!l)
        l = Append(Drawing->FindLayerDef(DXF_ENTITY_LAYER_GET(e)));

      ASSERT(l->Def);
      }

    // check the entity name
    pchar ename="",elayer="",estyle="",eblock="";
    if(!(ename = dx0_entity_name(e->entity_id)) )
      {
      int lnno = dxf_current_line(file->fp);
      LogWarning("DXF Load",LF_Exclamation,"Bad entity name( line# %d ) - attempting to skip over",lnno);
      continue;

      }

    // check the entity layer
    if( !testdxstring(DXF_ENTITY_LAYER_PTR(e),32))
      {
      DXF_ENTITY_LAYER_SET(e,"0");
      int lnno = dxf_current_line(file->fp);
      LogWarning("DXF Load",LF_Exclamation,"Bad entity layer( line# %d ) - assigning to 0",lnno);
      }
    l = Find(DXF_ENTITY_LAYER_GET(e));
    if (!l)
      {
      l = Append(Drawing->FindLayerDef(DXF_ENTITY_LAYER_GET(e)));
      int lnno = dxf_current_line(file->fp);
      LogWarning("DXF Load",LF_Exclamation,"Nonexistant layer %s found( line# %d ) - Appending",DXF_ENTITY_LAYER_GET(e),lnno);
      }
    elayer = DXF_ENTITY_LAYER_GET(e);

    // check the entity style
    if( DXF_ENTITY_IS_ATTDEF(e) )
      {
      if( !testdxstring(DXF_ATTDEF_STYLE_PTR(e),32))
        {
        DXF_ATTDEF_STYLE_SET(e,"STANDARD");
        int lnno = dxf_current_line(file->fp);
        LogWarning("DXF Load",LF_Exclamation,"Bad attdef style(line# %d) - assigned to STANDARD",lnno);
        }
      else if( Drawing->FindStyleDef(DXF_ATTDEF_STYLE_GET(e)))
        {
        DXF_ATTDEF_STYLE_SET(e,"STANDARD");
        int lnno = dxf_current_line(file->fp);
        LogWarning("DXF Load",LF_Exclamation,"Attrib style %s not found(line# %d) - assigned to STANDARD",DXF_ATTRIB_STYLE_GET(e),lnno);
        }
      estyle = DXF_ATTRIB_STYLE_GET(e);
      }


    if( DXF_ENTITY_IS_TEXT(e) )
      {
      if( !testdxstring(DXF_TEXT_STYLE_PTR(e),32))
        {
        DXF_TEXT_STYLE_SET(e,"STANDARD");
        int lnno = dxf_current_line(file->fp);
        LogWarning("DXF Load",LF_Exclamation,"Bad text style(line# %d) - assigned to STANDARD",lnno);
        }
      else if( Drawing->FindStyleDef(DXF_TEXT_STYLE_GET(e)))
        {
        DXF_TEXT_STYLE_SET(e,"STANDARD");
        int lnno = dxf_current_line(file->fp);
        LogWarning("DXF Load",LF_Exclamation,"Text style %s not found(line# %d) - assigned to STANDARD",DXF_ATTDEF_STYLE_GET(e),lnno);
        }
      estyle = DXF_TEXT_STYLE_GET(e);
      }



    if( DXF_ENTITY_IS_ATTRIB(e) )
      {
      if( !testdxstring(DXF_ATTRIB_STYLE_PTR(e),32))
        {
        DXF_ATTRIB_STYLE_SET(e,"STANDARD");
        int lnno = dxf_current_line(file->fp);
        LogWarning("DXF Load",LF_Exclamation,"Bad attrib style(line# %d) - assigned to STANDARD",lnno);
        }
      else if( Drawing->FindStyleDef(DXF_ATTRIB_STYLE_GET(e)))
        {
        DXF_ATTRIB_STYLE_SET(e,"STANDARD");
        int lnno = dxf_current_line(file->fp);
        LogWarning("DXF Load",LF_Exclamation,"Attdef style %s not found(line# %d) - assigned to STANDARD",DXF_ATTRIB_STYLE_GET(e),lnno);
        }
      estyle = DXF_ATTRIB_STYLE_GET(e);
      }

    //dbgpln("GetEntity Block:%-12s Entity:%-12s CGrfLayer:%-12s Style:%-12s",eblock,ename,elayer,estyle);

    l->Add(e);
    e = dxf_get_entity(file, NULL);

    }
  }

// ---------------------------------------------------------------------------

void LayerCollection::Put(DXF_FILE file)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    if (l->Def)
      l->Put(file);
  }

//----------------------------------------------------------------------------

void LayerCollection::Draw(pViewport vp, C3_BOX box, int color, int Lvl,CGrfLayer * iLay)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->Draw(vp, box, color, Lvl, l, iLay);
  }

//----------------------------------------------------------------------------

DML_LIST LayerCollection::SelectInsertsOnAttrCombo(DML_LIST ll,pchar *AttrTags,pchar *AttrVals, IRegExpPtr *pRE)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->SelectInsertsOnAttrCombo(ll, AttrTags, AttrVals, pRE);
  return ll;
  }

//----------------------------------------------------------------------------

DML_LIST LayerCollection::SelectNearbyCurves(DML_LIST ll,PT3 p,REAL tol)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->SelectNearbyCurves(ll, p, tol);
  return ll;
  }

//----------------------------------------------------------------------------

void LayerCollection::SetDiscColor(DML_LIST llist,int on)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->SetDiscColor(llist, on);
  }

//----------------------------------------------------------------------------

void LayerCollection::EntityInvalidate(DXF_ENTITY e,C3_CURVE c)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->EntityInvalidate(e, c);
  }

//----------------------------------------------------------------------------

void LayerCollection::PrintEntities(pViewport vp, FILE *fp)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->PrintEntities(vp, fp);
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Find_Tagged_Insert(pchar Val)
  {
  DXF_ENTITY e;
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    if (e = l->Find_Tagged_Insert(Val))
      return e;
  return NULL;
  }

//----------------------------------------------------------------------------

void LayerCollection::Explode(C3_TRANSFORM xfrm,DXF_ENTITY insert,pBlock b)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->Explode(xfrm, insert, b);
  }

//----------------------------------------------------------------------------

void LayerCollection::GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds)
  {
#if dbgdxfgetb
  dbgp("\nLayerCollection::GetBounds");
#endif
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->GetBounds(c3t, Bounds);
  }

//----------------------------------------------------------------------------

void LayerCollection::GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds, DML_LIST TheCurveLst, DML_LIST TheInsertLst, DML_LIST TheOtherLst)
  {
#if dbgdxfgetb
  dbgp("\nLayerCollection::GetBounds");
#endif
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->GetBounds(c3t, Bounds, TheCurveLst, TheInsertLst, TheOtherLst);
  }

//----------------------------------------------------------------------------

void LayerCollection::Clear()
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    l->Clear();
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::ClosestEntity(PT3 Point, REAL &DistMin)
  {
  DXF_ENTITY Cls = NULL;
  DistMin = 1e38;
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    REAL DistMinL;
    DXF_ENTITY ClsL = l->ClosestEntity(Point, DistMinL);
    if (DistMinL < DistMin)
      {
      Cls = ClsL;
      DistMin = DistMinL;
      }
    }
  return Cls;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::ClosestInsert(PT3 Point, REAL &DistMin)
  {
  DXF_ENTITY Cls = NULL;
  DistMin = 1e38;
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    REAL DistMinL;
    DXF_ENTITY ClsL = l->ClosestInsert(Point, DistMinL);
    if (DistMinL < DistMin)
      {
      Cls = ClsL;
      DistMin = DistMinL;
      }
    }
  return Cls;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::ClosestCurve(PT3 Point, REAL &DistMin)
  {
  C3_CURVE Cls = NULL;
  DistMin = 1e38;
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    REAL DistMinL;
    C3_CURVE ClsL = l->ClosestCurve(Point, DistMinL);
    if (DistMinL < DistMin)
      {
      Cls = ClsL;
      DistMin = DistMinL;
      }
    }
  return Cls;
  }

//----------------------------------------------------------------------------

void LayerCollection::AttrValuesInWin(pchar AttrTag, PT3 mn, PT3 mx, pStrng_List &Lst)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    DML_ITEM it;
    DXF_ENTITY e;
    WALK_ENTITY_LIST(l->InsertLst, it, e)
      {
      if (DXF_INSERT_PT(e)[0] >= mn[0] && DXF_INSERT_PT(e)[0] <= mx[0] &&
        DXF_INSERT_PT(e)[1] >= mn[1] && DXF_INSERT_PT(e)[1] <= mx[1])
        {
        DML_LIST l1 = DXF_INSERT_ATTRS(e);
        DML_ITEM it1;
        DXF_ENTITY e1;
        WALK_ENTITY_LIST(l1, it1, e1)
          if (_stricmp(DXF_ATTRIB_TAG_GET(e1), AttrTag)==0)
            {
            if (!Lst)
              Lst = new Strng_List();
            Lst->Append(DXF_ATTRIB_VALUE_GET(e1));
            }
        }
      }
    }
  }

//----------------------------------------------------------------------------

FILE * gs_hDXF=NULL;
long gs_DBHandle=0;

C3_CURVE LayerCollection::Add_PLine_Start(PT3 pt)
  {
#if dbgdxf
  dbgpln("Add_PLine_Start %s ",CurLayer);
#endif
  if (gs_hDXF)
    {
    fprintf(gs_hDXF, "0\nPOLYLINE\n  5\n%i\n  8\n0\n 66\n   1\n10\n%.18f\n 20\n%.18f\n 30\n%.18f\n",gs_DBHandle++, pt[0], pt[1], pt[2]);
    }

  C3_CURVE PLine = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    PLine = c3d_pcurve_init(pt);
    C3_CURVE_LAYER_SET(PLine,CurLayer);
    dml_append_data(l->CurveLst, PLine);
    }
  return PLine;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_PLine_Vertex(C3_CURVE PLine, PT3 pt)
  {
  if (gs_hDXF)
    {
    fprintf(gs_hDXF, "0\nVERTEX\n  5\n%i\n  8\n0\n10\n%.18f\n 20\n%.18f\n 30\n%.18f\n",gs_DBHandle++, pt[0], pt[1], pt[2]);
    }
  if (PLine)
    {
#if dbgdxf
    dbgpln("Add_PLine_Vertex pt %g  %g  %g",pt[0],pt[1],pt[2]);
#endif
    c3d_pcurve_add_line(PLine, pt);
    //      dxf_insert_add_attrib(NULL, UnitInsert, pt, 5.0, Tag,  "Tag", 0, 20,0.0,1.0,"");
    }

  return PLine;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_Arc(PT3 pts, PT3 ptm, PT3 pte)
  {
  C3_CURVE PLine = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
#if dbgdxf
    dbgpln("Add_Arc s %g  %g  %g",pts[0],pts[1],pts[2]);
    dbgpln("        m %g  %g  %g",ptm[0],ptm[1],ptm[2]);
    dbgpln("        e %g  %g  %g",pte[0],pte[1],pte[2]);
#endif

    PLine = c3d_arc_3pts(pts, ptm, pte);
    dml_append_data(l->CurveLst, PLine);


    ASSERT((l->CurveLst->length >= 0) && (l->CurveLst->length < MaxBigDim ));
    }
  return PLine;
  }
//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_Arc(PT3 pt, REAL r, REAL s, REAL e)
  {
  double NumFullCircles=fabs(s-e)/Degs2Rads(360.0);

  if (fabs(NumFullCircles-int(NumFullCircles))> 0.001)
    {
    if (gs_hDXF)
      {
      fprintf(gs_hDXF, "0\nARC\n  5\n%i\n  8\n0\n 10\n%.18f\n 20\n%.18f\n 30\n%.18f\n 40\n%.18f\n 50\n%.18f\n 51\n%.18f\n",
        gs_DBHandle++, pt[0], pt[1], pt[2], r, Rads2Degs(s), Rads2Degs(e));
      }
    PT3 pts, ptm, pte;
    c3v_set(pt[0]+r*cos(s), pt[1]+r*sin(s), 0.0, pts);
    c3v_set(pt[0]+r*cos((s+e)*0.5), pt[1]+r*sin((s+e)*0.5), 0.0, ptm);
    c3v_set(pt[0]+r*cos(e), pt[1]+r*sin(e), 0.0, pte);
    return Add_Arc(pts, ptm, pte);
    }

  C3_CURVE PLine = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    if (gs_hDXF)
      {
      fprintf(gs_hDXF, "0\nCIRCLE\n  5\n%i\n  8\n0\n 10\n%.18f\n 20\n%.18f\n 30\n%.18f\n 40\n%.18f\n",gs_DBHandle++, pt[0], pt[1], pt[2], r);
      }
    // A Full Circle
    PT3 n;
    c3v_set(0.0, 0.0, 1.0, n);

#if dbgdxf
    dbgpln("Add_Circle %g  %g  %g R:%g",pt[0],pt[1],pt[2],r);
#endif

    PLine = c3d_circle(pt, r, n);
    dml_append_data(l->CurveLst, PLine);

    ASSERT((l->CurveLst->length >= 0) && (l->CurveLst->length < MaxBigDim ));

    }
  return PLine;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_Circle(PT3 pt, REAL r, PT3 zaxis)
  {
  if (gs_hDXF)
    {
    fprintf(gs_hDXF, "0\nCIRCLE\n  5\n%i\n  8\n0\n 10\n%.18f\n 20\n%.18f\n 30\n%.18f\n 40\n%.18f\n",gs_DBHandle++, pt[0], pt[1], pt[2], r);
    }
  C3_CURVE PLine = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    PLine = c3d_circle(pt,r,zaxis);
    dml_append_data(l->CurveLst, PLine);
    }
  return PLine;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_Circle(PT3 pt1, PT3 pt2, PT3 pt3)
  {
  if (gs_hDXF)
    {
    //fprintf(gs_hDXF, "0\nCIRCLE\n  5\n%i\n  8\n0\n 10\n%.18f\n 20\n%.18f\n 30\n%.18f\n 40\n%.18f\n",gs_DBHandle++, pt[0], pt[1], pt[2], r);
    }
  C3_CURVE PLine = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    PLine = c3d_arc_3pts(pt1,pt2,pt3);
    dml_append_data(l->CurveLst, PLine);
    }
  return PLine;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Add_Entity(DXF_ENTITY e)
  {
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    dml_append_data(l->OtherLst,e);
    }
  return e;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Add_Insert(DXF_ENTITY e)
  {
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    dml_append_data(l->InsertLst,e);
    }
  return e;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_Square(double x,double y,double w,double h)
  {
  Pt_3f p;
  C3_CURVE c;

  p.Set(x,y    ,0.0); c = Add_PLine_Start(p.p());
  p.Set(x+w,y  ,0.0); Add_PLine_Vertex(c,p.p());
  p.Set(x+w,y+h,0.0); Add_PLine_Vertex(c,p.p());
  p.Set(x  ,y+h,0.0); Add_PLine_Vertex(c,p.p());
  p.Set(x  ,y  ,0.0); Add_PLine_Vertex(c,p.p());

  return c;
  }

//----------------------------------------------------------------------------

C3_CURVE LayerCollection::Add_Curve(C3_CURVE c)
  {
  CGrfLayer * l = Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    dml_insert_after(l->CurveLst, NULL, c);
  return c;
  }

//----------------------------------------------------------------------------

flag LayerCollection::TopOfList(C3_CURVE c)
  {
  CGrfLayer * l = FindLayer(c);
  if (!l)
    return False;

  while (Layers && Layers != l)
    {
    CGrfLayer * t = Layers;
    CGrfLayer * p = Layers;
    while (p && p->Nxt)
      p = p->Nxt;
    Layers = Layers->Nxt;
    p->Nxt = t;
    t->Nxt = NULL;
    }

  DML_ITEM item;
  C3_CURVE curve;
  WALK_CURVE_LIST(l->CurveLst,item, curve)
    {
    if (c==curve)
      {
      dml_remove_item(l->CurveLst, item);
      dml_insert_prior(l->CurveLst, dml_first(l->CurveLst), c);
      return True;
      }
    }
  return False;
  }

//----------------------------------------------------------------------------
// new code
flag LayerCollection::TranslateEntity(DXF_ENTITY e, Pt_3f from, Pt_3f to)
  {
  PT3 a;
  a[0] = from.x() - to.x();
  a[1] = from.y() - to.y();
  a[2] = from.z() - to.z();

  switch (DX_ENTITY_ID(e))
    {
    case DXF_POINT:
      c3v_sub(DXF_POINT_PT(e),a,DXF_POINT_PT(e));
      return 1;
      break;
    case DXF_LINE:
      c3v_sub(DXF_LINE_PT0(e),a,DXF_LINE_PT0(e));
      c3v_sub(DXF_LINE_PT1(e),a,DXF_LINE_PT1(e));
      return 1;
      break;
    case DXF_CIRCLE:
      c3v_sub(DXF_CIRCLE_CTR(e),a,DXF_CIRCLE_CTR(e));
      return 1;
      break;
    case DXF_ARC:
      c3v_sub(DXF_ARC_CTR(e),a,DXF_ARC_CTR(e));
      return 1;
      break;
    case DXF_TRACE:
      c3v_sub(DXF_TRACE_PT0(e),a,DXF_TRACE_PT0(e));
      c3v_sub(DXF_TRACE_PT1(e),a,DXF_TRACE_PT1(e));
      c3v_sub(DXF_TRACE_PT2(e),a,DXF_TRACE_PT2(e));
      c3v_sub(DXF_TRACE_PT3(e),a,DXF_TRACE_PT3(e));
      return 1;
      break;
    case DXF_SOLID:
      c3v_sub(DXF_SOLID_PT0(e),a,DXF_SOLID_PT0(e));
      c3v_sub(DXF_SOLID_PT1(e),a,DXF_SOLID_PT1(e));
      c3v_sub(DXF_SOLID_PT2(e),a,DXF_SOLID_PT2(e));
      c3v_sub(DXF_SOLID_PT3(e),a,DXF_SOLID_PT3(e));
      return 1;
      break;
    case DXF_TEXT:
      c3v_sub(DXF_TEXT_INS_PT(e),a,DXF_TEXT_INS_PT(e));
      c3v_sub(DXF_TEXT_AL_PT(e),a,DXF_TEXT_AL_PT(e));
      return 1;
      break;
    case DXF_INSERT:
      c3v_sub(DXF_INSERT_PT(e),a,DXF_INSERT_PT(e));
      DML_LIST Att;
      DML_ITEM it;

      Att = DXF_INSERT_ATTRS(e);
      DML_WALK_LIST(Att,it)
        {
        DXF_ENTITY ent = (DXF_ENTITY)DML_RECORD(it);
        TranslateEntity(ent,from,to);
        }
      return 1;
      break;
    case DXF_ATTDEF:
      c3v_sub(DXF_ATTDEF_PT(e),a,DXF_ATTDEF_PT(e));
      c3v_sub(DXF_ATTDEF_AL_PT(e),a,DXF_ATTDEF_AL_PT(e));
      return 1;
      break;
    case DXF_ATTRIB:
      c3v_sub(DXF_ATTRIB_PT(e),a,DXF_ATTRIB_PT(e));
      c3v_sub(DXF_ATTRIB_AL_PT(e),a,DXF_ATTRIB_AL_PT(e));
      return 1;
      break;
    case DXF_VERTEX:
      c3v_sub(DXF_VERTEX_PT(e),a,DXF_VERTEX_PT(e));
      return 1;
      break;

    default:
      dbgpln(" TranslateEntity unimplemented entity type = %d\n",(int)DX_ENTITY_ID(e));
      break;
    }
  return 0;
  }

//----------------------------------------------------------------------------

flag LayerCollection::TranslateEntity(C3_CURVE c, Pt_3f from, Pt_3f to)
  {
  PT3 a;
  a[0] = from.x() - to.x();
  a[1] = from.y() - to.y();
  a[2] = from.z() - to.z();
  c3c_translate(c, a);
  return 1;
  }

//----------------------------------------------------------------------------

static void MovePtForJustification(Attr_Settings Set, DXF_ENTITY e, DML_LIST style)
  {
  text_info ti;
  double dx,dy,xmin,xmax,ymin,ymax;

  text_info_from_dxf_attrib(&ti, style, e);
  scd_acad_text_corners(&ti, &xmin, &ymin, &xmax, &ymax);

  switch (Set.Hjust)
    {
    case Text_Left  : dx = 0;               break;
    case Text_Cntrd : dx = (xmax-xmin)/2.0; break;
    case Text_Right : dx = xmax;            break;
    }

  switch (Set.Vjust)
    {
    case Text_Bottom   : dy = ymin;                   break;
    case Text_Baseline : dy = 0.0;                    break;
    case Text_Middle   : dy = (ymax-ymin)/2.0 + ymin; break;
    case Text_Top      : dy = ymax;                   break;
    case Text_Align    : dy = 0.0;                    break;
    case Text_Fit      : dy = 0.0;                    break;
    }

  DXF_ATTRIB_PT(e)[0] -= dx;
  DXF_ATTRIB_PT(e)[1] -= dy;
  }

//----------------------------------------------------------------------------

static void MovePtForJustification(Text_Settings Set, DXF_ENTITY e, DML_LIST style)
  {
  text_info ti;
  double dx,dy,xmin,xmax,ymin,ymax;

  text_info_from_dxf_text(&ti, style, e);
  scd_acad_text_corners(&ti, &xmin, &ymin, &xmax, &ymax);

  switch (Set.Hjust)
    {
    case Text_Left  : dx = 0;               break;
    case Text_Cntrd : dx = (xmax-xmin)/2.0; break;
    case Text_Right : dx = xmax;            break;
    }

  switch (Set.Vjust)
    {
    case Text_Bottom   : dy = ymin;                   break;
    case Text_Baseline : dy = 0.0;                    break;
    case Text_Middle   : dy = (ymax-ymin)/2.0 + ymin; break;
    case Text_Top      : dy = ymax;                   break;
    case Text_Align    : dy = 0.0;                    break;
    case Text_Fit      : dy = 0.0;                    break;
    }

  DXF_TEXT_INS_PT(e)[0] -= dx;
  DXF_TEXT_INS_PT(e)[1] -= dy;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Add_Insert_Attrib(DXF_ENTITY Insert, pchar Tag, pchar Value,
                                              PT3 Pt, Attr_Settings &Set)
  {
  DXF_ENTITY t = Find_Attr(Insert, Tag);
  if (t==NULL)
    {
    double myscale = (Set.XScl<0.01) ? 1.0 : Set.XScl;
    if (!Value)
      Value = " ";
    t = dxf_insert_add_attrib(NULL, Insert, Pt, Set.Size, Value, Tag, Set.Flags, Set.Length, Set.Rot, myscale, "");
    MovePtForJustification(Set, t, Drawing->StyleLst);
    dxf_set_entity_color(t, -1);//DXF_COLOR_BYLAYER);
    dxf_set_entity_layer(t, CurLayer);
    }
  return t;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Create_Insert(pchar Typ, Pt_3f Pt_,int Color, Pt_3f Scl_, REAL Rot,
                                          pchar TagValue, pchar AssocTagValue, Pt_3f TagPt_, Attr_Settings &Set)
  {
  PT3 Pt,Scl,TagPt;

  c3v_set(Pt_.x(), Pt_.y(), Pt_.z(), Pt);
  c3v_set(Scl_.x(), Scl_.y(), Scl_.z(), Scl);
  c3v_set(TagPt_.x(), TagPt_.y(), TagPt_.z(), TagPt);

  DXF_ENTITY Insert = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    REAL sc1 = 1.0;
    Insert = dxf_create_insert(NULL, Typ, Pt,sc1,Rot,1,1,0.0,0.0);
    if (Insert)
      {
#if dbgdxf
      dbgp("  pt %g  %g  %g",Pt[0],Pt[1],Pt[2]);
      dbgpln("  sc %g  %g  %g",Scl[0],Scl[1],Scl[2]);
#endif
      DXF_INSERT_X_SCALE(Insert) = Scl[0];
      DXF_INSERT_Y_SCALE(Insert) = Scl[1];
      DXF_INSERT_Z_SCALE(Insert) = Scl[2];

      //      Pt_3f ptt = pt+Tagpt;
#if dbgdxf
      dbgpln("  Lay %s",DXF_LAYER_NAME_GET(l->Def));
#endif
      dxf_set_entity_layer(Insert, DXF_LAYER_NAME_GET(l->Def));
#if dbgdxf
      dbgpln("  Color %i",DXF_LAYER_COLOR(l->Def));
#endif
      dxf_set_entity_color(Insert, Color);//DXF_COLOR_BYLAYER);
#if dbgdxf
      dbgpln("  Extr");
#endif
      PT3 extr;
      c3v_set(0., 0., 1., extr);
      //      dxf_set_entity_extrusion(Insert, extr);
#if dbgdxf
      dbgpln("  XXX");
#endif

      PT3 Ptt;
      c3v_add(Pt, TagPt, Ptt);
      if( TagValue)
        {
        DXF_ENTITY t = Add_Insert_Attrib(Insert, TagAttribStr, TagValue, Ptt, Set);
        }
      if( AssocTagValue)
        {
        DXF_ENTITY a = Add_Insert_Attrib(Insert, AssocTagAttribStr, AssocTagValue, Ptt, Set);
        }
      //      20, 1.5, ptt, 0.0,
      //                                       (Show ? 0 : Attrib_InVisible), ColorTag, Text_Cntrd);
      dml_append_data(l->InsertLst, Insert);
      }
    }
#if dbgdxf
  dbgnln();
#endif
  return Insert;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Add_Text(pchar Txt, PT3 Pt, Text_Settings &Set)
  {
#if dbgdxf
  dbgpln("Add_Text %s  %s  [%s]",CurLayer, Txt);
#endif

  DXF_ENTITY Text = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    Text = dxf_create_text(NULL, Pt,Set.Size,Txt,Set.Rot,Set.XScl,Set.Style);
    MovePtForJustification(Set,Text,Drawing->StyleLst);
    if (Text)
      {
#if dbgdxf
      dbgpln("  pt %g  %g  %g",Pt[0],Pt[1],Pt[2]);
      dbgpln("  Lay %s",DXF_LAYER_NAME_GET(l->Def));
#endif
      dxf_set_entity_layer(Text, DXF_LAYER_NAME_GET(l->Def));
#if dbgdxf
      dbgpln("  Color %i",DXF_LAYER_COLOR(l->Def));
#endif
      dxf_set_entity_color(Text, Set.Color);//DXF_COLOR_BYLAYER);
      dml_append_data(l->OtherLst, Text);
      }
    }
#if dbgdxf
  dbgnln();
#endif
  return Text;
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Add_Attrib_Defn(pchar Tag, pchar Prompt, pchar Value,
                                            PT3 Pt, Attr_Settings &Set)
  {
#if dbgdxf
  //dbgfileflush(1);
  dbgpln("Add_Attrib Defn %s  %s %s %s ",CurLayer, Tag, Prompt, Value);
#endif

  DXF_ENTITY AttDef = NULL;
  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    AttDef = dxf_create_attdef(NULL, Pt,Set.Size,Value,Prompt,Tag,Set.Flags,Set.Length,Set.Rot,Set.XScl,Set.Style);
    if (AttDef)
      {
#if dbgdxf
      dbgpln("  pt %g  %g  %g",Pt[0],Pt[1],Pt[2]);
      dbgpln("  Lay %s",DXF_LAYER_NAME_GET(l->Def));
#endif
      dxf_set_entity_layer(AttDef, DXF_LAYER_NAME_GET(l->Def));
#if dbgdxf
      dbgpln("  Color %i",DXF_LAYER_COLOR(l->Def));
#endif
      dxf_set_entity_color(AttDef, -1);

      dml_append_data(l->OtherLst, AttDef);
      }
    }
#if dbgdxf
  dbgnln();
  //dbgfileflush(0);
#endif
  return AttDef;
  //  return Add_Attrib_Defn(CurLayer, Prompt, Tag, Value,
  //                         Pt, Set.Size,
  //                         Set.Flags, Set.Length, Set.Rot, Set.XScl, Set.Style);
  }

//----------------------------------------------------------------------------

DXF_ENTITY LayerCollection::Find_Attrib_Defn(pchar Tag)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    Entity_List_Info Info;
    DXF_ENTITY t = First_Ent(l->OtherLst, Info);
    while (t)
      {
      if (DXF_ENTITY_IS_ATTDEF(t))
        if (strcmp(DXF_ATTDEF_TAG_GET(t), Tag)==0)
          return t;
      t = Next_Ent(Info);
      }
    }
  return NULL;
  }

//----------------------------------------------------------------------------

CEntInView* LayerCollection::IsEntitySelected(pViewport v, DXF_ENTITY pEnt)
  {
  if (pEnt)
    {
    CEntInView* p = v->FirstSelectedEntity();
    while (p)
      {
      if (p->EntityPtr()==pEnt)
        return p;
      p = v->NextSelectedEntity();
      }
    }
  return NULL;
  }

//----------------------------------------------------------------------------

CEntInView* LayerCollection::IsCurveSelected(pViewport v, C3_CURVE c)
  {
  if (c)
    {
    CEntInView* p = v->FirstSelectedEntity();
    while (p)
      {
      if (p->CurvePtr()==c)
        return p;
      p = v->NextSelectedEntity();
      }
    }
  return NULL;
  }

//----------------------------------------------------------------------------

static void TranslatePt(PT3 p, PT3 Sc, PT3 Ec, PT3 Scl)
  {
  PT3 t1,t2;
  c3v_sub(p,Sc,t1);
  c3v_set(t1[0]*Scl[0], t1[1]*Scl[1], t1[2]*Scl[2], t2);
  c3v_add(t2,Ec,p);
  }

long LayerCollection::TranslateEntitiesInWin(pViewport vp,PT3 S1, PT3 S2, PT3 E1, PT3 E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred)
  {

  // this piece of code should ideally be used to translaye the items but
  // the CEntInView segments are not known form the list pointer. The list
  // data should be part of the entity but wiuld involve an extensive change
  // !! To be done after PID's
  //   CEntInView* p = vp->FirstSelectedEntity();
  //   if( !p )
  //       return 0;
  //   do{
  //      if( p->CurvePtr()){
  //         dbgpln(" Entity select list in translate has Curve%x\n",p->CurvePtr());
  //         translate_curve();
  //      }
  //      else if( p->EntityPtr()){
  //         dbgpln(" Entity select list in translate has Entity%x\n",p->EntityPtr());
  //         translate_entity();
  //      }
  //   }
  //   while( p = vp->NextSelectedEntity());
  // go through the list of marked points

  PT3 Mv, Sc, Ec, Scl, Sl, El;

  PT3 s1, s2;
  memcpy(s1, S1, sizeof(s1));
  memcpy(s2, S2, sizeof(s2));
  c3v_set(Min(s1[0], s2[0]), Min(s1[1], s2[1]), Min(s1[2], s2[2]), S1);
  c3v_set(Max(s1[0], s2[0]), Max(s1[1], s2[1]), Max(s1[2], s2[2]), S2);
  if (DoXScl || DoYScl || DoSpred)
    {
    PT3 e1, e2;
    memcpy(e1, E1, sizeof(e1));
    memcpy(e2, E2, sizeof(e2));
    c3v_set(Min(e1[0], e2[0]), Min(e1[1], e2[1]), Min(e1[2], e2[2]), E1);
    c3v_set(Max(e1[0], e2[0]), Max(e1[1], e2[1]), Max(e1[2], e2[2]), E2);
    }

  c3v_mid_pt(S1,S2,Sc);
  c3v_mid_pt(E1,E2,Ec);

  if (DoXScl || DoYScl || DoSpred)
    {
    c3v_sub(Ec,Sc,Mv);
    c3v_sub(S2,S1,Sl);
    c3v_sub(E2,E1,El);
    c3v_set(DoXScl ? El[0]/NZ(Sl[0]) : 1.0 ,DoYScl ? El[1]/NZ(Sl[1]) : 1.0, 1.0, Scl);
    }
  else
    {
    c3v_sub(E2,E1,Mv);
    c3v_add(S1,Mv,E1);
    c3v_add(S2,Mv,E2);
    c3v_mid_pt(E1,E2,Ec);
    c3v_set(1.0,1.0,1.0,Scl);
    }
  if (!DoX) Ec[0]=Sc[0];
  if (!DoY) Ec[1]=Sc[1];
  Ec[2]=Sc[2];

  dbgp("Move Entities %g %g %g",Mv[0],Mv[1],Mv[2]);
  dbgpln("  Scale %g %g %g",Scl[0],Scl[1],Scl[2]);
  dbgpln("  S1 %g %g %g",S1[0],S1[1],S1[2]);
  dbgpln("  S2 %g %g %g",S2[0],S2[1],S2[2]);
  dbgpln("  E1 %g %g %g",E1[0],E1[1],E1[2]);
  dbgpln("  E2 %g %g %g",E2[0],E2[1],E2[2]);

  long Cnt=0;
  CGrfLayer * l;
  WALK_LAYERS(Layers,NULL, l)
    {
    DML_ITEM it;
    DXF_ENTITY e;
    WALK_ENTITY_LIST(l->InsertLst, it, e)
      {
      if (DXF_INSERT_PT(e)[0] >= S1[0] && DXF_INSERT_PT(e)[0] <= S2[0] &&
        DXF_INSERT_PT(e)[1] >= S1[1] && DXF_INSERT_PT(e)[1] <= S2[1])
        {
        dbgpln("Move Entity %s",DXF_INSERT_BLOCK_GET(e));
        Cnt++;
        TranslatePt(DXF_INSERT_PT(e), Sc, Ec, Scl);
        DXF_INSERT_X_SCALE(e) *= Scl[0];
        DXF_INSERT_Y_SCALE(e) *= Scl[1];
        DXF_INSERT_Z_SCALE(e) *= Scl[2];

        DML_LIST l1 = DXF_INSERT_ATTRS(e);
        DML_ITEM it1;
        DXF_ENTITY e1;
        WALK_ENTITY_LIST(l1, it1, e1)
          {
          TranslatePt(DXF_ATTRIB_PT(e1), Sc, Ec, Scl);
          DXF_ATTRIB_X_SCALE(e1) *= Scl[0];
          }
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }

    C3_CURVE c, nc=NULL, oc=NULL;
    DML_ITEM oit = NULL/*mhmchange*/;
    WALK_CURVE_LIST(l->CurveLst,it, c)
      {
      if (nc && oit/*mhmchange*/)
        {
        dml_insert_prior(l->CurveLst, oit, nc);
        oit->pKenwalta = ((CEntInView*)(oit->pKenwalta))->EntityInvalidate();
        dml_remove_item(l->CurveLst, oit);
        c3d_free_curve(oc);
        nc=NULL;
        }

      int seg;
      REAL rad, sweep;
      PT3 ctr, ept1, normal;
      flag move, more, is_arc;

      move=0;
      more=1;
      c3c_ept0(c,ept1);
      seg=0;
      while (more)
        {
        move |= (ept1[0] >= S1[0] && ept1[0] <= S2[0] && ept1[1] >= S1[1] && ept1[1] <= S2[1]);
        more=c3c_get_pcurve_data(c,seg,&is_arc,ctr,ept1,&rad,normal,&sweep);
        seg++;
        }

      if (move)
        {
        oc=c;
        oit=it;
        dbgpln("Move Curve");
        more=1;

        c3c_ept0(c,ept1);
        seg=0;
        while (more)
          {
          dbgpln("  seg %i] %g,%g,%g",seg,ept1[0],ept1[1],ept1[2]);
          if (ept1[0] >= S1[0] && ept1[0] <= S2[0] && ept1[1] >= S1[1] && ept1[1] <= S2[1])
            TranslatePt(ept1, Sc, Ec, Scl);
          if (seg==0)
            nc=c3d_pcurve_init(ept1);
          else if (is_arc)
            {
            dbgpln("  ARC %i]",seg);
            }
          else
            {
            c3d_pcurve_add_line(nc,ept1);
            }

          more=c3c_get_pcurve_data(c,seg,&is_arc,ctr,ept1,&rad,normal,&sweep);
          seg++;
          }
        }
      }
    if (nc)
      {
      dml_insert_prior(l->CurveLst, oit, nc);
      oit->pKenwalta = ((CEntInView*)(oit->pKenwalta))->EntityInvalidate();
      dml_remove_item(l->CurveLst, oit);
      c3d_free_curve(oc);
      nc=NULL;
      }

    }
  return Cnt;
  }

//KGA: 30/10/96 New code for moving selected entities...
long LayerCollection::TranslateEntitiesInList(pViewport vp,PT3 S1, PT3 S2, PT3 E1, PT3 E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred)
  {
  bool doDbg=01; 
  if (doDbg)
    dbgpln(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

  // this piece of code should ideally be used to translaye the items but
  // the CEntInView segments are not known form the list pointer. The list
  // data should be part of the entity but wiuld involve an extensive change
  // !! To be done after PID's
  //   CEntInView* p = vp->FirstSelectedEntity();
  //   if( !p )
  //       return 0;
  //   do{
  //      if( p->CurvePtr()){
  //         dbgpln(" Entity select list in translate has Curve%x\n",p->CurvePtr());
  //         translate_curve();
  //      }
  //      else if( p->EntityPtr()){
  //         dbgpln(" Entity select list in translate has Entity%x\n",p->EntityPtr());
  //         translate_entity();
  //      }
  //   }
  //   while( p = vp->NextSelectedEntity());
  // go through the list of marked points

  PT3 Mv, Sc, Ec, Scl, Sl, El;

  PT3 s1, s2;
  memcpy(s1, S1, sizeof(s1));
  memcpy(s2, S2, sizeof(s2));
  c3v_set(Min(s1[0], s2[0]), Min(s1[1], s2[1]), Min(s1[2], s2[2]), S1);
  c3v_set(Max(s1[0], s2[0]), Max(s1[1], s2[1]), Max(s1[2], s2[2]), S2);
  if (DoXScl || DoYScl || DoSpred)
    {
    PT3 e1, e2;
    memcpy(e1, E1, sizeof(e1));
    memcpy(e2, E2, sizeof(e2));
    c3v_set(Min(e1[0], e2[0]), Min(e1[1], e2[1]), Min(e1[2], e2[2]), E1);
    c3v_set(Max(e1[0], e2[0]), Max(e1[1], e2[1]), Max(e1[2], e2[2]), E2);
    }

  c3v_mid_pt(S1,S2,Sc);
  c3v_mid_pt(E1,E2,Ec);

  if (DoXScl || DoYScl || DoSpred)
    {
    c3v_sub(Ec,Sc,Mv);
    c3v_sub(S2,S1,Sl);
    c3v_sub(E2,E1,El);
    c3v_set(DoXScl && TstNZ(Sl[0]) ? El[0]/NZ(Sl[0]) : 1.0 ,DoYScl && TstNZ(Sl[1]) ? El[1]/NZ(Sl[1]) : 1.0, 1.0, Scl);
    }
  else
    {
    c3v_sub(E2,E1,Mv);
    c3v_add(S1,Mv,E1);
    c3v_add(S2,Mv,E2);
    c3v_mid_pt(E1,E2,Ec);
    c3v_set(1.0,1.0,1.0,Scl);
    }
  if (!DoX) Ec[0]=Sc[0];
  if (!DoY) Ec[1]=Sc[1];
  Ec[2]=Sc[2];

  if (doDbg)
    {
    dbgp("Move Entities %g %g %g",Mv[0],Mv[1],Mv[2]);
    dbgpln("  Scale %g %g %g",Scl[0],Scl[1],Scl[2]);
    dbgpln("  S1 %g %g %g",S1[0],S1[1],S1[2]);
    dbgpln("  S2 %g %g %g",S2[0],S2[1],S2[2]);
    dbgpln("  E1 %g %g %g",E1[0],E1[1],E1[2]);
    dbgpln("  E2 %g %g %g",E2[0],E2[1],E2[2]);
    }

  long Cnt=0;
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    //translate selected entities...
    DML_ITEM it;
    DXF_ENTITY e;
    WALK_ENTITY_LIST(l->InsertLst, it, e)
      {
      if (IsEntitySelected(vp, e))
        {
        if (doDbg)
          dbgpln("Move Entity %s", DXF_INSERT_BLOCK_GET(e));
        Cnt++;
        TranslatePt(DXF_INSERT_PT(e), Sc, Ec, Scl);
        DXF_INSERT_X_SCALE(e) *= Scl[0];
        DXF_INSERT_Y_SCALE(e) *= Scl[1];
        DXF_INSERT_Z_SCALE(e) *= Scl[2];

        DML_LIST l1 = DXF_INSERT_ATTRS(e);
        if (l1)
          {
          DML_ITEM it1;
          DXF_ENTITY e1;
          WALK_ENTITY_LIST(l1, it1, e1)
            {
            TranslatePt(DXF_ATTRIB_PT(e1), Sc, Ec, Scl);
            DXF_ATTRIB_X_SCALE(e1) *= Scl[0];
            }
          }
        it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        }
      }

    //TODO: Move & scale all entity types in OtherLst!! For now translate text..
    WALK_ENTITY_LIST(l->OtherLst, it, e)
      {
      if (IsEntitySelected(vp, e))
        {
        //TranslateEntity(e, ?, ?)
        if (DXF_ENTITY_IS_TEXT(e))
          {
          if (doDbg)
            dbgpln("Move Entity TEXT");
          Cnt++;
          TranslatePt(DXF_TEXT_INS_PT(e), Sc, Ec, Scl);
          TranslatePt(DXF_TEXT_AL_PT(e), Sc, Ec, Scl);
          if( it->pKenwalta )
            it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
          }
        else
          {
          //TODO?
          }
        }
      }

    //translate selected curves...
    C3_CURVE nc=NULL, oc=NULL;
    C3_CURVE c;
    DML_ITEM oit = NULL;
    if (doDbg)
      dbgpln("Move CurveList %08x %08x", l->CurveLst, l);
    WALK_CURVE_LIST(l->CurveLst, it, c)
      {
      if (nc && oit)
        {
        if (doDbg)
          dbgp("XXXX ");
        dml_insert_prior(l->CurveLst, oit, nc);
        oit->pKenwalta = ((CEntInView*)(oit->pKenwalta))->EntityInvalidate();
        dml_remove_item(l->CurveLst, oit);
        c3d_free_curve(oc);
        nc = NULL;
        }

      if (doDbg)
        dbgp("Try %08x ", c);
      if (IsCurveSelected(vp, c))
        {
        if (doDbg)
          dbgp(" Move Curve %i", c->curve_type);
        oc = c;
        oit = it;
        if (C3_CURVE_IS_LINE(c))
          {//this will now be a polyline
          if (doDbg)
            dbgp("MkPoly ");
          PT3 ept0,ept1;
          ept0[0] = C3_CURVE_LINE(c)->pt[0][0]; ept0[1] = C3_CURVE_LINE(c)->pt[0][1]; ept0[2] = C3_CURVE_LINE(c)->pt[0][2];
          ept1[0] = C3_CURVE_LINE(c)->pt[1][0]; ept1[1] = C3_CURVE_LINE(c)->pt[1][1]; ept1[2] = C3_CURVE_LINE(c)->pt[1][2];
          TranslatePt(ept0, Sc, Ec, Scl);
          TranslatePt(ept1, Sc, Ec, Scl);
          if (1) // cnm - just move the line ???
            {
            C3_CURVE_LINE(c)->pt[0][0] = ept0[0]; C3_CURVE_LINE(c)->pt[0][1] = ept0[1]; C3_CURVE_LINE(c)->pt[0][2] = ept0[2];
            C3_CURVE_LINE(c)->pt[1][0] = ept1[0]; C3_CURVE_LINE(c)->pt[1][1] = ept1[1]; C3_CURVE_LINE(c)->pt[1][2] = ept1[2];
            if( it->pKenwalta )
              it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
            }
          else // why did we go through this ???
            {
            nc = c3d_pcurve_init(ept0);
            c3d_pcurve_add_line(nc, ept1);
            }
          }
        else
          {
          if (doDbg)
            dbgp("DoPoly ");
          REAL rad, sweep;
          PT3 ctr, ept1, normal;
          flag is_arc;
          c3c_ept0(c, ept1);
          flag more = 1;
          int seg = 0;
          while (more)
            {
            //dbgpln("  seg %i] %g,%g,%g", seg, ept1[0], ept1[1], ept1[2]);
            TranslatePt(ept1, Sc, Ec, Scl);
            if (seg==0)
              nc = c3d_pcurve_init(ept1);
            else if (is_arc)
              dbgpln("  ARC %i]", seg);
            else
              c3d_pcurve_add_line(nc, ept1);
            more = c3c_get_pcurve_data(c, seg, &is_arc, ctr, ept1, &rad, normal, &sweep);
            seg++;
            }
          }
        }
      if (doDbg)
        dbgpln("");
      }

    if (nc)
      {
      if (doDbg)
        dbgpln("ZZZZ ");
      dml_insert_prior(l->CurveLst, oit, nc);
      oit->pKenwalta = ((CEntInView*)(oit->pKenwalta))->EntityInvalidate();
      dml_remove_item(l->CurveLst, oit);
      c3d_free_curve(oc);
      nc = NULL;
      }

    }
  if (doDbg)
    dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  return Cnt;
  }

//----------------------------------------------------------------------------

flag LayerCollection::Delete(DXF_ENTITY e)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    DML_ITEM it;
    DXF_ENTITY ei;

    WALK_ENTITY_LIST(l->InsertLst, it, ei)
      {
      if (ei==e)
        {
        if( it->pKenwalta )
          {
          it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
          }
        dml_remove_item(l->InsertLst, it);
        dxf_free_entity(ei);
        return 1;
        }
      }

    WALK_ENTITY_LIST(l->OtherLst, it, ei)
      {
      if (ei==e)
        {
        if( it->pKenwalta )
          {
          it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
          }
        dml_remove_item(l->OtherLst, it);
        dxf_free_entity(ei);
        return 1;
        }
      }
    }
  /*
  old code - seemed to be causing a problem
  for (int iLst=0; iLst<3; iLst++)
  {
  DML_LIST pLst;
  switch (iLst)
  {
  case 0: pLst=l->InsertLst; break;
  case 1: pLst=l->CurveLst; break;
  case 2: pLst=l->OtherLst; break;
  }

  WALK_ENTITY_LIST(pLst, it, ei)
  if (ei==e)
  {
  if (((CEntInView*) (it->pKenwalta)))
  {
  if( it->pKenwalta )
  it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
  }
  dml_remove_item(l->InsertLst, it);
  dxf_free_entity(ei);
  return 1;
  }
  }
  }
  */
  return 0;
  }

//----------------------------------------------------------------------------

CEntInView* LayerCollection::FindEntInView(DXF_ENTITY e)
  {
  DML_ITEM x;
  if( x = FindItem(e))
    return (CEntInView*)(x->pKenwalta);
  return NULL;
  }

CEntInView* LayerCollection::FindEntInView(C3_CURVE c)
  {
  DML_ITEM x;
  if( x = FindItem(c))
    return (CEntInView*)(x->pKenwalta);
  return NULL;
  }

DML_ITEM LayerCollection::FindItem(DXF_ENTITY e)
  {
  DML_ITEM it;
  DXF_ENTITY ei;
  CGrfLayer * l;

  WALK_LAYERS(Layers,NULL, l)
    {
    WALK_ENTITY_LIST(l->InsertLst, it, ei)
      if (ei==e)
        return it;
    WALK_ENTITY_LIST(l->OtherLst, it, ei)
      if (ei==e)
        return it;
    }
  return NULL;
  }

//----------------------------------------------------------------------------

DML_ITEM LayerCollection::FindItem(C3_CURVE c)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers,NULL, l)
    {
    DML_ITEM it;
    C3_CURVE ci;
    WALK_CURVE_LIST(l->CurveLst, it, ci)
      if (ci==c)
        return it;
    }
  return NULL;
  }

//----------------------------------------------------------------------------

CGrfLayer * LayerCollection::FindLayer(DXF_ENTITY e)
  {
  DML_ITEM it;
  DXF_ENTITY ei;
  CGrfLayer * l;

  WALK_LAYERS(Layers,NULL, l)
    {
    WALK_ENTITY_LIST(l->InsertLst, it, ei)
      if (ei==e)
        return l;
    WALK_ENTITY_LIST(l->OtherLst, it, ei)
      if (ei==e)
        return l;
    }
  return NULL;
  }

//----------------------------------------------------------------------------

CGrfLayer * LayerCollection::FindLayer(C3_CURVE c)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers,NULL, l)
    {
    DML_ITEM it;
    C3_CURVE ci;
    WALK_CURVE_LIST(l->CurveLst, it, ci)
      if (ci==c)
        return l;
    }
  return NULL;
  }

//----------------------------------------------------------------------------

void LayerCollection::ChangeLayer(DXF_ENTITY e,LayerTypes NewLayer)
  {
  RemoveFromLayer(e);
  CGrfLayer * l = Find(LayerNames[NewLayer]);
  if (!l)
    l = Append(Drawing->FindLayerDef(LayerNames[NewLayer]));
  ASSERT(l);
  l->Add(e);
  }

//----------------------------------------------------------------------------

void LayerCollection::ChangeLayer(C3_CURVE c,LayerTypes NewLayer)
  {
  RemoveFromLayer(c);

  CGrfLayer * l = Find(LayerNames[NewLayer]);
  if (!l)
    l = Append(Drawing->FindLayerDef(LayerNames[NewLayer]));
  ASSERT(l);
  l->Add(c);
  }

//----------------------------------------------------------------------------

flag LayerCollection::Delete(C3_CURVE c)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    DML_ITEM it;
    C3_CURVE ci;
    WALK_CURVE_LIST(l->CurveLst,it, ci)
      if (ci==c)
        {
        if((CEntInView*)(it->pKenwalta))
          {
          it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
          }
        dml_remove_item(l->CurveLst, it);
        c3d_free_curve(ci);
        return 1;
        }
    }
  return 0;
  }

//----------------------------------------------------------------------------

flag LayerCollection::RemoveFromLayer(DXF_ENTITY e)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers,NULL,l)
    {
    DML_ITEM it;
    DXF_ENTITY ei;
    WALK_ENTITY_LIST(l->InsertLst, it, ei)
      if (ei==e)
        {
        if (it->pKenwalta)
          it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        dml_remove_item(l->InsertLst, it);
        return 1;
        }
      WALK_ENTITY_LIST(l->OtherLst, it, ei)
        if (ei==e)
          {
          if (it->pKenwalta)
            it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
          dml_remove_item(l->OtherLst, it);
          return 1;
          }
    }
  return 0;
  }

//----------------------------------------------------------------------------

flag LayerCollection::RemoveFromLayer(C3_CURVE c)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers,NULL,l)
    {
    DML_ITEM it;
    C3_CURVE ci;
    WALK_CURVE_LIST(l->CurveLst,it, ci)
      if (ci == c)
        {
        if (it->pKenwalta)
          it->pKenwalta = ((CEntInView*)(it->pKenwalta))->EntityInvalidate();
        dml_remove_item(l->CurveLst, it);
        return 1;
        }
    }
  return 0;
  }

//----------------------------------------------------------------------------

flag LayerCollection::Exists(DXF_ENTITY e)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers,NULL, l)
    {
    DML_ITEM it;
    DXF_ENTITY ei;
    WALK_ENTITY_LIST(l->InsertLst, it, ei)
      if (ei==e)
        return 1;
    WALK_ENTITY_LIST(l->OtherLst, it, ei)
      if (ei==e)
        return 1;
    }
  return 0;
  }

//----------------------------------------------------------------------------

flag LayerCollection::Exists(C3_CURVE c)
  {
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    DML_ITEM it;
    C3_CURVE ci;
    WALK_CURVE_LIST(l->CurveLst,it, ci)
      if (ci==c)
        return 1;
    }
  return 0;
  }

//============================================================================

Block::Block(pDXF_Drawing DrawingIn) : LayerCollection(DrawingIn, this)
  {
  Nxt = NULL;
  Def = NULL;
  Drawing = DrawingIn;
  IsNew = 0;
  //IsScd = 0;
  SaveIt = 1;
  SaveSym = 1;
  }

//----------------------------------------------------------------------------

Block::~Block()
  {
  dxf_free_block(Def);
  //mhm delete Def ???comeback
  }

//----------------------------------------------------------------------------

void Block::Get(DXF_FILE file, int What,int test_mode)
  {
  LayerCollection::Get(file, What, test_mode);
  }

//----------------------------------------------------------------------------

void Block::Put(DXF_FILE file)
  {
  LayerCollection::Put(file);
  }

//----------------------------------------------------------------------------

void Block::CountBlocksUsed(pDXF_Drawing pDrawing, CWordArray& BlkCnt)
  {
  int index;
  pBlock b;
  DML_ITEM it;
  DXF_ENTITY e;
  CGrfLayer * l;
  WALK_LAYERS(Layers, NULL, l)
    {
    WALK_ENTITY_LIST(l->InsertLst, it, e)
      if (DXF_ENTITY_IS_INSERT(e))
        {
        b = pDrawing->Blocks.Find(DXF_INSERT_BLOCK_GET(e), index);
        if (b)
          {
          if (BlkCnt[index]==0)
            b->CountBlocksUsed(pDrawing, BlkCnt);
          BlkCnt[index]++;
          }
        }
      WALK_ENTITY_LIST(l->OtherLst, it, e)
        if (DXF_ENTITY_IS_INSERT(e))
          {
          b = pDrawing->Blocks.Find(DXF_INSERT_BLOCK_GET(e), index);
          if (b)
            {
            if (BlkCnt[index]==0)
              b->CountBlocksUsed(pDrawing, BlkCnt);
            BlkCnt[index]++;
            }
          }
    }
  }

//============================================================================

BlockCollection::BlockCollection(pDXF_Drawing DrawingIn)
  {
  Blocks = NULL;
  Drawing = DrawingIn;
  }

//----------------------------------------------------------------------------

BlockCollection::~BlockCollection()
  {
  while (Blocks)
    {
    pBlock Nxt = Blocks->Nxt;
    delete Blocks;
    Blocks = Nxt;
    }
  }

//----------------------------------------------------------------------------

int BlockCollection::Count()
  {
  int Cnt = 0;
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    Cnt++;
    }
  return Cnt;
  }

//----------------------------------------------------------------------------

pBlock BlockCollection::Find(pchar BlkName)
  {
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    char* p = DXF_BLOCK_NAME_GET(b->Def);
    if (_stricmp(BlkName, p) == 0)
      return b;
    }
  return (pBlock)NULL;
  }

//----------------------------------------------------------------------------

pBlock BlockCollection::Find(pchar BlkName, int& index)
  {
  index = 0;
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    char* p = DXF_BLOCK_NAME_GET(b->Def);
    if (_stricmp(BlkName, p) == 0)
      return b;
    index++;
    }
  index = -1;
  return (pBlock)NULL;
  }
//----------------------------------------------------------------------------

pBlock BlockCollection::FindByIndex(int index)
  {
  int i=0;
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    if (i==index)
      return b;
    i++;
    }
  return (pBlock)NULL;
  }

// ---------------------------------------------------------------------------

flag DXF_Drawing::DivideCurve(C3_CURVE c1,C3_CURVE c2,C3_CURVE &cn1,C3_CURVE &cn2,double tol)
  {
  PARM parm_1,parm_2;
  int ret=0;
  C3_CURVE c3_1=c1;
  C3_CURVE c3_2=c2;

  if (c3_1 && c3_2)
    {
    PT3 Org, XAx, YAx;
    c3v_set(0., 0., 0., Org);
    c3v_set(1., 0., 0., XAx);
    c3v_set(0., 1., 0., YAx);

    // Check form intersection of curves
    C2_CURVE c2_1=c3d_convert_3d2d(c3_1, Org, XAx, YAx);
    C2_CURVE c2_2=c3d_convert_3d2d(c3_2, Org, XAx, YAx);
    DML_LIST IntList=dml_create_list();
    C2_INT_REC int_rec;
    DML_ITEM item;
    int nInts=0;
    if (c2c_intersect_ext(c2_1, c2_2, IntList)!=0)
      {
      double Dist=1.0e30;
      DML_FOR_LOOP(dml_first(IntList), item)
        {
        int_rec=(C2_INT_REC)dml_record(item);
        memcpy(&parm_1, C2_INT_REC_PARM1(int_rec), sizeof(parm_1));
        memcpy(&parm_2, C2_INT_REC_PARM2(int_rec), sizeof(parm_2));
        c2d_free_int_rec(int_rec);
        }

      //Got the point parm_1 on the 3d curve now split the curves
      cn1 = c3d_trim(c1, 0, parm_1);
      cn2 = c3d_trim(c1, parm_1, 0);
      return True;
      }
    }
  return False;
  }

// ---------------------------------------------------------------------------

flag DXF_Drawing::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  //change tag for models...
  flag TagChngd = 0;
  CGrfLayer * pLay = Layers;
  while (pLay)
    {
    TagChngd |= pLay->ChangeTag(pOldTag, pNewTag);
    pLay = pLay->Nxt;
    }

  return TagChngd;
  }

// ---------------------------------------------------------------------------

void DXF_Drawing::CollectTags(Strng_List & TagList)
  {
  CGrfLayer * pLay = Layers;
  while (pLay)
    {
    pLay->CollectTags(TagList);
    pLay = pLay->Nxt;
    }
  }

// ---------------------------------------------------------------------------

flag DXF_Drawing::FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr)
  {
  CGrfLayer * pLay = Layers;
  while (pLay)
    {
    if (pLay->FindTag(Tag, pLastMatchingInsert, pLastMatchingAttr))
      return true;
    pLay = pLay->Nxt;
    }
  return NULL;
  }

// ---------------------------------------------------------------------------

flag DXF_Drawing::UpdateModelAssocGraphics(CMdlAssocGraphicList & Grfs, CMdlAssocGraphicMap & Map)
  {
  CList<CMdlAssocGraphicX,CMdlAssocGraphicX&> GrfsX;
  //change tag for models...
  
  int WorkReqd = 0;
  CGrfLayer * pLay = Layers;
  while (pLay)
    {
    WorkReqd |= pLay->UpdateModelAssocGraphics(Grfs, Map, GrfsX);
    pLay = pLay->Nxt;
    }

  pDsp->Open();

  POSITION Pos=GrfsX.GetHeadPosition();
  while (Pos)
    {
    CMdlAssocGraphicX & GX = GrfsX.GetNext(Pos);
    CMdlAssocGraphic  & G = *GX.m_pAssGrf;
    switch (GX.m_eAction)
      {
      case CMdlAssocGraphicX::eAddNdAssocGrfs:
        {
        for (int i=0; i<G.m_AssocGrfs.GetSize(); i++)
          {
          CMdlAssocGraphic::CItem & I = G.m_AssocGrfs[i];
          if (!I.m_bExists && I.m_bShowIt)
            {
            PT3 pt;
            pt[0] = GX.m_NdX;
            pt[1] = GX.m_NdY;
            pt[2] = 0.0;

            DXF_ENTITY Ins = Add_AssocGrf(I.m_eAGType, pt, (LPSTR)(LPCSTR)G.m_sNdTag, (LPSTR)(LPCSTR)I.m_sTag, (DXF_ENTITY)GX.m_Entity);
            pDsp->Draw(Ins, -1);
            }
          else if (I.m_bExists && !I.m_bShowIt)
            {
            // Will Delete below;
            }
          }
        break;
        }
      case CMdlAssocGraphicX::eDelete:
        {
        pDsp->Draw(GX.m_Entity, GrfHelper.GR_BACKGROUND);
        Drawing->Delete(GX.m_Entity);
        break;
        }
      }
    }

  pDsp->Close();

  return WorkReqd;
  }

// ---------------------------------------------------------------------------

void DXF_Drawing::CollectTagInfo(bool AllInserts, CGrfTagInfoArray & GTIA)
  {
  CGrfLayer * pLay = Layers;
  while (pLay)
    {
    pLay->CollectTagInfo(this, AllInserts, GTIA);
    pLay = pLay->Nxt;
    }
  }

// ---------------------------------------------------------------------------

bool IgnoreArrows = true;

bool IsArrow(C3_CURVE c, double & Scale)
  {
  if (c && C3_CURVE_IS_PCURVE(c))
    {
    int SegCnt=0;
    C3_CURVE c3;
    while (1)
      {
      c3 = c3d_pcurve_segment(c, SegCnt);
      if (c3==NULL)
        break;
      SegCnt++;
      if (SegCnt>6)
        break;
      }
    if (SegCnt==6)
      {
      C3_CURVE c0,c2,c5;
      c0 = c3d_pcurve_segment(c, 0);
      c2 = c3d_pcurve_segment(c, 2);
      c5 = c3d_pcurve_segment(c, 5);
      const double x0 = C3_CURVE_LINE(c0)->pt[0][0];
      const double y0 = C3_CURVE_LINE(c0)->pt[0][1];
      const double x3 = C3_CURVE_LINE(c2)->pt[1][0];
      const double y3 = C3_CURVE_LINE(c2)->pt[1][1];
      const double x6 = C3_CURVE_LINE(c5)->pt[1][0];
      const double y6 = C3_CURVE_LINE(c5)->pt[1][1];
      //if arrow (x0,y0) = (x3,y3) = (x6,y6) ie identical points
      if (fabs(x0-x3)>1.0e-3 || fabs(y0-y3)>1.0e-3)
        return false;
      if (fabs(x0-x6)>1.0e-3 || fabs(y0-y6)>1.0e-3)
        return false;
      if (fabs(x3-x6)>1.0e-3 || fabs(y3-y6)>1.0e-3)
        return false;
      const double x1 = C3_CURVE_LINE(c0)->pt[1][0];
      const double y1 = C3_CURVE_LINE(c0)->pt[1][1];
      const double x2 = C3_CURVE_LINE(c2)->pt[0][0];
      const double y2 = C3_CURVE_LINE(c2)->pt[0][1];
      const double d0 = HYPOT(x1-x0, y1-y0);
      const double d2 = HYPOT(x2-x0, y2-y0);
      //if arrow sides 0 and 2 should have same length
      if (fabs(d0-d2)>1.0e-3)
        return false;
      Scale = d0 / HYPOT(ArrowLeftSz, ArrowUpSz);
      return true;
      }
    }
  return false;
  }
/*bool IsArrow(int SegCnt, C3_CURVE c)
{
if (SegCnt==6)
{
C3_CURVE c3;
c3 = c3d_pcurve_segment(c, 0);
if (C3_CURVE_LINE(c3)->pt[0][0]!=0.0 || C3_CURVE_LINE(c3)->pt[0][1]!=0.0)
return false;
c3 = c3d_pcurve_segment(c, 2);
if (C3_CURVE_LINE(c3)->pt[1][0]!=0.0 || C3_CURVE_LINE(c3)->pt[1][1]!=0.0)
return false;
c3 = c3d_pcurve_segment(c, 5);
if (C3_CURVE_LINE(c3)->pt[1][0]!=0.0 || C3_CURVE_LINE(c3)->pt[1][1]!=0.0)
return false;
return true;
}
return false;
}*/

static void CopyPT3(PT3 & A, PT3 & B)
  {
  A[0]=B[0];
  A[1]=B[1];
  A[2]=B[2];
  }
static void SwapPT3(PT3 & A, PT3 & B)
  {
  double q;
  q=A[0]; A[0]=B[0]; B[0]=q;
  q=A[1]; A[1]=B[1]; B[1]=q;
  q=A[2]; A[2]=B[2]; B[2]=q;
  }

void DXF_Drawing::CollectLinkInfo(CGrfTagInfo & GTI, CLinePointsArray & LPA)
  {
  pBlock b = Blocks.Find(DXF_INSERT_BLOCK_GET(GTI.e));
  if (b)
    {
    C3_TRANSFORM c3t;
    ConstructInsertMatrix(c3t, b, GTI.e);

    GTI.PointsCount = 0;

    CGrfLayer * l;
    WALK_LAYERS(b->Layers, NULL, l)
      {
      DML_ITEM it;
      C3_CURVE c;
      struct CLine
        { 
        PT3 St, En; 
        int jStAtt;
        int jEnAtt;
        int jStNxt;
        int jEnNxt;
        double StDist;
        double EnDist;
        bool Done;
        }  Lines[100];
      PT3  Pts[200];
      struct CSeg
        {
        int iStPt, iEnPt;
        int iStConn, iEnConn;
        } Segs[100];
      int nPts=0;
      int nSegs=1;

      int nLines=0;
      long SeqOffset = (GTI.PointsCount>0 ? LPA[GTI.PointsCount-1].seq+2 : 0);
      long index_offset = GTI.PointsCount;


#if dbgLineExport 
      dbgpln("%s", GTI.m_sTag());
#endif

      WALK_CURVE_LIST(l->CurveLst, it, c)
        {
        if (C3_CURVE_IS_LINE(c))
          {
          if (nLines==0)
            nLines=1;

          C3_CURVE c3 = c3d_copy(c);
          c3c_transform(c3, c3t);
          CopyPT3(Lines[nLines].St, C3_CURVE_LINE(c3)->pt[0]);
          CopyPT3(Lines[nLines].En, C3_CURVE_LINE(c3)->pt[1]);

#if dbgLineExport 
          dbgpln("%3i", nLines);
          dbgpln("  %10.3f %10.3f  %10.3f %10.3f ", C3_CURVE_LINE(c)->pt[0][0], C3_CURVE_LINE(c)->pt[0][1], Lines[nLines].St[0], Lines[nLines].St[1]);
          dbgpln("  %10.3f %10.3f  %10.3f %10.3f ", C3_CURVE_LINE(c)->pt[1][0], C3_CURVE_LINE(c)->pt[1][1], Lines[nLines].En[0], Lines[nLines].En[1]);
#endif

          nLines++;

          for (int i=1; i<nLines; i++)
            {
            CLine & L=Lines[i];
            // Fnd Lines Attached to i
            L.jStAtt=0;
            L.jEnAtt=0;
            L.jStNxt=0;
            L.jEnNxt=0;
            L.StDist=1e10;
            L.EnDist=1e10;
            double BigDist=1;
            for (int j=1; j<nLines; j++)
              {
              if (i!=j)
                {
                double StDs=Sqrt(Sqr(Lines[j].St[0]-L.St[0])+Sqr(Lines[j].St[1]-L.St[1]));
                double StDe=Sqrt(Sqr(Lines[j].En[0]-L.St[0])+Sqr(Lines[j].En[1]-L.St[1]));
                BigDist=Max(BigDist, Max(StDs, StDe));
                if (StDs<L.StDist)
                  {
                  L.jStAtt=j;
                  L.StDist=StDs;
                  }
                if (StDe<L.StDist)
                  {
                  L.jStAtt=-j;
                  L.StDist=StDe;
                  }

                double EnDs=Sqrt(Sqr(Lines[j].St[0]-L.En[0])+Sqr(Lines[j].St[1]-L.En[1]));
                double EnDe=Sqrt(Sqr(Lines[j].En[0]-L.En[0])+Sqr(Lines[j].En[1]-L.En[1]));
                BigDist=Max(BigDist, Max(EnDs, EnDe));
                if (EnDs<L.EnDist)
                  {
                  L.jEnAtt=j;
                  L.EnDist=EnDs;
                  }
                if (EnDe<L.EnDist)
                  {
                  L.jEnAtt=-j;
                  L.EnDist=EnDe;
                  }
                }
              }
            
            if (L.StDist>0.001*BigDist)
              {
              L.jStNxt=L.jStAtt;
              L.jStAtt=0;
              }
            if (L.EnDist>0.001*BigDist)
              {
              L.jEnNxt=L.jEnAtt;
              L.jEnAtt=0;
              }

#if dbgLineExport 
            dbgpln("                           %3i : %2i %2i %10.3f   %2i %2i %10.3f ", i, L.jStAtt, L.jStNxt, L.StDist, L.jEnAtt, L.jEnNxt, L.EnDist);
#endif
            }

          // Group Segments
          for (int i=1; i<nLines; i++)
            Lines[i].Done=false;

          nPts=0;
          nSegs=1;
          for (int i=1; i<nLines; i++)
            {
            CLine & L=Lines[i];
            if (L.Done)
              continue;
            if (L.jStAtt==0 && L.jEnAtt==0)
              {
              L.Done=true;
              Segs[nSegs].iStPt=nPts;
              CopyPT3(Pts[nPts], L.St);
              CopyPT3(Pts[nPts+1], L.En);
              nPts+=2;
              Segs[nSegs].iEnPt=nPts-1;
              nSegs+=1;
              }
            else if (L.jStAtt==0 || L.jEnAtt==0)
              {
              L.Done=true;
              int j;
              Segs[nSegs].iStPt=nPts;
              if (L.jStAtt==0)
                {
                CopyPT3(Pts[nPts], L.St);
                CopyPT3(Pts[nPts+1], L.En);
                j=L.jEnAtt;
                }
              else 
                {
                CopyPT3(Pts[nPts], L.En);
                CopyPT3(Pts[nPts+1], L.St);
                j=L.jStAtt;
                }
              nPts+=2;
              
              int iFrom=i;
              while (j!=0)
                {
                CLine & Lj=Lines[abs(j)];
                Lj.Done=true;
                if (abs(Lj.jStAtt)==iFrom)
                  {
                  CopyPT3(Pts[nPts], Lj.En);
                  iFrom=abs(j);
                  j=Lj.jEnAtt;
                  }
                else
                  {
                  CopyPT3(Pts[nPts], Lj.St);
                  iFrom=abs(j);
                  j=Lj.jStAtt;
                  }
                nPts+=1;
                }
              Segs[nSegs].iEnPt=nPts-1;
              nSegs+=1;
              }
            }


#if dbgLineExport 
          for (int i=0; i<nPts; i++)
            dbgpln("                           Pt %3i : %10.3f %10.3f", i, Pts[i][0], Pts[i][1]);
#endif

          // Move Seq around to get into order
          for (int i=1; i<nSegs; i++)
            {
            CSeg & iSg=Segs[i];
            iSg.iStConn=0;
            iSg.iEnConn=0;
            }

          //Do nSegs -1 passes - The last would be the biggest gap;
          for (int iPass=1; iPass<nSegs-1; iPass++)
            {
            double SmallGap=1e10;
            int iBigGap=-1;
            int jBigGap=-1;
            for (int i=1; i<nSegs; i++)
              {
              CSeg & iSg=Segs[i];
              for (int j=1; j<nSegs; j++)
                {
                CSeg & jSg=Segs[j];
                if (i==j)
                  continue;
                double StDs=Sqrt(Sqr(Pts[iSg.iStPt][0]-Pts[jSg.iStPt][0])+Sqr(Pts[iSg.iStPt][1]-Pts[jSg.iStPt][1]));
                double StDe=Sqrt(Sqr(Pts[iSg.iStPt][0]-Pts[jSg.iEnPt][0])+Sqr(Pts[iSg.iStPt][1]-Pts[jSg.iEnPt][1]));
                if (!iSg.iStConn && !jSg.iStConn && StDs<SmallGap)
                  {
                  iBigGap=i;
                  jBigGap=j;
                  SmallGap=StDs;
                  }
                if (!iSg.iStConn && !jSg.iEnConn && StDe<SmallGap)
                  {
                  iBigGap=i;
                  jBigGap=-j;
                  SmallGap=StDe;
                  }

                double EnDs=Sqrt(Sqr(Pts[iSg.iEnPt][0]-Pts[jSg.iStPt][0])+Sqr(Pts[iSg.iEnPt][1]-Pts[jSg.iStPt][1]));
                double EnDe=Sqrt(Sqr(Pts[iSg.iEnPt][0]-Pts[jSg.iEnPt][0])+Sqr(Pts[iSg.iEnPt][1]-Pts[jSg.iEnPt][1]));
                if (!iSg.iEnConn && !jSg.iStConn && EnDs<SmallGap)
                  {
                  iBigGap=-i;
                  jBigGap=j;
                  SmallGap=EnDs;
                  }
                if (!iSg.iEnConn && !jSg.iEnConn && EnDe<SmallGap)
                  {
                  iBigGap=-i;
                  jBigGap=-j;
                  SmallGap=EnDe;
                  }
                }
              }
#if dbgLineExport 
            dbgpln("                 SmallGap  %3i %3i : %10.3f ", iBigGap, jBigGap, SmallGap);
#endif
            if (iBigGap>0)
              Segs[iBigGap].iStConn=jBigGap;
            else
              Segs[-iBigGap].iEnConn=jBigGap;
            if (jBigGap>0)
              Segs[jBigGap].iStConn=iBigGap;
            else
              Segs[-jBigGap].iEnConn=iBigGap;
            }

#if dbgLineExport 
          for (int i=1; i<nSegs; i++)
            dbgpln("                           Sg %3i : %3i %3i %3i %3i", i, Segs[i].iStPt, Segs[i].iEnPt, Segs[i].iStConn, Segs[i].iEnConn);
#endif

          GTI.PointsCount = index_offset + nPts;
          LPA.SetSize(GTI.PointsCount);

          int Sq=SeqOffset;

          int nSeq=0;
          int iSeq=1;
          for (int i=1; i<nSegs; i++)
            {
            if (!Segs[i].iStConn)
              iSeq=i;
            else if (!Segs[i].iEnConn)
              iSeq=-i;
            }
          while (iSeq)
            {
            int St,En, j;
            if (iSeq>0)
              {
              St=Segs[iSeq].iStPt;
              En=Segs[iSeq].iEnPt+1;
              j=Segs[iSeq].iEnConn;
              }
            else
              {
              St=Segs[-iSeq].iEnPt;
              En=Segs[-iSeq].iStPt-1;
              j=Segs[-iSeq].iStConn;
              }
            int iChg=En>St?1:-1;
            for (int i=St; i!=En; i+=iChg)
              {
              REAL *P=Pts[i];
#if dbgLineExport 
              dbgpln("                                                 %3i %10.3f %10.3f", Sq, P[0], P[1]);
#endif
              LPA[index_offset+nSeq].Set(Sq, P[0], P[1], P[2]);
              nSeq++;
              Sq++;
              }
            Sq++; // Leave a gap is Seq
            iSeq=j;
            }

          c3d_free_curve(c3);
          }
        else if (C3_CURVE_IS_ARC(c))
          {
          nLines=0;
          LogNote("CollectLinkInfo", 0, "ARC not implemented");
          }
        else if (C3_CURVE_IS_ELLIPSE(c))
          {
          nLines=0;
          LogNote("CollectLinkInfo", 0, "ELLIPSE not implemented");
          }
        else if (C3_CURVE_IS_PCURVE(c))
          {
          nLines=0;
          int SegCnt=0;
          C3_CURVE c3;

          while (1)
            {
            c3 = c3d_pcurve_segment(c, SegCnt);
            if (c3==NULL)
              break;
            SegCnt++;
            }

          bool OK = (SegCnt>0);
          if (IgnoreArrows)
            {
            double ArrowScale;
            bool isArrow = (SegCnt==6 && IsArrow(c, ArrowScale));
            if (isArrow)
              OK = false;
            }

          if (OK)
            {
            C3_CURVE cnew = c3d_copy(c);
            c3c_transform(cnew, c3t);
            const long SeqOffset = (GTI.PointsCount>0 ? LPA[GTI.PointsCount-1].seq+2 : 0);
            const long index_offset = GTI.PointsCount;
            GTI.PointsCount += (SegCnt+1);
            LPA.SetSize(GTI.PointsCount);
            SegCnt=0;
            while (1)
              {
              c3 = c3d_pcurve_segment(cnew, SegCnt);
              if (c3==NULL)
                break;
              if (C3_CURVE_IS_LINE(c3))
                {
                if (SegCnt==0)
                  LPA[index_offset].Set(SeqOffset+SegCnt, C3_CURVE_LINE(c3)->pt[0][0], C3_CURVE_LINE(c3)->pt[0][1], C3_CURVE_LINE(c3)->pt[0][2]);
                LPA[index_offset+SegCnt+1].Set(SeqOffset+SegCnt+1, C3_CURVE_LINE(c3)->pt[1][0], C3_CURVE_LINE(c3)->pt[1][1], C3_CURVE_LINE(c3)->pt[1][2]);
                }
              SegCnt++;
              }
            c3d_free_curve(cnew);
            }
          /*
          while (1)
          {
          c3 = c3d_pcurve_segment(c, SegCnt);
          if (c3==NULL)
          break;
          SegCnt++;
          }
          if (SegCnt)
          {
          const long SeqOffset = (GTI.PointsCount>0 ? LPA[GTI.PointsCount-1].seq+2 : 0);
          const long index_offset = GTI.PointsCount;
          GTI.PointsCount += (SegCnt+1);
          LPA.SetSize(GTI.PointsCount);
          SegCnt=0;
          while (1)
          {
          c3 = c3d_pcurve_segment(c, SegCnt);
          if (c3==NULL)
          break;
          if (C3_CURVE_IS_LINE(c3))
          {
          if (SegCnt==0)
          LPA[index_offset].Set(SeqOffset+SegCnt, C3_CURVE_LINE(c3)->pt[0][0], C3_CURVE_LINE(c3)->pt[0][1], C3_CURVE_LINE(c3)->pt[0][2]);
          LPA[index_offset+SegCnt+1].Set(SeqOffset+SegCnt+1, C3_CURVE_LINE(c3)->pt[1][0], C3_CURVE_LINE(c3)->pt[1][1], C3_CURVE_LINE(c3)->pt[1][2]);
          }
          SegCnt++;
          }
          }
          */

          /*DML_ITEM item ;
          C3_WALK_PCURVE ( c, item )
          {
          int x=0;
          }*/
          }
        else if (C3_CURVE_IS_SPLINE(c))
          {
          LogNote("CollectLinkInfo", 0, "SPLINE not implemented");
          }
        //if( l->GetCurveBox(&box,c,c3t))
        double xm=C3_CURVE_X_MIN(c);
        }
      }
    }
  //    b->GetBounds(c3t, &Bounds);
  }

// ---------------------------------------------------------------------------
// Take each entity within insert e and transforms it through the insert transform
// and adds it to the drawing
flag DXF_Drawing::Explode(DXF_ENTITY insert)
  {
#if dbgexplode
  PrintEntities(pDsp->Vp1, "explode1.dbg");
  dbgpln("!!!!!!!!!!!!!!File explode1.dbg create\n!!!!!!!!!!!!!!!!");
#endif
  C3_TRANSFORM Imat;

  // Under Some Conditions this BlkName is no longer valid ?
  if (DXF_INSERT_BLOCK_GET(insert))
    {
    pBlock b = Drawing->Blocks.Find(DXF_INSERT_BLOCK_GET(insert));
    ASSERT(b);
    Drawing->ConstructInsertMatrix(Imat, b, insert);
    b->Explode(Imat, insert, b);
    Drawing->Delete(insert);
    }

#if dbgexplode
  PrintEntities(pDsp->Vp1, "explode2.dbg");
  dbgpln("!!!!!!!!!!!!!!File explode2.dbg create\n!!!!!!!!!!!!!!!!");
#endif
  return True;
  }

// ---------------------------------------------------------------------------

void DXF_Drawing::SetCurveLtype(C3_CURVE c,pchar ltype)
  {
  ASSERT(0);// not implemented
  }

// ---------------------------------------------------------------------------

pchar DXF_Drawing::GetCurveLtype(C3_CURVE c)
  {
  ASSERT(0);// not implemented
  return NULL;
  }

// ---------------------------------------------------------------------------

pBlock DXF_Drawing::ConstructBlockFromSelection(pViewport v,pchar KernalName,Pt_3f pt)
  {
  DWORD CW=FPP_DisableExceptions();
  Pt_3f basept;
  char nm[1024];
  CEntInView* p;
  pBlock b = (pBlock)NULL;

  basept.Set(0.0,0.0,0.0);
  int unum = Blocks.FindUnique(KernalName, -1);
  //if (unum>=0)
  sprintf(nm, "%s_%d", KernalName, Max(1,unum));
  //else
  //  strcpy(nm, KernalName);
  b = Add_Block(nm, basept.p());

  dbgpln(" Constructing new block %s\n",nm);
  p = v->FirstSelectedEntity();
  while (p)
    {
    if (p->CurvePtr())
      {
      //p->EntityInvalidate();
      C3_CURVE tmp = p->CurvePtr();
      TRACE("XLate Crv %g %g %g : %g %g %g",basept.X,basept.Y,basept.Z,pt.X,pt.Y,pt.Z);
      TranslateEntity(p->CurvePtr(), basept, pt);
      if (! RemoveFromLayer(p->CurvePtr()))
        {
        dbgpln(" REmove from layer failed\n");
        return (pBlock)NULL;
        }
      b->Add_Curve(tmp);
      //EntityInvalidate();
      //dbgpln(" Translating Curve for Construction\n");
      }
    else if (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
      {
      //p->EntityInvalidate();
      DXF_ENTITY tmp = p->EntityPtr();
      TRACE("XLate Ins %g %g %g : %g %g %g",pt.X,pt.Y,pt.Z,basept.X,basept.Y,basept.Z);
      TranslateEntity(p->EntityPtr(), pt, basept);
      if (! RemoveFromLayer(p->EntityPtr()))
        {
        dbgpln(" REmove from layer failed\n");
        return (pBlock)NULL;
        }
      b->Add_Insert(tmp);
      //dbgpln(" Translating Block for Construction\n");
      }
    else if (p->EntityPtr())
      {
      //p->EntityInvalidate();
      DXF_ENTITY tmp = p->EntityPtr();
      TRACE("XLate Ent %g %g %g : %g %g %g",basept.X,basept.Y,basept.Z,pt.X,pt.Y,pt.Z);
      TranslateEntity(p->EntityPtr(), basept, pt);
      if (! RemoveFromLayer(p->EntityPtr()))
        {
        dbgpln(" REmove from layer failed\n");
        return (pBlock)NULL;
        }
      b->Add_Entity(tmp);
      //dbgpln(" Translating Entity for Construction\n");
      }
    p = v->NextSelectedEntity();
    }
  FPP_RestoreExceptions(CW);
  return b;
  }

// ---------------------------------------------------------------------------

int BlockCollection::FindUnique(pchar CoreName, int NoFindRet)
  {
  int val = -99999999;
  int len = strlen(CoreName);
  //int CoreValue = -1;
  flag Found=0;

  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    if (b && _strnicmp(CoreName, DXF_BLOCK_NAME_GET(b->Def), len) == 0)
      {
      pchar s = DXF_BLOCK_NAME_GET(b->Def);
      s += len;
      while (*s && !isdigit(*s)) // cnm - Skip underscores etc
        s++;                     // cnm
      if (* s && (val < atoi(s)))
        {
        val = atoi(s);
        Found=1;
        }
      }
    }
  if (Found)
    return val+1;
  else
    return NoFindRet;
  }

// ---------------------------------------------------------------------------

void BlockCollection::EntityInvalidate(DXF_ENTITY e,C3_CURVE c)
  {
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    b->EntityInvalidate(e,c);
    }
  return;
  }

// ---------------------------------------------------------------------------

void BlockCollection::PrintEntities(pViewport vp, FILE *fp)
  {
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    (void)fprintf(fp,"Block Name %s\n",DXF_BLOCK_NAME_GET(b->Def));
    b->PrintEntities(vp,fp);
    }
  return;
  }

// ---------------------------------------------------------------------------

void BlockCollection::SetDiscColor(DML_LIST llist,int on)
  {
  pBlock b = NULL;
  WALK_BLOCKS(Blocks, b)
    {
    b->SetDiscColor(llist,on);
    }
  return;
  }

// ---------------------------------------------------------------------------

pBlock BlockCollection::Append(DXF_BLOCK block)
  {
  pBlock bn = new Block(Drawing);
  pBlock b;
  if (!Drawing->Blocks.Blocks)
    Drawing->Blocks.Blocks = bn;
  else
    {
    b = Drawing->Blocks.Blocks;
    while (b->Nxt)
      b = b->Nxt;
    b->Nxt = bn;
    }
  bn->Def = block;
  return bn;
  }

// ---------------------------------------------------------------------------

void BlockCollection::Free(pBlock block)
  {
  pBlock b = Drawing->Blocks.Blocks, bp;
  if (b == block)
    Drawing->Blocks.Blocks = b->Nxt;
  else
    {
    while (b != block)
      {
      bp = b;
      b = b->Nxt;
      }
    bp->Nxt = b->Nxt;
    }
  dxf_free_block(block->Def);
  delete b;
  }

// ---------------------------------------------------------------------------

void BlockCollection::Merge(DXF_FILE file)
  {
  DXF_BLOCK blockn;
  DML_ITEM itn;
  DML_LIST NewBlockLst = dxf_get_blocks(file, NULL);
  Strng s;
  pBlock b, bn;

  DML_WALK_LIST(NewBlockLst, itn)
    {
    blockn = (DXF_BLOCK)DML_RECORD(itn);
    s = DXF_BLOCK_NAME_GET(blockn);
    s.LRTrim();
    if (s.Len()>0)
      {//ignore blocks with blank names
      b = NULL;
      WALK_BLOCKS(Blocks, b)
        if (b && _stricmp(DXF_BLOCK_NAME_GET(blockn), DXF_BLOCK_NAME_GET(b->Def))==0)
          break;
      if (b)
        {
#if dbgdxf
        dbgnln();
        dbgp("Duplicate Block Found %s -------", DXF_BLOCK_NAME_GET(b->Def));
#endif
        Free(b);
        }
      bn = Append(blockn);
      bn->IsNew = 1;
      }
    else
      {
      dbgpln("Block without name Found -------");
      }
    }

  dml_free_list(NewBlockLst);
  }

// ---------------------------------------------------------------------------

void BlockCollection::Get(DXF_FILE file, int What,int test_mode)
  {
  pBlock b;
  WALK_BLOCKS(Blocks, b)
    if (b->IsNew)
      {
      if (dxf_set_filepos(file, DXF_BLOCK_NAME_GET(b->Def), DXF_FIRST_ENTITY))
        {
#if dbgdxf
        dbgpln("Get Block %s ---------------------",DXF_BLOCK_NAME_GET(b->Def));
#endif
        b->Clear();
        b->Get(file, What, test_mode);
        }
      b->IsNew = 0;
      }
  }

// ---------------------------------------------------------------------------

void BlockCollection::Put(DXF_FILE file)
  {
  pBlock b;
#if dbgdxf
  dbgpln("  BlockCollection::Put");
#endif

  WALK_BLOCKS(Blocks, b)
    if (b->Def)
      {
      if (b->SaveIt)
        {
#if dbgdxf
        dbgpln("Put Block %s ---------------------", DXF_BLOCK_NAME_GET(b->Def));
#endif
        char LayNm[100];
        MakeLayerName(LayNm, Drawing_Layer, "");
        dxf_open_block(file, DXF_BLOCK_NAME_GET(b->Def), LayNm, DXF_BLOCK_FLAGS(b->Def), DXF_BLOCK_BASE_PT(b->Def));
        b->Put(file);
        dxf_close_block(file);
        }
      else
        {
        b->SaveIt = 1;
        }
      }
  }

//============================================================================

void GetCoreName(pchar Name, pchar Fn)
  {
  pchar p = strrchr(Fn,'\\');
  if( !p )p = Fn;  else p++;
  (void)strcpy(Name,p);
  p = strrchr(Name,'.');
  if( p ) *p = '\0';
  }

//----------------------------------------------------------------------------

DXF_Drawing::DXF_Drawing(pGrf3D_Display pDsp_, pchar Fn, flag InitForSyscad) : LayerCollection(this, NULL), Blocks(this)
  {
  pDsp = pDsp_;

  strnset(Name, '\0', sizeof(Name));
  GotHdrInfo = 0;

#if dbgdxf
  dbgpln(" Making new DXF_Drawing\n");
#endif

  LTypeLst = dml_create_list();
  LayerLst = dml_create_list();
  StyleLst = dml_create_list();
  ViewLst  = dml_create_list();
  UcsLst   = dml_create_list();
  VPortLst = dml_create_list();

#if dbgdxf
  dbgpln(" DXF_Drawing list creation done\n");
#endif

  PT3 p;
  c3v_set_zero(p);
  c3a_box_init_pt(&Bounds, p);
  SetPageSize(A4_Page);
  MakeLayerName(CurLayer, Drawing_Layer, "");

#if dbgdxf
  dbgpln(" DXF_Drawing some setups done\n");
#endif

  if (InitForSyscad)
    {
#if dbgdxf
    dbgpln(" DXF_Drawing in Init for SysCAD\n");
#endif

    int i;
    REAL Lengths[10];
    DXF_TABLE_ENTRY lt[3],ly[4];
    lt[0] = dxf_create_ltype(NULL, "CONTINUOUS", "SOLID LINE", 64, 0.0, 0, NULL);
    Lengths[0] = 0.5;
    Lengths[1] = -0.25;
    Lengths[2] = 0.5;
    Lengths[3] = -0.25;
    Lengths[4] = 0.0;
    Lengths[5] = -0.25;
    lt[1] = dxf_create_ltype(NULL, "BORDER", "__ __ . __ __ . __ __ .", 64, 1.75, 6, Lengths);
    Lengths[0] = 0.0;
    Lengths[1] = -0.25;
    lt[2] = dxf_create_ltype(NULL, "DOT", ". . . . . . . . . . . . .", 64, 0.25, 2, Lengths);
    for (i = 0; i < 3; i++)
      dml_append_data(LTypeLst, lt[i]);

    ly[0] = dxf_create_layer(NULL, LayerNames[Drawing_Layer],  11, False, "CONTINUOUS");
    ly[1] = dxf_create_layer(NULL, LayerNames[IOPt_Layer], 4, False, "DOT");
    ly[2] = dxf_create_layer(NULL, LayerNames[Connection_Layer], 15, False, "CONTINUOUS");
    ly[3] = dxf_create_layer(NULL, LayerNames[Construction_Layer], 15, False, "CONTINUOUS");
    for (i = 0; i < 4; i++)
      dml_append_data(LayerLst, ly[i]);
#if dbgdxf
    dbgpln(" DXF_Drawing Init for SysCAD done\n");
#endif
    }

  if (Fn)
    {
    strcpy(Name, Fn);
    pchar a = strrchr(Name,'.');
    if (a) *a = NULL;
    }

  }

//----------------------------------------------------------------------------

DXF_Drawing::~DXF_Drawing()
  {
  typedef  void  (*PF_FREEFN)(void*);
  dml_destroy_list(LTypeLst  , (PF_FREEFN)dxf_free_table_entry);
  dml_destroy_list(LayerLst  , (PF_FREEFN)dxf_free_table_entry);
  dml_destroy_list(StyleLst  , (PF_FREEFN)dxf_free_table_entry);
  dml_destroy_list(ViewLst   , (PF_FREEFN)dxf_free_table_entry);
  dml_destroy_list(UcsLst    , (PF_FREEFN)dxf_free_table_entry);
  dml_destroy_list(VPortLst  , (PF_FREEFN)dxf_free_table_entry);
  }

//----------------------------------------------------------------------------

pchar DXF_Drawing::NameOfDrawing()
  {
  CString txt;
  CWnd *w = (pDsp->TheWnd)->GetParent();
  ASSERT( w && w->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
  w->GetWindowText(txt);
  if ((const char*)txt)
    {
    strcpy(Name, (const char*)txt);
    pchar aaa = strrchr(Name,'.');
    if (aaa) *aaa = 0;
    }
  //ASSERT(pDsp->TheWnd->IsKindOf(RUNTIME_CLASS(CGrfWnd)));
  //if (((pGrfWnd)(pDsp->TheWnd))->pDoc)
  //  txt = ((pGrfWnd)(pDsp->TheWnd))->pDoc->GetTitle();
  return Name;
  }

// ---------------------------------------------------------------------------

pBlock DXF_Drawing::LoadFromLib(pchar Path, pchar FileName, pchar BlockName)
  {
  char fn[1024];
  strcpy(fn, Path);
  if (fn[strlen(fn)-1]!='\\')
    strcat(fn, "\\");
  char * pcolon=strchr(FileName, ':');
  if (pcolon)
    {
    strncat(fn, FileName, pcolon-FileName);
    strcat(fn, "\\");
    strcat(fn, pcolon+1);
    }
  else
    strcat(fn, FileName);
  strcat(fn, ".dxf");

  DXF_FILE f = dxf_fopen(fn, DXF_READ);
  if (!f)
    return (pBlock)NULL;

  Pt_3f base;
  base.Set(0.0, 0.0, 0.0);
  pBlock b = Add_Block(BlockName, base.p());
  if (!b)
    {
    return (pBlock)NULL;
    dxf_fclose(f);
    }
  b->IsNew = 1;
  b->SetCurrentLayer(Zero_Layer);
  b->Get(f, 99,0);
  b->IsNew = 0;
  dxf_fclose(f);
  return b;
  }

//----------------------------------------------------------------------------

int DXF_Drawing::Load(pchar Fn, int What,int test_mode)
  {
  DWORD CW=FPP_DisableExceptions();

#if dbgdxf
  dbgpln(" DXF_Drawing doing load on Filename %s\n",Fn);
#endif
  int      i;
  DXF_FILE f = dxf_fopen(Fn, DXF_READ);
  if (f)
    {
    if (What & Load_Tables)
      {
      if (!GotHdrInfo)
        {
        for (i=0; i < N_HDR_VARS; i++)
          if (HdrVarSize[i])
            {
            HdrInfo[i] = new char [HdrVarSize[i]];
            memset(HdrInfo[i], 0, HdrVarSize[i]);
            HdrInfo[i] = (char *)dxf_get_hdr_var(f, HdrVarNames[i], HdrInfo[i]);
            }
          GotHdrInfo = 1;
        }

      MergeLTypeDefs(f);
      MergeLayerDefs(f);
      //
      // Strictly speaking all these list should be merged more carefully - rules not clear at this stage
      //
      dml_append_list(StyleLst,dxf_get_table_entries(f, DXF_STYLE, StyleLst));
      dml_append_list(ViewLst ,dxf_get_table_entries(f, DXF_VIEW , ViewLst));
      dml_append_list(UcsLst  ,dxf_get_table_entries(f, DXF_UCS  , UcsLst));
      dml_append_list(VPortLst,dxf_get_table_entries(f, DXF_VPORT, VPortLst));

      }

#if dbgdxf
    dbgpln(" DXF_Drawing tables loaded\n");
#endif
    if (What & Load_Blocks)
      {
      Blocks.Merge(f);
      Blocks.Get(f, What,test_mode);
      }
#if dbgdxf
    dbgpln(" DXF_Drawing blocks loaded\n");
#endif
    if (What & Load_Drawing)
      LayerCollection::Get(f, What,test_mode);
#if dbgdxf
    dbgpln(" DXF_Drawing entities loaded\n");
#endif

    if( !(f->file_status & DXF_EOFFOUND) )
      {
      dxf_fclose(f);

      char msg[1024];
      (void)sprintf(msg,"Dxf file read did not end on EOF token");
      AfxMessageBox(msg, MB_OK | MB_ICONEXCLAMATION);
      FPP_RestoreExceptions(CW);
      return -1;
      }


    if( f->file_status & DXF_PLINENOVERTS )
      {
      dxf_fclose(f);

      AfxMessageBox("A POLYLINE with < 2 vertices exists in dxf - A cleanup will be done", MB_OK | MB_ICONEXCLAMATION);
      if( CleanupDXF(Fn) )
        {
        AfxMessageBox("Cleanup succeeded - Reloading");
        FPP_RestoreExceptions(CW);
        return 1;
        }
      else
        {
        AfxMessageBox("Cleanup Failed");
        FPP_RestoreExceptions(CW);
        return -1;
        }
      }

    dxf_fclose(f);
    GetBounds();
    }
#if dbgdxf
  dbgpln("Load Done");
#endif

  FPP_RestoreExceptions(CW);
  return 0;
  }

//----------------------------------------------------------------------------

int DXF_Drawing::Save(pchar Fn)
  {
  DWORD CW=FPP_DisableExceptions();

  if (1)//gs_pPrj->bGrfRemoveUnusedBlks)
    {
    int Cnt = RemoveUnusedBlocks();
    //if (Cnt>0)
    //  LogNote("GrfCmds", 0, "%d unused blocks deleted from %s", Cnt, Fn);
    }

  int      i;

  DXF_FILE f = dxf_fopen(Fn, DXF_WRITE | DXF_ASCII);
  if (f)
    {
    dxf_open_header(f);
    if (GotHdrInfo)
      for (i=0; i < N_HDR_VARS; i++)
        if (HdrVarSize[i] && HdrInfo[i])
          dxf_put_hdr_var(f, HdrVarNames[i], HdrInfo[i]);
    dxf_close_header(f);

    dxf_open_tables(f);
    if (dml_length(LTypeLst) > 0)
      {
      dxf_open_table(f, DXF_LTYPE, dml_length(LTypeLst));
      dxf_put_table_entries(f, LTypeLst);
      dxf_close_table(f);
      }
    if (dml_length(LayerLst) > 0)
      {
      dxf_open_table(f, DXF_LAYER, dml_length(LayerLst));
      dxf_put_table_entries(f, LayerLst);
      dxf_close_table(f);
      }
    if (dml_length(StyleLst) > 0)
      {
      dxf_open_table(f, DXF_STYLE, dml_length(StyleLst));
      dxf_put_table_entries(f, StyleLst);
      dxf_close_table(f);
      }
    if (dml_length(ViewLst) > 0)
      {
      dxf_open_table(f, DXF_VIEW , dml_length(ViewLst));
      dxf_put_table_entries(f, ViewLst );
      dxf_close_table(f);
      }
    if (dml_length(UcsLst) > 0)
      {
      dxf_open_table(f, DXF_UCS  , dml_length(UcsLst));
      dxf_put_table_entries(f, UcsLst  );
      dxf_close_table(f);
      }
    if (dml_length(VPortLst) > 0)
      {
      dxf_open_table(f, DXF_VPORT, dml_length(VPortLst));
      dxf_put_table_entries(f, VPortLst);
      dxf_close_table(f);
      }
    dxf_close_tables(f);

    dxf_open_blocks(f);
    Blocks.Put(f);
    dxf_close_blocks(f);

    dxf_open_entities(f);
    LayerCollection::Put(f);
    dxf_close_entities(f);

    /**
    if (What & Load_Blocks)
    Blocks = Get_Blocks(f, Blocks);
    if (What & (Load_Tables | Load_Drawing))
    Layers = Get_Layers(f, Layers, What);
    **/
    dxf_fclose(f);

    }
#if dbgdxf
  dbgpln("Save Done");
  //dbgfileflush(0);
#endif
  FPP_RestoreExceptions(CW);

  return 0;
  }
//----------------------------------------------------------------------------

int DXF_Drawing::SaveSymbols(pchar FnPath, pchar DocGUID)
  {
  DWORD CW=FPP_DisableExceptions();

  if (1)//gs_pPrj->bGrfRemoveUnusedBlks)
    {
    int Cnt = RemoveUnusedBlocks();
    //if (Cnt>0)
    //  LogNote("GrfCmds", 0, "%d unused blocks deleted from %s", Cnt, Fn);
    }

  int      i;
  int    iBlk=0;
  pBlock pBlk=NULL;

  while (pBlk=Blocks.FindByIndex(iBlk++))
    {
    const char * pBlkNm=DXF_BLOCK_NAME_GET(pBlk->Def);
    if (strlen(pBlkNm)==0 || !pBlk->SaveIt)
      continue;

    // check that old blocks that may contain '*' are fixed up. * is invalid in files
    CString BlkNm=MakeValidBlockName((LPTSTR)pBlkNm);

    char Fn[2048];

    //strcpy(Fn, FnPath);
    sprintf(Fn, "%s%s%s.dxf", FnPath, BlkNm, DocGUID);
    //strcat(FnBuff, "pBlk->Fn);


    DXF_FILE f = dxf_fopen(Fn, DXF_WRITE | DXF_ASCII);
    if (f)
      {
      dxf_open_header(f);
      if (GotHdrInfo)
        for (i=0; i < N_HDR_VARS; i++)
          if (HdrVarSize[i] && HdrInfo[i])
            dxf_put_hdr_var(f, HdrVarNames[i], HdrInfo[i]);
      dxf_close_header(f);

      dxf_open_tables(f);
      if (dml_length(LTypeLst) > 0)
        {
        dxf_open_table(f, DXF_LTYPE, dml_length(LTypeLst));
        dxf_put_table_entries(f, LTypeLst);
        dxf_close_table(f);
        }
      if (dml_length(LayerLst) > 0)
        {
        dxf_open_table(f, DXF_LAYER, dml_length(LayerLst));
        dxf_put_table_entries(f, LayerLst);
        dxf_close_table(f);
        }
      if (dml_length(StyleLst) > 0)
        {
        dxf_open_table(f, DXF_STYLE, dml_length(StyleLst));
        dxf_put_table_entries(f, StyleLst);
        dxf_close_table(f);
        }
      if (dml_length(ViewLst) > 0)
        {
        dxf_open_table(f, DXF_VIEW , dml_length(ViewLst));
        dxf_put_table_entries(f, ViewLst );
        dxf_close_table(f);
        }
      if (dml_length(UcsLst) > 0)
        {
        dxf_open_table(f, DXF_UCS  , dml_length(UcsLst));
        dxf_put_table_entries(f, UcsLst  );
        dxf_close_table(f);
        }
      if (dml_length(VPortLst) > 0)
        {
        dxf_open_table(f, DXF_VPORT, dml_length(VPortLst));
        dxf_put_table_entries(f, VPortLst);
        dxf_close_table(f);
        }
      dxf_close_tables(f);

      //dxf_open_blocks(f);
      //pBlk->Put(f);
      //dxf_close_blocks(f);

      dxf_open_entities(f);
      pBlk->Put(f);
      //LayerCollection::Put(f);
      dxf_close_entities(f);

      /**
      if (What & Load_Blocks)
      Blocks = Get_Blocks(f, Blocks);
      if (What & (Load_Tables | Load_Drawing))
      Layers = Get_Layers(f, Layers, What);
      **/
      dxf_fclose(f);
      //LogNote("Graphics", 0, "Symbol %s Saved to %s", pBlkNm, Fn);
      gs_pCmd->Print("Symbol %s Saved to '%s'\n", pBlkNm, Fn);
      }
    else
      {
      gs_pCmd->Print("Symbol %s NOT Saved '%s' not opened", pBlkNm, Fn);
      //LogNote("Graphics", 0, "Symbol %s NOT Saved %s not opened", pBlkNm, Fn);
      }
    }
#if dbgdxf
  dbgpln("Save Done");
  //dbgfileflush(0);
#endif
  FPP_RestoreExceptions(CW);

  return 0;
  }
// start change
//----------------------------------------------------------------------------

void DXF_Drawing::SetGfxLayer(int Ocol,int Lvl,CGrfLayer * eLay,CGrfLayer * iLay,pchar EntityLay,int EntityColor,double EntityThickness)
  {
  if( Ocol != -1 )
    {
    grr_set_color(Ocol);
    }
  else
    {
    if((iLay && iLay->m_DiscColor == -1) || (eLay && eLay->m_DiscColor == -1))
      {

      if( EntityColor == 256  || EntityColor == -1  )// color is bylayer use layer color
        {
        if (_stricmp(EntityLay, "0") == 0 && Lvl > 1 && iLay && iLay->Def)
          {
          grr_set_color(DXF_LAYER_COLOR(iLay->Def));
          }
        else if (eLay)
          {
          grr_set_color(DXF_LAYER_COLOR(eLay->Def));
          }
        else
          {
          TRACE("Color not defined by values entered");
          grr_set_color(GR_WHITE);
          }
        }
      else if( EntityColor == 0 )          // color is black should be byblock
        {
        //TRACE("BYBLOCK COLORS NOT DONE");
        grr_set_color(GR_WHITE);
        }
      else if( EntityColor > 0 && EntityColor <= GR_LASTUSERCOLOR/*16 */)
        {
        grr_set_color(EntityColor);
        }
      else
        {
        //      dbgpln(" Bad entity color");
        }
      }
    else // autocad coloring now ignored and discipline colors are used
      {
      if (_stricmp(EntityLay, "0") == 0 && Lvl > 1 && iLay && iLay->m_DiscColor != -1 )
        {
        grr_set_color(iLay->m_DiscColor);
        }
      else if ( eLay && eLay->m_DiscColor != -1 )
        {
        grr_set_color(eLay->m_DiscColor);
        }
      else
        {
        TRACE("Bad Discipline color");
        grr_set_color(GR_LIGHTGRAY);
        }
      }
    }
#if WithLineThickness
  grr_set_thickness(EntityThickness);
#endif
  }

//----------------------------------------------------------------------------

void DXF_Drawing::ConstructInsertMatrix(C3_TRANSFORM Imat, pBlock b, DXF_ENTITY e)
  {
  C3_TRANSFORM c3t_scl,c3t_xlt,c3t_r0,c3t_rot;
  PT3 axis;

  Imat[0][1] = Imat[0][2] = Imat[0][3] = 0.0;
  Imat[1][0]              = Imat[1][2] = Imat[1][3] = 0.0;
  Imat[2][0] = Imat[2][1]              = Imat[2][3] = 0.0;
  Imat[0][0] = Imat[1][1] = Imat[2][2] = 1.0;

  c3t_scale(DXF_BLOCK_BASE_PT(b->Def), DXF_INSERT_X_SCALE(e), DXF_INSERT_Y_SCALE(e), DXF_INSERT_Z_SCALE(e), c3t_scl);
  c3v_set(0., 0., 1., axis);
  c3t_rotate(DXF_BLOCK_BASE_PT(b->Def), axis, Degs2Rads(DXF_INSERT_ROT_ANG(e)), c3t_rot);
  c3t_mult(c3t_scl, c3t_rot, c3t_r0);
  c3t_translate(DXF_INSERT_PT(e), c3t_xlt);
  c3t_mult(c3t_r0, c3t_xlt, Imat);
  }

//----------------------------------------------------------------------------

void DXF_Drawing::Draw(DXF_ENTITY e, pViewport vp, C3_BOX box, int color, int Lvl, CGrfLayer * pLay, CGrfLayer * iLay, DML_ITEM item)
  {
  DWORD CW = FPP_DisableExceptions();
#if dbgcolor
  if( pLay && iLay )
    {
    pchar elay = DXF_ENTITY_LAYER_GET(e);
    int   ecol = DXF_ENTITY_COLOR(e);
    pchar play = DXF_LAYER_NAME_GET(pLay->Def);
    int   pcol = DXF_LAYER_COLOR(pLay->Def);
    pchar ilay = DXF_LAYER_NAME_GET(iLay->Def);
    int   icol = DXF_LAYER_COLOR(iLay->Def);
    TRACE("Entity Drawer has EntityLay (%s %d) InsertLay (%s %d) EntityInfo (%s %d) Color=%d Level=%d",play,pcol,ilay,icol,elay,ecol,color,Lvl);
    }
  else
    {
    TRACE("Entity Drawer has NULL CGrfLayer");
    }
#endif

  int Ocolor=-1;
  if (color==-1 && gs_pPrj->ShowStatus())
    if (GrfHelper.iShowMdlStatusUnKnown)
      Ocolor=GR_MDLSTATUS_UNKNOWN;

  if (color!=-1 && item->pKenwalta)
    {
    CEntInView* l = (CEntInView*)(item->pKenwalta);
    item->pKenwalta = (void *)l->EntityInvalidate();
    }

  if (DXF_ENTITY_IS_INSERT(e) && item->pKenwalta == NULL)
    {
    pBlock b;
    b = Blocks.Find(DXF_INSERT_BLOCK_GET(e));
    if (b)
      {
      pchar entitytag = Find_Attr_Value(e, "TAG");
      int RqdColor= (color>=0 ? color : entitytag==NULL ? Ocolor : -1);

#if dbgdxfdraw
      dbgpln("  %*s Draw Block %s @ %g,%g,%g * %g,%g,%g %i [%i]%s",Lvl," ",
        DXF_BLOCK_NAME_GET(b->Def),
        DXF_INSERT_PT(e)[0],DXF_INSERT_PT(e)[1],DXF_INSERT_PT(e)[2],
        DXF_INSERT_X_SCALE(e), DXF_INSERT_Y_SCALE(e), DXF_INSERT_Z_SCALE(e),
        DXF_ENTITY_COLOR(e),
        RqdColor, entitytag);
#endif

      VP_TRANSFORM vpt;
      C3_TRANSFORM Imat,Imat_i;
      ConstructInsertMatrix(Imat, b, e);
      SetGfxLayer(color, Lvl, pLay, iLay, (pchar)DXF_ENTITY_LAYER_GET(e), (int)DXF_ENTITY_COLOR(e), DXF_ENTITY_THICKNESS(e));

      vpi_get_transform(vp->vp, vpt);
      //vp3_append_transform(vp->vp, c3t_r1);
      vp3_append_transform(vp->vp, Imat);
      C3_BOX_S boxt;
      //c3t_inverse(c3t_r1, c3t_r1_i);
      c3t_inverse(Imat, Imat_i);
      PT3 mn, mx;
      //c3t_eval_pt(C3_MIN_PT(box), c3t_r1_i, mn);
      //c3t_eval_pt(C3_MAX_PT(box), c3t_r1_i, mx);

      c3t_eval_pt(C3_MIN_PT(box), Imat_i, mn);
      c3t_eval_pt(C3_MAX_PT(box), Imat_i, mx);

      c3v_set(Min(mn[0], mx[0]), Min(mn[1], mx[1]), Min(mn[2], mx[2]), C3_MIN_PT(&boxt));
      c3v_set(Max(mn[0], mx[0]), Max(mn[1], mx[1]), Max(mn[2], mx[2]), C3_MAX_PT(&boxt));

#if dbgBlk
      if (Lvl == 1)
        dbgpln("  %*s Blkdbg opening insert segment for %s",Lvl," ",DXF_INSERT_BLOCK_GET(e));
      dbgpln("         Blkdbg INSERT");
#endif

      // Look at the number of fill tags on the insert and create a
      // CLVector to hold the seg ids
      if (Lvl==1)
        {
        int FillSize = 0;
        vp->Curseg_level = -1;
        pchar filltag = Find_Attr_Value(e, "FILLTAG");
        if (filltag)
          {
          pchar fillvar   = Find_Attr_Value(e, "FILLVAR");
          pchar fillmin   = Find_Attr_Value(e, "FILLMIN");
          pchar fillmax   = Find_Attr_Value(e, "FILLMAX");
          pchar fillcolor = Find_Attr_Value(e, "FILLCOLOR");
          if (fillvar && fillmin && fillmax)
            FillSize = 1;
          }

        int BarsSize = 0;
        pchar dynmtags = Find_Attr_Value(e, "DYNMTAGS");
        if (dynmtags)
          {
          pchar dynmvars = Find_Attr_Value(e, "DYNMVARS");
          pchar dynmmins = Find_Attr_Value(e, "DYNMMINS");
          pchar dynmmaxs = Find_Attr_Value(e, "DYNMMAXS");
          if (dynmvars && dynmmins && dynmmaxs)
            {
            Strng_List ltag,lvar;
            ltag.AppendTokString(dynmtags, " ");
            lvar.AppendTokString(dynmvars, " ");
            int n1 = ltag.Length();
            int n2 = lvar.Length();
            if (n1 && n1==n2)
              BarsSize = n1;
            }
          }

        int ToggSize = 0;
        pchar toggtags = Find_Attr_Value(e, "TOGGTAGS");
        if (toggtags)
          {
          pchar toggvars = Find_Attr_Value(e, "TOGGVARS");
          if (toggvars)
            {
            Strng_List ltag,lvar;
            ltag.AppendTokString(toggtags, " ");
            lvar.AppendTokString(toggvars, " ");
            int n1 = ltag.Length();
            int n2 = lvar.Length();
            if (n1 && n1==n2)
              ToggSize = n1;
            }
          }

        if (FillSize+BarsSize+ToggSize>0)
          {
          vp->Curseg_offset.SetSize(FillSize+BarsSize+ToggSize, -1);
          vp->Curseg_level  = 0;
          }
        ini_vector_buffer();
        }

      b->Draw(vp, &boxt, color, Lvl+1, iLay);
      vpi_set_transform(vp->vp, vpt);

      DML_LIST l1 = DXF_INSERT_ATTRS(e);
      DML_ITEM it1;
      DXF_ENTITY e1;
      if (l1)
        {
        WALK_ENTITY_LIST(l1, it1, e1)
          {
          Draw(e1, vp, &boxt, color, Lvl+1, pLay, iLay, item);
          }
        }

      if (Lvl==1)
        {
        char * pBlockName=DXF_BLOCK_NAME_GET(b->Def);
        int AllowBoxSlct=
          (pBlockName &&
          _strnicmp(pBlockName, "Pipe-", 5)!=0 &&
          _strnicmp(pBlockName, "Link-", 5)!=0);

        CEntInView* l = new CEntInView(item, e, (C3_CURVE)NULL, vp, AllowBoxSlct, RqdColor, -1, -1, DXF_ENTITY_THICKNESS(e), entitytag);
        if (!l->NumPoint())
          {
          delete l;
          l = NULL;
          item->pKenwalta = NULL;
          }
        else
          {
          item->pKenwalta = (void*)l;
          vp->AddDisplaySegment(l);
          l->Qdisplay();
          if (vp->Curseg_level!=-1)
            {
            int cnt = 0;
            pchar filltag = Find_Attr_Value(e, "FILLTAG");
            if (filltag)
              {
              char Tmp[32];
              sprintf(Tmp, "%d", SEG_TBFILL);
              pchar fillvar = Find_Attr_Value(e, "FILLVAR");
              pchar fillmin = Find_Attr_Value(e, "FILLMIN");
              pchar fillmax = Find_Attr_Value(e, "FILLMAX");
              pchar fillcolor = Find_Attr_Value(e, "FILLCOLOR");
              COLORREF Color = DEFAULTFILLCOLOUR;
              if (fillcolor)
                {
                Color = atol(fillcolor);
                l->SetFillColorRqd(Color);
                }

              if (fillvar && fillmin && fillmax)
                {
                //TRACE("Adding Fill %9s %9s %d\n", ptag->Str(), pvar->Str(), vp->Curseg_offset[cnt]);
                Strng WrkVar, WrkCnvTxt;
                TaggedObject::SplitTagCnv(fillvar, WrkVar, WrkCnvTxt);
                CCnvIndex CnvInx;
                double CurVal;
                if (pDsp->FetchTaggedSIValue(filltag, WrkVar(), CurVal, CnvInx, WrkCnvTxt))
                  {
                  DynamicSegment* pDS = new DynamicSegment(l, filltag, fillvar, CnvInx, WrkCnvTxt(), Tmp, DynFill, Color, atof(fillmin), atof(fillmax), CurVal, vp->Curseg_offset[cnt]);
                  vp->AddDynamicSegment(pDS);
                  vp->DrawDynamicSegment(pDS);
                  }
                cnt++;
                }
              }

            pchar dynmtags = Find_Attr_Value(e, "DYNMTAGS");
            if (dynmtags)
              {
              char Tmp[32];
              sprintf(Tmp, "%d", SEG_TBFILL);
              Strng_List ltag,lvar,lmin,lmax,lcol;
              pStrng     ptag,pvar,pmin,pmax,pcol;
              COLORREF   Color;

              pchar dynmvars = Find_Attr_Value(e, "DYNMVARS");
              pchar dynmmins = Find_Attr_Value(e, "DYNMMINS");
              pchar dynmmaxs = Find_Attr_Value(e, "DYNMMAXS");
              pchar dynmcols = Find_Attr_Value(e, "DYNMCOLOR");

              ltag.AppendTokString(dynmtags, " ");
              lvar.AppendTokString(dynmvars, " ");
              lmin.AppendTokString(dynmmins, " ");
              lmax.AppendTokString(dynmmaxs, " ");
              if (dynmcols)
                lcol.AppendTokString(dynmcols, " ");

              ptag = ltag.First();
              pvar = lvar.First();
              pmin = lmin.First();
              pmax = lmax.First();
              pcol = lcol.First();

              //int cnt = 0;
              while (ptag && pvar && pmin && pmax)
                {
                //TRACE("Adding dynamic bar %9s %9s %d\n", ptag->Str(), pvar->Str(), vp->Curseg_offset[cnt]);
                Strng WrkVar, WrkCnvTxt;
                TaggedObject::SplitTagCnv(pvar->Str(), WrkVar, WrkCnvTxt);
                CCnvIndex CnvInx;
                double CurVal;
                if (pDsp->FetchTaggedSIValue(ptag->Str(), pvar->Str(), CurVal, CnvInx, WrkCnvTxt))
                  {
                  Color = DEFAULTBARSCOLOUR;
                  if (pcol)
                    Color = atol(pcol->Str());
                  DynamicSegment* pDS = new DynamicSegment(l, ptag->Str(), pvar->Str(), CnvInx, WrkCnvTxt(), Tmp, DynBars, Color, atof(pmin->Str()), atof(pmax->Str()), CurVal, vp->Curseg_offset[cnt]);
                  vp->AddDynamicSegment(pDS);
                  vp->DrawDynamicSegment(pDS);
                  }

                cnt++;
                ptag = ltag.Next();
                pvar = lvar.Next();
                pmin = lmin.Next();
                pmax = lmax.Next();
                pcol = lcol.Next();
                }
              }

            pchar toggtags = Find_Attr_Value(e, "TOGGTAGS");
            if (toggtags)
              {
              char Tmp[32];
              sprintf(Tmp, "%d", SEG_TBFILL);
              Strng_List ltag,lvar;
              pStrng     ptag,pvar;
              COLORREF   Color = DEFAULTTOGGCOLOUR;
              pchar toggvars = Find_Attr_Value(e, "TOGGVARS");
              ltag.AppendTokString(toggtags, " ");
              lvar.AppendTokString(toggvars, " ");
              ptag = ltag.First();
              pvar = lvar.First();

              //int cnt = 0;
              while (ptag && pvar)
                {
                //TRACE("Adding toggle %9s %9s %d\n", ptag->Str(), pvar->Str(), vp->Curseg_offset[cnt]);
                Strng WrkVar, WrkCnvTxt;
                TaggedObject::SplitTagCnv(pvar->Str(), WrkVar, WrkCnvTxt);
                CCnvIndex CnvInx;
                double CurVal;
                if (pDsp->FetchTaggedSIValue(ptag->Str(), pvar->Str(), CurVal, CnvInx, WrkCnvTxt))
                  {
                  DynamicSegment* pDS = new DynamicSegment(l, ptag->Str(), pvar->Str(), CnvInx, WrkCnvTxt(), Tmp, DynToggle, Color, 0.0, 0.0, CurVal, vp->Curseg_offset[cnt]);
                  vp->AddDynamicSegment(pDS);
                  vp->DrawDynamicSegment(pDS);
                  }

                cnt++;
                ptag = ltag.Next();
                pvar = lvar.Next();
                }
              }

            vp->Curseg_offset.SetSize(1, -1);
            vp->Curseg_level = -1;
            }
          }
        }
      }
    else
      {
#if dbgBlk
      pchar iblk=DXF_INSERT_BLOCK_GET(e);
      pchar entitytag = Find_Attr_Value(e, "TAG");
      dbgpln("  %*s Blkdbg MISSING Block %s %s",Lvl," ",iblk,entitytag);
#endif
      }
#if dbgBlk
    if (Lvl==1)
      dbgpln("  %*s Blkdbg closing insert segment",Lvl," ");
#endif
    }
  else if (DXF_ENTITY_IS_TEXT(e) && item->pKenwalta == NULL)
    {
#if dbgBlk
    if (Lvl==1)
      dbgpln("  %*s Blkdbg opening text segment",Lvl," ");
    dbgpln("         Blkdbg TEXT");
#endif
    SetGfxLayer(color, Lvl, pLay, iLay, (pchar)DXF_ENTITY_LAYER_GET(e), (int)DXF_ENTITY_COLOR(e), DXF_ENTITY_THICKNESS(e));
    if (Lvl==1)
      ini_vector_buffer();
    dxf_text_to_viewport(e, StyleLst, vp->vp);
    if (Lvl==1)
      {
      int RqdColor= (color>=0 ? color : Ocolor);
      CEntInView* l = new CEntInView(item, e, (C3_CURVE)NULL, vp, false, RqdColor, -1, -1, DXF_ENTITY_THICKNESS(e), NULL);
      if (!l->NumPoint())
        {
        delete l;
        l = NULL;
        item->pKenwalta = NULL;
        }
      else
        {
        item->pKenwalta = (void*)l;
        vp->AddDisplaySegment(l);
        l->Qdisplay();
        }
      }
#if dbgBlk
    if (Lvl==1)
      dbgpln("  %*s Blkdbg closing text segment", Lvl," ");
#endif
    }
  else if (DXF_ENTITY_IS_ATTRIB(e) && item->pKenwalta == NULL)
    {
    if ((DXF_ATTRIB_AFLAGS(e) & DXF_ATTRIB_INVIS) == 0)
      {
#if dbgBlk
      if (Lvl==1)
        dbgpln("  %*s Blkdbg opening attrib segment",Lvl," ");
      dbgpln("         Blkdbg ATTRIB");
#endif
      SetGfxLayer(color, Lvl, pLay, iLay, (pchar)DXF_ENTITY_LAYER_GET(e), (int)DXF_ENTITY_COLOR(e), DXF_ENTITY_THICKNESS(e));
      if (Lvl==1)
        ini_vector_buffer();
      dxf_attrib_to_viewport(e, StyleLst, vp->vp);
      if (Lvl==1)
        {
        int RqdColor= (color>=0 ? color : Ocolor);
        CEntInView* l = new CEntInView(item, e, (C3_CURVE)NULL, vp, false, RqdColor, -1, -1, DXF_ENTITY_THICKNESS(e), NULL);
        if (!l->NumPoint())
          {
          delete l;
          l = NULL;
          item->pKenwalta = NULL;
          }
        else
          {
          item->pKenwalta = (void*)l;
          vp->AddDisplaySegment(l);
          l->Qdisplay();
          }
        }

#if dbgBlk
      if (Lvl==1)
        dbgpln("  %*s Blkdbg closing attrib segment",Lvl," ");
#endif
      }
    }

  //PrintEntities(pDsp->Vp1,"current.ent");
  if (item->pKenwalta)
    {
    flag DoDisp=true;
    CEntInView* l = (CEntInView*)(item->pKenwalta);
    int ovrcolor=-1;
    int ovrstyle=-1;
    if (color==-1 && gs_pPrj->ShowStatus())
      {
      pchar entitytag = l->Tag();
      if (entitytag && gs_pTheSFELib)
        {
        dword Status=gs_pTheSFELib->FE_GetModelStatus(entitytag);
        GrfHelper.MdlStatusColor(Status, color, ovrcolor, ovrstyle);
        }
      else
        color=GR_OTHERGRAPHICS;
      if (vp->m_bUpdateOnly && !vp->m_bColoursChgd)
        DoDisp=(color!=l->ColorDrawn()) || (ovrcolor!=l->OvrColorDrawn());
      }
    if (DoDisp)
      {
      l->SetColorRqd(color, ovrcolor, ovrstyle);//gs_pPrj->iOverlayStatusPen);
      l->Qdisplay();
      }
    }
  FPP_RestoreExceptions(CW);
  }

//----------------------------------------------------------------------------

void DXF_Drawing::Draw(C3_CURVE c, pViewport vp, C3_BOX box, int color, int Lvl,CGrfLayer * pLay,CGrfLayer * iLay,DML_ITEM item)
  {
  DWORD CW=FPP_DisableExceptions();

//dbgpln("DXF_Drawing::Draw");

#if dbgcolor
  if( pLay && iLay )
    {
    pchar elay = C3_CURVE_LAYER_GET(c);
    int   ecol = c->color;
    pchar play = DXF_LAYER_NAME_GET(pLay->Def);
    int   pcol = DXF_LAYER_COLOR(pLay->Def);
    pchar ilay = DXF_LAYER_NAME_GET(iLay->Def);
    int   icol = DXF_LAYER_COLOR(iLay->Def);

    TRACE("Curve Drawer has EntityLay (%s %d) InsertLay (%s %d) EntityInfo (%s %d) Color=%d Level=%d",play,pcol,ilay,icol,elay,ecol,color,Lvl);
    }
  else
    {
    TRACE("Curve Drawer has NULL CGrfLayer");
    }
#endif
#if dbgdxfdraw
  pchar s;
  if (C3_CURVE_IS_LINE(c)) s = "LINE";
  else if (C3_CURVE_IS_ARC(c)) s = "ARC";
  else if (C3_CURVE_IS_PCURVE(c)) s = "PCURVE";
  else if (C3_CURVE_IS_ELLIPSE(c)) s = "ELLIPSE";
  else if (C3_CURVE_IS_SPLINE(c)) s = "SPLINE";
  else s = "????";
  dbgpln("  %*s Draw Curve %s %g,%g,%g  %g,%g,%g",Lvl," ",s,
    C3_CURVE_X_MIN(c), C3_CURVE_Y_MIN(c), C3_CURVE_Z_MIN(c),
    C3_CURVE_X_MAX(c), C3_CURVE_Y_MAX(c), C3_CURVE_Z_MAX(c));
  s = (c3a_box_overlap(box, C3_CURVE_BOX(c)) ? "Y" : "n");
  dbgpln("  %*s InBox[%s]  %g,%g,%g  %g,%g,%g",Lvl," ",s,
    C3_MIN_X(box), C3_MIN_Y(box), C3_MIN_Z(box),
    C3_MAX_X(box), C3_MAX_Y(box), C3_MAX_Z(box));
#endif

  int Ocolor=-1;
  if (color==-1 && gs_pPrj->ShowStatus())
    if (GrfHelper.iShowMdlStatusUnKnown)
      Ocolor=GR_MDLSTATUS_UNKNOWN;

  if (color != -1 && item->pKenwalta)
    {
    CEntInView* l = (CEntInView*)(item->pKenwalta);
    item->pKenwalta = (void *)l->EntityInvalidate();
    }


  if( item->pKenwalta == NULL )
    {
    SetGfxLayer(color , Lvl , pLay , iLay , C3_CURVE_LAYER_GET(c) , (int)c->m_Color, c->m_Thickness);
#if dbgBlk
    if( Lvl == 1 )
      dbgpln("  %*s Blkdbg opening curve segment",Lvl," ");
    dbgpln("         Blkdbg CURVE");
#endif
    if( Lvl == 1 )
      ini_vector_buffer();
    v3d_curve ( c, NULL, NULL ,vp->vp);
    if( Lvl == 1 )
      {
      CEntInView* l = new CEntInView(item,(DXF_ENTITY)NULL,c,vp, false, Ocolor, -1, -1, c->m_Thickness, NULL);
      if( !l->NumPoint() )
        {
        delete l;
        l = NULL;
        item->pKenwalta = NULL;
        }
      else
        {
        item->pKenwalta = (void *)l;
        vp->AddDisplaySegment(l);
        l->Qdisplay();
        }
      }

#if dbgBlk
    if( Lvl == 1 )
      dbgpln("  %*s Blkdbg closing curve segment",Lvl," ");
#endif

    if( vp->Curseg_level != -1 && vp->Curseg_level != vp->Curseg_offset.GetLen() )
      {
      // if( _stricmp(DXF_LAYER_NAME(pLay->Def),LayerNames[Drawing_Layer]) == 0 )
      //   {
      vp->Curseg_offset[vp->Curseg_level] = get_buffer_offset();
      vp->Curseg_level += 1;
      //   }
      }

    }
  if( item->pKenwalta )
    {
    CEntInView* l = (CEntInView*)(item->pKenwalta);
    l->Qdisplay();
    }

  FPP_RestoreExceptions(CW);
  }

//----------------------------------------------------------------------------

void dump_vp(VP_VIEWPORT vp)
  {
  dbgpln("@  : %p",vp);
  dbgpln("XF : {%g %g %g %g}{%g %g %g %g}",
    vp->transform[0][0], vp->transform[0][1], vp->transform[0][2],  vp->transform[0][3],
    vp->transform[1][0], vp->transform[1][1], vp->transform[1][2],  vp->transform[1][3]);

  dbgpln("INV: {%g %g %g}{%g %g %g}{%g %g %g}",
    vp->inverse[0][0], vp->inverse[0][1], vp->inverse[0][2],
    vp->inverse[1][0], vp->inverse[1][1], vp->inverse[1][2],
    vp->inverse[2][0], vp->inverse[2][1], vp->inverse[2][2]);

  dbgpln("Grn: %g",vp->granularity);

  dbgpln("VW : {%g %g %g}{%g %g %g}{%g %g %g}",
    vp->view.view_ctr_pt[0], vp->view.view_ctr_pt[1], vp->view.view_ctr_pt[2],
    vp->view.view_x_vec[0], vp->view.view_x_vec[1], vp->view.view_x_vec[2],
    vp->view.view_y_vec[0], vp->view.view_y_vec[1], vp->view.view_y_vec[2]);

  dbgpln("SC : {%g %g}{%g %g}",
    vp->screen_box.minpt[0],
    vp->screen_box.minpt[1],
    vp->screen_box.maxpt[0],
    vp->screen_box.maxpt[1]);
  }

//----------------------------------------------------------------------------

BOOL DXF_Drawing::PrintEntities(pViewport vp, pchar fn)
  {
  FILE* fp = fopen(fn, "w");
  if (fp)
    {
    CWaitCursor Wait;
    fprintf(fp," Starting printing blocks ***********************************\n");
    Blocks.PrintEntities(vp,fp);
    fprintf(fp," Done printing blocks ***********************************\n");
    LayerCollection::PrintEntities(vp,fp);
    fclose(fp);
    return True;
    }
  return False;
  }

//----------------------------------------------------------------------------

flag DXF_Drawing::CleanupDXF(pchar fni)
  {
  char fno[1024],*pf;
  char fnb[1024];

  strcpy(fno,fni);
  if( (pf = strstr(fno,".dxf")))
    *pf = NULL;
  else if( (pf = strstr(fno,".DXF")))
    *pf = NULL;
  else
    return False;
  strcpy(fnb,fno);
  strcat(fno,".new");
  strcat(fnb,".bac");


  char s1[1000];
  char s2[1000];
  FILE *fpi = fopen(fni,"r");
  FILE *fpo = fopen(fno,"w");


  if( !fpi || !fpo  )
    return False;

  // TODO: Add your command handler code here

  while( fgets(s1,1000,fpi) && fgets(s2,1000,fpi)  )
    {
    if( (atoi(s1) == 0) && strstr(s2,"POLYLINE") )
      {
      Strng_List l;
      int vert = 0;

      // load the polyline and count the verts
      l.Append(s1);
      l.Append(s2);
      while( fgets(s1,1000,fpi) && fgets(s2,1000,fpi)  )
        {
        if( (atoi(s1) == 0) && !strstr(s2,"SEQEND") && !strstr(s2,"VERTEX"))
          {
          break;
          }
        l.Append(s1);
        l.Append(s2);
        if( (atoi(s1) == 0) && strstr(s2,"VERTEX") )
          {
          vert++;
          }
        }
      if( vert >= 2 )
        {
        pStrng ps = l.First();
        while( ps )
          {
          (void)fprintf(fpo,"%s",ps->Str());
          //TRACE("%s",ps->Str());
          ps = l.Next();
          }
        }
      else
        {
        pStrng ps = l.First();
        while( ps )
          {
          TRACE("%s",ps->Str());
          ps = l.Next();
          }
        AfxMessageBox("Bad polyline deleted");
        }
      }
    (void)fprintf(fpo,"%s",s1);
    (void)fprintf(fpo,"%s",s2);
    }
  fclose(fpi);
  fclose(fpo);

  _unlink(fnb);
  rename(fni,fnb);
  rename(fno,fni);

  char Msg[1024];

  (void)sprintf(Msg,"Original Dxf file backed up as %s",fnb);
  AfxMessageBox(Msg);


  return True;
  }


void DXF_Drawing::EntityInvalidate(DXF_ENTITY e,C3_CURVE c)
  {
  Blocks.EntityInvalidate(e,c);
  LayerCollection::EntityInvalidate(e,c);
  }


void DXF_Drawing::SetDiscColor(DML_LIST llist,int on)
  {
  Blocks.SetDiscColor(llist,on);
  LayerCollection::SetDiscColor(llist,on);
  }


DML_LIST DXF_Drawing::SelectInsertsOnAttrCombo(DML_LIST l, pchar *AttrTags, pchar *AttrValues, IRegExpPtr *pRE)
  {
  return LayerCollection::SelectInsertsOnAttrCombo(l, AttrTags, AttrValues, pRE);
  }

DML_LIST DXF_Drawing::SelectNearbyCurves(DML_LIST l, PT3 p, REAL tol)
  {
  return LayerCollection::SelectNearbyCurves(l, p, tol);
  }

// In the attr values here a "String" indicates the actual value req for a match
// In the attr values here a "" the attr must have a value assigned
//                           (Char *)NULL indicates value is dont care

static char * ProPipeTags[] =
  {
  "FUNCTION",
    "INST_ID",
    "SEQUENCE",
    "NPS_MAIN",
    //  "ITEM",
    (pchar)NULL
  };

static char * ProPipeVals[] =
  {
  "",
    "",
    "",
    "",
    //  "INS",
    (pchar)NULL
  };

DML_LIST DXF_Drawing::SelectProPipeInstruments()
  {

  //DML_LIST l = dml_create_list();
  return SelectInsertsOnAttrCombo(NULL,ProPipeTags,ProPipeVals, NULL);
  }

DML_LIST DXF_Drawing::SelectNearbyCurves(PT3 pt,REAL tol)
  {
  DML_LIST l = dml_create_list();
  return SelectNearbyCurves(l,pt,tol);
  }

void DXF_Drawing::Draw(pViewport vp, /*ZoomTypes ZoomType, C3_BOX ZoomBoxIn,*/ C3_BOX SubRgn, pchar LayName)
  {
  DWORD CW=FPP_DisableExceptions();
#if dbgdxfdraw
  ////dbgfileflush(1);
#endif

  PT3 v1;
  REAL w;
  REAL h;
  vp3_get_ctr(vp->vp, v1);
  vpi_get_wh(vp->vp, &w, &h);

  C3_BOX_S ZoomBox;
  c3v_set(v1[0]+(C3_MIN_X(SubRgn)-0.5)*w, v1[1]+(C3_MIN_Y(SubRgn)-0.5)*h, -1e10,C3_MIN_PT(&ZoomBox));
  c3v_set(v1[0]+(C3_MAX_X(SubRgn)+0.5)*w, v1[1]+(C3_MAX_Y(SubRgn)+0.5)*h, 1e10,C3_MAX_PT(&ZoomBox));

  // dump_vp(vp);

  LayerCollection::Draw(vp, &ZoomBox, -1,1,NULL);

#if dbgdxfdraw
  //dbgpln("DXF_Drawing::Draw Mem Used [%li]", MemUsed());
  //dbgfileflush(0);
#endif
  FPP_RestoreExceptions(CW);
  }

//----------------------------------------------------------------------------

void DXF_Drawing::CountBlocksUsed(CWordArray& BlkCnt)
  {
  const int Cnt = Blocks.Count();
  BlkCnt.SetSize(Cnt);
  for (int i=0; i<Cnt; i++)
    BlkCnt[i] = 0;
  LayerCollection::CountBlocksUsed(BlkCnt);
  }

//----------------------------------------------------------------------------

int DXF_Drawing::RemoveUnusedBlocks()
  {
  CWordArray BlkCnt;
  CountBlocksUsed(BlkCnt);
  int DelCnt = 0;
  for (int i=BlkCnt.GetSize()-1; i>=0; i--)
    {
#if dbgdxf
    pBlock b = Blocks.FindByIndex(i);
    ASSERT(b->Def);
    dbgpln("%6i  %6i %s %s", i, BlkCnt[i], (BlkCnt[i]==0/* && !b->IsScd*/) ? "Del" : "   ", DXF_BLOCK_NAME_GET(b->Def));
#endif
    if (BlkCnt[i]==0)
      {
      pBlock b = Blocks.FindByIndex(i);
      //if (!b->IsScd)
        {
        b->SaveIt = 0;
        DelCnt++;
        }
      }
    }
  return DelCnt;
  }

//----------------------------------------------------------------------------

void DXF_Drawing::SetPageSize(PageSizes PageSizeIn)
  {
  PageWd= (float)(A0_PageWidth / pow(sqrt(2.0), (PageSizeIn-A0_Page)));
  }

//----------------------------------------------------------------------------

flag DXF_Drawing::GetBounds()
  {
  PT3 p;
  c3v_set(1.1e30, 1.1e30, 1.1e30, p);
  c3a_box_init_pt(&Bounds, p);

  C3_TRANSFORM c3t;// = c3t_create();

  c3t[0][0] = c3t[0][1] = c3t[0][2] = c3t[0][3] = 0.0;
  c3t[1][0] = c3t[1][1] = c3t[1][2] = c3t[1][3] = 0.0;
  c3t[2][0] = c3t[2][1] = c3t[2][2] = c3t[2][3] = 0.0;
  c3t[0][0] = c3t[1][1] = c3t[2][2] = 1.0;

  LayerCollection::GetBounds(c3t, &Bounds);
  //c3t_free(c3t);

  //double xc = (float)(0.5*(C3_MAX_X(&Bounds)+C3_MIN_X(&Bounds)));
  double xc = (0.5*(C3_MAX_X(&Bounds)+C3_MIN_X(&Bounds)));
  double yc = (0.5*(C3_MAX_Y(&Bounds)+C3_MIN_Y(&Bounds)));
  return (xc < 1.0e30 && yc < 1.0e30);
  }

//----------------------------------------------------------------------------

flag DXF_Drawing::GetBounds(C3_BOX TheBounds, DML_LIST TheCurveLst, DML_LIST TheInsertLst, DML_LIST TheOtherLst)
  {
  PT3 p;
  c3v_set(1.1e30, 1.1e30, 1.1e30, p);
  c3a_box_init_pt(TheBounds, p);

  C3_TRANSFORM c3t;// = c3t_create();

  c3t[0][0] = c3t[0][1] = c3t[0][2] = c3t[0][3] = 0.0;
  c3t[1][0] = c3t[1][1] = c3t[1][2] = c3t[1][3] = 0.0;
  c3t[2][0] = c3t[2][1] = c3t[2][2] = c3t[2][3] = 0.0;
  c3t[0][0] = c3t[1][1] = c3t[2][2] = 1.0;

  LayerCollection::GetBounds(c3t, TheBounds,TheCurveLst, TheInsertLst, TheOtherLst);
  //c3t_free(c3t);

  //double xc = (float)(0.5*(C3_MAX_X(&Bounds)+C3_MIN_X(&Bounds)));
  double xc = (0.5*(C3_MAX_X(TheBounds)+C3_MIN_X(TheBounds)));
  double yc = (0.5*(C3_MAX_Y(TheBounds)+C3_MIN_Y(TheBounds)));
  return (xc < 1.0e30 && yc < 1.0e30);
  }

//----------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::ClosestInsert(Pt_SLW pt)
  {
  REAL DistMin=1.0e30;
  return LayerCollection::ClosestInsert(pt.World.p(), DistMin);
  }

//----------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::ClosestEntity(Pt_SLW pt)
  {
  REAL DistMin=1.0e30;
  return LayerCollection::ClosestEntity(pt.World.p(), DistMin);
  }
//----------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::ClosestAttr(pchar AttrTag, Pt_SLW pt)
  {
  REAL DistMin=1.0e30;
  DXF_ENTITY e = ClosestInsert(pt);

  DML_LIST l1 = DXF_INSERT_ATTRS(e);
  DML_ITEM it1;
  DXF_ENTITY e1;
  WALK_ENTITY_LIST(l1, it1, e1)
    {
    //TRACE("Att '%s'",DXF_ATTRIB_TAG(e1));
    if (_stricmp(DXF_ATTRIB_TAG_GET(e1), AttrTag)==0)
      return e1;
    }
  return NULL;
  }

//----------------------------------------------------------------------------

pStrng_List DXF_Drawing::AttrValuesInWin(pchar AttrTag, PT3 p1, PT3 p2, pStrng_List &Lst)
  {
  //  pStrng_List Lst = NULL;
  PT3 mn, mx;
  c3v_set(Min(p1[0], p2[0]), Min(p1[1], p2[1]), Min(p1[2], p2[2]), mn);
  c3v_set(Max(p1[0], p2[0]), Max(p1[1], p2[1]), Max(p1[2], p2[2]), mx);
  LayerCollection::AttrValuesInWin(AttrTag, mn, mx, Lst);
  return Lst;
  }

//----------------------------------------------------------------------------

DXF_ENTITY First_Ent(DML_LIST lst, Entity_List_Info &Info)
  {
  Info.lst = lst;
  if (Info.lst == NULL) return NULL;
  Info.itm=DML_FIRST(Info.lst);
  return ( Info.itm ? (DXF_ENTITY)DML_RECORD(Info.itm): NULL);
  }

//----------------------------------------------------------------------------

DXF_ENTITY Next_Ent(Entity_List_Info &Info)
  {
  Info.itm=DML_NEXT(Info.itm);
  return ( Info.itm ? (DXF_ENTITY)DML_RECORD(Info.itm): NULL);
  }

//----------------------------------------------------------------------------

DXF_ENTITY Find_Attr(DXF_ENTITY Insert, pchar Tag)
  {
  Entity_List_Info Info;
  if( !DXF_ENTITY_IS_INSERT(Insert))
    return NULL;
  DXF_ENTITY Att = First_Ent(Attr_List(Insert), Info);
  while (Att)
    {
    if (_stricmp(Tag, Attr_Tag(Att)) == 0)
      return Att;
    Att = Next_Ent(Info);
    }
  return NULL;
  }

//----------------------------------------------------------------------------

void Remove_Attr(DXF_ENTITY Insert, pchar Tag)
  {
  DML_LIST attlist = DXF_INSERT_ATTRS(Insert);
  if (!attlist)
    return;

  DML_ITEM item;
  DXF_ENTITY a;
  flag Done = 0;
  while (!Done)
    {
    item=DML_FIRST(attlist);
    while (item)
      {
      a = (DXF_ENTITY)DML_RECORD(item);
      pchar ptag = DXF_ATTRIB_TAG_GET(a);
      if (ptag && _stricmp(ptag, Tag)==0)
        {
        dml_remove_item(attlist, item);
        dxf_free_entity(a);
        break;
        }
      item = DML_NEXT(item);
      if (item==NULL)
        Done = 1;
      }
    }

  if (dml_length(attlist)==0)
    {
    DXF_INSERT_ATTRS(Insert) = NULL;
    dml_free_list(attlist);
    }
  }

//----------------------------------------------------------------------------

flag Reset_Attr(DXF_ENTITY Insert, pchar Tag,pchar NewVal)
  {
  DXF_ENTITY Att = Find_Attr(Insert,Tag);
  if(Att)
    {
    DXF_ATTRIB_VALUE_SET(Att,NewVal); // mhmmem
    //        dx0_strass(&DXF_ATTRIB_VALUE(Att),NewVal); // mhmmem

    return True;
    }
  return False;
  }





pchar Find_Attr_Value(DXF_ENTITY Insert, pchar Tag)
  {
  DXF_ENTITY Att = Find_Attr(Insert,Tag);

  if( !Att )
    return (pchar)NULL;

  return DXF_ATTRIB_VALUE_GET(Att);
  }

int Find_Attr_AFlags(DXF_ENTITY Insert, pchar Tag)
  {
  DXF_ENTITY Att = Find_Attr(Insert,Tag);

  if( !Att )
    return 0;

  return DXF_ATTRIB_AFLAGS(Att);
  }

int Find_Attr_GFlags(DXF_ENTITY Insert, pchar Tag)
  {
  DXF_ENTITY Att = Find_Attr(Insert,Tag);

  if( !Att )
    return 0;

  return DXF_ATTRIB_GFLAGS(Att);
  }

//----------------------------------------------------------------------------
flag DXF_Drawing::FindCOGfromSelection(pViewport vp, Pt_3f &cg,Pt_3f &boxll,Pt_3f &boxur)
  {
  C3_BOX box;
  REAL *pos;
  PT3 p0,p1;

  c3v_set(0.0,0.0,0.0,p0);
  c3v_set(0.0,0.0,0.0,p1);
  box = c3a_box(p0,p1);

  int cnt = 0;
  CEntInView* p = vp->FirstSelectedEntity();

  if( !p )
    return 0;
  do
    {                                  // just use the curves for now
    if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
      {
      pos = DXF_TEXT_INS_PT(p->EntityPtr());
      if (cnt == 0)
        {
        box = c3a_box_init_pt(box,pos);
        }
      else
        {
        c3a_box_append_pt(box,pos);
        }
      cnt++;
      }
    else if (p->EntityPtr() && DXF_ENTITY_IS_ATTRIB(p->EntityPtr()))
      {
      pos = DXF_ATTRIB_PT(p->EntityPtr());
      if (cnt == 0)
        {
        c3a_box_init_pt(box,pos);
        }
      else
        {
        c3a_box_append_pt(box,pos);
        }
      cnt++;
      }
    else if (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
      {
      pos = DXF_INSERT_PT(p->EntityPtr());
      if (cnt == 0)
        {
        c3a_box_init_pt(box,pos);
        }
      else
        {
        c3a_box_append_pt(box,pos);
        }
      cnt++;
      }
    else if (p->CurvePtr())
      {
      if (cnt == 0)
        {
        PT3 b1,b2;
        b1[0] = C3_CURVE_X_MIN(p->CurvePtr());
        b1[1] = C3_CURVE_Y_MIN(p->CurvePtr());
        b1[2] = C3_CURVE_Z_MIN(p->CurvePtr());
        b2[0] = C3_CURVE_X_MAX(p->CurvePtr());
        b2[1] = C3_CURVE_Y_MAX(p->CurvePtr());
        b2[2] = C3_CURVE_Z_MAX(p->CurvePtr());
        c3a_box_init_pt(box,b1);
        c3a_box_append_pt(box,b2);
        }
      else
        {
        PT3 b1,b2;
        b1[0] = C3_CURVE_X_MIN(p->CurvePtr());
        b1[1] = C3_CURVE_Y_MIN(p->CurvePtr());
        b1[2] = C3_CURVE_Z_MIN(p->CurvePtr());
        b2[0] = C3_CURVE_X_MAX(p->CurvePtr());
        b2[1] = C3_CURVE_Y_MAX(p->CurvePtr());
        b2[2] = C3_CURVE_Z_MAX(p->CurvePtr());
        c3a_box_append_pt(box,b1);
        c3a_box_append_pt(box,b2);
        }
      cnt++;
      }
    else
      {
      dbgpln(" Centre of gravity requeseted for unknown entity\n");
      }
    }
  while( p = vp->NextSelectedEntity());
  if( !cnt )
    return 0;


  cg.Set((C3_MIN_X(box)+C3_MAX_X(box))/2.0,
    (C3_MIN_Y(box)+C3_MAX_Y(box))/2.0,
    (C3_MIN_Z(box)+C3_MAX_Z(box))/2.0);


  boxll.Set(C3_MIN_X(box),C3_MIN_Y(box),C3_MIN_Z(box));
  boxur.Set(C3_MAX_X(box),C3_MAX_Y(box),C3_MAX_Z(box));

  c3a_free_box(box);
  return 1;
  }

//---------------------------------------------------------------------------

pBlock DXF_Drawing::Add_Block(pchar BlockName, REAL *basept)
  {
#if dbgdxf
  dbgp("AddBlock %s", BlockName);
#endif

  DXF_BLOCK block = dx0_alloc_block ( dx0_file_release ( NULL ) );
  //      dx0_strcpy ( DXF_BLOCK_NAME(block), BlockName) ;
  DXF_BLOCK_NAME_SET(block, BlockName) ;

  DXF_BLOCK_LAYER_SET(block,CurLayer);

  //dx0_strcpy ( block->block_entity_header.layer, CurLayer) ;
  //dxf_set_entity_layer(block,CurLayer);

  dx0_copy_point ( basept, DXF_BLOCK_BASE_PT(block)) ;
  DXF_BLOCK_FLAGS(block) = 0;

#if dbgdxf
  dbgpln("");
#endif

  pBlock b = Blocks.Append(block);
  return b;
  }

//---------------------------------------------------------------------------

flag Block::Find_Attdef_Settings(pchar Tag, Attr_Settings &Set, Pt_3f &Pos)
  {
  DXF_ENTITY ad = Find_Attrib_Defn(Tag);
  if (ad)
    {
    Pos.Set(DXF_ATTDEF_PT(ad)[0],
      DXF_ATTDEF_PT(ad)[1],
      DXF_ATTDEF_PT(ad)[2]);
    Attr_Settings &Set = Attr_Settings (
      50,                              // Length ?
      DXF_ATTDEF_GFLAGS(ad),           // Flags
      DXF_ATTDEF_HEIGHT(ad),           // Size
      DXF_ATTDEF_X_SCALE(ad),          // Scale
      DXF_ATTDEF_ROT_ANG(ad),          // Rot
      DXF_ENTITY_COLOR(ad),            // Color
      Text_Cntrd,                      // Hjust
      Text_Middle,                     // Vjsut
      DXF_ATTDEF_STYLE_GET(ad)             // Style
      );

    return 1;
    }
  return 0;
  }


DXF_ENTITY DXF_Drawing::Add_Attr(DXF_ENTITY Insert, pchar Tag, pchar Prompt, pchar Value,
                                 Pt_3f Pt, Attr_Settings &Set)
  {
  pBlock b = Blocks.Find(DXF_INSERT_BLOCK_GET(Insert));

  if (b)
    {
    DXF_ENTITY ad = b->Find_Attrib_Defn(Tag);
    if (!ad)
      {
      //float X,Y;
      Pt_3f Ptt;
      ad = b->Add_Attrib_Defn(Tag, Prompt, "", Ptt.p(), Set);
      }
    return b->Add_Insert_Attrib(Insert, Tag, Value, Pt.p(), Set);
    }
  return NULL;
  }

//---------------------------------------------------------------------------
/**
DXF_ENTITY DXF_Drawing::Add_Text(pchar Txt, Pt_3f Pt, Text_Settings &Set)
{
DXF_ENTITY e = Layers.Add_Text(CurLayer, Txt, Pt.p(), Set);
GetBounds();
return e;
}
**/
//----------------------------------------------------------------------------

void DXF_Drawing::MergeLTypeDefs(DXF_FILE f)
  {

  DXF_TABLE_ENTRY LTypen, LType;
  DML_ITEM itn, it;
  DML_LIST NewLTypeLst = dxf_get_table_entries(f, DXF_LTYPE, NULL);

  DML_WALK_LIST(NewLTypeLst, itn)
    {
    LTypen = (DXF_TABLE_ENTRY)DML_RECORD(itn);
    LType = NULL;
    DML_WALK_LIST(LTypeLst, it)
      {
      LType = (DXF_TABLE_ENTRY)DML_RECORD(it);
      if (_stricmp(DXF_LTYPE_NAME_GET(LTypen), DXF_LTYPE_NAME_GET(LType)) == 0)
        break;
      }
    if (it)
      dxf_free_table_entry(LTypen);
    else
      dml_append_data(LTypeLst, LTypen);
    }

  dml_free_list(NewLTypeLst);
  }

//----------------------------------------------------------------------------

void DXF_Drawing::MergeLayerDefs(DXF_FILE f)
  {

  DXF_TABLE_ENTRY layern, layer;
  DML_ITEM itn, it;
  DML_LIST NewLayerLst = dxf_get_table_entries(f, DXF_LAYER, NULL);

  DML_WALK_LIST(NewLayerLst, itn)
    {
    layern = (DXF_TABLE_ENTRY)DML_RECORD(itn);
    layer = NULL;
    DML_WALK_LIST(LayerLst, it)
      {
      layer = (DXF_TABLE_ENTRY)DML_RECORD(it);
      if (_stricmp(DXF_LAYER_NAME_GET(layern), DXF_LAYER_NAME_GET(layer)) == 0)
        {
        //  start change  - let the incoming layer on off state have precedence
        // probably not necesary
        DXF_LAYER_FLAGS(layer) = DXF_LAYER_FLAGS(layern);
        //DXF_LAYER_LTYPE(layer) = DXF_LAYER_LTYPE(layern);
        //  end change DXF_LAYER_COLOR(layer) = DXF_LAYER_COLOR(layern);

        break;
        }
      }
    if (it)
      {
#if dbgCNM
      dbgpln("  %*s  Found  CGrfLayer %s",0," ",DXF_LAYER_NAME_GET(layern));
#endif
      dxf_free_table_entry(layern);
      }
    else
      {
#if dbgCNM
      dbgpln("  %*s  Append CGrfLayer %s",0," ",DXF_LAYER_NAME_GET(layern));
#endif
      dml_append_data(LayerLst, layern);
      }
    }

  dml_free_list(NewLayerLst);
  }

//----------------------------------------------------------------------------


void DXF_Drawing::AddLayerDef(pchar LayerName,int color,pchar font)
  {
  DXF_TABLE_ENTRY layer = dxf_create_layer(NULL,LayerName,(INT)color,(BOOLEAN)0,(STRING)font);
  dml_append_data(LayerLst,layer);
  }


DXF_TABLE_ENTRY DXF_Drawing::FindStyleDef(pchar StyleName)
  {
  DXF_TABLE_ENTRY style = NULL;
  DML_ITEM it;

  if( !StyleName )
    return NULL;


  DML_WALK_LIST(StyleLst, it)
    {
    style = (DXF_TABLE_ENTRY)DML_RECORD(it);
    if (_stricmp(DXF_STYLE_NAME_GET(style), StyleName) == 0)
      return style;
    }
  return NULL;
  }










DXF_TABLE_ENTRY DXF_Drawing::FindLayerDef(pchar LayerName)
  {
  DXF_TABLE_ENTRY layer = NULL;
  DML_ITEM it;

  if( !LayerName )
    return NULL;


  DML_WALK_LIST(LayerLst, it)
    {
    layer = (DXF_TABLE_ENTRY)DML_RECORD(it);
    if (_stricmp(DXF_LAYER_NAME_GET(layer), LayerName) == 0)
      return layer;
    }
  //AfxMessageBox(LayerName);


  if (! it)
    {
    int cnt = 0,Color;
    pchar pf;

    while( LayerNames[cnt] )
      {
      if( strcmp(LayerNames[cnt],LayerName) == 0)
        {
        break;
        }
      cnt++;
      }
    if( LayerNames[cnt] )
      {
      Color = LayerColors[cnt];
      pf    = LayerFonts[cnt];
      }
    else
      {
      Color = 1;
      pf    = "CONTINUOUS";
      }
    layer = dxf_create_layer(NULL,LayerName,(INT)Color,(BOOLEAN)0,(STRING)pf);
    dml_append_data(LayerLst,layer);
    }
  return layer;
  }

//----------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::First_Insert_In_Win(Pt_SLW &P1, Pt_SLW &P2, Entity_List_Info &Info)
  {
  c3a_box_init_pt(&Info.Box, P1.World.p());
  c3a_box_append_pt(&Info.Box, P2.World.p());
  DXF_ENTITY Ent=First_Insert(Info);
  while (Ent)
    {
    C3_BOX_S b;
    c3a_box_init_pt(&b, DXF_INSERT_PT(Ent));
    if (c3a_box_overlap(&b, &Info.Box)) return Ent;
    Ent=Next_Insert(Info);
    }
  return NULL;
  }

//----------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Next_Insert_In_Win(Entity_List_Info &Info)
  {
  DXF_ENTITY Ent=Next_Insert(Info);
  while (Ent)
    {
    C3_BOX_S b;
    c3a_box_init_pt(&b, DXF_INSERT_PT(Ent));
    if (c3a_box_overlap(&b, &Info.Box)) return Ent;
    Ent=Next_Insert(Info);
    }
  return NULL;
  }

//----------------------------------------------------------------------------

void DXF_Drawing::Get_Attr_Box(pchar s,Pt_3f Pos,Attr_Settings &Set,double *xmin,double *xmax,double *ymin,double *ymax)
  {
  double xa,xb,ya,yb;
  text_info ti;
  memset(&ti, 0, sizeof(text_info));
  ti.on        = 1;
  ti.pos       = Pos.px();
  ti.text      = s;
  ti.style     = Set.Style;
  ti.height    = Set.Size;
  ti.oblique   = 0.0;
  ti.angle     = Degs2Rads(Set.Rot);
  ti.wfactor   = Set.XScl;

  switch( Set.Hjust )
    {
    case Text_Left:  ti.horiz_org = 0;  break;
    case Text_Cntrd: ti.horiz_org = 1;  break;
    case Text_Right: ti.horiz_org = 2;  break;
    }

  switch( Set.Vjust )
    {
    case Text_Bottom:   ti.vert_org = 0;  break;
    case Text_Baseline: ti.vert_org = 1;  break;
    case Text_Middle:   ti.vert_org = 2;  break;
    case Text_Top:      ti.vert_org = 3;  break;
    case Text_Align:    ti.vert_org = 4;  break;
    case Text_Fit:      ti.vert_org = 5;  break;
    }

  scd_acad_text_corners(&ti, &xa,&ya,&xb,&yb);

  if( *xmin > xa ) *xmin = xa;
  if( *ymin > ya ) *ymin = ya;
  if( *xmax < xb ) *xmax = xb;
  if( *ymax < yb ) *ymax = yb;
  }

//----------------------------------------------------------------------------

void DXF_Drawing::Get_Text_Box(pchar s,Pt_3f Pos,Text_Settings &Set,double *xmin,double *xmax,double *ymin,double *ymax)
  {
  double xa,xb,ya,yb;
  text_info ti;
  memset(&ti, 0, sizeof(text_info));
  ti.on        = 1;
  ti.pos       = Pos.px();
  ti.text      = s;
  ti.style     = Set.Style;
  ti.height    = Set.Size;
  ti.oblique   = 0.0;
  ti.angle     = Degs2Rads(Set.Rot);
  ti.wfactor   = Set.XScl;

  switch( Set.Hjust )
    {
    case Text_Left:  ti.horiz_org = 0;  break;
    case Text_Cntrd: ti.horiz_org = 1;  break;
    case Text_Right: ti.horiz_org = 2;  break;
    }

  switch( Set.Vjust )
    {
    case Text_Bottom:   ti.vert_org = 0;  break;
    case Text_Baseline: ti.vert_org = 1;  break;
    case Text_Middle:   ti.vert_org = 2;  break;
    case Text_Top:      ti.vert_org = 3;  break;
    case Text_Align:    ti.vert_org = 4;  break;
    case Text_Fit:      ti.vert_org = 5;  break;
    }

  scd_acad_text_corners(&ti, &xa,&ya,&xb,&yb);

  if( *xmin > xa ) *xmin = xa;
  if( *ymin > ya ) *ymin = ya;
  if( *xmax < xb ) *xmax = xb;
  if( *ymax < yb ) *ymax = yb;
  }

//----------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Create_Fill_Block(CExecObj *pDb, DXF_ENTITY Insert, pchar ptag, pchar pvar, double Min, double Max, C3_CURVE Curve, Attr_Settings &ASet, flag ApplyToAll, int FillColour)
  {//add dynamic fill area
  // Check that one tag and insert is True
  if (Insert==NULL || ptag==NULL || strlen(ptag)==0)
    {
    LogError("GrfCmds", 0, "A tagged unit must be selected");
    return NULL;
    }
  pBlock b = Blocks.Find(DXF_INSERT_BLOCK_GET(Insert));
  if (!b)
    return NULL;
  if (!pvar || strlen(pvar)==0)
    {
    LogError("GrfCmds", 0, "A variable name must be is required");
    return NULL;
    }
  if (Curve!=NULL && C3_CURVE_PCURVE(Curve)->buffer.n <= 2)
    {
    LogError("GrfCmds", 0, "At least 3 points are required on the curve to create a fill area");
    return NULL;
    }

  // Check the tag.var is ok
  Strng WrkTag;
  WrkTag.Set("%s.%s", ptag, pvar);
  CXM_Route ObjRoute;
  CXM_ObjectTag ObjTag(WrkTag(), TABOpt_Parms);//TABOpt_AllInfo);
  CXM_ObjectData ObjData;
  if (!pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
    {
    LogError("GrfCmds", 0, "Tag '%s' not found.", WrkTag());
    return NULL;
    }

  if (Max<=Min)
    {
    LogWarning("GrfCmds", 0, "Max (%g) should be greater than Min (%g).", Min, Max);
    Max = Min + 1.0;
    }

  if (Curve)
    {//creating new dynamic fill...
    // want to add curve to the block and cause all inserts of this name to have this behaviour
    //TODO: BUG: If a dynamic fill allready existed for this block, how do you delete the curve associated with it???
    RemoveFromLayer(Curve);

    // construct
    C3_TRANSFORM Imat,Imat_i;
    ConstructInsertMatrix(Imat, b, Insert);
    c3t_inverse(Imat, Imat_i);
    c3c_transform(Curve, Imat_i);
    b->Add_Curve(Curve);
    b->TopOfList(Curve);
    }

  // now add the dynamics tags to the insert
  char varbuf[1024];
  char minbuf[32];
  char maxbuf[32];
  char colorbuf[32];
  sprintf(varbuf, "%s", pvar);
  sprintf(minbuf, "%g", Min);
  sprintf(maxbuf, "%g", Max);
  sprintf(colorbuf, "%i", FillColour);

  int cnt = 0;
  if (ApplyToAll)
    {// got through all the inserts in the drawing
    DML_ITEM item;
    DXF_ENTITY entity;
    CGrfLayer * pLay = Layers;
    while (pLay)
      {
      WALK_ENTITY_LIST(pLay->InsertLst, item, entity)
        {
        pchar eTag   = Find_Attr_Value(entity, TagAttribStr);
        pchar eBlock = DXF_INSERT_BLOCK_GET(entity);
        pchar iBlock = DXF_INSERT_BLOCK_GET(Insert);

        if (!eTag)
          continue;
        if (!eBlock)
          continue;
        if (!iBlock)
          continue;
        if (_stricmp(eBlock, iBlock))
          continue;

        //remove all attributes...
        Remove_Attr(entity, "FILLTAG");
        Remove_Attr(entity, "FILLVAR");
        Remove_Attr(entity, "FILLMIN");
        Remove_Attr(entity, "FILLMAX");
        Remove_Attr(entity, "FILLCOLOR");
        //add new attributes ...
        ASet.Flags |= DXF_ATTRIB_INVIS;
        Add_Insert_Attrib(entity, "FILLTAG", eTag, DXF_INSERT_PT(entity), ASet);
        Add_Insert_Attrib(entity, "FILLVAR", varbuf, DXF_INSERT_PT(entity), ASet);
        Add_Insert_Attrib(entity, "FILLMIN", minbuf, DXF_INSERT_PT(entity), ASet);
        Add_Insert_Attrib(entity, "FILLMAX", maxbuf, DXF_INSERT_PT(entity), ASet);
        Add_Insert_Attrib(entity, "FILLCOLOR", colorbuf, DXF_INSERT_PT(entity), ASet);
        ASet.Flags &= ~DXF_ATTRIB_INVIS;

        EntityInvalidate(entity, NULL);
        pDsp->Vp1->Draw(entity, -1);
        cnt++;
        }
      pLay = pLay->Nxt;
      }
    }
  else
    {
    pchar eTag = Find_Attr_Value(Insert, TagAttribStr);  ASSERT(eTag);
    //remove all attributes...
    Remove_Attr(Insert, "FILLTAG");
    Remove_Attr(Insert, "FILLVAR");
    Remove_Attr(Insert, "FILLMIN");
    Remove_Attr(Insert, "FILLMAX");
    Remove_Attr(Insert, "FILLCOLOR");
    //add new attributes ...
    ASet.Flags |= DXF_ATTRIB_INVIS;
    Add_Insert_Attrib(Insert, "FILLTAG", eTag, DXF_INSERT_PT(Insert), ASet);
    Add_Insert_Attrib(Insert, "FILLVAR", varbuf, DXF_INSERT_PT(Insert), ASet);
    Add_Insert_Attrib(Insert, "FILLMIN", minbuf, DXF_INSERT_PT(Insert), ASet);
    Add_Insert_Attrib(Insert, "FILLMAX", maxbuf, DXF_INSERT_PT(Insert), ASet);
    Add_Insert_Attrib(Insert, "FILLCOLOR", colorbuf, DXF_INSERT_PT(Insert), ASet);
    ASet.Flags &= ~DXF_ATTRIB_INVIS;

    EntityInvalidate(Insert, NULL);
    pDsp->Vp1->Draw(Insert, -1);
    cnt++;
    }

  if (cnt==0 && Curve)
    b->Delete(Curve);
  else if (Curve)
    LogNote("GrfCmds", 0, "Dynamic fill area added to %d entities", cnt);
  else
    LogNote("GrfCmds", 0, "Dynamic fill area options updated for %d entities", cnt);
  return Insert;
  }

//--------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Delete_Fill_Block(DXF_ENTITY Insert, flag ApplyToAll)
  {//delete dynamic fill area
  // Check that one tag and insert is True
  if (Insert==NULL)
    {
    LogError("GrfCmds", 0, "A tagged unit must be selected");
    return NULL;
    }

  pBlock b = Blocks.Find(DXF_INSERT_BLOCK_GET(Insert));
  if (!b)
    return NULL;

  int cnt = 0;
  if (ApplyToAll)
    {// got through all the inserts in the drawing
    DML_ITEM item;
    DXF_ENTITY entity;
    CGrfLayer * pLay = Layers;
    while (pLay)
      {
      WALK_ENTITY_LIST(pLay->InsertLst, item, entity)
        {
        pchar eTag   = Find_Attr_Value(entity, TagAttribStr);
        pchar eBlock = DXF_INSERT_BLOCK_GET(entity);
        pchar iBlock = DXF_INSERT_BLOCK_GET(Insert);

        if (!eTag)
          continue;
        if (!eBlock)
          continue;
        if (!iBlock)
          continue;
        if (_stricmp(eBlock, iBlock))
          continue;

        //remove all attributes...
        Remove_Attr(entity, "FILLTAG");
        Remove_Attr(entity, "FILLVAR");
        Remove_Attr(entity, "FILLMIN");
        Remove_Attr(entity, "FILLMAX");
        Remove_Attr(entity, "FILLCOLOR");

        EntityInvalidate(entity, NULL);
        pDsp->Vp1->Draw(entity, -1);
        cnt++;
        }
      pLay = pLay->Nxt;
      }
    }
  else
    {
    //remove all attributes...
    Remove_Attr(Insert, "FILLTAG");
    Remove_Attr(Insert, "FILLVAR");
    Remove_Attr(Insert, "FILLMIN");
    Remove_Attr(Insert, "FILLMAX");
    Remove_Attr(Insert, "FILLCOLOR");

    EntityInvalidate(Insert, NULL);
    pDsp->Vp1->Draw(Insert, -1);
    cnt++;
    }

  LogNote("GrfCmds", 0, "Dynamic fill area removed from %d entities", cnt);
  return Insert;
  }

//--------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Create_Bars_Block(CExecObj *pDb, Strng_List &Vars, Strng_List &Tags, Strng_List &Mins, Strng_List &Maxs, Strng_List &Cols, Pt_3f Pos, Attr_Settings &ASet, Text_Settings &TSet, long DisplayOptions, flag NewInsert)
  {//add dynamic bars
  Strng tagbuf,varbuf,minbuf,maxbuf,colbuf,optbuf;
  Strng_List Taglist,Varlist,Minlist,Maxlist,Collist;
  pStrng ptmp,vtmp,ttmp,mntmp,mxtmp,cltmp;
  char Buff[2048];
  char DefCol[16];
  sprintf(DefCol, "%d", DEFAULTBARSCOLOUR);
  if (NewInsert)
    {
    // remove all duplicate Tags and blanks...
    for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
      {
      if (ptmp->Str() && ptmp->Length()>0)
        {
        pStrng pFound = Tags.FindFrom(ptmp->pNxt, ptmp->Str());
        if (pFound)
          Tags.Remove(ptmp);
        }
      else if (ptmp->Str() && ptmp->Length()==0)
        Tags.Remove(ptmp);
      }
    // Check all the Mentioned Tags are findable and delete if not
    for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
      {
      if (ptmp->Str())
        {
        CXM_Route ObjRoute;
        CXM_ObjectTag ObjTag(ptmp->Str(), TABOpt_Parms);//TABOpt_AllInfo);
        CXM_ObjectData ObjData;
        if (!pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
          {
          LogWarning("GrfCmds", 0, "Tag %s not found. This will be excluded", ptmp->Str());
          Tags.Remove(ptmp);
          }
        }
      }
    if (Tags.Length()==0)
      {
      LogError("GrfCmds", 0, "No valid Tags");
      return NULL;
      }
    // remove all duplicate variables and blanks...
    for (ptmp = Vars.First(); ptmp; ptmp = Vars.Next())
      {
      if (ptmp->Str() && ptmp->Length()>0)
        {
        pStrng pFound = Vars.FindFrom(ptmp->pNxt, ptmp->Str());
        if (pFound)
          Vars.Remove(ptmp);
        }
      else if (ptmp->Str() && ptmp->Length()==0)
        Vars.Remove(ptmp);
      }
    if (Vars.Length()==0)
      {
      LogError("GrfCmds", 0, "No valid variables");
      return NULL;
      }

    Strng Min;
    if (Mins.Length()>0)
      Min = Mins.First()->Str();
    else
      Min = "0";
    Strng Max;
    if (Maxs.Length()>0)
      Max = Maxs.First()->Str();
    else
      Max = "100";
    Strng Col;
    if (Cols.Length()>0)
      Col = Cols.First()->Str();
    else
      Col = DefCol;
    for (vtmp = Vars.First(); vtmp; vtmp = Vars.Next())
      {
      char* pVar = vtmp->Str();
      for (ttmp = Tags.First(); ttmp; ttmp = Tags.Next())
        {
        char* pTag = ttmp->Str();
        sprintf(Buff, "%s.%s", pTag, pVar);
        CXM_Route ObjRoute;
        CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
        CXM_ObjectData ObjData;
        if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
          {
          CPkDataItem* pPkDI = ObjData.FirstItem();
          Taglist.Append(pTag);
          if (pPkDI->CnvTxt())
            sprintf(Buff, "%s(%s)", pVar, pPkDI->CnvTxt());
          else
            sprintf(Buff, "%s", pVar);
          Varlist.Append(Buff);
          Minlist.Append(Min());
          Maxlist.Append(Max());
          Collist.Append(Col());
          varbuf += pVar;
          varbuf += ' ';
          }
        }
      }
    }
  else
    {
    const int Len = Tags.Length();
    if (Cols.Length()<Len)
      {
      for (int i=Cols.Length(); i<Len; i++)
        Cols.Append(DefCol);
      }
    if (Len==0 || Vars.Length()!=Len || Mins.Length()!=Len || Maxs.Length()!=Len || Cols.Length()!=Len)
      {
      LogError("GrfCmds", 0, "The same number of tags, vars, Mins, Maxs and Cols expected");
      return NULL;
      }
    vtmp = Vars.First();
    ttmp = Tags.First();
    mntmp = Mins.First();
    mxtmp = Maxs.First();
    cltmp = Cols.First();
    while (ttmp)
      {
      char* pTag = ttmp->Str();
      char* pVar = vtmp->Str();
      sprintf(Buff, "%s.%s", pTag, pVar);
      CXM_Route ObjRoute;
      CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        CPkDataItem* pPkDI = ObjData.FirstItem();
        Taglist.Append(pTag);
        if (pPkDI->CnvTxt())
          sprintf(Buff, "%s(%s)", pVar, pPkDI->CnvTxt());
        else
          sprintf(Buff, "%s", pVar);
        Varlist.Append(Buff);
        Minlist.Append(mntmp->Str());
        Maxlist.Append(mxtmp->Str());
        Collist.Append(cltmp->Str());
        varbuf += pVar;
        varbuf += ' ';
        }
      else
        LogWarning("GrfCmds", 0, "Tag %s.%s not found. This will be excluded", pTag, pVar);
      ttmp = Tags.Next();
      vtmp = Vars.Next();
      mntmp = Mins.Next();
      mxtmp = Maxs.Next();
      cltmp = Cols.Next();
      }
    }

  const int ntag = Taglist.Length();
  if (ntag==0)
    {
    LogError("GrfCmds", 0, "No valid Tag.Variable combinations found");
    return NULL;
    }

  CGrfLayer * l = Find(Dynamic_Layer);
  if (!l)
    l = Append(Drawing->FindLayerDef(Dynamic_Layer));
  if (!l)
    return NULL;


  Taglist.BuildTokString(tagbuf, " ");
  Minlist.BuildTokString(minbuf, " ");
  Maxlist.BuildTokString(maxbuf, " ");
  Collist.BuildTokString(colbuf, " ");

  double xmin,xmax,ymin,ymax;
  const bool WithLabels = (DisplayOptions==0);
  optbuf.Set("%d", DisplayOptions);
  if (WithLabels)
    {
    // for each tag find the max width and height
    xmin = 1.0e30;
    xmax = -1.0e30;
    ymin = 1.0e30;
    ymax = -1.0e30;
    TSet.Vjust = Text_Middle;
    ASet.Vjust = Text_Middle;
    TSet.Hjust = Text_Cntrd;
    ASet.Hjust = Text_Cntrd;
    Pt_3f Pt;
    ttmp = Taglist.First();
    vtmp = Varlist.First();
    mntmp = Minlist.First();
    mxtmp = Maxlist.First();
    cltmp = Collist.First();
    while (ttmp)
      {
      sprintf(Buff, " %s ", ttmp->Str());
      Get_Text_Box(Buff, Pt, TSet, &xmin, &xmax, &ymin, &ymax);
      sprintf(Buff, " %s ", vtmp->Str());
      Get_Text_Box(Buff, Pt, TSet, &xmin, &xmax, &ymin, &ymax);
      sprintf(Buff, " %s ", mntmp->Str());
      Get_Text_Box(Buff, Pt, TSet, &xmin, &xmax, &ymin, &ymax);
      sprintf(Buff, " %s ", mxtmp->Str());
      Get_Text_Box(Buff, Pt, TSet, &xmin, &xmax, &ymin, &ymax);

      ttmp = Taglist.Next();
      vtmp = Varlist.Next();
      mntmp = Minlist.Next();
      mxtmp = Maxlist.Next();
      cltmp = Collist.Next();
      }
    }
  else
    {
    xmin = 0.0;
    xmax = 6.0*TSet.Size;
    ymin = 0.0;
    ymax = 1.2*TSet.Size;
    }

  SetCurrentLayer(Dynamic_Layer, "");

  DXF_ENTITY ins = (DXF_ENTITY)NULL;

  int num = Blocks.FindUnique("DYNBLK_", 1);
  char name[128];
  sprintf(name, "DYNBLK_%d", num);

  Pt_3f zero;
  pBlock pDynblk = Add_Block(name, zero.p());
  pDynblk->SetCurrentLayer(Dynamic_Layer, "");

  double wdt = (xmax-xmin);
  double hgt = (ymax-ymin)*1.8;
  if (WithLabels)
    {
    const double spc = TSet.Size * 0.2;
    double x0,y0,h0,w0;
    for (int i=0; i<ntag; i++)
      {
      y0 = hgt * 3.0;  x0 = wdt * i;  w0 = wdt;  h0 = 15.0*hgt;
      SetCurveColor(pDynblk->Add_Square(x0+5.0*spc, y0+spc, w0-10.0*spc, h0-2.0*spc), -1);
      }

    y0 = 0.0;  x0 = 0.0;  w0 = wdt*ntag;  h0 = 19.0*hgt;
    SetCurveColor(pDynblk->Add_Square(x0, y0, w0, h0), -1);

    Pt_3f loc;
    PT3 pnt;
    pchar CurTag = Taglist.First()->Str();
    pchar CurVar = Varlist.First()->Str();
    pchar CurMax = Maxlist.First()->Str();
    pchar CurMin = Minlist.First()->Str();
    for (i=0; i<ntag; i++)
      {
      //TRACE(" Add Dynamic Current Column   %s %s %s %s\n",CurTag,CurVar,CurMin,CurMax);
      y0 = hgt * 0.0;  x0 = wdt * i;  w0 = wdt;  h0 = hgt;
      pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
      SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)), -1);
      dxf_set_entity_color(pDynblk->Add_Text(CurTag, pnt, TSet), -1);

      y0 = hgt * 1.0;  x0 = wdt * i;  w0 = wdt;  h0 = hgt;
      pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
      SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)), -1);
      dxf_set_entity_color(pDynblk->Add_Text(CurVar, pnt, TSet), -1);

      y0 = hgt * 2.0;  x0 = wdt * i;  w0 = wdt;  h0 = hgt;
      pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
      SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)), -1);
      dxf_set_entity_color(pDynblk->Add_Text(CurMin, pnt, TSet), -1);

      y0 = hgt * 18.0;  x0 = wdt * i;  w0 = wdt;  h0 = hgt;
      pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
      SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)), -1);
      dxf_set_entity_color(pDynblk->Add_Text(CurMax, pnt, TSet), -1);

      if (ptmp = Taglist.Next())
        CurTag = ptmp->Str();
      if (ptmp = Varlist.Next())
        CurVar = ptmp->Str();
      if (ptmp = Maxlist.Next())
        CurMax = ptmp->Str();
      if (ptmp = Minlist.Next())
        CurMin = ptmp->Str();
      }
    }
  else
    {
    double x0,y0,h0,w0;
    const double spc = 0.2;
    for (int i=0; i<ntag; i++)
      {
      y0 = hgt*0.5;  x0 = wdt * i;  w0 = wdt;  h0 = 19.0*hgt;
      SetCurveColor(pDynblk->Add_Square(x0+4.0*spc, y0+spc, w0-8.0*spc, h0-2.0*spc), -1);
      }

    y0 = 0.0;  x0 = 0.0;  w0 = wdt*ntag;  h0 = 20.0*hgt;
    SetCurveColor(pDynblk->Add_Square(x0, y0, w0, h0), -1);
    }

  Pt_3f Scl(1.0, 1.0, 1.0);
  if (!(ins = Create_Insert(name, Pos, -1, Scl)))
    return(ins);

  dxf_set_entity_color(ins, -1);

  ASet.Flags |= DXF_ATTRIB_INVIS;
  Add_Insert_Attrib(ins, "DYNMTAGS", tagbuf(), Pos.p(), ASet);
  Add_Insert_Attrib(ins, "DYNMVARS", varbuf(), Pos.p(), ASet);
  Add_Insert_Attrib(ins, "DYNMMINS", minbuf(), Pos.p(), ASet);
  Add_Insert_Attrib(ins, "DYNMMAXS", maxbuf(), Pos.p(), ASet);
  Add_Insert_Attrib(ins, "DYNMCOLOR", colbuf(), Pos.p(), ASet);
  Add_Insert_Attrib(ins, "DYNMOPTS", optbuf(), Pos.p(), ASet);
  ASet.Flags &= ~DXF_ATTRIB_INVIS;
  return ins;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Create_Toggle_Block(CExecObj *pDb, Strng_List &Vars, Strng_List &Tags, Pt_3f Pos, Attr_Settings &ASet, Text_Settings &TSet, double Scale)
  {
  char Buff[1024],*pTag,*pVar;
  pStrng ptmp,ttmp,vtmp;

  // remove all duplicate Tags and blanks...
  for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
    {
    if (ptmp->Str() && ptmp->Length()>0)
      {
      pStrng pFound = Tags.FindFrom(ptmp->pNxt, ptmp->Str());
      if (pFound)
        Tags.Remove(ptmp);
      }
    else if (ptmp->Str() && ptmp->Length()==0)
      Tags.Remove(ptmp);
    }
  // Check all the Mentioned Tags are findable and delete if not
  for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
    {
    if (ptmp->Str())
      {
      CXM_Route ObjRoute;
      CXM_ObjectTag ObjTag(ptmp->Str(), TABOpt_Parms);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      if (!pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        LogWarning("GrfCmds", 0, "Tag %s not found. This will be excluded", ptmp->Str());
        Tags.Remove(ptmp);
        }
      }
    }
  if (Tags.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid Tags");
    return NULL;
    }

  // remove all duplicate variables and blanks...
  for (ptmp = Vars.First(); ptmp; ptmp = Vars.Next())
    {
    if (ptmp->Str() && ptmp->Length()>0)
      {
      pStrng pFound = Vars.FindFrom(ptmp->pNxt, ptmp->Str());
      if (pFound)
        Vars.Remove(ptmp);
      }
    else if (ptmp->Str() && ptmp->Length()==0)
      Vars.Remove(ptmp);
    }
  if (Vars.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid variables");
    return NULL;
    }

  Strng_List Taglist,Varlist;
  Pt_3f Scl;
  Strng_List ValAttr;
  //pStrng ptmp,vtmp,ttmp,mintmp,maxtmp;//,ktmp;
  Pt_3f Pt;
  double xmin = 1.0e30,xmax = -1.0e30;
  double ymin = 1.0e30,ymax = -1.0e30;
  Strng_List tcol;
  char tagbuf[4096],varbuf[4096];

  TSet.Vjust = Text_Middle;
  ASet.Vjust = Text_Middle;
  TSet.Hjust = Text_Cntrd;
  ASet.Hjust = Text_Cntrd;
  Pt.Set(0,0,0);

  // foreach var and then tag find the max width and height
  strcpy(tagbuf," ");
  strcpy(varbuf," ");

  for (vtmp = Vars.First(); vtmp; vtmp = Vars.Next())
    {
    pVar = vtmp->Str();
    for (ttmp = Tags.First(); ttmp; ttmp = Tags.Next())
      {
      pTag = ttmp->Str();
      sprintf(Buff, "%s.%s", pTag, pVar);
      CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
      CXM_Route ObjRoute;
      CXM_ObjectData ObjData;
      if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        CPkDataItem* pPkDI = ObjData.FirstItem();
        if (pPkDI->Type()==tt_Bool || pPkDI->Type()==tt_Bit)
          {
          sprintf(Buff, " %s ", pTag); Taglist.Append(Buff);
          Get_Text_Box(Buff, Pt, TSet, &xmin, &xmax, &ymin, &ymax);
          sprintf(Buff," %s ", pVar); Varlist.Append(Buff);
          Get_Text_Box(Buff, Pt, TSet, &xmin, &xmax, &ymin, &ymax);
          //ValAttr.Append(Buff);

          strcat(tagbuf, pTag);
          strcat(varbuf, pVar);
          strcat(tagbuf, " ");
          strcat(varbuf, " ");
          }
        else
          LogWarning("GrfCmds", 0, "Tag %s.%s data type is incorrect", pTag, pVar);
        }
      else
        LogWarning("GrfCmds", 0, "Tag %s.%s not found. This will be excluded", pTag, pVar);
      }
    }

  const int ntag = Taglist.Length();
  if (!ntag)
    {
    LogError("GrfCmds", 0, "No valid Tag.Variable combinations found");
    return NULL;
    }
  pBlock pDynblk;
  CGrfLayer * l = Find(Dynamic_Layer);
  if (!l)
    l = Append(Drawing->FindLayerDef(Dynamic_Layer));
  if (!l)
    return NULL;

  SetCurrentLayer(Dynamic_Layer, "");
  int num = Blocks.FindUnique("DYNBLK_", 1);
  char name[1024];
  sprintf(name, "DYNBLK_%d", num);

  Pt_3f zero(0.0, 0.0, 0.0);
  pDynblk = Add_Block(name, zero.p());
  pDynblk->SetCurrentLayer(Dynamic_Layer, "");

  double wdt = xmax-xmin;
  double hgt = (ymax-ymin)*1.8;
  double spc = TSet.Size * 0.2;
  double x0,y0,h0,w0;
  Pt_3f loc;
  PT3 pnt;
  for (int i=0; i<ntag; i++)
    {
    y0 = hgt *  1.0;  x0 = wdt * i;  w0 = wdt; h0 =      wdt;
    pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
    double cx = (wdt * i ) + wdt /2.0;
    double cy = hgt + wdt /2.0;
    double cd = 0.9 * wdt/2.0;
    Pt_3f p;
    C3_CURVE c;
    for( int j = 0; j < 13; j++ )
      {
      double a = j*30.0*3.141/180.0;
      double x = cx + cd*sin(a);
      double y = cy + cd*cos(a);
      p.Set(x, y, 0.0);
      if (j==0)
        c = pDynblk->Add_PLine_Start(p.p());
      else
        pDynblk->Add_PLine_Vertex(c, p.p());
      }
    SetCurveColor(c,-1);
    }

  y0 = 0.0;  x0 = 0.0;  w0 = wdt*ntag; h0 = 2.0*hgt + wdt;
  SetCurveColor(pDynblk->Add_Square(x0,y0,w0,h0),-1);

  pchar CurTag = Taglist.First()->Str();
  pchar CurVar = Varlist.First()->Str();
  for (i=0; i<ntag; i++)
    {
    y0 = hgt *  0.0;  x0 = wdt * i;  w0 = wdt; h0 =      hgt;
    pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
    SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)),-1);
    dxf_set_entity_color(pDynblk->Add_Text(CurTag,pnt,TSet),-1);

    y0 = hgt *  1.0;  x0 = wdt * i;  w0 = wdt; h0 =      wdt;
    pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
    SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)),-1);

    y0 = hgt *  1.0 + wdt;  x0 = wdt * i;  w0 = wdt; h0 =      hgt;
    pnt[0] = x0+w0/2.0;  pnt[1] = y0+h0/2.0;  pnt[2] = 0.0;
    SetCurveColor(pDynblk->Add_Square(x0+spc,y0+spc,w0-(2.0*spc),h0-(2.0*spc)),-1);
    dxf_set_entity_color(pDynblk->Add_Text(CurVar,pnt,TSet),-1);

    pStrng ptmp;
    if (ptmp = Taglist.Next())
      CurTag = ptmp->Str();
    if (ptmp = Varlist.Next())
      CurVar = ptmp->Str();
    }

  Scl.Set(1.0, 1.0, 1.0);
  DXF_ENTITY ins = Create_Insert(name, Pos, -1, Scl);
  if (ins==NULL)
    return NULL;

  dxf_set_entity_color(ins, -1);

  ASet.Flags |= DXF_ATTRIB_INVIS;
  Add_Insert_Attrib(ins, "TOGGTAGS", tagbuf, Pos.p(), ASet);
  Add_Insert_Attrib(ins, "TOGGVARS", varbuf, Pos.p(), ASet);
  ASet.Flags &= ~DXF_ATTRIB_INVIS;
  return ins;
  }

//---------------------------------------------------------------------------

flag DXF_Drawing::ChangeTag_NewAnnotation_Block(pchar pOldTag, pchar pNewTag)
  {//change tag for annotation blocks...
  flag AnyTagAnnotChngd = 0;
  static char* aval[] = { "ANNOT_TAGS", "ANNOT_VARS", (pchar)NULL };
  static char* bval[] = { "", "", (pchar)NULL };
  pDsp->Open();
  pDsp->Vp1->ClearAllEntity();
  SelectInsertsOnAttrCombo(NULL, aval, bval, NULL);
  CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
  while (pEnt)
    {
    DXF_ENTITY OldIns = pEnt->EntityPtr();
    pchar pTag = Find_Attr_Value(OldIns, "ANNOT_TAGS");
    pchar pTLb = Find_Attr_Value(OldIns, "ANNOT_TAGLBLS");
    if (pTag && pTLb)
      {
      Strng_List Tags,TLbs;
      pStrng ptmp,ktmp;
      pchar p;
      p = pTag; p = strtok(p, " "); while (p) { Tags.Append(p); p = strtok(NULL, " "); }
    p = pTLb; p = strtok(p, " "); while (p) { TLbs.Append(p); p = strtok(NULL, " "); }
  ASSERT(Tags.Length()==TLbs.Length());
  for (ptmp = Tags.First(), ktmp = TLbs.First(); ptmp; ptmp = Tags.Next(), ktmp = TLbs.Next())
    {
    if (ptmp->Str() && ptmp->Len()>0 && _stricmp(pOldTag, ptmp->Str())==0)
      {
      AnyTagAnnotChngd = 1;
      ptmp->Set("%s", pNewTag);
      if (ktmp->Str() && ktmp->Len()>0 && _stricmp(pOldTag, ktmp->Str())==0)
        {
        ktmp->Set("%s", pNewTag); //change the label iif it was the same as the tag
        }
      }
    }

  REAL* pos = DXF_INSERT_PT(OldIns);
  double Size = DXF_ATTRIB_HEIGHT(Find_Attr(OldIns, "ANNOT_TAGS"));
  Attr_Settings AAtt = Attr_Settings(50, 0, Size, 0.8, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
  AAtt.Flags |= DXF_ATTRIB_INVIS;
  char Buf[8192];
  Buf[0] = 0;
  for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
    {
    strcat(Buf, " ");
    strcat(Buf, ptmp->Str());
    }
  Remove_Attr(OldIns, "ANNOT_TAGS");
  DXF_ENTITY t = Add_Insert_Attrib(OldIns, "ANNOT_TAGS", Buf, pos, AAtt);
  Buf[0] = 0;
  for (ptmp = TLbs.First(); ptmp; ptmp = TLbs.Next())
    {
    strcat(Buf, " ");
    strcat(Buf, ptmp->Str());
    }
  Remove_Attr(OldIns, "ANNOT_TAGLBLS");
  Add_Insert_Attrib(OldIns, "ANNOT_TAGLBLS", Buf, pos, AAtt);
  AAtt.Flags &= ~DXF_ATTRIB_INVIS;
  //EntityInvalidate(OldIns, NULL);
  //pDsp->Draw(OldIns, -1);
      }
    pEnt = pDsp->Vp1->NextSelectedEntity();
    }
  pDsp->Vp1->ClearAllEntity();
  pDsp->Close();
  return AnyTagAnnotChngd;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Update_NewAnnotation_Block(DXF_ENTITY e, CExecObj *pDb, Attr_Settings &Set, double Scale)
  {
  pchar pTag = Find_Attr_Value(e, "ANNOT_TAGS");
  pchar pTLb = Find_Attr_Value(e, "ANNOT_TAGLBLS");
  pchar pVar = Find_Attr_Value(e, "ANNOT_VARS");
  pchar pVLb = Find_Attr_Value(e, "ANNOT_VARLBLS");
  pchar pFmt = Find_Attr_Value(e, "ANNOT_TAGFMTS");
  pchar pCvs = Find_Attr_Value(e, "ANNOT_TAGCNVS");
  if (!pVar || !pVLb || !pTag || !pTLb || !pFmt || !pCvs)
    return NULL;

  Strng_List VarLst,VLbLst,TagLst,TLbLst,FmtLst,CvsLst;
  pchar p;
  p = pTag; p = strtok(p, " "); while (p) { TagLst.Append(p); p = strtok(NULL, " "); }
p = pTLb; p = strtok(p, " "); while (p) { TLbLst.Append(p); p = strtok(NULL, " "); }
p = pVar; p = strtok(p, " "); while (p) { VarLst.Append(p); p = strtok(NULL, " "); }
p = pVLb; p = strtok(p, " "); while (p) { VLbLst.Append(p); p = strtok(NULL, " "); }
p = pFmt; p = strtok(p, " "); while (p) { FmtLst.Append(p); p = strtok(NULL, " "); }
p = pCvs; p = strtok(p, " "); while (p) { CvsLst.Append(p); p = strtok(NULL, " "); }
//double Scale = DXF_INSERT_X_SCALE(e);
REAL *d = DXF_INSERT_PT(e);
Pt_3f Pos(d[0], d[1], d[2]);
return Create_NewAnnotation_Block(pDb, TagLst, TLbLst, VarLst, VLbLst, FmtLst, CvsLst, Pos, Set, Scale);
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Create_NewAnnotation_Block(CExecObj *pDb, Strng_List &Tags, Strng_List &TLbs, Strng_List &Vars, Strng_List &VLbs,
                                                   Strng_List &Fmts, Strng_List &Cvss, Pt_3f Pos, Attr_Settings &Set, double Scale)
  {
  if (Vars.Length()!=VLbs.Length() || Vars.Length()!=Fmts.Length() || Vars.Length()!=Cvss.Length())
    {
    //LogError("GrfCmds", 0, "Number of variable name != format names - aborting command");
    LogError("GrfCmds", 0, "Variable, variable-labels, format and engineering-unit lists should match");
    return NULL;
    }

  ASSERT(Tags.Length()==TLbs.Length());
  ASSERT(Vars.Length()==VLbs.Length() && Vars.Length()==Fmts.Length() && Vars.Length()==Cvss.Length());
  char Buff[2048],*pTag,*pTLb,*pVar,*pVLb,*pFmt,*pCvs;
  pStrng ptmp,ktmp,ttmp,vtmp,ftmp;

  // remove all duplicate Tags and blanks...
  for (ptmp = Tags.First(), ktmp = TLbs.First(); ptmp; ptmp = Tags.Next(), ktmp = TLbs.Next())
    {
    if (ptmp->Str() && ptmp->Length()>0)
      {
      pStrng pFound = Tags.FindFrom(ptmp->pNxt, ptmp->Str());
      if (pFound)
        {
        Tags.Remove(ptmp);
        TLbs.Remove(ktmp);
        }
      }
    else if (ptmp->Str() && ptmp->Length()==0)
      {
      Tags.Remove(ptmp);
      TLbs.Remove(ktmp);
      }
    }
  // Check all the Mentioned Tags are findable and delete if not
  for (ptmp = Tags.First(), ktmp = TLbs.First(); ptmp; ptmp = Tags.Next(), ktmp = TLbs.Next())
    {
    if (ptmp->Str())
      {
      CXM_Route ObjRoute;
      CXM_ObjectTag ObjTag(ptmp->Str(), TABOpt_Parms);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      if (!pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        LogWarning("GrfCmds", 0, "Tag %s not found. This will be excluded", ptmp->Str());
        Tags.Remove(ptmp);
        TLbs.Remove(ktmp);
        }
      }
    }
  if (Tags.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid Tags");
    return NULL;
    }

  // remove all duplicate variables and blanks...
  for (ptmp = Vars.First(), ktmp = VLbs.First(), ttmp = Fmts.First(), vtmp = Cvss.First(); ptmp; ptmp = Vars.Next(), ktmp = VLbs.Next(), ttmp = Fmts.Next(), vtmp = Cvss.Next())
    {
    if (ptmp->Str() && ptmp->Length()>0)
      {
      pStrng pFound = Vars.FindFrom(ptmp->pNxt, ptmp->Str());
      if (pFound)
        {
        Vars.Remove(ptmp);
        VLbs.Remove(ktmp);
        Fmts.Remove(ttmp);
        Cvss.Remove(vtmp);
        }
      }
    else if (ptmp->Str() && ptmp->Length()==0)
      {
      Vars.Remove(ptmp);
      VLbs.Remove(ktmp);
      Fmts.Remove(ttmp);
      Cvss.Remove(vtmp);
      }
    }
  if (Vars.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid variables");
    return NULL;
    }

  Pt_3f Scl;
  Strng_List ValAttr;
  Pt_3f Pt(0.0, 0.0, 0.0);
  double xmin_tit = 1.0e30,xmax_tit = -1.0e30;
  double xmin_col = 1.0e30,xmax_col = -1.0e30;
  double ymin = 1.0e30,ymax = -1.0e30;
  Set.Vjust = Text_Middle;

  //Make a string list of the title columns (titles down left side)...
  //Foreach Var parse tags until found else exclude from list
  ValAttr.Append("   ");// for the top left blank box
  ktmp = VLbs.First();
  ftmp = Fmts.First();
  ptmp = Cvss.First();
  for (vtmp = Vars.First(), pVar = vtmp->Str(); pVar; (vtmp = Vars.Next()) ? (pVar = vtmp->Str()) : (pVar = NULL))
    {
    pCvs = ptmp->Str();
    pVLb = ktmp->Str();
    for (ttmp = Tags.First(), pTag = ttmp->Str(); pTag; (ttmp = Tags.Next()) ? (pTag = ttmp->Str()) : (pTag = NULL))
      {
      //Get the Data for the title Column
      sprintf(Buff, "%s.%s", pTag, pVar);
      CXM_Route  ObjRoute;
      CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      //now it is possible to extract the first item from the data item structure ( if it exists )

      if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        CPkDataItem* pPkDI = ObjData.FirstItem();
        if (pPkDI->CnvIndex()!=0)// && pPkDI->CnvTxt())
          {
          if (_stricmp(pCvs, "*")==0 || Cnvs[pPkDI->CnvIndex()]->Find(pCvs)==NULL)
            sprintf(Buff, " %s(%s) ", pVLb, pPkDI->CnvTxt());
          else
            sprintf(Buff, " %s(%s) ", pVLb, pCvs);
          }
        else
          sprintf(Buff, " %s ", pVLb);
        break;
        }
      }
    if (pTag)
      {
      Get_Attr_Box(Buff, Pt, Set, &xmin_tit, &xmax_tit, &ymin, &ymax);
      ValAttr.Append(Buff);
      }
    else
      { //This pVar not found for any tag and should be removed form the list
      LogWarning("GrfCmds", 0, "Variable %s not found. This will be excluded", pVar);
      Vars.Remove(vtmp);
      VLbs.Remove(ktmp);
      Fmts.Remove(ftmp);
      Cvss.Remove(ptmp);
      }
    ktmp = VLbs.Next();
    ftmp = Fmts.Next();
    ptmp = Cvss.Next();
    }

  if (Vars.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid variables");
    return NULL;
    }
  if (Vars.Length()!=VLbs.Length() || Vars.Length()!=Fmts.Length() || Vars.Length()!=Cvss.Length())
    {
    //LogError("GrfCmds", 0, "Number of variable name != format names - aborting command");
    LogError("GrfCmds", 0, "Variable, variable-labels, format and engineering-unit lists should match");
    return NULL;
    }

  //Make a string list for each column: title then all the values for each tag...
  ttmp = TLbs.First();
  for (pTag = Tags.First()->Str(); pTag; (ptmp = Tags.Next()) ? (pTag = ptmp->Str()) : (pTag = NULL))
    {
    pTLb = ttmp->Str();
    sprintf(Buff, " %s ", pTLb);
    Get_Attr_Box(Buff, Pt, Set, &xmin_col, &xmax_col, &ymin, &ymax);
    ValAttr.Append(Buff);

    for (pVar = Vars.First()->Str(),pFmt = Fmts.First()->Str(),pCvs = Cvss.First()->Str();
      pVar,pFmt,pCvs;
      (ptmp = Vars.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL),
      (ktmp = Fmts.Next()) ? (pFmt = ktmp->Str()) : (pFmt = NULL),
      (ftmp = Cvss.Next()) ? (pCvs = ftmp->Str()) : (pCvs = NULL) )
      {
      sprintf(Buff, "%s.%s", pTag, pVar);
      CXM_Route  ObjRoute;
      CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))// if tag was located
        {
        CPkDataItem* pPkDI = ObjData.FirstItem();
        double d;
        if (pPkDI->CnvIndex()!=0)// && pPkDI->CnvTxt())
          {
          if (_stricmp(pCvs, "*")==0 || Cnvs[pPkDI->CnvIndex()]->Find(pCvs)==NULL)
            d = pPkDI->Value()->GetDouble(pPkDI->CnvIndex(), pPkDI->CnvTxt());
          else
            d = pPkDI->Value()->GetDouble(pPkDI->CnvIndex(), pCvs);
          }
        else
          d = pPkDI->Value()->GetDouble();
        int FmtIndex = FmtAttribute::FindFmt(pFmt);
        if (FmtIndex<0)
          FmtIndex = 2;
        Buff[0] = ' ';
        DefinedFmts[FmtIndex].FormatFloat(d, &Buff[1], sizeof(Buff)-2);
        strcat(Buff, " ");
        }
      else
        strcpy(Buff, " ");
      Get_Attr_Box(Buff, Pt, Set, &xmin_col, &xmax_col, &ymin, &ymax);
      ValAttr.Append(Buff);
      }
    ttmp = TLbs.Next();
    }

  CGrfLayer * l = Find(Annotation_Layer);
  if (!l)
    l = Append(Drawing->FindLayerDef(Annotation_Layer));
  if (!l)
    return NULL;
  SetCurrentLayer(Annotation_Layer, "");
  char name[1024];
  int num = Blocks.FindUnique("ANNBLK_",1);
  sprintf(name, "ANNBLK_%d", num);

  Pt_3f zero(0.0, 0.0, 0.0);
  pBlock pAnnblk = Add_Block(name, zero.p());
  pAnnblk->SetCurrentLayer(Annotation_Layer, "");

  double colwidth  = (xmax_col - xmin_col);
  double labwidth  = (xmax_tit - xmin_tit);
  double boxheight = (ymax - ymin) * 1.3;

  DXF_ENTITY ins = (DXF_ENTITY)NULL;
  C3_CURVE crv;
  double xval;
  // The title column
  CDVector YAttr(Vars.Length() + 2);
  for (int i=0; i<=Vars.Length()+1; i++)
    {
    PT3 p1,p2;
    p1[0] = 0.0;
    p1[1] = -1.0 * (i*boxheight);
    p1[2] = 0.0;
    p2[0] = (Tags.Length()*colwidth) + labwidth;
    p2[1] = -1.0 * (i*boxheight);
    p2[2] = 0.0;
    crv = pAnnblk->Add_Curve(c3d_line(p1, p2));
    SetCurveColor(crv, -1);
    YAttr[i] = Pos.y() + p1[1];
    }
  // The Tags columns
  CDVector XAttr((Tags.Length() + 2));
  for (i = 0; i <= Tags.Length()+1; i++ )
    {
    PT3 p1,p2;
    if (i==0)
      xval = 0.0;
    else if (i==1)
      xval = labwidth;
    else
      xval = labwidth + (i-1)*colwidth;

    p1[0] = xval;
    p1[1] = 0;
    p1[2] = 0.0;
    p2[0] = xval;
    p2[1] = ((Vars.Length()+1) * boxheight) * -1.0;
    p2[2] = 0.0;
    crv = pAnnblk->Add_Curve(c3d_line(p1, p2));
    SetCurveColor(crv, -1);
    XAttr[i] = Pos.x() + p1[0];
    }

  //Scl.Set(Scale,Scale,Scale);
  Scl.Set(1.0, 1.0, 1.0);
  ins = Create_Insert(name, Pos, -1, Scl);
  if (ins==NULL)
    return NULL;

  //Succesfully created the insert, now add all the attributes...
  for (i=0, pVar = ValAttr.First()->Str(); pVar; (ptmp = ValAttr.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL),i++ )
    {
    int yind = i%(Vars.Length()+1);
    int xind = i/(Vars.Length()+1);
    Pt_3f p((XAttr[xind]+XAttr[xind+1])/2.0, (YAttr[yind]+YAttr[yind+1])/2.0, 0.0);
    char nmbuf[1024];
    sprintf(nmbuf, "ANNOT%d", i);
    Add_Insert_Attrib(ins, nmbuf, pVar, p.p(), Set);
    }

  char Buf[8192];
  strcpy(Buf, "");
  for (pTag = Tags.First()->Str(); pTag; (ptmp = Tags.Next()) ? (pTag = ptmp->Str()) : (pTag = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pTag);
    }
  Set.Flags |= DXF_ATTRIB_INVIS;
  Add_Insert_Attrib(ins, "ANNOT_TAGS", Buf, Pos.p(), Set);
  strcpy(Buf, "");
  for (pTag = TLbs.First()->Str(); pTag; (ptmp = TLbs.Next()) ? (pTag = ptmp->Str()) : (pTag = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pTag);
    }
  Add_Insert_Attrib(ins, "ANNOT_TAGLBLS", Buf, Pos.p(), Set);
  strcpy(Buf, "");
  for( pVar = Vars.First()->Str(); pVar; (ptmp = Vars.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pVar);
    }
  Add_Insert_Attrib(ins, "ANNOT_VARS", Buf, Pos.p(), Set);
  strcpy(Buf, "");
  for( pVar = VLbs.First()->Str(); pVar; (ptmp = VLbs.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pVar);
    }
  Add_Insert_Attrib(ins, "ANNOT_VARLBLS", Buf, Pos.p(), Set);
  strcpy(Buf, "");
  for (pFmt = Fmts.First()->Str(); pFmt; (ptmp = Fmts.Next()) ? (pFmt = ptmp->Str()) : (pFmt = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pFmt);
    }
  Add_Insert_Attrib(ins, "ANNOT_TAGFMTS", Buf, Pos.p(), Set);
  strcpy(Buf, "");
  for( pVar = Cvss.First()->Str(); pVar; (ptmp = Cvss.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pVar);
    }
  Add_Insert_Attrib(ins, "ANNOT_TAGCNVS", Buf, Pos.p(), Set);
  Set.Flags &= ~DXF_ATTRIB_INVIS;
  return ins;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Update_Annotation_Block(DXF_ENTITY e, CExecObj *pDb, Attr_Settings &Set, double Scale)
  {
  pchar pFmt = Find_Attr_Value(e, "ANNOTFMTS");
  pchar pVar = Find_Attr_Value(e, "ANNOTVARS");
  pchar pTag = Find_Attr_Value(e, "ANNOTTAGS");
  if (!pVar || !pTag || !pFmt)
    return NULL;

  Strng_List VarLst,TagLst,FmtLst;
  pchar p;
  p = pVar; p = strtok(p, " "); while (p) { VarLst.Append(p); p = strtok(NULL, " "); }
p = pTag; p = strtok(p, " "); while (p) { TagLst.Append(p); p = strtok(NULL, " "); }
p = pFmt; p = strtok(p, " "); while (p) { FmtLst.Append(p); p = strtok(NULL, " "); }
//double Scale = DXF_INSERT_X_SCALE(e);
REAL *d = DXF_INSERT_PT(e);
Pt_3f Pos(d[0], d[1], d[2]);
return Create_Annotation_Block(pDb, VarLst, FmtLst, TagLst, Pos, Set, Scale);
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Create_Annotation_Block(CExecObj *pDb, Strng_List &Vars, Strng_List &Fmts, Strng_List &Tags, Pt_3f Pos, Attr_Settings &Set, double Scale)
  {
  char Buff[1024],*pTag,*pVar,*pFmt;
  pStrng ptmp,ktmp,ttmp,vtmp,ftmp;

  // remove all duplicate Tags and blanks...
  for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
    {
    if (ptmp->Str() && ptmp->Length()>0)
      {
      pStrng pFound = Tags.FindFrom(ptmp->pNxt, ptmp->Str());
      if (pFound)
        Tags.Remove(ptmp);
      }
    else if (ptmp->Str() && ptmp->Length()==0)
      Tags.Remove(ptmp);
    }
  // Check all the Mentioned Tags are findable and delete if not
  for (ptmp = Tags.First(); ptmp; ptmp = Tags.Next())
    {
    if (ptmp->Str())
      {
      CXM_Route ObjRoute;
      CXM_ObjectTag ObjTag(ptmp->Str(), TABOpt_Parms);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      if (!pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        LogWarning("GrfCmds", 0, "Tag %s not found. This will be excluded", ptmp->Str());
        Tags.Remove(ptmp);
        }
      }
    }
  if (Tags.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid Tags");
    return NULL;
    }

  // remove all duplicate variables and blanks...
  for (ptmp = Vars.First(); ptmp; ptmp = Vars.Next())
    {
    if (ptmp->Str() && ptmp->Length()>0)
      {
      pStrng pFound = Vars.FindFrom(ptmp->pNxt, ptmp->Str());
      if (pFound)
        Vars.Remove(ptmp);
      }
    else if (ptmp->Str() && ptmp->Length()==0)
      Vars.Remove(ptmp);
    }
  if (Vars.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid variables");
    return NULL;
    }

  Pt_3f Scl;
  Strng_List ValAttr;
  Pt_3f Pt(0.0, 0.0, 0.0);
  double xmin_tit = 1.0e30,xmax_tit = -1.0e30;
  double xmin_col = 1.0e30,xmax_col = -1.0e30;
  double ymin = 1.0e30,ymax = -1.0e30;
  Set.Vjust = Text_Middle;

  //Make a string list of the title columns (titles down left side)...
  //Foreach Var parse tags until found else exclude from list
  ValAttr.Append("   ");// for the top left blank box
  ftmp = Fmts.First();
  for (vtmp = Vars.First(), pVar = vtmp->Str(); pVar; (vtmp = Vars.Next()) ? (pVar = vtmp->Str()) : (pVar = NULL))
    {
    for (ttmp = Tags.First(), pTag = ttmp->Str(); pTag; (ttmp = Tags.Next()) ? (pTag = ttmp->Str()) : (pTag = NULL))
      {
      //Get the Data for the title Column
      sprintf(Buff, "%s.%s", pTag, pVar);
      CXM_Route  ObjRoute;
      CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      //now it is possible to extract the first item from the data item structure ( if it exists )

      if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
        {
        CPkDataItem* pPkDI = ObjData.FirstItem();
        if (pPkDI->CnvTxt())
          sprintf(Buff, " %s(%s) ", pVar, pPkDI->CnvTxt());
        else
          sprintf(Buff, " %s ", pVar);
        break;
        }
      }
    if (pTag)
      {
      Get_Attr_Box(Buff, Pt, Set, &xmin_tit, &xmax_tit, &ymin, &ymax);
      ValAttr.Append(Buff);
      }
    else
      { //This pVar not found for any tag and should be removed form the list
      LogWarning("GrfCmds", 0, "Variable %s not found. This will be excluded", pVar);
      Vars.Remove(vtmp);
      Fmts.Remove(ftmp);
      }
    ftmp = Fmts.Next();
    }

  if (Vars.Length()==0)
    {
    LogError("GrfCmds", 0, "No valid variables");
    return NULL;
    }
  if (Vars.Length() != Fmts.Length())
    {
    //LogError("GrfCmds", 0, "Number of variable name != format names - aborting command");
    LogError("GrfCmds", 0, "Variable list and format list should match");
    return NULL;
    }

  //Make a string list for each column: title then all the values for each tag...
  for (pTag = Tags.First()->Str(); pTag; (ptmp = Tags.Next()) ? (pTag = ptmp->Str()) : (pTag = NULL))
    {
    sprintf(Buff, " %s ", pTag);
    Get_Attr_Box(Buff, Pt, Set, &xmin_col, &xmax_col, &ymin, &ymax);
    ValAttr.Append(Buff);

    for (pVar = Vars.First()->Str(),pFmt = Fmts.First()->Str();
      pVar,pFmt;
      (ptmp = Vars.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL),
      (ktmp = Fmts.Next()) ? (pFmt = ktmp->Str()) : (pFmt = NULL) )
      {
      sprintf(Buff, "%s.%s", pTag, pVar);
      CXM_Route  ObjRoute;
      CXM_ObjectTag ObjTag(Buff, TABOpt_ValCnvs);//TABOpt_AllInfo);
      CXM_ObjectData ObjData;
      if (pDb->XReadTaggedItem(ObjTag, ObjData, ObjRoute))// if tag was located
        {
        CPkDataItem* pPkDI = ObjData.FirstItem();
        char sss[1024];
        sprintf(sss, pFmt, pPkDI->Value()->GetDouble(pPkDI->CnvIndex(), pPkDI->CnvTxt()));
        sprintf(Buff," %s ", sss);
        }
      else
        strcpy(Buff," ");
      Get_Attr_Box(Buff, Pt, Set, &xmin_col, &xmax_col, &ymin, &ymax);
      ValAttr.Append(Buff);
      }
    }

  CGrfLayer * l = Find(Annotation_Layer);
  if (!l)
    l = Append(Drawing->FindLayerDef(Annotation_Layer));
  if (!l)
    return NULL;
  SetCurrentLayer(Annotation_Layer, "");
  char name[1024];
  int num = Blocks.FindUnique("ANNBLK_",1);
  sprintf(name, "ANNBLK_%d", num);

  Pt_3f zero(0.0, 0.0, 0.0);
  pBlock pAnnblk = Add_Block(name, zero.p());
  pAnnblk->SetCurrentLayer(Annotation_Layer, "");

  double colwidth  = (xmax_col - xmin_col);
  double labwidth  = (xmax_tit - xmin_tit);
  double boxheight = (ymax - ymin) * 1.3;

  DXF_ENTITY ins = (DXF_ENTITY)NULL;
  C3_CURVE crv;
  double xval;
  // The title column
  CDVector YAttr(Vars.Length() + 2);
  for (int i=0; i<=Vars.Length()+1; i++)
    {
    PT3 p1,p2;
    p1[0] = 0.0;
    p1[1] = -1.0 * (i*boxheight);
    p1[2] = 0.0;
    p2[0] = (Tags.Length()*colwidth) + labwidth;
    p2[1] = -1.0 * (i*boxheight);
    p2[2] = 0.0;
    crv = pAnnblk->Add_Curve(c3d_line(p1, p2));
    SetCurveColor(crv, -1);
    YAttr[i] = Pos.y() + p1[1];
    }
  // The Tags columns
  CDVector XAttr((Tags.Length() + 2));
  for (i = 0; i <= Tags.Length()+1; i++ )
    {
    PT3 p1,p2;
    if (i==0)
      xval = 0.0;
    else if (i==1)
      xval = labwidth;
    else
      xval = labwidth + (i-1)*colwidth;

    p1[0] = xval;
    p1[1] = 0;
    p1[2] = 0.0;
    p2[0] = xval;
    p2[1] = ((Vars.Length()+1) * boxheight) * -1.0;
    p2[2] = 0.0;
    crv = pAnnblk->Add_Curve(c3d_line(p1, p2));
    SetCurveColor(crv, -1);
    XAttr[i] = Pos.x() + p1[0];
    }

  //Scl.Set(Scale,Scale,Scale);
  Scl.Set(1.0, 1.0, 1.0);
  ins = Create_Insert(name, Pos, -1, Scl);
  if (ins==NULL)
    return NULL;

  //Succesfully created the insert, now add all the attributes...
  for (i=0, pVar = ValAttr.First()->Str(); pVar; (ptmp = ValAttr.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL),i++ )
    {
    int yind = i%(Vars.Length()+1);
    int xind = i/(Vars.Length()+1);
    Pt_3f p((XAttr[xind]+XAttr[xind+1])/2.0, (YAttr[yind]+YAttr[yind+1])/2.0, 0.0);
    char nmbuf[1024];
    sprintf(nmbuf, "ANNOT%d", i);
    Add_Insert_Attrib(ins, nmbuf, pVar, p.p(), Set);
    }

  char Buf[4096];
  strcpy(Buf, "");
  for (pTag = Tags.First()->Str(); pTag; (ptmp = Tags.Next()) ? (pTag = ptmp->Str()) : (pTag = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pTag);
    }
  Set.Flags |= DXF_ATTRIB_INVIS;
  Add_Insert_Attrib(ins, "ANNOTTAGS", Buf, Pos.p(), Set);

  strcpy(Buf, "");
  for( pVar = Vars.First()->Str(); pVar; (ptmp = Vars.Next()) ? (pVar = ptmp->Str()) : (pVar = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pVar);
    }
  Add_Insert_Attrib(ins, "ANNOTVARS", Buf, Pos.p(), Set);
  strcpy(Buf, "");
  for (pFmt = Fmts.First()->Str(); pFmt; (ptmp = Fmts.Next()) ? (pFmt = ptmp->Str()) : (pFmt = NULL) )
    {
    strcat(Buf, " ");
    strcat(Buf, pFmt);
    }
  Add_Insert_Attrib(ins, "ANNOTFMTS", Buf, Pos.p(), Set);
  Set.Flags &= ~DXF_ATTRIB_INVIS;
  return ins;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Create_NewText_Block(char* Text, Pt_3f Pos, Attr_Settings &Set, double Scale)
  {
  if (Text==NULL || strlen(Text)==0)
    {
    LogError("GrfCmds", 0, "Text not specified");
    return NULL;
    }

  Pt_3f Scl;
  Strng_List ValAttr;
  Pt_3f Pt(0.0, 0.0, 0.0);
  double xmin_tit = 1.0e30,xmax_tit = -1.0e30;
  double xmin_col = 1.0e30,xmax_col = -1.0e30;
  double ymin = 1.0e30,ymax = -1.0e30;
  Set.Vjust = Text_Middle;

  CGrfLayer * l = Find(Annotation_Layer);
  if (!l)
    l = Append(Drawing->FindLayerDef(Annotation_Layer));
  if (!l)
    return NULL;
  SetCurrentLayer(Annotation_Layer, "");
  char name[1024];
  int num = Blocks.FindUnique("ANNTXT_",1);
  sprintf(name, "ANNTXT_%3d", num);

  Pt_3f zero(0.0, 0.0, 0.0);
  pBlock pAnnblk = Add_Block(name, zero.p());
  pAnnblk->SetCurrentLayer(Annotation_Layer, "");

  DXF_ENTITY ins = (DXF_ENTITY)NULL;
  C3_CURVE crv;
  PT3 p1,p2;
  p1[0] = -0.02;
  p1[1] = -0.01;
  p1[2] = 0.0;
  p2[0] = -0.01;
  p2[1] = -0.01;
  p2[2] = 0.0;
  crv = pAnnblk->Add_Curve(c3d_line(p1, p2));
  SetCurveColor(crv, -1);

  //Scl.Set(Scale,Scale,Scale);
  Scl.Set(1.0, 1.0, 1.0);
  ins = Create_Insert(name, Pos, -1, Scl);
  if (ins==NULL)
    return NULL;

  //Succesfully created the insert, now add all the attributes...
  //Pt_3f p(0.0, 0.0, 0.0);
  char nmbuf[32];
  sprintf(nmbuf, "TXT1");
  Add_Insert_Attrib(ins, nmbuf, Text, Pos.p(), Set);
  return ins;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Add_Loop(pViewport vp)
  {
  DXF_ENTITY ins = (DXF_ENTITY)NULL;
  return ins;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Add_Instr(pViewport vp)
  {
  DXF_ENTITY ins = (DXF_ENTITY)NULL;
  return ins;
  }

//---------------------------------------------------------------------------

int DXF_Drawing::FindCurveIntersect(pViewport vp, Pt_3f Pt, CEntInView* pEnt_1,CEntInView* pEnt_2, Pt_3f &IntPt, PARM parm_1, PARM parm_2)
  {
  int ret=0;
  C3_CURVE c3_1=pEnt_1->CurvePtr();
  C3_CURVE c3_2=pEnt_2->CurvePtr();

  flag cisp_1=C3_CURVE_IS_PCURVE(c3_1);
  flag cisp_2=C3_CURVE_IS_PCURVE(c3_2);

  C3_CURVE pc_1= c3_1;
  C3_CURVE pc_2= c3_2;
  //if( C3_CURVE_IS_PCURVE(c3_1) || C3_CURVE_IS_PCURVE(c3_1) )
  //  {
  //  AfxMessageBox("Intersect cannot handle pcurves");
  //  return(-1);
  //  }

  if (c3_1 && c3_2)
    {
    PT3 Org, XAx, YAx;
    c3v_set(0.,0.,0., Org);
    c3v_set(1.,0.,0., XAx);
    c3v_set(0.,1.,0., YAx);

    DML_LIST IntList=dml_create_list();
    int nInts=0;
    double Dist=1.0e30;
    double sp1,sp2;
    double ep1,ep2;

    double ip1=0.0;
    int SegCnt_1=0;
    while (1)
      {
      if (cisp_1)
        {
        c3_1=c3d_pcurve_segment(pc_1, SegCnt_1);
        if (c3_1==NULL)
          break;
        }
      else if (SegCnt_1>0)
        break;

      double ip2=0.0;
      int SegCnt_2=0;
      while (1)
        {
        if (cisp_2)
          {
          c3_2=c3d_pcurve_segment(pc_2, SegCnt_2);
          if (c3_2==NULL)
            break;
          }
        else if (SegCnt_2>0)
          break;

        C2_CURVE c2_1=c3d_convert_3d2d(c3_1, Org, XAx, YAx);
        C2_CURVE c2_2=c3d_convert_3d2d(c3_2, Org, XAx, YAx);

        C2_INT_REC int_rec;
        DML_ITEM item;
        if (c2c_intersect_ext(c2_1, c2_2, IntList)!=0)
          {
          DML_FOR_LOOP(dml_first(IntList), item)
            {
            int_rec=(C2_INT_REC)dml_record(item);
            double d=HYPOT(Pt.X-PT2_X(C2_INT_REC_PT(int_rec)), Pt.Y-PT2_Y(C2_INT_REC_PT(int_rec)));
            if (d<Dist)
              {
              Dist=d;
              IntPt.Set(PT2_X(C2_INT_REC_PT(int_rec)), PT2_Y(C2_INT_REC_PT(int_rec)), 0.0);
              memcpy(parm_1, C2_INT_REC_PARM1(int_rec), sizeof(*parm_1));
              //(*parm_1).parameter += (SegCnt_1);
              (*parm_1).parameter += (ip1);
              memcpy(parm_2, C2_INT_REC_PARM2(int_rec), sizeof(*parm_2));
              //(*parm_2).parameter += (SegCnt_2);
              (*parm_2).parameter += (ip2);
              }
            c2d_free_int_rec(int_rec);
            ++nInts;
            }
          dml_clear_list(IntList);
          }
        sp2=C3_CURVE_T0(c3_2);
        ep2=C3_CURVE_T1(c3_2);
        ip2+=(ep2-sp2);
        c2d_free_curve(c2_1);
        c2d_free_curve(c2_2);
        if (cisp_2)
          c3d_free_curve(c3_2);
        SegCnt_2++;
        }
      sp1=C3_CURVE_T0(c3_1);
      ep1=C3_CURVE_T1(c3_1);
      ip1+=(ep1-sp1);
      if (cisp_1)
        c3d_free_curve(c3_1);
      SegCnt_1++;
      }

    return nInts; // No Of Intersections
    }
  return -1; // Both entities not curves.
  }

//---------------------------------------------------------------------------
// return -2 = Bad Selection
// return -1 = Selected entities are not both curves
// return 0 = Selected curves do not intersect
// return >0 = No of intersections

int  DXF_Drawing::BreakCurvesAtIntersect(pViewport vp, Pt_3f Pt, Pt_3f &IntPt)
  {
  CEntInView* pE[2];
  pE[0] = vp->FirstSelectedEntity();
  pE[1] = vp->NextSelectedEntity();
  if(pE[0]==NULL || pE[1]==NULL)
    return -2;

  C3_CURVE c3[2];
  for (int i=0; i<2; i++)
    c3[i]=pE[i]->CurvePtr();
  if (c3[0]==NULL || c3[1]==NULL)
    return -1;

  PARM_S parm[2];
  int nInts=FindCurveIntersect(vp, Pt, pE[0], pE[1], IntPt, &parm[0], &parm[1]);
  if (nInts>0)
    {
    double sp,ep,ip,lp,tol;
    for (int i=0; i<2; i++)
      {
      sp=C3_CURVE_T0(c3[i]);
      ep=C3_CURVE_T1(c3[i]);
      ASSERT(ep>sp);
      lp=ep-sp;
      tol=lp*0.0001;
      ip=PARM_T(&parm[i]);
      dbgpln("break %i %s %8.4f %8.4f %8.4f",i,(ip>sp+tol && ip<ep-tol)?"YES":"no ",sp,ip,ep);
      if (ip>sp+tol && ip<ep-tol)
        {
        EntityInvalidate(NULL, c3[i]);
        //vp->Draw(c3[i], -1);
        C3_CURVE nc3=c3d_trim_t0(c3[i], ip);
        C3_CURVE c3_=c3c_trim_t1(c3[i], ip);
        //double sp1=C3_CURVE_T0(c3[i]);
        //double ep1=C3_CURVE_T1(c3[i]);
        //double sp2=C3_CURVE_T0(nc3);
        //double ep2=C3_CURVE_T1(nc3);
        EntityInvalidate(NULL, c3[i]);
        vp->Draw(c3[i], -1);

        CGrfLayer * pLay = FindLayer(c3[i]);
        ASSERT(pLay);
        nc3->m_Color=c3[i]->m_Color;
        nc3->m_Thickness=c3[i]->m_Thickness;
        pLay->Add(nc3);
        EntityInvalidate(NULL, nc3);
        vp->Draw(nc3, -1);
        //Draw(
        }
      }
    }
  return nInts;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Add_Unit(pViewport vp, Pt_3f &CTagPt, flag CTagPtOK, pchar equip_name, pchar Tag, pchar BlockName, pchar ModelName, Attr_Settings &Tag_Attr_Set)
  {
  pBlock b;
  int cnt = 0;
  DXF_ENTITY ins = (DXF_ENTITY)NULL;

  dbgpln("Adding Equipment\n");

  if (! equip_name && !Tag)
    {
    CEntInView* p = vp->FirstSelectedEntity();
    if( !p )
      return ins;
    do
      {
      if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
        {
        equip_name = (pchar)DXF_TEXT_VALUE_GET(p->EntityPtr());
        cnt++;
        }
      }
    while( p = vp->NextSelectedEntity());
    if (cnt != 1)
      {
      TRACE(" Building Equipment cannot find a tag name\n");
      return ins;
      }
    //dbgpln(" Building Equipment from selected tag %s\n",equip_name);
    }
  else
    {
    //dbgpln(" Building Equipment from input tag %s\n",equip_name);
    }

  // Remove Text if it is == tag, Equip or Modelname
  for (CEntInView* p = vp->FirstSelectedEntity(); p ; )
    {
    CEntInView* pn=vp->NextSelectedEntity();
    if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
      {
      Delete(p->EntityPtr());
      /* cnm remove all text .. it should be in attributes
      pchar pTxt=(pchar)DXF_TEXT_VALUE(p->EntityPtr());
      if (equip_name && strcmp(pTxt, equip_name)==0 ||
      Tag        && strcmp(pTxt, Tag)==0 ||
      ModelName  && strcmp(pTxt, ModelName)==0)
      Delete(p->EntityPtr());
      */
      }
    p=pn;
    }

  // Check this tag is Unique
  TRACE(" EQUIP_ID %s needs a uniqueness check\n",equip_name);

  // Make a origin from centre of gravity of the entities
  Pt_3f bpos,boxll,boxur;
  if (! FindCOGfromSelection(vp, bpos,boxll,boxur))
    {
    return ins;
    }

  // Now Make a block from the selected entities
  if( !(b = ConstructBlockFromSelection(vp,BlockName,bpos.p())))
    return ins;

  // Now add make an insert with this block and tag it
  if(!(ins = Create_Insert(DXF_BLOCK_NAME_GET(b->Def),bpos,GR_LIGHTGREEN)))
    return ins;

  Attr_Settings AttrVis = Tag_Attr_Set;
  Attr_Settings AttrInVis = Tag_Attr_Set;
  AttrInVis.Flags=DXF_ATTRIB_INVIS;

  if (!CTagPtOK)
    CTagPt=bpos;

  Add_Insert_Attrib(ins,TagAttribStr,       Tag          ,CTagPt.p(), AttrVis);
  //bpos.Set(CTagPt.x(),CTagPt.y()-4.0,CTagPt.z());
  //Add_Insert_Attrib(ins,"EQUIP_ID",  equip_name   ,CTagPt.p(), AttrInVis);
  //CTagPt.Set(CTagPt.x(),CTagPt.y()-4.0,CTagPt.z());
  //Add_Insert_Attrib(ins,"MODEL_TYPE",ModelName    ,CTagPt.p(),AttrInVis);


  return ins;
  }

//---------------------------------------------------------------------------

//DML_LIST DXF_Drawing::SelectInsertsOnAttrCombo(DML_LIST l,pchar *AttrTag,pchar *AttrValue)
//{
//
//    Layers->SelectInsertsOnAttrCombo(DML_LIST l,pchar *AttrTag,pchar *AttrValue)
//
//
//}

//---------------------------------------------------------------------------

void DXF_Drawing::DumpSelectedEntities(pViewport vp)
  {
  CEntInView* p = vp->FirstSelectedEntity();
  if( !p )
    return;
  do
    {
    if( p->EntityPtr())
      dump_entity(p->EntityPtr());
    if( p->CurvePtr())
      dump_curve(p->CurvePtr());
    }
  while( p = vp->NextSelectedEntity());
  }

//---------------------------------------------------------------------------

void DXF_Drawing::AlignSelectedInserts(pViewport vp)
  {
  Pt_3f cg,boxll,boxur;
  FindCOGfromSelection(vp,cg,boxll,boxur);
  double dx = boxur.x() - boxll.x();
  double dy = boxur.y() - boxll.y();
  double px = (boxur.x() + boxll.x())/2.0;
  double py = (boxur.y() + boxll.y())/2.0;

  CEntInView* p = vp->FirstSelectedEntity();
  if( !p )
    return;
  do
    {
    Pt_3f from,to;
    from.Set(DXF_INSERT_PT(p->EntityPtr())[0],DXF_INSERT_PT(p->EntityPtr())[1],DXF_INSERT_PT(p->EntityPtr())[2]);
    if( fabs(dx) < fabs(dy) )
      {
      to.Set(px,DXF_INSERT_PT(p->EntityPtr())[1],DXF_INSERT_PT(p->EntityPtr())[2]);
      }
    else
      {
      to.Set(DXF_INSERT_PT(p->EntityPtr())[0],py,DXF_INSERT_PT(p->EntityPtr())[2]);
      }
    TranslateEntity(p->EntityPtr(), from, to);
    DXF_ENTITY xxx = p->EntityPtr();
    EntityInvalidate(p->EntityPtr(),NULL);
    vp->Draw(xxx,-1);
    }
  while( p = vp->NextSelectedEntity());
  }

//---------------------------------------------------------------------------

struct CSym { bool m_Circle; bool m_IsStart; bool m_Done; PT3 m_Pt; };
static CSym MakeupSym[] =
  {
    {1, 0, 0,   {    -8 ,   1.5 }, },
    {0, 0, 0,   {  -9.5 ,     0 }, },
    {0, 0, 0,   {    -8 ,  -1.5 }, },
    {0, 1, 0,   {    -8 ,  -1.5 }, },
    {0, 0, 0,   {    -5 ,  -1.5 }, },
    {0, 0, 0,   {  -3.5 ,     0 }, },
    {0, 0, 0,   {    -5 ,   1.5 }, },
    {0, 0, 0,   {    -8 ,   1.5 }, },
    {0, 1, 0,   {  -3.5 ,     0 }, },
    {0, 0, 0,   {    -2 ,     0 }, },
    {0, 0, 1,                      },
  };
static CSym SpillSym[] =
  {                                                     
    {0, 1, 0,   {     2 ,     0 }, },
    {0, 0, 0,   {     6 ,     0 }, },
    {0, 0, 0,   {     6 ,    -2 }, },
    {0, 1, 0,   {     4 ,    -2 }, },
    {0, 0, 0,   {     6 ,    -4 }, },
    {0, 0, 0,   {     6 ,    -6 }, },
    {0, 1, 0,   {     8 ,    -2 }, },
    {0, 0, 0,   {     6 ,    -4 }, },
    {0, 0, 1,                      },
  };
static CSym VentSym[] =
  {
    {0, 1, 0,   {     2 ,     0 }, },
    {0, 0, 0,   {     6 ,     0 }, },
    {0, 0, 0,   {     6 ,     3 }, },
    {0, 1, 0,   {     4 ,     3 }, },
    {0, 0, 0,   {     6 ,     5 }, },
    {0, 0, 0,   {     8 ,     3 }, },
    {0, 0, 1,                      },
  };

DXF_ENTITY DXF_Drawing::Add_AssocGrf(eAssocGrfTypes Type, PT3 pt, LPCSTR NdTagStr, LPCSTR TagStr, DXF_ENTITY OwnerEntity)
  {
  pBlock  AssGrf;
  DXF_ENTITY InsSym;
  //REAL c_rad = 7.0;
  REAL t_hgt = 1.5;
  //REAL llength = 0.6*c_rad;
  Pt_3f AssocGrfPos;

  Pt_3f AssTagPos;
  Pt_3f zero;
  Attr_Settings Atag(50,DXF_ATTRIB_INVIS,t_hgt,1.0,0.0,GR_DARKGRAY,Text_Left ,Text_Baseline,"STANDARD");

  DML_LIST Lst=dml_create_list();
  DML_LIST EmptyLst=dml_create_list();
  dml_insert_after(Lst, NULL, OwnerEntity);
  C3_BOX_S Bnds ;                      
  GetBounds(&Bnds, EmptyLst, Lst, EmptyLst);

  dml_free_list(Lst);
  dml_free_list(EmptyLst);

  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    LPSTR BlockName=NULL;
    CSym * pSym=NULL;
    switch (Type)
      {
      case eAG_Makeup:
        {
        BlockName="MAKEUP_ASSOCGRF";
        pSym=MakeupSym;
        AssocGrfPos.Set(Bnds.minpt[0], Bnds.maxpt[1], pt[2]);
        AssTagPos.Set(0.0, 4.0, 0.0);
        AssTagPos = AssTagPos+ AssocGrfPos;                                          
        break;
        }
      case eAG_Spill:
        {
        BlockName="SPILL_ASSOCGRF";
        pSym=SpillSym;
        AssocGrfPos.Set(Bnds.maxpt[0], Bnds.minpt[1], pt[2]);
        AssTagPos.Set(0.0, 2.0, 0.0);
        AssTagPos = AssTagPos+ AssocGrfPos;                                          
        break;
        }
      case eAG_Vent:
        {
        BlockName="VENT_ASSOCGRF";
        pSym=VentSym;
        AssocGrfPos.Set(Bnds.maxpt[0], Bnds.maxpt[1], pt[2]);
        AssTagPos.Set(0.0, -2.0, 0.0);
        AssTagPos = AssTagPos+ AssocGrfPos;                                          
        break;
        }
      }
    
    if (pSym)
      {
      AssGrf = Blocks.Find(BlockName);
      if (!AssGrf)
        {
        //    num = Blocks.FindUnique(BlockName,0);
        //(void)sprintf(name,"ILOOP_%d",num);

        AssGrf = Add_Block(BlockName,zero.p());
        C3_CURVE cc = NULL;
        while (!pSym->m_Done)
          {
          if (pSym->m_Circle)
            {
            AssGrf->Add_Circle(pSym->m_Pt,(pSym+1)->m_Pt,(pSym+2)->m_Pt);
            pSym+=3;
            }
          else if (pSym->m_IsStart)
            {
            cc = AssGrf->Add_PLine_Start(pSym->m_Pt);
            pSym++;
            }
          else
            {
            AssGrf->Add_PLine_Vertex(cc,pSym->m_Pt);
            pSym++;
            }
          }
        }
      }

    InsSym = Create_Insert(BlockName, AssocGrfPos, GR_LIGHTCYAN);

    Add_Insert_Attrib(InsSym,TagAttribStr,        (LPSTR)TagStr,        AssTagPos.p(), Atag);
    Add_Insert_Attrib(InsSym,AssocTagAttribStr,  (LPSTR)NdTagStr,      AssTagPos.p(), Atag);

    
    }
  return InsSym;
  }

//---------------------------------------------------------------------------

DXF_ENTITY DXF_Drawing::Add_PidBubble(CEntInView* ent,PT3 pt,Pt_SLW x,pchar loop_id,pchar area_code,pchar equip_isa,pchar loop_type)
  {
  dbgpln("Adding pidbubble\n");

  int num;
  char name[1024];
  pBlock pidbub;
  DXF_ENTITY ins;
  REAL c_rad = 7.0;
  REAL t_hgt = 1.5;
  REAL llength = 0.6*c_rad;
  Pt_3f block_scale;
  Pt_3f block_pos;
  Pt_3f line_s;
  Pt_3f line_e;
  Pt_3f circ_c;
  Pt_3f area_code_pos;
  Pt_3f equip_isa_pos;
  Pt_3f loop_id_pos;
  Pt_3f loop_type_pos;
  Pt_3f tag_pos;
  Pt_3f zero;
  Attr_Settings Aloop_id   = Attr_Settings (50,0,t_hgt,1.0,0.0,GR_DARKGRAY,Text_Cntrd,Text_Baseline,"STANDARD");
  Attr_Settings Aarea_code = Attr_Settings (50,0,t_hgt,1.0,0.0,GR_DARKGRAY,Text_Cntrd,Text_Baseline,"STANDARD");
  Attr_Settings Aequip_isa = Attr_Settings (50,0,t_hgt,1.0,0.0,GR_DARKGRAY,Text_Cntrd,Text_Baseline,"STANDARD");
  Attr_Settings Atag       = Attr_Settings (50,0,t_hgt,1.0,0.0,GR_DARKGRAY,Text_Left ,Text_Baseline,"STANDARD");
  Attr_Settings Aloop_type = Attr_Settings (50,0,t_hgt,1.0,0.0,GR_DARKGRAY,Text_Left ,Text_Baseline,"STANDARD");


  block_pos.Set(pt);
  block_scale.Set(1.0,1.0,1.0);
  line_e.Set(0.0,llength,0.0);
  circ_c.Set(0.0,llength+c_rad,0.0);
  area_code_pos.Set(0.0,llength + 1.55* c_rad,0.0);
  equip_isa_pos.Set(0.0,llength + 1.1 * c_rad,0.0);
  loop_id_pos.Set  (0.0,llength - 0.6 * c_rad,0.0);
  tag_pos.Set      (c_rad,llength+c_rad            ,0.0);
  loop_type_pos.Set(c_rad,llength+c_rad - t_hgt*1.5,0.0);

  area_code_pos   = area_code_pos + block_pos;
  equip_isa_pos   = equip_isa_pos + block_pos;
  loop_id_pos     = loop_id_pos   + block_pos;
  tag_pos         = tag_pos       + block_pos;
  loop_type_pos   = loop_type_pos + block_pos;


  CGrfLayer * l=Find(CurLayer);
  if (!l)
    l = Append(Drawing->FindLayerDef(CurLayer));
  if (l)
    {
    num = Blocks.FindUnique("ILOOP_",0);
    (void)sprintf(name,"ILOOP_%d",num);

    pidbub = Add_Block(name,zero.p());
    pidbub->Add_Circle(circ_c.p(),c_rad);
    C3_CURVE cc = pidbub->Add_PLine_Start(line_s.p());
    pidbub->Add_PLine_Vertex(cc,line_e.p());
    ins = Create_Insert(name,block_pos,GR_LIGHTCYAN);

    Add_Insert_Attrib(ins,"AREA_CODE","A101", area_code_pos.p(), Aarea_code);
    Add_Insert_Attrib(ins,"EQUIP_ISA","P",    equip_isa_pos.p(), Aequip_isa);
    Add_Insert_Attrib(ins,"LOOP_ID",  "L100",   loop_id_pos.p(), Aloop_id);
    Add_Insert_Attrib(ins,"LOOP_TYPE","LTYPE",loop_type_pos.p(), Aloop_type);
    Add_Insert_Attrib(ins,"TAG",      "TAG",        tag_pos.p(), Atag);
    }
  return ins;
  }

//----------------------------------------------------------------------------

long DXF_Drawing::AddNodeGrfInfo(pNodeGrfInfo pNGI)
  {
  int subfig = 1;
  if (pNGI==NULL)
    return 0;

  const int MAXNDNO = 256;
#define DORANGE(X,Y) { XMn[NdNo]=Min(XMn[NdNo],X);XMx[NdNo]=Max(XMx[NdNo],X);YMn[NdNo]=Min(YMn[NdNo],Y);YMx[NdNo]=Max(YMx[NdNo],Y); };
#define INITRANGE() { XMn[NdNo]=0x7ffffff0; XMx[NdNo]=-XMn[NdNo]; YMn[NdNo]=XMn[NdNo]; YMx[NdNo]=XMx[NdNo]; };
  //#define LOADPT(pt, X, Y) { pt.X =(10.0*(X-XMn[NdNo]))/(XMx[NdNo]-XMn[NdNo]); pt.Y =(10.0*(Y-YMn[NdNo]))/(YMx[NdNo]-YMn[NdNo]); };
#define LOADPT(pt, x, y) { pt.X = x; pt.Y = y; pt.Z=0.0;};
  double XMn[MAXNDNO], XMx[MAXNDNO], YMn[MAXNDNO], YMx[MAXNDNO], X, Y;
  pchar pName, pIOPt,pVar,pType;

  // Scan for Extents
  long NdNo=0;
  INITRANGE();
  for (long Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        pName=pNGI->GetPChar();
        //dbgpln("Name  %s", pName);
        break;
      case DD_Scale:
        pNGI->SetScale(pNGI->GetDVal()/1000.0);
        break;
      case DD_In:
      case DD_Out:
        pIOPt=pNGI->GetPChar();
        //dbgpln("IOPt  %s", pIOPt);
        while (pNGI->GetCoOrd(X,Y))
          {
          DORANGE(X,Y);
          //dbgpln("      %li  %li", X, Y);
          }
        break;
      case DD_Poly:
        while (pNGI->GetCoOrd(X,Y))
          {
          DORANGE(X,Y);
          //dbgpln("Poly  %li  %li", X, Y);
          }
        break;
      case DD_Arc:
        if (pNGI->GetCoOrd(X,Y))
          {
          double Rad=pNGI->GetLength();
          DORANGE(X+Rad,Y+Rad);
          DORANGE(X-Rad,Y-Rad);
          double S=0,E=360;
          pNGI->GetPair(S,E);
          //dbgpln("Arc  %li  %li  %li  %li  %li", X, Y, R, S, E);
          }
        break;
      case DD_Arc3:
        while (pNGI->GetCoOrd(X,Y))
          {
          DORANGE(X,Y);
          //dbgpln("Poly  %li  %li", X, Y);
          }
        break;
      case DD_Dynm:
        pVar  = pNGI->GetAChar();
        pType = pNGI->GetAChar();
        while (pNGI->GetCoOrd(X,Y))
          {
          DORANGE(X,Y);
          //dbgpln("Poly  %li  %li", X, Y);
          }
        break;
      case DD_End:
        //dbgpln("Range X  %li  %li", XMn[NdNo], XMx[NdNo]);
        //dbgpln("Range Y  %li  %li", YMn[NdNo], YMx[NdNo]);
        NdNo++;
        ASSERT(NdNo<MAXNDNO);
        INITRANGE();
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }

  // Build Block
  pBlock b=NULL,bmal;
  //short PolyInx;
  Pt_3f pt;
  C3_CURVE C3;
  NdNo=0;

  CString FnB, FnH, FnT;
  gs_hDXF=NULL;
  for (Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        pName=pNGI->GetPChar();
        b=Blocks.Find(pName);
        if (b==NULL)
          {
          //dbgpln("Build Block %s", pName);
          Pt_3f basept;
          basept.Zero();
          Strng BlkName(MakeValidBlockName(pName));
          b = Add_Block(BlkName(), basept.p());
          //b->IsScd = 1;
          if (0)
            {
            if (gs_hDXF)
              {
              FILE *h=fopen(FnT, "rt");
              if (h)
                {
                char Buff[1204];
                while (fgets(Buff, sizeof(Buff), h))
                  fputs(Buff, gs_hDXF);
                fclose(h);
                }
              fclose(gs_hDXF);
              }

            gs_hDXF=NULL;
            CString P(BaseGrfSymbolFiles());
            FnB.Format("%s%s.dxf", P, BlkName());
            FnH=P+"DXFHead.txt";
            FnT=P+"DXFTail.txt";

            if (!FileExists((LPTSTR)(LPCTSTR)FnB))
              {
              gs_DBHandle=1;
              gs_hDXF=fopen(FnB, "wt");
              FILE *h=fopen(FnH, "rt");
              if (h)
                {
                char Buff[1204];
                while (fgets(Buff, sizeof(Buff), h))
                  fputs(Buff, gs_hDXF);
                fclose(h);
                }
              }
            }
          }
        else
          {
          //dbgpln("Skip Block %s", pName);
          //b=NULL;
          b->Clear();
          }
        break;
      case DD_Scale:
        pNGI->SetScale(pNGI->GetDVal()/1000.0);
        break;
      case DD_In:
      case DD_Out:
        pIOPt=pNGI->GetPChar();
        if (b)
          {
          double Xl, Xh, Yl, Yh;
          Xl=Xh=Yl=Yh=0.0;
          b->SetCurrentLayer(IOPt_Layer);
          //dbgpln("IOPt  %s", pC);
          if(pNGI->GetCoOrd(X,Y))
            {
            LOADPT(pt, X, Y);


            Xl=Xh=X;
            Yl=Yh=Y;
            C3=b->Add_PLine_Start(pt.p());
            while (pNGI->GetCoOrd(X,Y))
              {
              LOADPT(pt, X, Y);
              Xl=Min(Xl,X);
              Xh=Max(Xh,X);
              Yl=Min(Yl,Y);
              Yh=Max(Yh,Y);
              b->Add_PLine_Vertex(C3, pt.p());
              }
            }
          double H=Max(1.,Yh-Yl);
          double W=Max(1.,Xh-Xl);
          if (W>=H)
            {
            LOADPT(pt, Xl, Yl);
            double WScl=W/(H*Max(1,(int)strlen(pIOPt)));
            Text_Settings TS(H, WScl, 0.0, GR_LIGHTGRAY, Text_Left,Text_Baseline, "STANDARD");
            b->Add_Text(pIOPt, pt.p(), TS);
            }
          else
            {
            double W=Max(1.,Yh-Yl);
            double H=Max(1.,Xh-Xl);
            double WScl=W/(H*Max(1,(int)strlen(pIOPt)));
            LOADPT(pt, Xh, Yl);
            Text_Settings TS(H, WScl, 90.0, GR_LIGHTGRAY, Text_Left,Text_Baseline, "STANDARD");
            b->Add_Text(pIOPt, pt.p(), TS);
            }
          }
        else
          while (pNGI->GetCoOrd(X,Y))
            {
            };
        break;
      case DD_Dynm:
        if (b)
          {
          pVar = pNGI->GetAChar();
          pType = pNGI->GetAChar();
          b->SetCurrentLayer(Drawing_Layer);
          char newname[100],newnum[100];
          (void)sprintf(newnum,"_sf%d",subfig++);
          strcpy(newname,pName);
          strcat(newname,newnum);
          dbgpln("Build Nested Dynamic Block %s", pName);
          Pt_3f basept;
          basept.Zero();
          bmal = Add_Block(newname, basept.p());
          if(pNGI->GetCoOrd(X,Y))
            {
            LOADPT(pt, X, Y);
            C3=bmal->Add_PLine_Start(pt.p());
            while (pNGI->GetCoOrd(X,Y))
              {
              LOADPT(pt, X, Y);
              bmal->Add_PLine_Vertex(C3, pt.p());
              }
            }
          // now need an insert
          Pt_3f sc,pti,ptt;
          REAL s = 1.0;                  // 2.5 mm
          sc.Set(s,s,s);
          pti.Set(0.0,0.0,0.0);
          ptt.Set(0.,s,0.);
          REAL ang = 0.0;
          Attr_Settings AS = Attr_Settings (50, 0, 1.5, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd,Text_Baseline, "STANDARD");
          DXF_ENTITY insert = b->Create_Insert(newname,pti,GR_LIGHTCYAN);
          Add_Insert_Attrib(insert, "DYNAM_VAR", pVar, pti.p(), AS);
          Add_Insert_Attrib(insert, "DYNAM_TYP", pType, pti.p(), AS);
          }
        else
          {
          pVar = pNGI->GetAChar();
          pType = pNGI->GetAChar();
          while (pNGI->GetCoOrd(X,Y))
            {
            };
          }
        break;
      case DD_Poly:
        if (b)
          {
          b->SetCurrentLayer(Drawing_Layer);
          if(pNGI->GetCoOrd(X,Y))
            {
            LOADPT(pt, X, Y);
            C3=b->Add_PLine_Start(pt.p());
            while (pNGI->GetCoOrd(X,Y))
              {
              LOADPT(pt, X, Y);
              b->Add_PLine_Vertex(C3, pt.p());
              }
            if (gs_hDXF)
              {
              fprintf(gs_hDXF, "0\nSEQEND\n  5\n%i\n  8\n0\n",gs_DBHandle++);
              }
            }
          }
        else
          while (pNGI->GetCoOrd(X,Y))
            {
            };
        break;
      case DD_Arc:
        if (pNGI->GetCoOrd(X,Y))
          {
          double Rad=pNGI->GetLength();
          double S=0,E=360;
          pNGI->GetPair(S,E);
          if (b)
            {
            LOADPT(pt, X, Y);
            b->SetCurrentLayer(Drawing_Layer);
            C3=b->Add_Arc(pt.p(), Rad, Degs2Rads(S), Degs2Rads(E));
            }
          }
        break;
      case DD_Arc3:
        {
        double Xs,Xm,Xe,Ys,Ym,Ye;
        if (pNGI->GetCoOrd(Xs,Ys))
          if (pNGI->GetCoOrd(Xm,Ym))
            if (pNGI->GetCoOrd(Xe,Ye))
              if (b)
                {
                Pt_3f pts, ptm, pte;
                LOADPT(pts, Xs, Ys);
                LOADPT(ptm, Xm, Ym);
                LOADPT(pte, Xe, Ye);
                b->SetCurrentLayer(Drawing_Layer);
                C3=b->Add_Arc(pts.p(), ptm.p(), pte.p());
                }
              break;
        }
      case DD_End:
        NdNo++;
        ASSERT(NdNo<MAXNDNO);
        b=NULL;
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }
  if (gs_hDXF)
    {
    FILE *h=fopen(FnT, "rt");
    if (h)
      {
      char Buff[1204];
      while (fgets(Buff, sizeof(Buff), h))
        fputs(Buff, gs_hDXF);
      fclose(h);
      }
    fclose(gs_hDXF);
    }
  return NdNo;
  }

//----------------------------------------------------------------------------

flag DXF_Drawing::GetNodeGrfIOs(pNodeGrfInfo pNGI, pchar NodeName, long ReqdIOs, Strng_List &IOL)
  {
  IOL.Clear();

  if (pNGI==NULL)
    return False;

  double X,Y;
  pchar pName, pIOPt,pVar,pType;

  // Scan for Extents
  flag Found=0;
  for (long Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        pName=pNGI->GetPChar();
        Found=(_stricmp(NodeName, pName)==0);
        break;
      case DD_Scale:
        pNGI->SetScale(pNGI->GetDVal()/1000.0);
        break;
      case DD_In:
      case DD_Out:
        pIOPt=pNGI->GetPChar();
        if (Found && (Code == ReqdIOs))
          IOL.Append(pIOPt);
        while (pNGI->GetCoOrd(X, Y))
          {
          };
        break;
      case DD_Poly:
        while (pNGI->GetCoOrd(X, Y))
          {
          };
        break;
      case DD_Arc:
        if (pNGI->GetCoOrd(X,Y))
          {
          double Rad=pNGI->GetLength();
          double S=0,E=360;
          pNGI->GetPair(S,E);
          }
        break;
      case DD_Arc3:
        while (pNGI->GetCoOrd(X, Y))
          {
          };
        break;
      case DD_Dynm:
        pVar  = pNGI->GetAChar();
        pType = pNGI->GetAChar();
        while (pNGI->GetCoOrd(X, Y))
          {
          };
        break;
      case DD_End:
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }

  return IOL.Length()>0;
  }

//===========================================================================

void Big_Curs(HDC my_HDC, int x0, int y0)
  {
  POINT Pt;
  MoveToEx(my_HDC, x0, -32000, &Pt);
  LineTo(my_HDC, x0,  32000);
  MoveToEx(my_HDC, -32000, y0, &Pt);
  LineTo(my_HDC,  32000, y0);
  }

//---------------------------------------------------------------------------

void Rect_Curs(HDC my_HDC, int x0, int y0, int x1, int y1)
  {
  POINT Pt;
  MoveToEx(my_HDC, x0,y0, &Pt);
  LineTo(my_HDC, x1,y0);
  LineTo(my_HDC, x1,y1);
  LineTo(my_HDC, x0,y1);
  LineTo(my_HDC, x0,y0);
  }

//---------------------------------------------------------------------------

void Rubber_Curs(HDC my_HDC, int x0, int y0, int x1, int y1)
  {
  POINT Pt;
  MoveToEx(my_HDC, x0,y0, &Pt);
  LineTo(my_HDC, x1,y1);
  }

//---------------------------------------------------------------------------

void Image_Curs(HDC my_HDC, Pt_3i Base, Pt_3i Curnt, int N, Pt_3i * Pt)
  {
  dbgpln("Image_Curs %5i %5i",Curnt.X,Curnt.Y);
  POINT tPt;
  int xoff=Curnt.X-Base.X;
  int yoff=Curnt.Y-Base.Y;
  int i=0;
  MoveToEx(my_HDC, xoff+Pt[i].X, yoff+Pt[i].Y, &tPt);
  for ( ;i < N; i++)
    LineTo(my_HDC, xoff+Pt[i].X, yoff+Pt[i].Y);
  }

//============================================================================

void DList_Curs(HDC my_HDC, Pt_3i Base, Pt_3i Curnt, CEntInView* pEnt)
  {
  int xoff=Curnt.X-Base.X;
  int yoff=Curnt.Y-Base.Y;
  pEnt->Qdisplay(xoff * -1,yoff * -1);
  }

//============================================================================

void Grf3Drw_Entry()
  {
  for (int i=0; i < N_HDR_VARS; i++)
    if (strlen(HdrVarNames[i]))
      {
      HdrVarSize[i] = dxf_hdr_var_sizeof(HdrVarNames[i]);
      HdrVarSize[i] = 0;
      }
    else
      HdrVarSize[i] = 0;
  }

/*KGA 27/5/96: This class was implemented to try fix some memory leaks...*/
class GrfEntryExit
  {
  public:
    ~GrfEntryExit();
  };

GrfEntryExit::~GrfEntryExit()
  {
  free_buffer_points();
  free_buffer_entries();
  free_buffer_type();
  free_mypolypoly_buffer();
  }

GrfEntryExit GEE;

//#pragma startup Init

//============================================================================
