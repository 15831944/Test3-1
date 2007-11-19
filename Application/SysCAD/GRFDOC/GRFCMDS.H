//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GRFCMDS_H
#define  __GRFCMDS_H

#include "grfcmcmd.h"
#include "sfe_clnt.h"
#include "mdlslctu.h"
#include "mdlslctl.h"
#include "project.h"

//#ifdef __GRFCMDS_CPP
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif

//===========================================================================

extern void GrfProjectClosing();

extern flag GetAFileName(pchar Fn1In, pchar Sel1, pchar Fn1, int Fn1Len, pchar Sel2, pchar Fn2, int Fn2Len, flag StripExtn, flag MustExist);

//===========================================================================

enum ToolBarIDs
  {
  DynTBZoom,
  DynTBDelete,
  DynTBMarkEntity,
  DynTBMoveEntity,
  DynTBAlterEntity,
  DynTBDumpEntity,
  DynTBHelp,
  DynTBExplode,
  DynTBExplodeSymb,
  DynTBMoveLink,
  DynTBSaveGroup,
  DynTBTagChange
  };

// ==========================================================================

enum MouseBtnActs
  {
  MBA_None,
  MBA_AccessClosest,
  MBA_AccessMenu,
  MBA_QuickView,
  MBA_QuickViewPin,
  MBA_QuickViewMerge,
  MBA_MdlActionsMenu,
  MBA_ShowPressNets,
  MBA_ShowFlowNets,
  MBA_ClearMarks,
  MBA_Switch,
  MBA_AltAccessClosest,
  MBA_WiringClosest,
  MBA_GotoConnected,
  };

//===========================================================================

const long MaxVerts=100;

enum LinePtType { LPT_Line, LPT_Break };
enum LineDrawMethod { LDM_Orth, LDM_Direct };
enum ArrowRuleMethod { ARM_None, ARM_CentreDirn, ARM_EndDirn, ARM_EndDirnAndLast, ARM_Centre, ARM_Ends, ARM_EndAndCentre, ARM_EndAndCentreEx };

class CLineDrawHelper
  {
  public:
    CLineDrawHelper();
    void            Init(Grf3D_Display* pDsp, DXF_Drawing *pDrw);
    void            InitPoints();
    bool            ValidGraphics();
    bool            AddPoint(Pt_SLW & Pt, LinePtType Type);
    bool            AddPoint(Pt_3f & Pt, LinePtType Type);
    bool            AddPoint(LinePtType Type);
    void            RemovePoint();
    void            ShowLine();
    void            HideLine();
    int             FindLongestSegment();
    double          GetDistance(int index);
    double          GetAngle(int index);
    void            GetMidPoint(int index, Pt_3f & Pt);
    void            AddArrow(Pt_3f & Pt, double ang, double Thickness);
    void            AddArrow(int index, double Thickness);
    void            AddArrowMidPoint(int index, double Thickness);
    void            AddArrows(double Thickness);
    void            AddLineData(double Thickness);

    int             NVerts() { return m_iNVerts; };
    LineDrawMethod  LineMethod() { return m_eLineMethod; };
    void            SetLineMethod(LineDrawMethod LineMethod) { m_eLineMethod = LineMethod; };
    ArrowRuleMethod ArrowRule() { return m_eArrowRule; };
    void            SetArrowRule(ArrowRuleMethod ArrowRule) { m_eArrowRule = ArrowRule; };
    void            SetArrowScale(double ArrowScale) { m_dArrowScale = ArrowScale; };
    void            SetShowDigPoints(bool ShowDigPoints) { m_bShowDigPoint = ShowDigPoints; };
    double          LineWidth() { return m_LineWidth; };
    void            SetLineWidth(double LineWidth) { m_LineWidth = LineWidth; };
    LPCTSTR         Tag() { return m_sTag(); };
    void            SetTag(LPCTSTR Tag) { m_sTag = Tag; };
    void            SetTag(LPCTSTR Tag, double X, double Y, double Z) { m_sTag = Tag; m_TagPt.X=X; m_TagPt.Y=Y; m_TagPt.Z=Z; };
    Pt_3f      &    TagPt() { return m_TagPt; };
    void            SetTagPt(double X, double Y, double Z) { m_TagPt.X=X; m_TagPt.Y=Y; m_TagPt.Z=Z; };
    bool            ShowTag() { return m_bShowTag; };
    double          TagRotation() { return m_TagRotation; };
    Pt_3f      &    TagScale() { return m_TagScale; };
    void            SetShowTag(bool ShowTag) { m_bShowTag = ShowTag; };
    void            SetTagProps(double Scale, double Rot) { m_TagScale.X = Scale; m_TagScale.Y = Scale; m_TagScale.Z = Scale; m_TagRotation = Rot; };
    void            SetAssocTag(LPCTSTR AssocTag) { m_sAssocTag = AssocTag; }; 
    LPCTSTR         AssocTag() { return m_sAssocTag(); };

    Pt_3f           VertWorld(int i) { return m_Verts[i].World; };

  protected:
    LineDrawMethod    m_eLineMethod;
    ArrowRuleMethod   m_eArrowRule;
    double            m_dArrowScale;
    double            m_LineWidth;
    int               m_iNVerts;
    bool              m_bShowDigPoint;
    Pt_SLW            m_Verts[MaxVerts];
    int               m_PType[MaxVerts];
    Grf3D_Display   * m_pDsp;
    DXF_Drawing     * m_pDrw;
    Strng             m_sTag;
    Strng             m_sAssocTag;
    bool              m_bShowTag;
    double            m_TagRotation;
  public:
    Pt_3f             m_TagPt;
    Pt_3f             m_TagScale;
    Pt_3f             m_InsertPt;
    Block           * m_pBlock;

  };

//===========================================================================

//===========================================================================

class CInsertBlk
  {
  public :
    CInsertBlk() : \
      NdScl(1,1,1),
      GrpScl(1,1,1),
      Rotate(0),
      e(0),
      MdlInsertErr(0)
      {
      MdlInsertErr=0;
      e=NULL;
      m_iFixupRule=0;
      m_NDBOptions=eNDB_StdImport;
      m_Entity=0;
      m_EntInVw=NULL;
      m_pOldTags=NULL;
      m_pNewTags=NULL;
      };

  public :
    Strng         ATag, AClass, ASymbol, ATagBase;
    Strng         m_sGroupLibrary, m_sGroupName, m_sGuid;
    Pt_SLW        Pt;
    Pt_3f         NdScl;
    Pt_3f         GrpScl;
    double        Rotate;
    DXF_ENTITY    e;
    int           MdlInsertErr;
    int           m_iFixupRule;
    Strng         m_sString1, m_sString2;
    CStringArray * m_pOldTags;
    CStringArray * m_pNewTags;

    eScdNDBOptions m_NDBOptions;
    DXF_ENTITY    m_Entity;
    CEntInView    *m_EntInVw;
    CRectangleF   m_Rect;

    //CRectangleF   m_BoundRct;
  };

class CConnectBlk
  {
  public :
    Strng ATag, AClass, ASymbol, ATagBase;
    Strng SrcTag, SrcIO, DstTag, DstIO, SrcTS, DstTS;
    int iLineMethod;
    int NVerts;
    Pt_SLW Verts[MaxVerts];
    int LstTk[MaxVerts];
    DXF_ENTITY e;
    CConnectBlk() { iLineMethod=0; NVerts=0; e=NULL; };
  };
class CChangeBlk
  {
  public :
    class CChangeItem
      {
      public :

        CChangeItem()
          {
          m_Changed=false;
          }

        Strng         ATag, AClass, ASymbol, ATagBase, ADrwGroup;
        Pt_SLW        Pt;
        DXF_ENTITY    m_Entity;
        CEntInView    *m_EntInVw;
        Pt_3f         Scl;
        float         Rotate;
        bool          m_Changed;
      };

    CChangeBlk() : \
      Scl(1,1,1),
      Rotate(0),
      e(0)
      //MdlInsertErr(0)
      {
      m_Entity=0;
      m_EntInVw=NULL;
      //m_MvEnt=NULL;
      m_bMoving=false;
      };

    CArray <CChangeItem,CChangeItem&> m_Items;

    Strng         ATag, AClass, ASymbol, ATagBase, ADrwGroup;
    Pt_SLW        Pt;
    DXF_ENTITY    m_Entity;
    CEntInView    *m_EntInVw;
    Pt_3f         Scl;
    float         Rotate;
    DXF_ENTITY    e;
    bool          m_bMoving;
    //CEntInView    *m_MvEnt;
  };

//===========================================================================

class CIOClassArray : public CArray<dword, dword> {};
_FWDDEF(GrfCmdBlk)
class DllImportExport GrfCmdBlk : public GrfComCmdBlk
  {
  public:
    Pt_3f_Array         Point_Data;            // for geneal use
    CString             DrawingFile;
    flag                bGrfLoadDefered:1,
                        DigStartTag:1,
                        bCTagPtSet:1,
                        bFlag1:1,
                        bFlag2:1,
                        bFlag3:1,
                        bFlag4:1;
    flag                DoX:1,
                        DoY:1,
                        DoXScl:1,
                        DoYScl:1,
                        DoSpred:1,
                        DoRestrained:1;
    flag                HideTag:1,
                        HideEqpId:1,
                        HideTagOff:1,
                        BlocksOn:1,
                        VarsOn:1,
                        AccSelect:1;
    //static flag        bLicBlock;   //true if licensing conditions are exceeded
    //static int         iNoOfUnits;  //no of units
    //static int         iNoOfIllegalUnits;  //no of illegal units based on licensing
    DXF_Drawing       * pDrw;
    int                 TagWidth, ColWidth;
    pStrng_List         TagList, ModList;
    Strng               Mods;
    Pt_SLW              Pt1, Pt2, Pt3, Pt4, DPt, RefPt;
    CEntInView        * pEnt1;
    Pt_3f               NdScl,GrpScl,Delta,Pos;
    float               Rotate;
    long                InsNo;
    int                 iDispOpts;
    int                 i1, i2;
    double              d1, d2;
    double              Size, Rotation;
    double              TextWidth, TextSize;
    double              ArrowScale, PrevArrowScale;
    BOOL                m_bAutoWire;
    DXF_ENTITY          Like;
    sint                AttrName;
    Strng               AText;
    Strng               AVar, ATag, TagBase, AClass, AUnitId, ASymbol, ALayer;
    Strng               m_sGroupLibrary, m_sGroupName;
    Strng               SrcTag, DstTag, SrcIO, DstIO, SrcTS, DstTS;
    Strng_List          SrcIOs, DstIOs, VarReq, VLbReq, TagReq, TLbReq, FmtReq, CvsReq, MinReq, MaxReq, ColReq;
    int                 nEntSelects;
    eScdTagFixupRules   m_iFixupRule;
    Strng               m_sString1, m_sString2;
    CStringArray      * m_pOldTags;
    CStringArray      * m_pNewTags;

    Pt_SLW              CTagPt;
    CEntInView        * pTxtEnt[2];
    int                 nTxtEnt;
    Text_Settings       Text_Set;
    Attr_Settings       Attr_Set, Tag_Attr_Set;
    Pt_3f               Tag_InsertPt;
    int                 iTBxpos;     //grf dynamic toolbar position
    int                 iTBypos;     //grf dynamic toolbar position
    Strng               sLogMsg;     //log message to be sent
    HCURSOR             OldCursor;

    Strng               LnkTag;
    DXF_ENTITY          OldEntity;

    CSArray             SrcIONames, DstIONames;
    CSArray             SrcIODescs, DstIODescs;
    CIOClassArray       SrcIOClass, DstIOClass;
    CSArray             SrcTermStrips, DstTermStrips;
    int                 iSrcIOIndex, iDstIOIndex;

    static MouseBtnActs  m_Left;
    static MouseBtnActs  m_LeftDbl;
    static MouseBtnActs  m_LeftCtrl;
    static MouseBtnActs  m_LeftShft;
    static MouseBtnActs  m_LeftCtSh;
    static MouseBtnActs  m_Right;
    static MouseBtnActs  m_RightDbl;
    static MouseBtnActs  m_RightCtrl;
    static MouseBtnActs  m_RightShft;
    static MouseBtnActs  m_RightCtSh;

  public:
    GrfCmdBlk(int ExtraCmds, int ExtraCIS);//pGrfWnd Grf_, pGrfDoc Doc_);
    ~GrfCmdBlk();
    virtual void   InitForCmd();
    virtual void   OnActivateDoc(flag bActivate);
    virtual void   BuildVNT();
    virtual flag   UpdateCmdStr(int nID, CCmdUI* pCmdUI);

    //static void    CalcUnits(flag ForceCnt = false);
    //static void    CalcLicUnits();
    int            CheckEntity(char* pTag);

    void           DoModify(char *pTag, float angle, float x, float y, float width, float height, int follColor, bool mirrorX, bool mirrorY) ;

    char *         DoToggleItem(char *pTag);
    char *         DoActionMenu(char *pTag);
    char *         DoAccessMenu(char *pTag);
    void           DoObjectAccess();
    void           DoObjectView();
    void           DoConfig();
    //void           MakeUniqueTag(rStrng ATag, pchar Hd, char SepChar, long &InsNo);
    void           DoDivide();
    void           DoExplode();
    void           DoAttach();
    void           DoHelpUnit();
    void           DoInsert();
    bool           DoInsertNodeGrf(CInsertBlk* CB, bool SkipTagTest=false);
    bool           DoInsertNode(CInsertBlk* CB);
    bool           DoInsertGroup(CInsertBlk* CB);
    bool           DoInsertLinkGrf(CLineDrawHelper & LDH, char* SrcTag=NULL, char* DstTag=NULL, bool SkipTagTest=false);
    bool           DoInsertLinkGrf(CConnectBlk* CB, bool SkipTagTest=false);
    bool           DoInsertLink(CConnectBlk* CB);
    bool           DoChangeNodeGuid(pchar Tag, pchar Guid);
    flag           GetConnsForUnit(char* Tag, flag Inputs, dword RqdClass, Strng &IOTag, CSArray &IONames, CSArray &IODescs, CIOClassArray &IOClass, CSArray &IODefaultTSs, Pt_3f *pDigOff, Pt_3f *pDrwScl);
    CEntInView*    GetClosest(Pt_SLW CurrentPt, flag FindNode, flag FindLink, flag FindDirectLink, Strng& Tag);
    CEntInView*    GetClosest(Pt_SLW CurrentPt, flag FindInsert);
    CEntInView*    GetClosest(Pt_SLW CurrentPt, BOOL& AlreadySelected, flag FindInsert);
    CEntInView*    GetClosestText(Pt_SLW CurrentPt);
    CEntInView*    GetClosestEntity(Pt_SLW CurrentPt, DXF_ENTITY pEntityToFind);

    void           DoConnect();
    void           DoMoveLink();
    void           DoChangeUnit();
    void           DoConstructSymbol();
    void           DoConstructUnit();
    void           DoConstructLink();
    void           DoConstructTie();
    //void           DoAttachModel();

    void           DoMoveEntity();
    void           DoAlterEntity();
    void           DoTranslate();
    void           DoDelete();
    void           DoBulkTagChange();
    //void           DoList();
    //void           DoSetListFmt();

    void           DeleteDrawing();
    void           Load(pchar FnDraw, flag DoMerge);
    void           ImportPDS(pchar FnDraw, flag DoMerge);
    //void LoadDSL(pchar FlwID, pchar FnDSL, pchar Share_);
    void           Save(pchar FnDraw, flag FullSave);
    void           SaveSymbols(pchar SymPath, pchar DocGUID);
    void           DoLoadMerge(flag DoMerge);
    void           DoMerge();
    void           DoLoad();
    void           DoSave();
    void           DoImportPDS();
    void           DoSetAutoXEC();

    void           DoInsertGroup();
    void           DoSaveGroup();

    void           DoSaveSymbols();

    void           DoZoom();
    void           DoReDraw();
    void           DoCopyBitmap();

    flag           IsLike(DXF_ENTITY Tst);
    void           ShowAttrs(DXF_ENTITY Ins, flag Tags);
    void           HideAttrs(DXF_ENTITY Ins, flag Tags);
    void           MoveAttrs(DXF_ENTITY Ins, flag Tags, Pt_3f Delta);
    DXF_ENTITY     FirstAttr(DXF_ENTITY Ins, flag Tags);

    //void           DoShowHideAttrs(flag Show, flag Tags);
    //void           DoShowAttrs();
    //void           DoHideAttrs();
    //void           DoMoveAttrs();
    void           DoUpdateTags();

    void           DoCreateFill();
    void           DoUpdFill();
    void           DoCreateBars();
    void           DoUpdBars();
    //void           DoCreateAnno();
    //void           DoRefreshAnno();
    //void           DoUpdAnno();
    void           DoNewCreateAnno();
    void           DoNewRefreshAnno();
    void           DoNewUpdAnno();
    void           DoCreateTogg();
    void           DoUpdTogg();
    void           DoAlterText();

    void           DoAddTrace();

    void           DoCfgLayers();
    void           PanAndSetCursor();
    void           DoSelect();
    void           DoFind();
    void           DoDump();
    void           DoAlign();

    void           DoBreakLine();

    void           Do3DImport();

    //void           DoMemory();
    //void           DoShutDown();
    //void           DoDebug();

    DXF_ENTITY     AddUnitDrawing(pchar TagBase,pchar DrawTyp, pchar ModelTyp, pchar Tag, pchar AssocTag,
                                  Pt_3f Pt, Pt_3f Scl, float Rotate,
                                  flag CompleteBlock,
                                  Attr_Settings &ASet = Attr_Settings (50,0,1.5,1.0,0.0,GR_DARKGRAY,Text_Cntrd,Text_Baseline,"STANDARD"),
                                  Pt_3f * TagPt = NULL);
    int            AddUnitModel(pchar ModelTyp, pchar Tag, pchar pGuidStr=NULL);
    DXF_ENTITY     AddLink(pchar Typ, pchar Tag,
                         pchar SrcTag, pchar SrcOut,
                         pchar DstTag, pchar DstIn,
                         double ArrowScale,
                         //double Length, double Diameter,
                         pPt_SLW Verts, int NVerts, Attr_Settings &ASet);
    DXF_ENTITY     AddLinkDrawing(CLineDrawHelper & LDH);
    int            AddLinkModel(char* Typ, char* Tag, char* SrcTag, char* SrcOut, char* DstTag, char* DstIn);

  // CNM protected:
    void SetToolbar(int iDynToolbar);
    void RemoveToolbar();
  protected:
    void           ActivateGWnd();

    inline flag    TestDlgBusy() { return gs_pCmd->IsDlgBusy(); };
    inline flag    TestRunning() { return (gs_Exec.Busy() || gs_pCmd->IsDlgBusy()); };
    inline flag    TestMdl() { return (gs_pPrj==NULL || !gs_pPrj->ModelsAttached() || gs_pCmd->IsDlgBusy()); };
    inline flag    TestMdlRunning() { return (gs_pPrj==NULL || !gs_pPrj->ModelsAttached() || gs_Exec.Busy() || gs_pCmd->IsDlgBusy()); };

  public: // Document Methods
    virtual flag   TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs);
    virtual flag   TestSymbolNameUnique(Strng &Sym, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueSym);

    virtual BOOL   DoReadDocument(const char *pszPathName, FILE* pFile);
    virtual BOOL   DoWriteDocument(const char *pszPathName, FILE* pFile);

  };

//===========================================================================
#undef DllImportExport
#endif
