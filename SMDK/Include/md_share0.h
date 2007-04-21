//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_SHARE0_H)
#define MD_SHARE0_H

// ----------------------------------------------------------------------

typedef ULONG64 DDEF_Flags;

const DDEF_Flags DDEF_NOCOMPARE           = 0x0000000000020000; // Maybe masked out ??
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
//const DDEF_Flags DDEF_STARTMATRIX         = 0x0020000000000000;
//const DDEF_Flags DDEF_STARTROW            = 0x0040000000000000;
//const DDEF_Flags DDEF_ENDMATRIX           = 0x0080000000000000;
const DDEF_Flags DDEF_TAGNOGRF            = 0x0100000000000000;
const DDEF_Flags DDEF_TAGCASESENS         = 0x0200000000000000;

//                                            6 5  4 3  2 1
//                                            0628406284062840
const int     DDEF_CHILDSHIFT             = 40;

inline DDEF_Flags ChildLevel(int i) { return (DDEF_CHILDMASK) & (((ULONG64)i)<<DDEF_CHILDSHIFT); }

typedef DDEF_Flags MD_Flags;

// ----------------------------------------------------------------------

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

// ----------------------------------------------------------------------
// DataTypes
const unsigned char tt_NULL        =  0;
const unsigned char tt_Bool        =  1;
const unsigned char tt_Bit         =  2;
const unsigned char tt_Byte        =  3;
const unsigned char tt_Word        =  4;
const unsigned char tt_DWord       =  5;
const unsigned char tt_Char        =  6;
const unsigned char tt_Int         =  7;
const unsigned char tt_Short       =  8;
const unsigned char tt_Long        =  9;
const unsigned char tt_Flt16       = 10;
const unsigned char tt_Float       = 11;
const unsigned char tt_Double      = 12;
const unsigned char tt_Generic     = 13;
const unsigned char tt_pChar       = 14;
const unsigned char tt_ppChar      = 15;
const unsigned char tt_Strng       = 16;
const unsigned char tt_RqdPage     = 17;
const unsigned char tt_OptPage     = 18;
const unsigned char tt_Column      = 19;
const unsigned char tt_Grid        = 20;
const unsigned char tt_Grid_E      = 21;
const unsigned char tt_ColHdr      = 22;
const unsigned char tt_RowHdr      = 23;
const unsigned char tt_RowStart    = 24;
const unsigned char tt_Text        = 25;
const unsigned char tt_Struct      = 26;
const unsigned char tt_Element     = 27;
const unsigned char tt_Object      = 28;
const unsigned char tt_Array       = 29;
const unsigned char tt_Struct_E    = 30;
const unsigned char tt_Element_E   = 31;
const unsigned char tt_Object_E    = 32;
const unsigned char tt_Array_E     = 33;
const unsigned char tt_Blob        = 34;
const unsigned char tt_Table       = 35;

typedef struct {unsigned char t; LPCTSTR Str;} TagObjTypeStruct;
const TagObjTypeStruct TagObjTypeNames[] =
  {
    { tt_NULL,      "NULL" },
    { tt_Bool,      "Bool" },
    { tt_Bit,       "Bit" },
    { tt_Byte,      "Byte" },
    { tt_Word,      "Word" },
    { tt_DWord,     "DWord" },
    { tt_Char,      "Char" },
    { tt_Int,       "Int" },
    { tt_Short,     "Short" },
    { tt_Long,      "Long" },
    { tt_Flt16,     "Flt16" },
    { tt_Float,     "Float" },
    { tt_Double,    "Double" },
    { tt_Generic,   "Generic" },
    { tt_pChar,     "pChar" },
    { tt_ppChar,    "ppChar" },
    { tt_Strng,     "Strng" },
    { tt_RqdPage,   "RqdPage" },
    { tt_OptPage,   "OptPage" },
    { tt_Column,    "Column" },
    { tt_Grid,      "Grid<" },
    { tt_Grid_E,    "Grid>" },
    { tt_ColHdr,    "ColHdr" },
    { tt_RowHdr,    "RowHdr" },
    { tt_RowStart,  "Row<" },
    { tt_Text,      "Text" },
    { tt_Struct,    "{" },
    { tt_Element,   "(" },
    { tt_Object,    "<" },
    { tt_Array,     "[" },
    { tt_Struct_E,  "}" },
    { tt_Element_E, ")" },
    { tt_Object_E,  ">" },
    { tt_Array_E,   "]" },
    { tt_Blob,      "BLOB" },
    { tt_Table,     "Table" },
    { tt_NULL,      NULL}
  };

// ----------------------------------------------------------------------
/*#undef DllImportExport
#if defined(__MATHLIB_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ========================================================================

#undef DllImportExport*/

#endif // !defined(MD_SHARE0_H
