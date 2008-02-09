//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __GRFCMDS_CPP
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
#include "scdver.h" // WITHCTRLOBJ-EDIT
#include "ordwnd.h"
#include "statswnd.h"
#include "tknpars.h"
#include "sfe_base.h"
#include "elnkwiring.h"
#include "wirepanel.h"
#include "gendlgs.h"
#include "neutraldlgs.h"
#include "neutralgrf.h"
#include "neutralmdl.h"
#include "bulktagchange.h"
#include "assocgrftag.h"
#include "svcconn.h"

//#include "optoff.h"
                        
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgdxfmem 0x0
#define dbgdrw    0x01

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

//===========================================================================

int iCurrentToolbar = -1;

static UINT BASED_CODE GrfZoomIds[] =
  {
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_ZoomLeft,
  ID_GRF_ZoomRight,
  ID_GRF_ZoomUp,
  ID_GRF_ZoomDown,
  ID_GRF_ZoomIn,
  ID_GRF_ZoomOut,
  ID_GRF_ZoomAll,
  };

static UINT BASED_CODE GrfDelIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_IWin,
  ID_GRF_CWin,
  //  ID_SEPARATOR,
  //ID_GRF_Next,
  //  ID_SEPARATOR,
  //ID_GRF_Redraw,
  };

static UINT BASED_CODE GrfTagChgIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_IWin,
  ID_GRF_CWin,
  //  ID_SEPARATOR,
  //ID_GRF_Next,
  //  ID_SEPARATOR,
  //ID_GRF_Redraw,
  };

static UINT BASED_CODE GrfMarkEntityIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_MarkAttach,
  ID_GRF_MarkTag,
  ID_GRF_Goto,
  };

static UINT BASED_CODE GrfMoveEntityIds[] =
  {
  //ID_GRF_Esc,
  ID_GRF_Done,
  ID_GRF_IWin,
  ID_GRF_CWin,
    ID_SEPARATOR,
  //ID_GRF_Next,
  //  ID_SEPARATOR,
  ID_GRF_Redraw,
  };

static UINT BASED_CODE GrfAlterEntityIds[] =
  {
  //ID_GRF_Esc,
  ID_GRF_Done,
  ID_GRF_IWin,
  ID_GRF_CWin,
    ID_SEPARATOR,
  ID_GRF_ScalePlus,
  ID_GRF_ScaleMinus,
    ID_SEPARATOR,
  ID_GRF_RotatePlus,
  ID_GRF_RotateMinus,
    ID_SEPARATOR,
  ID_GRF_Dialog,
  ID_GRF_Redraw,
  };

static UINT BASED_CODE GrfDumpEntityIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
  ID_GRF_CLear,
  ID_GRF_IWin,
  ID_GRF_CWin,
    ID_SEPARATOR,
  ID_GRF_DumpAllToFile,
  };

static UINT BASED_CODE GrfHelpIds[] =
  {
  ID_GRF_Done,
  };

static UINT BASED_CODE GrfExplodeIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_IWin,
  ID_GRF_CWin,
  //  ID_SEPARATOR,
  //ID_GRF_Next,
  //  ID_SEPARATOR,
  //ID_GRF_Redraw,
  };

static UINT BASED_CODE GrfExplodeSymbIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_IWin,
  ID_GRF_CWin,
    ID_SEPARATOR,
  ID_GRF_MarkTag,
  };

static UINT BASED_CODE GrfRedrawLinkIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_Back,
  ID_GRF_Ortho,
  ID_GRF_Free,
  ID_GRF_Break,
  //  ID_SEPARATOR,
  //ID_GRF_Scale,
  };

static UINT BASED_CODE GrfSaveGroupIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
    ID_SEPARATOR,
  ID_GRF_IWin,
  ID_GRF_CWin,
  };


//Note: Toolbar titles MUST be specified and they MUST be unique
static CDynToolBarInfo DynToolBars[] =
  {
    { "Zoom",           IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfZoomIds)/sizeof(UINT), GrfZoomIds },
    { "Delete",         IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfDelIds)/sizeof(UINT), GrfDelIds },
    { "Mark Entity",    IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfMarkEntityIds)/sizeof(UINT), GrfMarkEntityIds },
    { "Move Entity",    IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfMoveEntityIds)/sizeof(UINT), GrfMoveEntityIds },
    { "Alter Entity",   IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfAlterEntityIds)/sizeof(UINT), GrfAlterEntityIds },
    { "Dump Entity",    IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfDumpEntityIds)/sizeof(UINT), GrfDumpEntityIds },
    { "Help",           IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfHelpIds)/sizeof(UINT), GrfHelpIds },
    { "Explode",        IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfExplodeIds)/sizeof(UINT), GrfExplodeIds },
    { "Explode Symb",   IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfExplodeSymbIds)/sizeof(UINT), GrfExplodeSymbIds },
    { "Redraw Link",    IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfRedrawLinkIds)/sizeof(UINT), GrfRedrawLinkIds },
    { "Save Group",     IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfSaveGroupIds)/sizeof(UINT), GrfSaveGroupIds },
    { "Tag Change",     IDR_GRFSYMBOLS, IDW_TOOLBAR_9, sizeof(GrfTagChgIds)/sizeof(UINT), GrfTagChgIds },
  };

//===========================================================================

void GrfProjectClosing()
  {
  CInsertLinkDlg::Reset();
  CInsertUnitDlg::Reset();
  CNeutralInsertGroupDlg::Reset();
  }

//===========================================================================

bool ParseTheTag(Strng & ATag, bool PromptChanges)
  {
  bool OK = true;
  Strng PrevTag(ATag);
  if (ATag.Len()>SCDPartTagLen-2)
    {//a) too long
    ATag = ATag.Left(SCDPartTagLen-2);
    if (PromptChanges)
      LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag is too long, truncated to:\n'%s'", ATag());
    PrevTag = ATag;
    OK = false; //tag changed
    }

  if (TaggedObject::ValidateTagDotChanged(ATag))
    {//b) illegal characters
    if (PromptChanges)
      LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag contains illegal characters:\n'%s'\nChanged to:\n%s", PrevTag(), ATag());
    OK = false; //tag changed
    }
  return OK;
  }

//---------------------------------------------------------------------------

int CheckTheTag(Strng & ATag, char* Class, char* Prefix, bool PromptChanges)
  {
  int RetCode = 0;
  Strng PrevTag(ATag);
  if (!ParseTheTag(ATag, PromptChanges))
    {
    PrevTag = ATag;
    RetCode = -1; //tag changed
    }

  Strng UniqueTag;
  CUniqueTagRulesBlk TRB(Class, Prefix);
  if (!gs_pPrj->TestModelTagUnique(ATag(), TRB, UniqueTag, false))
    {// Not unique
    ATag = UniqueTag();
    if (PromptChanges)
      LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Duplicate Tag:\n%s\nChanged to:\n%s", PrevTag(), ATag());
    RetCode = -2;
    }
  return RetCode;
  }

//---------------------------------------------------------------------------

int FindTheTag(Strng & ATag, char* Class)
  {
  int RetCode = 0;
  Strng PrevTag(ATag);
  if (!ParseTheTag(ATag, true))
    {
    PrevTag = ATag;
    RetCode = -1; //tag changed
    }
  else
    {
    Strng UniqueTag;
    CUniqueTagRulesBlk TRB(Class, "XXX");
    if (gs_pPrj->TestModelTagUnique(ATag(), TRB, UniqueTag, false))
      {// Unique ie we did not found it
      RetCode=-2;
      }
    }
  return RetCode;
  }

#if SYSCAD10

//TACKY !! needs some thought !!

#define SCD10ENTER                        \
  {                                       \
  int NOpens=pDsp->Opens;                 \
  for (int ixx=0; ixx<NOpens; ixx++)      \
    pDsp->Close();

#define SCD10LEAVE                        \
  for (int ixx=0; ixx<NOpens; ixx++)      \
    pDsp->Open();                         \
  }

static LPCSTR Scd10GetTag(DXF_ENTITY e)
  {
  LPCSTR pTag;
  if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
    {
    if (Find_Attr_Value(e, AssocTagAttribStr)==NULL) // is not an AssocTag
      return pTag;
    }
  return NULL;
  }

static LPCSTR Scd10GetGrfGuid(DXF_ENTITY e)
  {
  LPCSTR pGuid;
  if (DXF_ENTITY_IS_INSERT(e) && (pGuid = Find_Attr_Value(e, GuidAttribStr)))
    {
    //if (Find_Attr_Value(e, GuidAttribStr)==NULL) // is not an AssocTag
    return pGuid;
    }
  return NULL;
  }

static LPCSTR Scd10GetInsertName(DXF_ENTITY e)
  {
  if (DXF_ENTITY_IS_INSERT(e))
    {
    return DXF_INSERT_BLOCK_GET(e);
    }
  return NULL;
  }

#else
#define SCD10ENTER
#define SCD10LEAVE
#endif
//===========================================================================

//flag GrfCmdBlk::bLicBlock = 0;
//int GrfCmdBlk::iNoOfUnits = 0;
//int GrfCmdBlk::iNoOfIllegalUnits = 0;

//===========================================================================
//Note:These initial values should match CMousePage::OnDefault1() in gendlgs.cpp
MouseBtnActs GrfCmdBlk::m_Left      = MBA_QuickView;
MouseBtnActs GrfCmdBlk::m_LeftDbl   = MBA_AccessClosest;
MouseBtnActs GrfCmdBlk::m_LeftCtrl  = MBA_QuickViewPin;
MouseBtnActs GrfCmdBlk::m_LeftShft  = MBA_MdlActionsMenu;
MouseBtnActs GrfCmdBlk::m_LeftCtSh  = MBA_QuickViewMerge;
MouseBtnActs GrfCmdBlk::m_Right     = MBA_AccessClosest;
MouseBtnActs GrfCmdBlk::m_RightDbl  = MBA_None;
MouseBtnActs GrfCmdBlk::m_RightCtrl = MBA_WiringClosest;//MBA_ShowPressNets;
MouseBtnActs GrfCmdBlk::m_RightShft = MBA_AltAccessClosest;//MBA_ShowFlowNets;
MouseBtnActs GrfCmdBlk::m_RightCtSh = MBA_ClearMarks;

//---------------------------------------------------------------------------

GrfCmdBlk::GrfCmdBlk(int ExtraCmds, int ExtraCIS) :
GrfComCmdBlk(64 + ExtraCmds, 64 + ExtraCIS),
  Attr_Set(50, 0, 3.0, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD"),
  Text_Set(2.0, 1.0, 0.0, GR_LIGHTGRAY, Text_Left, Text_Baseline, "STANDARD")
  {
  TagWidth = 12;
  ColWidth = 8;

  bGrfLoadDefered=0;

  TagList = NULL;
  ModList = NULL;
  DigStartTag = 0;

  InsNo = 1;

  NdScl.Set(1.0, 1.0, 1.0);
  GrpScl.Set(1.0, 1.0, 1.0);
  Rotate = (float)0.0;
  //Length = 10.0;
  //Diameter = 0.1;

  pDrw = NULL;
  Like = NULL;
  Rotation = 0.0;
  Size = 1.0;
  ArrowScale = 2.5;
  TextSize = 2.0;
  TextWidth = 0.8;
  m_bAutoWire =  true;

  OldCursor=NULL;

  HideTag=TRUE;
  HideEqpId=TRUE;
  HideTagOff=False;
  Tag_Attr_Set=Attr_Set;
  Tag_InsertPt.X=dNAN;

  //bWPMdlSlctOk=0;

  Point_Data.SetSize(0,16);
  pGrfTB = NULL;
  iTBxpos = 5;
  iTBypos = 70;
  m_iFixupRule = eFixup_Prefix;
  }

//---------------------------------------------------------------------------

GrfCmdBlk::~GrfCmdBlk()
  {
  RemoveToolbar();
  if (pDrw)
    delete pDrw;
  Point_Data.RemoveAll();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::BuildVNT()
  {
  //TRACE("Open Graphics VNT");
  ObjAccFn->Set((CmdFn)&GrfCmdBlk::DoObjectAccess, EF_ERS);
  ObjVwFn->Set((CmdFn)&GrfCmdBlk::DoObjectView, EF_ERS);

  SETVNT("ACCess",    "Parms", "@ SELect", 1, (CmdFn)&GrfCmdBlk::DoConfig, EF_ERS);
  SETVNT("ACCess",    "Vars",  "@ SELect", 1, (CmdFn)&GrfCmdBlk::DoConfig, EF_ERS);
  SETVNT("INsert",    "UNit",  "@ Tag Basetag Scale XScale YScale Rotate XY X Y Model * ", 1, (CmdFn)&GrfCmdBlk::DoInsert, EF_Edit);
  SETVNT("INsert",    "SYmbol", "@ Tag Basetag Scale XScale YScale Rotate XY X Y Model * ", 1, (CmdFn)&GrfCmdBlk::DoInsert, EF_Edit);
  SETVNT("INsert",    "LInk",  "@ Tag BASetag Ortho Free BRk BACk SNd SIo DNd DIo MOdel MEthod * ", 1, (CmdFn)&GrfCmdBlk::DoConnect, EF_Edit);
  SETVNT("CHange",    "UNit",  "@ * = ", 1, (CmdFn)&GrfCmdBlk::DoChangeUnit, EF_Edit);
  SETVNT("MOve",      "LInk",  "@ Lnk Ortho Free BRk BAck", 1, (CmdFn)&GrfCmdBlk::DoMoveLink, EF_Edit);
  SETVNT("CONStruct", "SYmbol","@ IWin CWin Tag Unitid Symbol Model * CTag ", 1, (CmdFn)&GrfCmdBlk::DoConstructSymbol, EF_ERS);
  SETVNT("CONStruct", "UNit", "@ IWin CWin Tag Unitid Symbol Model * CTag ", 1, (CmdFn)&GrfCmdBlk::DoConstructUnit, EF_ERS);
  SETVNT("CONStruct", "LInk", "@ IWin CWin SNd SIo DNd DIo Tag Unitid SYmbol Model * CTag ", 1, (CmdFn)&GrfCmdBlk::DoConstructLink, EF_ERS);
  SETVNT("CONStruct", "Tie", "Intersect ", 1, (CmdFn)&GrfCmdBlk::DoConstructTie, EF_ERS);

  SETVNT("DElete",    "UNit", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  SETVNT("DElete",    "SYmbol", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  SETVNT("EXplode",   "UNit", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoExplode, EF_Edit);
  SETVNT("EXplode",   "SYmbol", "@ IWin CWin Redraw ; Tag", 1, (CmdFn)&GrfCmdBlk::DoExplode, EF_Edit);
  SETVNT("HElp",      "Unit", "@", 1, (CmdFn)&GrfCmdBlk::DoHelpUnit, EF_ERS);
  SETVNT("TAg",       "Change", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoBulkTagChange, EF_Edit);

  SETVNT("DIVide",    "LInes", "@", 1, (CmdFn)&GrfCmdBlk::DoDivide, EF_Edit);
  SETVNT("ALIgn",     "ENtity", "@", 1, (CmdFn)&GrfCmdBlk::DoAlign, EF_Edit);
  SETVNT("ALTer",     "ENtity", "@ IWin CWin ; REdraw SP SM RP RM * D1", 1, (CmdFn)&GrfCmdBlk::DoAlterEntity, EF_Edit);
  SETVNT("MArk",      "ENtity", "@ IWin CWin CLear Attach Tag Goto *", 5, (CmdFn)&GrfCmdBlk::DoSelect, EF_Edit);
  SETVNT("FInd",      "ENtity", "Goto ", 1, (CmdFn)&GrfCmdBlk::DoFind, EF_Edit);
  SETVNT("DUmp",      "ENtity", "@ IWin CWin CLear File", 1, (CmdFn)&GrfCmdBlk::DoDump, EF_Edit);
  SETVNT("TRanslate", "ENtity", "@ ; Win XScale YScale XY X Y NX NY SCale SPread", 1, (CmdFn)&GrfCmdBlk::DoTranslate, EF_Edit);
  SETVNT("MOve",      "ENtity", "@ IWin CWin ; Redraw", 1, (CmdFn)&GrfCmdBlk::DoMoveEntity, EF_Edit);
  SETVNT("LOad",      "DRawing", AName, -1, (CmdFn)&GrfCmdBlk::DoLoad, EF_Stop);
  SETVNT("MErge",     "DRawing", AName, -1, (CmdFn)&GrfCmdBlk::DoMerge, EF_Stop);
  SETVNT("IMport",    "PDS",     AName, -1, (CmdFn)&GrfCmdBlk::DoImportPDS, EF_Stop);
  SETVNT("IMport",    "ELMs",    AName, -1, (CmdFn)&GrfCmdBlk::Do3DImport, EF_Stop);
  SETVNT("SAve",      "DRawing", AName, -1, (CmdFn)&GrfCmdBlk::DoSave, EF_Stop);
  SETVNT("SAve",      "SYmbols", AName, -1, (CmdFn)&GrfCmdBlk::DoSaveSymbols, EF_Stop);
  SETVNT("SEt",       "AUToXEC", AName, -1, (CmdFn)&GrfCmdBlk::DoSetAutoXEC, EF_ERS);
  SETVNT("COPy",      "BITmap", N_A, -1, (CmdFn)&GrfCmdBlk::DoCopyBitmap, EF_ERS);
  SETVNT("ZOom",      "View", "Win All Page Left Right Down Up IN OUt Set Purge ISo ONce", 1, (CmdFn)&GrfCmdBlk::DoZoom, EF_ERS);
  SETVNT("REDraw",     N_A, N_A, -1, (CmdFn)&GrfCmdBlk::DoReDraw, EF_ERS);

  SETVNT("INsert",    "Group",  "@ Scale XScale YScale Rotate XY X Y * ", 1, (CmdFn)&GrfCmdBlk::DoInsertGroup, EF_Edit);
  SETVNT("SAve",      "Group",  "@ IWin CWin ", 1, (CmdFn)&GrfCmdBlk::DoSaveGroup, EF_Edit);

  //SETVNT("SHOw",      "Tags", "@ Win All Like NOLike Size Rotation Position NOSize NORotation NAme Display NODisplay", 1, (CmdFn)&GrfCmdBlk::DoShowAttrs, EF_ERS);
  //SETVNT("HIde",      "Tags", "@ Win All Like NOLike", 1, (CmdFn)&GrfCmdBlk::DoHideAttrs, EF_ERS);
  //SETVNT("MOve",      "Tags", "@ Win All Like NOLike", 1, (CmdFn)&GrfCmdBlk::DoMoveAttrs, EF_ERS);
  SETVNT("UPdate",    "TAgs", "@ IWin CWin * All Like NOLike SIze NOSize ROtation NORotation NAme NOName Position NOPosition SHow Hide REdraw NEXt", 1, (CmdFn)&GrfCmdBlk::DoUpdateTags, EF_ERS);

  SETVNT("CReate",    "Fill", "@ Vars * MIn MAx All", 1, (CmdFn)&GrfCmdBlk::DoCreateFill, EF_ERS);
  SETVNT("UPdate",    "Fill", "@ Delete", 1, (CmdFn)&GrfCmdBlk::DoUpdFill, EF_ERS);
  SETVNT("CReate",    "Bars", "@ * Tags Vars MIn MAx Scale", 1, (CmdFn)&GrfCmdBlk::DoCreateBars, EF_ERS);
  SETVNT("UPdate",    "Bars", "@ *", 1, (CmdFn)&GrfCmdBlk::DoUpdBars, EF_ERS);
  SETVNT("CReate",    "Annotation", "@ * Tags Vars Scale", 1, (CmdFn)&GrfCmdBlk::DoNewCreateAnno, EF_ERS);
  SETVNT("REFresh",   "Annotation", "All", -1, (CmdFn)&GrfCmdBlk::DoNewRefreshAnno, EF_ERS);
  SETVNT("UPdate",    "Annotation", "@ *", 1, (CmdFn)&GrfCmdBlk::DoNewUpdAnno, EF_ERS);
  SETVNT("CReate",    "TOggle", "@ * Tags Vars Scale", 1, (CmdFn)&GrfCmdBlk::DoCreateTogg, EF_ERS);
  SETVNT("UPdate",    "TOggle", "@ *", 1, (CmdFn)&GrfCmdBlk::DoUpdTogg, EF_ERS);
  SETVNT("ALTer",     "TExt", "@ * ; Create Update Apply Scale Rotation Txt", 1, (CmdFn)&GrfCmdBlk::DoAlterText, EF_ERS);

  SETVNT("CONFigure", "LAyers", "* LAyer_name HIde SHow COlor", -1, (CmdFn)&GrfCmdBlk::DoCfgLayers, EF_ERS);
  SETVNT("BReak",     "LIne", "Int", 1, (CmdFn)&GrfCmdBlk::DoBreakLine, EF_ERS);

  SETCIS(ID_GRF_Zoom, "zoom view ");
  SETCIS(ID_GRF_DoZoomAll, "zoom view all\r");
  SETCIS(ID_GRF_DoZoomOnce, "zoom view once ");
  SETCIS(ID_GRF_Insert, "insert unit * ");
  SETCIS(ID_GRF_Connect, "insert link ");
  SETCIS(ID_GRF_MoveLink, "move link ");
  SETCIS(ID_GRF_DeleteUnit, "delete unit ");
  SETCIS(ID_GRF_DeleteSymbol, "delete symbol ");
  SETCIS(ID_GRF_BulkTagChange, "tag change ");
  SETCIS(ID_GRF_CreateGroup, "save group ");
  SETCIS(ID_GRF_InsertGroup, "insert group * ");
  SETCIS(ID_GRF_ConstructUnit, "construct unit * ");
  SETCIS(ID_GRF_ConstructLink, "construct link ");
  SETCIS(ID_GRF_ConstructTie, "construct tie ");
  SETCIS(ID_GRF_ConstructSymbol, "construct symbol * ");
  SETCIS(ID_GRF_HelpUnit, "help unit ");
  SETCIS(ID_GRF_DoRedraw, "redraw\r");
  SETCIS(ID_GRF_ChangeUnit, "change unit ");
  SETCIS(ID_GRF_CopyBitmap, "copy bitmap\r");
  SETCIS(ID_GRF_ExplodeUnit, "explode unit ");
  SETCIS(ID_GRF_ExplodeSymbol, "explode symbol ");
  SETCIS(ID_GRF_SaveSymbols, "save symbols\r");

  SETCIS(ID_GRF_CreateFill, "create fill * ");
  SETCIS(ID_GRF_UpdateFill, "update fill ");
  SETCIS(ID_GRF_CreateDynamic, "create bars * ");
  SETCIS(ID_GRF_UpdateDynamic, "update bars ");
  SETCIS(ID_GRF_CreateAnnotation, "create annotation * ");
  SETCIS(ID_GRF_UpdateAnnotation, "update annotation ");
  SETCIS(ID_GRF_RefreshAnnotation, "refresh annotation\r");
  SETCIS(ID_GRF_RefreshAllAnnotation, "refresh annotation all\r");
  SETCIS(ID_GRF_CreateToggle, "create toggle * ");
  SETCIS(ID_GRF_UpdateToggle, "update toggle ");
  SETCIS(ID_GRF_InsertSymbol, "insert symbol * ");
  SETCIS(ID_GRF_InsertText, "alter text * create ");
  SETCIS(ID_GRF_UpdateText, "alter text * update ");
  SETCIS(ID_GRF_LoadDrawing, "load drawing *\r");
  SETCIS(ID_GRF_SaveDrawing, "save drawing *\r");
  SETCIS(ID_GRF_ImportPDSFile, "import PDS *\r");
  SETCIS(ID_GRF_ImportELMS3DFile, "import elms *\r");
  SETCIS(ID_GRF_MarkEntities, "mark entity ");
  SETCIS(ID_GRF_DumpEntities, "dump entity ");
  SETCIS(ID_GRF_ConfigLayers, "configure layers * ");
  SETCIS(ID_GRF_MoveEntity, "move entity ");
  SETCIS(ID_GRF_TagAnnot, "update tags * ");
  SETCIS(ID_GRF_AlterEntity, "alter entity ");

  SETCISD(ID_GRF_ZoomIso, "zoom view iso ");
  SETCISD(ID_GRF_ZoomAll, "all ");
  SETCISD(ID_GRF_ZoomIn, "in ");
  SETCISD(ID_GRF_ZoomOut, "out ");
  SETCISD(ID_GRF_ZoomLeft, "left ");
  SETCISD(ID_GRF_ZoomRight, "right ");
  SETCISD(ID_GRF_ZoomUp, "up ");
  SETCISD(ID_GRF_ZoomDown, "down ");
  SETCISD(ID_GRF_Done, "\r");
  SETCISD(ID_GRF_Esc, "\x1b");
  SETCISD(ID_GRF_Next, ";");
  SETCISD(ID_GRF_IWin, "iwin ");
  SETCISD(ID_GRF_CWin, "cwin ");
  SETCISD(ID_GRF_Redraw, "redraw ");
  SETCISD(ID_GRF_CLear, "clear ");
  SETCISD(ID_GRF_MarkTag, "tag ");
  SETCISD(ID_GRF_MarkAttach, "attach ");
  SETCISD(ID_GRF_Goto, "* ");
  SETCISD(ID_GRF_DumpAllToFile, "file ");
  SETCISD(ID_GRF_ScalePlus, "sp ");
  SETCISD(ID_GRF_ScaleMinus, "sm ");
  SETCISD(ID_GRF_RotatePlus, "rp ");
  SETCISD(ID_GRF_RotateMinus, "rm ");
  SETCISD(ID_GRF_Dialog, "* ");
  SETCISD(ID_GRF_Back, "back ");
  SETCISD(ID_GRF_Ortho, "ortho ");
  SETCISD(ID_GRF_Free, "free ");
  SETCISD(ID_GRF_Break, "brk ");

  GrfComCmdBlk::BuildVNT();

  CProfINIFile PF(PrjIniFile());
  iTBxpos = Min(PF.RdInt("GrfDynamicToolbar", "XPos", iTBxpos), GetSystemMetrics(SM_CXVIRTUALSCREEN)-50);
  iTBypos = Min(PF.RdInt("GrfDynamicToolbar", "YPos", iTBypos), GetSystemMetrics(SM_CYVIRTUALSCREEN)-GetSystemMetrics(SM_CYCAPTION)-15);
  }

//---------------------------------------------------------------------------

flag GrfCmdBlk::UpdateCmdStr(int nID, CCmdUI* pCmdUI)
  {
  BOOL NotBlocked = !(gs_pCmd->KBDBlocked());
  byte En = 2;
  switch (nID)
    {
    #if !WITHIMPORTPDS
    case ID_GRF_ImportPDSFile:
      En = 0;
      break;
    #endif
    case ID_GRF_Insert:
    case ID_GRF_ChangeUnit:
    case ID_GRF_Connect:
    case ID_GRF_MoveLink:
    case ID_GRF_ConstructUnit:
    case ID_GRF_ConstructLink:
    case ID_GRF_ConstructTie:
    case ID_GRF_ConstructSymbol:
    case ID_GRF_CreateFill:
    case ID_GRF_UpdateFill:
    case ID_GRF_CreateDynamic:
    case ID_GRF_UpdateDynamic:
    case ID_GRF_CreateAnnotation:
    case ID_GRF_UpdateAnnotation:
    case ID_GRF_CreateToggle:
    case ID_GRF_UpdateToggle:
    case ID_GRF_InsertSymbol:
    case ID_GRF_InsertText:
    case ID_GRF_UpdateText:
    case ID_GRF_DeleteUnit:
    case ID_GRF_BulkTagChange:
    case ID_GRF_ExplodeUnit:
    case ID_GRF_CreateGroup:
    case ID_GRF_InsertGroup:
      En = (NotBlocked && !TestMdlRunning());
      break;
    case ID_HELP_UserDocs:
    case ID_GRF_HelpUnit:
      En = (NotBlocked && !TestMdl());
      break;
    case ID_GRF_DeleteSymbol:
    case ID_GRF_ExplodeSymbol:
    case ID_GRF_SaveSymbols:
    case ID_GRF_LoadDrawing:
    case ID_GRF_SaveDrawing:
    #if WITHIMPORTPDS
    case ID_GRF_ImportPDSFile:
    #endif
    case ID_GRF_ImportELMS3DFile:
    case ID_GRF_CLear:
    case ID_GRF_DumpEntities:
    case ID_GRF_DumpAllToFile:
    case ID_GRF_ConfigLayers:
      En = (NotBlocked && !TestRunning());
      break;
    case ID_GRF_Zoom:
    case ID_GRF_DoZoomOnce:
    case ID_GRF_DoZoomAll:
    case ID_GRF_DoRedraw:
    case ID_GRF_Redraw:
    case ID_GRF_CopyBitmap:
    case ID_GRF_ZoomIso:
    case ID_GRF_ZoomAll:
    case ID_GRF_ZoomIn:
    case ID_GRF_ZoomOut:
    case ID_GRF_ZoomLeft:
    case ID_GRF_ZoomRight:
    case ID_GRF_ZoomUp:
    case ID_GRF_ZoomDown:
    case ID_GRF_MarkEntities:
    case ID_GRF_TagAnnot:
    case ID_GRF_MoveEntity:
    case ID_GRF_AlterEntity:
    case ID_GRF_MarkTag:
    case ID_GRF_MarkAttach:
    case ID_GRF_Goto:
    case ID_GRF_RefreshAnnotation:
    case ID_GRF_RefreshAllAnnotation:
      En = (NotBlocked && !TestDlgBusy());
      break;
    case ID_GRF_Done:
    case ID_GRF_Esc:
    case ID_GRF_Next:
    case ID_GRF_IWin:
    case ID_GRF_CWin:
    case ID_GRF_Back:
    case ID_GRF_Break:
      En = (NotBlocked);
      break;
    case ID_GRF_Ortho:
      En = (NotBlocked && !CInsertLinkDlg::sm_bDrawOrth);
      break;
    case ID_GRF_Free:
      En = (NotBlocked && CInsertLinkDlg::sm_bDrawOrth);
      break;

    //put any incomplete commands here to disable them
    //case XXX:
      //En = 0;
      //break;
    }

  #if CK_LICENSINGON
  if (En==1)
    {//valid ID AND Enabled, check IDs affected by license...
    switch (nID)
      {
      case ID_GRF_Insert:
      case ID_GRF_ChangeUnit:
      case ID_GRF_Connect:
      case ID_GRF_MoveLink:
      case ID_GRF_ConstructUnit:
      case ID_GRF_ConstructLink:
      case ID_GRF_ConstructTie:
      case ID_GRF_ConstructSymbol:
      case ID_GRF_MoveEntity:
      case ID_GRF_LoadDrawing:
      case ID_GRF_ImportPDSFile:
      case ID_GRF_ImportELMS3DFile:
      case ID_GRF_ConfigLayers:
      case ID_GRF_CreateFill:
      case ID_GRF_UpdateFill:
      case ID_GRF_CreateDynamic:
      case ID_GRF_UpdateDynamic:
      case ID_GRF_CreateAnnotation:
      case ID_GRF_UpdateAnnotation:
      case ID_GRF_CreateToggle:
      case ID_GRF_UpdateToggle:
      case ID_GRF_InsertSymbol:
      case ID_GRF_InsertText:
      case ID_GRF_UpdateText:
      case ID_GRF_AlterEntity:
      case ID_GRF_SaveSymbols:
      case ID_GRF_CreateGroup:
      case ID_GRF_InsertGroup:
        En = (gs_License.AllowFullLic() /*&& !gs_LicenseCnt.xBlocked()*/);
        break;
      case ID_GRF_SaveDrawing:
      case ID_GRF_TagAnnot:
        En = (!gs_License.Blocked());
        break;
      case ID_GRF_DeleteUnit:
      case ID_GRF_BulkTagChange:
      case ID_GRF_ExplodeUnit:
      case ID_GRF_DeleteSymbol:
      case ID_GRF_ExplodeSymbol:
        En = (gs_License.AllowFullLicFlag());
        break;
      }
    }
  #endif

  #if !WITHGRFGROUP
  if (nID==ID_GRF_CreateGroup || nID==ID_GRF_InsertGroup)
    En = 0;
  #endif

  if (En<2)
    {//valid ID
    pCmdUI->Enable(En);
    pCmdUI->SetCheck(0);
    return True;
    }
  return False;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::SetToolbar(int iDynToolbar)
  {
  iCurrentToolbar = iDynToolbar;
  RemoveToolbar();
  pGrfTB = pGrfTBMngr->AddToolbar(DynToolBars[iDynToolbar].pName);
  if (pGrfTB==NULL)
    {
    ASSERT(FALSE); //Should not get here!!!
    pGrfTBMngr->RemoveToolbar(DynToolBars[iDynToolbar].pName);
    pGrfTB = pGrfTBMngr->AddToolbar(DynToolBars[iDynToolbar].pName); //second time lucky?
    }

  switch (iDynToolbar)
    {
    case DynTBExplodeSymb:
      pGrfTB->AddButtons(DynToolBars[iDynToolbar].iBitmapID, DynToolBars[iDynToolbar].IDs, 6);
      pGrfTB->AddTextButton("TagOnly", DynToolBars[iDynToolbar].IDs[6]);
      break;
    case DynTBMarkEntity:
      pGrfTB->AddButtons(DynToolBars[iDynToolbar].iBitmapID, DynToolBars[iDynToolbar].IDs, 3);
      pGrfTB->AddTextButton("Attached", DynToolBars[iDynToolbar].IDs[3]);
      pGrfTB->AddSeparator();
      pGrfTB->AddTextButton("Inserted", DynToolBars[iDynToolbar].IDs[4]);
      pGrfTB->AddSeparator();
      pGrfTB->AddComboButton(DynToolBars[iDynToolbar].IDs[5]);
      break;
    /*case DynTBMoveLink:
      pGrfTB->AddButtons(DynToolBars[iDynToolbar].iBitmapID, DynToolBars[iDynToolbar].IDs, 8);
      pGrfTB->AddEditButton(DynToolBars[iDynToolbar].IDs[8]);
      break;*/
    default:
      pGrfTB->AddButtons(DynToolBars[iDynToolbar].iBitmapID, DynToolBars[iDynToolbar].IDs, DynToolBars[iDynToolbar].iLen);
      break;
    }
  pGrfTB->SetPos(iTBxpos, iTBypos);
  pGrfTB->Create(TRUE, TRUE, 0, CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, TRUE, TRUE);
  CString DocTitle = pGWnd->GetDocument()->GetTitle();
  DocTitle = DocTitle.Left(DocTitle.GetLength()-4);
  pGrfTB->AppendToTitle((char*)(const char*)DocTitle);

  //do specials...
  switch (iDynToolbar)
    {
    case DynTBMarkEntity:
      {
      CComboBox* pCBox = pGrfTB->GetComboButton(0);
      char Buffer[8192];
      CProfINIFile PF(PrjIniFile());
      if (PF.RdSection("FindTags", Buffer, sizeof(Buffer))>0)
        {
        char* pBuff = Buffer;
        while (strlen(pBuff)>0)
          {
          char* pName;
          char* pFindTxt;
          PF.GetProfItem(pBuff, pName, pFindTxt);
          if (pFindTxt)
            pCBox->AddString(pFindTxt);
          }
        pCBox->SetCurSel(0);
        }
      break;
      }
    /*case DynTBMoveLink:
      {
      CEdit* pEdit = pGrfTB->GetEditButton(0);
      char Buffer[64];
      sprintf(Buffer, "%f", ArrowScale);
      pEdit->SetWindowText(Buffer);
      break;
      }*/
    default: break;
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::RemoveToolbar()
  {
  if (pGrfTB)
    {
    CProfINIFile PF(PrjIniFile());
    CPoint pt;
    if (pGrfTB->GetPos(pt))
      {
      iTBxpos = pt.x;
      iTBypos = pt.y;
      PF.WrInt("GrfDynamicToolbar", "XPos", iTBxpos);
      PF.WrInt("GrfDynamicToolbar", "YPos", iTBypos);
      }
    switch (iCurrentToolbar)
      {
      case DynTBMarkEntity:
        {
        if (pGrfTB->ComboListChanged())
          {
          CComboBox* pCBox = pGrfTB->GetComboButton(0);
          char Buff[8192];
          int Pos = 0;
          for (int i=0; i<16 && i<pCBox->GetCount(); i++)
            {
            CString FindTxt;
            pCBox->GetLBText(i, FindTxt);
            int Len = FindTxt.GetLength();
            if (Len>0)
              {
              sprintf(&Buff[Pos], "Find_%02d=%s", i, (const char*)FindTxt);
              Pos = Pos + 8 + Len;
              Buff[Pos++] = 0;
              }
            }
          Buff[Pos++] = 0;
          Buff[Pos++] = 0;
          PF.WrSection("FindTags", Buff);
          }
        break;
        }
      default: break;
      }
    pGrfTBMngr->RemoveToolbar((char*)pGrfTB->GetTitle());
    pGrfTB = NULL;
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::ActivateGWnd()
  {
  if (pGWnd->GetFocus()!=pGWnd)
    MDIActivateThis(pGWnd); //bring the window to the front
  if (pGWnd->GetParent()->IsIconic())
    pGWnd->GetParent()->ShowWindow(SW_RESTORE);
  }

//---------------------------------------------------------------------------

int GrfCmdBlk::CheckEntity(char* pTag)
  {
  CXM_ObjectTag ObjTag(pTag, TABOpt_Parms);//TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (XReadTaggedItem(ObjTag, ObjData, Route))
    {
    //CPkDataItem * pItem = ObjData.FirstItem();
    Strng Class;
    Class = ObjData.FirstItem()->Class();
    pTagObjClass pTagObjC = TagObjClass::FindClassId(Class());
    if (pTagObjC==NULL)
      pTagObjC = TagObjClass::FindClassName(Class());
    if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 ||
                     _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 ||
                     _stricmp(ElecLinkGrp, pTagObjC->Group())==0 ||
                     _stricmp(AirLinkGrp, pTagObjC->Group())==0))
      return 2; //link
    if (pTagObjC && (_stricmp(DirectLinkGrp, pTagObjC->Group())==0))
      return 3; //DirectLinkGrp
    return 1; //unit/node
    }
  return 0; //error
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::InitForCmd()
  {
  if (TagList)
    delete TagList;
  if (ModList)
    delete ModList;
  TagList = NULL;
  ModList = NULL;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::OnActivateDoc(flag bActivate)
  {
  GrfComCmdBlk::OnActivateDoc(bActivate);
  if (pDrw==NULL && bActivate)
    {
    pDsp->Open();  // Create Viewports etc. if neccessary
    pDrw = new DXF_Drawing(pDsp, "", True);
    pDsp->SetViewDrawing(pDrw);
    pDsp->SetZoom(Zoom_All, 0.0, 0.0, 280.0, 198.0);
    pDsp->Close();
    }
  }

//---------------------------------------------------------------------------

flag GetAFileName(pchar Fn1In, pchar Sel1, pchar Fn1, int Fn1Len,
                  pchar Sel2, pchar Fn2, int Fn2Len,
                  flag StripExtn, flag MustExist)
  {
  if (Fn1In == NULL)
    return False;

  char DrvI[_MAX_DRIVE], DirI[_MAX_DIR], FNmI[_MAX_FNAME], ExtI[_MAX_EXT];
  char Drv1[_MAX_DRIVE], Dir1[_MAX_DIR], FNm1[_MAX_FNAME], Ext1[_MAX_EXT];
  _splitpath(Fn1In, DrvI, DirI, FNmI, ExtI);
  _splitpath(Sel1, Drv1, Dir1, FNm1, Ext1);

  #if dbgdrw
  if (Fn1In)
    dbgpln("FileName %s", Fn1In);
  else
    dbgpln("FileName NULL");
  dbgpln("FileName %s", Fn1In);
  dbgpln("Fn1In, :%s", Fn1In);
  dbgpln("Sel1,  :%s", Sel1);
  dbgpln("Fn1,   :%s", Fn1);
  dbgpln("Sel2,  :%s", Sel2);
  dbgpln("Fn2,   :%s", Fn2);
  #endif

  flag FNOk = False;
  char* E = (ExtI && (strpbrk(ExtI, "*?") == NULL) && (strlen(ExtI)>0)) ? ExtI : Ext1;

  char InitDir[128], FilenameOnly[64];
  _makepath(FilenameOnly, "", "", FNmI, E);
  if (strlen(DrvI) + strlen(DirI) > 0)
    {
    _makepath(Fn1, DrvI, DirI, FNmI, E);
    _makepath(InitDir, DrvI, DirI, "", "");
    }
  else
    {
    _makepath(Fn1, Drv1, Dir1, FNmI, E);
    _makepath(InitDir, Drv1, Dir1, "", "");
    }

  if (strpbrk(FNmI, "?*") || strpbrk(E, "?*"))
    {
    OPENFILENAME of;
    char Ttl[64], Filter[256];
    memset(Filter, 0, sizeof(Filter));
    int Pos = 0;
    if (E && strlen(E)>0)
      {
      if (_stricmp(&E[1], "dxf")==0)
        sprintf(Filter, "Drawing Files (*%s)", E);
      else
        sprintf(Filter, "(*%s)", E);
      Pos = strlen(Filter) + 1;
      Filter[Pos++] = '*';
      strcpy(&Filter[Pos], E);
      Pos = Pos + strlen(E) + 1;
      }
    strcpy(&Filter[Pos], "All Files (*.*)");
    Pos = Pos + strlen(&Filter[Pos]) + 1;
    strcpy(&Filter[Pos], "*.*");
    strcpy(Ttl, "");
    strlwr(InitDir);
    strlwr(FilenameOnly);
    strcpy(Fn1, FilenameOnly);

    of.lStructSize       = sizeof(of);
    of.hwndOwner         = AfxGetMainWnd()->m_hWnd;
    of.hInstance         = NULL;
    of.lpstrFilter       = Filter;
    of.lpstrCustomFilter = NULL;
    of.nMaxCustFilter    = 0;
    of.nFilterIndex      = 1;
    of.lpstrFile         = Fn1;
    of.nMaxFile          = Fn1Len;
    of.lpstrFileTitle    = Ttl;
    of.nMaxFileTitle     = sizeof(Ttl);
    of.lpstrInitialDir   = InitDir;
    of.lpstrTitle        = NULL;
    if (MustExist)
      of.Flags           = /*OFN_FILEMUSTEXIST |*/ OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE;
    else
      of.Flags           = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE;
    of.nFileOffset       = 0;
    of.nFileExtension    = 0;
    of.lpstrDefExt       = (ExtI && strlen(ExtI)>0 ? &ExtI[1] : (Ext1 && strlen(Ext1)>0 ? &Ext1[1] : NULL));
    of.lCustData         = 0;
    of.lpfnHook          = NULL;
    of.lpTemplateName    = NULL;

    #pragma chFIXIT(These 2 Calls will create a problem with the locale use CSCDFileDialog)
    if (MustExist)
      FNOk = ::GetOpenFileName(&of);
    else
      FNOk = ::GetSaveFileName(&of);
    }
  else
    FNOk = (strlen(FNmI)>0);
  strlwr(Fn1); //strupr(Fn1);
  #if dbgdrw
  dbgpln("GetAFileName Fn1: %s", Fn1);
  #endif
  if (FNOk)
    {
    if (MustExist)
      {
      WIN32_FIND_DATA ffblk;
      HANDLE Handle = FindFirstFile(Fn1, &ffblk);
      FNOk = (Handle!=INVALID_HANDLE_VALUE);
      if (Handle)
        FindClose(Handle);
      }

    if (!FNOk)
      {
      gs_pCmd->Print("FILE '%s' NOT FOUND\n", Fn1);
      LogError("GrfCmds", 0, "File '%s' not found", Fn1);
      }

    if (Sel2)
      {
      char Drv2[_MAX_DRIVE], Dir2[_MAX_DIR], FNm2[_MAX_FNAME], Ext2[_MAX_EXT];
      _splitpath(Sel2, Drv2, Dir2, FNm2, Ext2);
      _splitpath(Fn1, Drv1, Dir1, FNm1, Ext1);

      if (strlen(Drv2) + strlen(Dir2) > 0)
        _makepath(Fn2, Drv2, Dir2, FNm1, Ext2);
      else
        _makepath(Fn2, Drv1, Dir1, FNm1, Ext2);

      #if dbgdrw
      dbgpln("GetAFileName Fn2: %s", Fn2);
      #endif

      if (MustExist)
        {
        WIN32_FIND_DATA ffblk;
        HANDLE Handle = FindFirstFile(Fn2, &ffblk);
        FNOk = (Handle!=INVALID_HANDLE_VALUE);
        if (Handle)
          FindClose(Handle);
        }
      strupr(Fn2);
      }
    }

  if (FNOk && StripExtn)
    {
    strlwr(Fn1);
    char* dot = strrchr(Fn1, '.');
    if (dot)
      *dot = '\0';
    }
  #if dbgdrw
  dbgpln("GetAFileName OK: %i", FNOk);
  #endif
  return FNOk;
  }

//---------------------------------------------------------------------------

inline SFEFlwLib * FE() { return gs_pPrj->FlwLib(); };

//---------------------------------------------------------------------------

void AddMenuItems(bool ConfigOnly , CMenu &Menu, CMdlActionArray & Acts, CArray <CMenu*, CMenu*> & XMenus)
  {
  Strng XMenuName;
  bool AddTheMenu=false;
  for (int i=0; i<Acts.GetSize(); i++)
    {
    CMdlAction & MA = Acts[i];
    if (MA.iType==MAT_Switch)
      continue;

    if (ConfigOnly == (MA.iType==MAT_Config))
      if (strchr(MA.cName, '\t'))
        {
        char * pTab=strchr(MA.cName, '\t');
        int Len=(pTab-MA.cName);
        Strng ThisName=MA.cName;
        ThisName.SetLength(Len);
        Strng ThisItem=pTab+1;
        CMenu *pPop;
        if (XMenuName!=ThisName)
          {
          XMenuName=ThisName;
          pPop=new CMenu;
          pPop->CreatePopupMenu();
          XMenus.Add(pPop);
          Menu.AppendMenu(MF_POPUP, (UINT)pPop->m_hMenu, XMenuName());
          }
        else
          pPop=XMenus[XMenus.GetUpperBound()];
        pPop->AppendMenu(MF_STRING|(MA.fEnabled ? 0 : MF_GRAYED), 200+i,  ThisItem());
        }
      else
        {
        Menu.AppendMenu(MF_STRING|(MA.fEnabled ? 0 : MF_GRAYED), 200+i,  MA.cName);
        XMenuName="";
        }
    }
  }

//---------------------------------------------------------------------------

void AddMenuItems(bool ConfigOnly , CMenu &Menu, CMdlGraphicArray & Grfs, CArray <CMenu*, CMenu*> & XMenus)
  {
  Strng XMenuName;
  bool AddTheMenu=false;
  for (int i=0; i<Grfs.GetSize(); i++)
    {
    CMdlGraphic & MG = Grfs[i];
    //if (MG.m_iType==MGT_Switch)
    //  continue;

    //if (ConfigOnly == (MG.m_iType==MGT_Config))
      if (strchr(MG.m_cName, '\t'))
        {
        char * pTab=strchr(MG.m_cName, '\t');
        int Len=(pTab-MG.m_cName);
        Strng ThisName=MG.m_cName;
        ThisName.SetLength(Len);
        Strng ThisItem=pTab+1;
        CMenu *pPop;
        if (XMenuName!=ThisName)
          {
          XMenuName=ThisName;
          pPop=new CMenu;
          pPop->CreatePopupMenu();
          XMenus.Add(pPop);
          Menu.AppendMenu(MF_POPUP, (UINT)pPop->m_hMenu, XMenuName());
          }
        else
          pPop=XMenus[XMenus.GetUpperBound()];
        pPop->AppendMenu(MF_STRING|(MG.m_fEnabled ? 0 : MF_GRAYED), 300+i,  ThisItem());
        }
      else
        {
        Menu.AppendMenu(MF_STRING|(MG.m_fEnabled ? 0 : MF_GRAYED), 300+i,  MG.m_cName);
        XMenuName="";
        }
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoModify(char *pTag, float angle, float x, float y, float width, float height, int follColor, bool mirrorX, bool mirrorY) 
{
}

//---------------------------------------------------------------------------

char * GrfCmdBlk::DoToggleItem(char *pTag)
  {
  if (pTag==NULL)
    {
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, 2500.0); // 50.0
    CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
    pDsp->Vp1->ClrSelectionAllList();

    if (pEnt && pEnt->EntityPtr())
      {
      DXF_ENTITY e = pEnt->EntityPtr();
      pTag = Attr_Value(Find_Attr(e, TagAttribStr));
      }
    }

  CMdlActionArray Acts;
  if (pTag)
    {
    //Strng Tag(pTag);

    if (gs_Exec.GetModelAction(pTag, Acts))
      {
      for (int i=0; i<Acts.GetSize(); i++)
        {
        CMdlAction & MA = Acts[i];
        if (MA.iType==MAT_Switch)
          {
          Strng Tag(pTag);
          gs_Exec.SetModelAction(pTag, MA);
          pTag=NULL;
          break;
          }
        }
      }
    }

  return pTag;
  }

//---------------------------------------------------------------------------

char * GrfCmdBlk::DoActionMenu(char *pTag)
  {
  if (pTag==NULL)
    {
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, 2500.0); // 50.0
    CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
    pDsp->Vp1->ClrSelectionAllList();

    if (pEnt && pEnt->EntityPtr())
      {
      DXF_ENTITY e = pEnt->EntityPtr();
      pTag = Attr_Value(Find_Attr(e, TagAttribStr));
      }
    }

  CMenu Menu;
  CMenu AuditMenu;
  Menu.CreatePopupMenu();
  CMdlActionArray Acts;
  CMdlGraphicArray Grfs;
  CArray <CMenu*, CMenu*> XMenus;
  Strng_List ActHolds;
  if (pTag)
    {
    Strng Itm;
    Strng Tmp;
    int nActs, nGrfs;

    ActHolds.Append(pTag);
    FE()->FE_TagOperation(FETOp_GetHold, ActHolds);

    Strng ClassId;
    FE()->RequestModelClassId(pTag, ClassId);
    bool DoWire=(ClassId.XStrICmp("E_Cable")==0);

    Tmp.Set("Access '%s'", pTag);
    Menu.AppendMenu(MF_STRING/*|MF_GRAYED*/, 131,  Tmp());
    Tmp.Set("Access Alt '%s'", pTag);
    Menu.AppendMenu(MF_STRING/*|MF_GRAYED*/, 132,  Tmp());
    Menu.AppendMenu(MF_SEPARATOR, -1);

    nActs=0;
    if (gs_Exec.GetModelAction(pTag, Acts))
      {
      nActs=Acts.GetSize();
      AddMenuItems(false, Menu, Acts, XMenus);
      }

    int Gray=ActHolds.First() && (ActHolds.First()->Index()<0);
    int Chk=!Gray && ActHolds.First() && (ActHolds.First()->Index()>0);
    Menu.AppendMenu(MF_STRING|(Gray?MF_GRAYED:0)|(Chk?MF_CHECKED:0), 106,  "Hold");

    if (0 || nActs>0)
      Menu.AppendMenu(MF_SEPARATOR, -1);

    nGrfs=0;
    if (gs_Exec.GetModelGraphic(pTag, Grfs))
      {
      nGrfs=Grfs.GetSize();
      AddMenuItems(false, Menu, Grfs, XMenus);
      }

    if (0 || nGrfs>0)
      Menu.AppendMenu(MF_SEPARATOR, -1);

#if WITHDOCUMENTATIONLINK
    Menu.AppendMenu(MF_STRING, 180,  "Docu&ment ...");
#endif
    Menu.AppendMenu(MF_STRING, 104,  "Change Tag ...");
    Menu.AppendMenu(MF_STRING, 105,  "Data Transfer ...");
    Menu.AppendMenu(MF_STRING, 107,  "Associated Graphics ...");
    Menu.AppendMenu(MF_SEPARATOR, -1);
    Menu.AppendMenu(MF_STRING, 101,  "Empty");
    Menu.AppendMenu(MF_STRING, 102,  "Zero Flows");
    Menu.AppendMenu(MF_STRING|(DefNetDynamicMode()?0:MF_GRAYED), 103,  "Preset");
    Menu.AppendMenu(MF_STRING/*|(TaggedObject::NetDynamicMethod()?0:MF_GRAYED)*/, 119,  "Reset Stats");
    Menu.AppendMenu(MF_SEPARATOR, -1);
    Menu.AppendMenu(MF_STRING|(DoWire?0:MF_GRAYED), 130,  "Wiring");
    AuditMenu.CreatePopupMenu();
    Menu.AppendMenu(MF_POPUP, (UINT)AuditMenu.m_hMenu, "Mass && Energy Audit");
    AuditMenu.AppendMenu(MF_STRING, 140,  "Audit All");
    AuditMenu.AppendMenu(MF_STRING, 141,  "Mass");
    AuditMenu.AppendMenu(MF_STRING, 142,  "Mass Total");
    AuditMenu.AppendMenu(MF_STRING, 143,  "Heat");
    AuditMenu.AppendMenu(MF_STRING, 144,  "Heat Total");
    AuditMenu.AppendMenu(MF_SEPARATOR, -1);
    AuditMenu.AppendMenu(MF_STRING, 133,  "Simple (Compact)");
    AuditMenu.AppendMenu(MF_STRING, 134,  "Full (Compact)");
    AuditMenu.AppendMenu(MF_STRING, 135,  "Simple (All Joins)");
    AuditMenu.AppendMenu(MF_STRING, 136,  "Full (All Joins)");
    if (nActs>0)
      {
      Menu.AppendMenu(MF_SEPARATOR, -1);
      AddMenuItems(true, Menu, Acts, XMenus);
      }
    if (nGrfs>0)
      {
      Menu.AppendMenu(MF_SEPARATOR, -1);
      AddMenuItems(true, Menu, Grfs, XMenus);
      }
    }

  POINT ScreenPoint = {(int)pDsp->CurrentPt.Screen.X+5, (int)pDsp->CurrentPt.Screen.Y+5};// = point;
  pDsp->TheWnd->ClientToScreen(&ScreenPoint);
  CRect ClickRect(0,0,2048,2048);
  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
      ScreenPoint.x, ScreenPoint.y, pDsp->TheWnd, &ClickRect);
  Menu.DestroyMenu();
  switch (RetCd)
    {
    case 0: // No Selection
      pTag=NULL;
      break;
    case 100:
      break;
    case 107:
      {
      CAssocGrfTag Dlg(pTag);
      Dlg.DoModal();
      break;
      }
    case 101:
    case 102:
    case 103:
    case 119:
      {
      Strng_List SL;
      SL.Append(pTag);
      if (RetCd==101)
        gs_Exec.SetModelState(MSA_Empty, SL);
      else if (RetCd==102)
        gs_Exec.SetModelState(MSA_ZeroFlows, SL);
      else if (RetCd==103)
        gs_Exec.SetModelState(MSA_PreSet, SL);
      else if (RetCd==119)
        gs_Exec.SetModelState(MSA_DynStatsRunInit, SL);
      gs_AccessWnds.RefreshData(-1, false);
      pTag=NULL;
      break;
      }
    case 130:
      CELnkWiring::Start(pTag);
      break;
    case 131:
      {
      CWaitMsgCursor Wait("Accessing ...");
      Wait.UpdateMsg("Access %s", pTag);
      gs_AccessWnds.AccessNode(-1, pTag);
      break;
      }
    case 132:
      {
      CWaitMsgCursor Wait("Accessing ...");
      Wait.UpdateMsg("Access %s", pTag);
      gs_AccessWnds.AccessNode(-2, pTag);
      break;
      }
    case 133:
    case 134:
    case 135:
    case 136:
      {
      char* pTxt = new char[strlen(pTag)+1];
      strcpy(pTxt, pTag);
      WPARAM Options = 0;
      if (RetCd<=134)
        Options|=SUB_NA_CLS_COMPACT;
      if (RetCd%2==1)
        Options|=SUB_NA_CLS_SUMMARY;
      ScdMainWnd()->PostMessage(WMU_NODEAUDIT, Options, (LPARAM)pTxt);
      break;
      }
    case 140:
    case 141:
    case 142:
    case 143:
    case 144:
      {
      char* pTxt = new char[strlen(pTag)+1];
      strcpy(pTxt, pTag);
      WPARAM Options = 0;

      switch (RetCd)
        {
        case 140:  Options = SUB_NA_ADT_MASS      | 
                             SUB_NA_ADT_MASSTOTAL |
                             SUB_NA_ADT_HEAT      |
                             SUB_NA_ADT_HEATTOTAL ; break;
        case 141:  Options = SUB_NA_ADT_MASS      ; break;
        case 142:  Options = SUB_NA_ADT_MASSTOTAL ; break;
        case 143:  Options = SUB_NA_ADT_HEAT      ; break;
        case 144:  Options = SUB_NA_ADT_HEATTOTAL ; break;
        }

      ScdMainWnd()->PostMessage(WMU_NODEAUDIT, Options, (LPARAM)pTxt);
      break;
      }

    case 104:
      {
      CChangeTag ChgTag(pTag, True);
      ChgTag.DoModal();
      pTag=NULL;
      break;
      }
    case 105:
      {
      Strng T, ObjClassId;
      T.Set("%s.ClassId", pTag);

      CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
      CXM_ObjectData ObjData;
      CXM_Route      Route;

      if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
        {
        CPkDataItem * pItem=ObjData.FirstItem();
        if (IsStrng(pItem->Type()))
          {
          ObjClassId = pItem->Value()->GetString("", "", 0, "");
          CCopyTagBlkDlg CopyBlkDlg(this, ObjClassId(), pTag);
          CopyBlkDlg.DoModal();
          }
        }
      else
        LogError((char*)pTag, 0, "ClassId not found");

      }
      break;
    case 106:
      if (ActHolds.First())
        {
        ActHolds.First()->SetIndex(ActHolds.First()->Index()==0); // Toggle
        FE()->FE_TagOperation(FETOp_SetHold, ActHolds);
        }
      break;
#if WITHDOCUMENTATIONLINK
    case 180:
//MSWORD        ?
     break;
#endif
    default:
      if (RetCd>=200 && RetCd<300)
        {
        CMdlAction & MA = Acts[RetCd-200];
        switch (MA.iType)
          {
          case MAT_State:
          case MAT_Config:
          case MAT_Follow:
            {
            Strng Tag(pTag);
            gs_Exec.SetModelAction(pTag, MA);
            pTag=NULL;
            break;
            }
          case MAT_Value:
            {                        
            CMdlValueSet::Add(pTag, MA, ScreenPoint.x, ScreenPoint.y, pWnd);
            pTag=NULL;
            break;
            }
          case MAT_Switch:
            INCOMPLETECODEMSG("Should not get here", __FILE__, __LINE__);
            break;
          }
        }
      else if (RetCd>=300)
        {
        CMdlGraphic & MG = Grfs[RetCd-300];
        switch (MG.m_iType)
          {
          case MGT_Simple:
            CMdlGraphicShow::Add(pTag, ScreenPoint.x, ScreenPoint.y, true, false, pWnd);
            break;
          //case MGT_State:
          //case MGT_Config:
          //case MGT_Follow:
          //  {
          //  Strng Tag(pTag);
          //  //gs_Exec.SetModelAction(Tag, MA);
          //  pTag=NULL;
          //  break;
          //  }
          //case MGT_Value:
          //  {
          //  //CMdlValueSet::Add(pTag, MA, ScreenPoint.x, ScreenPoint.y, pWnd);
          //  pTag=NULL;
          //  break;
          //  }
          //case MGT_Switch:
          //  INCOMPLETECODEMSG("Should not get here");
          //  break;
          }
        }
    break;
    }
  for (int i=0; i<XMenus.GetSize(); i++)
    delete XMenus[i];
  return pTag;
  }

//---------------------------------------------------------------------------

char * GrfCmdBlk::DoAccessMenu(char *pTag)
  {
  Strng Tag, Tmp;
  flag NearTag=(pTag!=NULL);
  if (pTag==NULL)
    {
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, 2500.0); // 50.0
    CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
    pDsp->Vp1->ClrSelectionAllList();

    if (pEnt && pEnt->EntityPtr())
      {
      DXF_ENTITY e = pEnt->EntityPtr();
      pTag = Attr_Value(Find_Attr(e, TagAttribStr));
      }
    }

  CMenu Menu;
  Menu.CreatePopupMenu();
  CMdlActionArray Acts;
  if (pTag)
    {
    Strng ClassId;
    FE()->RequestModelClassId(pTag, ClassId);
    bool DoWire=(ClassId.XStrICmp("E_Cable")==0);

    Strng Itm;
    Strng Tag(pTag);
    Itm.Set("Access '%s'", pTag);
    Menu.AppendMenu(MF_STRING, 100,  Itm());
    if (gs_Exec.GetModelAction(pTag, Acts))
      {
      Menu.AppendMenu(MF_SEPARATOR, -1);
      for (int i=0; i<Acts.GetSize(); i++)
        Menu.AppendMenu(MF_STRING|(Acts[i].fEnabled ? 0 : MF_GRAYED), 200+i,  Acts[i].cName);
      }
    Menu.AppendMenu(MF_SEPARATOR, -1);
    Menu.AppendMenu(MF_STRING, 104,  "Change Tag ...");
    Menu.AppendMenu(MF_STRING, 120,  "Data Transfer ...");
    Tmp.Set(NearTag ? "Empty '%s'" : "Empty All", pTag);
    Menu.AppendMenu(MF_STRING, 101,  Tmp());
    Tmp.Set(NearTag ? "Zero Flows '%s'" : "Zero All Flows", pTag);
    Menu.AppendMenu(MF_STRING, 102,  Tmp());
    Tmp.Set(NearTag ? "Preset '%s'" : "Preset All", pTag);
    Menu.AppendMenu(MF_STRING|(DefNetDynamicMode() ? 0 : MF_GRAYED), 103,  Tmp());
    Tmp.Set(NearTag ? "Reset Stats '%s'" : "Reset All Stats", pTag);
    Menu.AppendMenu(MF_STRING/*|(TaggedObject::NetDynamicMethod() ? 0 : MF_GRAYED)*/, 119,  Tmp());
    //Menu.AppendMenu(MF_STRING|(DoWire?0:MF_GRAYED), 130,  "Wiring");
    Menu.AppendMenu(MF_SEPARATOR, -1);
    }

  Menu.AppendMenu(MF_STRING, 105,  "Plant Model");
  Menu.AppendMenu(MF_STRING, 125,  "Plant Area");
  if (DefNetProbalMode())
    Menu.AppendMenu(MF_STRING, 106,  "ProBal Configuration");
  else
    Menu.AppendMenu(MF_STRING, 107,  "Dynamic Configuration");
#if WITHCTRLOBJ
  Menu.AppendMenu(MF_STRING, 111,  "Controls");
#endif
  Menu.AppendMenu(MF_STRING, 113,  "Power Supplies");
#if WITHIOOBJ
  Menu.AppendMenu(MF_STRING, 114,  "I/O Points");
#endif
  Menu.AppendMenu(MF_SEPARATOR, -1);
  Menu.AppendMenu(MF_STRING, 108,  "Specie Data");
  Menu.AppendMenu(MF_STRING, 115,  "Tolerances");
  Menu.AppendMenu(MF_STRING, 109,  "Tear Blocks");
  Menu.AppendMenu(MF_STRING, 110,  "Change Stats");
#if WITHORDEROBJ
  Menu.AppendMenu(MF_STRING|(COrdWindow::GetFrame() ? 0 : MF_GRAYED), 112,  "Evaluation Order");
  Menu.AppendMenu(MF_STRING|(CStatsWindow::GetFrame() ? 0 : MF_GRAYED), 123,  "Runtime Statistics");
#endif
  Menu.AppendMenu(MF_SEPARATOR, -1);
  //Menu.AppendMenu(MF_STRING, 121,  "Copy bitmap coloured");
  //Menu.AppendMenu(MF_STRING, 122,  "Copy bitmap b&&w");
  Menu.AppendMenu(MF_STRING, 122,  "Copy bitmap");
  POINT ScreenPoint = {(int)pDsp->CurrentPt.Screen.X+5, (int)pDsp->CurrentPt.Screen.Y+5};// = point;
  pDsp->TheWnd->ClientToScreen(&ScreenPoint);
  CRect ClickRect(0, 0, 2048, 2048);
  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
      ScreenPoint.x, ScreenPoint.y, pDsp->TheWnd, &ClickRect);
  Menu.DestroyMenu();
  switch (RetCd)
    {
    case 0: // No Selection
      pTag=NULL;
      break;
    case 100:
      break;
    case 101:
    case 102:
    case 103:
    case 119:
      {
      Strng_List SL;
      if (NearTag)
        SL.Append(pTag);
      if (RetCd==101)
        gs_Exec.SetModelState(MSA_Empty, SL);
      else if (RetCd==102)
        gs_Exec.SetModelState(MSA_ZeroFlows, SL);
      else if (RetCd==103)
        gs_Exec.SetModelState(MSA_PreSet, SL);
      else if (RetCd==119)
        gs_Exec.SetModelState(MSA_DynStatsRunInit, SL);
      gs_AccessWnds.RefreshData(-1, false);
      pTag=NULL;
      break;
      }
    case 104:
      {
      CChangeTag ChgTag(pTag, True);
      ChgTag.DoModal();
      pTag=NULL;
      break;
      }
    case 130:
      CELnkWiring::Start(pTag);
      break;
    case 105: pTag=PlantModelTag; break;
    case 125: pTag=PlantAreaTag;  break;
    case 106: pTag="$PB";   break;
    case 107: pTag="$Dyn";  break;
    case 108: pTag="$SDB";  break;
    case 109: pTag="$Tear"; break;
    case 110: pTag="$Chg";  break;
#if WITHCTRLOBJ
    case 111:
      pTag=NULL;
      ScdMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_CONTROLS);
      break;
#endif
#if WITHORDEROBJ
    case 112:
      pTag=NULL;//"$Order";
      COrdWindow::Show(true);
      break;
    case 123:
      pTag=NULL;//"$Order";
      CStatsWindow::Show(true);
      break;
#endif
    case 113: pTag="$Pwr";  break;
#if WITHIOOBJ
    case 114: pTag="$IO";  break;
#endif
    case 115: pTag="$Tol";  break;
    case 120:
      {
      Strng T, ObjClassId;
      T.Set("%s.ClassId", pTag);

      CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
      CXM_ObjectData ObjData;
      CXM_Route      Route;

      if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
        {
        CPkDataItem * pItem=ObjData.FirstItem();
        if (IsStrng(pItem->Type()))
          {
          ObjClassId = pItem->Value()->GetString("", "", 0, "");
          CCopyTagBlkDlg CopyBlkDlg(this, ObjClassId(), pTag);
          CopyBlkDlg.DoModal();
          }
        }
      else
        LogError((char*)pTag, 0, "ClassId not found");
      pTag=NULL;
      break;
      }
    case 121:
    case 122:
      pGWnd->CopyBMPtoClipBoard(RetCd-121);
      break;
    default:
      if (RetCd>=200)
        {
        CMdlAction & MA = Acts[RetCd-200];
        switch (MA.iType)
          {
          case MAT_State:
            {
            gs_Exec.SetModelAction(pTag, MA);
            pTag=NULL;
            break;
            }
          case MAT_Value:
            {
//                    Strng Tag(pTag);
            CMdlValueSet::Add(pTag, MA, ScreenPoint.x, ScreenPoint.y, pWnd);
//                      *pC=new CMdlValueSet(Tag, MA, ScreenPoint.x, ScreenPoint.y, pWnd);
//                    CMdlValueSet *pC=new CMdlValueSet(Tag, MA, ScreenPoint.x, ScreenPoint.y, pWnd);
            //SMV.DoModal();
            pTag=NULL;
            break;
            }
          }
        }
    break;
    }
  return pTag;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoObjectAccess()
  {
  if (TestDlgBusy() || gs_pCmd->KBDBlocked())
    return; //blocked
  pDsp->Open();
  if (gs_pCmd->Busy())
    gs_pCmd->ExtendCmdLine("\r");

  //Access_Busy = 1;
  Strng Tag;
  pchar pTag = NULL;
  POINT ScreenPoint;
  MouseBtnActs  DwnAction = MBA_None;
  MouseBtnActs  UpAction = MBA_None;

  CWaitMsgCursor Wait("Accessing ...");

  switch (When)
    {
    case EX_MOUSE_LDCLK :
    case EX_MOUSE_LDOWN :
    case EX_MOUSE_RDCLK :
    case EX_MOUSE_RDOWN :
      {
      //CWaitMsgCursor Wait("Access tag");
      pTag = NULL;
      if (pDrw)
        {
        // Why was this restriction here CNM : if (!gs_pCmd->CtrlDown())
          pDsp->Vp1->ClearAllEntity();
        pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
        pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
        pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, 10.0); // 50.0
        CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
        pDsp->Vp1->ClrSelectionAllList();

        if (pEnt && pEnt->EntityPtr())
          {
          DXF_ENTITY e = pEnt->EntityPtr();
          pTag = Attr_Value(Find_Attr(e, TagAttribStr));
          }
        }
      ScreenPoint.x=(int)pDsp->CurrentPt.Screen.X+5;
      ScreenPoint.y=(int)pDsp->CurrentPt.Screen.Y+5;
      pDsp->TheWnd->ClientToScreen(&ScreenPoint);

      switch (When)
        {
        case EX_MOUSE_LDCLK :
          DwnAction=m_LeftDbl;
          break;
        case EX_MOUSE_LDOWN :
          if (gs_pCmd->CtrlDown() && gs_pCmd->ShiftDown())
            DwnAction=m_LeftCtSh;
          else if (gs_pCmd->CtrlDown())
            DwnAction=m_LeftCtrl;
          else if (gs_pCmd->ShiftDown())
            DwnAction=m_LeftShft;
          else
            DwnAction=m_Left;
          break;
        case EX_MOUSE_RDCLK :
          DwnAction=m_RightDbl;
          break;
        case EX_MOUSE_RDOWN :
          if (gs_pCmd->CtrlDown() && gs_pCmd->ShiftDown())
            DwnAction=m_RightCtSh;
          else if (gs_pCmd->CtrlDown())
            DwnAction=m_RightCtrl;
          else if (gs_pCmd->ShiftDown())
            DwnAction=m_RightShft;
          else
            DwnAction=m_Right;
          break;
        }

      break;
      }
    case EX_MOUSE_LUP :
    case EX_MOUSE_RUP :
      {
      CMdlValueShow::TryClear(false);
      break;
      }
    case EX_RESULTS :
      DumpErrors();
      break;
    }

  switch (DwnAction)
    {
    case MBA_None:
      break;
    case MBA_AccessMenu:
      pTag=NULL;
      // fall thru
    case MBA_AccessClosest:
    case MBA_AltAccessClosest:
      if (pTag==NULL)
        pTag=DoAccessMenu(pTag);
      if (pTag)
        {
        Wait.UpdateMsg("Access %s", pTag);
        gs_AccessWnds.AccessNode((DwnAction==MBA_AltAccessClosest) ? -2:-1, pTag);
        }
      break;
    case MBA_GotoConnected:
      if (pTag==NULL)
        pTag=DoAccessMenu(pTag);
      if (pTag)
        {
        CMdlActionArray Acts;
        int nActs=0;
        bool FollowDone=false;
        if (gs_Exec.GetModelAction(pTag, Acts))
          {
          nActs=Acts.GetSize();
          for (int i=0; i<nActs; i++)
            {
            if (Acts[i].iType==MAT_Follow)
              {
              gs_Exec.SetModelAction(pTag, Acts[i]);
              FollowDone=true;
              }
            }
          }
        if (!FollowDone)
          {
          CGrfDoc * pGrfDoc = dynamic_cast<CGrfDoc*>(pDoc);
          char* pTxt = new char[strlen(pTag)+1+strlen(pGrfDoc->GetTitle())+1];
          strcpy(pTxt, pTag);
          strcat(pTxt, "\t"); 
          strcat(pTxt, pGrfDoc->GetTitle()); // current graphics page
          ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_MARK|SUB_TAG_ACCESS, (LPARAM)pTxt);
          };
        }
      break;
    case MBA_WiringClosest:
      if (pTag==NULL)
        pTag=DoAccessMenu(pTag);
      if (pTag)
        {
        //CELnkWiring::Start(pTag);
        CWirePanelWindow::Show(pTag, true);
        }
      break;
    case MBA_QuickView:
      if (pTag)
        CMdlValueShow::TryAdd(pTag, ScreenPoint.x, ScreenPoint.y, false, false, pWnd);
      break;
    case MBA_QuickViewPin:
      if (pTag)
        CMdlValueShow::TryAdd(pTag, ScreenPoint.x, ScreenPoint.y, true, false, pWnd);
      break;
    case MBA_QuickViewMerge:
      if (pTag)
        CMdlValueShow::TryAdd(pTag, ScreenPoint.x, ScreenPoint.y, true, true, pWnd);
      break;
    case MBA_MdlActionsMenu:
      if (pTag)
        DoActionMenu(pTag);
      break;
    case MBA_ShowPressNets:
      if (pTag)
        {
        Strng_List ActHolds;
        ActHolds.Append(pTag);
        FE()->FE_TagOperation(FETOp_MarkPrsNets, ActHolds);
        }
      break;
    case MBA_ShowFlowNets:
      if (pTag)
        {
        Strng_List ActHolds;
        ActHolds.Append(pTag);
        FE()->FE_TagOperation(FETOp_MarkFlwNets, ActHolds);
        }
      break;
    case MBA_ClearMarks:
      if (1)
        {
        // What is doing here ???
        if (pTag)
          AddTextToClipboard(AfxGetMainWnd(), pTag, "\t");
        Strng_List ActHolds;
        ActHolds.Append("*");
        FE()->FE_TagOperation(FETOp_ClrMarked, ActHolds);
        }
      break;
    case MBA_Switch:
      if (pTag)
        DoToggleItem(pTag);
      break;
    }

  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoObjectView()
  {
  if (TestDlgBusy() || gs_pCmd->KBDBlocked())
    return; //blocked
  }
//---------------------------------------------------------------------------

void GrfCmdBlk::DoConfig()
  {
  pDsp->Open();
  //if (!Access_Busy)
    {
    //Access_Busy = 1;
    switch (When)
      {
      case EX_MODIFIER_RDY :
        switch (Modifier)
          {
          case ModNoun :
            AccSelect = 0;
            break;
          case 1 :                     // Dig
            gs_pCmd->SetDigInfo(GC_NoCurs, 1);
            break;
          case 2 :
            AccSelect = 1;
            break;
          default :;
          }
        break;
      case EX_MOUSE_LUP :
        {
        CWaitCursor Wait;
        pchar pTag = Attr_Value(pDrw->ClosestAttr(TagAttribStr, pDsp->CurrentPt));
        if (pTag)
          gs_AccessWnds.AccessNode(-1, pTag);//, Noun == 1, AccSelect, False);
        else
          gs_pCmd->Print("Nothing Found\n");
        break;
        }
      case EX_EXEC :
        break;
      case EX_DOIT :
        break;
      case EX_RESULTS :
        DumpErrors();
        break;
      }
    //Access_Busy = 0;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoInsert()
  {
  static CInsertUnitDlg* pMdlDlg = NULL;
  if (gs_Exec.Busy())
    return;
  CInsertBlk* CB = (CInsertBlk*)(gs_pCmd->GetDataBlk());
  flag DoIt = 0;
  pDsp->Open();
  enum MId { MID_Dig=1, MID_Tag, MID_BaseTag,
             MID_Scale, MID_XScale, MID_YScale, MID_Rotate,
             MID_XY, MID_X, MID_Y, MID_Model, MID_Dlg };
  bool DoMdl = (Noun==1);
  bool DoSym = (Noun==1 || Noun==2);
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          bFlag3 = 0;  //set true if CalcUnits must be called
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            Pt1.Zero();
            Pt2.Zero();
            ATag = "";
            AClass = "";
            ASymbol = "";
            HideTagOff=False;
            }
          break;
        case MID_Dig : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Tag : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_BaseTag : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_Scale : gs_pCmd->SetParmInfo(AFloat,  2); break;
        case MID_XScale : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_YScale : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Rotate : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_XY     : gs_pCmd->SetParmInfo(AFloat, 2); break;
        case MID_X      : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Y      : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Model  : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_Dlg    : pMdlDlg = new CInsertUnitDlg(DoMdl?CDlgWhat_InsUnit:CDlgWhat_InsSymb, false, pMdl, AfxGetMainWnd()); break;
        default :;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Tag     : ATag = gs_pCmd->LastToken(); break;
        case MID_BaseTag : TagBase = gs_pCmd->LastToken(); break;
        case MID_Scale   : gs_pCmd->dParm(NdScl[gs_pCmd->ParmNumber()]); break;
        case MID_XScale  : gs_pCmd->dParm(NdScl.X); break;
        case MID_YScale  : gs_pCmd->dParm(NdScl.Y); break;
        case MID_Rotate  : gs_pCmd->fParm(Rotate); break;
        case MID_XY      : gs_pCmd->dParm(Pt1.World[gs_pCmd->ParmNumber()]); break;
        case MID_X       : gs_pCmd->dParm(Pt1.World.X); gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Y       : gs_pCmd->dParm(Pt1.World.Y); gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Dlg     : AClass = gs_pCmd->LastToken(); break;
        default :;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case MID_Dig :
        case MID_X   :
        case MID_Y   :
          {
          if (pMdlDlg!=NULL)
            pMdlDlg->SaveLastModelData();
          switch (Modifier)
            {
            case MID_Dig : Pt1 = pDsp->CurrentPt; break;
            case MID_X   : Pt1.GetY(pDsp->CurrentPt); break;
            case MID_Y   : Pt1.GetX(pDsp->CurrentPt); break;
            }
          CB = new CInsertBlk;
          gs_pCmd->SetDataBlk((void *) CB);

          bool DoAddModel=DoMdl;
          if (pMdlDlg)
            {
            pMdlDlg->CompleteForUse(true);
            if (DoMdl)
              DoAddModel=!pMdlDlg->m_bExistingModel;
            ATag = pMdlDlg->m_Tag();
            AClass = pMdlDlg->m_ModelClass();

            ASymbol = pMdlDlg->m_SymbolName();
            TagBase = pMdlDlg->m_BaseTag;
            NdScl.X = pMdlDlg->Scl_X();
            NdScl.Y = pMdlDlg->Scl_Y();
            Rotate = (float)pMdlDlg->Rotation();
            HideTag = pMdlDlg->m_HideTag;
            HideEqpId = True;//pMdlDlg->m_HideEqpId;
            const int tblen = TagBase.Len();
            }

          CB->ATag = ATag;
          CB->AClass = AClass;
          CB->ASymbol = ASymbol;
          CB->ATagBase = TagBase;
          CB->Pt = Pt1;
          CB->NdScl = NdScl;
          //CB->Scl.X = Max(0.001, CB->Scl.X);
          //CB->Scl.Y = Max(0.001, CB->Scl.Y);
          //CB->Scl.Z = Max(0.001, CB->Scl.Z);
          CB->NdScl.Z = CB->NdScl.X; //make Z scale same as X
          CB->Rotate = Range(-360.0F, Rotate, 360.0F);
          CB->MdlInsertErr = -1;

          pDsp->Show_Dig_Point(Pt1);

          bool AllOK=true;
          bool ChkTag=true;
          switch (FindTheTag(CB->ATag, CB->AClass()))
            {
            case 0: // Exists
              if (DoAddModel)
                {
                if (AfxMessageBox("Model Exists\nAdd Symbol Only", MB_YESNO|MB_ICONQUESTION)==IDYES)
                  {
                  ChkTag=false;
                  DoAddModel = false;
                  }
                else
                  AllOK=false;
                }
              else
                ChkTag=true;
              break;
            case -1:  // Bad
              break;
            case -2: // Not Found
              if (!DoAddModel && DoMdl)
                {
                if (AfxMessageBox("Model is Missing\nAdd It", MB_YESNO|MB_ICONQUESTION)==IDYES)
                  {
                  DoAddModel =true;
                  ChkTag=true;
                  }
                else
                  AllOK=false;
                }
              break;
            };

          if (DoMdl && gs_License.NodeCountExceeded(1, eLic_MsgBox))
            {
            LogWarning("License", LF_DoAfxMsgBox|LF_Exclamation, "Maximum number of units (%d) exceeded", gs_License.MaxNodesAllowed());
            AllOK=false;
            }

          if (AllOK)
            {
            if (ChkTag)
              CheckTheTag(CB->ATag, CB->AClass(), TagBase.Length() == 0 ? CB->AClass() : TagBase(), true);
            
            Tag_Attr_Set.Flags=HideTag ? DXF_ATTRIB_INVIS : 0;

#if SYSCAD10         
            if (gs_pPrj->SvcActive)
              {
              SCD10ENTER;
              //What about Symbol only ??????????
              gs_pPrj->Svc.GCBCreateNode((CGrfDoc*)pDoc, PrjName(), pDoc->GetTitle(), /*CreateGUIDStr(),*/ CB->ATag(), CB->ASymbol(), CB->AClass(), CB->Pt.World, CB->NdScl, (float)CB->Rotate);

              SCD10LEAVE;
              }
            else
#endif
              {
              CB->e = AddUnitDrawing(CB->ATagBase(), CB->ASymbol(), CB->AClass(), CB->m_sGuid(), DoAddModel?CB->ATag():NULL, NULL, CB->Pt.World, CB->NdScl, (float)CB->Rotate, True, Tag_Attr_Set);
              if (CB->e)
                {
                Strng TheGuid;
                pDsp->Draw(CB->e, -1);
                if (DoAddModel)
                  CB->MdlInsertErr = AddUnitModel(CB->AClass(), CB->ATag(), TheGuid());
                pDsp->Draw(CB->e, -1);
                //            pDrw->EntityInvalidate(en, NULL);
                }
              }
            }

          if (pMdlDlg)
            {
            pMdlDlg->m_Tag = CB->ATag();
            pMdlDlg->m_bTagChgd = 0;
            pMdlDlg->m_bExistingModel = false;
            pMdlDlg->UpdateData(False);
            pMdlDlg->ChooseNewTag();
            }

          ATag="";
          break;
          }
        default :;
        }
      if (pMdlDlg)
        pMdlDlg->SetFocus();
      break;
    case EX_EXEC :
      break;
    case EX_DOIT :
      if (pMdlDlg!=NULL)
        pMdlDlg->SaveLastModelData();
      if (gs_pCmd->OwnerOfDataBlk())
        {
        delete CB;
        gs_pCmd->SetDataBlk(NULL);
        }
      break;
    case EX_UNDO :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        if (pDrw->Exists(CB->e))
          {
          pDsp->Hide_Dig_Point(CB->Pt);
          pDsp->Draw(CB->e, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(CB->e);
          }

        dbgpln("Delete %s", CB->ATag);
        if (CB->MdlInsertErr==0 && pMdl!=NULL)
          {
          CMdlValueSet::Clear();
          int RetCode = gs_Exec.DeleteTags(Strng_List(CB->ATag()));
          if (RetCode!=EODT_DONE)
            LogError(CB->ATag(), LF_DoAfxMsgBox|LF_Exclamation, "Model not deleted");
          else
            bFlag3 = 1;
          /*int err = pMdl->DeleteNodeModel(CB->ATag());
          if (err)
            {
            //Strng Msg;
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model not deleted[%i]\n%s", err, CB->ATag());
            //LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, Msg());
            //AfxMessageBox(Msg(), MB_ICONEXCLAMATION);
            LogError(CB->ATag(), 0, "GrfCmds: Model not deleted[%i]", err);
            }
          else
            bFlag3 = 1;*/
          }
        delete CB;
        gs_pCmd->SetDataBlk(NULL);
        }
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      if (pMdlDlg)
        {
        pMdlDlg->DestroyWindow();
        delete pMdlDlg;
        pMdlDlg = NULL;
        }
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }

  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoChangeUnit()
  {
  static CInsertUnitDlg* pMdlDlg = NULL;
  if (gs_Exec.Busy())
    return;
  CChangeBlk* CB = (CChangeBlk*)(gs_pCmd->GetDataBlk());
  flag DoIt = 0;
  pDsp->Open();
  enum MId { MID_Dig=1, MID_Dlg, MID_Update};//};
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          bFlag3 = 0;  //set true if CalcUnits must be called
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            Pt1.Zero();
            Pt2.Zero();
            ATag = "";
            AClass = "";
            ASymbol = "";
            HideTagOff=False;
            //gs_pCmd->SetDigInfo(GC_NoCurs, 1);
            pGWnd->SetCursor(IDC_SELECTUNIT);
            }
          break;
        case MID_Dig :
          gs_pCmd->SetDigInfo(GC_NoCurs, 1);
          break;
        case MID_Update:
          if (CB && pMdlDlg)
            {
            CWaitMsgCursor Wait("Changing entities");
            pMdlDlg->CompleteForUse(false);
            ATag = pMdlDlg->m_Tag();
            Strng AssocTag;

            int DeletesFailedCnt = 0;
            int MdlDeletes = 0;
            if (CB->m_EntInVw)
              {
              DXF_ENTITY e = CB->m_EntInVw->EntityPtr();
              C3_CURVE   c = CB->m_EntInVw->CurvePtr();
              if (1 && c && pDrw->Exists(c))
                {
                pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
                pDrw->Delete(c);
                }
              if (e && pDrw->Exists(e))
                {
                if (DXF_ENTITY_IS_INSERT(e))
                  {                                   
                  AssocTag = Find_Attr_Value(e, AssocTagAttribStr);
                  
                  Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
                  pDsp->SetCPtWorld(Pos, Pt1);
                  }
                //pchar pTag;
                //if (0)//DelMdl)
                //  {
                //  if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
                //    {
                //    CMdlValueSet::Clear();
                //    int RetCode = gs_Exec.DeleteTags(Strng_List(pTag));
                //    if (RetCode!=EODT_DONE)
                //      {
                //      LogError(pTag, 0, "Model not deleted");
                //      DeletesFailedCnt++;
                //      }
                //    else
                //      MdlDeletes++;
                //    }
                //  }
                if (1)
                  {
                  pDsp->Draw(e, GrfHelper.GR_BACKGROUND);
                  pDrw->Delete(e);
                  }
                }
              }
            if (DeletesFailedCnt)
              {
              if (DeletesFailedCnt==1)
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "A model was not deleted");
              else
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "%d models were not deleted.", DeletesFailedCnt);
              }

            ASymbol = pMdlDlg->m_SymbolName();
            TagBase = pMdlDlg->m_BaseTag;
            NdScl.X = pMdlDlg->Scl_X();
            NdScl.Y = pMdlDlg->Scl_Y();
            Rotate = (float)pMdlDlg->Rotation();
            HideTag = pMdlDlg->m_HideTag;
            HideEqpId = True;//pMdlDlg->m_HideEqpId;


            CB->ATag = pMdlDlg->m_Tag();
            CB->AClass = pMdlDlg->m_ModelClass();
            CB->ASymbol = pMdlDlg->m_SymbolName();
            CB->ATagBase = pMdlDlg->m_BaseTag;

            CB->Pt = Pt1;
            CB->Scl = NdScl;
            CB->Scl.Z = CB->Scl.X; //make Z scale same as X
            CB->Rotate = Range(-360.0F, Rotate, 360.0F);

            //pDsp->Show_Dig_Point(Pt1);
            Tag_Attr_Set.Flags=HideTag ? DXF_ATTRIB_INVIS : 0;

            CB->e = AddUnitDrawing(CB->ATagBase(), CB->ASymbol(), CB->AClass(), 
              (LPSTR)(LPCSTR)TaggedObject::CreateGuidStr(), CB->ATag(), AssocTag(),
              CB->Pt.World, CB->Scl, CB->Rotate, True, Tag_Attr_Set);
            
            pDsp->Draw(CB->e, -1);

            pDsp->Vp1->ClearAllEntity();
            pDsp->Vp1->ClrSelectionAllList();

            BOOL AlreadySelected=false;
            CEntInView *p = pDsp->Vp1->FindEntInView4Entity(CB->e);

            CB->m_Entity=CB->e;
            CB->m_EntInVw=p;

            int iWork=CB->m_Items.GetUpperBound();
            CB->m_Items[iWork].m_Entity  = CB->m_Entity;
            CB->m_Items[iWork].m_EntInVw = CB->m_EntInVw;
            CB->m_Items[iWork].m_Changed=true;

            if (0 && CB->e)
              {
              //CB->MdlInsertErr = AddUnitModel(CB->AClass(), CB->ATag());
              //pDsp->Draw(CB->e, -1);
              //            pDrw->EntityInvalidate(en, NULL);
              }
          break;
          }
        default :;
        }
      break;
    case EX_PARAMETER_RDY :
      break;
    case EX_MOUSE_LDOWN :
      switch (Modifier)
        {
        case MID_Dig :
          if (CB && pMdlDlg && CB->m_EntInVw && CB->m_Entity)
            {
            dbgpln("Down (%.2f,%.2f)", pDsp->CurrentPt.World.X, pDsp->CurrentPt.World.Y);
            CB->m_bMoving=true;
            CB->Pt=pDsp->CurrentPt;
            }
        }
      break;
    case EX_MOUSE_LMOVE :
      switch (Modifier)
        {
        case MID_Dig :
          if (CB && pMdlDlg && CB->m_EntInVw)// && CB->m_Entity)
            {
            if (CB->m_bMoving && CB->m_EntInVw)
              {
              DXF_ENTITY e = CB->m_EntInVw->EntityPtr();
              C3_CURVE   c = CB->m_EntInVw->CurvePtr();
              dbgpln("Move %#08x %#08x (%.2f,%.2f)", e,c,pDsp->CurrentPt.World.X, pDsp->CurrentPt.World.Y);

              if (c && pDrw->Exists(c))
                {
                pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
                //pDrw->Delete(c);
                }
              if (e && pDrw->Exists(e))
                {
                if (DXF_ENTITY_IS_INSERT(e))
                  {
                  dbgpln("                                      Undraw (%.2f,%.2f)", DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1]);
                  pDsp->Draw(e, GrfHelper.GR_BACKGROUND);

                  DXF_INSERT_PT(e)[0]+=pDsp->CurrentPt.World.X-CB->Pt.World.X;
                  DXF_INSERT_PT(e)[1]+=pDsp->CurrentPt.World.Y-CB->Pt.World.Y;
                  DXF_INSERT_PT(e)[2]+=pDsp->CurrentPt.World.Z-CB->Pt.World.Z;
                  dbgpln("                                      Draw   (%.2f,%.2f)", DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1]);
                  pDsp->Draw(e, -1);
                  CB->m_EntInVw = pDsp->Vp1->FindEntInView4Entity(e);
                  }
                }
              if (c && pDrw->Exists(c))
                {
                pDsp->Draw(c, -1);
                //pDrw->Delete(c);
                }
              }
            CB->Pt=pDsp->CurrentPt;
            }
          break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case MID_Dig :
          {
          if (pMdlDlg!=NULL)
            pMdlDlg->SaveLastModelData();

          if (CB)
            CB->m_bMoving=false;
          CWaitCursor Wait;
          BOOL AlreadySelected=false;
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          CEntInView *p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
          if (p)
            {
            DXF_ENTITY e=p->EntityPtr();
            if (e && DXF_ENTITY_IS_INSERT(e))
              {
              char * pTag = Find_Attr_Value(e, TagAttribStr);
              char * pSymbol = DXF_INSERT_BLOCK_GET(e);
              double XScl=DXF_INSERT_X_SCALE(e);
              double YScl=DXF_INSERT_Y_SCALE(e);
              //double XScl=DXF_INSERT_Z_SCALE(e);
              double Rot=DXF_INSERT_ROT_ANG(e);
              bool TagHidden = (Find_Attr_AFlags(e, TagAttribStr) & DXF_ATTRIB_INVIS )!=0;


              Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
              pDsp->SetCPtWorld(Pos, Pt1);
              //Pt1=DXF_INSERT_PT(e);

              if (pTag)
                {
                CXM_ObjectTag ObjTag(pTag, TABOpt_Parms);//TABOpt_AllInfoOnce);
                CXM_ObjectData ObjData;
                CXM_Route Route;
                if (XReadTaggedItem(ObjTag, ObjData, Route))
                  {
                  Strng Class;
                  Class = ObjData.FirstItem()->Class();

                  TagObjClass * pTagObjC = TagObjClass::FindClassId(Class());
                  if (pTagObjC==NULL)
                    pTagObjC = TagObjClass::FindClassName(Class());
                  if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 ||
                                  _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 ||
                                  _stricmp(ElecLinkGrp, pTagObjC->Group())==0 ||
                                  _stricmp(AirLinkGrp, pTagObjC->Group())==0))
                    {
                    pDsp->Vp1->ClearAllEntity();
                    pDsp->Vp1->ClrSelectionAllList();
                    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Cannot change a link");
                    }
                  else
                    {
                    ATag=pTag;
                    AClass=pTagObjC->ShortDesc();
                    if (AClass.GetLength()<1)
                      AClass=pTagObjC->ClassId();

                    //Pt1 = pDsp->CurrentPt;
                    if (pMdlDlg==NULL)
                      {
                      CB = new CChangeBlk;
                      gs_pCmd->SetDataBlk((void *) CB);
                      pMdlDlg = new CInsertUnitDlg(CDlgWhat_ChgUnit, false, pMdl, AfxGetMainWnd());
                      }

                    pMdlDlg->HoldFeedback(true);
                    CB->m_Entity=e;
                    CB->m_EntInVw=p;
                    ASymbol=pSymbol;

                    //ASymbol=pTagObjC->DrwGroup();


                    //MakeValidSymbolName(ASymbol());

                    pMdlDlg->SetTag(ATag());
                    pMdlDlg->SetTagHidden(TagHidden);
                    pMdlDlg->SetClass(AClass());
                    pMdlDlg->SetBlockName(ASymbol(), pTagObjC->DrwGroup(), pTagObjC->ClassId());
                    pMdlDlg->SetOriginalScales(XScl, YScl, Rot);

                    pMdlDlg->HoldFeedback(false);
                    // Keep Undo Info
                    CChangeBlk::CChangeItem Itm;
                    Itm.ATag=ATag;
                    Itm.AClass=AClass;
                    Itm.ASymbol=ASymbol;
                    Itm.Pt=Pt1;
                    Itm.Scl.X=XScl;
                    Itm.Scl.Y=YScl;
                    Itm.Scl.Z=XScl;
                    Itm.Rotate=(float)Rot;
                    Itm.m_Entity=e;
                    Itm.m_EntInVw=p;
                    Itm.m_Changed=false;

                    CB->m_Items.Add(Itm);

                    }
                  //break;
                  }
                else
                  {
                  pDsp->Vp1->ClearAllEntity();
                  pDsp->Vp1->ClrSelectionAllList();
                  LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Class No Found");
                  }
                }
              else
                {
                pDsp->Vp1->ClearAllEntity();
                pDsp->Vp1->ClrSelectionAllList();
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Bad Graphics Entity");
                }
              }
            else
              {
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              LogError("GrfCmds", 0, "Nothing Found");
              }
            }
          break;
          }
        default :;
        }
      if (pMdlDlg)
        pMdlDlg->SetFocus();
      break;
    case EX_EXEC :
      break;
    case EX_DOIT :
      if (pMdlDlg!=NULL)
        pMdlDlg->SaveLastModelData();
      if (gs_pCmd->OwnerOfDataBlk())
        {
        delete CB;
        gs_pCmd->SetDataBlk(NULL);

        gs_pCmd->ExtendCmdLine("\x1b");
        }
      break;
    case EX_UNDO :
      if (CB && gs_pCmd->OwnerOfDataBlk())
        {
        CWaitMsgCursor Wait("Undoing Changes");
        for (int iUnDo=0; iUnDo<CB->m_Items.GetCount(); iUnDo++)
          {
          CChangeBlk::CChangeItem &Itm=CB->m_Items[iUnDo];
          if (Itm.m_Changed)
            {
            int DeletesFailedCnt = 0;
            int MdlDeletes = 0;
            if (Itm.m_EntInVw)
              {
              DXF_ENTITY e = Itm.m_EntInVw->EntityPtr();
              C3_CURVE   c = Itm.m_EntInVw->CurvePtr();
              if (1 && c && pDrw->Exists(c))
                {
                pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
                pDrw->Delete(c);
                }
              if (e && pDrw->Exists(e))
                {
                if (DXF_ENTITY_IS_INSERT(e))
                  {
                  Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
                  pDsp->SetCPtWorld(Pos, Pt1);
                  }
                //pchar pTag;
                //if (0)//DelMdl)
                //  {
                //  if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
                //    {
                //    CMdlValueSet::Clear();
                //    int RetCode = gs_Exec.DeleteTags(Strng_List(pTag));
                //    if (RetCode!=EODT_DONE)
                //      {
                //      LogError(pTag, 0, "Model not deleted");
                //      DeletesFailedCnt++;
                //      }
                //    else
                //      MdlDeletes++;
                //    }
                //  }
                if (1)
                  {
                  pDsp->Draw(e, GrfHelper.GR_BACKGROUND);
                  pDrw->Delete(e);
                  }
                }
              }
            if (DeletesFailedCnt)
              {
              if (DeletesFailedCnt==1)
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "A model was not deleted");
              else
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "%d models were not deleted.", DeletesFailedCnt);
              }

            CB->ATag = Itm.ATag;
            CB->AClass = Itm.AClass;
            CB->ASymbol = Itm.ASymbol;
            CB->ATagBase = "AAA";//Itm.TagBase;
            CB->Pt = Itm.Pt;
            CB->Scl = Itm.Scl;
            CB->Rotate = Itm.Rotate;

            //pDsp->Show_Dig_Point(Pt1);
            Tag_Attr_Set.Flags=HideTag ? DXF_ATTRIB_INVIS : 0;
            CB->e = AddUnitDrawing(CB->ATagBase(), CB->ASymbol(), CB->AClass(),
                                   (LPSTR)(LPCSTR)TaggedObject::CreateGuidStr(), 
                                   CB->ATag(), NULL, CB->Pt.World, CB->Scl, CB->Rotate, True, Tag_Attr_Set);
            pDsp->Draw(CB->e, -1);

            pDsp->Vp1->ClearAllEntity();
            pDsp->Vp1->ClrSelectionAllList();

            //BOOL AlreadySelected=false;
            //CEntInView *p = pDsp->Vp1->SelectClosestEntity(CB->Pt, AlreadySelected);

            //CB->m_Entity=CB->e;
            //CB->m_EntInVw=p;

            if (0 && CB->e)
              {
              //CB->MdlInsertErr = AddUnitModel(CB->AClass(), CB->ATag());
              //pDsp->Draw(CB->e, -1);
              //            pDrw->EntityInvalidate(en, NULL);
              }
            }
          }


        //if (pDrw->Exists(CB->e))
        //  {
        //  pDsp->Draw(CB->e, GrfHelper.GR_BACKGROUND);
        //  pDrw->Delete(CB->e);
        //  }

        //dbgpln("Delete %s", CB->ATag);
        //if (CB->MdlInsertErr==0 && pMdl!=NULL)
        //  {
        //  CMdlValueSet::Clear();
        //  int RetCode = gs_Exec.DeleteTag(CB->ATag());
        //  if (RetCode!=EODT_DONE)
        //    LogError(CB->ATag(), LF_DoAfxMsgBox|LF_Exclamation, "Model not deleted");
        //  else
        //    bFlag3 = 1;
        //  /*int err = pMdl->DeleteNodeModel(CB->ATag());
        //  if (err)
        //    {
        //    //Strng Msg;
        //    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model not deleted[%i]\n%s", err, CB->ATag());
        //    //LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, Msg());
        //    //AfxMessageBox(Msg(), MB_ICONEXCLAMATION);
        //    LogError(CB->ATag(), 0, "GrfCmds: Model not deleted[%i]", err);
        //    }
        //  else
        //    bFlag3 = 1;*/
        //  }
        delete CB;
        gs_pCmd->SetDataBlk(NULL);
        }
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      if (pMdlDlg)
        {
        pMdlDlg->DestroyWindow();
        delete pMdlDlg;
        pMdlDlg = NULL;
        }
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }

  pDsp->Close();
  }

//---------------------------------------------------------------------------

CLineDrawHelper::CLineDrawHelper()
  {
  m_dArrowScale = 2.5; // size 2.5 mm
  m_eLineMethod = LDM_Orth;
  m_eArrowRule  = ARM_EndDirnAndLast;
  m_bShowTag    = true;
  m_TagScale.X  = 1.0;
  m_TagScale.Y  = 1.0;
  m_TagScale.Z  = 1.0;
  m_TagRotation = 0.0;

  Init(NULL, NULL);
  }

void CLineDrawHelper::Init(Grf3D_Display* pDsp, DXF_Drawing *pDrw)
  {
  m_pDsp = pDsp;
  m_pDrw = pDrw;
  m_iNVerts = 0;
  m_bShowDigPoint = m_pDsp ? true : false;
  m_pBlock = NULL;
  m_TagScale.X  = 1.0;
  m_TagScale.Y  = 1.0;
  m_TagScale.Z  = 1.0;
  m_TagRotation = 0.0;
  m_sAssocTag="";
  }

void CLineDrawHelper::InitPoints()
  {
  m_iNVerts = 0;
  }

bool CLineDrawHelper::ValidGraphics()
  {
  if (m_iNVerts>0 && m_PType[m_iNVerts-1]==LPT_Break)
    {//cannot end with a break
    m_iNVerts--;
    if (m_bShowDigPoint)
      m_pDsp->Hide_Dig_Point(m_Verts[m_iNVerts]);
    }

  if (m_iNVerts>1)
    return true;
  return false;
  }

bool CLineDrawHelper::AddPoint(Pt_SLW & Pt, LinePtType Type)
  {
  if (m_iNVerts>=MaxVerts)
    return false; //too many points
  if (m_iNVerts<=1 && Type==LPT_Break)
    return false; //must start with at least one line segment (2 points)
  if (Type==LPT_Break && m_iNVerts>0 && m_PType[m_iNVerts-1]==LPT_Break)
    {//cannot have two line-breaks in a row, overwrite previous
    m_iNVerts--;
    if (m_bShowDigPoint)
      m_pDsp->Hide_Dig_Point(m_Verts[m_iNVerts]);
    }
  m_Verts[m_iNVerts] = Pt;
  m_PType[m_iNVerts] = Type;
  Pt_SLW & p1 = m_Verts[m_iNVerts];
  if (m_iNVerts>0)
    {
    Pt_SLW & p0 = m_Verts[m_iNVerts-1];
    if (m_eLineMethod==LDM_Orth)
      {
      if (fabs(p1.World.X - p0.World.X) > fabs(p1.World.Y - p0.World.Y))
        p1.GetY(p0);
      else
        p1.GetX(p0);
      }
    if (m_pDsp && m_PType[m_iNVerts]!=LPT_Break)
      m_pDsp->Show_Dig_Line(p0, p1);
    }
  if (m_bShowDigPoint)
    m_pDsp->Show_Dig_Point(p1);
  m_iNVerts++;
  return true;
  }

bool CLineDrawHelper::AddPoint(Pt_3f & Pt, LinePtType Type)
  {
  Pt_SLW Pt0;
  Pt0.World = Pt;
  return AddPoint(Pt0, Type);
  }

bool CLineDrawHelper::AddPoint(LinePtType Type)
  {
  return AddPoint(m_pDsp->CurrentPt, Type);
  }

void CLineDrawHelper::RemovePoint()
  {
  if (m_iNVerts==0)
    return;
  m_iNVerts--;
  if (m_iNVerts>0 && m_PType[m_iNVerts]!=LPT_Break)
    m_pDsp->Hide_Dig_Line(m_Verts[m_iNVerts-1], m_Verts[m_iNVerts]);
  if (m_bShowDigPoint)
    m_pDsp->Hide_Dig_Point(m_Verts[m_iNVerts]);
  }

void CLineDrawHelper::ShowLine()
  {
  if (m_iNVerts==0)
    return;
  if (m_bShowDigPoint)
    m_pDsp->Show_Dig_Point(m_Verts[0]);
  for (int i=1; i<m_iNVerts; i++)
    {
    if (m_PType[i]!=LPT_Break)
      m_pDsp->Show_Dig_Line(m_Verts[i-1], m_Verts[i]);
    if (m_bShowDigPoint)
      m_pDsp->Show_Dig_Point(m_Verts[i]);
    }
  }

void CLineDrawHelper::HideLine()
  {
  if (m_iNVerts==0)
    return;
  if (m_bShowDigPoint)
    m_pDsp->Hide_Dig_Point(m_Verts[0]);
  for (int i=1; i<m_iNVerts; i++)
    {
    if (m_PType[i]!=LPT_Break)
      m_pDsp->Hide_Dig_Line(m_Verts[i-1], m_Verts[i]);
    if (m_bShowDigPoint)
      m_pDsp->Hide_Dig_Point(m_Verts[i]);
    }
  }

int CLineDrawHelper::FindLongestSegment()
  {
  double big = 0.0;
  int bigi = -1;
  for (int i=1; i<m_iNVerts; i++)
    {
    if (m_PType[i]!=LPT_Break)
      {
      const double d = GetDistance(i);
      if (d>big)
        {
        big = d;
        bigi = i;
        }
      }
    }
  return bigi;
  }

double CLineDrawHelper::GetDistance(int index)
  {
  if (index>0 && index<m_iNVerts)
    return m_Verts[index].World.Dist(m_Verts[index - 1].World);
  return 0.0;
  }

double CLineDrawHelper::GetAngle(int index)
  {
  if (index>0 && index<m_iNVerts)
    {
    Pt_3f & p0 = m_Verts[index-1].World;
    Pt_3f & p1 = m_Verts[index].World;
    return atan2(p1.Y - p0.Y, p1.X - p0.X);
    }
  return 0.0;
  }

void CLineDrawHelper::GetMidPoint(int index, Pt_3f & Pt)
  {
  if (index>0 && index<m_iNVerts)
    {
    Pt_3f & p0 = m_Verts[index-1].World;
    Pt_3f & p1 = m_Verts[index].World;
    Pt.Set((p1.X + p0.X) * 0.5, (p1.Y + p0.Y) * 0.5, (p1.Z + p0.Z) * 0.5);
    }
  }

void CLineDrawHelper::AddArrow(Pt_3f & Pt, double ang, double Thickness)
  {
  //draw arrow...
  Pt_3f vpt;
  C3_CURVE Arrow = m_pBlock->Add_PLine_Start(vpt.p());
  vpt.Set(-ArrowLeftSz, ArrowUpSz, 0.0);
  m_pBlock->Add_PLine_Vertex(Arrow, vpt.p());
  vpt.Set(-ArrowLeftSz, -ArrowUpSz, 0.0);
  m_pBlock->Add_PLine_Vertex(Arrow, vpt.p());
  vpt.Zero();
  m_pBlock->Add_PLine_Vertex(Arrow, vpt.p());
  vpt.Set(-0.8, 0.08, 0.0);
  m_pBlock->Add_PLine_Vertex(Arrow, vpt.p());
  vpt.Set(-0.8, -0.08, 0.0);
  m_pBlock->Add_PLine_Vertex(Arrow, vpt.p());
  vpt.Zero();
  m_pBlock->Add_PLine_Vertex(Arrow, vpt.p());

  Arrow->m_Thickness=Thickness;

  // scale and rotate the arrowhead
  PT3 o,v;
  o[0] = 0.0;  o[1] = 0.0;  o[2] = 0.0;
  v[0] = 0.0;  v[1] = 0.0;  v[2] = 1.0;
  c3c_scale(Arrow, o, (REAL)m_dArrowScale); 
  c3c_rotate(Arrow, o, v, (REAL)ang);

  //move to correct position
  Pt_3f P;
  P.Set(Pt.X-m_InsertPt.X, Pt.Y-m_InsertPt.Y, Pt.Z-m_InsertPt.Z);
  m_pDrw->TranslateEntity(Arrow, P, vpt);
  }

void CLineDrawHelper::AddArrow(int index, double Thickness)
  {
  if (index>0 && index<m_iNVerts)
    {
    const double ang = GetAngle(index);
    AddArrow(m_Verts[index].World, ang, Thickness);
    }
  }

void CLineDrawHelper::AddArrowMidPoint(int index, double Thickness)
  {
  if (index>0 && index<m_iNVerts)
    {
    Pt_3f Pt;
    GetMidPoint(index, Pt);
    const double ang = GetAngle(index);
    AddArrow(Pt, ang, Thickness);
    }
  }

void CLineDrawHelper::AddArrows(double Thickness)
  {
  switch (m_eArrowRule)
    {
    case ARM_None: break;
    case ARM_Centre: 
      {
      double big = 0.0;
      int bigi = -1;
      for (int i=1; i<m_iNVerts; i++)
        {
        if (m_PType[i]!=LPT_Break)
          {
          double d = GetDistance(i);
          if (d>big)
            {
            big = d;
            bigi = i;
            }
          }
        if (i==m_iNVerts-1 || m_PType[i]==LPT_Break)
          {
          AddArrowMidPoint(bigi, Thickness);
          big = 0.0;
          bigi = -1;
          }
        }
      break;
      }
    case ARM_Ends:
      for (int i=1; i<m_iNVerts; i++)
        {
        if (i==m_iNVerts-1)
          AddArrow(i, Thickness);
        else if (m_PType[i]==LPT_Break)
          AddArrow(i-1, Thickness);
        }
      break;
    case ARM_EndDirnAndLast: 
    case ARM_EndDirn: 
      {
      bool DoneOne = false;
      int Angi = 1;
      double PrevAng = GetAngle(1);
      for (int i=2; i<m_iNVerts; i++)
        {
        if (m_PType[i]!=LPT_Break)
          {
          const double Ang = GetAngle(i);
          if (fabs(PrevAng-Ang)>0.1)
            {
            const double d = GetDistance(Angi);
            if (d>m_dArrowScale*1.9)
              {
              AddArrow(Angi, Thickness);
              DoneOne = true;
              }
            }
          Angi = i;
          PrevAng = Ang;
          }
        }
      if (m_eArrowRule==ARM_EndDirnAndLast || !DoneOne)
        AddArrow(m_iNVerts-1, Thickness);
      break;
      }
    case ARM_CentreDirn: 
      {
      int bigi = 1;
      double big = GetDistance(1);
      double PrevAng = GetAngle(1);
      for (int i=2; i<m_iNVerts; i++)
        {
        if (m_PType[i]!=LPT_Break)
          {
          const double Ang = GetAngle(i);
          if (fabs(PrevAng-Ang)>0.1)
            {
            if (big>m_dArrowScale*1.9)
              AddArrowMidPoint(bigi, Thickness);
            bigi = i;
            big = GetDistance(i);
            }
          else
            {
            const double d = GetDistance(i);
            if (d>big)
              {
              big = d;
              bigi = i;
              }
            }
          PrevAng = Ang;
          }
        }
      AddArrowMidPoint(m_iNVerts-1, Thickness);
      break;
      }
    case ARM_EndAndCentre: 
      {
      double big = 0.0;
      int bigi = -1;
      for (int i=1; i<m_iNVerts; i++)
        {
        if (m_PType[i]!=LPT_Break)
          {
          double d = GetDistance(i);
          if (d>big)
            {
            big = d;
            bigi = i;
            }
          }
        if (i==m_iNVerts-1 || m_PType[i]==LPT_Break)
          {
          const int endi = (i==m_iNVerts-1 ? i : i-1);
          AddArrow(endi, Thickness);
          bool DoMidPoint = true;
          if (endi==bigi && big<m_dArrowScale*3.0)
            {//do not want two close or overlapping arrows
            DoMidPoint = false;
            }
          if (DoMidPoint)
            AddArrowMidPoint(bigi, Thickness);
          big = 0.0;
          bigi = -1;
          }
        }
      break;
      }
    case ARM_EndAndCentreEx: 
      {
      double big = 0.0;
      int bigi = -1;
      double big2 = 0.0;
      int big2i = -1;
      for (int i=1; i<m_iNVerts; i++)
        {
        if (m_PType[i]!=LPT_Break)
          {
          double d = GetDistance(i);
          if (d>big)
            {
            big2 = big;
            big2i = bigi;
            big = d;
            bigi = i;
            }
          }
        if (i==m_iNVerts-1 || m_PType[i]==LPT_Break)
          {
          const int endi = (i==m_iNVerts-1 ? i : i-1);
          if (i==m_iNVerts-1)
            AddArrow(endi, Thickness); //always draw arrow at end of last segment
          else if (big>m_dArrowScale*2.5)
            AddArrow(endi, Thickness); //there is a long segment so draw arrow at end of last segment

          if (endi!=bigi && big>m_dArrowScale*2.0)
            AddArrowMidPoint(bigi, Thickness); //arrow on midpoint of longest segment
          else if (big2i>=0 && big2>m_dArrowScale*3.0)
            AddArrowMidPoint(big2i, Thickness); //arrow on midpoint of second longest segment
          big = 0.0;
          bigi = -1;
          big2 = 0.0;
          big2i = -1;
          }
        }
      break;
      }
    }
  }

void CLineDrawHelper::AddLineData(double Thickness)
  {
  C3_CURVE Link = NULL;
  //Add the line segments to the block
  Pt_3f Pt;
  for (int i=0; i<m_iNVerts; i++)
    {
    Pt.Set(m_Verts[i].World.X-m_InsertPt.X, m_Verts[i].World.Y-m_InsertPt.Y, m_Verts[i].World.Z-m_InsertPt.Z);
    if (m_PType[i]!=LPT_Break)
      {
      if (Link)
        m_pBlock->Add_PLine_Vertex(Link, Pt.p());
      else
        Link = m_pBlock->Add_PLine_Start(Pt.p());
      }
    else
      {//start new poly line
      Link = m_pBlock->Add_PLine_Start(Pt.p());
      }
    }
  if(Link)
    Link->m_Thickness=2;
  }

//---------------------------------------------------------------------------

CLineDrawHelper TheLDH;

//---------------------------------------------------------------------------

void CreateDefaultLinkLine(CLineDrawHelper & LDH, pGrf3D_Display pDsp, Pt_SLW* pPt1, Pt_SLW* pPt2, bool Direct)
  {
  LDH.InitPoints();
  LDH.AddPoint(*pPt1, LPT_Line);
  LineDrawMethod PrevLineMethod = LDH.LineMethod();
  if (Direct)
    {
    LDH.SetLineMethod(LDM_Direct);
    LDH.AddPoint(*pPt2, LPT_Line);
    }
  else
    {
    LDH.SetLineMethod(LDM_Orth);
    const double xlen = pPt2->World.X - pPt1->World.X;
    const double ylen = pPt2->World.Y - pPt1->World.Y;
    if (fabs(ylen)<1.0e-9 || fabs(xlen)<1.0e-9 || fabs(ylen/xlen)<0.01 || fabs(xlen/ylen)<0.01)//test if either is zero or if one is 100 times bigger than the other
      {
      LDH.AddPoint(*pPt2, LPT_Line);
      }
    else
      {
      Pt_SLW Pt;
      Pt_3f Pos(pPt1->World);
      if (fabs(ylen)>fabs(xlen))
        {
        Pos.Y += (ylen/2.0);
        pDsp->SetCPtWorld(Pos, Pt);
        LDH.AddPoint(Pt, LPT_Line);
        Pos.X = pPt2->World.X;
        pDsp->SetCPtWorld(Pos, Pt);
        LDH.AddPoint(Pt, LPT_Line);
        }
      else
        {
        Pt_3f Pos(pPt1->World);
        Pos.X += (xlen/2.0);
        pDsp->SetCPtWorld(Pos, Pt);
        LDH.AddPoint(Pt, LPT_Line);
        Pos.Y = pPt2->World.Y;
        pDsp->SetCPtWorld(Pos, Pt);
        LDH.AddPoint(Pt, LPT_Line);
        }
      LDH.AddPoint(*pPt2, LPT_Line);
      }
    }
  LDH.SetLineMethod(PrevLineMethod);
  }

void OldCreateDefaultLinkLine(CConnectBlk* CB, pGrf3D_Display pDsp, Pt_SLW* pPt1, Pt_SLW* pPt2, flag Direct)
  {
  CB->Verts[0] = *pPt1;
  if (Direct)
    {
    CB->Verts[1] = *pPt2;
    CB->NVerts = 2;
    }
  else
    {
    const double xlen = pPt2->World.X - pPt1->World.X;
    const double ylen = pPt2->World.Y - pPt1->World.Y;
    if (fabs(ylen)<1.0e-9 || fabs(xlen)<1.0e-9 || fabs(ylen/xlen)<0.01 || fabs(xlen/ylen)<0.01)//test if either is zero or if one is 100 times bigger than the other
      {
      CB->Verts[1] = *pPt2;
      CB->NVerts = 2;
      }
    else
      {
      Pt_3f Pos(pPt1->World);
      if (fabs(ylen)>fabs(xlen))
        {
        Pos.Y += (ylen/2.0);
        pDsp->SetCPtWorld(Pos, CB->Verts[1]);
        Pos.X = pPt2->World.X;
        pDsp->SetCPtWorld(Pos, CB->Verts[2]);
        }
      else
        {
        Pt_3f Pos(pPt1->World);
        Pos.X += (xlen/2.0);
        pDsp->SetCPtWorld(Pos, CB->Verts[1]);
        Pos.Y = pPt2->World.Y;
        pDsp->SetCPtWorld(Pos, CB->Verts[2]);
        }
      CB->Verts[3] = *pPt2;
      CB->NVerts = 4;
      }
    }
  }

//---------------------------------------------------------------------------

void HideLine(CConnectBlk* CB, pGrf3D_Display pDsp)
  {
  if (CB->NVerts)
    pDsp->Hide_Dig_Point(CB->Verts[0]);
  for (int i=1; i<CB->NVerts; i++)
    {
    pDsp->Hide_Dig_Point(CB->Verts[i]);
    pDsp->Hide_Dig_Line(CB->Verts[i-1], CB->Verts[i]);
    }
  }

//---------------------------------------------------------------------------

void ShowLine(CConnectBlk* CB, pGrf3D_Display pDsp)
  {
  if (CB->NVerts)
    pDsp->Show_Dig_Point(CB->Verts[0]);
  for (int i=1; i<CB->NVerts; i++)
    {
    pDsp->Show_Dig_Point(CB->Verts[i]);
    pDsp->Show_Dig_Line(CB->Verts[i-1], CB->Verts[i]);
    }
  }

//---------------------------------------------------------------------------

bool GrfCmdBlk::DoInsertNodeGrf(CInsertBlk* CB, bool SkipTagTest)
  {
  if (!SkipTagTest && gs_pPrj->FlwLib()->FE_TestModelTagUnique(CB->ATag()))
    {
    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model for Tag '%s' should exist!", CB->ATag());
    return false;
    }
  pDsp->Open();

  Tag_Attr_Set.Flags = HideTag && !CB->m_TagVisible ? DXF_ATTRIB_INVIS : 0;
  Tag_InsertPt.Set(CB->m_TagPt.X-CB->Pt.World.X, CB->m_TagPt.Y-CB->Pt.World.Y, CB->m_TagPt.Z-CB->Pt.World.Z);
  CB->e = AddUnitDrawing(CB->ATagBase(), CB->ASymbol(), CB->AClass(), CB->m_sGuid(), CB->ATag(), NULL, CB->Pt.World, CB->NdScl, (float)CB->Rotate, True, Tag_Attr_Set, &Tag_InsertPt);
  if (CB->e)
    {
    if (CB->m_Rect.Width()>0)
      {
      DML_LIST Lst=dml_create_list();
      DML_LIST EmptyLst=dml_create_list();
      dml_insert_after(Lst, NULL, CB->e);
      C3_BOX_S Bnds ;                      
      pDrw->GetBounds(&Bnds, EmptyLst, Lst, EmptyLst);

      dml_free_list(Lst);
      dml_free_list(EmptyLst);


      //DXF_INSERT_PT(CB->e)[0]   = CB.m_Rect.MidX()-PageRct.Left();
      //DXF_INSERT_PT(CB->e)[1]   = PageRct.Top()-CB.m_Rect.MidY();
      //DXF_INSERT_PT(CB->e)[2]   = 0;
      DXF_INSERT_X_SCALE(CB->e)  = DXF_INSERT_X_SCALE(CB->e)  * CB->m_Rect.Width()/Max(1.0, (C3_MAX_X(&Bnds)-C3_MIN_X(&Bnds)));
      DXF_INSERT_Y_SCALE(CB->e)  = DXF_INSERT_Y_SCALE(CB->e)  * CB->m_Rect.Height()/Max(1.0, (C3_MAX_Y(&Bnds)-C3_MIN_Y(&Bnds)));

      //DXF_INSERT_ROT_ANG(CB->e) = -angle;
      //  }

      pDrw->EntityInvalidate(CB->e);
      pWnd->Invalidate(true);


      int xxx=0;
      }
    pDsp->Draw(CB->e, -1);
    }
  pDsp->Close();
  return (CB->e!=NULL);
  }

//---------------------------------------------------------------------------

bool GrfCmdBlk::DoInsertNode(CInsertBlk* CB)
  {
  bool OK = false;
  pDsp->Open();

  CheckTheTag(CB->ATag, CB->AClass(), TagBase.Length() == 0 ? CB->AClass() : TagBase(), true);

  CB->MdlInsertErr = AddUnitModel(CB->AClass(), CB->ATag(), CB->m_sGuid());
  if (CB->MdlInsertErr==0)
    {
    if (DoInsertNodeGrf(CB))
      {
      OK = true;
      //pDsp->Draw(CB->e, -1);
      }
    }

  pDsp->Close();
  return OK;
  }

//---------------------------------------------------------------------------

bool GrfCmdBlk::DoInsertLinkGrf(CConnectBlk* CB, bool SkipTagTest)
  {
  if (!SkipTagTest && gs_pPrj->FlwLib()->FE_TestModelTagUnique(CB->ATag()))
    {
    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model for Tag '%s' should exist!", CB->ATag());
    return false;
    }
  pDsp->Open();
  CB->e=NULL;
  bool OK=true;
  if (CB->NVerts<0)
    {//create vertices based on line draw method...
    bool SrcOk = false;
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    char *a1[] = { TagAttribStr,    (pchar)NULL };
    char *b1[] = { CB->SrcTag(), (pchar)NULL };
    pDrw->SelectInsertsOnAttrCombo(NULL, a1, b1, NULL);
    CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
    if (p && p->EntityPtr())
      {
      DXF_ENTITY e = p->EntityPtr();
      Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
      pDsp->SetCPtWorld(Pos, Pt1);
      SrcOk = true;
      }

    bool DstOk = false;
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    char *a2[] = { TagAttribStr,    (pchar)NULL };
    char *b2[] = { CB->DstTag(), (pchar)NULL };
    pDrw->SelectInsertsOnAttrCombo(NULL, a2, b2, NULL);
    p = pDsp->Vp1->FirstSelectedEntity();
    if (p && p->EntityPtr())
      {
      DXF_ENTITY e = p->EntityPtr();
      Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
      pDsp->SetCPtWorld(Pos, Pt2);
      DstOk = true;
      }
    OK = (SrcOk && DstOk);
    if (OK)
      OldCreateDefaultLinkLine(CB, pDsp, &Pt1, &Pt2, CB->iLineMethod==0);
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    }
  if (OK)
    {
    Tag_Attr_Set.Flags=HideTag ? DXF_ATTRIB_INVIS : 0;
    CB->e = AddLink(NULL, (LPSTR)(LPCSTR)TaggedObject::CreateGuidStr(), CB->ATag(), CB->SrcTag(), CB->SrcIO(), CB->DstTag(), CB->DstIO(), ArrowScale, CB->Verts, CB->NVerts, Tag_Attr_Set);
    pDsp->Draw(CB->e, GR_WHITE);
    pWnd->Invalidate();
    }
  pDsp->Close();
  return (CB->e!=NULL);
  }

bool GrfCmdBlk::DoInsertLinkGrf(CLineDrawHelper & LDH, char* SrcTag, char* DstTag, bool SkipTagTest)
  {
  if (!SkipTagTest && gs_pPrj->FlwLib()->FE_TestModelTagUnique((LPTSTR)LDH.Tag()))
    {
    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model for Tag '%s' should exist!", LDH.Tag());
    return false;
    }
  pDsp->Open();
  bool OK=true;
  if (LDH.NVerts()==0 && SrcTag && DstTag)
    {//create vertices based on line draw method...
    bool SrcOk = false;
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    char *a1[] = { TagAttribStr,    (pchar)NULL };
    char *b1[] = { SrcTag, (pchar)NULL };
    pDrw->SelectInsertsOnAttrCombo(NULL, a1, b1, NULL);
    CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
    if (p && p->EntityPtr())
      {
      DXF_ENTITY e = p->EntityPtr();
      Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
      pDsp->SetCPtWorld(Pos, Pt1);
      SrcOk = true;
      }

    bool DstOk = false;
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
    char *a2[] = { TagAttribStr,    (pchar)NULL };
    char *b2[] = { DstTag, (pchar)NULL };
    pDrw->SelectInsertsOnAttrCombo(NULL, a2, b2, NULL);
    p = pDsp->Vp1->FirstSelectedEntity();
    if (p && p->EntityPtr())
      {
      DXF_ENTITY e = p->EntityPtr();
      Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
      pDsp->SetCPtWorld(Pos, Pt2);
      DstOk = true;
      }
    OK = (SrcOk && DstOk);
    if (OK)
      CreateDefaultLinkLine(LDH, pDsp, &Pt1, &Pt2, false);//LDH.iLineMethod==0);
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    }

  DXF_ENTITY e = NULL;
  if (OK)
    OK = LDH.ValidGraphics();
  if (OK)
    {
    e = AddLinkDrawing(LDH);
    pDsp->Draw(e, GR_WHITE);
    pWnd->Invalidate();
    }
  pDsp->Close();
  return (e!=NULL);
  }

//---------------------------------------------------------------------------

bool GrfCmdBlk::DoInsertLink(CConnectBlk* CB)
  {
  bool OK = false;
  pDsp->Open();

  CheckTheTag(CB->ATag, CB->AClass(), TagBase.Length() == 0 ? CB->AClass() : TagBase(), true);

  int err = gs_pPrj->AddNodeConnect(CB->AClass(), CB->ATag(), CB->SrcTag(), CB->SrcIO(), CB->DstTag(), CB->DstIO());
  if (err)
    {
    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Connection not Made:\n%s:%s\n%s.%s->%s.%s", CB->AClass(), CB->ATag(), CB->SrcTag(), CB->SrcIO(), CB->DstTag(), CB->DstIO());
    }
  else
    {
    DoInsertLinkGrf(CB);
    }
  pDsp->Close();
  return OK;
  }

//---------------------------------------------------------------------------

bool GrfCmdBlk::DoChangeNodeGuid(pchar Tag, pchar Guid)
  {
  bool OK = gs_pPrj->ChangeNodeGuid(Tag, Guid)!=0;
  return OK;
  };

//---------------------------------------------------------------------------

bool GrfCmdBlk::DoInsertGroup(CInsertBlk* CB)
  {
  bool OK = false;
#if WITHGRFGROUP
//  pDsp->Open();

  if (CB->m_sGroupName.GetLength()>0)
    {
    CNeutralImportExport NImport;
    NImport.SetImportOrigin(CDblTriPt(CB->Pt.World.X, CB->Pt.World.Y, CB->Pt.World.Z), CDblTriPt(CB->GrpScl.X, CB->GrpScl.Y, CB->GrpScl.Z));
    NImport.SetImportFilter(CB->m_sGroupName());
    NImport.SetImportTagFixups(CB->m_iFixupRule, CB->m_sString1(), CB->m_sString2(), CB->m_pOldTags, CB->m_pNewTags);
    OK=NImport.DoImportGroup(CB->m_NDBOptions, dynamic_cast<CGrfDoc*>(pDoc), CB->m_sGroupLibrary(), CB->m_sGroupLibrary());
    }

//  pDsp->Close();
#endif
  return OK;
  }

//---------------------------------------------------------------------------

flag GrfCmdBlk::GetConnsForUnit(char *Tag, flag Inputs, dword RqdClass, Strng &IOTag, CSArray &IONames, CSArray &IODescs, CIOClassArray &IOClass, CSArray &IODefaultTSs, Pt_3f *pDigOff=NULL, Pt_3f *pDrwScl=NULL)
  {
  CWordArray IdCnt;
  CWordArray IdDone;
  CWordArray GrpCnt;
  CDWordArray Flags;
  IdCnt.SetSize(8);
  RequestModelIOConnRec ConnInfo;
  int j = 0;
  while (gs_pPrj->RequestModelIOConn(Tag, j++, ConnInfo))
    {
    IdCnt.SetSize(Max(IdCnt.GetSize(), (int)ConnInfo.iId+1));
    GrpCnt.SetSize(Max(GrpCnt.GetSize(), ConnInfo.iGrp+1));
    if (!ConnInfo.fAutoConnect)
      {
      IdCnt[ConnInfo.iId]++;
      GrpCnt[ConnInfo.iGrp]+=1;
      }
    }

      // Is dig in the target area
  int iIODefault=-1;
  bool UseDigPt=false;
  Pt_3f DigPt;
  if (pDigOff && pDrwScl)
    {
    UseDigPt=true;
    DigPt = (*pDigOff)*(*pDrwScl);
    }

  Flags.SetSize(0);
  IONames.SetSize(0);
  IODescs.SetSize(0);
  IOClass.SetSize(0);
  IODefaultTSs.SetSize(0);
  CRequestModelIOInfoRec IOInfo;
  int Cnt;
  for (int nIO=0; gs_pPrj->RequestModelIOInfoByIndex(Tag, nIO, IOInfo); nIO++)
    if (IOInfo.iId>=IdDone.GetSize() || !IdDone[IOInfo.iId])
      {
      int Avail = (IOInfo.Input && Inputs || IOInfo.Output && !Inputs) && !IOInfo.m_Hidden;
      if (Avail && (RqdClass & IOInfo.m_dwType))
        {
        IdDone.SetAtGrow(IOInfo.iId, 1);
        Cnt=IOInfo.iId<IdCnt.GetSize() ? IdCnt[IOInfo.iId] : 0;
        IONames.AddStrng(IOInfo.Name());
        IOClass.Add(IOInfo.m_dwType);
        IODefaultTSs.AddStrng(IOInfo.m_sDefaultTS()?IOInfo.m_sDefaultTS():"");
        Strng Desc;
        Flags.SetAtGrow(IONames.GetSize()-1, MF_STRING | (Avail ? 0 : MF_GRAYED) | (Cnt<IOInfo.nIOMax ? 0 : MF_GRAYED));

        Strng Name(IOInfo.Desc() ? IOInfo.Desc() : IOInfo.Name());

        if (Cnt<IOInfo.nIORqd && IOInfo.iGrp<GrpCnt.GetSize() && GrpCnt[IOInfo.iGrp]>0)
          Desc.Set("%s  [%d Reqd]", Name(), IOInfo.nIORqd-Cnt);
        else// if (Cnt<IOInfo.nIOMax)
          Desc.Set("%s  [%d] ", Name(), IOInfo.nIOMax-Cnt);
        if (IOInfo.iGrp>0)
          {
          Strng Grp;
          Grp.Set("  Grp:%d", IOInfo.iGrp);
          Desc+=Grp();
          }
        IODescs.AddStrng(Desc());
        
        if (UseDigPt)  
          {
          if (DigPt.X>IOInfo.m_GrfTarget.m_Left   && DigPt.X<IOInfo.m_GrfTarget.m_Right && 
              DigPt.Y>IOInfo.m_GrfTarget.m_Bottom && DigPt.Y<IOInfo.m_GrfTarget.m_Top)
            {
            iIODefault=IONames.GetSize()-1;
            };
          }
        
        }
      }

  if (IODescs.GetSize()==0)
    return False;

  if (Inputs)
    iDstIOIndex=-1;
  else
    iSrcIOIndex=-1;

  int RetCd=0;
  //while (1) // ?? CNM Must work out out to force a selection ? Maybe a small dialog
    {

    if (IODescs.GetSize()==1 && ((Flags[0]&MF_GRAYED)==0))
      {
      RetCd=Inputs ? IDM_DSTIO_0 : IDM_SRCIO_0;
      }
    else
      {
      CMenu Menu;
      Menu.CreatePopupMenu();

      if (iIODefault>=0)
        {
        Menu.AppendMenu(Flags[iIODefault], (Inputs ? IDM_DSTIO_0 : IDM_SRCIO_0)+iIODefault,   IODescs[iIODefault]());
        Menu.AppendMenu(MF_SEPARATOR, -1);
        }

      for (int nIO=0; nIO<IODescs.GetSize(); nIO++)
        Menu.AppendMenu(Flags[nIO], (Inputs ? IDM_DSTIO_0 : IDM_SRCIO_0)+nIO,   IODescs[nIO]());

      POINT ScreenPoint = {(int)pDsp->CurrentPt.Screen.X, (int)pDsp->CurrentPt.Screen.Y};// = point;
      pDsp->TheWnd->ClientToScreen(&ScreenPoint);

      CRect ClickRect(0,0,2048,2048);
      RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
          ScreenPoint.x, ScreenPoint.y, pDsp->TheWnd, &ClickRect);
      Menu.DestroyMenu();
      }

    if (RetCd && RetCd!=IDM_MANUALIO)
      {
      if (Inputs)
        iDstIOIndex=RetCd-IDM_DSTIO_0;
      else
        iSrcIOIndex=RetCd-IDM_SRCIO_0;
      return True;
      }
    }

  return False;
  }

//---------------------------------------------------------------------------

CEntInView* GrfCmdBlk::GetClosest(Pt_SLW CurrentPt, flag FindNode, flag FindLink, flag FindDirectLink, Strng& Tag, Strng& GrfGuid, Strng& MdlGuid)
  {
  const int MaxCnt = 8;
  CEntInView* EntList[MaxCnt];
  double DistList[MaxCnt];
  int CntFound = pDsp->Vp1->SelectClosestEntities(CurrentPt, EntList, DistList, MaxCnt);
  int Cnt=0;
  while (Cnt<CntFound)
    {
    CEntInView* p = EntList[Cnt];
    if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
      {
      DXF_ENTITY e = p->EntityPtr();
      Tag = Find_Attr_Value(e, TagAttribStr);
      GrfGuid = Find_Attr_Value(e, GuidAttribStr);
      if (Tag() && gs_pPrj->GetNodeGuid(Tag(), MdlGuid))
        {
        int RetCode = CheckEntity(Tag());
        const flag IsNode = (RetCode==1);
        const flag IsLink = (RetCode==2);
        const flag IsDirect = (RetCode==3);
        if (RetCode!=0)
          {
          if (FindNode && IsNode)
            return p;
          if (FindLink && IsLink)
            return p;
          if (FindDirectLink && IsDirect)
          return p;
        }
      }
      }
    Cnt++;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

CEntInView* GrfCmdBlk::GetClosest(Pt_SLW CurrentPt, BOOL& AlreadySelected, flag FindInsert)
  {
  AlreadySelected = false;
  const int MaxCnt = 8;
  CEntInView* EntList[MaxCnt];
  double DistList[MaxCnt];
  int CntFound = pDsp->Vp1->SelectClosestEntities(CurrentPt, EntList, DistList, MaxCnt);
  int Cnt=0;
  while (Cnt<CntFound)
    {
    CEntInView* p = EntList[Cnt];
    if (p)
      {
      BOOL IsInsert = (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()));
      if ((FindInsert && IsInsert) || (!FindInsert && !IsInsert))
        {
        if (p->IfMarkBit())
          AlreadySelected = true;
        return p;
        }
      }
    Cnt++;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

CEntInView* GrfCmdBlk::GetClosest(Pt_SLW CurrentPt, flag FindInsert)
  {
  const int MaxCnt = 8;
  CEntInView* EntList[MaxCnt];
  double DistList[MaxCnt];
  int CntFound = pDsp->Vp1->SelectClosestEntities(CurrentPt, EntList, DistList, MaxCnt);
  int Cnt=0;
  while (Cnt<CntFound)
    {
    CEntInView* p = EntList[Cnt];
    if (p)
      {
      BOOL IsInsert = (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()));
      if ((FindInsert && IsInsert) || (!FindInsert && !IsInsert))
        return p;
      }
    Cnt++;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

CEntInView* GrfCmdBlk::GetClosestText(Pt_SLW CurrentPt)
  {
  const int MaxCnt = 8;
  CEntInView* EntList[MaxCnt];
  double DistList[MaxCnt];
  int CntFound = pDsp->Vp1->SelectClosestEntities(CurrentPt, EntList, DistList, MaxCnt);
  int Cnt=0;
  while (Cnt<CntFound)
    {
    CEntInView* p = EntList[Cnt];
    if (p)
      {
      if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
        return p;
      }
    Cnt++;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

CEntInView* GrfCmdBlk::GetClosestEntity(Pt_SLW CurrentPt, DXF_ENTITY pEntityToFind)
  {
  const int MaxCnt = 8;
  CEntInView* EntList[MaxCnt];
  double DistList[MaxCnt];
  int CntFound = pDsp->Vp1->SelectClosestEntities(CurrentPt, EntList, DistList, MaxCnt);
  int Cnt=0;
  while (Cnt<CntFound)
    {
    CEntInView* p = EntList[Cnt];
    if (p)
      {
      if (p->EntityPtr()==pEntityToFind)
        return p;
      }
    Cnt++;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoConnect()
  {
  static CInsertLinkDlg* pLnkDlg = NULL;
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  CConnectBlk* CB = ((CConnectBlk*)gs_pCmd->GetDataBlk());

  //SETVNT("INsert",    "LInk",  "@ Tag BASetag Ortho Free BRk BACk SNd SIo DNd DIo MOdel MEthod * ", 1, (CmdFn)&GrfCmdBlk::DoConnect, EF_Edit);
  enum MId { MID_Dig=1, MID_Tag, MID_BaseTag, MID_Ortho, MID_Free, MID_Brk, MID_Back,
             MID_SNd, MID_SIo, MID_DNd, MID_DIo, MID_MOdel, MID_MEthod, MID_Dlg};

  bool DoUndo = false;
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            gs_pCmd->SetDlgBusy();
            TheLDH.Init(pDsp, pDrw);
            CB = new CConnectBlk;
            gs_pCmd->SetDataBlk((void*)CB);
            HideTagOff = false;
            i1 = 0; //count of failures to select unit
            bFlag1 = 0; //has a dig point been drawn
            bFlag2 = 0; //next dig point is a break
            gs_pCmd->ExtendCmdLine("SNd ");
            iSrcIOIndex=-1;
            iDstIOIndex=-1;
            }
          break;
        case MID_Dig     : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Tag     : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_BaseTag : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_Ortho   : CInsertLinkDlg::SetDrawOrth(1); TheLDH.SetLineMethod(LDM_Orth); break;
        case MID_Free    : CInsertLinkDlg::SetDrawOrth(0); TheLDH.SetLineMethod(LDM_Direct); break;
        case MID_Brk     : bFlag2 = 1; break;
        case MID_Back    : DoUndo = true; break;
        case MID_SNd     : gs_pCmd->SetDigInfo(GC_NoCurs, 1); gs_pCmd->SetParmInfo(AName, 1); pGWnd->SetCursor(IDC_SOURCECURS); break;
        case MID_SIo     : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_DNd     : gs_pCmd->SetDigInfo(GC_BigCurs, 1); gs_pCmd->SetParmInfo(AName, 1); pGWnd->SetCursor(IDC_DESTCURS); break;
        case MID_DIo     : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_MOdel   : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_MEthod  :
          if (pLnkDlg)
            {
            TheLDH.HideLine();
            bFlag1 = 0;
            bFlag2 = 0;
            switch (CInsertLinkDlg::DrawMethod())
              {
              case 0: TheLDH.InitPoints(); break;
              case 1: CreateDefaultLinkLine(TheLDH, pDsp, &Pt1, &Pt2, pLnkDlg->DrawSingleSeg()); break;
              case 2: CreateDefaultLinkLine(TheLDH, pDsp, &Pt3, &Pt4, pLnkDlg->DrawSingleSeg()); break;
              }
            TheLDH.ShowLine();
            pLnkDlg->SetPoints(TheLDH.NVerts());
            pLnkDlg->SetFocus();
            }
          break;
        case MID_Dlg     :
          {
          if (SrcTag.Length()==0)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No Source Tag");
          else if (DstTag.Length()==0)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No Destination Tag");
          else
            {
            LPCSTR pCls="";
            if (iSrcIOIndex>=0 && iSrcIOIndex<SrcIONames.GetSize())
              {
              if (SrcIOClass[iSrcIOIndex]&nc_MLnk)
                pCls=FlwLinkGrp;
              else if (SrcIOClass[iSrcIOIndex]&nc_CLnk)
                pCls=CtrlLinkGrp;
              else if (SrcIOClass[iSrcIOIndex]&nc_ELnk)
                pCls=ElecLinkGrp;
              else if (SrcIOClass[iSrcIOIndex]&nc_ALnk)
                pCls=AirLinkGrp;
              }
            if (iSrcIOIndex>=0)
              SrcTS = SrcTermStrips[iSrcIOIndex];
            if (iDstIOIndex>=0)
              DstTS = DstTermStrips[iDstIOIndex];
            if (iSrcIOIndex>=0 && iSrcIOIndex<SrcIONames.GetSize() &&
                iDstIOIndex>=0 && iDstIOIndex<DstIONames.GetSize())
              {
              SrcIO = SrcIONames[iSrcIOIndex];
              DstIO = DstIONames[iDstIOIndex];
              // Get SrcIOTag & DstIOTag - Also Pass IO lists to dlg
              flag SlctIOTags = (SrcIO.Length()==0 || DstIO.Length()==0);
              pLnkDlg = new CInsertLinkDlg(pCls, pMdl, SrcTS(), DstTS(), AfxGetMainWnd());
              bFlag1 = 0;
              bFlag2 = 0;
              switch (CInsertLinkDlg::DrawMethod())
                {
                case 0: TheLDH.InitPoints(); break;
                case 1: CreateDefaultLinkLine(TheLDH, pDsp, &Pt1, &Pt2, pLnkDlg->DrawSingleSeg()); break;
                case 2: CreateDefaultLinkLine(TheLDH, pDsp, &Pt3, &Pt4, pLnkDlg->DrawSingleSeg()); break;
                }
              TheLDH.ShowLine();
              pLnkDlg->SetPoints(TheLDH.NVerts());
              }
            else
              pLnkDlg = NULL;
            }

          if (pLnkDlg==NULL || pLnkDlg->m_hWnd==NULL)
            {
            if (pLnkDlg && pLnkDlg->NoOfModels()==0)
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No 'link' model types available");
            else
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Incomplete connect information");
            gs_pCmd->ExtendCmdLine("\x1b\x1b\x1b\x1b");
            }
          break;
          }
        default :;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Tag     : ATag = gs_pCmd->LastToken(); break;
        case MID_BaseTag : TagBase = gs_pCmd->LastToken(); break;
        case MID_SIo :
          {
          SrcIO = gs_pCmd->LastToken();
          CRequestModelIOInfoRec Info;
          flag GotSrcOut=0;
          for (int nIns=0; gs_pPrj->RequestModelIOInfoByIndex(SrcTag(), nIns, Info); nIns++)
            if (!Info.Input && Info.Name.XStrICmp(SrcIO())==0)
              GotSrcOut=1;
          if (!GotSrcOut)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Src IO %s Not Found", SrcIO());
          break;
          }
        case MID_DIo :
          {
          DstIO = gs_pCmd->LastToken();
          CRequestModelIOInfoRec Info;
          flag GotDstOut=0;
          for (int nIns=0; gs_pPrj->RequestModelIOInfoByIndex(DstTag(), nIns, Info); nIns++)
            if (Info.Input && Info.Name.XStrICmp(DstIO())==0)
              GotDstOut=1;
          if (!GotDstOut)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Dst IO %s Not Found", DstIO());
          break;
          }
        case MID_MOdel : AClass = gs_pCmd->LastToken(); break;
        default :;
        }
      break;
    case EX_MOUSE_LUP :
      {
      flag AddAVert = 0;
      switch (Modifier)
        {
        case MID_SNd :
        case MID_DNd :
          {
          CWaitCursor Wait;
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          Strng TmpTag;
          Strng GrfGuid, MdlGuid;
          CEntInView* p = GetClosest(pDsp->CurrentPt, true, false, false, TmpTag, GrfGuid, MdlGuid);
          if (p)
            {
            pDsp->Vp1->SelectEntity(p);
            DXF_ENTITY e = p->EntityPtr();
            Pos.Set(DXF_INSERT_PT(e)[0], DXF_INSERT_PT(e)[1], DXF_INSERT_PT(e)[2]);
            Pt_3f Scl, Off;
            Off=pDsp->CurrentPt.World-Pos;
            Scl.Set(DXF_INSERT_X_SCALE(e), DXF_INSERT_Y_SCALE(e), DXF_INSERT_Z_SCALE(e));
            Strng TagMod;
            pGWnd->SetCursor();
            if (Modifier==MID_SNd)
              {
              SrcTag = TmpTag;
              SrcGrfGuid = GrfGuid;
              SrcMdlGuid = MdlGuid;
              SrcIO = "O";
              GetConnsForUnit(SrcTag(), False, nc_MLnk|nc_CLnk|nc_ELnk|nc_ALnk, SrcIO, SrcIONames, SrcIODescs, SrcIOClass, SrcTermStrips, &Off, &Scl);
              if (SrcIONames.GetCount())
                {
                TagMod.Set(",%s.%s ", SrcTag(), SrcIO());
                gs_pCmd->ExtendModifier(TagMod());
                gs_pCmd->ExtendCmdLine("DNd ");
                Pt1 = pDsp->CurrentPt;
                pDsp->SetCPtWorld(Pos, Pt3);
                if (CInsertLinkDlg::DrawMethod()==1)
                  pDsp->Show_Dig_Point(Pt1);
                else if (CInsertLinkDlg::DrawMethod()==2)
                  pDsp->Show_Dig_Point(Pt3);
                }
              else
                {
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s does not have a valid input connection IO", SrcTag());
                gs_pCmd->ExtendCmdLine("\x1b\x1b");
                }
              }
            else
              {
              DstTag = TmpTag;
              DstGrfGuid = GrfGuid;
              DstMdlGuid = MdlGuid;
              DstIO="I";
              GetConnsForUnit(DstTag(), True, iSrcIOIndex>=0?SrcIOClass[iSrcIOIndex]:0, DstIO, DstIONames, DstIODescs, DstIOClass, DstTermStrips, &Off, &Scl);
              if (DstIONames.GetCount())
                {
                TagMod.Set(",%s.%s ", DstTag(), DstIO());
                gs_pCmd->ExtendModifier(TagMod());
                gs_pCmd->ExtendCmdLine("*");
                pGWnd->SetCursor(IDC_POINTPOS);
                Pt2 = pDsp->CurrentPt;
                pDsp->SetCPtWorld(Pos, Pt4);
                if (CInsertLinkDlg::DrawMethod()==1)
                  pDsp->Show_Dig_Point(Pt2);
                else if (CInsertLinkDlg::DrawMethod()==2)
                  pDsp->Show_Dig_Point(Pt4);
                }
              else
                {
                LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s does not have a valid input connection IO", DstTag());
                gs_pCmd->ExtendCmdLine("\x1b\x1b");
                }
              }
            }
          else
            {
            i1++;
            if (i1<3)
              {
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Need to select a valid '%s unit', try again (attempt %d of 3).", (Modifier==MID_SNd) ? "source" : "destination", i1+1);
              gs_pCmd->ExtendModifier(",x ");
              gs_pCmd->ExtendCmdLine(Modifier==MID_SNd ? "SNd " : "DNd "); //try again
              }
            else
              {//give up and exit...
              LogError("GrfCmds", LF_DoAfxMsgBox, "Command canceled!  Need to select a valid '%s unit'.", (Modifier==MID_SNd) ? "source" : "destination");
              gs_pCmd->ExtendCmdLine("\x1b\x1b");
              }
            }
          }
          break;
        case MID_SIo :
          pDsp->Show_Dig_Point(pDsp->CurrentPt);
          SrcIO = ".O";
          AddAVert = 1;
          gs_pCmd->ExtendModifier(",");
          gs_pCmd->ExtendModifier(SrcIO());
          break;
        case MID_DIo :
          DstIO = "";
          AddAVert = 1;
          gs_pCmd->ExtendModifier(",");
          gs_pCmd->ExtendModifier(DstIO());
          break;
        default :;
        }
      if (Modifier == MID_Dig || AddAVert)
        {
        if (!bFlag1)
          {
          TheLDH.HideLine();
          TheLDH.InitPoints();
          }
        bFlag1 = 1;
        if (gs_pCmd->CtrlDown())
          bFlag2 = 1;
        TheLDH.SetLineMethod(CInsertLinkDlg::DrawOrth() ? LDM_Orth : LDM_Direct);
        TheLDH.AddPoint(bFlag2 ? LPT_Break : LPT_Line);
        bFlag2 = 0;
        if (pLnkDlg)
          pLnkDlg->SetPoints(TheLDH.NVerts());
        }
      if (pLnkDlg)
        pLnkDlg->SetFocus();
      break;
      }
    case EX_EXEC :
      break;
    case EX_DOIT :
      {
      CWaitCursor Wait;
      if (TheLDH.ValidGraphics())
        {
        if (pLnkDlg)
          {
          pLnkDlg->CompleteForUse();
          TheLDH.SetArrowRule((ArrowRuleMethod)(pLnkDlg->ArrowRuleMethod()));
          TheLDH.SetLineWidth(pLnkDlg->LineWidth());
          ATag = pLnkDlg->m_Tag();
          AClass = pLnkDlg->m_ModelName();
          TagBase = pLnkDlg->m_BaseTag();
          HideTag = pLnkDlg->m_HideTag;
          ArrowScale = pLnkDlg->m_ArrowScale;
          m_bAutoWire = pLnkDlg->m_AutoWire;
          TaggedObject::ValidateTag(AClass);
          TaggedObject::ValidateTag(SrcTag);
          TaggedObject::ValidateTag(DstTag);
          TaggedObject::ValidateTag(SrcIO);
          TaggedObject::ValidateTag(DstIO);
          }

        CheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), true);

        if (SrcIO.Length() == 0)
          {
          pchar p = SrcTag.XStrRChr('.');
          if (p)
            {
            SrcIO = p + 1;
            SrcTag.SetLength(SrcTag.Length() - strlen(p));
            }
          }
        if (DstIO.Length() == 0)
          {
          pchar p = DstTag.XStrRChr('.');
          if (p)
            {
            DstIO = p + 1;
            DstTag.SetLength(DstTag.Length() - strlen(p));
            }
          }

        if (SrcTag.Length() > 0 && SrcIO.Length() > 0 && DstTag.Length() > 0 && DstIO.Length() > 0)
          {
#if SYSCAD10         
          if (gs_pPrj->SvcActive)
            {
            SCD10ENTER;
            CPointFList ControlPoints;
            for (int i=0; i<TheLDH.NVerts(); i++)
              {
              float PtX = int(TheLDH.VertWorld(i).X) + 0.5f; // needs to be x.5mm to meet grid in 10.
              float PtY = int(TheLDH.VertWorld(i).Y) + 0.5f; // needs to be x.5mm to meet grid in 10.
              ControlPoints.AddTail(CPointF(PtX, PtY));
              }

  //CString ModelGuid  = pNode->Guid();
  //CString SrcMdlGuid = pNode->Nd_Rmt(0)->Guid();
  //CString SrcGrfGuid = "Grf GUID Fetch Failed";
  //CString SrcPort    = pNode->IODesc_Rmt(0)->IOName();
  //CString DstMdlGuid = pNode->Nd_Rmt(1)->Guid();
  //CString DstGrfGuid = "Grf GUID Fetch Failed";
  //CString DstPort    = pNode->IODesc_Rmt(1)->IOName();

  //DO_ENTRY_GGTPSM("DoCreateLinkE", GraphicGuid, ModelGuid, I.m_sTag, MakePath(projectPath, Grp.m_sTitle),"","");

  //CExistingItems::CGroupIndex SInx;
  //if (GI.m_TagMap.Lookup(pNode->Nd_Rmt(0)->Tag(), SInx))
  //  SrcGrfGuid = Grp.m_GTIA[SInx.m_iGTIA].m_sGuid();
  //else
  //  LogError("Upgrade2Scd10", 0, "Tag Not in Graphics %s", pNode->Nd_Rmt(0)->Tag());

  //CExistingItems::CGroupIndex DInx;
  //if (GI.m_TagMap.Lookup(pNode->Nd_Rmt(1)->Tag(), DInx))
  //  DstGrfGuid = Grp.m_GTIA[DInx.m_iGTIA].m_sGuid();
  //else
  //  LogError("Upgrade2Scd10", 0, "Tag Not in Graphics %s", pNode->Nd_Rmt(1)->Tag());

            gs_pPrj->Svc.GCBCreateLink((CGrfDoc*)pDoc, PrjName(), pDoc->GetTitle(), ATag(), AClass(), 
              SrcGrfGuid(), DstGrfGuid(), SrcMdlGuid(), DstMdlGuid(), 
              SrcIO(), DstIO(), ControlPoints);
            SCD10LEAVE;
            }
          else
#endif 
            {
            if (AddLinkModel(AClass(), ATag(), SrcTag(), SrcIO(), DstTag(), DstIO()))
              {
              Tag_Attr_Set.Flags = HideTag ? DXF_ATTRIB_INVIS : 0;
              TheLDH.SetShowTag(Tag_Attr_Set.Flags==0);
              TheLDH.SetArrowScale(ArrowScale);
              TheLDH.SetTag(ATag());
              TheLDH.SetTagPt(0.0, 1.5, 0.0); //place the tag slightly above the insert point
              DXF_ENTITY e = AddLinkDrawing(TheLDH);
              pDsp->Draw(e, GR_WHITE);
              pWnd->Invalidate();
              }  
            }
          }
        else
          {
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No Connection");
          }
        }
      else
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No vertices for link");
      delete CB;
      CB = NULL;
      gs_pCmd->SetDataBlk(NULL);

      if (SrcIOClass[iSrcIOIndex]&nc_ELnk)
        {
        if (m_bAutoWire && SrcTS() && DstTS())
          {
          CLinkWiring LW;
          LW.m_LinkTag=ATag();
          LW.m_bAutoWire=true;
          LW.m_SrcTS=SrcTS();
          LW.m_DstTS=DstTS();
          if (gs_pPrj->SetLinkWiring(LW)<0)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "AutoWire failed");
          }
        else
          CELnkWiring::Start(ATag());
        }
      break;
      }
    case EX_UNDO :
      DoUndo = true;
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      if (CB)
        delete CB;
      gs_pCmd->SetDataBlk(NULL);
      pGWnd->SetCursor();
      if (pLnkDlg)
        {
        pLnkDlg->DestroyWindow();
        delete pLnkDlg;
        pLnkDlg = NULL;
        }
      TheLDH.Init(NULL, NULL);
      gs_pCmd->SetDlgBusy(0);
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }
  if (DoUndo)
    {
    TheLDH.RemovePoint();
    if (pLnkDlg)
      pLnkDlg->SetPoints(TheLDH.NVerts());
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

double TryGetArrowScale(DXF_Drawing* pDrw, DXF_ENTITY entity)
  {
  Block* b = pDrw->Blocks.Find(DXF_INSERT_BLOCK_GET(entity));
  if (b)
    {
    double Scale;
    CGrfLayer* l;
    WALK_LAYERS(b->Layers, NULL, l)
      {
      DML_ITEM it;
      C3_CURVE c;
      bool FoundArrow = false;
      WALK_CURVE_LIST(l->CurveLst, it, c)
        {
        if (IsArrow(c, Scale))
          return Scale;
        }
      }
    }
  return -1.0;
  }

void GrfCmdBlk::DoMoveLink()
  {
  static CInsertLinkDlg* pLnkDlg = NULL;
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  CConnectBlk* CB = ((CConnectBlk*)gs_pCmd->GetDataBlk());

  //SETVNT("MOve",      "LInk",  "@ Lnk Ortho Free BRk BAck", 1, (CmdFn)&GrfCmdBlk::DoMoveLink, EF_Edit);
  enum MId { MID_Dig=1, MID_Lnk, MID_Ortho, MID_Free, MID_Brk, MID_Back };

  bool DoCleanup = false;
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          OldEntity = NULL;
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            TheLDH.Init(pDsp, pDrw);
            CB = new CConnectBlk;
            gs_pCmd->SetDataBlk((void*)CB);
            bFlag1 = 0; //hide tag
            bFlag2 = 0; //next dig point is a break
            gs_pCmd->ExtendCmdLine("Lnk ");
            }
          break;
        case MID_Dig     : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Ortho   : CInsertLinkDlg::SetDrawOrth(1); TheLDH.SetLineMethod(LDM_Orth); break;
        case MID_Free    : CInsertLinkDlg::SetDrawOrth(0); TheLDH.SetLineMethod(LDM_Direct); break;
        case MID_Brk     : bFlag2 = 1; break;
        case MID_Back    : TheLDH.RemovePoint(); break;
        case MID_Lnk     : gs_pCmd->SetDigInfo(GC_NoCurs, 1); gs_pCmd->SetParmInfo(AName, 1); pGWnd->SetCursor(IDC_SELECTLINK); break;
        default :;
        }
      break;
    case EX_PARAMETER_RDY :
      break;
    case EX_MOUSE_LUP :
      {
      if (Modifier==MID_Lnk)
        {
        CWaitCursor Wait;
        pDsp->Vp1->ClearAllEntity();
        pDsp->Vp1->ClrSelectionAllList();
        Strng GrfGuid, MdlGuid;
        CEntInView* p = GetClosest(pDsp->CurrentPt, false , true, true, LnkTag, GrfGuid, MdlGuid);
        if (p && p->EntityPtr())
          {
          ActivateGWnd();
          pDsp->Vp1->SelectEntity(p);
          OldEntity = p->EntityPtr();
          const double ArrowScl = TryGetArrowScale(pDrw, OldEntity);
          if (ArrowScl<0.0)
            {
            CProfINIFile PF(PrjIniFile());
            ArrowScale = PF.RdDouble("InsertLink", "$ArrowScale", 2.5);
            }
          else
            ArrowScale = ArrowScl;
          bFlag1 = ((Attr_AFlags(Find_Attr(OldEntity, TagAttribStr))&Attrib_InVisible)!=0);
          Strng TagMod;
          pGWnd->SetCursor();
          TagMod.Set(",%s ", LnkTag());
          gs_pCmd->ExtendModifier(TagMod());
          gs_pCmd->ExtendCmdLine("*");
          pGWnd->SetCursor(IDC_POINTPOS);
          pLnkDlg = new CInsertLinkDlg(pMdl, LnkTag(), AfxGetMainWnd());
          pLnkDlg->SetData(ArrowScale, bFlag1);
          }
        else
          {
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Need to select a valid link, try again.");
          gs_pCmd->ExtendModifier(",x");
          gs_pCmd->ExtendCmdLine("Lnk "); //try again
          }
        }
      if (Modifier==MID_Dig)
        {
        if (gs_pCmd->CtrlDown())
          bFlag2 = 1;
        TheLDH.SetLineMethod(CInsertLinkDlg::DrawOrth() ? LDM_Orth : LDM_Direct);
        TheLDH.AddPoint(bFlag2 ? LPT_Break : LPT_Line);
        bFlag2 = 0;
        if (pLnkDlg)
          pLnkDlg->SetPoints(TheLDH.NVerts());
        }
      break;
      }
    case EX_EXEC :
      break;
    case EX_DOIT :
      {
      CWaitCursor Wait;
      if (TheLDH.ValidGraphics())
        {
        if (LnkTag.Length())
          {
#if SYSCAD10         
          if (gs_pPrj->SvcActive)
            {
            SCD10ENTER;
            CPointFList ControlPoints;
            for (int i=0; i<TheLDH.NVerts(); i++)
              {
              Pt_3f Pt = TheLDH.VertWorld(i);
              ControlPoints.AddTail(CPointF(Pt.X, Pt.Y));
              }

            CRectangleF TA(0.0, 0.0, 0.0, 0.0);
            gs_pPrj->Svc.GCBModifyLinkPts((CGrfDoc*)pDoc, PrjFile(), pDoc->GetTitle(), LnkTag(), /*AClass(), SrcTag(), DstTag(), SrcIO(), DstIO(),*/ ControlPoints, TA, 0.0, !bFlag1);
            SCD10LEAVE;
            }
          else
#endif
            {
          Strng AssocTag;

            if (OldEntity)
              {//delete previous...
              //char * pTag = Attr_Value(Find_Attr(OldEntity, TagAttribStr));
            AssocTag = Find_Attr_Value(OldEntity, AssocTagAttribStr);
              pDsp->Draw(OldEntity, GrfHelper.GR_BACKGROUND);
              pDrw->Delete(OldEntity);
              }

            //create & draw new graphics...
            if (pLnkDlg)
              {
              pLnkDlg->CompleteForUse();
              TheLDH.SetArrowRule((ArrowRuleMethod)(pLnkDlg->ArrowRuleMethod()));
              TheLDH.SetLineWidth(pLnkDlg->LineWidth());
              bFlag1 = pLnkDlg->m_HideTag;
              ArrowScale = pLnkDlg->m_ArrowScale;
              }
            Tag_Attr_Set.Flags = bFlag1 ? DXF_ATTRIB_INVIS : 0;
            TheLDH.SetShowTag(!bFlag1);
            TheLDH.SetArrowScale(ArrowScale);
            TheLDH.SetTag(LnkTag());
          TheLDH.SetAssocTag(AssocTag());
            DXF_ENTITY e = AddLinkDrawing(TheLDH);
            pDsp->Draw(e, GR_WHITE);
            pWnd->Invalidate();
            }
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Valid link must be selected");
        delete CB;
        gs_pCmd->SetDataBlk(NULL);
        }
      else
        {
        TheLDH.HideLine();
        //gs_pCmd->ExtendCmdLine("\x1b\x1b"); This doesn't work!!!
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No vertices for link");
        DoCleanup = true;
        }
      break;
      }
    case EX_UNDO :
      TheLDH.RemovePoint();
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      DoCleanup = true;
      break;
    default :;
    }
  if (DoCleanup)
    {
    if (CB)
      delete CB;
    gs_pCmd->SetDataBlk(NULL);
    pGWnd->SetCursor();
    LnkTag="";
    OldEntity=NULL;
    if (pLnkDlg)
      {
      pLnkDlg->DestroyWindow();
      delete pLnkDlg;
      pLnkDlg = NULL;
      }
    TheLDH.Init(NULL, NULL);
    gs_pCmd->SetDlgBusy(0);
    gs_pCmd->FreeDblClk();
    }
  pDsp->Close();
  }

//===========================================================================

static char OKTagChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static char C0TagChars[] = "/-_,:;?% ";
static char C1TagChars[] = "/-_._____";

static void MakeATag(char* ca, int len, char* pc)
  {
  char* c0=ca;
  for ( ; *pc!=0; pc++, *ca=0)
    if (strchr(OKTagChars, *pc))
      {
      *ca=*pc;
      ca++;
      }
    else if (ca!=c0)
      {
      char* p = strchr(C0TagChars, *pc);
      if (p)
        {
        *ca=C1TagChars[(int)(p-&C0TagChars[0])];
        ca++;
        }
      }
  }

//===========================================================================
// This command allows the user to dig random entities
// and from the entities will construct a Unit. The Unit will
// be given a name which is either entered or is text in the digitised list.
// the origin of the figure will be at the centre of the text if existing
// otherwise at the centre of gravity of the entities.

void GrfCmdBlk::DoConstructSymbol()
  {
  if (gs_Exec.Busy())
    return;
  static CInsertUnitDlg * sm_pCnsDlg = NULL;
  enum MID { MID_Dig=1, MID_IWin, MID_CWin, MID_Tag, MID_Eqp,
             MID_Symbol, MID_Model, MID_Dlg, MID_CTag};
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            pDsp->Vp1->ClearAllEntity();
            ATag = "";
            AUnitId = "";
            ASymbol  = "";
            AClass = "";
            nTxtEnt=0;
            pTxtEnt[0]=NULL;
            pTxtEnt[1]=NULL;
            pEnt1 = NULL;
            nEntSelects=0;
            bCTagPtSet=0;
            HideTagOff=False;
            gs_pCmd->ExtendCmdLine("CTag ");
            }
          break;
        case MID_Dig    : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTGRF); break;
        case MID_IWin   : gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1); pGWnd->SetCursor(IDC_SELECTBOXGRF); break;
        case MID_CWin   : gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1); pGWnd->SetCursor(IDC_SELECTBOXGRF); break;
        case MID_CTag   : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTTAG); break;
        case MID_Tag    : gs_pCmd->SetParmInfo(AName, 1); ATag = ""; break;
        case MID_Eqp    : gs_pCmd->SetParmInfo(AName, 1); AUnitId = ""; break;
        case MID_Symbol : gs_pCmd->SetParmInfo(AName, 1); ASymbol = ""; break;
        case MID_Model  : gs_pCmd->SetParmInfo(AName, 1); AClass = ""; break;
        //case MID_Dlg    : pMdlSlct = new CMdlSlct(pMdl, FlwUnitGrp, False, True, True, "", "", "", "", HideTag, HideEqpId, AfxGetMainWnd()); break;
        //case MID_Dlg    : sm_pCnsDlg = new CConstructDlg(pMdl, true, CDlgWhat_Unit, AfxGetMainWnd()); break;
        case MID_Dlg    : sm_pCnsDlg = new CInsertUnitDlg(CDlgWhat_ConUnit, true, pMdl, AfxGetMainWnd()); break;
        default :
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Tag    : ATag = gs_pCmd->LastToken(); break;
        case MID_Eqp    : AUnitId = gs_pCmd->LastToken(); break;
        case MID_Symbol : ASymbol = gs_pCmd->LastToken(); break;
        case MID_Model  : AClass = gs_pCmd->LastToken(); break;
        default : break;
        }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      switch (Modifier)
        {
        case MID_Dig :
        case MID_IWin :
        case MID_CWin :
          {
          CEntInView* p;
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==MID_Dig)
            {
            BOOL AlreadySelected;
            p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
            if (p && AlreadySelected)
              pDsp->Vp1->DeSelectEntity(p);
            }
          else if (Modifier==MID_IWin)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else if (Modifier==MID_CWin)
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          nEntSelects=0;
          flag FndPrev=false;
          CEntInView* pFndFst = NULL;
          p = pDsp->Vp1->FirstSelectedEntity();
          while (p)
            {
            if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && Find_Attr_Value(p->EntityPtr(), TagAttribStr))
              {
              if (pFndFst==NULL)
                pFndFst = p;
              if (pEnt1==p)
                FndPrev = true;
              pDsp->Vp1->DeSelectEntity(p);
              }
            else
              nEntSelects++;
            p = pDsp->Vp1->NextSelectedEntity();
            }
          if (!FndPrev)
            pEnt1 = pFndFst;
          if (pEnt1)
            {
            pDsp->Vp1->SelectEntity(pEnt1);
            nEntSelects++;
            if (sm_pCnsDlg)
              {
              char* pUnitTag = Find_Attr_Value(pEnt1->EntityPtr(), TagAttribStr);
              sm_pCnsDlg->SetTag(pUnitTag);
              }
            }
          pGWnd->SetCursor(IDC_SELECTGRF);
          if (sm_pCnsDlg)
            {
            if (Modifier==MID_IWin)
              sm_pCnsDlg->SetIWin(False);
            else if (Modifier==MID_CWin)
              sm_pCnsDlg->SetCWin(False);
            sm_pCnsDlg->SetEntityCount(nEntSelects);
            }
          break;
          }
        case MID_CTag:
          {
          pDsp->Vp1->ClrSelectionAllList();
          CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && Find_Attr_Value(p->EntityPtr(), TagAttribStr))
            {
            pEnt1 = p;
            //pDsp->Vp1->DeSelectEntity(p);
            //p = NULL;//try again
            }
          if (p && p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
            {
            bCTagPtSet=1;
            CTagPt.World.Set( DXF_TEXT_INS_PT(p->EntityPtr())[0],
                              DXF_TEXT_INS_PT(p->EntityPtr())[1],
                              DXF_TEXT_INS_PT(p->EntityPtr())[2] );
            }
          if (p==NULL)
            gs_pCmd->ExtendCmdLine("CTag "); //try again
          else
            {
            nEntSelects = 1;
            if (sm_pCnsDlg)
              sm_pCnsDlg->SetEntityCount(nEntSelects);
            pGWnd->SetCursor(IDC_SELECTGRF);
            }
          break;
          }
        default :
          break;
        }
      switch (Modifier)
        {
        case MID_Dig  :
        case MID_IWin :
        case MID_CWin :
        case MID_CTag :
          {
          int Cnt=0;
          CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
          while (p && nTxtEnt<1)
            {
            if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
              {
              flag Try=1;
              for (int i=0; i<nTxtEnt; i++)
                if (p==pTxtEnt[i])
                  Try=0;
              if (Try)
                {
                char ca[1024];
                MakeATag(ca, sizeof(ca), (pchar)DXF_TEXT_VALUE_GET(p->EntityPtr()));

                pTxtEnt[nTxtEnt]=p;
                nTxtEnt++;
                if (sm_pCnsDlg)
                  {
                  Strng UniqueTag, ATag(ca);
                  CUniqueTagRulesBlk TRB(AClass(), ATag(), '/');
                  if (gs_pPrj->TestModelTagUnique(ATag(), TRB, UniqueTag, True))
                    {
                    //LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag:\n%s\n not Found", ATag());
                    //ATag=UniqueTag();
                    }
                  sm_pCnsDlg->SetTag(ATag());
                  HideTagOff=True;
                  }
                else
                  ATag=ca;
                if (nTxtEnt==1)
                  {
                  text_info Ti;
                  text_info_from_dxf_text(&Ti,pDrw->StyleLst,p->EntityPtr());
                  // Since these origins are already aligned we must have no justifying
                  Attr_Settings TSet(50,
                                     Ti.flags | Ti.on ? 0 : DXF_ATTRIB_INVIS,
                                     Ti.height,
                                     Ti.wfactor,
                                     Rads2Degs(Ti.angle),
                                     GR_DARKGRAY,
                                     Text_Left,Text_Baseline,
                                     Ti.style);

                  Tag_Attr_Set=TSet;
                  }
                }
              }
            p = pDsp->Vp1->NextSelectedEntity();
            }
          }
        }
      if (sm_pCnsDlg)
        sm_pCnsDlg->SetFocus();
      break;
      }
    case EX_EXEC :
    case EX_DOIT :
      if (nEntSelects==0)
        {
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No entities Selected");
        break;
        }

      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->CompleteForUse(true);
        ATag = sm_pCnsDlg->m_Tag();
        AClass = sm_pCnsDlg->m_Model();
        TagBase = sm_pCnsDlg->m_BaseTag();
        HideTag = sm_pCnsDlg->m_HideTag;
        TaggedObject::ValidateTag(AClass);
        }

      if (AClass.Len()==0)
        AClass = "X";
      if (ATag.Len()>0 && AClass.Len()>0)
        {
        ParseTheTag(ATag, true);
        Strng UniqueTag;
        CUniqueTagRulesBlk TRB(AClass(), TagBase.Length() == 0 ? AClass() : TagBase());
        if (!gs_pPrj->TestModelTagUnique(ATag(), TRB, UniqueTag, False))
          {
          if (ASymbol.Length()==0)
            ASymbol = AClass.GetLength()>0?"EQP":AClass();

          DXF_ENTITY pEnt;
          //TRACE("CTAG %g %g %g", CTagPt.World.X, CTagPt.World.Y, CTagPt.World.Z);
          Tag_Attr_Set.Flags = HideTag && !HideTagOff ? DXF_ATTRIB_INVIS : 0;
          if ((pEnt=pDrw->Add_Unit(pDsp->Vp1, CTagPt.World, bCTagPtSet, AUnitId(), ATag(), ASymbol(), AClass(), Tag_Attr_Set))!=NULL)
            {
            pDsp->Draw(pEnt, -1);
            //AddUnitModel(AClass(), ATag());
            }
          else
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Symbol Not constructed");
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Symbol Not constructed, as Tag '%s' Not Found", ATag());
        }
      else
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag & Model must be specified");
      pDsp->Vp1->ClrSelectionAllList();
      pDsp->Vp1->ClearAllEntity();
      break;
    case EX_UNDO :
      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->SetIWin(False);
        sm_pCnsDlg->SetCWin(False);
        }
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pEnt1 = NULL;
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->DestroyWindow();
        delete sm_pCnsDlg;
        sm_pCnsDlg = NULL;
        }
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }
  pDsp->Close();
  }

//===========================================================================
// This command allows the user to dig random entitiies ( not inserts however ?? OK Now cnm)
// and from the entities will construct a piece of Unitment. The Unitment will
// be given a name which is either entered or is text in the digitised list.
// the origin of the figure will be at the centre of the text if existing
// otherwise at the centre of gravity of the entities.

void GrfCmdBlk::DoConstructUnit()
  {
  //SETVNT("CONStruct", "UNit", "@ IWin CWin Tag Unitid Symbol Model * CTag ", 1, (CmdFn)&GrfCmdBlk::DoConstructUnit, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static CInsertUnitDlg* sm_pCnsDlg = NULL;
  enum MID { MID_Dig=1, MID_IWin, MID_CWin, MID_Tag, MID_Eqp,
             MID_Symbol, MID_Model, MID_Dlg, MID_CTag};
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          bFlag1 = 0;
          i1 = 0;
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            pDsp->Vp1->ClearAllEntity();
            ATag = "";
            AUnitId = "";
            ASymbol = "";
            AClass = "";
            nTxtEnt=0;
            pTxtEnt[0]=NULL;
            pTxtEnt[1]=NULL;
            nEntSelects=0;
            bCTagPtSet=0;
            HideTagOff=False;
            gs_pCmd->ExtendCmdLine("CTag ");
            }
          break;
        case MID_Dig    : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTGRF); break;
        case MID_IWin   :
        case MID_CWin   : gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1); pGWnd->SetCursor(IDC_SELECTBOXGRF); break;
        case MID_CTag   : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTTAG); break;
        case MID_Tag    : gs_pCmd->SetParmInfo(AName, 1); ATag = ""; break;
        case MID_Eqp    : gs_pCmd->SetParmInfo(AName, 1); AUnitId = ""; break;
        case MID_Symbol : gs_pCmd->SetParmInfo(AName, 1); ASymbol = ""; break;
        case MID_Model  : gs_pCmd->SetParmInfo(AName, 1); AClass = ""; break;
        case MID_Dlg    : sm_pCnsDlg = new CInsertUnitDlg(CDlgWhat_ConUnit, false, pMdl, AfxGetMainWnd()); break;
        default :
          break;
        }
      break;
      }
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Tag    : ATag = gs_pCmd->LastToken(); break;
        case MID_Eqp    : AUnitId = gs_pCmd->LastToken(); break;
        case MID_Symbol : ASymbol = gs_pCmd->LastToken(); break;
        case MID_Model  : AClass = gs_pCmd->LastToken(); break;
        default : break;
        }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      switch (Modifier)
        {
        case MID_Dig :
        case MID_IWin :
        case MID_CWin :
          {
          CEntInView* p;
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==MID_Dig)
            {
            BOOL AlreadySelected;
            p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
            if (p && AlreadySelected)
              pDsp->Vp1->DeSelectEntity(p);
            }
          else if (Modifier==MID_IWin)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else if (Modifier==MID_CWin)
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          nEntSelects=0;
          p = pDsp->Vp1->FirstSelectedEntity();
          while (p)
            {
            if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && Find_Attr_Value(p->EntityPtr(), TagAttribStr))
              pDsp->Vp1->DeSelectEntity(p);
            else
              nEntSelects++;
            p = pDsp->Vp1->NextSelectedEntity();
            }
          pGWnd->SetCursor(IDC_SELECTGRF);
          if (sm_pCnsDlg)
            {
            if (Modifier==MID_IWin)
              sm_pCnsDlg->SetIWin(False);
            else if (Modifier==MID_CWin)
              sm_pCnsDlg->SetCWin(False);
            sm_pCnsDlg->SetEntityCount(nEntSelects);
            }
          break;
          }
        case MID_CTag:
          {
          pDsp->Vp1->ClrSelectionAllList();
          CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && Find_Attr_Value(p->EntityPtr(), TagAttribStr))
            {
            pDsp->Vp1->DeSelectEntity(p);
            //p = NULL;//try again
            }
          if (p && p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
            {
            bCTagPtSet=1;
            CTagPt.World.Set( DXF_TEXT_INS_PT(p->EntityPtr())[0],
                              DXF_TEXT_INS_PT(p->EntityPtr())[1],
                              DXF_TEXT_INS_PT(p->EntityPtr())[2] );
            }
          if (p==NULL)
            {
            i1++;
            if (i1>2)
              {
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Need to select an unconfigured graphic or tag, try again.");
              }
            if (i1<7)
              gs_pCmd->ExtendCmdLine("CTag "); //try again
            else
              {//give up and exit...
              gs_pCmd->ExtendCmdLine("\x1b\x1b\x1b");
              }
            }
          else
            {
            i1 = 0;
            nEntSelects = 1;
            if (sm_pCnsDlg)
              sm_pCnsDlg->SetEntityCount(nEntSelects);
            pGWnd->SetCursor(IDC_SELECTGRF);
            }
          break;
          }
        default :
          break;
        }
      switch (Modifier)
        {
        case MID_Dig  :
        case MID_IWin :
        case MID_CWin :
        case MID_CTag :
          {
          int Cnt=0;
          CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
          while (p && nTxtEnt<1)
            {
            if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
              {
              flag Try=1;
              for (int i=0; i<nTxtEnt; i++)
                if (p==pTxtEnt[i])
                  Try=0;
              if (Try)
                {
                char ca[1024];
                MakeATag(ca, sizeof(ca), (pchar)DXF_TEXT_VALUE_GET(p->EntityPtr()));

                pTxtEnt[nTxtEnt]=p;
                nTxtEnt++;

                ATag = ca;
                CheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), true);

                if (sm_pCnsDlg)
                  {
                  sm_pCnsDlg->SetTag(ATag());
                  HideTagOff=True;
                  }

                if (nTxtEnt==1)
                  {
                  text_info Ti;
                  text_info_from_dxf_text(&Ti, pDrw->StyleLst, p->EntityPtr());
                  // Since these origins are already aligned we must have no justifying
                  Attr_Settings TSet(50,
                                     Ti.flags | Ti.on ? 0 : DXF_ATTRIB_INVIS,
                                     Ti.height,
                                     Ti.wfactor,
                                     Rads2Degs(Ti.angle),
                                     GR_DARKGRAY,
                                     Text_Left, Text_Baseline,
                                     Ti.style);

                  Tag_Attr_Set=TSet;
                  }
                }
              }
            p = pDsp->Vp1->NextSelectedEntity();
            }
          }
        }
      if (sm_pCnsDlg)
        sm_pCnsDlg->SetFocus();
      break;
      }
    case EX_EXEC :
    case EX_DOIT :
      bFlag1 = 1; //need to do the work
      break;
    case EX_UNDO :
      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->SetIWin(False);
        sm_pCnsDlg->SetCWin(False);
        }
      break;
    case EX_RESULTS :
      if (bFlag1)
        {
        if (nEntSelects==0)
          {
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No entities Selected");
          break;
          }

        if (sm_pCnsDlg)
          {
          sm_pCnsDlg->CompleteForUse(true);
          ATag = sm_pCnsDlg->m_Tag();
          AClass = sm_pCnsDlg->m_Model();
          TagBase = sm_pCnsDlg->m_BaseTag();
          HideTag = sm_pCnsDlg->m_HideTag;
          TaggedObject::ValidateTag(AClass);
          }
        if (ATag.Len()>0 && AClass.Len()>0)
          {
          CheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), true);

          if (ASymbol.Length()==0)
            ASymbol = AClass.GetLength()>0?AClass():"EQP";

          DXF_ENTITY pEnt;
          //TRACE("CTAG %g %g %g", CTagPt.World.X, CTagPt.World.Y, CTagPt.World.Z);
          Tag_Attr_Set.Flags = HideTag && !HideTagOff ? DXF_ATTRIB_INVIS : 0;
          if ((pEnt=pDrw->Add_Unit(pDsp->Vp1, CTagPt.World, bCTagPtSet, AUnitId(), ATag(), ASymbol(), AClass(), Tag_Attr_Set))!=NULL)
            {
            pDsp->Draw(pEnt, -1);
            AddUnitModel(AClass(), ATag());
            }
          else
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit Not constructed");
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag & Model must be specified");
        //pDsp->Vp1->ClrSelectionAllList();
        //pDsp->Vp1->ClearAllEntity();
        }
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->DestroyWindow();
        delete sm_pCnsDlg;
        sm_pCnsDlg = NULL;
        }
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------
// This command allows the user to dig random entities
// and from the entities will construct a link. The link will
// be given a name which is either entered or is text in the digitised list.
// the origin of the figure will be at the centre of the text if existing
// otherwise at the centre of gravity of the entities.

void GrfCmdBlk::DoConstructLink()
  {
  //SETVNT("CONStruct", "LInk", "@ IWin CWin SNd SIo DNd DIo Tag Unitid SYmbol Model * CTag ", 1, (CmdFn)&GrfCmdBlk::DoConstructLink, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static CInsertUnitDlg* sm_pCnsDlg = NULL;
  enum MID { MID_Dig=1, MID_IWin, MID_CWin, MID_SNd, MID_SIo, MID_DNd, MID_DIo,
             MID_Tag, MID_Eqp, MID_Symbol, MID_Model, MID_Dlg, MID_CTag};
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          bFlag1 = 0;
          i1 = 0;
          if (!pMdl)
            {
            gs_pCmd->ExtendCmdLine("\x1b");
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            }
          else
            {
            gs_pCmd->SetDlgBusy();
            pDsp->Vp1->ClearAllEntity();
            i1 = 0; //count of failures to select unit
            ATag = "";
            AUnitId = "";
            ASymbol = "";
            AClass = "";
            nTxtEnt=0;
            pTxtEnt[0]=NULL;
            pTxtEnt[1]=NULL;
            gs_pCmd->ExtendCmdLine("SNd ");
            nEntSelects=0;
            bCTagPtSet=0;
            HideTagOff=False;
            }
          break;
        case MID_Dig    : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTGRF); break;
        case MID_IWin   :
        case MID_CWin   : gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1); pGWnd->SetCursor(IDC_SELECTBOXGRF); break;
        case MID_CTag   : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTTAG); break;
        case MID_SNd    : gs_pCmd->SetDigInfo(GC_NoCurs, 1); gs_pCmd->SetParmInfo(AName, 1); pGWnd->SetCursor(IDC_SOURCECURS); break;
        case MID_SIo    : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_DNd    : gs_pCmd->SetDigInfo(GC_NoCurs, 1); gs_pCmd->SetParmInfo(AName, 1); pGWnd->SetCursor(IDC_DESTCURS); break;
        case MID_DIo    : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_Tag    : gs_pCmd->SetParmInfo(AName, 1); ATag = ""; break;
        case MID_Eqp    : gs_pCmd->SetParmInfo(AName, 1); AUnitId = ""; break;
        case MID_Symbol : gs_pCmd->SetParmInfo(AName, 1); ASymbol = ""; break;
        case MID_Model  : gs_pCmd->SetParmInfo(AName, 1); AClass = ""; break;
        case MID_Dlg    :
          {
          if (SrcTag.Length()==0)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No Source Tag");
          else if (DstTag.Length()==0)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No Destination Tag");
          else
            {
            if (iSrcIOIndex>=0 && iSrcIOIndex<SrcIONames.GetSize() &&
                iDstIOIndex>=0 && iDstIOIndex<DstIONames.GetSize())
              {
              SrcIO = SrcIONames[iSrcIOIndex];
              DstIO = DstIONames[iDstIOIndex];
              // Get SrcIOTag & DstIOTag - Also Pass IO lists to dlg
              flag SlctIOTags = (SrcIO.Length()==0 || DstIO.Length()==0);
              //sm_pCnsDlg = new CConstructDlg(pMdl, false, SlctIOTags, AfxGetMainWnd());

              CDlgWhat What=CDlgWhat_Null;
              if (iSrcIOIndex>=0 && iSrcIOIndex<SrcIONames.GetSize())
                {
                if (SrcIOClass[iSrcIOIndex]&nc_MLnk)
                  What=CDlgWhat_ConLink;
                else if (SrcIOClass[iSrcIOIndex]&nc_CLnk)
                  What=CDlgWhat_ConCLnk;
                else if (SrcIOClass[iSrcIOIndex]&nc_ELnk)
                  What=CDlgWhat_ConELnk;
                else if (SrcIOClass[iSrcIOIndex]&nc_ALnk)
                  What=CDlgWhat_ConALnk;
                }
              sm_pCnsDlg = new CInsertUnitDlg(What, false, pMdl, AfxGetMainWnd());
              }
            else
              {
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Incomplete connect information");
              gs_pCmd->ExtendCmdLine("\x1b\x1b\x1b\x1b");
              }
            }
          break;
          }
        default : break;
        }
      break;
      }
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Tag    : ATag = gs_pCmd->LastToken(); break;
        case MID_Eqp    : AUnitId = gs_pCmd->LastToken(); break;
        case MID_Symbol : ASymbol = gs_pCmd->LastToken(); break;
        case MID_Model  : AClass = gs_pCmd->LastToken(); break;
        case MID_SIo :
          {
          SrcIO = gs_pCmd->LastToken();
          CRequestModelIOInfoRec Info;
          flag GotSrcOut=0;
          for (int nIns=0; gs_pPrj->RequestModelIOInfoByIndex(SrcTag(), nIns, Info); nIns++)
            if (!Info.Input && Info.Name.XStrICmp(SrcIO())==0)
              GotSrcOut=1;
          if (!GotSrcOut)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Src IO %s Not Found", SrcIO());
          }
          break;
        case MID_DIo :
          {
          DstIO = gs_pCmd->LastToken();
          CRequestModelIOInfoRec Info;
          flag GotDstOut=0;
          for (int nIns=0; gs_pPrj->RequestModelIOInfoByIndex(DstTag(), nIns, Info); nIns++)
            if (Info.Input && Info.Name.XStrICmp(DstIO())==0)
              GotDstOut=1;
          if (!GotDstOut)
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Dst IO %s Not Found", DstIO());
          }
          break;
        default : break;
        }
      break;
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      switch (Modifier)
        {
        case MID_Dig :
        case MID_IWin :
        case MID_CWin :
          {
          CEntInView* p;
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==MID_Dig)
            {
            BOOL AlreadySelected;
            p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
            if (p && AlreadySelected)
              pDsp->Vp1->DeSelectEntity(p);
            }
          else if (Modifier==MID_IWin)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else if (Modifier==MID_CWin)
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          nEntSelects=0;
          p = pDsp->Vp1->FirstSelectedEntity();
          while (p)
            {
            if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && Find_Attr_Value(p->EntityPtr(), TagAttribStr))
              pDsp->Vp1->DeSelectEntity(p);
            else
              nEntSelects++;
            p = pDsp->Vp1->NextSelectedEntity();
            }
          pGWnd->SetCursor(IDC_SELECTGRF);
          if (sm_pCnsDlg)
            {
            if (Modifier==MID_IWin)
              sm_pCnsDlg->SetIWin(False);
            else if (Modifier==MID_CWin)
              sm_pCnsDlg->SetCWin(False);
            sm_pCnsDlg->SetEntityCount(nEntSelects);
            }
          break;
          }
        case MID_CTag:
          {
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && Find_Attr_Value(p->EntityPtr(), TagAttribStr))
            {
            pDsp->Vp1->DeSelectEntity(p);
            //p = NULL; //try again
            }
          if (p && p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
            {
            bCTagPtSet=1;
            CTagPt.World.Set( DXF_TEXT_INS_PT(p->EntityPtr())[0],
                              DXF_TEXT_INS_PT(p->EntityPtr())[1],
                              DXF_TEXT_INS_PT(p->EntityPtr())[2] );
            }
          if (p==NULL)
            gs_pCmd->ExtendCmdLine("CTag "); //try again
          else
            {
            nEntSelects = 1;
            if (sm_pCnsDlg)
              sm_pCnsDlg->SetEntityCount(nEntSelects);
            pGWnd->SetCursor(IDC_SELECTGRF);
            }
          break;
          }
        case MID_SNd :
        case MID_DNd :
          {
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          Strng TmpTag, GrfGuid, MdlGuid;
          CEntInView* p = GetClosest(pDsp->CurrentPt, true, false, false, TmpTag, GrfGuid, MdlGuid);
          flag ConnOK = False;
          if (p)
            {
            pDsp->Vp1->SelectEntity(p);
            pGWnd->SetCursor();
            Strng TagMod;
            if (Modifier==MID_SNd)
              {
              SrcTag = TmpTag;
              SrcIO = "O";
              ConnOK = GetConnsForUnit(SrcTag(), False, nc_MLnk|nc_CLnk|nc_ELnk|nc_ALnk, SrcIO, SrcIONames, SrcIODescs, SrcIOClass, SrcTermStrips);
              if (ConnOK && iSrcIOIndex<0)
                ConnOK = False;
              TagMod.Set(",%s.%s ", SrcTag(), SrcIO());
              }
            else
              {
              DstTag = TmpTag;
              DstIO = "I";
              ConnOK = GetConnsForUnit(DstTag(), True, iSrcIOIndex>=0 ? SrcIOClass[iSrcIOIndex] : nc_MLnk|nc_CLnk|nc_ELnk|nc_ALnk, DstIO, DstIONames, DstIODescs, DstIOClass, DstTermStrips);
              if (ConnOK && iDstIOIndex<0)
                ConnOK = False;
              TagMod.Set(",%s.%s ", DstTag(), DstIO());
              }
            if (ConnOK)
              {
              gs_pCmd->ExtendModifier(TagMod());
              gs_pCmd->ExtendCmdLine((Modifier==MID_SNd) ? "DNd " : "* CTag ");
              TRACE(TagMod());
              i1 = 0;
              }
            }
          if (!ConnOK)
            {
            i1++;
            if (i1<3)
              {
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Need to select a valid '%s unit', try again (attempt %d of 3).", (Modifier==MID_SNd) ? "source" : "destination", i1+1);
              gs_pCmd->ExtendModifier(",x ");
              gs_pCmd->ExtendCmdLine(Modifier==MID_SNd ? "SNd " : "DNd "); //try again
              }
            else
              {//give up and exit...
              LogError("GrfCmds", LF_DoAfxMsgBox, "Command canceled!  Need to select a valid '%s unit'.", (Modifier==MID_SNd) ? "source" : "destination");
              gs_pCmd->ExtendCmdLine("\x1b\x1b");
              }
            }
          break;
          }
        case MID_SIo :
          pDsp->Show_Dig_Point(pDsp->CurrentPt);
          SrcIO = ".O";
          gs_pCmd->ExtendModifier(",");
          gs_pCmd->ExtendModifier(SrcIO());
          TRACE1("SrcIODig  %s", SrcIO());
          break;
        case MID_DIo :
          DstIO = "";
          gs_pCmd->ExtendModifier(",");
          gs_pCmd->ExtendModifier(DstIO());
          TRACE1("DstIODig  %s", DstIO());
          break;
        default :
          break;
        }
      switch (Modifier)
        {
        case MID_Dig :
        case MID_IWin :
        case MID_CWin :
          {
          int Cnt=0;
          CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
          while (p && nTxtEnt<1)
            {
            if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
              {
              flag Try=1;
              for (int i=0; i<nTxtEnt; i++)
                if (p==pTxtEnt[i])
                  Try=0;
              if (Try)
                {
                char ca[1024];
                MakeATag(ca, sizeof(ca), (pchar)DXF_TEXT_VALUE_GET(p->EntityPtr()));

                pTxtEnt[nTxtEnt]=p;
                nTxtEnt++;
                
                ATag = ca;
                CheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), true);
                
                if (sm_pCnsDlg)
                  {
                  sm_pCnsDlg->SetTag(ATag());
                  HideTagOff=True;
                  }

                if (nTxtEnt==1)
                  {
                  text_info Ti;
                  text_info_from_dxf_text(&Ti,pDrw->StyleLst,p->EntityPtr());
                  Attr_Settings TSet(50,
                                     Ti.flags | Ti.on ? 0 : DXF_ATTRIB_INVIS,
                                     Ti.height,
                                     Ti.wfactor,
                                     Ti.angle,
                                     GR_DARKGRAY,
                                     Ti.horiz_org==DXF_TEXT_L ? Text_Left :
                                    (Ti.horiz_org==DXF_TEXT_R ? Text_Right : Text_Cntrd),
                                     Ti.vert_org==DXF_TEXT_TOP ? Text_Top :
                                    (Ti.vert_org==DXF_TEXT_BOTTOM ? Text_Bottom :
                                    (Ti.vert_org==DXF_TEXT_MIDDLE ? Text_Middle : Text_Baseline)),
                                     Ti.style);

                  Tag_Attr_Set=TSet;
                  if (Ti.horiz_org>0 || Ti.vert_org>0)
                    {
                    CTagPt.World.X=PT3_X(DXF_TEXT_AL_PT(p->EntityPtr()));
                    CTagPt.World.Y=PT3_Y(DXF_TEXT_AL_PT(p->EntityPtr()));
                    CTagPt.World.Z=PT3_Z(DXF_TEXT_AL_PT(p->EntityPtr()));
                    }
                  else
                    {
                    CTagPt.World.X=PT3_X(Ti.pos);
                    CTagPt.World.Y=PT3_Y(Ti.pos);
                    CTagPt.World.Z=PT3_Z(Ti.pos);
                    }
                  }
                }
              }
            p = pDsp->Vp1->NextSelectedEntity();
            }
          }
        }
      if (sm_pCnsDlg)
        sm_pCnsDlg->SetFocus();
      break;
      }
    case EX_EXEC :
    case EX_DOIT :
      bFlag1 = 1; //need to do the work
      break;
    case EX_UNDO :
      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->SetIWin(False);
        sm_pCnsDlg->SetCWin(False);
        }
      pGWnd->SetCursor();
      break;
    case EX_RESULTS :
      if (bFlag1)
        {
        if (nEntSelects==0)
          {
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No entities Selected");
          break;
          }

        if (sm_pCnsDlg)
          {
          sm_pCnsDlg->CompleteForUse(true);
          ATag = sm_pCnsDlg->m_Tag();
          AClass = sm_pCnsDlg->m_Model();
          TagBase = sm_pCnsDlg->m_BaseTag();
  //        SrcTag = sm_pCnsDlg->m_SrcTag.GetBuffer(0);
  //        DstTag = sm_pCnsDlg->m_DstTag.GetBuffer(0);
          HideTag = sm_pCnsDlg->m_HideTag;
          TaggedObject::ValidateTag(AClass);
          TaggedObject::ValidateTag(SrcTag);
          TaggedObject::ValidateTag(DstTag);
          TaggedObject::ValidateTag(SrcIO);
          TaggedObject::ValidateTag(DstIO);
          }

        if (ATag.Length()>0 && AClass.Length()>0)
          {
          CheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), true);

          if (AUnitId.Length()==0)
            AUnitId="";
          if (ASymbol.Length()==0)
            {
            ASymbol="_";
            ASymbol+=ATag();
            }
          DXF_ENTITY pEnt;
          Tag_Attr_Set.Flags=HideTag && !HideTagOff ? DXF_ATTRIB_INVIS : 0;
          if ((pEnt=pDrw->Add_Unit(pDsp->Vp1, CTagPt.World, bCTagPtSet, AUnitId(), ATag(), ASymbol(), AClass(), Tag_Attr_Set))!=NULL)
            {
            pDsp->Draw(pEnt, -1);
            CMdlValueSet::Clear();
            int err = gs_pPrj->AddNodeConnect(AClass(), ATag(), SrcTag(), SrcIO(), DstTag(), DstIO());
            if (err)
              {
              Strng S;
              S.Set("Connection not Made:\n%s:%s\n%s.%s->%s.%s",
                    AClass(), ATag(), SrcTag(), SrcIO(), DstTag(), DstIO());
              LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, S());
              }
            }
          else
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Link Not constructed");
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag & Model must be specified");
          //pDrw->PrintEntities(pDsp->Vp1,"After.ent");
        pDsp->Vp1->ClrSelectionAllList();
        pDsp->Vp1->ClearAllEntity();

        if (SrcIOClass[iSrcIOIndex]&nc_ELnk)
          {
          if (m_bAutoWire && SrcTS() && DstTS())
            {
            CLinkWiring LW;
            LW.m_bAutoWire=true;
            LW.m_LinkTag=ATag();
            LW.m_SrcTS=SrcTS();
            LW.m_DstTS=DstTS();
            gs_pPrj->SetLinkWiring(LW);
            }
          else
            CELnkWiring::Start(ATag());
          }
        }
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      if (sm_pCnsDlg)
        {
        sm_pCnsDlg->DestroyWindow();
        delete sm_pCnsDlg;
        sm_pCnsDlg = NULL;
        }
      gs_pCmd->SetDlgBusy(0);
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoConstructTie()
  {
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pGWnd->SetCursor(IDC_SELECTGRF);
          Like = NULL;
          bFlag1 = 0; //has a dig point cross been drawn
          break;
        case 1 :
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetDigInfo(GC_BigCurs, 2);
          pGWnd->SetCursor(IDC_SELECTGRF);
          nEntSelects=0;
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      switch (Modifier)
        {
        case 1 :
          {
          CEntInView* p = GetClosest(pDsp->CurrentPt, false);
          if (p)
            {
            pDsp->Vp1->SelectEntity(p);
            if (nEntSelects++==0)
              {
              Pt1=pDsp->CurrentPt;
              pDsp->Show_Dig_Point(Pt1);
              bFlag1=1;
              }
            else
              {
              nEntSelects=0;
              if (pDrw->BreakCurvesAtIntersect(pDsp->Vp1, Pt1.World.p(), Pos)>0)
                {
                //todo : improve this (assumes Tie-1 model exists with prefix X!!!)
                Strng GrfSymb;
                ATag="X_1";
                AClass="Tie-1";
                GrfSymb="MixerSplitter:Tie-1";
                TagBase="X";
                TaggedObject::ValidateTag(AClass);
                CheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), false);

                Pt_3f Scl(1.0, 1.0, 1.0);
                float Rotate = (float)0.0;
                Tag_Attr_Set.Flags = DXF_ATTRIB_INVIS; // Force It Off
                DXF_ENTITY pEnt = AddUnitDrawing("", GrfSymb(), AClass(), (LPSTR)(LPCSTR)TaggedObject::CreateGuidStr(), ATag(), NULL, Pos, Scl, Rotate, True, Tag_Attr_Set);
                Like = pEnt;
                if (pEnt)
                  {
                  pDsp->Draw(pEnt, -1);
                  AddUnitModel(AClass(), ATag());
                  }
                else
                  LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit Not constructed");
                gs_pCmd->ExtendCmdLine("\r"); //command complete
                }
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              }
            }
          break;
          }
        }
      break;
      }
    case EX_EXEC :
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      if (bFlag1)
        pDsp->Hide_Dig_Point(Pt1);
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      if (Like)
        {//select/hilight tie just inserted...
        pDsp->SetCPtWorld(Pos, Pt4);
        CEntInView* p = GetClosest(Pt4, true);
        if (p)
          pDsp->Vp1->SelectEntity(p);
        //gs_pCmd->SetRetCode(1);
        gs_pCmd->Print("Tie '%s' inserted\n", ATag());
        }
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------
/*
void GrfCmdBlk::DoAttachModel()
// This command to allow the user to digitise a valid piece of Unitment
// and relate it to a model
  {
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
        {
        switch (Modifier)
          {
          case 1 :
            gs_pCmd->SetDigInfo(GC_BigCurs, 1);
            break;
          case 2 :
            //input a name
            break;
          default :
            break;
          }
        }
      break;
    case EX_MOUSE_LUP :
        {
        CWaitCursor Wait;
        switch (Modifier)
          {
          case 1 :
            pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            break;
          default :
            break;
          }
        }
      break;
    case EX_DOIT :
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
      break;
    default :;
    }
  pDsp->Close();
  };
*/

//---------------------------------------------------------------------------

void GrfCmdBlk::DoBreakLine()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          ActivateGWnd();
          pDsp->Vp1->ClearAllEntity();
          break;
        case 1 :
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetDigInfo(GC_BigCurs, 2);
          nEntSelects=0;
          break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 :
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          if (nEntSelects++==0)
            Pt1=pDsp->CurrentPt;
          break;
        }
      break;
    case EX_EXEC :
      {
      Pt_3f IntPt;
      pDrw->BreakCurvesAtIntersect(pDsp->Vp1, Pt1.World.p(), IntPt);
//      pDsp->Vp1->ClearAllEntity();
      }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      break;
    default :;
    }
  pDsp->Close();
  };

// mhm end change
//---------------------------------------------------------------------------

void GrfCmdBlk::DoTranslate()
  {
  //SETVNT("TRanslate", "ENtity", "@ ; Win XScale YScale XY X Y NX NY SCale SPread", 1, (CmdFn)&GrfCmdBlk::DoTranslate, EF_Edit);
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          DoX = 1;
          DoY = 1;
          DoXScl = 0;
          DoYScl = 0;
          DoSpred = 0;
          Like = NULL;
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          pGWnd->SetCursor(IDC_SELECTGRF);
          break;
        case 1 : // Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          pGWnd->SetCursor(Like ? IDC_POS : IDC_SELECTGRF);
          break;
        case 3 : gs_pCmd->SetDigInfo(GC_BigCurs | GC_RectCurs, 2); break; // Win
        case 4 : DoXScl = 1; break;
        case 5 : DoYScl = 1; break;
        case 6 : DoX = 1; DoY = 1; break;
        case 7 : DoY = 0; DoX = 1; break;
        case 8 : DoX = 0; DoY = 1; break;
        case 9 : DoX = 0; break;
        case 10 : DoY = 0; break;
        case 11 : DoXScl = 1; DoYScl = 1; break;
        case 12 : DoSpred = 1; break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 : //Dig
          if (Like==NULL)
            {
            pDsp->Vp1->ClrSelectionAllList();
            //pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            //pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            CEntInView* pEnt = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            if (pEnt && pEnt->EntityPtr())
              {
              Like = pEnt->EntityPtr();
              pGWnd->SetCursor(IDC_POS);
              Pt1 = pDsp->StartPt;
              Pt2 = Pt1;
              }
            //Like = pDrw->ClosestInsert(pDsp->CurrentPt);
            }
          else
            {
            pDsp->Draw(Like, GrfHelper.GR_BACKGROUND);
            pDrw->TranslateEntity(Like, Pt1.World, pDsp->CurrentPt.World);
            Pt1 = pDsp->CurrentPt;
            pDrw->EntityInvalidate(Like, NULL);
            pDsp->Vp1->ClearAllEntity();
            pDsp->Draw(Like, -1);
            }
          break;
        case 2 : //Win
          switch (gs_pCmd->DigNo())
            {
            case 1 :                // the first digitise
              Pt1 = pDsp->StartPt;
              Pt2 = pDsp->EndPt;
              pDsp->Show_Dig_Rect(Pt1, Pt2);
              pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
              if (! DoXScl && ! DoYScl && ! DoSpred)
                {
                pDsp->SetCursImgBase(Pt1.Logical);
                pDsp->SetCursImgPt(0, Pt1.Logical.X, Pt1.Logical.Y);
                pDsp->SetCursImgPt(1, Pt2.Logical.X, Pt1.Logical.Y);
                pDsp->SetCursImgPt(2, Pt2.Logical.X, Pt2.Logical.Y);
                pDsp->SetCursImgPt(3, Pt1.Logical.X, Pt2.Logical.Y);
                pDsp->SetCursImgPt(4, Pt1.Logical.X, Pt1.Logical.Y);
                gs_pCmd->SetDigInfo(GC_BigCurs | GC_ImageCurs);
                }
              break;
            case 2 :                // the second
              pDrw->TranslateEntitiesInWin(pDsp->Vp1, Pt1, Pt2, pDsp->StartPt, pDsp->EndPt, DoX, DoY, DoXScl, DoYScl, DoSpred);
              /*
                 pDsp->ReDraw(Pt1, Pt2);
                 for (int j=0; j<3; j++)
                 {
                 Pt_3i d;
                 d[j]=pDsp->EndPt.Logical[j]-pDsp->StartPt.Logical[j];
                 Pt1.Logical[j]+=d[j];
                 Pt2.Logical[j]+=d[j];
                 }
                 pDsp->ReDraw(Pt1, Pt2);
               */
              pWnd->Invalidate();
              break;
            }
          break;
        }
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy() && Like)
        {
        Like = NULL;
        pGWnd->SetCursor(IDC_SELECTGRF);
        }
      break;
    case EX_EXEC : break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      Like = NULL;
      pDsp->Vp1->ClrSelectionAllList();
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      break;
    default :;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoMoveEntity()
  {
  //SETVNT("MOve", "ENtity", "@ IWin CWin ; Redraw", 1, (CmdFn)&GrfCmdBlk::DoMoveEntity, EF_Edit);
  enum MID { MID_Dig=1, MID_IWin, MID_CWin, MID_Next, MID_Redraw };
    pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          SetToolbar(DynTBMoveEntity);
          Like = NULL;
          pEnt1 = NULL;
          bFlag1 = 0; //has a dig point cross been drawn
          bFlag2 = 0; //has IWin or CWin been used
          bFlag3 = 0; //has a dig rect been drawn
          bFlag4 = 0; //has RefPt been Drawn
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pGWnd->SetCursor(IDC_SELECTGRF);
          break;
        case MID_Dig :
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          //pGWnd->SetCursor(Like ? IDC_POS : IDC_SELECTGRF);
          break;
        case MID_IWin :
        case MID_CWin :
          pGWnd->SetCursor(IDC_SELECTBOX);
          gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1);
          break;
        case MID_Redraw :
          pWnd->Invalidate();
          if (bFlag3)
            pDsp->Show_Dig_Rect(Pt3, Pt4);
          break;
        }
      break;
    case EX_MOUSE_LDOWN :
      switch (Modifier)
        {
        case MID_Dig :
          if (Like || pEnt1)
            {
            if (bFlag4)
              pDsp->Hide_Dig_Point(RefPt);
            RefPt=pDsp->StartPt;
            pDsp->Show_Dig_Point(RefPt);
            bFlag4=True;
            }
          break;
        case MID_IWin :
        case MID_CWin :
          if (bFlag2)
            {
            if (bFlag4)
              pDsp->Hide_Dig_Point(RefPt);
            RefPt=pDsp->StartPt;
            pDsp->Show_Dig_Point(RefPt);
            bFlag4=True;
            }
          break;
        }
      break;
    case EX_MOUSE_LUP :
      if (bFlag4)
        pDsp->Hide_Dig_Point(RefPt);
      bFlag4=False;
      switch (Modifier)
        {
        case MID_Dig :
          if (bFlag2)
            {//move all entities in select window
            Pt_SLW E1,E2,D;
            D = pDsp->CurrentPt;
            D -= pDsp->StartPt;
            E1 = Pt3;
            E1 += D;
            E2 = Pt4;
            E2 += D;
            pDrw->TranslateEntitiesInList(pDsp->Vp1, Pt3, Pt4, E1, E2, 1, 1, 1, 1, 0);
            gs_pCmd->ExtendCmdLine(";"); //SelectionList will be clear or invalid, cannot continue repositioning !!!
            pWnd->Invalidate();
            }
          else
            {
            if (pEnt1==NULL)
              {//select one entity...
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              //pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
              //pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
              pEnt1 = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
              if (pEnt1)
                {
                Like = pEnt1->EntityPtr();
                pGWnd->SetCursor(IDC_POS);
                Pt1 = pDsp->CurrentPt;
                Pt3 = Pt1;
                }
              pDsp->Vp1->SelectAssocEntities();
              }
            else
              {//move the selected entity...
              if (Like)
                {
#if SYSCAD10         
                if (gs_pPrj->SvcActive)
                  {
                  Pt_3f Delta=pDsp->CurrentPt.World;
                  Delta=Delta-pDsp->StartPt.World;

                  gs_pCmd->ExtendCmdLine(";"); //SelectionList will be clear or invalid, cannot continue repositioning !!!

                  LPCSTR pGrfGuid= Scd10GetGrfGuid(Like);
                  if (pGrfGuid)
                    {
                    SCD10ENTER;
                    gs_pPrj->Svc.GCBModifyNodePosition((CGrfDoc*)pDoc, Like, pGrfGuid, Delta);
                    SCD10LEAVE;
                    }
                  else
                    LogError("MoveNode", 0, "Guid does not exist");
                  }
                else
#endif
                  {
                  pDsp->Draw(Like, GrfHelper.GR_BACKGROUND);
                  pDrw->TranslateEntity(Like, pDsp->StartPt.World, pDsp->CurrentPt.World);
                  gs_pCmd->ExtendCmdLine(";"); //SelectionList will be clear or invalid, cannot continue repositioning !!!
                  pDrw->EntityInvalidate(Like, NULL);
                  pDsp->Draw(Like, -1);
                  }
                Pt3 = pDsp->CurrentPt;
                }
              else
                {
                //need some code like this ...
                /*pDsp->Draw(pEnt1, GrfHelper.GR_BACKGROUND);
                pDrw->TranslateEntity(pEnt1, Pt3.World, pDsp->CurrentPt.World);
                pDrw->EntityInvalidate(pEnt1, NULL);
                pDsp->Draw(pEnt1, -1);*/

                Pt4 = pDsp->CurrentPt; //any arb point != Pt3
                Pt_SLW E;
                E = pDsp->CurrentPt;
                E += Pt4;
                E -= Pt3;
                pDrw->TranslateEntitiesInList(pDsp->Vp1, Pt3, Pt4, pDsp->CurrentPt, E, 1, 1, 1, 1, 0);
                gs_pCmd->ExtendCmdLine(";"); //SelectionList will be clear or invalid, cannot continue repositioning !!!
                pWnd->Invalidate();

                /*C3_CURVE c = pEnt1->CurvePtr();
                if (c)
                  {
                  pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
                  pDrw->TranslateEntity(c, Pt3.World, pDsp->CurrentPt.World); //moves to WRONG position!!
                  pDrw->EntityInvalidate(NULL, c);
                  pDsp->Draw(c, -1);
                  }
                pDsp->Vp1->ClearAllEntity();
                pDsp->Hide_Dig_Point(Pt3);
                Pt3 = pDsp->CurrentPt;
                pDsp->Show_Dig_Point(Pt3);*/
                }
              }
            }
          break;
        case MID_IWin :
        case MID_CWin :
          if (bFlag3)
            pDsp->Hide_Dig_Rect(Pt3, Pt4);
          Pt1 = pDsp->StartPt;
          Pt2 = pDsp->EndPt;
          Pt3 = Pt1;
          Pt4 = Pt2;
          pDsp->Show_Dig_Rect(Pt3, Pt4);
          bFlag3 = 1;
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          bFlag2 = 1;
          pDsp->Vp1->SelectAssocEntities();
          pGWnd->SetCursor(IDC_POS);
          break;
        }
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        {
        if (bFlag2 || pEnt1)
          {
          bFlag2 = 0;
          pEnt1 = NULL;
          Like = NULL;
          pGWnd->SetCursor(IDC_SELECTGRF);
          if (bFlag3)
            pDsp->Hide_Dig_Rect(Pt3, Pt4);
          bFlag3 = 0;
          bFlag4 = 0;
          }
        }
      break;
    case EX_EXEC : break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      Like = NULL;
      pEnt1 = NULL;
      pDsp->Vp1->ClrSelectionAllList();
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      if (bFlag3)
        pDsp->Hide_Dig_Rect(Pt3, Pt4);
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

//typedef CArray <DXF_ENTITY, DXF_ENTITY&> EntityArray;
typedef CArray <CEntInView*, CEntInView*&> EntityArray;

void GrfCmdBlk::DoAlterEntity()
  {
  //SETVNT("ALter", "ENtity", "@ IWin CWin ; REdraw SP SM RP RM * D1 D2", 1, (CmdFn)&GrfCmdBlk::DoAlterEntity, EF_Edit);
  static CScaleRotateDlg* pSclRotDlg = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          SetToolbar(DynTBAlterEntity);
          Like = NULL;
          pEnt1 = NULL;
          bFlag2 = 0; //has IWin or CWin been used
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pGWnd->SetCursor(IDC_SELECTGRF);
          break;
        case 1 : // Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : // IWin
        case 3 : // CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 5 : // Redraw
          pWnd->Invalidate();
          break;
        case 10 : // *
          if (pSclRotDlg)
            {
            pSclRotDlg->DestroyWindow();
            delete pSclRotDlg;
            }
          pSclRotDlg = new CScaleRotateDlg(AfxGetMainWnd());
          if (pSclRotDlg && Like && !bFlag2)
            pSclRotDlg->SetParms(DXF_INSERT_PT(Like)[0], DXF_INSERT_PT(Like)[1], DXF_INSERT_X_SCALE(Like), DXF_INSERT_Y_SCALE(Like), DXF_INSERT_ROT_ANG(Like));
          break;
        case 11 : // d1
          if (pSclRotDlg && Like)
            {
            EntityArray List;
            CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
            while (p)
              {
              if (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
                {
                DXF_ENTITY en = p->EntityPtr();
                pDsp->Draw(en, GrfHelper.GR_BACKGROUND);
                DXF_INSERT_ROT_ANG(en) = pSclRotDlg->Data.Rotation;
                DXF_INSERT_X_SCALE(en) = pSclRotDlg->Data.XScale;
                DXF_INSERT_Y_SCALE(en) = pSclRotDlg->Data.YScale;
                DXF_INSERT_Z_SCALE(en) = pSclRotDlg->Data.XScale; //make Z scale same as X
                DXF_INSERT_PT(en)[0] = pSclRotDlg->Data.XPos;
                DXF_INSERT_PT(en)[1] = pSclRotDlg->Data.YPos;
                pDrw->EntityInvalidate(en, NULL);
                CEntInView* pEnt = pDsp->Vp1->Draw(en, -1);
                List.Add(pEnt);
                }
              p = pDsp->Vp1->NextSelectedEntity();
              }
            const int jj = List.GetSize();
            if (jj>0)
              {
              Like = List[0]->EntityPtr();
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              for (int i=0; i<jj; i++)
                {
                if (List[i])
                  pDsp->Vp1->SelectEntity(List[i]);
                }
              }
            }
          break;
        case 6 : // Scale Plus
        case 7 : // Scale Minus
        case 8 : // Rotate Plus
        case 9 : // Rotate Minus
          if (bFlag2 || Like)
            {
            const BOOL Scaling = (Modifier==6 || Modifier==7);
            const double Factor = (Modifier==6 ? 1.6 : 0.625);
            EntityArray List;
            CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
            while (p)
              {
              if (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
                {
                DXF_ENTITY en = p->EntityPtr();
                pDsp->Draw(en, GrfHelper.GR_BACKGROUND);
                if (Scaling)
                  {
                  DXF_INSERT_X_SCALE(en) *= Factor;
                  DXF_INSERT_Y_SCALE(en) *= Factor;
                  DXF_INSERT_Z_SCALE(en) *= Factor;
                  }
                else
                  DXF_INSERT_ROT_ANG(en) += (Modifier==8 ? -45.0 : 45.0);
                pDrw->EntityInvalidate(en, NULL);
                CEntInView* pEnt = pDsp->Vp1->Draw(en, -1);
                List.Add(pEnt);
                }
              p = pDsp->Vp1->NextSelectedEntity();
              }
            const int jj = List.GetSize();
            if (jj>0)
              {
              Like = List[0]->EntityPtr();
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              for (int i=0; i<jj; i++)
                {
                if (List[i])
                  pDsp->Vp1->SelectEntity(List[i]);
                }
              }
            if (pSclRotDlg && Like && !bFlag2)
              pSclRotDlg->UpdateParms(DXF_INSERT_PT(Like)[0], DXF_INSERT_PT(Like)[1], DXF_INSERT_X_SCALE(Like), DXF_INSERT_Y_SCALE(Like), DXF_INSERT_ROT_ANG(Like));
            }
          break;
        }
      break;
    case EX_MOUSE_LDOWN :
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 : //Dig
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          bFlag2 = 0;
          pEnt1 = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          if (pEnt1)
            Like = pEnt1->EntityPtr();
          if (pSclRotDlg && Like)
            pSclRotDlg->SetParms(DXF_INSERT_PT(Like)[0], DXF_INSERT_PT(Like)[1], DXF_INSERT_X_SCALE(Like), DXF_INSERT_Y_SCALE(Like), DXF_INSERT_ROT_ANG(Like));
          break;
        case 2 : //IWin
        case 3 : //CWin
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          bFlag2 = 1;
          pGWnd->SetCursor(IDC_SELECTGRF);
          if (pSclRotDlg)
            pSclRotDlg->SetGroup(0.0, 0.0, 0.0);
          break;
        }
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        {
        pEnt1 = NULL;
        Like = NULL;
        pGWnd->SetCursor(IDC_SELECTGRF);
        pDsp->Vp1->ClearAllEntity();
        pDsp->Vp1->ClrSelectionAllList();
        bFlag2 = 0;
        }
      break;
    case EX_EXEC : break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      Like = NULL;
      pEnt1 = NULL;
      pDsp->Vp1->ClrSelectionAllList();
      pDsp->Vp1->ClearAllEntity();
      if (pSclRotDlg)
        {
        pSclRotDlg->DestroyWindow();
        delete pSclRotDlg;
        pSclRotDlg = NULL;
        }
      pGWnd->SetCursor();
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::PanAndSetCursor()
  {
  CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
  DXF_ENTITY e = pEnt->EntityPtr();

  if (gs_pPrj->m_bGrfPromptForZoom)
    {

    C3_BOX_S Bounds;
    PT3 p;
    c3v_set(1.1e30, 1.1e30, 1.1e30, p);
    c3a_box_init_pt(&Bounds, p);
    C3_TRANSFORM c3t;

    c3t[0][0] = c3t[0][1] = c3t[0][2] = c3t[0][3] = 0.0;
    c3t[1][0] = c3t[1][1] = c3t[1][2] = c3t[1][3] = 0.0;
    c3t[2][0] = c3t[2][1] = c3t[2][2] = c3t[2][3] = 0.0;
    c3t[0][0] = c3t[1][1] = c3t[2][2] = 1.0;

    // Get bound on the block
    pBlock b = pDrw->Blocks.Find(DXF_INSERT_BLOCK_GET(e));
    pDrw->ConstructInsertMatrix(c3t, b, e);
    b->GetBounds(c3t, &Bounds);

    double ZBx0=pDsp->Vp1->ZoomBox.minpt[0];
    double ZBy0=pDsp->Vp1->ZoomBox.minpt[1];
    double ZBx1=pDsp->Vp1->ZoomBox.maxpt[0];
    double ZBy1=pDsp->Vp1->ZoomBox.maxpt[1];

    double xl=ZBx1-ZBx0;
    double yl=ZBy1-ZBy0;

    xl=Max(xl, C3_MAX_X(&Bounds) - C3_MIN_X(&Bounds));
    yl=Max(yl, C3_MAX_Y(&Bounds) - C3_MIN_Y(&Bounds));

    double xc=(C3_MIN_X(&Bounds)+C3_MAX_X(&Bounds))/2.0;
    double yc=(C3_MIN_Y(&Bounds)+C3_MAX_Y(&Bounds))/2.0;

    double w = Max(C3_MAX_X(&pDrw->m_Bounds)-C3_MIN_X(&pDrw->m_Bounds), (REAL)1.0);
    double h = Max(C3_MAX_Y(&pDrw->m_Bounds)-C3_MIN_Y(&pDrw->m_Bounds), (REAL)1.0);

    double x0=pDrw->m_Bounds.minpt[0]-0.025*w;
    double y0=pDrw->m_Bounds.minpt[1]-0.025*h;
    double x1=pDrw->m_Bounds.maxpt[0]+0.025*w;
    double y1=pDrw->m_Bounds.maxpt[1]+0.025*h;

    double xlh=xl/2.0;
    double ylh=yl/2.0;
    if (xc-xlh<x0)
      xc-=xc-xlh-x0;
    if (xc+xlh>x1)
      xc-=xc+xlh-x1;
    if (yc-ylh<y0)
      yc-=yc-ylh-y0;
    if (yc+ylh>y1)
      yc-=yc+ylh-y1;

    pDsp->SetZoom(Zoom_Win, xc-xlh, yc-ylh, xc+xlh, yc+ylh);

    pWnd->Invalidate(True);
    }

  if (gs_pPrj->m_bGrfMoveCursor)
    {
    Pt_3f world;
    world.X=DXF_INSERT_PT(e)[0];//xc;
    world.Y=DXF_INSERT_PT(e)[1];//yc;
    world.Z=0.0;
    pDsp->SetCPtWorld(world, Pt4);
    SetCursorPos((int)Pt4.Screen.X, (int)Pt4.Screen.Y);
    }
  }

//---------------------------------------------------------------------------

static char * a999[] =
  {
  "Tag", (pchar)NULL
  };
static char * b999[] =
  {
  "", (pchar)NULL
  };

void GrfCmdBlk::DoSelect()
  {
  //SETVNT("MArk", "ENtity", "@ IWin CWin CLear Attach Tag Goto *", 5, (CmdFn)&GrfCmdBlk::DoSelect, EF_Edit);
  pDsp->Open();
  flag DoGoto = 0;
  flag GotoFromToolbar = 0;
  //pDsp->Vp1->TestDynamicList();
  switch (When)
    {
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 7:
          b999[0] = gs_pCmd->LastToken();
          DoGoto = 1;
          break;
        }
      break;
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          ActivateGWnd();
          pDsp->Vp1->ClearAllEntity();
          SetToolbar(DynTBMarkEntity);
          break;
        case 1 : // Mark the selected entity (not very useful)
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          pGWnd->SetCursor(IDC_SELECTUNIT);
          break;
        case 2 : // IWin
        case 3 : // CWin; Mark everything in the select window (not very useful)
          pGWnd->SetCursor(IDC_SELECTBOX);
          gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1);
          break;
        case 4 : // Clear all selected entities (not very useful)
          pDsp->Vp1->ClearAllEntity();
          break;
        case 5 : // Mark the unit/pipe and all attached units/pipes
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetDigInfo(GC_NoCurs, 1);
          pGWnd->SetCursor(IDC_SELECTUNIT);
          break;
        case 6 : // Mark all tagged inserts
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
          pDrw->SelectInsertsOnAttrCombo(NULL, a999, b999, NULL);
          break;
        case 7 : // Goto a specific tag entered from command window
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetParmInfo(AName, 1);
          break;
        case 8 : // Goto a specific tag, selected from toolbar
          pDsp->Vp1->ClearAllEntity();
          if (pGrfTB && pGrfTB->CurSelLen()>0)
            {
            b999[0] = pGrfTB->CurSel();
            DoGoto = 1;
            GotoFromToolbar = 1;
            }
          break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 :
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          pGWnd->SetCursor();
          break;
        case 2 :
        case 3 :
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          break;
        case 5 :
          {
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
          CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          char* pUnitTag;
          if (p && p->EntityPtr() && (pUnitTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr)))
            {
            RequestModelIOConnRec Info;
            int i = 0;
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            pDsp->Vp1->AddSelectionAttribList(TagAttribStr);

            while (gs_pPrj->RequestModelIOConn(pUnitTag, i++, Info))
              {
              char* a[] = { TagAttribStr, (char*)NULL };
              char* b[] = { Info.Tag(), (char*)NULL };
              pDrw->SelectInsertsOnAttrCombo(NULL, a, b, NULL);
              }
            }
          pGWnd->SetCursor();
          break;
          }
        }
      break;
    case EX_DOIT :
    case EX_UNDO :
      break;
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
    case EX_RESULTS :
      pGWnd->SetCursor();
      RemoveToolbar();
      DumpErrors();
      break;
    }

  if (DoGoto)
    {
    if (!pGWnd->bDonePaint)
      {//drawing has never been painted ie display list has not been built, force it...
      pGWnd->Dsp.Open(NULL);
      CRect ClpBox;
      pGWnd->GetClientRect(&ClpBox);
      pGWnd->Dsp.Paint(ClpBox);
      pGWnd->Dsp.Close();
      pGWnd->bDonePaint = 1;
      }
    if (gs_pPrj->m_bGrfRegExpOn)
      {
      IRegExpPtr *pre= new IRegExpPtr("VBScript.RegExp");
      pDrw->SelectInsertsOnAttrCombo(NULL, a999, b999, pre);
      delete pre;
      }
    else
      pDrw->SelectInsertsOnAttrCombo(NULL, a999, b999, NULL);

    if ((pDsp->Vp1)->FirstSelectedEntity())
      {
      gs_pCmd->SetRetCode(1);
      gs_pCmd->Print("Goto '%s' succeeded\n", b999[0]);
      ActivateGWnd();
      Strng ss;
      CString DocTitle = pGWnd->GetDocument()->GetTitle();
      if (gs_pPrj && (gs_pPrj->m_bGrfPromptForZoom || gs_pPrj->m_bGrfMoveCursor))
        {
        PanAndSetCursor();
        }
      else
        {
        LogNote("GrfCmds", 0, "%s has been found on %s.", b999[0], (const char*)DocTitle);
        }
      if (gs_pPrj->bDoGrfAutoAccess)
        {
        char* pTxt = new char[strlen(b999[0])+1];
        strcpy(pTxt, b999[0]);
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCESS, (LPARAM)pTxt);
        }
      }
    else
      {
      gs_pCmd->Print("Goto '%s' failed\n", b999[0]);
      //AfxMessageBox("Goto has not found the requested Tag.\nTry another drawing.");//, MB_ICONQUESTION);
      if (GotoFromToolbar)
        {
        char* pTxt = new char[strlen(b999[0])+1];
        strcpy(pTxt, b999[0]);
        ScdMainWnd()->PostMessage(WMU_TAGACTION, gs_pPrj->bDoGrfAutoAccess ? SUB_TAG_MARK|SUB_TAG_ACCESS: SUB_TAG_MARK|SUB_TAG_GOTO, (LPARAM)pTxt);
        }
      }
    gs_pPrj->bDoGrfAutoAccess = 0;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoFind()
  {
  //SETVNT("MArk", "ENtity", "@ IWin CWin CLear Attach Tag Goto *", 5, (CmdFn)&GrfCmdBlk::DoSelect, EF_Edit);
  pDsp->Open();
  flag DoGoto = 0;
  flag GotoFromToolbar = 0;
  //pDsp->Vp1->TestDynamicList();
  switch (When)
    {
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 1:
          b999[0] = gs_pCmd->LastToken();
          DoGoto = 1;
          break;
        }
      break;
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          pDsp->Vp1->ClearAllEntity();
//          SetToolbar(DynTBMarkEntity);
          break;
        case 1 : // Goto a specific tag, selected from toolbar
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetParmInfo(AName, 1);
//          pDsp->Vp1->ClearAllEntity();
//          DoGoto = 1;
//          if (pGrfTB && pGrfTB->CurSelLen()>0)
//            {
//            b999[0] = pGrfTB->CurSel();
//            DoGoto = 1;
//            GotoFromToolbar = 1;
//            }
          break;
        }
      break;
    case EX_MOUSE_LUP :
//      switch (Modifier)
//        {
//        }
      break;
    case EX_DOIT :
    case EX_UNDO :
      break;
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
    case EX_RESULTS :
      pGWnd->SetCursor();
//      RemoveToolbar();
      DumpErrors();
      break;
    }

  if (DoGoto)
    {
    if (!pGWnd->bDonePaint)
      {//drawing has never been painted ie display list has not been built, force it...
      pGWnd->Dsp.Open(NULL);
      CRect ClpBox;
      pGWnd->GetClientRect(&ClpBox);
      pGWnd->Dsp.Paint(ClpBox);
      pGWnd->Dsp.Close();
      pGWnd->bDonePaint = 1;
      }
    IRegExpPtr *pRE=(gs_pPrj->m_bGrfRegExpOn) ? new IRegExpPtr("VBScript.RegExp") : NULL;
    pDrw->SelectInsertsOnAttrCombo(NULL, a999, b999, pRE);
    //pDrw->SelectInsertsOnAttrCombo(NULL, a999, b999);
    if ((pDsp->Vp1)->FirstSelectedEntity())
      {
      gs_pCmd->SetRetCode(1);
      gs_pCmd->Print("Goto '%s' succeeded\n", b999[0]);
      ActivateGWnd();
      if (gs_pPrj && (gs_pPrj->m_bGrfPromptForZoom || gs_pPrj->m_bGrfMoveCursor))
        {
        PanAndSetCursor();
        }
      else
        {
        CString DocTitle = pGWnd->GetDocument()->GetTitle();
        LogNote("GrfCmds", 0, "%s has been found on %s.", b999[0], (const char*)DocTitle);
        }
      if (gs_pPrj->bDoGrfAutoAccess)
        {
        char* pTxt = new char[strlen(b999[0])+1];
        strcpy(pTxt, b999[0]);
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCESS, (LPARAM)pTxt);
        }
      }
    else
      {
      gs_pCmd->Print("Goto '%s' failed\n", b999[0]);
      //AfxMessageBox("Goto has not found the requested Tag.\nTry another drawing.");//, MB_ICONQUESTION);
      if (GotoFromToolbar)
        {
        char* pTxt = new char[strlen(b999[0])+1];
        strcpy(pTxt, b999[0]);
        ScdMainWnd()->PostMessage(WMU_TAGACTION, gs_pPrj->bDoGrfAutoAccess ? SUB_TAG_MARK|SUB_TAG_ACCESS: SUB_TAG_MARK|SUB_TAG_GOTO, (LPARAM)pTxt);
        }
      }
    gs_pPrj->bDoGrfAutoAccess = 0;
    delete pRE;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoDump()
  {
  //SETVNT("DUmp", "ENtity", "@ IWin CWin CLear File", 1, (CmdFn)&GrfCmdBlk::DoDump, EF_Edit);
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          pDsp->Vp1->ClearAllEntity();
          SetToolbar(DynTBDumpEntity);
          break;
        case 1 : //Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : //IWin
        case 3 : //CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 4 : //Clear
          pDsp->Vp1->ClearAllEntity();
          break;
        case 5 : //File
          {
          pDsp->Vp1->ClearAllEntity();
          Strng Fn;
          Fn.Set("%ssyscad.dmp", PrjFiles());
          if (pDrw->PrintEntities(pDsp->Vp1, Fn()))
            {
            LogNote("GrfCmds", 0, "All graphics entities dumped in '%s'", Fn());
            gs_pCmd->Print("All graphics entities dumped in '%s'", Fn());
            }
          break;
          }
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      switch (Modifier)
        {
        case 1 :
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          break;
        case 2 :
        case 3 :
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          break;
        }
      break;
      }
    case EX_DOIT :
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      pDrw->DumpSelectedEntities(pDsp->Vp1);
      DumpErrors();
      pGWnd->SetCursor();
      RemoveToolbar();
      break;
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      RemoveToolbar();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoAlign()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :                // good for initialise
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 1 :
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        default :
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      switch (Modifier)
        {
        case ModNoun :
        case 1 :
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          pDsp->Vp1->ClrSelectionAllList();
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        }
      break;
      }
    case EX_EXEC :
    case EX_DOIT :
      pDrw->AlignSelectedInserts(pDsp->Vp1);
      pDsp->Vp1->ClearAllEntity();
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------
//
// Takes two entities and divides the second at the point of intersection or
// closest perpendicular point
//
//---------------------------------------------------------------------------

typedef struct tDivBlk
  {
  C3_CURVE c1;
  C3_CURVE c2;
  C3_CURVE cn1;
  C3_CURVE cn2;
  }
  DivBlk;

void GrfCmdBlk::DoDivide()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :
          DoRestrained = 1;
          break;
        case 1 :
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 3 :
          DoRestrained = 0;
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      switch (Modifier)
        {
        case 1 :
          {
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_LINE);
          pDsp->Vp1->AddSelectionEntityList(DXF_POLYLINE);
          CEntInView* ppp = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          C3_CURVE c = NULL;
          if (ppp)
            c = ppp->CurvePtr();
          if (c)
            {
            flag done = 0;
            DivBlk * db = (DivBlk *) gs_pCmd->GetDataBlk(0);
            if( !db || ( db && db->c1 && db->c2 ))
              {
              DivBlk * DB = new DivBlk;
              DB->c1   = c;
              DB->c2   = NULL;
              DB->cn1  = NULL;
              DB->cn2  = NULL;
              gs_pCmd->SetDataBlk((void *)DB);
              }
            else if (db && db->c1 && db->c2 == NULL)
              {

              if( C3_CURVE_IS_PCURVE(db->c1) || C3_CURVE_IS_PCURVE(c) )
                {
                AfxMessageBox("Divide cannot handle pcurves");
                return;
                }

              if( pDrw->DivideCurve(db->c1,c,db->cn1,db->cn2,1.0))
                {
                CGrfLayer * l = pDrw->FindLayer(c);
                ASSERT(l);
                db->c2 = c;
                l->Add(db->cn1);
                l->Add(db->cn2);
                pDsp->Draw(db->c2, GrfHelper.GR_BACKGROUND);
                pDsp->Draw(db->cn1,GR_GREEN);
                pDsp->Draw(db->cn2,GR_BLUE);
                }
              }
            else
              {
              ASSERT(0);
              }
            }
          break;
          }
        default : ;
        }
      break;
      }
    case EX_DOIT :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        DivBlk * DB = (DivBlk *) (gs_pCmd->GetDataBlk());
        if (pDrw->Exists(DB->c1) && pDrw->Exists(DB->c2) && pDrw->Exists(DB->cn1) && pDrw->Exists(DB->cn2) )
          {
          pDsp->Draw(DB->cn1, GrfHelper.GR_BACKGROUND);
          pDsp->Draw(DB->cn2, GrfHelper.GR_BACKGROUND);
          pDsp->Draw(DB->c2,  GrfHelper.GR_BACKGROUND);
          pDsp->Draw(DB->cn1, -1);
          pDsp->Draw(DB->cn2, -1);
          pDrw->Delete(DB->c2);
          gs_pCmd->SetDataBlk(NULL);
          delete DB;
          }
        }
      break;
    case EX_UNDO :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        DivBlk * DB = (DivBlk *) (gs_pCmd->GetDataBlk());
        if (pDrw->Exists(DB->c1) && pDrw->Exists(DB->c2) && pDrw->Exists(DB->cn1) && pDrw->Exists(DB->cn2) )
          {
          pDsp->Draw(DB->cn1, GrfHelper.GR_BACKGROUND);
          pDsp->Draw(DB->cn2, GrfHelper.GR_BACKGROUND);
          pDsp->Draw(DB->c2,  GrfHelper.GR_BACKGROUND);
          pDsp->Draw(DB->c2,  -1);
          pDrw->Delete(DB->cn1);
          pDrw->Delete(DB->cn2);
          gs_pCmd->SetDataBlk(NULL);
          delete DB;
          }
        }
    case EX_RESULTS :
      DumpErrors();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------
//
// Takes an insert and explodes it into the drawing.Any related flowsheet
// unit or pipe will be deleted
//
//---------------------------------------------------------------------------

struct tExpBlkEntry
  {
  DXF_ENTITY e;
  char Tag[SCDFullTagLen+1];
  };

typedef CArray <tExpBlkEntry, tExpBlkEntry&> ExpBlk;

void GrfCmdBlk::DoExplode()
  {
  //SETVNT("EXplode",   "UNit",   "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoExplode, EF_Edit);
  //SETVNT("EXplode",   "SYmbol", "@ IWin CWin Redraw ; Tag", 1, (CmdFn)&GrfCmdBlk::DoExplode, EF_Edit);
  enum MId { MID_Dig=1, MID_IWin, MID_CWin, MID_Redraw, MID_Next, MID_Tag };
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  flag DoMdl = (Noun==1); //DoMdl = true for "Explode Unit"
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          //SetToolbar(DoMdl ? DynTBExplode : DynTBExplodeSymb); TODO
          SetToolbar(DynTBExplode);
          pDsp->Vp1->ClearAllEntity();
          bFlag2 = 0;  //set true to explode Tag Attribute only
          bFlag3 = 0;  //set true if CalcUnits must be called
          DoRestrained = 1;
          break;
        case MID_Dig :
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case MID_IWin :
        case MID_CWin :
          gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case MID_Redraw :
          pWnd->Invalidate(True);
          break;
        case MID_Tag :
          if (!DoMdl)
            {
            bFlag2 = !bFlag2;
            //if (pGrfTB) TODO
            //  pGrfTB->SetTextButtonText(bFlag2 ? "Block" : "TagOnly", 0);
            }
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      flag DoIt = 0;
      switch (Modifier)
        {
        case MID_Dig :
          {
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          DoIt = 1;
          const bool AllowUnSelect = false;
          if (AllowUnSelect)
            {
            BOOL AlreadySelected;
            CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
            if (p && AlreadySelected)
              {
              BOOL Ok = true;
              if (DoMdl && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
                {
                pchar pTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr);
                if (pTag)
                  {
                  RequestModelIOConnRec Info;
                  int i = 0;
                  while (gs_pPrj->RequestModelIOConn(pTag, i++, Info) && Ok)
                    {
                    if (strcmp(Info.Group(), FlwLinkGrp)==0 || strcmp(Info.Group(), CtrlLinkGrp)==0)
                      Ok = false; //cannot unselect model with links!!!
                    }
                  }
                //if (pTag && !LinkEntity(pTag))
                //  Ok = flase; //cannot unselect model with links!!!
                }
              if (Ok)
                {
                pDsp->Vp1->DeSelectEntity(p);
                //TODO : Need to remove entity from db list !!!
                ASSERT_ALWAYS(FALSE, "not implemented...", __FILE__, __LINE__);
                DoIt = 0;
                }
              }
            }
          else
            pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          break;
          }
        case MID_IWin :
        case MID_CWin :
          {
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          DoIt = 1;
          break;
          }
        }
      int jj = 0;
      while (DoIt && jj<2)
        {
        //build list of entities to be exploded,
        //do this twice, second parse picks up attached links...
        CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
        while (p)
          {
          DXF_ENTITY e = p->EntityPtr();
          if (e)
             {
            //int ok = 0;
            pchar pTag = Find_Attr_Value(e, TagAttribStr);
            if (pTag)
              {
              if (Find_Attr_Value(e, AssocTagAttribStr)!=NULL) // Is AssocGrf
                {
                pDsp->Vp1->DeSelectEntity(p);
                e=NULL;
                LogNote(pTag, 0, "Cannot select Associated Graphics for Explode");
                }
              }
            //if (pTag)
            //  {
            //  ok=1;
            //  //char msg[1024];
            //  //if (DoMdl)
            //  //  sprintf(msg,"Exploding this block will cause model %s to be deleted.\nContinue?",pTag);
            //  //else
            //  //  sprintf(msg,"Exploding this block may cause model %s to be inaccessible.\nContinue?",pTag);
            //  //if (AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION)==IDYES)
            //  //  ok = 1;
            //  //else
            //  //  pDsp->Draw(e,-1);
            //  }
            //else
            //  ok = 0; // CNM was 1 but crashes
            // Already added

            ExpBlk* db = (ExpBlk*)gs_pCmd->GetDataBlk();
            if (e && db)
              {
              ExpBlk& DB = *db;
              for (int i=0; e && (i<DB.GetSize()); i++)
                {
                if (DB[i].e == e)
                  e = NULL;
                }
              }

            if (e)
              {
              tExpBlkEntry E;
              E.e = e;
              if (pTag)
                {
                strcpy(E.Tag, pTag);
                //add all attached links...
                if (jj==0 && DoMdl && DXF_ENTITY_IS_INSERT(e))
                  {
                  RequestModelIOConnRec Info;
                  pDsp->Vp1->ClrSelectionAllList();
                  pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
                  pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
                  int i = 0;
                  while (gs_pPrj->RequestModelIOConn(pTag, i++, Info))
                    {
                    if (strcmp(Info.Group(), FlwLinkGrp)==0 || strcmp(Info.Group(), CtrlLinkGrp)==0)
                      {
                      char *a[] = { TagAttribStr,    (pchar)NULL };
                      char *b[] = { Info.Tag(), (pchar)NULL };
                      pDrw->SelectInsertsOnAttrCombo(NULL, a, b, NULL);
                      }
                    }
                  }
                }
              else
                strcpy(E.Tag, "");
              if (!db)
                {
                db = new ExpBlk;
                gs_pCmd->SetDataBlk((void*)db);
                }
              db->Add(E);
              }
            }
          p = pDsp->Vp1->NextSelectedEntity();
          }
        jj++;
        if (!DoMdl)
          jj++;
        }
      break;
      }
    case EX_TERM_SEMICOLON :
      if (gs_pCmd->IsEscBusy())
        break;
    case EX_DOIT :
    //case EX_EXEC :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        CWaitMsgCursor Wait("Exploding graphics entities");
        ExpBlk* db = (ExpBlk*)(gs_pCmd->GetDataBlk());
        if (db)
          {
          ExpBlk & DB = *db;
          Strng_List DelTags;
          CArray<long, long> DBIndices;
          for (int i=0; i < DB.GetSize(); i++)
            {
            dbgpln("Delete %8x", DB[i].e);
            if (strlen(DB[i].Tag))
              {
              dbgpln("Delete %s", DB[i].Tag);
              DelTags.Append(DB[i].Tag);
              DBIndices.Add(i);
              if (!DoMdl)
                DelTags.Last()->SetIndex(EODT_DONE);
              }
            else if (pDrw->Exists(DB[i].e))
              {
              CEntInView* pE = pDrw->FindEntInView(DB[i].e);
              if (pE)
                pE->ClrMarkBit();
              //TODO : if (bFlag2) Explode Tag Attribute Only!
              pDsp->Draw(DB[i].e, GrfHelper.GR_BACKGROUND);
              pDrw->Explode(DB[i].e);
              }
            }

          if (DelTags.Length()>0)
            {
            int iDBIndex=0;
            CMdlValueSet::Clear();
            int RetCode = (DoMdl ? gs_Exec.DeleteTags(DelTags) : EODT_DONE);
            for (Strng * pTag=DelTags.First(); pTag; pTag=DelTags.Next())
              {
              int i=DBIndices[iDBIndex++];
              switch (pTag->Index())
                {
                case EODT_FAILED:
                  if (DoMdl)
                    LogError(pTag->Str(), 0, "Model not deleted");
                  break;
                case EODT_DONE:
                  {
                  if (DoMdl)
                    bFlag3 = 1;
                  if (pDrw->Exists(DB[i].e))
                    {
                    CEntInView* pE = pDrw->FindEntInView(DB[i].e);
                    if (pE)
                      pE->ClrMarkBit();

                    //TODO : if (bFlag2) Explode Tag Attribute Only!
                    pDsp->Draw(DB[i].e, GrfHelper.GR_BACKGROUND);
                    pDrw->Explode(DB[i].e);
                    }
                  break;
                  }
                }
              }
            }
          gs_pCmd->SetDataBlk(NULL);
          delete db;
          }
        pDsp->Vp1->ClearAllEntity();
        }
      break;
    case EX_UNDO :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        ExpBlk* db = (ExpBlk*)(gs_pCmd->GetDataBlk());
        if (db)
          {
          ExpBlk & DB = *db;
          for (int i=0; i < DB.GetSize(); i++)
            if (DB[i].e)
              {
              CEntInView* pE = pDrw->FindEntInView(DB[i].e);
              if (pE)
                pE->ClrMarkBit();
              pDsp->Draw(DB[i].e, -1);       //GR_WHITE);
              }
          gs_pCmd->SetDataBlk(NULL);
          delete db;
          }
        }
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------
//
// Takes an insert and explodes it into the drawing.Any related flowsheet
// unit or pipe will be deleted
//
//---------------------------------------------------------------------------
#define UseDoAttach_ 0

void GrfCmdBlk::DoAttach()
  {
  #if UseDoAttach_
  //static pchar tag = (pchar) NULL;
  //static pchar Unit_id = (pchar) NULL;
  //static pchar model = (pchar) NULL;
  enum MID { MID_Dig=1, MID_IWin, MID_CWin, MID_Tag, MID_Eqp,
             MID_Symbol, MID_Model, MID_Dlg, MID_SELTag, MID_SELEqp, MID_CTag};
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
        {
        switch (Modifier)
          {
          case ModNoun :                // good for initialise
            ActivateGWnd();
            if (!pMdl)
              {
              gs_pCmd->ExtendCmdLine("\x1b");
              LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
              }
            else
              {
              TRACE0(" Clearing Selection Bits\n");
              pDsp->Vp1->ClearAllEntity();
              ATag = "";
              AUnitId = "";
              ASymbol  = "";
              AClass = "";
              ATagSlct=1;
              AEqpSlct=0;
              nTxtEnt=0;
              pTxtEnt[0]=NULL;
              pTxtEnt[1]=NULL;
              nEntSelects=0;
              bCTagPtSet=0;
              HideTagOff=False;

              gs_pCmd->ExtendCmdLine("CTag ");
              }
            break;
          case MID_Dig    : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTGRF); break;
          case MID_IWin   : gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1); pGWnd->SetCursor(IDC_SELECTBOXGRF); break;
          case MID_CWin   : gs_pCmd->SetDigInfo(GC_NoCurs | GC_RectCurs, 1); pGWnd->SetCursor(IDC_SELECTBOXGRF); break;
          case MID_CTag   : gs_pCmd->SetDigInfo(GC_NoCurs, 1); pGWnd->SetCursor(IDC_SELECTTAG); break;
          case MID_Tag    : gs_pCmd->SetParmInfo(AName, 1); ATag = ""; break;
          case MID_Eqp    : gs_pCmd->SetParmInfo(AName, 1); AUnitId = ""; break;
          case MID_Symbol : gs_pCmd->SetParmInfo(AName, 1); ASymbol = ""; break;
          case MID_Model  : gs_pCmd->SetParmInfo(AName, 1); AClass = ""; break;
          case MID_Dlg    : pMdlSlct = new CMdlSlct(pMdl, FlwUnitGrp, False, True, True, "", "", "", "", HideTag, HideEqpId, AfxGetMainWnd()); break;
          case MID_SELTag :
          case MID_SELEqp : gs_pCmd->SetParmInfo(AnInt, 1); break;
          default :
            break;
          }
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Tag    : ATag = gs_pCmd->LastToken(); break;
        case MID_Eqp    : AUnitId = gs_pCmd->LastToken(); break;
        case MID_Symbol : ASymbol = gs_pCmd->LastToken(); break;
        case MID_Model  : AClass = gs_pCmd->LastToken(); break;
        case MID_SELTag : gs_pCmd->iParm(ATagSlct); break;
        case MID_SELEqp : gs_pCmd->iParm(AEqpSlct); break;
        default : break;
        }
    case EX_MOUSE_LUP :
        {
        CWaitCursor Wait;
        switch (Modifier)
          {
          case MID_Dig :
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            pGWnd->SetCursor(IDC_SELECTGRF);
            nEntSelects++;
            break;
          case MID_IWin :
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
            //gs_pCmd->SetDigInfo(GC_BigCurs | GC_RectCurs, 1);
            pGWnd->SetCursor(IDC_SELECTGRF);
            nEntSelects++;
            if (pMdlSlct)
              pMdlSlct->SetIWin(False);
            break;
          case MID_CWin :
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
            //gs_pCmd->SetDigInfo(GC_BigCurs | GC_RectCurs, 1);
            pGWnd->SetCursor(IDC_SELECTGRF);
            nEntSelects++;
            if (pMdlSlct)
              pMdlSlct->SetCWin(False);
            break;
          case MID_CTag:
            {
            //CTagPt=pDsp->CurrentPt;
            //mhmstart
            pDsp->Vp1->ClrSelectionAllList();
            CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            if( p && p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
              {
              bCTagPtSet=1;
              CTagPt.World.Set(
                        DXF_TEXT_INS_PT(p->EntityPtr())[0],
                        DXF_TEXT_INS_PT(p->EntityPtr())[1],
                        DXF_TEXT_INS_PT(p->EntityPtr())[2]
                        );
              }

            // mhmend
            pGWnd->SetCursor(IDC_SELECTGRF);
            }
            break;
          default :
            break;
          }
        switch (Modifier)
          {
          case MID_Dig  :
          case MID_IWin :
          case MID_CWin :
          case MID_CTag :  //mhmchange
            {
            if (pMdlSlct)
              {
              pMdlSlct->UpdateData(True);
              ATagSlct=pMdlSlct->m_TagSlct;
              AEqpSlct=pMdlSlct->m_EqpSlct;
              }
            int Cnt=0;
            CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
            while (p && nTxtEnt<2)
              {
              if (p->EntityPtr() && DXF_ENTITY_IS_TEXT(p->EntityPtr()))
                {
                flag Try=1;
                for (int i=0; i<nTxtEnt; i++)
                  if (p==pTxtEnt[i])
                    Try=0;
                if (Try)
                  {
                  char ca[1024];
                  MakeATag(ca, sizeof(ca), (pchar)DXF_TEXT_VALUE_GET(p->EntityPtr()));

                  pTxtEnt[nTxtEnt]=p;
                  nTxtEnt++;
  
                  xxATag = ca;
                  xxCheckTheTag(ATag, AClass(), TagBase.Length() == 0 ? AClass() : TagBase(), true);

                  if (nTxtEnt==ATagSlct)
                    if (pMdlSlct)
                      {
                      Strng UniqueTag, ATag(ca);
                      if (!gs_pPrj->TestModelTagUnique(ATag(), ATag(), '/', UniqueTag, True))
                        {
                        //Strng Msg;
                        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Duplicate Tag:\n%s\nChanged to:\n%s", ATag(), UniqueTag());
                        //LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, Msg());
                        //AfxMessageBox(Msg(), MB_ICONEXCLAMATION);
                        ATag=UniqueTag();
                        }
                      pMdlSlct->SetTag(ATag());
                      HideTagOff=True;
                      }
                    else
                      ATag=ca;
                  if (nTxtEnt==AEqpSlct)
                    if (pMdlSlct)
                      pMdlSlct->SetUnitId(ca);
                    else
                      AUnitId=ca;
                  if (nTxtEnt==1)
                    {
                    text_info Ti;
                    //,DML_LIST stylelst,DXF_ENTITY e)
                    text_info_from_dxf_text(&Ti,pDrw->StyleLst,p->EntityPtr());
                    // Since these origins are already aligned we must have no justifying
                    Attr_Settings TSet(50,
                                       Ti.flags | Ti.on ? 0 : DXF_ATTRIB_INVIS,
                                       Ti.height,
                                       Ti.wfactor,
                                       Rads2Degs(Ti.angle),
                                       GR_DARKGRAY,
                                       Text_Left,Text_Baseline,
                                       Ti.style);

                    Tag_Attr_Set=TSet;
                    }
                  }
                }
              p = pDsp->Vp1->NextSelectedEntity();
              }
            }
          }
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      //pDrw->PrintEntities(pDsp->Vp1,"Before.ent");
      if (nEntSelects==0)
        {
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "No entities Selected");
        break;
        }

      if (pMdlSlct)
        {
        pMdlSlct->UpdateData(True);
        ATag=pMdlSlct->m_Tag.GetBuffer(0);
        AClass=pMdlSlct->m_ModelName();
        ASymbol=pMdlSlct->m_Symbol.GetBuffer(0);
        AUnitId=pMdlSlct->m_UnitId.GetBuffer(0);
        TagBase=pMdlSlct->m_BaseTag();
        HideTag=pMdlSlct->m_HideTag;
        HideEqpId=pMdlSlct->m_HideEqpId;
        TaggedObject::ValidateTagDot(ATag);
        TaggedObject::ValidateTag(AClass);
        TaggedObject::ValidateTag(ASymbol);
        TaggedObject::ValidateTag(AUnitId);
        }
      if (ATag.Length()>0 && AClass.Length()>0)
        {
        Strng UniqueTag;
        if (!gs_pPrj->TestModelTagUnique(ATag(), TagBase.Length() == 0 ? AClass() : TagBase() , '_', UniqueTag, False))
          ATag=UniqueTag();
        if (AUnitId.Length()==0)
          AUnitId="";
        if (ASymbol.Length()==0)
          ASymbol = AClass.GetLength()>0?"EQP":AClass();

        DXF_ENTITY pEnt;
        TRACE("CTAG %g %g %g", CTagPt.World.X, CTagPt.World.Y, CTagPt.World.Z);
        Tag_Attr_Set.Flags=HideTag && !HideTagOff ? DXF_ATTRIB_INVIS : 0;
        if ((pEnt=pDrw->Add_Unit(pDsp->Vp1, CTagPt.World, bCTagPtSet, AUnitId(), ATag(), ASymbol(), AClass(), Tag_Attr_Set))!=NULL)
          {
          pDsp->Draw(pEnt, -1);
          AddUnitModel(AClass(), ATag());
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit Not constructed");
        }
      else
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Tag & Model must be specified");
        //pDrw->PrintEntities(pDsp->Vp1,"After.ent");
      pDsp->Vp1->ClrSelectionAllList();
      pDsp->Vp1->ClearAllEntity();
      break;
    case EX_UNDO :
      if (pMdlSlct)
        {
        pMdlSlct->SetIWin(False);
        pMdlSlct->SetCWin(False);
        }
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      if (pMdlSlct)
        {
        //bWPMdlSlctOk=pMdlSlct->GetWindowPlacement(&WPMdlSlct);
        pMdlSlct->DestroyWindow();
        delete pMdlSlct;
        pMdlSlct=NULL;
        }
      break;
    }
  pDsp->Close();
  #endif
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoHelpUnit()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          pDsp->Vp1->ClearAllEntity();
          pGWnd->SetCursor(IDC_HELPCURS);
          SetToolbar(DynTBHelp);
          break;
        case 1 :                     // Dig
          pDsp->Vp1->ClearAllEntity();
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        default :;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      switch (Modifier)
        {
        case 1 :
          {
          pDsp->Vp1->ClrSelectionAllList();
          CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          pchar pTag;

          if (p && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && (pTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr)))
            {
            char* pTxt = new char[strlen(pTag)+1];
            strcpy(pTxt, pTag);
            ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_MDLHELP, (LPARAM)pTxt);

            /*if (pMdl)
              {
              CXM_ObjectTag ObjTag(pTag, TABOpt_Parms);//TABOpt_AllInfoOnce);
              CXM_ObjectData ObjData;
              CXM_Route Route;
              if (XReadTaggedItem(ObjTag, ObjData, Route))
                {
                CPkDataItem * pItem = ObjData.FirstItem();
                Strng Class;
                Class = pItem->Class();
                pTagObjClass pTagObj = TagObjClass::FindClassId(Class());
                if (pTagObj==NULL)
                  pTagObj = TagObjClass::FindClassName(Class());
                if (pTagObj)
                  Class = pTagObj->ShortDesc();
                //pchar pMdlDLL=pMdl->RequestModelDLLPath();
                //if (pMdlDLL)
                //  {
                //  Strng HlpFile(pMdlDLL);
                //  int l=HlpFile.Length();
                //  if (_stricmp(&HlpFile[l-4], ".dll")==0)
                //    HlpFile.SetLength(l-4);
                //  HlpFile+=".hlp";
                //  ::WinHelp(pWnd->m_hWnd, HlpFile(), HELP_PARTIALKEY, (UINT)Class());
                //  }
                gs_pPrj->ModelHelp(Class());
                }
              }*/
            pDsp->Vp1->ClrSelectionAllList();
            }
          break;
          }
        default :;
        }
      break;
      }
    case EX_EXEC :
      break;
    case EX_UNDO :
      pDsp->Vp1->ClearAllEntity();
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      RemoveToolbar();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoDelete()
  {
  //SETVNT("DElete",    "UNit", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  //SETVNT("DElete",    "SYmbol", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  flag DelMdl = (Noun==1);
  flag DelSym = (Noun==1) || (Noun==2);
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :
          {
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          SetToolbar(DynTBDelete);
          pDsp->Vp1->ClearAllEntity();
          DoRestrained = 1;
          break;
          }
        case 1 : //Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : //IWin
        case 3 : //CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 4 : //Redraw
          pWnd->Invalidate(True);
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      bFlag1 = true;
      switch (Modifier)
        {
        case 1 :
          {
          pDsp->Vp1->ClrSelectionAllList();
          const bool AllowUnSelect = false;
          //kga 12/7/00 : set AllowUnSelect to false for now because,
          //may need improving and similar functionality is required in Explode!
          if (AllowUnSelect)
            {
            BOOL AlreadySelected;
            CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
            if (p)
              {
              if (AlreadySelected)
                {
                BOOL Ok = true;
                if (DelMdl && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
                  {
                  pchar pTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr);
                  if (pTag)
                    {
                    RequestModelIOConnRec Info;
                    int i = 0;
                    while (gs_pPrj->RequestModelIOConn(pTag, i++, Info) && Ok)
                      {
                      if (strcmp(Info.Group(), FlwLinkGrp)==0 || strcmp(Info.Group(), CtrlLinkGrp)==0)
                        Ok = false; //cannot unselect model with links!!!
                      }
                    }
                  //if (pTag && !LinkEntity(pTag))
                  //  Ok = flase; //cannot unselect model with links!!!
                  }
                if (Ok)
                  {
                  pDsp->Vp1->DeSelectEntity(p);
                  bFlag1 = false;
                  }
                }
              }
            }
          else
            pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          break;
          }
        case 2 :
        case 3 :
          {
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          break;
          }
        }

      if (DelMdl && bFlag1)
        {
        CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
        while (p)
          {
          pchar pTag;
          if (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && (pTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr)))
            {
            pchar pAssocTag = Find_Attr_Value(p->EntityPtr(), AssocTagAttribStr);
            if (pAssocTag==NULL) // cannot select 
              {
              RequestModelIOConnRec Info;
              int i = 0;
              pDsp->Vp1->ClrSelectionAllList();
              pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
              pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
              while (gs_pPrj->RequestModelIOConn(pTag, i++, Info))
                {
                if (!Info.fIsDirectConnect && (strcmp(Info.Group(), FlwLinkGrp)==0 || strcmp(Info.Group(), CtrlLinkGrp)==0))
                  {
                  char *a[] = { TagAttribStr,    (pchar)NULL };
                  char *b[] = { Info.Tag(), (pchar)NULL };
                  pDrw->SelectInsertsOnAttrCombo(NULL, a, b, NULL);
                  }
                }
              }
            else
              {
              pDsp->Vp1->DeSelectEntity(p);
              LogNote(pTag, 0, "Cannot select Associated Graphics for Delete");
              }
            }
          p = pDsp->Vp1->NextSelectedEntity();
          }
        }
      break;
      }
    case EX_TERM_SEMICOLON :
      if (gs_pCmd->IsEscBusy())
        break;
    case EX_EXEC :
      {
      CWaitMsgCursor Wait("Deleting entities");
      int DeletesFailedCnt = 0;
      int MdlDeletes = 0;

      Strng_List DelTags;
      CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
      while (pEnt)
        {
        DXF_ENTITY e = pEnt->EntityPtr();
        C3_CURVE   c = pEnt->CurvePtr();
        if (DelSym && c && pDrw->Exists(c))
          {
          pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(c);
          }
        if (e && pDrw->Exists(e))
          {
#if SYSCAD10         
          if (gs_pPrj->SvcActive)
            {
            if (DelMdl)
              { 
              LPCSTR pGrfGuid= Scd10GetGrfGuid(e);
              //    if (pGrfGuid)

              //LPCSTR pTag = Scd10GetTag(e);
              if (pGrfGuid)
                {
                CMdlValueSet::Clear();
                SCD10ENTER;

                LPCTSTR InsName=Scd10GetInsertName(e);
                if (InsName && strstr(InsName, "LNKBLK"))
                  gs_pPrj->Svc.GCBDeleteLink(e, pGrfGuid);
                else
                  gs_pPrj->Svc.GCBDeleteNode(e, pGrfGuid);
                SCD10LEAVE;
                }
              }
            }
          else
#endif
            {
            pchar pTag;
            if (DelMdl)
              {
              if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
                {
                CMdlValueSet::Clear();

                if (Find_Attr_Value(e, AssocTagAttribStr)==NULL) // is not an AssocTag
                  {
                  DelTags.Append(pTag);
                  //int RetCode = gs_Exec.DeleteTag(pTag);
                  //if (RetCode!=EODT_DONE)
                  //  {
                  //  LogError(pTag, 0, "Model not deleted");
                  //  DeletesFailedCnt++;
                  //  }
                  //else
                  //  MdlDeletes++;
                  }
                }
              }
            if (DelSym)
              {
              pDsp->Draw(e, GrfHelper.GR_BACKGROUND);
              pDrw->Delete(e);
              }
            }
          }
        pEnt = pDsp->Vp1->NextSelectedEntity();
        }
      if (DelTags.Length()>0)
        {
        int RetCode = gs_Exec.DeleteTags(DelTags);
        for (Strng * pTag=DelTags.First(); pTag; pTag=DelTags.Next())
          {
          switch (pTag->Index())
            {
            case EODT_FAILED:
              LogError(pTag->Str(), 0, "Model not deleted");
              DeletesFailedCnt++;
              break;
            case EODT_DONE:
              MdlDeletes++;
              break;
            }
          }
        }
      if (DeletesFailedCnt)
        {
        if (DeletesFailedCnt==1)
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "A model was not deleted");
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "%d models were not deleted.", DeletesFailedCnt);
        }
      pDsp->Vp1->ClearAllEntity();
      break;
      }
    case EX_UNDO :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      DumpErrors();
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoBulkTagChange()
  {
  //SETVNT("DElete",    "UNit", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  //SETVNT("DElete",    "SYmbol", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  flag DelMdl = (Noun==1);
  flag DelSym = (Noun==1) || (Noun==2);
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :
          {
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          SetToolbar(DynTBTagChange);
          pDsp->Vp1->ClearAllEntity();
          DoRestrained = 1;
          break;
          }
        case 1 : //Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : //IWin
        case 3 : //CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 4 : //Redraw
          pWnd->Invalidate(True);
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      bFlag1 = true;
      switch (Modifier)
        {
        case 1 :
          {
          pDsp->Vp1->ClrSelectionAllList();
          const bool AllowUnSelect = false;
          //kga 12/7/00 : set AllowUnSelect to false for now because,
          //may need improving and similar functionality is required in Explode!
          if (AllowUnSelect)
            {
            BOOL AlreadySelected;
            CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
            if (p && AlreadySelected)
              {
              BOOL Ok = true;
              if (Ok)
                {
                pDsp->Vp1->DeSelectEntity(p);
                bFlag1 = false;
                }
              }
            }
          else
            pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          break;
          }
        case 2 :
        case 3 :
          {
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          break;
          }
        }

      break;
      }
    case EX_TERM_SEMICOLON :
      if (gs_pCmd->IsEscBusy())
        break;
    case EX_EXEC :
      {
      CWaitMsgCursor Wait("Deleting entities");
      int DeletesFailedCnt = 0;
      int MdlDeletes = 0;
      CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
      CStringList Tags;
      while (pEnt)
        {
        DXF_ENTITY e = pEnt->EntityPtr();
        C3_CURVE   c = pEnt->CurvePtr();
        if (DelSym && c && pDrw->Exists(c))
          {
          pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(c);
          }
        if (e && pDrw->Exists(e))
          {
          pchar pTag;
          if (DelMdl)
            {
            if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
              {
              CMdlValueSet::Clear();
              dbgpln("ChangeTag %s", pTag);
              Tags.AddTail(pTag);
              }
            }
          }
        pEnt = pDsp->Vp1->NextSelectedEntity();
        }
  
      RemoveToolbar();

      CBulkTagChange Dlg(&Tags, NULL, false, pGWnd);
      if (Dlg.DoModal())
        {
        }
      pDsp->Vp1->ClearAllEntity();
      break;
      }
    case EX_UNDO :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      DumpErrors();
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoInsertGroup()
  {
  static CNeutralInsertGroupDlg * pGroupDlg = NULL;
  if (gs_Exec.Busy())
    return;
  CInsertBlk* CB = (CInsertBlk*)(gs_pCmd->GetDataBlk());
  flag DoIt = 0;
  pDsp->Open();
  enum MId { MID_Dig=1, /*MID_Tag, MID_BaseTag,*/
             MID_Scale, MID_XScale, MID_YScale, MID_Rotate,
             MID_XY, MID_X, MID_Y, /*MID_Model,*/ MID_Dlg };
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          bFlag3 = 0;  //set true if CalcUnits must be called
          if (!pMdl)
            {
            LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
            gs_pCmd->ExtendCmdLine("\x1b");
            }
          else
            {
            Pt1.Zero();
            Pt2.Zero();
            ATag = "";
            AClass = "";
            ASymbol = "";
            HideTagOff=False;
            m_sGroupName="";
            m_sGroupLibrary="";
            m_sString1="";
            m_sString2="";
            m_pOldTags=NULL;
            m_pNewTags=NULL;
            }
          break;
        case MID_Dig : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        //case MID_Tag : gs_pCmd->SetParmInfo(AName, 1); break;
        //case MID_BaseTag : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_Scale : gs_pCmd->SetParmInfo(AFloat,  2); break;
        case MID_XScale : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_YScale : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Rotate : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_XY     : gs_pCmd->SetParmInfo(AFloat, 2); break;
        case MID_X      : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Y      : gs_pCmd->SetParmInfo(AFloat, 1); break;
        //case MID_Model  : gs_pCmd->SetParmInfo(AName, 1); break;
        case MID_Dlg    : pGroupDlg = new CNeutralInsertGroupDlg(AfxGetMainWnd());
                          //(CDlgWhat_InsUnit, false, pMdl, AfxGetMainWnd()); break;
        default :;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        //case MID_Tag     : ATag = gs_pCmd->LastToken(); break;
        //case MID_BaseTag : TagBase = gs_pCmd->LastToken(); break;
        case MID_Scale   : gs_pCmd->dParm(NdScl[gs_pCmd->ParmNumber()]); break;
        case MID_XScale  : gs_pCmd->dParm(NdScl.X); break;
        case MID_YScale  : gs_pCmd->dParm(NdScl.Y); break;
        case MID_Rotate  : gs_pCmd->fParm(Rotate); break;
        case MID_XY      : gs_pCmd->dParm(Pt1.World[gs_pCmd->ParmNumber()]); break;
        case MID_X       : gs_pCmd->dParm(Pt1.World.X); gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Y       : gs_pCmd->dParm(Pt1.World.Y); gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Dlg     : AClass = gs_pCmd->LastToken(); break;
        default :;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case MID_Dig :
        case MID_X   :
        case MID_Y   :
          {
          switch (Modifier)
            {
            case MID_Dig : Pt1 = pDsp->CurrentPt; break;
            case MID_X   : Pt1.GetY(pDsp->CurrentPt); break;
            case MID_Y   : Pt1.GetX(pDsp->CurrentPt); break;
            }
          CB = new CInsertBlk;
          gs_pCmd->SetDataBlk((void *) CB);

          bool AllOK=true;
          if (pGroupDlg)
            {
            AllOK=pGroupDlg->CompleteForUse();
            m_iFixupRule      = pGroupDlg->GetFixupRule();
            m_sString1        = pGroupDlg->GetString1();
            m_sString2        = pGroupDlg->GetString2();
            m_pOldTags        = pGroupDlg->GetOldTags();
            m_pNewTags        = pGroupDlg->GetNewTags();
            m_sGroupLibrary   = pGroupDlg->GetDatabaseName();
            m_sGroupName      = pGroupDlg->GetGroup();
            }
          CB->m_sGroupLibrary = m_sGroupLibrary;
          CB->m_sGroupName    = m_sGroupName;
          CB->m_iFixupRule    = m_iFixupRule;
          CB->m_sString1      = m_sString1;
          CB->m_sString2      = m_sString2;
          CB->m_pOldTags      = m_pOldTags;
          CB->m_pNewTags      = m_pNewTags;

          CB->ATag = ATag;
          CB->AClass = AClass;
          CB->ASymbol = ASymbol;
          CB->ATagBase = TagBase;
          CB->Pt = Pt1;
          CB->GrpScl = GrpScl;
          //CB->Scl.X = Max(0.001, CB->Scl.X);
          //CB->Scl.Y = Max(0.001, CB->Scl.Y);
          //CB->Scl.Z = Max(0.001, CB->Scl.Z);
          CB->GrpScl.Z = CB->GrpScl.X; //make Z scale same as X
          CB->Rotate = Range(-360.0F, Rotate, 360.0F);
          CB->MdlInsertErr = -1;

          if (AllOK && CB->m_sGroupName.GetLength()>0)
            {
            pDsp->Show_Dig_Point(Pt1);

            //CDocTemplate & GT = ScdApp()->GraphTemplate();
            CNeutralImportExport NImport;
            NImport.SetImportOrigin(CDblTriPt(CB->Pt.World.X, CB->Pt.World.Y, CB->Pt.World.Z), CDblTriPt(CB->GrpScl.X, CB->GrpScl.Y, CB->GrpScl.Z));
            NImport.SetImportFilter(CB->m_sGroupName());
            NImport.SetImportTagFixups(eFixup_ExchangeTags, "", "", CB->m_pOldTags, CB->m_pNewTags);
            NImport.DoImportGroup(eNDB_StdImport, dynamic_cast<CGrfDoc*>(pDoc), CB->m_sGroupLibrary(), CB->m_sGroupLibrary());
            }
          if (pGroupDlg)
            pGroupDlg->ResetAfterImport();

          ATag="";
          break;
          }
        default :;
        }
      if (pGroupDlg)
        pGroupDlg->SetFocus();
      break;
    case EX_EXEC :
      break;
    case EX_DOIT :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        delete CB;
        gs_pCmd->SetDataBlk(NULL);
        }
      break;
    case EX_UNDO :
      if (gs_pCmd->OwnerOfDataBlk())
        {
        if (pDrw->Exists(CB->e))
          {
          pDsp->Draw(CB->e, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(CB->e);
          }

        dbgpln("Delete %s", CB->ATag);
        if (CB->MdlInsertErr==0 && pMdl!=NULL)
          {
          CMdlValueSet::Clear();
          int RetCode = gs_Exec.DeleteTags(Strng_List(CB->ATag()));
          if (RetCode!=EODT_DONE)
            LogError(CB->ATag(), LF_DoAfxMsgBox|LF_Exclamation, "Model not deleted");
          else
            bFlag3 = 1;
          /*int err = pMdl->DeleteNodeModel(CB->ATag());
          if (err)
            {
            //Strng Msg;
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model not deleted[%i]\n%s", err, CB->ATag());
            //LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, Msg());
            //AfxMessageBox(Msg(), MB_ICONEXCLAMATION);
            LogError(CB->ATag(), 0, "GrfCmds: Model not deleted[%i]", err);
            }
          else
            bFlag3 = 1;*/
          }
        delete CB;
        gs_pCmd->SetDataBlk(NULL);
        }
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pGWnd->SetCursor();
      if (pGroupDlg)
        {
        pGroupDlg->DestroyWindow();
        delete pGroupDlg;
        pGroupDlg = NULL;
        }
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }

  pDsp->Close();
  };

//---------------------------------------------------------------------------

void GrfCmdBlk::DoSaveGroup()
  {
#if WITHGRFGROUP
  static CNeutralExportDBDlg * pDBDlg = NULL;
  //SETVNT("DElete",    "UNit", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  //SETVNT("DElete",    "SYmbol", "@ IWin CWin Redraw ;", 1, (CmdFn)&GrfCmdBlk::DoDelete, EF_Edit);
  if (gs_Exec.Busy())
    return;
  pDsp->Open();
  flag SaveMdl = 1;//(Noun==1);
  flag SaveSym = 1;//(Noun==1) || (Noun==2);
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      switch (Modifier)
        {
        case ModNoun :
          {
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          SetToolbar(DynTBSaveGroup);
          pDsp->Vp1->ClearAllEntity();
          DoRestrained = 1;
          pGWnd->SetCursor(IDC_ORIGINPOS);
          bFlag4=false;
          Pt1.Zero();
          //if (!pMdl)
          //  {
          //  gs_pCmd->ExtendCmdLine("\x1b");
          //  LogError("GrfCmds", LF_Exclamation, "No Models Loaded");
          //  }
          //else
          //  {
          //  Pt1.Zero();
          //  Pt2.Zero();
          //  ATag = "";
          //  AClass = "";
          //  ASymbol = "";
          //  HideTagOff=False;
          //  }

          break;
          }
        case 1 : //Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          pGWnd->SetCursor(IDC_NODEPOS);
          break;
        case 2 : //IWin
        case 3 : //CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 4 : //Redraw
          //pWnd->Invalidate(True);
          pDBDlg = NULL;//new CNeutralExportDBDlg(AfxGetMainWnd()); break;
          // DIal
          break;
        }
      break;
      }
    case EX_MOUSE_LUP :
      {
      CWaitCursor Wait;
      bFlag1 = true;
      switch (Modifier)
        {
        case 1 :
          {
          pDsp->Vp1->ClrSelectionAllList();
          if (!bFlag4)
            {
            int xxx=0; // GetOrigin;
            //pGWnd->SetCursor(IDC_NODEPOS);
            bFlag4=1;
            Pt1=pDsp->CurrentPt;
            pDsp->Show_Dig_CrossHair(Pt1, true, true);
            }
          else
            {
            const bool AllowUnSelect = false;
            //kga 12/7/00 : set AllowUnSelect to false for now because,
            //may need improving and similar functionality is required in Explode!
            if (AllowUnSelect)
              {
              BOOL AlreadySelected;
              CEntInView* p = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt, AlreadySelected);
              if (p && AlreadySelected)
                {
                BOOL Ok = true;
                if (SaveMdl && p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()))
                  {
                  pchar pTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr);
                  if (pTag)
                    {
                    RequestModelIOConnRec Info;
                    int i = 0;
                    while (gs_pPrj->RequestModelIOConn(pTag, i++, Info) && Ok)
                      {
                      if (strcmp(Info.Group(), FlwLinkGrp)==0 || strcmp(Info.Group(), CtrlLinkGrp)==0)
                        Ok = false; //cannot unselect model with links!!!
                      }
                    }
                  //if (pTag && !LinkEntity(pTag))
                  //  Ok = flase; //cannot unselect model with links!!!
                  }
                if (Ok)
                  {
                  pDsp->Vp1->DeSelectEntity(p);
                  bFlag1 = false;
                  }
                }
              }
            else
              pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            break;
            }
          }
        case 2 :
        case 3 :
          {
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          break;
          }
        }

      if (SaveMdl && bFlag1)
        {
        //CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
        //while (p)
        //  {
        //  pchar pTag;
        //  if (p->EntityPtr() && DXF_ENTITY_IS_INSERT(p->EntityPtr()) && (pTag = Find_Attr_Value(p->EntityPtr(), TagAttribStr)))
        //    {
        //    RequestModelIOConnRec Info;
        //    int i = 0;
        //    pDsp->Vp1->ClrSelectionAllList();
        //    pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
        //    pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
        //    while (gs_pPrj->RequestModelIOConn(pTag, i++, Info))
        //      {
        //      if (strcmp(Info.Group(), FlwLinkGrp)==0 || strcmp(Info.Group(), CtrlLinkGrp)==0)
        //        {
        //        char *a[] = { TagAttribStr,    (pchar)NULL };
        //        char *b[] = { Info.Tag(), (pchar)NULL };
        //        pDrw->SelectInsertsOnAttrCombo(NULL, a, b, NULL);
        //        }
        //      }
        //    }
        //  p = pDsp->Vp1->NextSelectedEntity();
        //  }
        }
      pGWnd->SetCursor(IDC_NODEPOS);
      break;
      }
    case EX_TERM_SEMICOLON :
      if (gs_pCmd->IsEscBusy())
        break;
    case EX_EXEC :
    //  break;
    //case EX_DOIT :
      {

      DML_LIST CurveLst  = dml_create_list();
      DML_LIST InsertLst = dml_create_list();
      DML_LIST OtherLst  = dml_create_list();

      CNeutralSaveGroupDlg Dlg(pDsp);
      CWaitMsgCursor Wait("Saving entities");
      int SavesFailedCnt = 0;
      int MdlSaves = 0;
      CEntInView * pEnt = pDsp->Vp1->FirstSelectedEntity();
      while (pEnt)
        {
        DXF_ENTITY e = pEnt->EntityPtr();
        C3_CURVE   c = pEnt->CurvePtr();
        if (SaveSym && c && pDrw->Exists(c))
          {
          DML_ITEM it = dml_append_data(CurveLst, c);
          //pDsp->Draw(c, GrfHelper.GR_BACKGROUND);
          //pDrw->Delete(c);
          }
        if (e && pDrw->Exists(e))
          {
          DML_ITEM it = dml_append_data(InsertLst, e);
          pchar pTag;
          if (SaveMdl)
            {
            if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
              {
              CMdlValueSet::Clear();
              Dlg.AddTag(pTag);
              }
            }
          if (SaveSym)
            {
            //pDsp->Draw(e, GrfHelper.GR_BACKGROUND);
            //pDrw->Delete(e);
            }
          //pDsp->Vp1->
          }
        pEnt = pDsp->Vp1->NextSelectedEntity();
        }

      C3_BOX_S TheBounds;
      if (pDrw->GetBounds(&TheBounds, CurveLst, InsertLst, OtherLst))
        {
        Dlg.m_Lo=CDblTriPt(TheBounds.minpt[0], TheBounds.minpt[1], TheBounds.minpt[2]);
        Dlg.m_Hi=CDblTriPt(TheBounds.maxpt[0], TheBounds.maxpt[1], TheBounds.maxpt[2]);
        Dlg.m_OrigDig.m_X=Pt1.World.X;
        Dlg.m_OrigDig.m_Y=Pt1.World.Y;
        Dlg.m_OrigDig.m_Z=Pt1.World.Z;

        //if (SavesFailedCnt)
        //  {
        //  if (SavesFailedCnt==1)
        //    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "A model was not saved to template");
        //  else
        //    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "%d models were not saved to template.", SavesFailedCnt);
        //  }

        if (Dlg.DoModal())
          {
          if (Dlg.m_bAllOK)
            {
            //CDocTemplate & GT = ScdApp()->GraphTemplate();
            //CNeutralImportExport NExport;
            //NExport.SetGroup(Dlg.GetGroup());
            //NExport.SetGroupDesc(Dlg.m_sGroupDesc);
            ////NExport.SetExportTagList(&Dlg.m_RqdTags, &Dlg.m_NewTags);
            //NExport.SetExportCoOrds(Dlg.m_Origin, Dlg.m_Lo, Dlg.m_Hi, Dlg.m_AlignLo, Dlg.m_AlignHi);
            //NExport.SetExportTagList(&Dlg.m_RqdTags, &Dlg.m_NewTags);
            //NExport.DoExport(eNDB_StdExport, Dlg.m_sDatabaseFull, Dlg.m_sDatabaseFull);
            }
          }
        else
          {
          // No Bounds
          }
        }

      //TODO ??? Clear
      dml_free_list(CurveLst);
      dml_free_list(InsertLst);
      dml_free_list(OtherLst);

      //if (MdlDeletes)
      //  gs_LicenseCnt.CalcUnits();
      pDsp->Vp1->ClearAllEntity();
      gs_pCmd->ExtendCmdLine("\r");
      pGWnd->SetCursor();
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      pDsp->Show_Dig_CrossHair(Pt1,false,true);
      break;
      }
    case EX_UNDO :
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      DumpErrors();
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Show_Dig_CrossHair(Pt1,false,true);
      pDsp->Vp1->ClearAllEntity();
      pGWnd->SetCursor();
      RemoveToolbar();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
#endif
  };

//---------------------------------------------------------------------------

void GrfCmdBlk::DeleteDrawing()
  {
  pDsp->SetViewDrawing(NULL);
  delete pDrw;
  pDrw = new DXF_Drawing(pDsp, "", True);
  pDsp->SetViewDrawing(pDrw);
  pDsp->Open();
  pDsp->SetZoom(Zoom_All, 0.0, 0.0, 280.0, 198.0);
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::Load(pchar FnDraw, flag DoMerge)
  {
  flag UseBusyDlg = !CDlgBusy::IsOpen();
  if (UseBusyDlg)
    CDlgBusy::Open("");
  char Buff[256];
  sprintf(Buff, "Loading drawing %s", FnDraw ? FnDraw : "");
  CStatusBarMsg BarMsg(Buff);

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

  if (FnDraw)
    {
    t = time(NULL);
    if (UseBusyDlg)
      CDlgBusy::SetLine(1, "Loading Drawing :\n\n %s", FnDraw);
    if (pDrw == NULL)
      {
      pDrw = new DXF_Drawing(pDsp, "", True);
      pDsp->SetViewDrawing(pDrw);
      }

    int test_mode = 1;
    int err = pDrw->Load(FnDraw, DoMerge ? Load_Merge : Load_All, test_mode);

    if (err == 1)  // indicates a load error followed by a ok cleanup
      {
      delete pDrw;
      pDrw = new DXF_Drawing(pDsp, "", True);
      pDsp->SetViewDrawing(pDrw);

      int test_mode = 1;
      if (pDrw->Load(FnDraw, DoMerge ? Load_Merge : Load_All,test_mode))
        {
        AfxMessageBox("Cannot recover drawing - Aborting");
        if (UseBusyDlg)
          CDlgBusy::Close();
        return;
        }
      }
    else if (err == -1)
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

  if (FnDraw)
    pWnd->Invalidate(True);

  if (UseBusyDlg)
    CDlgBusy::Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::Save(pchar FnDraw, flag FullSave)
  {
  if (FnDraw && pDrw)
    {
    CDlgBusy::Open("");
    CDlgBusy::SetLine(1, "Saving DXF :\n\n %s", FnDraw);
    pDrw->Save(FnDraw);
    CDlgBusy::Close();
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::SaveSymbols(pchar SymPath, pchar DocGUID)
  {
  if (SymPath && pDrw)
    {
    CDlgBusy::Open("");
    CDlgBusy::SetLine(1, "Saving Symbols:\n\n %s", SymPath);
    pDrw->SaveSymbols(SymPath, DocGUID);
    CDlgBusy::Close();
    }
  }

//---------------------------------------------------------------------------

Strng sPrevDxfDir; //last directory used by dxf load or save

//---------------------------------------------------------------------------

void GrfCmdBlk::DoLoadMerge(flag DoMerge)
  {
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC :
      {
      switch (Noun)
        {
        case 1 :
          {
          char Fn1[_MAX_PATH], FnD[_MAX_PATH];
          Fn1[0] = 0;
          sprintf(FnD, "%s*.dxf", sPrevDxfDir.Len()==0 ? PrjFiles() : sPrevDxfDir());
          if (GetAFileName(gs_pCmd->Token(2), FnD, Fn1, sizeof(Fn1), NULL, NULL, 0, False, True))
            {
            CWaitCursor Wait;
            sPrevDxfDir.FnDrivePath(Fn1);
            __time64_t t = _time64(NULL);
            Load(Fn1, DoMerge);
            pWnd->Invalidate(True);
            gs_pCmd->Print("Load Elapsed time = %ld secs\n", time(NULL) - t);
            #if dbgdrw
            dbgpln("Load Elapsed time = %ld secs", time(NULL) - t);
            #endif
            }
          break;
          }
        }
      break;
      }
    case EX_RESULTS :
      DumpErrors();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoMerge()
  {
  DoLoadMerge(True);
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoLoad()
  {
  dbgMemoryState("Start Load");
  DoLoadMerge(False);
  dbgMemoryState("Load Done");
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoImportPDS()
  {
  #if WITHIMPORTPDS
  dbgMemoryState("Start Import");
//  DoLoadMerge(False);

  pDsp->Open();
  switch (When)
    {
    case EX_EXEC :
      {
      switch (Noun)
        {
        case 1 :
          {
          char Fn1[_MAX_PATH], FnD[_MAX_PATH];
          Fn1[0] = 0;
          sprintf(FnD, "%s*.n", sPrevDxfDir.Len()==0 ? PrjFiles() : sPrevDxfDir());
          if (GetAFileName(gs_pCmd->Token(2), FnD, Fn1, sizeof(Fn1), NULL, NULL, 0, False, True))
            {
            CWaitCursor Wait;
            sPrevDxfDir.FnDrivePath(Fn1);
            __time64_t t = _time64(NULL);
            ImportPDS(Fn1, false);
            pWnd->Invalidate(True);
            gs_pCmd->Print("Load Elapsed time = %ld secs\n", time(NULL) - t);
            #if dbgdrw
            dbgpln("Load Elapsed time = %ld secs", time(NULL) - t);
            #endif
            }
          break;
          }
        }
      break;
      }
    case EX_RESULTS :
      DumpErrors();
      break;
    }
  pDsp->Close();

  dbgMemoryState("Import Done");
  #endif
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoSave()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC :
      {
      switch (Noun)
        {
        case 1 :
          {
          char Fn1[_MAX_PATH], FnD[_MAX_PATH];
          Fn1[0] = 0;
          sprintf(FnD, "%s*.dxf", sPrevDxfDir.Len()==0 ? PrjFiles() : sPrevDxfDir());
          if (GetAFileName(gs_pCmd->Token(2), FnD, Fn1, sizeof(Fn1), NULL, NULL, 0, False, False))
            {
            CWaitMsgCursor Wait("Saving drawing");
            sPrevDxfDir.FnDrivePath(Fn1);
            __time64_t t = _time64(NULL);
            Save(Fn1, False);
            gs_pCmd->Print("Save Elapsed time = %ld secs\n", time(NULL) - t);
            #if dbgdrw
            dbgpln("Save Elapsed time = %ld secs", time(NULL) - t);
            #endif
            }
          break;
          }
        }
      break;
      }
    case EX_RESULTS :
      DumpErrors();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoSaveSymbols()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC :
      {
      switch (Noun)
        {
        case 1 :
          {
          //char Fn1[_MAX_PATH], FnD[_MAX_PATH];
          // TEMPORARY
          if (DrawingFile.GetLength()>0)
            {
            Strng Fn1(PrjFiles());
            Fn1.FnCheckEndBSlash();
            if (gs_pCmd->Token(2))
              Fn1+=gs_pCmd->Token(2);
            else
              Fn1+="Symbols";
            Fn1.FnCheckEndBSlash();
            Strng Fn2(DrawingFile);
            Fn2.FnName();
            Fn1+=Fn2;
            Fn1.FnCheckEndBSlash();
            if (FnCreateDirectory(Fn1(), true))
              {
              CWaitMsgCursor Wait("Saving Symbols");
              __time64_t t = _time64(NULL);
              SaveSymbols(Fn1(), dynamic_cast<CGrfDoc*>(pDoc)->m_sDocGUID());
              gs_pCmd->Print("Save Elapsed time = %ld secs\n", time(NULL) - t);
              #if dbgdrw
              dbgpln("Save Elapsed time = %ld secs", time(NULL) - t);
              #endif
              }

            //Fn1[0] = 0;
            //sprintf(FnD, "%s*.dxf", sPrevDxfDir.Len()==0 ? PrjFiles() : sPrevDxfDir());
            //if (GetAFileName(gs_pCmd->Token(2), FnD, Fn1, sizeof(Fn1), NULL, NULL, 0, False, False))
            //  {
            //  CWaitMsgCursor Wait("Saving Symbols");
            //  //sPrevDxfDir.FnDrivePath(Fn1);
            //  __time64_t t = _time64(NULL);
            //  SaveSymbols(Fn1());
            //  gs_pCmd->Print("Save Elapsed time = %ld secs\n", time(NULL) - t);
            //  #if dbgdrw
            //  dbgpln("Save Elapsed time = %ld secs", time(NULL) - t);
            //  #endif
            //  }
            }
          else
            LogError("Graphics", LF_DoAfxMsgBox|LF_Exclamation, "Project must be saved before Save Symbols");
          break;
          }
        }
      break;
      }
    case EX_RESULTS :
      DumpErrors();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoSetAutoXEC()
  {
  switch (When)
    {
    case EX_EXEC :
      {
      Strng Msg;
      AutoXEC = gs_pCmd->Token(2);
      }
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoZoom()
  {
  //SETVNT("ZOom",      "View", "Win All Page Left Right Down Up IN OUt Set Purge ISo ONce", 1, (CmdFn)&GrfCmdBlk::DoZoom, EF_ERS);
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      {
      flag DoRepaint = 1;
      switch (Modifier)
        {
        case ModNoun : ActivateGWnd(); DoRepaint = 0; SetToolbar(DynTBZoom); break;
        case 1 : gs_pCmd->SetDigInfo(GC_RectCurs, 1); DoRepaint = 0; pGWnd->SetCursor(IDC_SELECTBOX); break;
        case 2 : pDsp->SetZoom(Zoom_All); break;
        case 3 : pDsp->SetZoom(Zoom_Page, 0.0, 0.0, 280.0, 198.0); break;
        case 4 : pDsp->SetZoom(Zoom_PanRel, -0.25, 0.0, 1.0, 1.0); break;
        case 5 : pDsp->SetZoom(Zoom_PanRel, 0.25, 0.0, 1.0, 1.0); break;
        case 6 : pDsp->SetZoom(Zoom_PanRel, 0.0, -0.25, 1.0, 1.0); break;
        case 7 : pDsp->SetZoom(Zoom_PanRel, 0.0, 0.25, 1.0, 1.0); break;
        case 8 : pDsp->SetZoom(Zoom_PanRel, 0.0, 0.0, 0.5, 0.5); break;
        case 9 : pDsp->SetZoom(Zoom_PanRel, 0.0, 0.0, 2.0, 2.0); break;
        case 10: gs_pCmd->SetDigInfo(GC_RectCurs, 1); DoRepaint = 0; pGWnd->SetCursor(IDC_SELECTBOX); break;
        case 11: gs_pCmd->SetDigInfo(GC_RectCurs, 1); DoRepaint = 0; pGWnd->SetCursor(IDC_SELECTBOX); break;
        case 12: pDsp->SetZoom(Zoom_Iso); break;
        case 13: RemoveToolbar(); gs_pCmd->SetDigInfo(GC_RectCurs, 1); DoRepaint = 0; pGWnd->SetCursor(IDC_SELECTBOX); break;
        default: DoRepaint = 0; break;
        }
      if (DoRepaint)
        {
        pDrw->EntityInvalidate();
        pWnd->Invalidate(True);
        }
      break;
      }
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 :
        case 13:
          pDsp->SetZoom(Zoom_Win, pDsp->StartPt, pDsp->EndPt);
          pDrw->EntityInvalidate();
          pWnd->Invalidate(True);      // RepaintWindow(pWnd->m_hWnd);
          pGWnd->SetCursor();
          if (Modifier==13)
            {
            gs_pCmd->ExtendCmdLine("\r");
            }
          break;
        case 10:
          pDsp->SetZoom(Zoom_Set, pDsp->StartPt, pDsp->EndPt);
          pDrw->EntityInvalidate();
          pWnd->Invalidate(True);      // RepaintWindow(pWnd->m_hWnd);
          pGWnd->SetCursor();
          break;
        case 11:
          pDsp->SetZoom(Zoom_Purge, pDsp->StartPt, pDsp->EndPt);
          pDrw->EntityInvalidate();
          pWnd->Invalidate(True);      // RepaintWindow(pWnd->m_hWnd);
          pGWnd->SetCursor();
          break;
        default :;
        }
      break;
    case EX_ALLUNDONE :
      DumpErrors();
    case EX_RESULTS :
      pGWnd->SetCursor();
      RemoveToolbar();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoReDraw()
  {
  if (When==EX_EXEC)
    {
    if (gs_pTheSFELib)
      {
      gs_pTheSFELib->SetLoadAllModelStatus();
      gs_pTheSFELib->SetLoadAllModelAssocGrfs();
      }

    pDsp->Open();
    pDrw->EntityInvalidate();
    //pDsp->SetZoom(Zoom_PanRel, 0.0, 0.0, 1.0, 1.0); //when in full zoom, this is NOT always correct!!
    pWnd->Invalidate(True);
    pDsp->Close();
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoCopyBitmap()
  {
  //SETVNT("COPy",      "BITmap", N_A, -1, (CmdFn)&GrfCmdBlk::DoCopyBitmap, EF_ERS);
  if (When==EX_EXEC)
    {
    pDsp->Open();
    //todo : bring window to front!
    pGWnd->CopyBMPtoClipBoard(1);
    pDsp->Close();
    }
  }

//---------------------------------------------------------------------------

flag GrfCmdBlk::IsLike(DXF_ENTITY Tst)
  {
  if (Like == NULL)
    return 1;
  if ((strcmp(Insert_Block(Tst), Insert_Block(Like)) == 0) &&
      (fabs(Insert_Rotation(Tst) - Insert_Rotation(Like)) < 1.0))
    return 1;
  return 0;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::ShowAttrs(DXF_ENTITY Ins, flag Tags)
  {
  Entity_List_Info EntInfo;
  DXF_ENTITY TAtt = First_Ent(Attr_List(Ins), EntInfo);
  int n = 1;
  while (TAtt)
    {
    if (Tags == Attr_Is(TAtt, TagAttribStr))
      {
      if (Valid(Size))
        {
        Attr_Pt(TAtt)[1] -= n * (Size - Attr_Size(TAtt));
        Attr_Size(TAtt) = Size;
        }
      if (Valid(Rotation))
        {
        Attr_Rotation(TAtt) = Rotation;
        }
      if (Valid(Delta.X))
        {
        Attr_Pt(TAtt)[0] = Insert_BasePt(Ins)[0] + Delta.X;
        Attr_Pt(TAtt)[1] = Insert_BasePt(Ins)[1] + Delta.Y - n * (Attr_Size(TAtt));
        }
      if (AttrName > 0)
        {
        int TLen = strlen (Attr_Tag (TAtt));
        int VLen = strlen (Attr_Value (TAtt));
        if ((_strnicmp(Attr_Tag(TAtt), Attr_Value(TAtt), TLen) != 0) && (Attr_Value(TAtt)[TLen] != ':'))
          {
          pchar vs = & (Attr_Value(TAtt)[TLen + 1]);
          memmove(vs, Attr_Value(TAtt), VLen + 1);
          Attr_Value(TAtt)[TLen] = ':';
          memmove(Attr_Value(TAtt), Attr_Tag(TAtt), TLen);
          }
        }
      if (AttrName < 0)
        {
        int TLen = strlen (Attr_Tag (TAtt));
        if ((_strnicmp(Attr_Tag(TAtt), Attr_Value(TAtt), TLen) == 0) && (Attr_Value(TAtt)[TLen] == ':'))
          {
          pchar vs = & (Attr_Value(TAtt)[TLen + 1]);
          memmove(Attr_Value(TAtt), vs, strlen(vs) + 1);
          }
        }
      Show_Attr(TAtt);
      n++;
      }
    TAtt = Next_Ent(EntInfo);
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::HideAttrs(DXF_ENTITY Ins, flag Tags)
  {
  Entity_List_Info EntInfo;
  DXF_ENTITY TAtt = First_Ent(Attr_List(Ins), EntInfo);
  while (TAtt)
    {
    if (Tags == Attr_Is(TAtt, TagAttribStr))
      Hide_Attr(TAtt);
    TAtt = Next_Ent(EntInfo);
    }
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::MoveAttrs(DXF_ENTITY Ins, flag Tags, Pt_3f Delta)
  {
  Entity_List_Info EntInfo;
  DXF_ENTITY TAtt = First_Ent(Attr_List(Ins), EntInfo);
  int n = 0; //set n=0 for bottom left reference
  //int n = 1; //set n=1 for top left reference
  while (TAtt)
    {
    if (Tags == Attr_Is(TAtt, TagAttribStr))
      {
      Attr_Pt(TAtt)[0] = Insert_BasePt(Ins)[0] + Delta.X;
      Attr_Pt(TAtt)[1] = Insert_BasePt(Ins)[1] + Delta.Y - (n * (Attr_Size(TAtt)));
      n++;
      }
    TAtt = Next_Ent(EntInfo);
    }
  }

//---------------------------------------------------------------------------

DXF_ENTITY GrfCmdBlk::FirstAttr(DXF_ENTITY Ins, flag Tags)
  {
  Entity_List_Info EntInfo;
  DXF_ENTITY TAtt = First_Ent(Attr_List(Ins), EntInfo);
  while (TAtt)
    {
    if (Tags == Attr_Is(TAtt, TagAttribStr))
      return TAtt;
    TAtt = Next_Ent(EntInfo);
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoUpdateTags()
  {
  //SETVNT("UPdate", "TAgs", "@ IWin CWin * All Like NOLike SIze NOSize ROtation NORotation NAme NOName Position NOPosition SHow Hide REdraw NEXt", 1, (CmdFn)&GrfCmdBlk::DoUpdateTags, EF_ERS);
  enum MId { MID_Dig=1, MID_IWin, MID_CWin, MID_Dlg, MID_All, MID_Like, MID_NoLike, MID_Size, MID_NoSize, MID_Rotation, MID_NoRotation, MID_Name, MID_NoName, MID_Position, MID_NoPosition, MID_Show, MID_Hide, MID_Redraw, MID_Next };
  static CUpdateTagsDlg* pTagsDlg = NULL;
  pDsp->Open();
  switch (When)
    {
    DXF_ENTITY Ins;
    DXF_ENTITY TAtt;
    Entity_List_Info InsInfo;
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          pDsp->Vp1->ClearAllEntity();
          Size=dNAN;
          Rotation=dNAN;
          Delta.X=dNAN;
          Like = NULL;
          AttrName = 0;
          bFlag1 = 1; //show tags
          bFlag2 = 0; //setting tag position
          bFlag3 = 0; //selecting position
          break;
        case MID_Dig :
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case MID_IWin :
        case MID_CWin :
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case MID_Dlg :
          if (pTagsDlg)
            {
            pTagsDlg->DestroyWindow();
            delete pTagsDlg;
            }
          pTagsDlg = new CUpdateTagsDlg(AfxGetMainWnd());
          break;
        case MID_All :
          Ins = pDrw->First_Insert(InsInfo);
          while (Ins)
            {
            if (IsLike(Ins))
              {
              pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
              if (bFlag1)
                ShowAttrs(Ins, 1);
              else
                HideAttrs(Ins, 1);
              pDsp->Draw(Ins, -1);
              }
            Ins = pDrw->Next_Insert(InsInfo);
            }
          pDrw->EntityInvalidate();
          pDsp->Vp1->ClearAllEntity();
          pWnd->Invalidate();
          Size=dNAN;
          Rotation=dNAN;
          Delta.X=dNAN;
          Like = NULL;
          AttrName = 0;
          bFlag1 = 1; //show tags
          break;
        case MID_Like : gs_pCmd->SetDigInfo(GC_NoCurs, 1); break;
        case MID_NoLike : Like = NULL; break;
        case MID_Size :gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_NoSize : Size=dNAN; break;
        case MID_Rotation : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_NoRotation : Rotation=dNAN; break;
        case MID_Name : AttrName = 1; break;
        case MID_NoName : AttrName = -1; break;
        case MID_Position :
          //gs_pCmd->SetDigInfo(GC_RubberCurs, 1);
          pGWnd->SetCursor(IDC_SELECTUNIT);
          bFlag2 = 1;
          bFlag3 = 0;
          if (pTagsDlg)
            pTagsDlg->SetDescription("Select unit");
          break;
        case MID_NoPosition :
          pGWnd->SetCursor();
          Delta.X=dNAN;
          bFlag2 = 0;
          if (pTagsDlg)
            pTagsDlg->SetDescription("");
          break;
        case MID_Show : bFlag1 = 1; break;
        case MID_Hide : bFlag1 = 0; break;
        case MID_Redraw : pWnd->Invalidate(); break;
        case MID_Next :
          pGWnd->SetCursor(IDC_SELECTUNIT);
          bFlag3 = 0;
          if (pTagsDlg)
            pTagsDlg->SetDescription("Select unit");
          //gs_pCmd->ExtendCmdLine("\r");//stops working when command line gets full, try prevent this!
          //gs_pCmd->ExtendCmdLine("update tags * ");
          if (bFlag2)
            {
            pDsp->Vp1->ClearAllEntity();
            pDsp->Vp1->ClrSelectionAllList();
            }
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Size : gs_pCmd->dParm(Size); break;
        case MID_Rotation : gs_pCmd->dParm(Rotation); break;
        }
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        {
        CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
        while (pEnt)
          {
          Ins = pEnt->EntityPtr();
          if (Ins && IsLike(Ins))
            {
            pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
            if (bFlag1)
              ShowAttrs(Ins, 1);
            else
              HideAttrs(Ins, 1);
            pDsp->Draw(Ins, -1);
            }
          pEnt = pDsp->Vp1->NextSelectedEntity();
          }
        pDrw->EntityInvalidate();
        pDsp->Vp1->ClearAllEntity();
        pWnd->Invalidate();
        pGWnd->SetCursor();
        Size=dNAN;
        Rotation=dNAN;
        Delta.X=dNAN;
        Like = NULL;
        AttrName = 0;
        bFlag1 = 1; //show tags
        bFlag2 = 0; //setting tag position
        bFlag3 = 0; //selecting position
        gs_pCmd->ExtendCmdLine("\r");//stops working when command line gets full, try prevent this!
        gs_pCmd->ExtendCmdLine("update tags * ");
        }
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case MID_Dig :
          if (bFlag2)
            {
            if (bFlag3)
              {//selecting tag position...
              CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
              if (pEnt==NULL)
                {
                pEnt = GetClosest(Pt1, true);
                if (pEnt)
                  pDsp->Vp1->SelectEntity(pEnt);
                }

              if (pEnt && pEnt->EntityPtr())
                {
                Ins = pEnt->EntityPtr();
                TAtt = Find_Attr(Ins, TagAttribStr);// FirstAttr(Ins, 1);
                if (TAtt)
                  {
                  Delta.X = pDsp->CurrentPt.World.X - DXF_ATTRIB_PT(TAtt)[0];// Insert_BasePt(Ins)[0];
                  Delta.Y = pDsp->CurrentPt.World.Y - DXF_ATTRIB_PT(TAtt)[1];// Insert_BasePt(Ins)[1];
#if SYSCAD10         
                  DXF_ENTITY GAtt = Find_Attr(Ins, GuidAttribStr);// FirstAttr(Ins, 1);
                  if (gs_pPrj->SvcActive && GAtt)
                    {
                    //Delta.X = pDsp->CurrentPt.World.X - DXF_ATTRIB_PT(TAtt)[0];// Insert_BasePt(Ins)[0];
                    //Delta.Y = pDsp->CurrentPt.World.Y - DXF_ATTRIB_PT(TAtt)[1];// Insert_BasePt(Ins)[1];
                    SCD10ENTER;
                    //What about Symbol only ??????????
                    gs_pPrj->Svc.GCBModifyTagG((CGrfDoc*)pDoc, TAtt, Find_Attr_Value(Ins, GuidAttribStr),  
                      Delta, DXF_ATTRIB_HEIGHT(TAtt), DXF_ATTRIB_ROT_ANG(TAtt), (DXF_ATTRIB_AFLAGS(TAtt)&DXF_ATTRIB_INVIS)==0);

                    SCD10LEAVE;
                    }
                  else
#endif
                    {
                    //Delta.X = pDsp->CurrentPt.World.X - DXF_ATTRIB_PT(TAtt)[0];// Insert_BasePt(Ins)[0];
                    //Delta.Y = pDsp->CurrentPt.World.Y - DXF_ATTRIB_PT(TAtt)[1];// Insert_BasePt(Ins)[1];
                    pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
                    MoveAttrs(Ins, 1, Delta);
                    pDrw->EntityInvalidate(Ins, NULL);
                    pDsp->Draw(Ins, -1);
                    pDsp->Vp1->ClearAllEntity();
                    pDsp->Vp1->ClrSelectionAllList();
                    CEntInView* p = GetClosestEntity(Pt1, Ins);

                    if (p)
                      pDsp->Vp1->SelectEntity(p);
                    //pWnd->Invalidate();
                    }
                  }
                }
              }
            else
              {
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              Pt1 = pDsp->StartPt;
              CEntInView* p = GetClosest(Pt1, true);
              if (p && p->EntityPtr())
                {
                Ins = p->EntityPtr();
                TAtt = Find_Attr(Ins, TagAttribStr);// FirstAttr(Ins, 1);
                if (TAtt)
                  {
                  pDsp->Vp1->SelectEntity(p);
                  bFlag3 = 1;
                  pGWnd->SetCursor(IDC_POS);
                  if (pTagsDlg)
                    {
                    char Buff[1024];
                    sprintf(Buff, "Select tag annotation position");
                    DXF_ENTITY e = p->EntityPtr();
                    if (e && pDrw->Exists(e))
                      {
                      pchar pTag;
                      if (DXF_ENTITY_IS_INSERT(e) && (pTag = Find_Attr_Value(e, TagAttribStr)))
                        sprintf(Buff, "%s : Select tag annotation position", pTag);
                      }
                    pTagsDlg->SetDescription(Buff);
                    }
                  }
                }
              }
            }
          else
            {
            pDsp->Vp1->ClrSelectionAllList();
            BOOL AlreadySelected;
            CEntInView* p = GetClosest(pDsp->CurrentPt, AlreadySelected, true);
            if (p)
              {
              if (AlreadySelected)
                pDsp->Vp1->DeSelectEntity(p);
              else
                pDsp->Vp1->SelectEntity(p);
              }
            }
          break;
        case MID_IWin :
        case MID_CWin :
          {
          pGWnd->SetCursor();
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
          while (pEnt)
            {
            Ins = pEnt->EntityPtr();
            if (Ins && !DXF_ENTITY_IS_INSERT(Ins))
              {
              pDsp->Vp1->DeSelectEntity(pEnt);
              }
            pEnt = pDsp->Vp1->NextSelectedEntity();
            }
          break;
          }
        case MID_Like :
          Like = pDrw->ClosestInsert(pDsp->CurrentPt);
          break;
        default :;
        }
      break;
    case EX_DOIT :
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      if (pTagsDlg)
        {
        pTagsDlg->DestroyWindow();
        delete pTagsDlg;
        pTagsDlg = NULL;
        }
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoCreateFill()
  {
  //SETVNT("CReate", "Fill", "@ Vars * MIn MAx All", 1, (CmdFn)&GrfCmdBlk::DoCreateFill, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static flag doit;
  static C3_CURVE TheCurve = NULL;
  static DXF_ENTITY TheInsert = NULL;
  static CCreateFillDlg* pFillDlg = NULL;

  pDsp->Open();
  switch (When)
    {
    case EX_TERM_SEMICOLON :
      break;
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          Size = 1.0;
          doit = 0;
          TheCurve = NULL;
          TheInsert = NULL;
          ATag = "";
          AVar = "Lvl";
          d1 = 0.0; //Min
          d2 = 100.0; //Max
          i1 = 1; //Apply to All
          i2 = DEFAULTFILLCOLOUR;
          pDsp->Vp1->ClearAllEntity();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          break;
        case 1 :
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 :
          gs_pCmd->SetParmInfo(AName, 1);
          break;
        case 3 :
          if (pFillDlg)
            {
            pFillDlg->DestroyWindow();
            delete pFillDlg;
            }
          pFillDlg = new CCreateFillDlg(AfxGetMainWnd(), ATag(), AVar(), d1, d2, 0, i1, i2);
          break;
        case 4 :
        case 5 :
          gs_pCmd->SetParmInfo(AFloat,1);
          break;
        case 6 :
          gs_pCmd->SetParmInfo(AnInt, 1);
          break;
        default:
          break;
        }
      break;
    case EX_MOUSE_LUP:
      switch( Modifier )
        {
        case 1:
          if (ATag.Length()==0)
            {//dig on model to get tag...
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            CEntInView* pEnt = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            if (pEnt && pEnt->EntityPtr())
              {
              pchar TagV = Find_Attr_Value(pEnt->EntityPtr(), TagAttribStr);
              TheInsert = pEnt->EntityPtr();
              ATag = TagV;
              if (pFillDlg)
                pFillDlg->SetTag(ATag());
              pGWnd->SetCursor(IDC_POINTPOS);
              }
            }
          else
            {//dig nearest entity for dynamic fill area...
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            PT3 Pos;
            CEntInView* pEnt = pDsp->Vp1->SelectLocOnEntity(pDsp->CurrentPt, Pos);
            if (pEnt)
              {
              if (TheCurve)
                {
                pDsp->Draw(TheCurve, GrfHelper.GR_BACKGROUND);
                pDrw->Add_PLine_Vertex(TheCurve, Pos);
                pDsp->Draw(TheCurve, GR_GREEN);
                }
              else
                TheCurve = pDrw->Add_PLine_Start(Pos);
              if (pFillDlg)
                pFillDlg->SetPointsCount(C3_CURVE_PCURVE(TheCurve)->buffer.n);
              }
            }
          break;
        default : ;
        }
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 2 :
          AVar = gs_pCmd->LastToken();
          if (pFillDlg)
            pFillDlg->SetVar(AVar());
          break;
        case 4 :
          gs_pCmd->dParm(d1);
          if (pFillDlg)
            pFillDlg->SetMin(d1);
          break;
        case 5 :
          gs_pCmd->dParm(d2);
          if (pFillDlg)
            pFillDlg->SetMax(d2);
          break;
        case 6 :
          gs_pCmd->iParm(i1);
          if (pFillDlg)
            pFillDlg->SetApplyToAll(i1);
          break;
        default : ;
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      doit = 1;
      break;
    case EX_UNDO :
      switch (Modifier)
        {
        case 1:
          if (TheCurve == NULL)
            {
            ATag = "";
            pDsp->Vp1->ClearAllEntity();
            pDsp->Clear_Dig_Point_List();
            pGWnd->SetCursor(IDC_SELECTUNIT);
            if (pFillDlg)
              pFillDlg->SetTag(ATag());
            }
          else
            {
            pDsp->Draw(TheCurve, GrfHelper.GR_BACKGROUND);
            c3d_pcurve_remove_last(TheCurve);
            pDsp->Clear_Last_Dig_Point_List();
            if (C3_CURVE_PCURVE(TheCurve)->buffer.n == 0)
              {
              pDrw->Delete(TheCurve);
              TheCurve = NULL;
              if (pFillDlg)
                pFillDlg->SetPointsCount(0);
              }
            else
              {
              pDrw->EntityInvalidate(Like, NULL);
              pDsp->Draw(TheCurve, -1);
              if (pFillDlg)
                pFillDlg->SetPointsCount(C3_CURVE_PCURVE(TheCurve)->buffer.n);
              }
            }
          break;
        case 2:
          AVar = "";
          if (pFillDlg)
            pFillDlg->SetVar(AVar());
          break;
        /*case 4 :
          d1 = 0.0;
          if (pFillDlg)
            pFillDlg->SetMin(d1);
          break;
        case 5 :
          d2 = 100.0;
          if (pFillDlg)
            pFillDlg->SetMax(d2);
          break;*/
        }
      break;
    case EX_ALLUNDONE :
      doit = 0;
      if (TheCurve)
        {
        pDsp->Draw(TheCurve, GrfHelper.GR_BACKGROUND);
        pDrw->Delete(TheCurve);
        TheCurve = NULL;
        }
      TheInsert = NULL;
      AVar = "";
      ATag = "";
      pDsp->Vp1->ClearAllEntity();
      pDsp->Clear_Dig_Point_List();
      if (pFillDlg)
        {
        pFillDlg->DestroyWindow();
        delete pFillDlg;
        pFillDlg = NULL;
        }
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    case EX_RESULTS :
      DumpErrors();
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    default :;
    }

  if (doit)
    {
    CWaitMsgCursor Wait("Creating dynamic fill");
    if (pFillDlg)
      {
      pFillDlg->UpdateData(True);
      if (pFillDlg->m_Vars.GetLength()>0)
        AVar = (const char*)pFillDlg->m_Vars;
      d1 = pFillDlg->m_Min;
      d2 = pFillDlg->m_Max;
      i1 = pFillDlg->m_ApplyFillToAll;
      i2 = pFillDlg->FillColour();
      }
    Attr_Settings AAtt = Attr_Settings(50, 0, Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
    DXF_ENTITY dyninsert = pDrw->Create_Fill_Block((CExecObj*)pGWnd, TheInsert, ATag(), AVar(), d1, d2, TheCurve, AAtt, i1, i2);
    if (dyninsert)
      pDsp->Draw(dyninsert, -1);

    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    pDsp->Hide_Dig_Point_List();
    pDsp->Clear_Dig_Point_List();
    doit = 0;
    TheCurve = NULL;
    TheInsert = NULL;
    ATag = "";
    AVar = "";
    if (pFillDlg)
      {
      pFillDlg->DestroyWindow();
      delete pFillDlg;
      pFillDlg = NULL;
      }
    pGWnd->SetCursor();
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoUpdFill()
  {
  //SETVNT("UPdate", "Fill", "@ Delete", 1, (CmdFn)&GrfCmdBlk::DoUpdFill, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static DXF_ENTITY TheInsert = NULL;
  static CCreateFillDlg* pFillDlg = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          TheInsert = NULL;
          Size = 1.0;
          ATag = "";
          AVar = "";
          i1 = 1;
          i2 = DEFAULTFILLCOLOUR;
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->AddSelectionAttribList("FILLTAG");
          pGWnd->SetCursor(IDC_SELECTUNIT);
          break;
        case 1 : // a digitise
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : //delete
          if (pFillDlg && TheInsert)
            {
            pFillDlg->UpdateData(True);
            i1 = pFillDlg->m_ApplyFillToAll;
            DXF_ENTITY dyninsert = pDrw->Delete_Fill_Block(TheInsert, i1);
            if (dyninsert)
              pDsp->Draw(dyninsert, -1);
            pFillDlg->DestroyWindow();
            delete pFillDlg;
            pFillDlg = NULL;
            }
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 : // a digitise
          {
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
          pDsp->Vp1->ClrSelectionAllList();
          if (pEnt && pEnt->EntityPtr())
            {
            pchar pTag = Find_Attr_Value(pEnt->EntityPtr(), "FILLTAG");
            pchar pVar = Find_Attr_Value(pEnt->EntityPtr(), "FILLVAR");
            pchar pMin = Find_Attr_Value(pEnt->EntityPtr(), "FILLMIN");
            pchar pMax = Find_Attr_Value(pEnt->EntityPtr(), "FILLMAX");
            pchar pColor = Find_Attr_Value(pEnt->EntityPtr(), "FILLCOLOR");
            if (pTag && pVar && pMin && pMax)
              {
              const long iColor = (pColor ? atol(pColor) : DEFAULTFILLCOLOUR);
              ATag = pTag;
              TheInsert = pEnt->EntityPtr();
              if (pFillDlg)
                {
                pFillDlg->SetTag(pTag);
                pFillDlg->SetVar(pVar);
                pFillDlg->SetMin(atof(pMin));
                pFillDlg->SetMax(atof(pMax));
                pFillDlg->SetPointsCount(-1);
                pFillDlg->SetFillColour(iColor);
                }
              else
                pFillDlg = new CCreateFillDlg(AfxGetMainWnd(), pTag, pVar, atof(pMin), atof(pMax), -1, i1, iColor);
              Size = DXF_ATTRIB_HEIGHT(Find_Attr(TheInsert, "FILLTAG")); // retrieve the size of the text from the attribute
              }
            }
          break;
          }
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      if (pFillDlg && TheInsert)
        {
        CWaitMsgCursor Wait("Updating dynamic fill");
        pFillDlg->UpdateData(True);
        if (pFillDlg->m_Vars.GetLength()>0)
          AVar = (const char*)pFillDlg->m_Vars;
        d1 = pFillDlg->m_Min;
        d2 = pFillDlg->m_Max;
        i1 = pFillDlg->m_ApplyFillToAll;
        i2 = pFillDlg->FillColour();
        Attr_Settings AAtt = Attr_Settings(50, 0, Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        DXF_ENTITY dyninsert = pDrw->Create_Fill_Block((CExecObj*)pGWnd, TheInsert, ATag(), AVar(), d1, d2, NULL, AAtt, i1, i2);
        if (dyninsert)
          pDsp->Draw(dyninsert, -1);
        pFillDlg->DestroyWindow();
        delete pFillDlg;
        pFillDlg = NULL;
        }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      if (pFillDlg)
        {
        pFillDlg->DestroyWindow();
        delete pFillDlg;
        pFillDlg = NULL;
        }
      TheInsert = NULL;
      pGWnd->SetCursor();
      break;
    }

  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoCreateBars()
  {
  //SETVNT("CReate", "Bars", "@ * Tags Vars MIn MAx Scale", 1, (CmdFn)&GrfCmdBlk::DoCreateBars, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static CCreateBarDlg* pBarsDlg = NULL;
  static bool TagsOn;
  pDsp->Open();
  switch (When)
    {
    case EX_MOUSE_LDOWN:
      break;
    case EX_MODIFIER_RDY :
      switch( Modifier )
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          VarReq.Clear();
          TagReq.Clear();
          MinReq.Clear();
          MaxReq.Clear();
          ColReq.Clear();
          TagsOn = true;
          pDsp->Vp1->ClearAllEntity();
          Size = 1.0;
          d1 = 0.0; //Min
          d2 = 100.0; //Max
          iDispOpts = 0;
          Like = NULL;
          break;
        case 1 : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case 2 :
          if (pBarsDlg)
            {
            pBarsDlg->DestroyWindow();
            delete pBarsDlg;
            }
          pBarsDlg = new CCreateBarDlg(AfxGetMainWnd(), &TagReq, &VarReq, TagsOn, d1, d2, DEFAULTBARSCOLOUR, Size, 0);
          break;
        case 3 : gs_pCmd->SetParmInfo(AName, 1); break;
        case 4 : gs_pCmd->SetParmInfo(AName, 1); break;
        case 5 :
        case 6 :
        case 7 :
          gs_pCmd->SetParmInfo(AFloat, 1); break;
        }
      break;
    case EX_MOUSE_LUP:
      switch( Modifier )
        {
        case ModNoun :
          break;
        case 1 :
          if (TagsOn)
            {// Select another tagged ent
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            CEntInView* pEnt = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            if (pEnt)
              {
              pchar TagV = Find_Attr_Value(pEnt->EntityPtr(), TagAttribStr);
              if (TagReq.Find(TagV)==NULL)
                {
                TagReq.Append(TagV);
                if (pBarsDlg)
                  pBarsDlg->UpdateTagList(FALSE);
                }
              }
            }
          else
            {// set position of dynamic bars
            pDsp->Vp1->ClrSelectionAllList();
            //pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            //pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            if (Like)
              {
              pDsp->Draw(Like, GrfHelper.GR_BACKGROUND);
              pDrw->TranslateEntity(Like, Pt1.World, pDsp->CurrentPt.World);
              Pt1 = pDsp->CurrentPt;
              pDrw->EntityInvalidate(Like, NULL);
              pDsp->Vp1->ClearAllEntity();
              pDsp->Draw(Like, -1);
              }
            }
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 3 :
          {
          Strng s = gs_pCmd->LastToken();
          if (TagReq.Find(s())==NULL)
            {
            TagReq.Append(s());
            if (pBarsDlg)
              pBarsDlg->UpdateTagList(FALSE);
            }
          //gs_pCmd->TestAddAnotherParm();
          break;
          }
        case 4 :
          {
          Strng s = gs_pCmd->LastToken();
          if (VarReq.Find(s())==NULL)
            {
            VarReq.Append(s());
            if (pBarsDlg)
              pBarsDlg->UpdateVarList(FALSE);
            }
          //gs_pCmd->TestAddAnotherParm();
          break;
          }
        case 5 :
          gs_pCmd->dParm(d1);
          if (pBarsDlg)
            pBarsDlg->SetMin(d1);
          break;
        case 6 :
          gs_pCmd->dParm(d2);
          if (pBarsDlg)
            pBarsDlg->SetMax(d2);
          break;
        case 7 :
          gs_pCmd->dParm(Size);
          if (pBarsDlg)
            pBarsDlg->SetScale(Size);
          break;
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        {
        if (TagsOn)
          {
          int Col = DEFAULTBARSCOLOUR;
          if (pBarsDlg)
            {
            pBarsDlg->UpdateTagList(TRUE);
            pBarsDlg->UpdateVarList(TRUE);
            d1 = pBarsDlg->m_Min;
            d2 = pBarsDlg->m_Max;
            Size = Max(0.01, pBarsDlg->m_Scale);
            Col = pBarsDlg->FillColour();
            }
          if (TagReq.Length()==0)
            {
            LogError("GrfCmds", 0, "At least one Tag is required");
            break;
            }
          if (VarReq.Length() == 0)
            {
            LogError("GrfCmds", 0, "At least one Variable name is required");
            break;
            }

          Text_Settings TAtt = Text_Settings(Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
          Attr_Settings AAtt = Attr_Settings (50, 0, Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
          MinReq.Clear();
          MaxReq.Clear();
          ColReq.Clear();
          char Buff[64];
          sprintf(Buff, "%g", d1);
          MinReq.Append(Buff);
          sprintf(Buff, "%g", d2);
          MaxReq.Append(Buff);
          sprintf(Buff, "%d", Col);
          ColReq.Append(Buff);

          iDispOpts = 1;
          Like = pDrw->Create_Bars_Block((CExecObj*)pGWnd, VarReq, TagReq, MinReq, MaxReq, ColReq, pDsp->CurrentPt.World, AAtt, TAtt, iDispOpts, True);
          Pt1 = pDsp->CurrentPt;
          if (Like)
            {
            if (pBarsDlg)
              pBarsDlg->SetDoneCreate();
            pGWnd->SetCursor(IDC_BARPOS);
            pDsp->Draw(Like, -1);
            }
          else
            TagsOn = false;
          }
        else
          {
          pDsp->Vp1->ClearAllEntity();
          TagReq.Clear();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          if (pBarsDlg)
            pBarsDlg->UpdateTagList(FALSE);
          }
        TagsOn = !TagsOn;
        if (pBarsDlg)
          pBarsDlg->SetTagsOn(TagsOn);
        }
      break;
    case EX_UNDO :
      /*switch (Modifier)
        {
        case 1:
          if (TagReq.Length() && TagsOn)
            TagReq.Remove(TagReq.Last());
          break;
        case 3:
          if (TagReq.Length())
            TagReq.Remove(TagReq.Last());
          break;
        case 4:
          if (VarReq.Length())
            VarReq.Remove(VarReq.Last());
          break;
        }*/
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      VarReq.Clear();
      TagReq.Clear();
      MinReq.Clear();
      MaxReq.Clear();
      ColReq.Clear();
      if (pBarsDlg)
        {
        pBarsDlg->DestroyWindow();
        delete pBarsDlg;
        pBarsDlg = NULL;
        }
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoUpdBars()
  {
  if (gs_Exec.Busy())
    return;
  static CUpdateBarDlg* pBarsDlg = NULL;
  static DXF_ENTITY TheBars = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          Size = 1.0;
          iDispOpts = 0;
          TheBars = NULL;
          VarReq.Clear();
          TagReq.Clear();
          MinReq.Clear();
          MaxReq.Clear();
          ColReq.Clear();
          pGWnd->SetCursor(IDC_BARSELECT);
          break;
        case 1    : // a digitise
          {
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->AddSelectionAttribList("DYNMTAGS");
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
          pDsp->Vp1->ClrSelectionAllList();
          if (pEnt && pEnt->EntityPtr())
            {
            Strng_List TagL,VarL,MinL,MaxL;
            pchar pTag = Find_Attr_Value(pEnt->EntityPtr(), "DYNMTAGS");
            pchar pVar = Find_Attr_Value(pEnt->EntityPtr(), "DYNMVARS");
            pchar pMin = Find_Attr_Value(pEnt->EntityPtr(), "DYNMMINS");
            pchar pMax = Find_Attr_Value(pEnt->EntityPtr(), "DYNMMAXS");
            pchar pCol = Find_Attr_Value(pEnt->EntityPtr(), "DYNMCOLOR");
            pchar pOpt = Find_Attr_Value(pEnt->EntityPtr(), "DYNMOPTS");
            if (pTag && pVar && pMin && pMax)
              {
              TheBars = pEnt->EntityPtr();
              // retrieve the size of the text from the attribute
              Size = DXF_ATTRIB_HEIGHT(Find_Attr(pEnt->EntityPtr(), "DYNMTAGS"));
              // Retrieve the position
              Pos.Set(DXF_INSERT_PT(pEnt->EntityPtr())[0], DXF_INSERT_PT(pEnt->EntityPtr())[1], DXF_INSERT_PT(pEnt->EntityPtr())[2]);
              // build the lists
              TagReq.Clear();
              VarReq.Clear();
              MinReq.Clear();
              MaxReq.Clear();
              ColReq.Clear();
              TagReq.AppendTokString(pTag, " ");
              VarReq.AppendTokString(pVar, " ");
              MinReq.AppendTokString(pMin, " ");
              MaxReq.AppendTokString(pMax, " ");
              if (pCol)
                ColReq.AppendTokString(pCol, " ");
              if (pOpt && strlen(pOpt)>0)
                iDispOpts = atol(pOpt);
              else
                iDispOpts = 0;

              if (pBarsDlg)
                {
                pBarsDlg->DestroyWindow();
                delete pBarsDlg;
                }
              pBarsDlg = new CUpdateBarDlg(AfxGetMainWnd(), &TagReq, &VarReq, &MinReq, &MaxReq, &ColReq, 0, Size, iDispOpts, true);
              }
            }
          break;
          }
        }
      break;
    case EX_PARAMETER_RDY :
    case EX_MOUSE_LUP :
      break;
    case EX_EXEC :
    case EX_DOIT :
      if (pBarsDlg && TheBars && pBarsDlg->Changed())
        {
        Size = Max(0.01, pBarsDlg->m_Scale);
        Text_Settings TAtt = Text_Settings(Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        Attr_Settings AAtt = Attr_Settings(50, 0, Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        iDispOpts = pBarsDlg->DispOpts();
        pBarsDlg->UpdateLists();
        Like = pDrw->Create_Bars_Block((CExecObj*)pGWnd, VarReq, TagReq, MinReq, MaxReq, ColReq, Pos, AAtt, TAtt, iDispOpts, False);
        if (Like)
          {
          pDsp->Draw(TheBars, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(TheBars);
          pDsp->Draw(Like, -1);
          }
        }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      if (pBarsDlg)
        {
        pBarsDlg->DestroyWindow();
        delete pBarsDlg;
        pBarsDlg = NULL;
        }
      TheBars = NULL;
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoCreateTogg()
  {
  //SETVNT("CReate", "Toggle", "@ * Tags Vars Scale", 1, (CmdFn)&GrfCmdBlk::DoCreateTogg, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static bool TagsOn;
  static CCreateBarDlg* pToggDlg = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MOUSE_LDOWN:
      break;
    case EX_MODIFIER_RDY :
      switch( Modifier )
        {
        case ModNoun :
          gs_pCmd->BlockDblClk();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          VarReq.Clear();
          TagReq.Clear();
          TagsOn = true;
          pDsp->Vp1->ClearAllEntity();
          Like = NULL;
          Size = 0.3;
          break;
        case 1 : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case 2 :
          if (pToggDlg)
            {
            pToggDlg->DestroyWindow();
            delete pToggDlg;
            }
          pToggDlg = new CCreateBarDlg(AfxGetMainWnd(), &TagReq, &VarReq, TagsOn, 0.0, 0.0, DEFAULTTOGGCOLOUR, Size, 2);
          break;
        case 3 : gs_pCmd->SetParmInfo(AName, 1); break;
        case 4 : gs_pCmd->SetParmInfo(AName, 1); break;
        case 5 : gs_pCmd->SetParmInfo(AFloat, 1); break;
        }
      break;
    case EX_MOUSE_LUP:
      switch( Modifier )
        {
        case 1 :
          if (TagsOn)
            {// Select another tagged ent
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            CEntInView* pEnt = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            if (pEnt)
              {
              pchar TagV = Find_Attr_Value(pEnt->EntityPtr(), TagAttribStr);
              if (TagReq.Find(TagV)==NULL)
                {
                TagReq.Append(TagV);
                if (pToggDlg)
                  pToggDlg->UpdateTagList(FALSE);
                }
              }
            }
          else
            {// set position of annotation
            pDsp->Vp1->ClrSelectionAllList();
            if (Like)
              {
              pDsp->Draw(Like, GrfHelper.GR_BACKGROUND);
              pDrw->TranslateEntity(Like, Pt1.World, pDsp->CurrentPt.World);
              Pt1 = pDsp->CurrentPt;
              pDrw->EntityInvalidate(Like, NULL);
              pDsp->Vp1->ClearAllEntity();
              pDsp->Draw(Like, -1);
              }
            }
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 3 :
          {
          Strng s = gs_pCmd->LastToken();
          if (TagReq.Find(s())==NULL)
            {
            TagReq.Append(s());
            if (pToggDlg)
              pToggDlg->UpdateTagList(FALSE);
            }
          break;
          }
        case 4 :
          {
          Strng s = gs_pCmd->LastToken();
          if (VarReq.Find(s())==NULL)
            {
            VarReq.Append(s());
            if (pToggDlg)
              pToggDlg->UpdateVarList(FALSE);
            }
          break;
          }
        case 5 :
          gs_pCmd->dParm(Size);
          if (pToggDlg)
            pToggDlg->SetScale(Size);
          break;
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        {
        if (TagsOn)
          {
          if (pToggDlg)
            {
            pToggDlg->UpdateTagList(TRUE);
            pToggDlg->UpdateVarList(TRUE);
            Size = Max(0.01, pToggDlg->m_Scale);
            }
          if (TagReq.Length()==0)
            {
            LogError("GrfCmds", 0, "At least one Tag is required");
            break;
            }
          if (VarReq.Length() == 0)
            {
            LogError("GrfCmds", 0, "At least one Variable name is required");
            break;
            }
          Text_Settings TAtt = Text_Settings(Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
          Attr_Settings AAtt = Attr_Settings(50, 0, Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
          Like = pDrw->Create_Toggle_Block((CExecObj*)pGWnd, VarReq, TagReq, pDsp->CurrentPt.World, AAtt, TAtt, Size);
          Pt1 = pDsp->CurrentPt;
          if (Like)
            {
            /*Pt_3i Pt;
            Pt.Set(0,0,0);
            pDsp->Draw(Like, -1);
            pDsp->SetCursImgBase();
            pDsp->SetCursEntity(pDrw, Like);
            gs_pCmd->SetDigInfo(GC_DListCurs, 1);*/
            if (pToggDlg)
              pToggDlg->SetDoneCreate();
            pGWnd->SetCursor(IDC_TOGGPOS);
            pDsp->Draw(Like, -1);
            }
          else
            TagsOn = false;
          }
        else
          {
          pDsp->Vp1->ClearAllEntity();
          TagReq.Clear();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          if (pToggDlg)
            pToggDlg->UpdateTagList(FALSE);
          }
        TagsOn = !TagsOn;
        if (pToggDlg)
          pToggDlg->SetTagsOn(TagsOn);
        }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      VarReq.Clear();
      TagReq.Clear();
      if (pToggDlg)
        {
        pToggDlg->DestroyWindow();
        delete pToggDlg;
        pToggDlg = NULL;
        }
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoUpdTogg()
  {
  //SETVNT("UPdate", "Toggle", "@ *", 1, (CmdFn)&GrfCmdBlk::DoUpdTogg, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static CUpdateBarDlg* pBarsDlg = NULL;
  static DXF_ENTITY TheTogg = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          Size = 1.0;
          TheTogg = NULL;
          VarReq.Clear();
          TagReq.Clear();
          pGWnd->SetCursor(IDC_TOGGSELECT);
          break;
        case 1    : // a digitise
          {
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->AddSelectionAttribList("TOGGTAGS");
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
          pDsp->Vp1->ClrSelectionAllList();
          if (pEnt && pEnt->EntityPtr())
            {
            Strng_List TagL,VarL;
            pchar pTag = Find_Attr_Value(pEnt->EntityPtr(), "TOGGTAGS");
            pchar pVar = Find_Attr_Value(pEnt->EntityPtr(), "TOGGVARS");
            if (pTag && pVar)
              {
              TheTogg = pEnt->EntityPtr();
              // retrieve the size of the text from the attribute
              Size = DXF_ATTRIB_HEIGHT(Find_Attr(pEnt->EntityPtr(), "TOGGTAGS"));
              // Retrieve the position
              Pos.Set(DXF_INSERT_PT(pEnt->EntityPtr())[0], DXF_INSERT_PT(pEnt->EntityPtr())[1], DXF_INSERT_PT(pEnt->EntityPtr())[2]);
              // build the lists
              TagReq.Clear();
              VarReq.Clear();
              TagReq.AppendTokString(pTag, " ");
              VarReq.AppendTokString(pVar, " ");
              if (pBarsDlg)
                {
                pBarsDlg->DestroyWindow();
                delete pBarsDlg;
                }
              pBarsDlg = new CUpdateBarDlg(AfxGetMainWnd(), &TagReq, &VarReq, &MinReq, &MaxReq, NULL, 0, Size, 0, false);
              }
            }
          break;
          }
        }
      break;
    case EX_PARAMETER_RDY :
    case EX_MOUSE_LUP :
      break;
    case EX_EXEC :
    case EX_DOIT :
      if (pBarsDlg && TheTogg && pBarsDlg->Changed())
        {
        Size = Max(0.01, pBarsDlg->m_Scale);
        Text_Settings TAtt = Text_Settings(Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        Attr_Settings AAtt = Attr_Settings(50, 0, Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        pBarsDlg->UpdateLists();
        Like = pDrw->Create_Toggle_Block((CExecObj*)pGWnd, VarReq, TagReq, Pos, AAtt, TAtt, Size);
        if (Like)
          {
          pDsp->Draw(TheTogg, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(TheTogg);
          pDsp->Draw(Like, -1);
          }
        }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      if (pBarsDlg)
        {
        pBarsDlg->DestroyWindow();
        delete pBarsDlg;
        pBarsDlg = NULL;
        }
      TheTogg = NULL;
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoNewCreateAnno()
  {
  //SETVNT("CReate", "Annotation", "@ * Size", 1, (CmdFn)&GrfCmdBlk::DoNewCreateAnno, EF_ERS);
  if (gs_Exec.Busy())
    return;
  static bool TagsOn;
  static CCreateBarDlg* pAnnotDlg = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MOUSE_LDOWN:
      break;
    case EX_MODIFIER_RDY :
      switch( Modifier )
        {
        case ModNoun :
          ActivateGWnd();
          gs_pCmd->BlockDblClk();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          TagReq.Clear();
          TLbReq.Clear();
          VarReq.Clear();
          VLbReq.Clear();
          FmtReq.Clear();
          CvsReq.Clear();
          TagsOn = true;
          pDsp->Vp1->ClearAllEntity();
          Like = NULL;
          Size = 1.5;
          break;
        case 1 : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case 2 :
          if (pAnnotDlg)
            {
            pAnnotDlg->DestroyWindow();
            delete pAnnotDlg;
            }
          pAnnotDlg = new CCreateBarDlg(AfxGetMainWnd(), &TagReq, &VarReq, TagsOn, 0.0, 0.0, DEFAULTBARSCOLOUR, Size, 1);
          break;
        case 3 : gs_pCmd->SetParmInfo(AFloat, 1); break;
        }
      break;
    case EX_MOUSE_LUP:
      switch( Modifier )
        {
        case 1 :
          if (TagsOn)
            {// Select another tagged ent
            pDsp->Vp1->ClrSelectionAllList();
            pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
            pDsp->Vp1->AddSelectionAttribList(TagAttribStr);
            CEntInView* pEnt = pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
            if (pEnt)
              {
              pchar TagV = Find_Attr_Value(pEnt->EntityPtr(), TagAttribStr);
              if (TagReq.Find(TagV)==NULL)
                {
                TagReq.Append(TagV);
                if (pAnnotDlg)
                  pAnnotDlg->UpdateTagList(FALSE);
                }
              }
            }
          else
            {// set position of annotation
            pDsp->Vp1->ClrSelectionAllList();
            if (Like)
              {
              pDsp->Draw(Like, GrfHelper.GR_BACKGROUND);
              pDrw->TranslateEntity(Like, Pt1.World, pDsp->CurrentPt.World);
              Pt1 = pDsp->CurrentPt;
              pDrw->EntityInvalidate(Like, NULL);
              pDsp->Vp1->ClearAllEntity();
              pDsp->Draw(Like, -1);
              }
            }
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 3 :
          gs_pCmd->dParm(Size);
          if (pAnnotDlg)
            pAnnotDlg->SetScale(Size);
          break;
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        {
        if (TagsOn)
          {
          if (pAnnotDlg)
            {
            pAnnotDlg->UpdateTagList(TRUE);
            pAnnotDlg->UpdateVarList(TRUE);
            Size = Max(0.01, pAnnotDlg->m_Scale);
            }
          if (TagReq.Length()==0)
            {
            LogError("GrfCmds", 0, "At least one Tag is required");
            break;
            }
          if (VarReq.Length() == 0)
            {
            LogError("GrfCmds", 0, "At least one Variable name is required");
            break;
            }

          if (FmtReq.Length()==0 || FmtReq.Length()!=VarReq.Length())
            {
            FmtReq.Clear();
            for (int i=0; i<VarReq.Length(); i++)
              FmtReq.Append("*.##");
            }
          if (CvsReq.Length()==0 || CvsReq.Length()!=VarReq.Length())
            {
            CvsReq.Clear();
            for (int i=0; i<VarReq.Length(); i++)
              CvsReq.Append("*");
            }
          char* pTxt;
          pStrng ptmp;
          if (TLbReq.Length()!=TagReq.Length())
            {
            TLbReq.Clear();
            for (pTxt = TagReq.First()->Str(); pTxt; (ptmp = TagReq.Next()) ? (pTxt = ptmp->Str()) : (pTxt = NULL) )
              TLbReq.Append(pTxt);
            }
          if (VLbReq.Length()!=VarReq.Length())
            {
            VLbReq.Clear();
            for (pTxt = VarReq.First()->Str(); pTxt; (ptmp = VarReq.Next()) ? (pTxt = ptmp->Str()) : (pTxt = NULL) )
              VLbReq.Append(pTxt);
            }
          Attr_Settings AAtt = Attr_Settings(50, 0, Size, 0.8, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
          Like = pDrw->Create_NewAnnotation_Block((CExecObj*)pGWnd, TagReq, TLbReq, VarReq, VLbReq, FmtReq, CvsReq, pDsp->CurrentPt.World, AAtt, Size);
          Pt1 = pDsp->CurrentPt;
          if (Like)
            {
            /*Pt_3i Pt;
            Pt.Set(0,0,0);
            pDsp->Draw(Like, -1);
            pDsp->SetCursImgBase();
            pDsp->SetCursEntity(pDrw, Like);
            gs_pCmd->SetDigInfo(GC_DListCurs, 1);*/
            if (pAnnotDlg)
              pAnnotDlg->SetDoneCreate();
            pGWnd->SetCursor(IDC_ANNOTPOS);
            pDsp->Draw(Like, -1);
            }
          else
            TagsOn = false;
          }
        else
          {
          pDsp->Vp1->ClearAllEntity();
          TagReq.Clear();
          pGWnd->SetCursor(IDC_SELECTUNIT);
          if (pAnnotDlg)
            pAnnotDlg->UpdateTagList(FALSE);
          }
        TagsOn = !TagsOn;
        if (pAnnotDlg)
          pAnnotDlg->SetTagsOn(TagsOn);
        }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      TagReq.Clear();
      TLbReq.Clear();
      VarReq.Clear();
      VLbReq.Clear();
      FmtReq.Clear();
      CvsReq.Clear();
      if (pAnnotDlg)
        {
        pAnnotDlg->DestroyWindow();
        delete pAnnotDlg;
        pAnnotDlg = NULL;
        }
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoNewRefreshAnno()
  {
  //SETVNT("REFresh", "Annotation", "All", -1, (CmdFn)&GrfCmdBlk::DoNewRefreshAnno, EF_ERS);
  if (When==EX_MODIFIER_RDY)
    {
    pDsp->Open();
    switch( Modifier )
      {
      case ModNoun : bFlag1 = 0; break;//refresh all annotation
      case 1 : bFlag1 = 1; break;
      }
    pDsp->Close();
    return;
    }
  if (When != EX_EXEC)
    return;
  if (bFlag1)
    {
    pGWnd->SendCmdToAll("refresh annotation\r");
    return;
    }
  CWaitMsgCursor Wait("Updating annotation blocks");
  static char* aval[] = { "ANNOT_TAGS", "ANNOT_VARS", (pchar)NULL };
  static char* bval[] = { "", "", (pchar)NULL };
  DXF_ENTITY OldIns,NewIns;
  pDsp->Open();
  pDsp->Vp1->ClearAllEntity();
  pDrw->SelectInsertsOnAttrCombo(NULL, aval, bval, NULL);
  CEntInView* pEnt = pDsp->Vp1->FirstSelectedEntity();
  while (pEnt)
    {
    Size = DXF_ATTRIB_HEIGHT(Find_Attr(pEnt->EntityPtr(), "ANNOT_TAGS"));
    Attr_Settings AAtt = Attr_Settings(50, 0, Size, 0.8, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
    OldIns = pEnt->EntityPtr();
    NewIns = pDrw->Update_NewAnnotation_Block(OldIns, (CExecObj*)pGWnd, AAtt, Size);
    pDsp->Draw(OldIns, GrfHelper.GR_BACKGROUND);
    pDrw->Delete(OldIns);
    pDsp->Draw(NewIns, -1);
    pEnt = pDsp->Vp1->NextSelectedEntity();
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoNewUpdAnno()
  {
  if (gs_Exec.Busy())
    return;
  static CUpdateAnnotDlg1* pAnnotDlg = NULL;
  static DXF_ENTITY TheAnnot = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          Size = 1.0;
          TheAnnot = NULL;
          VarReq.Clear();
          TagReq.Clear();
          pGWnd->SetCursor(IDC_ANNOTSELECT);
          break;
        case 1    : // a digitise
          {
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
          pDsp->Vp1->AddSelectionAttribList("ANNOT_TAGS");
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
          pDsp->Vp1->ClrSelectionAllList();
          if (pEnt && pEnt->EntityPtr())
            {
            pchar pTag = Find_Attr_Value(pEnt->EntityPtr(), "ANNOT_TAGS");
            pchar pTLb = Find_Attr_Value(pEnt->EntityPtr(), "ANNOT_TAGLBLS");
            pchar pVar = Find_Attr_Value(pEnt->EntityPtr(), "ANNOT_VARS");
            pchar pVLb = Find_Attr_Value(pEnt->EntityPtr(), "ANNOT_VARLBLS");
            pchar pFmt = Find_Attr_Value(pEnt->EntityPtr(), "ANNOT_TAGFMTS");
            pchar pCvs = Find_Attr_Value(pEnt->EntityPtr(), "ANNOT_TAGCNVS");
            if (pTag && pTLb && pVar && pVLb && pFmt && pCvs)
              {
              TheAnnot = pEnt->EntityPtr();
              // retrieve the size of the text from the attribute
              Size = DXF_ATTRIB_HEIGHT(Find_Attr(pEnt->EntityPtr(), "ANNOT_TAGS"));
              // Retrieve the position
              Pos.Set(DXF_INSERT_PT(pEnt->EntityPtr())[0], DXF_INSERT_PT(pEnt->EntityPtr())[1], DXF_INSERT_PT(pEnt->EntityPtr())[2]);
              // build the lists
              TagReq.Clear();
              TLbReq.Clear();
              VarReq.Clear();
              VLbReq.Clear();
              FmtReq.Clear();
              CvsReq.Clear();
              TagReq.AppendTokString(pTag, " ");
              TLbReq.AppendTokString(pTLb, " ");
              VarReq.AppendTokString(pVar, " ");
              VLbReq.AppendTokString(pVLb, " ");
              FmtReq.AppendTokString(pFmt, " ");
              CvsReq.AppendTokString(pCvs, " ");
              if (pAnnotDlg)
                {
                pAnnotDlg->DestroyWindow();
                delete pAnnotDlg;
                }
              pAnnotDlg = new CUpdateAnnotDlg1(AfxGetMainWnd(), &TagReq, &TLbReq, &VarReq, &VLbReq, &FmtReq, &CvsReq, Size);
              }
            }
          break;
          }
        }
      break;
    case EX_PARAMETER_RDY :
    case EX_MOUSE_LUP :
      break;
    case EX_EXEC :
    case EX_DOIT :
      if (pAnnotDlg && TheAnnot && pAnnotDlg->Changed())
        {
        Size = Max(0.01, pAnnotDlg->m_Scale);
        //Text_Settings TAtt = Text_Settings(Size, 1.0, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        Attr_Settings AAtt = Attr_Settings(50, 0, Size, 0.8, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        pAnnotDlg->UpdateLists();
        //Like = pDrw->Create_Annotation_Block((CExecObj*)pGWnd, VarReq, FmtReq, TagReq, Pos, AAtt, Size);
        Like = pDrw->Create_NewAnnotation_Block((CExecObj*)pGWnd, TagReq, TLbReq, VarReq, VLbReq, FmtReq, CvsReq, Pos, AAtt, Size);
        if (Like)
          {
          pDsp->Draw(TheAnnot, GrfHelper.GR_BACKGROUND);
          pDrw->Delete(TheAnnot);
          pDsp->Draw(Like, -1);
          }
        }
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      if (pAnnotDlg)
        {
        pAnnotDlg->DestroyWindow();
        delete pAnnotDlg;
        pAnnotDlg = NULL;
        }
      TheAnnot = NULL;
      pGWnd->SetCursor();
      pDsp->Vp1->ClearAllEntity();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoAlterText()
  {
  if (gs_Exec.Busy())
    return;
#define UseBlockAndAttrib 0

  //SETVNT("ALTer",     "TExt", "@ * ; Create Update Apply Scale Rotation Txt", 1, (CmdFn)&GrfCmdBlk::DoAlterText, EF_ERS);
  enum MId { MID_Dig=1, MID_Dlg, MID_Next, MID_Create, MID_Update, MID_Apply, MID_Scale, MID_Rotation, MID_Txt };
  static CGrfTextDlg* pTextDlg = NULL;
  pDsp->Open();
  bool doApply = false;
  bool doMove = false;
  bool doClear = false;
  switch (When)
    {
    case EX_MOUSE_LDOWN:
      break;
    case EX_MODIFIER_RDY :
      switch( Modifier )
        {
        case ModNoun :
          gs_pCmd->BlockDblClk();
          ActivateGWnd();
          pDsp->Vp1->ClearAllEntity();
          pDsp->Vp1->ClrSelectionAllList();
          TextSize = 2.0;
          Rotation = 0.0;
          TextWidth = 0.8;
          AText = "";
          OldEntity = NULL; //placed text
          bFlag1 = 1; //create text mode
          break;
        case MID_Dig : gs_pCmd->SetDigInfo(GC_BigCurs, 1); break;
        case MID_Dlg :
          if (pTextDlg)
            {
            pTextDlg->DestroyWindow();
            delete pTextDlg;
            }
          pTextDlg = new CGrfTextDlg(AfxGetMainWnd());
          //pTextDlg->SetData(TextSize, TextWidth, Rotation);
          pTextDlg->SetCreateMode(bFlag1);
          pTextDlg->SetTextSelected(OldEntity!=NULL || !bFlag1);
          pGWnd->SetCursor((!bFlag1 && OldEntity==NULL) ? IDC_SELECTTEXT : IDC_POS);
          break;
        case MID_Create : bFlag1 = 1; doClear = true; break;
        case MID_Update : bFlag1 = 0; doClear = true; break;
        case MID_Apply : doApply = true; break;
        case MID_Next : doClear = true; break;
        case MID_Scale : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Rotation : gs_pCmd->SetParmInfo(AFloat, 1); break;
        case MID_Txt : gs_pCmd->SetParmInfo(AName, 1); break;
        }
      break;
    case EX_MOUSE_LUP:
      switch( Modifier )
        {
        case MID_Dig :
          {
          if (!bFlag1 && OldEntity==NULL)
            {//find text entity...
            pDsp->Vp1->ClearAllEntity();
            pDsp->Vp1->ClrSelectionAllList();
            CEntInView* p = GetClosestText(pDsp->CurrentPt);
            if (p)
              {
              OldEntity = p->EntityPtr();
              TextSize = DXF_TEXT_HEIGHT(OldEntity);
              Rotation = DXF_TEXT_ROT_ANG(OldEntity);
              TextWidth = DXF_TEXT_X_SCALE(OldEntity);
              AText = DXF_TEXT_VALUE_GET(OldEntity);
              Pos.Set(DXF_TEXT_INS_PT(OldEntity)[0], DXF_TEXT_INS_PT(OldEntity)[1], DXF_TEXT_INS_PT(OldEntity)[2]);
              pDsp->SetCPtWorld(Pos, Pt1);
              if (pTextDlg)
                {
                pTextDlg->SetData(TextSize, TextWidth, Rotation);
                pTextDlg->SetText(AText());
                }
              pDsp->Vp1->SelectEntity(p);
              }
            else
              {
              pDsp->Vp1->ClearAllEntity();
              pDsp->Vp1->ClrSelectionAllList();
              }
            pTextDlg->SetTextSelected(OldEntity!=NULL || !bFlag1);
            pGWnd->SetCursor((!bFlag1 && OldEntity==NULL) ? IDC_SELECTTEXT : IDC_POS);
            }
          else
            {//update and position text...
            doApply = true;
            doMove = true;
            }
          break;
          }
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case MID_Scale :
          gs_pCmd->dParm(TextSize);
          if (pTextDlg)
            pTextDlg->SetScale(TextSize);
          break;
        case MID_Rotation :
          gs_pCmd->dParm(Rotation);
          if (pTextDlg)
            pTextDlg->SetRotation(Rotation);
          break;
        case MID_Txt :
          AText = gs_pCmd->LastToken();
          if (pTextDlg)
            pTextDlg->SetText(AText());
          break;
        }
      break;
    case EX_EXEC :
    case EX_DOIT :
      break;
    case EX_TERM_SEMICOLON :
      if (!gs_pCmd->IsEscBusy())
        doClear = true;
      break;
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      pDsp->Vp1->ClearAllEntity();
      if (pTextDlg)
        {
        pTextDlg->DestroyWindow();
        delete pTextDlg;
        pTextDlg = NULL;
        }
      OldEntity = NULL;
      pGWnd->SetCursor();
      gs_pCmd->FreeDblClk();
      break;
    }

  if (doApply)
    {
    if (pTextDlg)
      {
      pTextDlg->CompleteForUse();
      AText = pTextDlg->m_sText;
      TextSize = pTextDlg->m_TextScale;
      TextWidth = pTextDlg->m_TextWidth;
      Rotation = pTextDlg->m_Rotation;
      }
    if (Rotation<0.0)
      Rotation += 360.0;
    AText.LRTrim(" \t\r");
    if (AText.Len()==0)
      {
      AText = "Text";
      if (pTextDlg)
        pTextDlg->SetText(AText());
      }
    if (OldEntity==NULL)
      {
      if (doMove)
        {
        #if UseBlockAndAttrib
        Attr_Settings AAtt = Attr_Settings(50, 0, TextSize, TextWidth, 0.0, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        OldEntity = pDrw->Create_NewText_Block(AText(), pDsp->CurrentPt.World, AAtt, TextSize);
        #else
        //Text_Settings TAtt = Text_Settings(TextSize, TextWidth, Rotation, GR_DARKGRAY, Text_Cntrd, Text_Baseline, "STANDARD");
        Text_Settings TAtt = Text_Settings(TextSize, TextWidth, Rotation, GR_DARKGRAY, Text_Left, Text_Baseline, "STANDARD");
        PT3 pnt;
        pnt[0] = pDsp->CurrentPt.World.X;
        pnt[1] = pDsp->CurrentPt.World.Y;
        pnt[2] = pDsp->CurrentPt.World.Z;
        OldEntity = pDrw->Add_Text(AText(), pnt, TAtt);
        //NOTE: text gets added to OtherLst in drawing, perhaps it should be InsertLst!?!
        #endif
        Pt1 = pDsp->CurrentPt;
        }
      }
    else
      {
      pDsp->Draw(OldEntity, GrfHelper.GR_BACKGROUND);
      if (doMove)
        {
        pDrw->TranslateEntity(OldEntity, Pt1.World, pDsp->CurrentPt.World);
        Pt1 = pDsp->CurrentPt;
        }
      DXF_TEXT_HEIGHT(OldEntity) = TextSize;
      DXF_TEXT_ROT_ANG(OldEntity) = Rotation;
      DXF_TEXT_X_SCALE(OldEntity) = TextWidth;
      DXF_TEXT_VALUE_SET(OldEntity, AText());
      pDrw->EntityInvalidate(OldEntity, NULL);
      }
    if (OldEntity)
      {
      pDsp->Draw(OldEntity, -1);
      pDsp->Vp1->ClearAllEntity();
      pDsp->Vp1->ClrSelectionAllList();
      CEntInView* p = GetClosestEntity(Pt1, OldEntity);
      if (p)
        pDsp->Vp1->SelectEntity(p);
      }
    pTextDlg->SetTextSelected(OldEntity!=NULL || !bFlag1);
    }

  if (doClear)
    {
    OldEntity = NULL; 
    pDsp->Vp1->ClearAllEntity();
    pDsp->Vp1->ClrSelectionAllList();
    if (pTextDlg)
      {
      pTextDlg->SetCreateMode(bFlag1);
      pTextDlg->SetTextSelected(OldEntity!=NULL || !bFlag1);
      }
    pGWnd->SetCursor((!bFlag1 && OldEntity==NULL) ? IDC_SELECTTEXT : IDC_POS);
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoCfgLayers()
  {
  //SETVNT("CONFigure", "LAyers", "* LAyer_name HIde SHow COlor", -1, (CmdFn)&GrfCmdBlk::DoCfgLayers, EF_ERS);
  static CLayersDlg* pLayDlg = NULL;
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          ActivateGWnd();
          ALayer = "";
          i1 = 0; //color
          break;
        case 1 : //dialog
          if (pLayDlg)
            {
            pLayDlg->DestroyWindow();
            delete pLayDlg;
            }
          pLayDlg = new CLayersDlg(this, AfxGetMainWnd());
          break;
        case 2 : //Layer_name
          gs_pCmd->SetParmInfo(AName, 1);
          break;
        case 3 : //Hide
          if (ALayer.Length()>0 && pDrw->SetLayerState(pDsp->Vp1, ALayer(), 1, 0x01))
            {
            if (pLayDlg==NULL)
              pWnd->Invalidate();
            }
          break;
        case 4 : //Show
          if (ALayer.Length()>0 && pDrw->SetLayerState(pDsp->Vp1, ALayer(), 0, 0x01))
            {
            if (pLayDlg==NULL)
              pWnd->Invalidate();
            }
          break;
        case 5 : //Color
          gs_pCmd->SetParmInfo(AnInt, 1);
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 2 :
          ALayer = gs_pCmd->LastToken();
          break;
        case 5 :
          gs_pCmd->iParm(i1);
          if (ALayer.Length()>0 && pDrw->SetLayerColor(pDsp->Vp1, ALayer(), i1))
            {
            if (pLayDlg==NULL)
              pWnd->Invalidate();
            }
          break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 :
        case 2 :
        case 3 :
          break;
        default :;
        }
      break;
    case EX_DOIT :
    case EX_UNDO :
      break;
    case EX_RESULTS :
      DumpErrors();
    case EX_ALLUNDONE :
      if (pLayDlg)
        {
        pLayDlg->DestroyWindow();
        delete pLayDlg;
        pLayDlg = NULL;
        }
      break;
    default :;
    }
  pDsp->Close();
  }

//===========================================================================

DXF_ENTITY GrfCmdBlk::AddUnitDrawing(char* TagBase_, char* DrawTyp_, char* ModelTyp, char* Guid, char* Tag, char* AssocTag, Pt_3f Pt, Pt_3f Scl, float Rotate, flag CompleteBlock, Attr_Settings &ASet, Pt_3f * TagPt)
  {
  if (DrawTyp_==NULL)
    DrawTyp_="??";
  if (ModelTyp==NULL)
    ModelTyp="??";
  char DrawTyp[_MAX_PATH];
  strcpy(DrawTyp, DrawTyp_);
  _strlwr(DrawTyp);
  char* p = strstr(DrawTyp, ".dxf");
  if (p)
    *p = 0;
  char *pStar=strchr(DrawTyp, '*');
  if (pStar)
    *pStar='$';

  CString DrawBlockName=MakeValidBlockName(DrawTyp);

  // See if an block of this name exists and is loaded...
  Block* b = pDrw->Blocks.Find((LPTSTR)(LPCTSTR)DrawBlockName);
  if (!b)
    {
    if (GrfSymbolImportFiles())
      b = pDrw->LoadFromLib(GrfSymbolImportFiles(), DrawTyp, (LPTSTR)(LPCTSTR)DrawBlockName);
    if (b==NULL)
      b = pDrw->LoadFromLib(GrfSymbolFiles(), DrawTyp, (LPTSTR)(LPCTSTR)DrawBlockName);
    if (b==NULL)
      b = pDrw->LoadFromLib(BaseGrfSymbolFiles(), DrawTyp, (LPTSTR)(LPCTSTR)DrawBlockName);
    }

  if (!b)
    {//Unknown drawing symbol, try revert to default...
    TagObjClass * pTagObjC = TagObjClass::FindClassId(ModelTyp);
    if (pTagObjC==NULL)
      pTagObjC = TagObjClass::FindClassName(ModelTyp);
    if (pTagObjC)
      {
      DrawBlockName.Format("%s:%s", pTagObjC->DrwGroup(), ModelTyp);
      DrawBlockName=MakeValidBlockName((char*)(LPCSTR)DrawBlockName);
      b = pDrw->Blocks.Find((LPTSTR)(LPCTSTR)DrawBlockName);
      if (!b)
        LogNote("GrfCmds", 0, "Drawing '%s' not found for '%s', use default", DrawTyp_, Tag?Tag:"Symbol");
      }
    }

//LogNote("xx",0,"%s",DrawBlockName);
  flag DoIt = 0;
  if (b)
    {
    DrawBlockName=DXF_BLOCK_NAME_GET(b->Def);
    DoIt = 1;
    }
  else if (gs_pPrj != NULL)
    {
    pDrw->AddNodeGrfInfo(gs_pPrj->GetNodeDrawings());
    b = pDrw->Blocks.Find((LPTSTR)(LPCTSTR)DrawBlockName);
    if (b)
      DoIt = 1;
    }

  if (DoIt)
    {
    bool TagHidden = (ASet.Flags & DXF_ATTRIB_INVIS) != 0;
    Attr_Settings TagAttSet(ASet);
    double Xto = 0.0;
    double Yto = 0.0;
    if (TagPt && Valid(TagPt->X))
      {//tag position set explicitly
      Xto=TagPt->X;
      Yto=TagPt->Y;
      }
    else 
      {
      Pt_3f Pos;
      if (b->Find_Attdef_Settings(TagAttribStr, TagAttSet, Pos))
        {//get tag position from attrib definition
        Xto=Pos.X;
        Yto=Pos.Y;
        }
      else if (b->Find_TextToAttrib(TagAttribStr, Pos))
        {//get tag position from text
        Xto=Pos.X;
        Yto=Pos.Y;
        }
      }
    TagAttSet.Flags=TagHidden ? DXF_ATTRIB_INVIS : 0;
    Pt_3f Ptt(Xto * Scl.X, Yto * Scl.Y, 0.);

    #if dbgdrw
    dbgpln("Add Tag Sc:%5.2f,%5.2f  Pt:%6.1f,%6.1f  Ptt:%6.1f,%6.1f    Blk:%s Tag:%s",
      Scl.X, Scl.Y, Pt.X, Pt.Y, Ptt.X, Ptt.Y, (LPCTSTR)DrawBlockName, Tag);
    #endif

    DXF_ENTITY e = pDrw->Create_Insert(DXF_BLOCK_NAME_GET(b->Def), Pt, GR_LIGHTGREEN, Scl, Rotate, Guid, Tag, AssocTag, Ptt, TagAttSet);
    if (AssocTag)
          pDrw->GetBounds();
    return e;
    }
  else
    {
    LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Drawing '%s' not inserted for '%s' (%s)", DrawBlockName, Tag, ModelTyp);
    return NULL;
    }
  }

//---------------------------------------------------------------------------

int GrfCmdBlk::AddUnitModel(char* ModelTyp, char* Tag, pchar pGuidStr)
  {
  int err = -1;
  if (!gs_License.NodeCountExceeded(1, eLic_MsgBox))
    {
    if (strlen(Tag)>SCDPartTagLen-2)
      {
      LogError("GrfCmds", 0, "Model not inserted, tag name is too long (%s)", Tag);
      }
    else
      {
      CMdlValueSet::Clear();
      char * SubClass=NULL; // tobe Set / passed into this subroutine
      err = gs_pPrj->AddNodeModel(ModelTyp, SubClass, Tag, pGuidStr);
      if (err)
        LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Model not inserted[%i]\n%s:%s", err, ModelTyp, Tag);
      }
    }
  return err;
  }

//---------------------------------------------------------------------------

DXF_ENTITY GrfCmdBlk::AddLink(pchar Typ, pchar Guid, pchar Tag,
           pchar SrcTag, pchar SrcOut,
           pchar DstTag, pchar DstIn,
           double ArrowScale,
           //double xLength, double xDiameter,
           pPt_SLW Verts, int NVerts, Attr_Settings &ASet)
  {
  DXF_ENTITY newinsert;
  Strng InsMsg;
  C3_CURVE Link = NULL;
  double big = 0.0;
  int i,bigi = 0;
  for (i=1; i<NVerts; i++)          // find the longest segment on the curve
    {
    Pt_3f & p0 = Verts[i - 1].World;
    Pt_3f & p1 = Verts[i].World;
    double d = p1.Dist(p0);
    if (d>big)
      {
      big = d;
      bigi = i;
      }
    }
  if (!bigi)
    return NULL;

  Pt_3f & p0 = Verts[bigi - 1].World;
  Pt_3f & p1 = Verts[bigi].World;
  REAL s = 2.5;
  Pt_3f sc(1.0, 1.0, 1.0);
  Pt_3f pti((p1.X + p0.X) * 0.5, (p1.Y + p0.Y) * 0.5, (p1.Z + p0.Z) * 0.5);
  Pt_3f ptt(0., s, 0.);
  REAL ang = atan2(p1.Y - p0.Y, p1.X - p0.X);

  Pt_3f basept, vpt;
  basept.Zero();

  char nm[1024];
  pchar KernalName = "LNKBLK_";
  int unum = pDrw->Blocks.FindUnique(KernalName, -1);
  sprintf(nm, "%s_%d", KernalName, Max(1, unum));

  pBlock b = pDrw->Add_Block(nm, basept.p());
  DXF_ENTITY a = b->Add_Attrib_Defn(TagAttribStr, "Tag ", "*", basept.p(), ASet);
  vpt.Zero();
  C3_CURVE p = b->Add_PLine_Start(vpt.p());
  vpt.Set(-1., 0.2, 0.);
  b->Add_PLine_Vertex(p, vpt.p());
  vpt.Set(-1., -0.2, 0.);
  b->Add_PLine_Vertex(p, vpt.p());
  vpt.Zero();
  b->Add_PLine_Vertex(p, vpt.p());
  vpt.Set(-0.8, 0.08, 0.);
  b->Add_PLine_Vertex(p, vpt.p());
  vpt.Set(-0.8, -0.08, 0.);
  b->Add_PLine_Vertex(p, vpt.p());
  vpt.Zero();
  b->Add_PLine_Vertex(p, vpt.p());

  // scale and rotate the arrowhead
  PT3 o,v;
  o[0] = 0.0;  o[1] = 0.0;  o[2] = 0.0;
  v[0] = 0.0;  v[1] = 0.0;  v[2] = 1.0;
  c3c_scale(p, o, (REAL)ArrowScale);            // size 2.5 mm
  c3c_rotate(p, o, v, (REAL)ang);

  // now add the line to the block
  for (i=0; i<NVerts; i++)
    {
    Pt_3f & ptmp = Verts[i].World;

    ptmp.X -= pti.X;
    ptmp.Y -= pti.Y;
    ptmp.Z -= pti.Z;

    if (Link)
      b->Add_PLine_Vertex(Link, ptmp.p());
    else
      Link = b->Add_PLine_Start(ptmp.p());
    }

  newinsert = pDrw->Create_Insert(nm, pti, GR_WHITE/*GR_LIGHTCYAN*/, sc, 0.0, Guid, Tag, NULL, ptt, ASet);

  if (Typ)
    {
    CMdlValueSet::Clear();
    int err = gs_pPrj->AddNodeConnect(Typ, Tag, SrcTag, SrcOut, DstTag, DstIn);
    if (err)
      {
      //Strng Msg;
      LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Connection not Made:\n%s:%s\n%s.%s->%s.%s", Typ, Tag, SrcTag, SrcOut, DstTag, DstIn);
      //LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, Msg());
      //AfxMessageBox(Msg(), MB_ICONEXCLAMATION);
  //===========================================================================
  // NOTE: NBNBNB: Using LogError, LogWarning etc with second parameter non-zero
  //       eg MB_OK then because the SysCAD message window displays a pop-up
  //       message window then the cmdblk "loses focus" resulting in calls such
  //       as EX_RESULTS not to be called. ie DO NOT use LogXXX with a pop-up in
  //       the middle of processing a command. This seems to happen if a number
  //       of commands on the same line (including a \r ??) are being processed.
  //       You have been warned!!!
  //===========================================================================
      }
    }
  return newinsert;
  }

//---------------------------------------------------------------------------

int GrfCmdBlk::AddLinkModel(char* Typ, char* Tag, char* SrcTag, char* SrcOut, char* DstTag, char* DstIn)
  {
  int OK = False;
  if (Typ)
    {
    CMdlValueSet::Clear();
    int err = gs_pPrj->AddNodeConnect(Typ, Tag, SrcTag, SrcOut, DstTag, DstIn);
    if (err)
      {
      LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Connection not Made:\n%s:%s\n%s.%s->%s.%s", Typ, Tag, SrcTag, SrcOut, DstTag, DstIn);
      }
    else
      OK = True;
    }
  return OK;
  }

//---------------------------------------------------------------------------

DXF_ENTITY GrfCmdBlk::AddLinkDrawing(CLineDrawHelper & LDH)
  {
  int LongSegIndex = LDH.FindLongestSegment();
  if (LongSegIndex<0)
    return NULL;

  LDH.GetMidPoint(LongSegIndex, LDH.m_InsertPt);

  char nm[512];
  char* KernalName = "LNKBLK_";
  const int unum = pDrw->Blocks.FindUnique(KernalName, -1);
  sprintf(nm, "%s_%d", KernalName, Max(1, unum));

  Pt_3f basept;
  //create block...
  LDH.m_pBlock = pDrw->Add_Block(nm, basept.p());
  //add tag...
  Tag_Attr_Set.Flags = (LDH.ShowTag() ? 0 : DXF_ATTRIB_INVIS);
  DXF_ENTITY a = LDH.m_pBlock->Add_Attrib_Defn(TagAttribStr, "Tag ", "*", basept.p(), Tag_Attr_Set);
  //DXF_ENTITY_THICKNESS(a) = LDH.LineWidth();
  //add arrows...
  LDH.AddArrows(0);
  //add line segments...
  LDH.AddLineData(LDH.LineWidth());

  Pt_3f sc(1.0,1.0,1.0);//LDH.m_TagScale.X, LDH.m_TagScale.Y, LDH.m_TagScale.Z);
  REAL s = 2.5;


  //Pt_3f ptt(LDH.m_InsertPt.X-LDH.m_TagPt.X, LDH.m_InsertPt.Y-LDH.m_TagPt.Y, LDH.m_InsertPt.Z-LDH.m_TagPt.Z);
  //Pt_3f ptt(LDH.m_TagPt.X/*-LDH.m_InsertPt.X*/, LDH.m_TagPt.Y/*-LDH.m_InsertPt.Y*/, LDH.m_TagPt.Z/*-LDH.m_InsertPt.Z*/);
  Pt_3f ptt(LDH.m_TagPt.X-LDH.m_InsertPt.X, LDH.m_TagPt.Y-LDH.m_InsertPt.Y, LDH.m_TagPt.Z-LDH.m_InsertPt.Z);
  Attr_Settings SetMem=Tag_Attr_Set;
  Tag_Attr_Set.Size*=LDH.m_TagScale.X;
  Tag_Attr_Set.Rot=LDH.TagRotation();
  DXF_ENTITY newinsert = pDrw->Create_Insert(nm, LDH.m_InsertPt, GR_WHITE/*GR_LIGHTCYAN*/, sc, LDH.TagRotation(), 
    (LPTSTR)LDH.Guid(), (LPTSTR)LDH.Tag(), (LPTSTR)LDH.AssocTag(), ptt, Tag_Attr_Set);
  DXF_ENTITY_THICKNESS(newinsert) = LDH.LineWidth();
  Tag_Attr_Set=SetMem;
  LDH.SetAssocTag("");
  return newinsert;
  }

//---------------------------------------------------------------------------
//
//flag GrfCmdBlk::ModelsAttached() { return FALSE;};
//
////---------------------------------------------------------------------------
//
//pNodeGrfInfo GrfCmdBlk::GetNodeDrawings()
//  {
//  LogError("GrfCmds", LF_Exclamation, "GetNodeDrawings NOT Implemented");
//  return NULL;
//  }
//
////---------------------------------------------------------------------------

flag GrfCmdBlk::TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs)
  {
  if (pTag==NULL || strlen(pTag)==0)
    return false;
  Entity_List_Info InsInfo;
  Entity_List_Info EntInfo;
  DXF_ENTITY Ins = pDrw->First_Insert(InsInfo);
  while (Ins)
    {
    DXF_ENTITY TAtt = First_Ent(Attr_List(Ins), EntInfo);
    if (TAtt && Attr_Is(TAtt, TagAttribStr))
      {
      pchar pAttr=Attr_Value(TAtt);
      if (_stricmp(pTag, pAttr) == 0)
        return false; //found tag
      }
    Ins = pDrw->Next_Insert(InsInfo);
    }
  return true;
  }

//---------------------------------------------------------------------------

flag GrfCmdBlk::TestSymbolNameUnique(Strng &Sym, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueSym)
  {
  int unum = pDrw->Blocks.FindUnique(TRB.BaseTag(), -1);
  if (unum>=0)
    UniqueSym.Set("%s%c%d",TRB.BaseTag(),TRB.SepChar(),unum);
  else
    UniqueSym.Set("%s%c1",TRB.BaseTag(),TRB.SepChar());
  return False;
  }

//---------------------------------------------------------------------------

BOOL GrfCmdBlk::DoReadDocument(const char * pszPathName, FILE * pFile)
  {
  CWaitCursor Wait;
  GrfComCmdBlk::DoReadDocument(pszPathName, pFile);
  char TmpFn[512];
  strcpy(TmpFn, pszPathName);
  pchar p = strrchr(TmpFn, '.');
  if (p) * p = 0;
  strcat(TmpFn, ".DXF");
  DrawingFile=TmpFn;
  if (gs_pPrj->m_bGrfDeferLoad && gs_pPrj->bDoingLoad)
    bGrfLoadDefered=1;
  else
    {
    Load(TmpFn, False);
    bGrfLoadDefered=0;
    }
  return True;
  }

//---------------------------------------------------------------------------

BOOL GrfCmdBlk::DoWriteDocument(const char * pszPathName, FILE * pFile)
  {
  CWaitCursor Wait;
  GrfComCmdBlk::DoWriteDocument(pszPathName, pFile);
  if (!bGrfLoadDefered)
    {
    char TmpFn[512];
    strcpy(TmpFn, pszPathName);
    pchar p = strrchr(TmpFn, '.');
    if (p) * p = 0;
    strcat(TmpFn, ".DXF");
    Save(TmpFn, True);
    DrawingFile=TmpFn;
    }
  return True;
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::Do3DImport()
  {
  if (When!=EX_EXEC)
    return;


  if (0)
    {
    Strng ClassId;
    CEvalOrderArray Info;
    long iRet=gs_pPrj->m_pFlwLib->FE_GetEvalOrder(0, 1, 1, TV_AbsChg, Info);
    for (int t=0; t<Info.GetSize(); t++)
      {
      CEvalOrderItem &I=Info[t];
      flag b = gs_pPrj->RequestModelClassId(I.m_sTag(), ClassId);
      long NoFIOs=I.m_FIOs.GetSize();
      for (int i=0; i<NoFIOs; i++)
        {
        CEvalOrderIOItem &F=Info[t].m_FIOs[i];
        LogNote("EvalOrd", 0, "%2d:%s : %s  (%s)", i, I.m_sTag(), F.m_sRmtDesc(), ClassId());
        }
      }

    bool PrevHasNOIO=false;
    double Y=0.0;
    double X=0.0;
    //Y+=1;
    for (t=0; t<Info.GetSize(); t++)
      {
      CEvalOrderItem &I=Info[t];
      flag b = gs_pPrj->RequestModelClassId(I.m_sTag(), ClassId);
      if (_stricmp(ClassId(), "pipe-1")!=0)
        {
        long NoFIOs=I.m_FIOs.GetSize();
        long NoCIOs=I.m_CIOs.GetSize();
        long NoXIOs=I.m_XIOs.GetSize();

        int NTFIn=0, NTFOut=0;
        int NTXIn=0, NTXOut=0;
        bool PrevInSeq=false;
        bool NextInSeq=false;
        for (int i=0; i<NoFIOs; i++)
          {
          CEvalOrderIOItem &F=Info[t].m_FIOs[i];
          int iRmtOrd=F.m_lRmtProcOrd;
          if (F.m_bIn && iRmtOrd==I.m_lProcOrd-1)
            PrevInSeq=true;
          else if (F.m_bOut && iRmtOrd==I.m_lProcOrd+1)
            NextInSeq=true;
          }

        for (i=0; i<NoFIOs; i++)
          {
          CEvalOrderIOItem &F=Info[t].m_FIOs[i];
          if (F.m_bIn)
            NTFIn++;
          else if (F.m_bOut)
            NTFOut++;
          }
        for (i=0; i<NoXIOs; i++)
          {
          CEvalOrderIOItem &X=Info[t].m_XIOs[i];
          if (X.m_bIn)
            NTXIn++;
          else //if (X.m_bOut)
            NTXOut++;
          }

        if (1)
          {
          CInsertBlk CB;
          CB.ATag=I.m_sTag();
          CB.AClass=ClassId();
          CB.Pt.World.X=X;
          CB.Pt.World.Y=Y;
          CB.Pt.World.Z=0.0;
          CB.NdScl.X=1.0;
          CB.NdScl.Y=1.0;
          CB.NdScl.Z=1.0;
          CB.Rotate=0.0;
          CB.ATagBase="x";//MInfo.DefTag();
          //if (Symbol && strchr(Symbol, '.'))
          //  CB.ASymbol=Symbol;
          //else
          //  {
          //  CB.ASymbol=MInfo.DefTag();
          //  CB.ASymbol+='.';
          //  CB.ASymbol+=Symbol;
          //  }
          CB.ASymbol="Default";
          DoInsertNodeGrf(&CB);
          }

        LogNote("Insert", 0, "%4g %4g %s", Y, X, I.m_sTag());
        bool ThisHasNOIO=(NTFIn==0 && NTFOut==0);
        if (PrevInSeq || PrevHasNOIO && ThisHasNOIO)
          {
          //X+=100.0;
          Y+=40.0;
          X=0.0;
          }
        else
          {
          X+=80.0;
          //Y+=100.0;
          //X=0.0;
          }

        PrevHasNOIO=ThisHasNOIO;

        //int NTot;
        //int NTot=Max(1, Max((PrevInSeq?NTFIn-1:NTFIn)+NTCIn+NTXIn, (NextInSeq?NTFOut-1:NTFOut)+NTCOut+NTXOut));
        //Y+=NTot;
        }

      }

    for (t=0; t<Info.GetSize(); t++)
      {
      CEvalOrderItem &I=Info[t];
      flag b = gs_pPrj->RequestModelClassId(I.m_sTag(), ClassId);
      if (_stricmp(ClassId(), "pipe-1")==0)
        {
        if (1)
          {
          //b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
          RequestConnModelInfoRec MCInfo;
          b = gs_pPrj->RequestModelConnInfo(I.m_sTag(), MCInfo);
          CConnectBlk CB;
          CB.ATag=I.m_sTag();
          CB.AClass=ClassId();
          CB.SrcTag=MCInfo.SrcTag();
          //CB.SrcIO=SrcIO;
          CB.DstTag=MCInfo.DstTag();
          //CB.DstIO=DstIO;
          CB.iLineMethod=0;
          CB.NVerts=-1;

          //CB.ATagBase=MInfo.DefTag();
          //CB.ASymbol=MInfo.DefTag();
          CB.ASymbol+="Default";

          DoInsertLinkGrf(&CB);
          }

        }

      }

    return;
    }

  char Fn1[_MAX_PATH], FnD[_MAX_PATH];
  Fn1[0] = 0;
  sprintf(FnD, "%s*.txt", sPrevDxfDir.Len()==0 ? PrjFiles() : sPrevDxfDir());
  if (GetAFileName("*", FnD, Fn1, sizeof(Fn1), NULL, NULL, 0, False, True))
    {
    CWaitCursor Wait;
    CString ATag,SrcTag,DstTag;
    CString AClass;
    PT3 Pos;
    DXF_ENTITY pEnt;
    double x0,y0,z0;

    Pt_3f Scl(1.0, 1.0, 1.0);
    float Rotate = (float)0.0;
    Tag_Attr_Set.Flags = DXF_ATTRIB_INVIS; // Force It Off

    sPrevDxfDir.FnDrivePath(Fn1);
    Strng InFileName, InFileName2;
    Strng Folder;
    Folder = sPrevDxfDir;
    //Folder = "d:\\elmse\\mine01\\";
    InFileName = Folder;
    InFileName += "repnodes.txt";
    InFileName2 = Folder;
    InFileName2 += "repsects.txt";

    CDlgBusy::Open("");
    CDlgBusy::SetLine(1, "Import : %s", InFileName());
    int NodeCnt = 0;
    //NodeName,x,y,z,Area,ClassGrp,ServiceLvl,RunNo,GridRef,JoinType,ConnectedNodes,CableEnds,Quantity,MatID,Existing,User1,User2,User3";
    CTokenParser* f = new CTokenParser(0, InFileName(), TRUE, TRUE, FALSE);
    if (f->Start())
      {
      CString s;
      int LineCount = 0;
      s = f->GetNextLine();//get header line
      while (!f->AtEnd())
        {
        //load fields...
        f->NextCSVToken(ATag);
        if (f->AtEnd())
          break;
        f->NextCSVToken(s);//x
        x0 = atof((const char*)s);
        f->NextCSVToken(s);//y
        y0 = atof((const char*)s);
        f->NextCSVToken(s);//z
        z0 = atof((const char*)s);
        f->NextCSVToken(s);//Area
        f->NextCSVToken(s);//ClassGrp
        f->NextCSVToken(s);//ServiceLvl
        f->NextCSVToken(s);//RunNo
        f->NextCSVToken(s);//GridRef
        f->NextCSVToken(s);//JoinType
        f->NextCSVToken(s);//ConnectedNodes
        f->NextCSVToken(s);//CableEnds
        f->NextCSVToken(s);//Quantity
        f->NextCSVToken(AClass);//MatID
        //f->NextCSVToken(s);//Existing
        //f->NextCSVToken(s);//User1
        if (AClass=="")
          AClass = "Tie-1"; //assume tie

        Pos[0]=x0;
        Pos[1]=y0;
        Pos[2]=z0;
        pEnt = AddUnitDrawing("", (char*)(const char*)AClass, (char*)(const char*)AClass, (LPSTR)(LPCSTR)TaggedObject::CreateGuidStr(), (char*)(const char*)ATag, NULL, Pos, Scl, Rotate, True, Tag_Attr_Set);
        if (pEnt)
          {
          pDsp->Draw(pEnt, -1);
          AddUnitModel((char*)(const char*)AClass, (char*)(const char*)ATag);
          NodeCnt++;
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s Not constructed", (const char*)ATag);
        LineCount++;
        if (!f->AtLastToken())
          s = f->GetNextLine();
        if (LineCount % 10 == 0)
          CDlgBusy::SetLine(3, "Node : %d  (Line:%d)", NodeCnt, LineCount);
        }
      f->End();
      }
    delete f;

    /*if (gs_pTheSFELib)
      gs_pTheSFELib->SetLoadAllModelStatus();

    pDsp->Open();
    pDsp->SetZoom(Zoom_All);
    pDrw->EntityInvalidate();
    pWnd->Invalidate(True);
    pDsp->Close();*/

    CDlgBusy::SetLine(1, "Import : %s", InFileName2());
    int SectCnt = 0;
    //SectionName,Width,Height,TtlRacks,SrcNode,DstNode,SectType,Mounting,Bearing,Area,ClassGrp,RunNo,GridRef,Length,PhysicalLength,Cables,CableMass,TTlSpaceTake,MatID,Existing,User1,User2,User3,P_racks,C_racks,I_racks,P_space,C_space,I_space,MassPerMeter,Mid_x,Mid_y,Mid_z
    //SectionName,Width,Height,TtlRacks,SrcNode,DstNode,SectType,Mounting,Bearing,Area,ClassGrp,ServiceLvl,RunNo,GridRef,Length,PhysicalLength,Cables,CableMass,TTlSpaceTake,MatID,Existing,User1,User2,User3,P_racks,C_racks,I_racks,P_space,C_space,I_space,MassPerMeter,Mid_x,Mid_y,Mid_z
    f = new CTokenParser(0, InFileName2(), TRUE, TRUE, FALSE);
    if (f->Start())
      {
      char *a[] = { TagAttribStr,    (pchar)NULL };
      CEntInView* pEn0;
      CEntInView* pEn1;
      DXF_ENTITY en;
      ArrowScale = 2.5;
      CString ASrcIO, ADstIO;
      ASrcIO = "Out";
      ADstIO = "In";
      CConnectBlk* CB = new CConnectBlk;
      CB->NVerts = 2;
      Pt_SLW& p0 = CB->Verts[0];
      Pt_SLW& p1 = CB->Verts[1];
      CString s;
      int LineCount = 0;
      s = f->GetNextLine();//get header line
      while (!f->AtEnd())
        {
        //load fields...
        f->NextCSVToken(ATag);
        if (f->AtEnd())
          break;
        f->NextCSVToken(s);//width
        f->NextCSVToken(s);//height
        f->NextCSVToken(s);//TtlRacks
        f->NextCSVToken(SrcTag);
        f->NextCSVToken(DstTag);
        f->NextCSVToken(s);//SectType
        f->NextCSVToken(s);//Mounting
        f->NextCSVToken(s);//Bearing
        f->NextCSVToken(s);//Area
        f->NextCSVToken(s);//ClassGrp
        f->NextCSVToken(s);//ServiceLvl
        f->NextCSVToken(s);//RunNo
        f->NextCSVToken(s);//GridRef
        f->NextCSVToken(s);//Length
        f->NextCSVToken(s);//PhysicalLength
        f->NextCSVToken(s);//Cables
        f->NextCSVToken(s);//CableMass
        f->NextCSVToken(s);//TTlSpaceTake
        f->NextCSVToken(AClass);//MatID
        //f->NextCSVToken(s);//Existing
        //f->NextCSVToken(s);//User1
        if (AClass=="")
          AClass = "Pipe-1"; //assume pipe

        pDsp->Vp1->ClearAllEntity();
        char *b0[] = { (char*)(const char*)SrcTag, (char*)NULL };
        pDrw->SelectInsertsOnAttrCombo(NULL, a, b0, NULL);
        pEn0 = pDsp->Vp1->FirstSelectedEntity();
        if (pEn0 && pEn0->EntityPtr() && DXF_ENTITY_IS_INSERT(pEn0->EntityPtr()))
          {
          en = pEn0->EntityPtr();
          p0.World.X = DXF_INSERT_PT(en)[0];
          p0.World.Y = DXF_INSERT_PT(en)[1];
          p0.World.Z = DXF_INSERT_PT(en)[2];
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s not found", (const char*)SrcTag);

        pDsp->Vp1->ClearAllEntity();
        char *b1[] = { (char*)(const char*)DstTag, (char*)NULL };
        pDrw->SelectInsertsOnAttrCombo(NULL, a, b1, NULL);
        pEn1 = pDsp->Vp1->FirstSelectedEntity();
        if (pEn1 && pEn1->EntityPtr() && DXF_ENTITY_IS_INSERT(pEn1->EntityPtr()))
          {
          en = pEn1->EntityPtr();
          p1.World.X = DXF_INSERT_PT(en)[0];
          p1.World.Y = DXF_INSERT_PT(en)[1];
          p1.World.Z = DXF_INSERT_PT(en)[2];
          }
        else
          LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s not found", (const char*)DstTag);

        if (pEn0)
          {
          Strng XASrcIO;
          GetConnsForUnit((char*)(const char*)SrcTag, False, nc_MLnk|nc_CLnk|nc_ELnk|nc_ALnk, XASrcIO, SrcIONames, SrcIODescs, SrcIOClass, SrcTermStrips);
          if (SrcIONames.GetSize()==0)
            {
            pEn0 = NULL;
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s does not have a valid output connection IO for %s", (const char*)SrcTag, (const char*)ATag);
            }
          else
            ASrcIO = SrcIONames[0]();
          }

        if (pEn1)
          {
          Strng XADstIO;
          GetConnsForUnit((char*)(const char*)DstTag, True, SrcIOClass[iSrcIOIndex], XADstIO, DstIONames, DstIODescs, DstIOClass, DstTermStrips);
          if (DstIONames.GetSize()==0)
            {
            pEn1 = NULL;
            LogError("GrfCmds", LF_DoAfxMsgBox|LF_Exclamation, "Unit %s does not have a valid input connection IO for %s", (const char*)DstTag, (const char*)ATag);
            }
          else
            ADstIO = DstIONames[0]();
          }

        if (pEn0 && pEn1)
          {
          pEnt = AddLink((char*)(const char*)AClass, (LPSTR)(LPCSTR)TaggedObject::CreateGuidStr(), (char*)(const char*)ATag, (char*)(const char*)SrcTag,
                         (char*)(const char*)ASrcIO, (char*)(const char*)DstTag, (char*)(const char*)ADstIO,
                         ArrowScale, CB->Verts, CB->NVerts, Tag_Attr_Set);
          if (pEnt)
            {
            pDsp->Draw(pEnt, -1);
            pWnd->Invalidate();
            SectCnt++;
            }
          }

        LineCount++;
        if (!f->AtLastToken())
          s = f->GetNextLine();
        if (LineCount % 10 == 0)
          CDlgBusy::SetLine(3, "Section : %d  (Line:%d)", SectCnt, LineCount);
        }
      f->End();
      delete CB;
      CB = NULL;
      }

    delete f;
    CDlgBusy::Close();

    if (gs_pTheSFELib)
      {
      gs_pTheSFELib->SetLoadAllModelStatus();
      gs_pTheSFELib->SetLoadAllModelAssocGrfs();
      }

    pDsp->Open();
    pDsp->SetZoom(Zoom_All);
    pDrw->EntityInvalidate();
    pWnd->Invalidate(True);
    pDsp->Close();
    }
  }

//---------------------------------------------------------------------------

/*void GrfCmdBlk::DoShowHideAttrs(flag Show, flag Tags)
  {
  //SETVNT("SHow", "Annotation", "@ IWin CWin All Like NOLike Size NOSize Rotation NORotation Display NODisplay NAme NOName Position", 1, (CmdFn)&GrfCmdBlk::DoShowAttrs, EF_ERS);
  pDsp->Open();
  switch (When)
    {
    DXF_ENTITY Ins;
    DXF_ENTITY TAtt;
    Entity_List_Info InsInfo;
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          Invalidate(Size);
          Invalidate(Rotation);
          Invalidate(Delta.X);
          Like = NULL;
          AttrName = 0;
          Display = 1;
          break;
        case 1 : //Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : //IWin
        case 3 : //CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 4 : //All
          Ins = pDrw->First_Insert(InsInfo);
          while (Ins)
            {
            if (IsLike(Ins))
              {
              pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
              if (Show)
                ShowAttrs(Ins, Tags);
              else
                HideAttrs(Ins, Tags);
              pDsp->Draw(Ins, -1);
              }
            Ins = pDrw->Next_Insert(InsInfo);
            }
          Invalidate(Delta.X);
          break;
        case 5 : //Like
          gs_pCmd->SetDigInfo(GC_NoCurs, 1);
          break;
        case 6 : //NOLike
          Like = NULL;
          break;
        case 7 : //Size
          gs_pCmd->SetParmInfo(AFloat, 1);
          break;
        case 8 : //NOSize
          Invalidate(Size);
          break;
        case 9 : //Rotation
          gs_pCmd->SetParmInfo(AFloat, 1);
          break;
        case 10 : //NORotation
          Invalidate(Rotation);
          break;
        case 11 : //Display
          Display = 1;
          break;
        case 12 : //NODisplay
          Display = 0;
          break;
        case 13 :
          AttrName = 1; //Name
          break;
        case 14 : //NOName
          AttrName = -1;
          break;
        case 15 : //Position
          gs_pCmd->SetDigInfo(GC_RubberCurs, 1);
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 7 : gs_pCmd->dParm(Size); break;
        case 9 : gs_pCmd->dParm(Rotation); break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 :
        case 2 :
        case 3 :
          if (Modifier == 1)
            Ins = pDrw->ClosestInsert(pDsp->CurrentPt);
          else
            {
            pGWnd->SetCursor();
            Ins = pDrw->First_Insert_In_Win(pDsp->StartPt, pDsp->EndPt, InsInfo);
            }
          while (Ins)
            {
            if (IsLike(Ins))
              {
              pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
              if (Show)
                ShowAttrs(Ins, Tags);
              else
                HideAttrs(Ins, Tags);
              pDsp->Draw(Ins, -1);
              }
            if (Modifier == 1)
              Ins = NULL;
            else
              Ins = pDrw->Next_Insert_In_Win(InsInfo);
            }
          Invalidate(Delta.X);
          break;
        case 5 :
          Like = pDrw->ClosestInsert(pDsp->CurrentPt);
          break;
        case 15 :
          Ins = pDrw->ClosestInsert(pDsp->StartPt);
          TAtt = FirstAttr(Ins, Tags);
          if (TAtt)
            {
            Delta.X = pDsp->CurrentPt.World.X - Insert_BasePt(Ins)[0];
            Delta.Y = pDsp->CurrentPt.World.Y - Insert_BasePt(Ins)[1];
            pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
            MoveAttrs(Ins, Tags, Delta);
            pDsp->Draw(Ins, -1);
            }
          break;
        default :;
        }
      break;
    case EX_DOIT :
    case EX_UNDO :
      break;
    case EX_RESULTS :
pWnd->Invalidate(True);
      DumpErrors();
      break;
    }
  pDsp->Close();
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoShowAttrs()
  {
  DoShowHideAttrs(1, Noun == 1);
  }

//---------------------------------------------------------------------------

void GrfCmdBlk::DoHideAttrs()
  {
  DoShowHideAttrs(0, Noun == 1);
  }*/

//---------------------------------------------------------------------------

/*void GrfCmdBlk::DoMoveAttrs()
  {
  pDsp->Open();
  flag Tags = (Noun == 1);
  switch (When)
    {
      DXF_ENTITY Ins;
      Entity_List_Info InsInfo;
    case EX_MODIFIER_RDY :
      Size = 0.0;
      switch (Modifier)
        {
        case ModNoun : Delta.X = 0.; Delta.Y = 0.; Invalidate(Rotation); Like = NULL; break;
        case 1 : gs_pCmd->SetDigInfo(GC_RubberCurs, 1); break;
        case 2 : gs_pCmd->SetDigInfo(GC_RectCurs, 1); break;
        case 4 : gs_pCmd->SetDigInfo(GC_NoCurs, 1); break; // Like
        case 5 : Like = NULL;
        }
      break;
    case EX_MOUSE_LUP :
        {
        switch (Modifier)
          {
          case 1 :
              {
              Ins = pDrw->ClosestInsert(pDsp->StartPt);
              DXF_ENTITY TAtt = FirstAttr(Ins, Tags);
              if (TAtt)
                {
                Delta.X = pDsp->CurrentPt.World.X - Insert_BasePt(Ins)[0];
                Delta.Y = pDsp->CurrentPt.World.Y - Insert_BasePt(Ins)[1];
                pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
                MoveAttrs(Ins, Tags, Delta);
                pDsp->Draw(Ins, -1);
                }
              }
            break;
          case 2 :
          case 3 :
              {
              if (Modifier == 2)
                Ins = pDrw->First_Insert_In_Win(pDsp->StartPt, pDsp->EndPt, InsInfo);
              else
                Ins = pDrw->First_Insert(InsInfo);
              while (Ins)
                {
                if (IsLike(Ins))
                  {
                  pDsp->Draw(Ins, GrfHelper.GR_BACKGROUND);
                  MoveAttrs(Ins, Tags, Delta);
                  pDsp->Draw(Ins, -1);
                  }
                if (Modifier == 2)
                  Ins = pDrw->Next_Insert_In_Win(InsInfo);
                else
                  Ins = pDrw->Next_Insert(InsInfo);
                }
              }
            break;
          case 4 :
            Like = pDrw->First_Insert(InsInfo);
            break;
          default :;
          }
        }
      break;
    case EX_RESULTS :
      DumpErrors();
      break;
    }
  pDsp->Close();
  }*/

//---------------------------------------------------------------------------
/*
void GrfCmdBlk::DoColor()
  {
  pDsp->Open();
  switch (When)
    {
    case EX_MODIFIER_RDY :
      switch (Modifier)
        {
        case ModNoun :
          pDsp->Vp1->ClearAllEntity();
          AColor = -1;
          break;
        case 1 : //Dig
          gs_pCmd->SetDigInfo(GC_BigCurs, 1);
          break;
        case 2 : //IWin
        case 3 : //CWin
          gs_pCmd->SetDigInfo(GC_RectCurs, 1);
          pGWnd->SetCursor(IDC_SELECTBOX);
          break;
        case 4 : //Color
          gs_pCmd->SetParmInfo(AnInt, 1);
          break;
        }
      break;
    case EX_PARAMETER_RDY :
      switch (Modifier)
        {
        case 4 :
          gs_pCmd->iParm(AColor);
          //pDrw->DumpSelectedEntities(pDsp->Vp1);
          CEntInView* p = pDsp->Vp1->FirstSelectedEntity();
          if (p)
            {
            do
              {
              if (p->EntityPtr())
                {
                dxf_set_entity_color(p->EntityPtr(), AColor);
                }
              if (p->CurvePtr())
                {
                pDrw->SetCurveColor(p->CurvePtr(), AColor);
                //if (DXF_ENTITY_IS_CURVE(e))
                //  {
                //  C3_CURVE c = dx3_entity_to_curve(e);
                //  c->color = DXF_ENTITY_COLOR(e);
                //  }
                //SetCurveColor(crv,-1);
                //if (DXF_ENTITY_COLOR_STATUS(e))
                //  dxf_set_entity_color(e,-1);
                //dump_curve(p->CurvePtr());
                }
              }
            while (p = pDsp->Vp1->NextSelectedEntity());
            }
          pWnd->Invalidate(True);
          break;
        }
      break;
    case EX_MOUSE_LUP :
      switch (Modifier)
        {
        case 1 :
          pDsp->Vp1->ClrSelectionAllList();
          pDsp->Vp1->SelectClosestEntity(pDsp->CurrentPt);
          break;
        case 2 :
        case 3 :
          {
          pGWnd->SetCursor();
          CWaitCursor Wait;
          pDsp->Vp1->ClrSelectionAllList();
          if (Modifier==2)
            pDsp->Vp1->SelectEntitiesInBox(pDsp->StartPt, pDsp->EndPt);
          else
            pDsp->Vp1->SelectEntitiesInCrossBox(pDsp->StartPt, pDsp->EndPt);
          break;
          }
        default :;
        }
      break;
    case EX_DOIT :
    case EX_UNDO :
      break;
    case EX_RESULTS :
      pWnd->Invalidate(True);
      DumpErrors();
      break;
    default :;
    }
  pDsp->Close();
  }
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
