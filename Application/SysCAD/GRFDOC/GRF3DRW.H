//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GRF3DRW_H
#define  __GRF3DRW_H

#ifndef __DXFSTUFF_H
#include "dxfstuff.h"
#endif
#ifndef __GPWFUNCS_H
#include "gpwfuncs.h"
#endif
#ifndef  __NODEDRAW_H
#include "nodedraw.h"
#endif
#ifndef  __VECTORS_H
#include "vectors.h"
#endif
//#include "executiv.h"
#include "sfe_base.h"

//#if defined(__GRF3DRW_CPP) || defined(__GRF3DSP_CPP)
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif
#define WithLineThickness 0

#import "..\scdlib\vbscript.tlb" no_namespace rename("RGB","rgb")

//===========================================================================
#define UseNewAnnot 1

#define LINT_ARGS
//===========================================================================

_FWDDEF(CLVector)
_FWDDEF(DynamicSegment)
class CEntInView; //_FWDDEF(CEntInView)
_FWDDEF(Viewport)
_FWDDEF(Grf3D_Display)
_FWDDEF(Pt_3i)
_FWDDEF(Pt_2f)
_FWDDEF(Pt_3f)
_FWDDEF(Pt_SLW)
_FWDDEF(Attr_Settings)
_FWDDEF(Text_Settings)
//_FWDDEF(CGrfLayer)
_FWDDEF(LayerCollection)
_FWDDEF(Block)
_FWDDEF(BlockCollection)
_FWDDEF(DXF_Drawing)

//===========================================================================

const byte MSStyle_Off      = 0;
const byte MSStyle_Solid    = 1;
const byte MSStyle_LDash    = 2;
const byte MSStyle_SDash    = 3;
const byte MSStyle_Dots     = 4;

//---------------------------------------------------------------------------

const LPTSTR TagAttribStr         = "Tag";
const LPTSTR AssocTagAttribStr    = "AssocTag";
const LPTSTR HideInsertAttribStr  = "HideInsert";
const LPTSTR ValuesAttribStr      = "Values";
const LPTSTR GuidAttribStr        = "Guid";

extern bool EntityIsHidden(DXF_ENTITY e);

//---------------------------------------------------------------------------

class CGrfHelper
  {
  public:
    CGrfHelper();
    ~CGrfHelper();
    GR_COLOR  GR_BACKGROUND;
    COLORREF  BackGroundRGB;
    COLORREF  DigPenRGB;
    CBrush*   pBrush;
    CPen*     pDigPen;

    DWORD     iShowMdlStatusInActive:4,
              iShowMdlStatusError:4,
              iShowMdlStatusWarning:4,
              iShowMdlStatusTear:4,
              iShowMdlStatusMacMdl:4,
              iShowMdlStatusOff:4,
              iShowMdlStatusOn:4,
              iShowMdlStatusLFlwVap:4,
              iShowMdlStatusLFlwSol:4,
              iShowMdlStatusLFlwP:4,
              iShowMdlStatusLFlwN:4,
              iShowMdlStatusLNoFlw:4,
              iShowMdlStatusClear:4,
              iShowMdlStatusUnKnown:4,
              iShowMdlStatusXferLnk:4,
              iShowMdlStatusSimpleLnk:4,
              iShowMdlStatusLinearLnk:4,
              iShowMdlStatusFullLnk:4,
              iShowMdlStatusNettQmP:4,
              iShowMdlStatusNettQmN:4,
              iShowMdlStatusUFlw:4,
              iShowMdlStatusUNoFlw:4,
              iShowMdlStatusIsSrc:4,
              iShowMdlStatusIsSnk:4;

    void MdlStatusColor(dword Status, int &Color, int &OvrColor, int &OvrStyle);
  };

extern CGrfHelper GrfHelper;

//---------------------------------------------------------------------------

class Pt_3i
  {
  public:
    int X,Y,Z;

    Pt_3i() { X = 0; Y = 0; Z = 0; };
    Pt_3i(int Xi, int Yi=0, int Zi=0) { X = Xi; Y = Yi; Z = Zi; };
    ~Pt_3i() {};
    int& operator[](const int i){return ((int*)&X)[i];}
    void Zero() { X = 0; Y = 0; Z = 0; };
    void Set(int Xi=0, int Yi=0, int Zi=0) { X=Xi; Y=Yi; Z=Zi; };
    int *p() { return (int*)&X; };
    void FromWorld(VP_VIEWPORT vp, Pt_3f &World, int What=7);
  };

//---------------------------------------------------------------------------

class Pt_2f
  {
  public:
    REAL X,Y;

    Pt_2f() { X = 0.0; Y = 0.0; };
    Pt_2f(REAL Xi, REAL Yi=0.0) { X = Xi; Y = Yi; };
    ~Pt_2f() {};
    REAL& operator[](const int i) { return ((REAL*)&X)[i]; };
    void Zero() { X = 0.0; Y = 0.0; };
    void Set(REAL Xi=0.0, REAL Yi=0.0) { X = Xi; Y = Yi; };
    REAL* p() { return (REAL*)(&X); };
    REAL GetX() { return X; }
    REAL GetY() { return Y; }
  };

//---------------------------------------------------------------------------

class Pt_3f
  {
  public:
    REAL X,Y,Z;

    Pt_3f() { X = 0.0; Y = 0.0; Z = 0.0; };
    Pt_3f(REAL Xi, REAL Yi=0.0, REAL Zi=0.0) { X = Xi; Y = Yi; Z = Zi; };
    Pt_3f(const PT3 p) { X = p[0]; Y = p[1]; Z = p[2]; };
    ~Pt_3f() {};

    REAL& operator[](const int i) { return ((REAL*)&X)[i]; }
    Pt_3f operator+(Pt_3f & Pt);
    Pt_3f operator-(Pt_3f & Pt);
    Pt_3f operator*(Pt_3f & Pt);

    void Zero() { X = 0.0; Y = 0.0; Z = 0.0; };
    void Set(REAL Xi=0.0, REAL Yi=0.0, REAL Zi=0.0) { X = Xi; Y = Yi; Z = Zi; };
    void Set(PT3 Pt) { X = Pt[0]; Y = Pt[1]; Z = Pt[2]; };
    REAL Dist(Pt_3f & Pt) { return c3v_dist(p(), Pt.p()); };
    void Unit();
    void Scale(double Scale);

    REAL* p() { return (REAL*)&X; };

    REAL x() { return X; }
    REAL y() { return Y; }
    REAL z() { return Z; }

    REAL *px() { return &X; }
    REAL *py() { return &Y; }
    REAL *pz() { return &Z; }
  };


typedef CArray <Pt_3f, Pt_3f&> Pt_3f_Array;

//---------------------------------------------------------------------------

class DllImportExport Pt_SLW
  {
  public:
    Pt_2f        Screen;
    Pt_3i        Logical;
    Pt_3f        World;

    Pt_SLW(){Zero();};
    ~Pt_SLW() {};
    void Zero();
    void Get(const Pt_SLW &Pt);
    void GetX(const Pt_SLW &Pt);
    void GetY(const Pt_SLW &Pt);
    void GetZ(const Pt_SLW &Pt);
    Pt_SLW operator+=(Pt_SLW &Pt);
    Pt_SLW operator-=(Pt_SLW &Pt);
  };

typedef CArray <Pt_SLW, Pt_SLW&> Pt_SLW_Array;

//---------------------------------------------------------------------------

class Line_2f
  {
  private:
    Pt_2f start,end;

  public:
    void   Set(REAL x1,REAL y1,REAL x2,REAL y2);
    void   Set(Pt_2f start,Pt_2f end);
    REAL   PerpParm(Pt_2f pt,REAL &distance);
    int    LineSegmentCross(Line_2f l1,Line_2f l2,REAL &parm1,REAL &parm2);
  };

//===========================================================================

enum PageSizes  {A0_Page, A1_Page, A2_Page, A3_Page, A4_Page, A5_Page};
enum Text_Hjust {Text_Left, Text_Cntrd, Text_Right};
enum Text_Vjust {Text_Bottom,Text_Baseline,Text_Middle,Text_Top,Text_Align,Text_Fit};
enum DiscTypes  {Disc_None,Disc_Instr};

enum  LayerTypes            {Drawing_Layer,  IOPt_Layer, Connection_Layer, Construction_Layer, Annotation_Layer,   Dynamic_Layer,   Pid_Equip,   Pid_Error,     Pid_New,   Pid_Disused,   Zero_Layer};
const pchar LayerNames[]  = {   "SCD_DRAW",  "SCD_IOPT",       "SCD_CONN",         "SCD_CONS",        "SCD_ANN",       "SCD_DYN", "PID_EQUIP", "PID_ERROR",   "PID_NEW", "PID_DISUSED",   "0",NULL};
const int   LayerColors[] = {      GR_CYAN,    GR_GREEN,         GR_WHITE,             GR_RED,        GR_YELLOW,        GR_GREEN,   GR_YELLOW,      GR_RED,          -1,      GR_GREEN,   GR_WHITE,NULL};
const pchar LayerFonts[]  = { "CONTINUOUS","CONTINUOUS",     "CONTINUOUS",       "CONTINUOUS",     "CONTINUOUS",    "CONTINUOUS","CONTINUOUS","CONTINUOUS","CONTINUOUS",  "CONTINUOUS",   "CONTINUOUS",NULL};

#define Attrib_InVisible DXF_ATTRIB_INVIS

extern const float  A0_PageWidth; // millimeters

extern void MakeLayerName(pchar FullNm, LayerTypes LayerType, pchar CGrfLayer);

#define Load_Tables  0x0001
#define Load_Blocks  0x0002
#define Load_Drawing 0x0004
#define Load_All     0x0007
#define Load_Merge   0x0006

//===========================================================================

const byte DynFill = 0;
const byte DynBars = 1;
const byte DynToggle = 2;

const double ArrowLeftSz = 1.0;
const double ArrowUpSz = 0.2;

//===========================================================================

#define N_HDR_VARS 112

typedef struct Entity_List_Info
  {
  DML_LIST lst;
  DML_ITEM itm;
  C3_BOX_S Box;
  } Entity_List_Info;

inline DML_LIST    Attr_List(DXF_ENTITY e) {return DXF_INSERT_ATTRS(e);};
extern DXF_ENTITY  First_Ent(DML_LIST lst, Entity_List_Info &Info);
extern DXF_ENTITY  Next_Ent(Entity_List_Info &Info);
extern DXF_ENTITY  Find_Attr(DXF_ENTITY Insert, pchar Tag);
extern flag        Reset_Attr(DXF_ENTITY Insert, pchar Tag,pchar NewVal);
extern pchar       Find_Attr_Value(DXF_ENTITY Insert, pchar Tag);
extern int         Find_Attr_AFlags(DXF_ENTITY Insert, pchar Tag);
extern int         Find_Attr_GFlags(DXF_ENTITY Insert, pchar Tag);
extern bool        IsArrow(C3_CURVE c, double & Scale);

inline pchar       Attr_Tag(DXF_ENTITY e) {return (e ? DXF_ATTRIB_TAG_GET(e) : NULL);};
//inline pchar       Attr_Prompt(DXF_ENTITY e) {return (e ? DXF_ATTRIB_PROMPT(e) : NULL);};
inline pchar       Attr_Value(DXF_ENTITY e) {return DXF_ATTRIB_VALUE_GET(e);};
inline REAL       &Attr_Size(DXF_ENTITY e) {return DXF_ATTRIB_HEIGHT(e);};
inline REAL       &Attr_Rotation(DXF_ENTITY e) {return DXF_ATTRIB_ROT_ANG(e);};
inline REAL       *Attr_Pt(DXF_ENTITY e) {return DXF_ATTRIB_PT(e);};
inline int        &Attr_AFlags(DXF_ENTITY e) {return DXF_ATTRIB_AFLAGS(e);};
inline int        &Attr_GFlags(DXF_ENTITY e) {return DXF_ATTRIB_GFLAGS(e);};
inline flag        Attr_Is(DXF_ENTITY e, pchar RqdTag) {return (_stricmp(DXF_ATTRIB_TAG_GET(e), RqdTag)==0);};
inline void        Show_Attr(DXF_ENTITY e) {DXF_ATTRIB_AFLAGS(e) &= ~Attrib_InVisible;};
inline void        Hide_Attr(DXF_ENTITY e) {DXF_ATTRIB_AFLAGS(e) |= Attrib_InVisible;};
inline REAL       *Insert_BasePt(DXF_ENTITY Ins) {return DXF_INSERT_PT(Ins);};
inline pchar       Insert_Block(DXF_ENTITY Ins) {return DXF_INSERT_BLOCK_GET(Ins);};
inline REAL       &Insert_Rotation(DXF_ENTITY Ins) {return DXF_INSERT_ROT_ANG(Ins);};

//----------------------------------------------------------------------------

class Attr_Settings
  {
  public:
    Attr_Settings()
      {
      Length =  50;
      Flags  =  0;
      Size   =  3.0;
      XScl   =  1.0;
      Rot    =  0.0;
      Color  =  GR_RED;
      Hjust  =  Text_Cntrd;
      Vjust  =  Text_Middle;
      Style  =  "STANDARD";
      }

    Attr_Settings(int LengthIn, int FlagsIn, REAL SizeIn, REAL XSclIn, REAL RotIn, int ColorIn, Text_Hjust HjustIn,Text_Vjust VjustIn, pchar StyleIn)
      {
      Length = LengthIn;
      Flags =  FlagsIn;
      Size =   SizeIn;
      XScl =   XSclIn;
      Rot =    RotIn;
      Color =  ColorIn;
      Hjust =  HjustIn;
      Vjust =  VjustIn;
      Style =  StyleIn;
      }

  public:
    int        Length;
    int        Flags;
    REAL       Size;
    REAL       XScl;
    REAL       Rot;
    int        Color;
    Text_Hjust Hjust;
    Text_Vjust Vjust;
    pchar      Style;

  };

//---------------------------------------------------------------------------

class Text_Settings
  {
  public:
    Text_Settings(REAL SizeIn, REAL XSclIn, REAL RotIn, int ColorIn, Text_Hjust HjustIn,Text_Vjust VjustIn, pchar StyleIn)
      {
      Size =   SizeIn;
      XScl =   XSclIn;
      Rot =    RotIn;
      Color =  ColorIn;
      Hjust =  HjustIn;
      Vjust =  VjustIn;
      Style =  StyleIn;
      }

  public:
    REAL       Size;
    REAL       XScl;
    REAL       Rot;
    int        Color;
    Text_Hjust Hjust;
    Text_Vjust Vjust;
    pchar      Style;

  };

//---------------------------------------------------------------------------

class CGrfTagInfo
  {
  public:
    CGrfTagInfo()
      { 
      m_Node.m_X=0.0;
      m_Node.m_Y=0.0;
      m_Node.m_Z=0.0; 
      m_Node.m_XScale=1.0;
      m_Node.m_YScale=1.0;
      m_Node.m_ZScale=1.0; 
      m_Node.m_Rotation=0.0; 
      m_Node.m_Visible=true; 
      m_Tag.m_X=0.0;
      m_Tag.m_Y=0.0;
      m_Tag.m_Z=0.0; 
      m_Tag.m_XScale=1.0;
      m_Tag.m_YScale=1.0;
      m_Tag.m_ZScale=1.0; 
      m_Tag.m_Rotation=0.0; 
      m_Tag.m_Visible=true; 
      m_bValid=0; 
      m_bHasTag=0; 
      m_LoBnd.m_X=0.0;
      m_LoBnd.m_Y=0.0;
      m_LoBnd.m_Z=0.0; 
      m_HiBnd.m_X=0.0;
      m_HiBnd.m_Y=0.0;
      m_HiBnd.m_Z=0.0; 
      e=NULL; 
      };

  public:
    Strng m_sTag;
    Strng m_sSymbol;
    Strng m_sClass;
    Strng m_sDrwGroup;
    Strng m_sGuid;
    flag m_bValid;
    flag m_bHasTag;
    struct CSymbol{
      double m_X;
      double m_Y;
      double m_Z;
      double m_XScale;
      double m_YScale;
      double m_ZScale;
      double m_Rotation;
      BOOL   m_Visible;
      } m_Node, m_Tag;
    struct CPoint{
      double m_X;
      double m_Y;
      double m_Z;
      } m_LoBnd, m_HiBnd;

    DXF_ENTITY e;
    long PointsCount;

  };

class LinePoint
  {
  public:
    LinePoint()
      { x=y=z=0.0; seq=0; };
    LinePoint(long seq_, double x_, double y_, double z_)
      { x=x_; y=y_; z=z_; };
    void Set(long seq_, double x_, double y_, double z_)
      { seq=seq_; x=x_; y=y_; z=z_; };

  public:
    long seq; //sequence number (skip one for line break)
    double x;
    double y;
    double z;
  };

class CLinePointsArray : public CArray<LinePoint, LinePoint&>
  {
  public:
  };

class CGrfTagInfoArray : public CArray<CGrfTagInfo, CGrfTagInfo&>
  {
  public:
  };

//---------------------------------------------------------------------------

class DllImportExport CMdlAssocGrfX
  {
  public:
    enum eActions { eNull, eAddNdAssocGrfs, eKeep, eShow, eHide, eDelete, eChgTag };
    CMdlAssocGrfX()
      {
      m_eAction        = eNull; 
      m_pAssocGrf        = NULL;
      m_iAssoc         = 0;
      m_NdX            = 0;
      m_NdY            = 0;
      m_Entity         = NULL;
      };
    CMdlAssocGrfX(eActions eAction, CMdlAssocGrf * pAssocGrf, int iAssoc, double NdX, double NdY, DXF_ENTITY Entity, LPCSTR NewTag, LPCSTR NewNdTag)
      {
      m_eAction        = eAction;
      m_pAssocGrf      = pAssocGrf;
      m_iAssoc         = iAssoc;
      m_NdX            = NdX;
      m_NdY            = NdY;
      m_Entity         = Entity;
      m_sNewTag        = NewTag;
      m_sNewNdTag      = NewNdTag;
      };

  public:

    eActions            m_eAction;
    CMdlAssocGrf      * m_pAssocGrf;
    int                 m_iAssoc;
    double              m_NdX;
    double              m_NdY;
    DXF_ENTITY          m_Entity;
    CString             m_sNewTag;
    CString             m_sNewNdTag;
  };

class CGrfLayer
  {
  public:
    CGrfLayer(pDXF_Drawing DrawingIn, pBlock BlockIn);
    ~CGrfLayer();

    flag             ChangeTag(pchar pOldTag, pchar pNewTag);
    void             CollectTags(Strng_List & TagList);
    void             CollectTagInfo(DXF_Drawing * pDrawing, bool AllInserts, CGrfTagInfoArray & GTIA);
    flag             FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr);
    flag             PutModelAssocGrfsChanges(CMdlAssocGrfArray & Grfs, CMdlAssocGrfMap & Map, CList<CMdlAssocGrfX,CMdlAssocGrfX&> & GrfsFound);   
    
    flag             GetEntityBox(C3_BOX_S *box,DXF_ENTITY e,double *BiggestDim = NULL);
    flag             GetCurveBox(C3_BOX_S *box,C3_CURVE c,C3_TRANSFORM c3t,double *BiggestDim = NULL);
    flag             GetCurveBox(C3_BOX_S *box,C3_CURVE c,double *BiggestDim = NULL);
    void             Add(DXF_ENTITY e);
    void             Add(C3_CURVE c);
    void             Put(DXF_FILE f);
    void             Clear();
    void             CountBlocksUsed(CWordArray& BlkCnt);
    void             Draw(pViewport vp, C3_BOX box, int color, int Lvl,CGrfLayer * pLay,CGrfLayer * iLay);
    DML_LIST         SelectInsertsOnAttrCombo(DML_LIST l,pchar *AttrTags,pchar *AttrVals, IRegExpPtr *pRE);
    DML_LIST         SelectNearbyCurves(DML_LIST l,PT3 p,REAL tol);
    void             GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds);
    void             GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds, DML_LIST TheCurveLst, DML_LIST TheInsertLst, DML_LIST TheOtherLst);
    void             Explode(C3_TRANSFORM x, DXF_ENTITY insert, pBlock  b);
    DXF_ENTITY       ClosestInsert(PT3 Point, REAL &DistMin);
    DXF_ENTITY       ClosestEntity(PT3 Point, REAL &DistMin);
    C3_CURVE         ClosestCurve(PT3 Point, REAL &DistMin);
    void             EntityInvalidate(DXF_ENTITY e = NULL,C3_CURVE c = NULL);
    void             PrintEntities(pViewport vp,FILE *fp);
    DXF_ENTITY       Find_Tagged_Insert(pchar pVal);
    void             SetDiscColor(DML_LIST dlay = NULL,int on= NULL);

  public:
    DXF_TABLE_ENTRY  Def;            // Pointer to this layers info
    DML_LIST         CurveLst;
    DML_LIST         InsertLst;
    DML_LIST         OtherLst;
    int              m_Color;
    int              m_DiscColor;
    double           m_Thickness;
    CGrfLayer *      Nxt;
    pDXF_Drawing     Drawing;        // Pointer to parent drawing
    pBlock           Block;          // pointer to parent block NULL if drawing else defined ( Ithink mhm )

  };

//---------------------------------------------------------------------------

class LayerCollection
  {
  public:
    LayerCollection();
    LayerCollection(pDXF_Drawing DrawingIn, pBlock BlockIn);
    ~LayerCollection();

    void             SetCurrentLayer(LayerTypes Typ, pchar LayName = NULL);
    void             SetCurrentLayer(pchar LayName);
    pchar            CurrentLayer() { return CurLayer; };
    BOOL             SetLayerState(pViewport vp, pchar LayName, BOOL set, int state);
    BOOL             SetLayerColor(Viewport * vp, pchar LayName, int color);
    void             CountBlocksUsed(CWordArray& BlkCnt);
    CGrfLayer *           Find(pchar LayerName);
    CGrfLayer *           Find(LayerTypes Typ) {return Find(LayerNames[Typ]);};
    CGrfLayer *           Append(DXF_TABLE_ENTRY layer);
    void             Free(CGrfLayer * layer);
    void             Get(DXF_FILE file,int What,int test_mode);
    void             Put(DXF_FILE file);
    void             Draw(Viewport * vp, C3_BOX box, int color, int Lvl,CGrfLayer * iLay);
    DML_LIST         SelectInsertsOnAttrCombo(DML_LIST l,pchar *AttrTag,pchar *AttrVal, IRegExpPtr *pRE);
    DML_LIST         SelectNearbyCurves(DML_LIST l,PT3 p,REAL tol);
    void             Explode(C3_TRANSFORM x, DXF_ENTITY insert, pBlock  b);
    void             GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds);
    void             GetBounds(C3_TRANSFORM c3t, C3_BOX Bounds, DML_LIST TheCurveLst, DML_LIST TheInsertLst, DML_LIST TheOtherLst);
    void             Clear();
    DXF_ENTITY       ClosestEntity(PT3 Point, REAL &DistMin);
    DXF_ENTITY       ClosestInsert(PT3 Point, REAL &DistMin);
    C3_CURVE         ClosestCurve(PT3 Point, REAL &DistMin);
    void             AttrValuesInWin(pchar AttrTag, PT3 mn, PT3 mx, pStrng_List &Lst);

    flag             TranslateEntity(DXF_ENTITY e,Pt_3f frompos,Pt_3f topos);
    flag             TranslateEntity(C3_CURVE c,Pt_3f frompos,Pt_3f topos);
    DXF_ENTITY       Add_Entity(DXF_ENTITY e);
    DXF_ENTITY       Add_Insert(DXF_ENTITY e);
    C3_CURVE         Add_Curve(C3_CURVE c);
    C3_CURVE         Add_Square(double x,double y,double w,double h);
    C3_CURVE         Add_PLine_Start(PT3 pt);
    C3_CURVE         Add_PLine_Vertex(C3_CURVE PLine, PT3 pt);
    C3_CURVE         Add_Arc(PT3 pt, REAL r, REAL s, REAL e);
    C3_CURVE         Add_Arc(PT3 pts, PT3 ptm, PT3 pte);
    C3_CURVE         Add_Circle(PT3 pt, REAL r){ PT3 a; a[0] = 0.0; a[1] = 0.0; a[2] = 1.0; return Add_Circle(pt,r,a); };
    C3_CURVE         Add_Circle(PT3 pt, REAL r, PT3 z);
    C3_CURVE         Add_Circle(PT3 pt1, PT3 pt2, PT3 pt3);

    DXF_ENTITY       Add_Insert_Attrib(DXF_ENTITY Insert, pchar Tag, pchar Value,
                                       PT3 Pt, Attr_Settings &Set);


    DXF_ENTITY       Create_Insert(pchar Typ, Pt_3f Pt,int Color = -1, Pt_3f Scl = Pt_3f(1.0,1.0,1.0), REAL Rot = 0.0,
                                pchar TagValue = (pchar)NULL, pchar AssocTagValue =  (char*)NULL, Pt_3f TagPt = Pt_3f(0.0,0.0,0.0),
                                Attr_Settings &Set = Attr_Settings (50,0,1.5,1.0,0.0,GR_DARKGRAY,Text_Cntrd,Text_Baseline,"STANDARD"));

    //DXF_ENTITY       Create_Insert(pchar Typ, PT3 Pt, int Color, PT3 Scl, REAL Rot );

    //DXF_ENTITY       Create_Insert(pchar Typ, PT3 Pt ,int Color);

    DXF_ENTITY       Add_Text(pchar Txt, PT3 Pt, Text_Settings &Set);
    DXF_ENTITY       Find_Text(pchar Txt);
    DXF_ENTITY       Add_Attrib_Defn(pchar Tag, pchar Prompt, pchar Value,
                                     PT3 Pt, Attr_Settings &Set);
    DXF_ENTITY       Find_Attrib_Defn(pchar Tag);


    CEntInView*      IsEntitySelected(Viewport * v, DXF_ENTITY pEnt);
    CEntInView*      IsCurveSelected(Viewport * v, C3_CURVE c);
    long             TranslateEntitiesInWin(Viewport * vp,PT3 S1, PT3 S2, PT3 E1, PT3 E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred);
    long             TranslateEntitiesInList(Viewport * vp,PT3 S1, PT3 S2, PT3 E1, PT3 E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred);

    void             ChangeLayer(DXF_ENTITY e,LayerTypes NewLayer);
    void             ChangeLayer(C3_CURVE   c,LayerTypes NewLayer);
    CEntInView*      FindEntInView(DXF_ENTITY e);
    CEntInView*      FindEntInView(C3_CURVE c);
    DML_ITEM         FindItem(DXF_ENTITY e);
    DML_ITEM         FindItem(C3_CURVE c);
    CGrfLayer *      FindLayer(DXF_ENTITY e);
    CGrfLayer *      FindLayer(C3_CURVE c);
    DXF_ENTITY       FindInsertNear(REAL X, REAL Y, REAL Z, REAL Tol);
    flag             Delete(DXF_ENTITY e);
    flag             Delete(C3_CURVE c);
    flag             Hide(DXF_ENTITY e, flag On);
    //flag             Hide(C3_CURVE c, flag On);
    flag             RemoveFromLayer(DXF_ENTITY e);
    flag             RemoveFromLayer(C3_CURVE c);
    flag             Exists(DXF_ENTITY e);
    flag             Exists(C3_CURVE c);
    void             PrintEntities(Viewport * vp,FILE *fp);
    DXF_ENTITY       Find_Tagged_Insert(pchar pVal);
    void             EntityInvalidate(DXF_ENTITY e = NULL,C3_CURVE c = NULL);
    void             SetDiscColor(DML_LIST dlay = NULL,int on= NULL);
    flag             TopOfList(C3_CURVE c);

    void             ChangeTag(pchar pOldTag, pchar pNewTag);
    void             CollectTags(Strng_List & TagList);
    flag             FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr);
    flag             PutModelAssocGrfsChanges(CMdlAssocGrfArray & Grfs, CMdlAssocGrfMap & Map, CList<CMdlAssocGrfX,CMdlAssocGrfX&> & GrfsFound);   

  public:
    CGrfLayer *           Layers;    // Pointer to my first layer
    pDXF_Drawing     Drawing;   // Pointer to parent drawing
    pBlock           Block;     // Pointer to Parent block;
    char             CurLayer[128];

  };

//---------------------------------------------------------------------------

class Block : public LayerCollection
  {
  public:
    Block(DXF_Drawing* DrawingIn);
    ~Block();
    void             Get(DXF_FILE file, int What, int test_mode);
    void             Put(DXF_FILE file);
    void             CountBlocksUsed(DXF_Drawing* pDrawing, CWordArray& BlkCnt);
    flag             Find_Attdef_Settings(pchar Tag,Attr_Settings &Set,Pt_3f &pt);
    flag             Find_TextToAttrib(pchar Txt, Pt_3f &Pos);
    //void             SetDiscColor(DML_LIST dlay = NULL,int on= NULL);

  public:
    DXF_BLOCK         Def;       // this block DXF header info
    Block            *Nxt;       // Pointer to Next block in collection
    DXF_Drawing      *Drawing;   // Pointer to parent drawing
    char              IsNew:1,   //
//                      IsScd:1,   // Has SysCAD added/created this block
                      SaveIt:1,  // Save this block to dxf
                      SaveSym:1;  // Save this block to dxf as a symbol
  };

//---------------------------------------------------------------------------

class BlockCollection
  {
  public:
    BlockCollection(pDXF_Drawing DrawingIn);
    ~BlockCollection();
    int              Count();
    pBlock           Find(pchar BlkName);
    pBlock           Find(pchar BlkName, int& index);
    pBlock           FindByIndex(int index);
    int              FindUnique(pchar CoreName, int NoFindRet=0);
    void             PrintEntities(Viewport * vp,FILE *fp);
    void             EntityInvalidate(DXF_ENTITY e = NULL,C3_CURVE c = NULL);
    void             SetDiscColor(DML_LIST dlay=NULL,int on=NULL);
    pBlock           Append(DXF_BLOCK block);
    void             Free(pBlock block);
    void             Merge(DXF_FILE file);
    void             Get(DXF_FILE file, int What,int test_mode);
    void             Put(DXF_FILE file);
    pBlock           MakePidBase(pchar BlkName);

  public:
    Block           *Blocks;   // Pointer to first block in list of blocks for the drawing
    DXF_Drawing     *Drawing;  // Pointer to the drawing

  };

//---------------------------------------------------------------------------

class DllImportExport DXF_Drawing : public LayerCollection
  {
  public:
    DXF_Drawing(Grf3D_Display *pDsp,pchar NameIn, flag InitForSyscad);
    ~DXF_Drawing();
    int               Load(pchar NameIn, int What,int test_mode);
    int               Save(pchar NameOut);
    int               SaveSymbols(pchar SymPath, pchar DocGUID);
    pchar             NameOfDrawing();

    void              ConstructInsertMatrix(C3_TRANSFORM Imat,pBlock b,DXF_ENTITY e);
    void              EntityInvalidate(DXF_ENTITY e = NULL,C3_CURVE c = NULL);
    BOOL              PrintEntities(Viewport * vp, pchar file_name);
    flag              CleanupDXF(pchar fn);
    void              SetDiscColor(DML_LIST dlay = NULL,int on = NULL);

    void              Draw(DXF_ENTITY e, Viewport * vp, C3_BOX box, int color, int Lvl,CGrfLayer * pLay,CGrfLayer * iLay,DML_ITEM item);
    void              Draw(C3_CURVE   c, Viewport * vp, C3_BOX box, int color, int Lvl,CGrfLayer * pLay,CGrfLayer * iLay,DML_ITEM item);
    void              Draw(Viewport * vp, C3_BOX SubRgn, pchar LayName);
    void              SetGfxLayer(int Ocol,int Lvl,CGrfLayer * eLay,CGrfLayer * iLay,pchar EntityLay,int EntityColor,double EntityThickness);


    void              Get_Attr_Box(pchar s,Pt_3f Pos,Attr_Settings &Set,double *xmin,double *xmax,double *ymin,double *ymax);
    void              Get_Text_Box(pchar s,Pt_3f Pos,Text_Settings &Set,double *xmin,double *xmax,double *ymin,double *ymax);

    flag              ChangeTag(pchar pOldTag, pchar pNewTag);
    void              CollectTags(Strng_List & TagList);
    void              CollectTagInfo(bool AllInserts, CGrfTagInfoArray & GTIA);
    void              CollectLinkInfo(CGrfTagInfo & GTI, CLinePointsArray & LPA);
    flag              FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr);
    flag              UpdateModelAssocGrfs();   

    DML_LIST          SelectNearbyCurves(PT3 p,REAL tol);
    DML_LIST          SelectNearbyCurves(DML_LIST l,PT3 p,REAL tol);
    DML_LIST          SelectProPipeInstruments();
    DML_LIST          SelectInsertsOnAttrCombo(DML_LIST l,pchar *AttrTag,pchar *AttrVal, IRegExpPtr *pRE);

    void              CountBlocksUsed(CWordArray& BlkCnt);
    int               RemoveUnusedBlocks();
    void              SetPageSize(PageSizes PageSizeIn);
    float             PageWidth() {return PageWd;};
    float             PageHeight() {return (float)(PageWd/sqrt(2.0));};

    flag              GetBounds();
    flag              GetBounds(C3_BOX TheBounds, DML_LIST TheCurveLst, DML_LIST TheInsertLst, DML_LIST TheOtherLst);
    DXF_ENTITY        ClosestEntity(Pt_SLW pt);
    DXF_ENTITY        ClosestInsert(Pt_SLW pt);
    DXF_ENTITY        ClosestAttr(pchar AttrTag, Pt_SLW pt);

    pStrng_List       AttrValuesInWin(pchar AttrTag, PT3 p1, PT3 p2, pStrng_List &Lst);
    pStrng_List       AttrValuesInWin(pchar AttrTag, Pt_SLW p1, Pt_SLW p2, pStrng_List &Lst) {return AttrValuesInWin(AttrTag, p1.World.p(), p2.World.p(), Lst);};

    long              TranslateEntitiesInWin(Viewport * vp,PT3 S1, PT3 S2, PT3 E1, PT3 E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred)
                        { return LayerCollection::TranslateEntitiesInWin(vp,S1,S2,E1,E2, DoX, DoY, DoXScl, DoYScl, DoSpred); };
    long              TranslateEntitiesInWin(Viewport * vp,Pt_SLW S1, Pt_SLW S2, Pt_SLW E1, Pt_SLW E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred)
                        { return TranslateEntitiesInWin(vp,S1.World.p(), S2.World.p(), E1.World.p(), E2.World.p(), DoX, DoY, DoXScl, DoYScl, DoSpred); };
    long              TranslateEntitiesInList(Viewport * vp,PT3 S1, PT3 S2, PT3 E1, PT3 E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred)
                        { return LayerCollection::TranslateEntitiesInList(vp,S1,S2,E1,E2, DoX, DoY, DoXScl, DoYScl, DoSpred); };
    long              TranslateEntitiesInList(Viewport * vp,Pt_SLW S1, Pt_SLW S2, Pt_SLW E1, Pt_SLW E2, flag DoX, flag DoY, flag DoXScl, flag DoYScl, flag DoSpred)
                        { return TranslateEntitiesInList(vp,S1.World.p(), S2.World.p(), E1.World.p(), E2.World.p(), DoX, DoY, DoXScl, DoYScl, DoSpred); };

    pBlock            Add_Block(pchar BlockName, REAL *basept);
    pBlock            LoadFromLib(pchar Path, pchar FileName, pchar BlockName);
    pBlock            ConstructBlockFromSelection(Viewport * v,pchar KernalName,Pt_3f pt);
    flag              FindCOGfromSelection(Viewport * vp,Pt_3f &pt,Pt_3f &boxll,Pt_3f &boxur);
    DXF_ENTITY        Add_Attr(DXF_ENTITY Insert, pchar Tag, pchar Prompt, pchar Value,
                               Pt_3f Pt, Attr_Settings &Set);
    void              MergeLTypeDefs(DXF_FILE f);
    void              MergeLayerDefs(DXF_FILE f);
    DXF_TABLE_ENTRY   FindLayerDef(pchar LayerName);
    DXF_TABLE_ENTRY   FindStyleDef(pchar LayerName);

    void              AddLayerDef(pchar LayerName,int color,pchar font);
    DXF_TABLE_ENTRY   FindLayerDef(LayerTypes Typ) { return FindLayerDef(LayerNames[Typ]); };

    void              SetCurveColor(C3_CURVE c, int color) { c->m_Color = color; };
    int               GetCurveColor(C3_CURVE c) { return c->m_Color; };
    void              SetCurveThickness(C3_CURVE c, double Thickness) { c->m_Thickness = Thickness; };
    double            GetCurveThickness(C3_CURVE c) { return c->m_Thickness; };
    void              SetCurveLayer(C3_CURVE c, pchar layer) { C3_CURVE_LAYER_SET(c, layer); };
    pchar             GetCurveLayer(C3_CURVE c) { return (C3_CURVE_LAYER_GET(c)); };
    void              SetCurveLtype(C3_CURVE c, pchar ltype);
    pchar             GetCurveLtype(C3_CURVE c);

    DXF_ENTITY        First_Insert(Entity_List_Info &Info) { CGrfLayer * p=Find(CurLayer); return (p ? First_Ent(p->InsertLst, Info) : NULL); };
    DXF_ENTITY        Next_Insert(Entity_List_Info &Info) { return Next_Ent(Info); };
    DXF_ENTITY        First_Insert_In_Win(Pt_SLW &P1, Pt_SLW &P2, Entity_List_Info &Info);
    DXF_ENTITY        Next_Insert_In_Win(Entity_List_Info &Info);

    long              AddNodeGrfInfo(pNodeGrfInfo pNGI);
    flag              GetNodeGrfIOs(pNodeGrfInfo pNGI, pchar NodeName, long ReqdIOs, Strng_List &IOL);
    DXF_ENTITY        Add_AssocGrf(eAssocGrfTypes Type, PT3 pt, int TypeCount, LPCSTR NdTag, LPCSTR Tag, DXF_ENTITY OwnerEntity);
    DXF_ENTITY        Add_PidBubble(CEntInView* ent,PT3 pt,Pt_SLW pta,pchar loop_id,pchar area_id,pchar equip_isa,pchar loop_type);
    DXF_ENTITY        Add_Unit(Viewport * vp, Pt_3f &CTagPt, flag CTagPtOK, pchar equip_name, pchar Tag, pchar BlockName, pchar ModelName, Attr_Settings &Tag_Attr_Set);
    DXF_ENTITY        Add_Instr(Viewport * vp);
    DXF_ENTITY        Add_Loop(Viewport * vp);
    DXF_ENTITY        Create_Annotation_Block(CExecObj *pDb, Strng_List &Vars, Strng_List &Fmts, Strng_List &Tags, Pt_3f Pos, Attr_Settings &Set, double Scale);
    DXF_ENTITY        Update_Annotation_Block(DXF_ENTITY e, CExecObj *pDb, Attr_Settings &Set, double Scale);
    DXF_ENTITY        Create_NewAnnotation_Block(CExecObj *pDb, Strng_List &Tags, Strng_List &TLbs, Strng_List &Vars, Strng_List &VLbs, Strng_List &Fmts, Strng_List &Cvss, Pt_3f Pos, Attr_Settings &Set, double Scale);
    DXF_ENTITY        Update_NewAnnotation_Block(DXF_ENTITY e, CExecObj *pDb, Attr_Settings &Set, double Scale);
    flag              ChangeTag_NewAnnotation_Block(pchar pOldTag, pchar pNewTag);
    DXF_ENTITY        Create_Fill_Block(CExecObj *pDb, DXF_ENTITY Insert, pchar ptag, pchar pvar, double Min, double Max, C3_CURVE Curve, Attr_Settings &ASet, flag ApplyToAll, int FillColour);
    DXF_ENTITY        Delete_Fill_Block(DXF_ENTITY Insert, flag ApplyToAll);
    DXF_ENTITY        Create_Bars_Block(CExecObj *pDb, Strng_List &Vars, Strng_List &Tags, Strng_List &Mins, Strng_List &Maxs, Strng_List &Cols, Pt_3f Pos, Attr_Settings &ASet, Text_Settings &TSet, long DisplayOptions, flag NewInsert);
    DXF_ENTITY        Create_Toggle_Block(CExecObj *pDb, Strng_List &Vars, Strng_List &Tags, Pt_3f Pos, Attr_Settings &ASet, Text_Settings &TSet, double Scale);
    DXF_ENTITY        Create_NewText_Block(char* Text, Pt_3f Pos, Attr_Settings &Set, double Scale);
    void              DumpSelectedEntities(Viewport * vp);
    void              AlignSelectedInserts(Viewport * vp);

    flag              Explode(DXF_ENTITY e);
    flag              DivideCurve(C3_CURVE c1,C3_CURVE c2,C3_CURVE &cn1,C3_CURVE &cn2,double Tol = 1.0e-6);

    int               FindCurveIntersect(Viewport * vp, Pt_3f Pt, CEntInView* pEnt_1,CEntInView* pEnt_2, Pt_3f &IntPt, PARM parm_1, PARM parm_2);
    int               BreakCurvesAtIntersect(Viewport * vp, Pt_3f Pt, Pt_3f &IntPt);

  public:
    char            Name[128];
    Grf3D_Display * pDsp;

    char          * HdrInfo[N_HDR_VARS];
    char            GotHdrInfo;

    C3_BOX_S        Bounds;
    float           PageWd;

    dword           m_MdlAssocGrfSeqNo;

    BlockCollection Blocks;            // Instance of blockcollection for this drawings blocks

    DML_LIST LTypeLst;
    DML_LIST LayerLst;
    DML_LIST StyleLst;
    DML_LIST ViewLst;
    DML_LIST UcsLst;
    DML_LIST VPortLst;

  };

//===========================================================================

#define WALK_CURVE_LIST(list,it,crv) \
for ( it=DML_FIRST(list) ;\
      (it ? (crv = (C3_CURVE)DML_RECORD(it)): NULL);it=DML_NEXT(it))

#define WALK_ENTITY_LIST(list,it,e) \
for ( it=DML_FIRST(list) ; \
      ( it ? ( e = (DXF_ENTITY)DML_RECORD(it)): NULL); \
      it=DML_NEXT(it))

#define WALK_LAYERS(layers, LayName, l) \
for ( l=layers ; \
     ( l != NULL); \
       l = l->Nxt ) \
  if (LayName == NULL || (l->Def!=NULL && _strnicmp(LayName, DXF_LAYER_NAME_GET(l->Def), strlen(LayName)) == 0))

#define WALK_BLOCKS(blocks, b) \
for ( b=blocks ; \
     ( b != NULL); \
       b = b->Nxt )

//===========================================================================

extern void Big_Curs(HDC my_HDC, int x0, int y0);
extern void Rect_Curs(HDC my_HDC, int x0, int y0, int x1, int y1);
extern void Rubber_Curs(HDC my_HDC, int x0, int y0, int x1, int y1);
extern void Image_Curs(HDC my_HDC, Pt_3i Base, Pt_3i Curnt, int N, Pt_3i * Pt);
extern void DList_Curs(HDC my_HDC, Pt_3i Base, Pt_3i Curnt, CEntInView* pEnt);

extern void Grf3Drw_Entry();


//===========================================================================

#define GC_NoCurs      0x01
#define GC_BigCurs     0x02
#define GC_RectCurs    0x04
#define GC_RubberCurs  0x08
#define GC_ImageCurs   0x10
#define GC_DListCurs   0x20

enum ViewDirections {Vw_Top, Vw_Front};
enum ZoomTypes      {Zoom_Win, Zoom_All, Zoom_Page, Zoom_PanAbs, Zoom_PanRel,Zoom_Set , Zoom_Purge, Zoom_Iso};
#define MaxCursImgPts 64

//===========================================================================

class Viewport
  {
  public:
    Viewport(ViewDirections VwDir, double ulx, double uly, double lrx, double lry, pDXF_Drawing DrwIn = NULL);
    ~Viewport();
    VP_VIEWPORT     Vp() {return vp;};
    void            Show() {if (vp) vpi_show(vp); };
    void            Paint(RECT &Rect, PAINTSTRUCT& PaintInfo);
    void            Paint(RECT &Rect, CRect &UpdateRect);
    CEntInView*      Draw(DXF_ENTITY e, int Colour = -1);
    void            Draw(C3_CURVE c, int Colour = -1);
    void            ApplyDeltaMatrix();
    void            RepaintDisplayList();
    void            AddDisplaySegment(CEntInView* ptr);
    void            DelDisplaySegment(CEntInView* ptr);
    void            PrintDisplayList(FILE *fp);

    void            AddDynamicSegment(DynamicSegment* p);
    DynamicSegment* FindDynamicSegment(pchar Tag, pchar Var, byte DynTyp);
    void            DelDynamicSegment(CEntInView* p);
    void            DrawDynamicSegment(DynamicSegment* p);
    void            DrawDynamicSegment(DynamicSegment* p,double val);
    void            UpdateDynamicList(CEntInView *l);
    void            TestDynamicList();
    void            PrintDynamicList();

    CEntInView*     FindEntInView4Entity(DXF_ENTITY e);
    CEntInView*     SelectClosestEntity(Pt_SLW pt, BOOL& AlreadySelected, double MaxDist=1.0e30);
    CEntInView*     SelectClosestEntity(Pt_SLW cpoint, double MaxDist=1.0e30);
    void            SelectAssocEntities();
    int             SelectClosestEntities(Pt_SLW cpoint, CEntInView** pEntLst, double* DistList, int MaxCnt, double MaxDist=1.0e30);
    CEntInView*     SelectEntity(CEntInView* best);
    void            SelectEntitiesInBox(Pt_SLW start,Pt_SLW end);
    CEntInView*     SelectLocOnEntity(Pt_SLW start,PT3 p);
    void            SelectEntitiesInCrossBox(Pt_SLW start,Pt_SLW end);
    void            ClearAllEntity();
    void            DeSelectEntity(CEntInView*);
    CEntInView*     FirstSelectedEntity();
    CEntInView*     NextSelectedEntity();
    CEntInView*     PrevSelectedEntity();
    CEntInView*     LastSelectedEntity();

    flag            EntityIsSelectable(DXF_ENTITY e,C3_CURVE c);
    inline void     AddSelectionEntityList(long entity_type) { SelectOnEntity |= entity_type; };
    inline void     AddSelectionLayerList(pchar LayerName) { SelectOnLayer.Append(LayerName); };
    inline void     AddSelectionAttribList(pchar AttrTag) { SelectOnAttrTag.Append(AttrTag); };
    inline void     ClrSelectionEntityList() { SelectOnEntity = 0; };
    inline void     ClrSelectionLayerList() { SelectOnLayer.Clear(); };
    inline void     ClrSelectionAttribList() { SelectOnAttrTag.Clear(); };
    void            ClrSelectionAllList();

    //int             MarkCount(){ return MarkCount; };

    Viewport      * Nxt, *Prv;
    VP_VIEWPORT     vp;
    pDXF_Drawing    Drw;
    ZoomTypes       ZoomType;
    C3_BOX_S        ZoomBox;
    CEntInView    * cur, *curprv, *curnxt;// All the funny little flags to enable
                                      // parsing lists while deleteing

    double          zoom_xscale,zoom_yscale,zoom_xoffset,zoom_yoffset;

    CEntInView*     disp_list;     // the list of entities for this view
    DynamicSegment* dynm_list;     // list of dynamic segments
    int             MarkCount;

    long            SelectOnEntity;
    Strng_List      SelectOnLayer;
    Strng_List      SelectOnAttrTag;

    CLVector        Curseg_offset;
    int             Curseg_level;

    flag            m_bUpdateOnly, m_bColoursChgd;
  };

//===========================================================================

class DllImportExport Grf3D_Display
  {
  public:
    Grf3D_Display(int XAspect=707, int YAspect=500);
    Grf3D_Display(CWnd *TheWndIn, int XAspect=707, int YAspect=500);
    ~Grf3D_Display();
    int             CursImgLen;
    Pt_3i           CursImgPts[MaxCursImgPts];
    Pt_3i           CursImgBase;
    CEntInView      *CursEnt;

    CCriticalSection Sect;

    Pt_SLW_Array    Dig_Point_List;  // List of point digitised on the screen

    void            Open(CDC* pDC = NULL, CRect * pDspRect=NULL);
    void            Close();
    void            SetPrintInfo(CPrintInfo* pInfo) { PrintRect = pInfo->m_rectDraw; };
    void            Paint(PAINTSTRUCT& PaintInfo);
    void            Paint(CRect &UpdateRect);
    void            Draw(DXF_ENTITY e, int Colour);
    void            Draw(C3_CURVE c, int Colour);
    void            ReDraw(Pt_SLW p1, Pt_SLW p2);
    void            XORCursor();
    flag            ChangeTag(pchar pOldTag, pchar pNewTag);
    void            CollectTags(Strng_List & TagList);
    flag            FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr);

    void            SetCursImgBase();
    void            SetCursImgBase(Pt_3i Pt);
    void            SetCursImgPt(int N, Pt_3i Pt);
    void            SetCursImgPt(int N, int X, int Y);
    void            SetCursEntity(pDXF_Drawing pDrw,DXF_ENTITY ent);
    void            SetCursEntity(pDXF_Drawing pDrw,C3_CURVE c);

    void            ButtonDownBegin(POINT point, byte Button, Cursor_Types CursNo);
    void            LeftButtonAction();
    void            ButtonDownEnd(POINT point, byte Button);
    void            ButtonUpBegin(POINT point, byte Button);
    void            ButtonUpEnd(POINT point, byte Button);
    void            MouseMoveBegin(POINT point);
    void            MouseMoveEnd(POINT point);
    Viewport      * CurrentView() {return Vp1;};
    Viewport      * AddView(ViewDirections VwDir, double ulx, double uly, double lrx, double lry, pDXF_Drawing DrwIn);
    void            SetViewDrawing(DXF_Drawing * DrwIn, Viewport *Vp=NULL);
    DXF_Drawing   * CurrentDrawing(Viewport * Vp=NULL);
    void            SetCurrentPt(POINT point);
    void            SetCurrentPtWorld(Pt_3f pnt);
    void            SetCPtWorld(Pt_3f world, Pt_SLW  &CPt);
    void            Show();

    void            Add_Dig_Point_List(Pt_SLW &Pt);
    void            Show_Dig_Point_List();
    void            Clear_Dig_Point_List();
    void            Hide_Dig_Point_List();
    void            Clear_Last_Dig_Point_List();

    flag            FetchTaggedSIValue(pchar Tag, pchar Var, double& Val, CCnvIndex & CnvInx, Strng &CnvTxt);
    flag            ToggleTaggedSIValue(pchar Tag,pchar Var,long& Val, CCnvIndex & CnvInx, Strng &CnvTxt);

    void            Show_Dig_Point(Pt_SLW &Pt, flag On=1, flag XOR=0);
    void            Hide_Dig_Point(Pt_SLW &Pt);
    void            Show_Dig_Line(Pt_SLW &Pt1, Pt_SLW &Pt2, flag On=1, flag XOR=0);
    void            Hide_Dig_Line(Pt_SLW &Pt1, Pt_SLW &Pt2);
    void            Show_Dig_Rect(Pt_SLW &Pt1, Pt_SLW &Pt2, flag On=1);
    void            Hide_Dig_Rect(Pt_SLW &Pt1, Pt_SLW &Pt2);
    void            Show_Dig_CrossHair(Pt_SLW &Pt, flag On=1, flag XOR=0);
    void            Move_Dig_CrossHair(Pt_SLW &Pt);
    void            SetZoom(ZoomTypes ZoomTypeIn, Pt_SLW &Pt1, Pt_SLW &Pt2);
    void            SetZoom(ZoomTypes ZoomTypeIn, REAL wxl=0.0, REAL wyl=0.0, REAL wxh=1.0, REAL wyh=1.0);

    void            SetUpdateFlags(flag UpdOnly, flag ColoursChgd);
    Pt_SLW          CurrentPt, StartPt, EndPt;
    PT3             LocOnPt;

    RECT            Rect;
    RECT            PrintRect;
    HPEN            hOldPen;
    HBRUSH          hOldBrush;
    int             OROPMode;

    byte            Opens;
    flag            DoPrimaryInit:1,
                    ButtonDown:1,
                    bPrinting:1;
    HDC             my_HDC;
    HDC             HDC_List[10];
    HDC             HDC_Prev[10];
    GR_COLOR        COLOR_Prev[10];
    flag            OwnsDC[10];

    CWnd*           TheWnd;
    int             m_XAspect;
    int             m_YAspect;

    Cursor_Types    Cursor;
    Viewport       *Vp1;

    Pt_SLW          m_XHairPt;
    flag            m_XHairOn;
    flag            m_XHairXOR;

  };

//===========================================================================

#define   SEGMENT_MARKED    1

class CEntInView
  {
  private:
    CEntInView*                nxt;
    CEntInView*                prv;
    Viewport                  *vp;     // The viewport to whick it points
    DML_ITEM                 item;     // The list on whick the entity appears
    C3_CURVE                    c;
    DXF_ENTITY                  e;
    LONGPOINT              *instr;     // the coordinates of the polyline
    int                       *ni;     // the number of coordinates in each seg
    short                   *type;     // the type and color of each seg
    int                       nni;     // the number of each segs
    int                    ninstr;     // total number of instaructions
    int                   segflag;     // the flag
    // cnm
    int                  fBoxSlct;     // the flag
    int               m_iColorRqd;     // color overide
    int            m_iOvrColorRqd;     // color overide - Dash
    int              m_iOvrPenRqd;     // pen overide - Dash/dot
    int             m_iColorDrawn;     // color drawn
    int          m_iOvrColorDrawn;     // color drawn
    COLORREF      rgbFillColorRqd;
    double         m_ThicknessRqd;     // Thickness
    char *                   pTag;     // assoctag

  public:
    CEntInView(DML_ITEM i,DXF_ENTITY e,C3_CURVE c,pViewport vp, int BoxSlct, int ColorRqd, int OvrColorRqd, int OvrPenRqd, double Thick, char * Tag);
    ~CEntInView();

    CEntInView*  GetNextValue();
    void        SetNextValue(CEntInView* cur);
    CEntInView*  GetPrevValue();
    void        SetPrevValue(CEntInView* cur);
    void        Qhot(REAL temperature);
    void        Qprint(FILE *fp);
    void        Qdisplay(int xoff = 0,int yoff = 0);
    void        Qdisplay(int segment,double level,double level2,int dytype);
    void        Qhighlight();
    void        SetMarkBit();//{ segflag |= SEGMENT_MARKED;  };
    void        ClrMarkBit();//{ segflag &= ~SEGMENT_MARKED; };
    int         IfMarkBit(){ return segflag&SEGMENT_MARKED; };
    int         MarkBit(){ return segflag; };
    int         EntityMustHaveBeenDeleted(){ return !e && !c; };
    void        DisplayOneSegment(long segment);
    void        Qtransform(double xscl,double yscl,double xoff,double yoff);
    C3_CURVE    CurvePtr(){ return c; }
    DXF_ENTITY  EntityPtr(){ return e; }
    int         NumPoint(){return ninstr; }
    REAL        SegmentProximityTest(int x,int y, flag &InBox);
    REAL        SegmentClosest(int x,int y,REAL &xx,REAL &yy);
    int         SegmentInBox(int x1,int y1,int x2,int y2);
    int         SegmentCrossBox(int x1,int y1,int x2,int y2);
    int         SegmentInCrossBox(int x1,int y1,int x2,int y2);

    CEntInView*  EntityInvalidate();
    void        SetSegmentType(int indx,int type);
    pViewport   VP(){ return vp; }
    int         AllowBoxSelect() { return fBoxSlct; };

    double      ThicknessRqd() { return m_ThicknessRqd; };
    int         ColorRqd() { return m_iColorRqd; };
    int         ColorDrawn() { return m_iColorDrawn; };
    int         OvrColorRqd() { return m_iOvrColorRqd; };
    int         OvrPenRqd() { return m_iOvrColorRqd; };
    int         OvrColorDrawn() { return m_iOvrColorDrawn; };
    void        SetColorRqd(int c, int oc=-1, int op=MSStyle_SDash) { m_iColorRqd=c; m_iOvrColorRqd=oc; m_iOvrPenRqd=op; };
    COLORREF    FillColorRqd() { return rgbFillColorRqd; };
    void        SetFillColorRqd(COLORREF Fill=RGB(0,0,0)) { rgbFillColorRqd=Fill; };
    char *      Tag() { return pTag; };

  };

//===========================================================================

class DynamicSegment
  {
  private:
    byte            iDynTyp; //dynamic type: DynFill,DynBars or DynToggle
    int             seg;
    CEntInView*     ptr;
    DynamicSegment* nxt;
    DynamicSegment* prv;
    CString         Var;
    CString         Tag;
    CString         CnvTxt;
    CString         Type; //segment fill type
    CCnvIndex       iCnv;
//  public:
    double          display_min;
    double          display_max;
    double          display_sivalue;
    double          display_last;
    int             display_freq;
    COLORREF        display_col;

  public:
    DynamicSegment(CEntInView* p, pchar Tag_, pchar Var_, CCnvIndex CnvInx, pchar CnvTxt_, pchar Type_, byte DynTyp, COLORREF Col_, double minv_, double maxv_, double CurVal, int segment);
    ~DynamicSegment();

    inline DynamicSegment* GetNext() { return nxt; };
    inline DynamicSegment* GetPrev() { return prv; };
    inline CEntInView*     GetEntInView() { return ptr; };
    inline void            SetEntInView(CEntInView *l) { ptr = l; };
    inline void            SetNext(DynamicSegment* p) { nxt = p; };
    inline void            SetPrev(DynamicSegment* p) { prv = p; };
    inline int             GetSeg() { return seg; };
    inline pchar           GetVar() { return (pchar)(const char*)Var; };
    inline pchar           GetTag() { return (pchar)(const char*)Tag; };
    inline pchar           GetCnvTxt() { return (pchar)(const char*)CnvTxt; };
    inline CCnvIndex       GetCnvIndex() { return iCnv; };
    inline void            SetCnvIndex(CCnvIndex Cnv) { iCnv = Cnv; };
    inline pchar           GetType() { return (pchar)(const char*)Type; };
    inline byte            GetDynTyp() { return iDynTyp; };

    inline COLORREF        GetDispCol() { return display_col; };
    inline bool            UseFillColor() { return (iDynTyp==DynBars); };
    inline double          GetSIValue() { return display_sivalue; };
    inline void            SetSIValue(double v) { display_sivalue=v; };
    inline double          GetHumanValue(double v) { return Cnvs[(GetCnvIndex())]->Human(v, GetCnvTxt()); };
    inline double          GetSclValue(double v) { return Valid(v) ? Range(0.000001, (v - display_min)/GTZ(display_max - display_min), 0.999999) : 0.000001; };
    inline double          GetLastSclValue() { return display_last; };
    inline void            SetLastSclValue(double v) { display_last=v; };

  };

//===========================================================================

#define  GR_FIRSTSTATUSCOLOR    GR_FIRSTUSERCOLOR

#define  GR_MDLSTATUS_INACTIVE      (GR_FIRSTSTATUSCOLOR+16)
#define  GR_MDLSTATUS_ERROR         (GR_FIRSTSTATUSCOLOR+0)
#define  GR_MDLSTATUS_WARNING       (GR_FIRSTSTATUSCOLOR+1)
#define  GR_MDLSTATUS_TEAR          (GR_FIRSTSTATUSCOLOR+2)
#define  GR_MDLSTATUS_MACMDL        (GR_FIRSTSTATUSCOLOR+15)
#define  GR_MDLSTATUS_OFF           (GR_FIRSTSTATUSCOLOR+3)
#define  GR_MDLSTATUS_ON            (GR_FIRSTSTATUSCOLOR+4)
#define  GR_MDLSTATUS_ISSRC         (GR_FIRSTSTATUSCOLOR+5)
#define  GR_MDLSTATUS_ISSNK         (GR_FIRSTSTATUSCOLOR+6)
#define  GR_MDLSTATUS_NETTQMP       (GR_FIRSTSTATUSCOLOR+7)
#define  GR_MDLSTATUS_NETTQMN       (GR_FIRSTSTATUSCOLOR+8)
#define  GR_MDLSTATUS_UFLW          (GR_FIRSTSTATUSCOLOR+9)
#define  GR_MDLSTATUS_UNOFLW        (GR_FIRSTSTATUSCOLOR+10)
#define  GR_MDLSTATUS_LFLWP         (GR_FIRSTSTATUSCOLOR+11)
#define  GR_MDLSTATUS_LFLWN         (GR_FIRSTSTATUSCOLOR+17)
#define  GR_MDLSTATUS_LNOFLW        (GR_FIRSTSTATUSCOLOR+12)
#define  GR_MDLSTATUS_CLEAR         (GR_FIRSTSTATUSCOLOR+13)
#define  GR_MDLSTATUS_UNKNOWN       (GR_FIRSTSTATUSCOLOR+14)
#define  GR_MDLSTATUS_LXFER         (GR_FIRSTSTATUSCOLOR+18)
#define  GR_MDLSTATUS_LSIMPLE       (GR_FIRSTSTATUSCOLOR+19)
#define  GR_MDLSTATUS_LLINEAR       (GR_FIRSTSTATUSCOLOR+20)
#define  GR_MDLSTATUS_LFULL         (GR_FIRSTSTATUSCOLOR+21)
#define  GR_MDLSTATUS_LFLWVAP       (GR_FIRSTSTATUSCOLOR+22)
#define  GR_MDLSTATUS_LFLWSOL       (GR_FIRSTSTATUSCOLOR+23)
//#define  GR_MDLSTATUS_BLACK         (GR_FIRSTSTATUSCOLOR+24)
//#define  GR_MDLSTATUS_ELEC          (GR_FIRSTSTATUSCOLOR+25)

#define  GR_OTHERGRAPHICS           (GR_FIRSTUSERCOLOR+32)

//                                  0x00BBGGRR
#define  GRREF_MDLSTATUS_INACTIVE   0x00008080
#define  GRREF_MDLSTATUS_ERROR      0x000000ff
#define  GRREF_MDLSTATUS_WARNING    0x000080ff
#define  GRREF_MDLSTATUS_TEAR       0x008080ff
#define  GRREF_MDLSTATUS_MACMDL     0x00ffffff
#define  GRREF_MDLSTATUS_OFF        0x0000ffff
#define  GRREF_MDLSTATUS_ON         0x0000ff00
#define  GRREF_MDLSTATUS_ISSRC      0x00008000
#define  GRREF_MDLSTATUS_ISSNK      0x00008000
#define  GRREF_MDLSTATUS_NETTQMP    0x008080ff
#define  GRREF_MDLSTATUS_NETTQMN    0x008080ff
#define  GRREF_MDLSTATUS_UFLW       0x00ffff00
#define  GRREF_MDLSTATUS_UNOFLW     0x00ff8000
#define  GRREF_MDLSTATUS_LFLWVAP    0x0080ff80    
#define  GRREF_MDLSTATUS_LFLWSOL    0x00ff8000    
#define  GRREF_MDLSTATUS_LFLWP      0x00ffff00
#define  GRREF_MDLSTATUS_LFLWN      0x00ff8000
#define  GRREF_MDLSTATUS_LNOFLW     0x00808000
#define  GRREF_MDLSTATUS_UNKNOWN    0x00808080
#define  GRREF_MDLSTATUS_CLEAR      0x00fefefe
#define  GRREF_MDLSTATUS_LXFER      0x000080ff
#define  GRREF_MDLSTATUS_LSIMPLE    0x008080ff
#define  GRREF_MDLSTATUS_LLINEAR    0x0080ff80
#define  GRREF_MDLSTATUS_LFULL      0x00ff8080
#define  GRREF_MDLSTATUS_BLACK      0x00000000
#define  GRREF_MDLSTATUS_ELEC       0x0020e0e0

#define  GRREF_OTHERGRAPHICS        0x00808080

#undef DllImportExport
#endif

