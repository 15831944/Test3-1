//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TAGOBJ_H
#define  __TAGOBJ_H

//---------------------------------------------------------------------------

#include "scdver.h"
#include "datacnvs.h"
#include "fixedit.h"
#include "errorlog.h"
#include "datatype.h"
#include "scdarray.h"
#include "propertybase.h"
#include "scdver.h"
#include "..\..\..\smdk\include\md_share3.h"

#ifdef WITHSCDINTERFACES
#pragma message ("----- COM Interfaces included -----")
#include "..\..\Com\ScdIF\ScdIF.h"
#include <atlbase.h>
#include "ScdCOMTmpl.h"

#endif


#undef DllImportExport

#if defined(__TAGOBJ_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

_FWDDEF(DDBShortcut);
_FWDDEF(DDBTagMap);
_FWDDEF(DDBPkListItem);
_FWDDEF(DDBPkListMap);
_FWDDEF(DDBPkListArray);
class CPkDataItem;
class CPkDataList;
//_FWDDEF(CPkDataItem)
//_FWDDEF(CPkDataList)
_FWDDEF(DataDefnBlk)
_FWDDEF(TagObjClass)
_FWDDEF(TaggedObject)
_FWDDEF(TagAccessBlk)
class CFieldMonitor;
class CFieldMonitorTO;
class CFieldMonitorTOCls;

//===========================================================================
//
//
//
//===========================================================================

#ifndef _DEBUG
#define DOINLINE 1
#else
#define DOINLINE 0
#endif

#if (DOINLINE)
#define INLINE inline
#else
#define INLINE
#endif

#ifdef _RELEASE
//ensure no overhead checking tags,cnvs,etc when in full release mode!
#define DoDDBChecks 0
#else
#define DoDDBChecks 1/*(!defined(_MANAGED))// && !(0 && SYSCAD10))*/
#endif

#if DoDDBChecks
const int DDBCheckListSize=128;
extern DllImportExport int bDoneCnvWarningOnce;
extern DllImportExport int bContinueChecking;
extern DllImportExport int gs_DevelopementChecks;
DllImportExport void DevelopementChecksOn();
DllImportExport void DevelopementChecksOff();
//#endif
//DllImportExport void CheckTagStuff(DataDefnBlk* pDDB);
#endif

//===========================================================================
//
//
//
//===========================================================================

#define PlantModelTag   "PlantModel"
#define PlantAreaTag    "PlantArea"

//===========================================================================
//
//
//
//===========================================================================
// From xTaggedData

const LPSTR TagSeparator=".";
const  int   SCDPartTagLen    = 64;
const  int   SCDFullTagLen    = 255;

typedef char SCDPartTag[SCDPartTagLen+1];
typedef char SCDFullTag[SCDFullTagLen+1];

inline int TOElementTagDigitCnt(bool Wide=true) { return Wide ? (PrjFileVerNo()>=76 ? 6:3):1; } // allow for more than 1000 elements

//===========================================================================
//
//
//
//===========================================================================

typedef word   TOItemNoType;

typedef byte TagUniqueness;
const byte TU_IndividuallyUnique=0;
const byte TU_UniqueInFamily=1;

typedef byte TagObjAttachment;
const byte TOA_Unknown   = 0x00;
const byte TOA_Embedded  = 0x01;
const byte TOA_Free      = 0x02;
const byte TOA_Temporary = 0x04;
const byte TOA_Hidden    = 0x08;
const byte TOA_All       = TOA_Embedded|TOA_Free|TOA_Temporary;

const long MaxTABTagLen      = 128;

//===========================================================================
//
//
//
//===========================================================================

enum TOFilingTypes {TOF_DBase, TOF_SnapShot, TOF_Scenario};


typedef char TABTag[MaxTABTagLen];

class DllImportExport CObjectCountBlk 
  {
  public:
    CObjectCountBlk()
      {
      Clear();
      }
    
    void Clear()
      {
      m_nNodeCount    = 0;
      m_nLinkCount    = 0;
      m_nConnCount    = 0;
      m_nFlangeCount  = 0;
      m_nSystemCount  = 0;
      };

    CString ToString();
    CString ToStringFull();
    CString ToStringOf(CObjectCountBlk & Of);
    CString ToStringSummaryOf(CObjectCountBlk & Of);
    long TotalCount() { return m_nNodeCount+m_nLinkCount+m_nConnCount+m_nFlangeCount+m_nSystemCount; };

  public:
    long           m_nNodeCount;
    long           m_nLinkCount;
    long           m_nConnCount;
    long           m_nFlangeCount;
    long           m_nSystemCount;
  };

class DllImportExport FilingControlBlock
  {
  protected:
    Strng          sRoot;
    Strng          sLabel;
    Strng          sXName;
    Strng          sObjTag;
    TOFilingTypes  eType;
    HANDLE         hFile;
    DWORD          dwStartPos;
    DWORD          dwCurrentPos;
    DWORD          dwEndPos;
    Strng          xm_sOldPrjFiles;
    long           m_PrjFileVerNo;
    byte           bSaveAs:1,
                   bFarCopy:1,
                   bCreateObj:1;

  public:
    CObjectCountBlk m_Counts;

  public:
    FilingControlBlock(char * Root, char * Label, TOFilingTypes Type);

    char* Root() { return sRoot(); };
    char* Label() { return sLabel(); };
    TOFilingTypes Type() { return eType; };
    void SetXName(char *Name) { sXName=Name; };
    void SetObjTag(char *Tag) { sObjTag=Tag; };
    long PrjFileVerNo() { return m_PrjFileVerNo; };
    void SetPrjFileVerNo(long VerNo) { m_PrjFileVerNo=VerNo; };
    char* XName() { return sXName(); };
    char* ObjTag() { return sObjTag(); };
    flag SaveAs() { return bSaveAs; };
    char* OldPrjFiles() { return xm_sOldPrjFiles(); };
    flag CreateObj() { return bCreateObj; };
    void InitialiseForRead(HANDLE File, DWORD StartPos, DWORD EndPos);
    void InitialiseForWrite(HANDLE File, DWORD StartPos);
    void SetSaveAs(char* pPrevPrj);
    void SetCreateObj(int On) { bCreateObj=On; };
    //flag CopyFile(char* pFN);
    BOOL ReadFile(LPVOID lpBuffer,// address of buffer that receives data
                  DWORD nNumberOfBytesToRead,// number of bytes to read
                  LPDWORD lpNumberOfBytesRead);// address of number of bytes read
    BOOL WriteFile(LPCVOID lpBuffer,// address of data to write to file
                   DWORD nNumberOfBytesToWrite,// number of bytes to write
                   LPDWORD lpNumberOfBytesWritten);// address of number of bytes written
    DWORD SetFilePointer(LONG lDistanceToMove,// number of bytes to move file pointer
                         DWORD dwMoveMethod);// how to move
    BOOL Eof();
  };

//===========================================================================
//
//
//
//===========================================================================

enum DDBPages {DDB_NoPage, DDB_RqdPage, DDB_OptPage};
const byte MAXSTRUCTNESTING  = 32;

enum DDBTasks {DDBTask_FndData=1, DDBTask_GetData=2, DDBTask_PutData=4};
const UINT DDBHashMax=31;  // Should be Prime

//---------------------------------------------------------------------------

typedef void GlblTopologyChgFun(DWORD pData);
struct GlblTopologyChg
  {
  GlblTopologyChgFun  * Fun;
  dword                 Data;
  };

//---------------------------------------------------------------------------

class CTagRefStatusFnHook
  {
  public:
    virtual CTagRefStatus GetTagRefStatus(LPCTSTR Tag)=0;
  };

//---------------------------------------------------------------------------

class TaggedObject;
class DllImportExport DDBDataInfo
  {
  public:
    DDBDataInfo()
      {
      m_psFullItemTag     = NULL;
      m_bCurrentComplete  = false;
      }
    DDBDataInfo & operator=(const DDBDataInfo & X)
      {
      pTag                = X.pTag;
      pSym                = X.pSym;
      iType               = X.iType;
      iFlags              = X.iFlags;
      iCnv                = X.iCnv;
      pCnvTxt             = X.pCnvTxt;
      pCnvFam             = X.pCnvFam;
      pThis               = X.pThis;
      m_pRootObj          = X.m_pRootObj;
      pData               = X.pData;
      lHandle             = X.lHandle;
      lHandle2            = X.lHandle2;
      dwUserInfo          = X.dwUserInfo;
      wBlobSize           = X.wBlobSize;
      pValueLst           = X.pValueLst;
      pValueLstMem        = X.pValueLstMem;
      m_bCurrentComplete  = X.m_bCurrentComplete;
      m_sCurrentTag       = X.m_sCurrentTag;
      m_nParms            = X.m_nParms;
      for (int i = 0; i<X.m_nParms; i++)
        m_dParms[i]         = X.m_dParms[i];
      return *this;
      }

    DDEF_Flags      GetValue(TABOptions Opts, PkDataUnion &DataValue);
    void            PutValue(TABOptions Opts, PkDataUnion &Data);

    void            GetDefinedData(TABOptions Opts, CPkDataList & List /*PkDataListInfo * pInfo*/);
    void            PutDefinedData(TABOptions Opts, CPkDataList & List /*PkDataListInfo * pInfo*/, CPkDataIter &Item);

    static PkDataUnion * FixGenericData(PkDataUnion &LclDataValue, PkDataUnion &DataValue, byte iType, CCnvIndex iCnv, DDBValueLst * pValueList);
    flag            PutDataBlocked(TABOptions Opts, DDEF_Flags Flags);
    void            MonitorParameter(TABOptions Opts, PkDataUnion *pNewValue);

    // Type Specific Gets and Puts
    DDEF_Flags      GetBool  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutBool  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetChar  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutChar  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetByte  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutByte  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetWord  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutWord  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetDWord  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutDWord  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetInt  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutInt  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetShort  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutShort  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetLong  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutLong  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetFloat  (TABOptions Opts, PkDataUnion &DataValue);
    void            PutFloat  (TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetDouble(TABOptions Opts, PkDataUnion &DataValue);
    void            PutDouble(TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetString(TABOptions Opts, PkDataUnion &DataValue);
    void            PutString(TABOptions Opts, PkDataUnion &DataValue);

    DDEF_Flags      GetBlob(TABOptions Opts, PkDataUnion &DataValue);
    void            PutBlob(TABOptions Opts, PkDataUnion &DataValue);

    static CTagRefStatusFnHook *gs_TagRefGlblFn;

  public:
    char            *pTag;
    char            *pSym;
    Strng           *m_psFullItemTag;
    byte            iType;
    DDEF_Flags      iFlags;
    CCnvIndex       iCnv;
    LPSTR           pCnvTxt;
    LPSTR           pCnvFam;
    TaggedObject    *pThis;
    TaggedObject    *m_pRootObj;

    void *          pData;
    long            lHandle;
    long            lHandle2;
    dword           dwUserInfo;
    word            wBlobSize;

    DDBValueLst     *pValueLst;
    DDBValueLstMem  *pValueLstMem;

    int             m_nParms;
    double          m_dParms[3];

    bool            m_bCurrentComplete;
    Strng           m_sCurrentTag;

  };

class DDBShortcutVersion
  {
  public:
    dword           dwDataVerObj;
    dword           dwDataVerGlbl;
    DDBShortcutVersion()
      {
      dwDataVerObj=0xffffffff;
      dwDataVerGlbl=0xffffffff;
      }
  };

class DllImportExport DDBShortcut : public DDBDataInfo
  {
  //DEFINE_SPARES(DDBShortcut)
  public:
    flag            fOK;
    DDBShortcutVersion DataVer;
    Strng           sFullItemTag;
    Strng           sTag;
    Strng           sSym;
    Strng           sCnvTxt;
    Strng           sCnvFam;
    DDBValueLstMem  ValueLstMem;

    DDBShortcut() { m_psFullItemTag=&sFullItemTag; fOK=False; };
    //DDBShortcut() { m_bCurrentComplete=true; pFullItemTag=&m_sCurrentTag; fOK=False; };
    DDBShortcut & operator=(const DDBShortcut & X);
    void          Clear() { fOK=False; };
    void          Set(DDBDataInfo * pDI,  LPSTR FullItemTag, dword CurDataVer);
    flag          TestShortcut(char *FullItemTag, dword ObjectVersion);
  };

//===========================================================================
//NBNB: Any changes to TOC flags need to be reflected in the SMDK M??? equivilant options!

//solve mode options...
const DWORD TOC_PROBAL       = 0x00000001; //is probal model
const DWORD TOC_DYNAMICFLOW  = 0x00000002; //is dynamic model (flow)
const DWORD TOC_DYNAMICFULL  = 0x00000004; //is dynamic model (pressure)
const DWORD TOC_ELECTRICAL   = 0x00000008; //is dynamic model (electrical)

const DWORD TOC_SOLVE_MASK   = 0x0000000f; //mask for bits associated with solve mode groups

//options associated with license options... (should use ONE of the following)
const DWORD TOC_STD_KENWALT  = 0x00000010; //part of standard Kenwalt models
const DWORD TOC_HEATBAL      = 0x00000020; //Kenwalt Heat Exchange models
const DWORD TOC_POWERPLANT   = 0x00000040; //Kenwalt Power Plant models (was "Extra Heat Exchange" before Build 126)
const DWORD TOC_SIZEDIST     = 0x00000080; //Kenwalt size-distribution/communition models
const DWORD TOC_ALUMINA      = 0x00000100; //Kenwalt alumina models
const DWORD TOC_SMDKRUNTIME  = 0x00000200; //SMDK runtime BlackBox models
const DWORD TOC_POWERDIST    = 0x00000400; //Power Distribution
const DWORD TOC_CLIENT         = 0x00000800; //custom/client models (eg Argyle, Cleveland, Simutron, etc)
const DWORD TOC_MDL_UNUSED4  = 0x00001000; //
const DWORD TOC_MDL_UNUSED3  = 0x00002000; //
const DWORD TOC_MDL_UNUSED2  = 0x00004000; //
const DWORD TOC_MINESERVE    = 0x00008000; //
const DWORD TOC_RTTS         = 0x00010000; //Rio Tinto Technical Services owned models
const DWORD TOC_ALCAN        = 0x00020000; //Alcan/Nabalco owned models
const DWORD TOC_QALEXTRA     = 0x00040000; //additional QAL owned models
const DWORD TOC_QAL          = 0x00080000; //standard QAL owned models

const DWORD TOC_MDL_MASK     = 0x000ffff0; //mask for bits associated with licensing model groups

//options for groups/sorting... (should use ONE of the following)
const DWORD TOC_GRP_GENERAL   = 0x80000000; //general group
const DWORD TOC_GRP_ENERGY    = 0x40000000; //energy transfer group
const DWORD TOC_GRP_SEPAR     = 0x20000000; //mass separation group
const DWORD TOC_GRP_SIZEDST   = 0x10000000; //size distribution group
const DWORD TOC_GRP_TRANSFER  = 0x08000000; //mass transfer group
const DWORD TOC_GRP_ALUMINA   = 0x04000000; //alumina group
const DWORD TOC_GRP_POWERDIST = 0x02000000; //Power Reticulation/Elect/Pneumatic
const DWORD TOC_GRP_UNUSED5   = 0x01000000; //? group
const DWORD TOC_GRP_UNUSED4   = 0x00800000; //? group
const DWORD TOC_GRP_UNUSED3   = 0x00400000; //? group
const DWORD TOC_GRP_UNUSED2   = 0x00200000; //? group
const DWORD TOC_GRP_UNUSED1   = 0x00100000; //? group

const DWORD TOC_GRP_MASK      = 0xfff00000; //mask for bits associated with licensing model groups

//common combinations...
const DWORD TOC_ALL     = TOC_PROBAL | TOC_DYNAMICFLOW | TOC_DYNAMICFULL;
const DWORD TOC_STDALL  = TOC_PROBAL | TOC_DYNAMICFLOW | TOC_DYNAMICFULL | TOC_STD_KENWALT;
const DWORD TOC_SYSTEM  = TOC_PROBAL | TOC_DYNAMICFLOW | TOC_DYNAMICFULL | TOC_STD_KENWALT | TOC_GRP_GENERAL ;

//===========================================================================

#if WITHTAGMONITOR

class DllImportExport CFieldMonitor
  {
  public:
    static bool   m_On;
    static DWORD  m_dwCount;
    static DWORD  gs_dwChgSeqNo;
    DWORD         m_dwChgSeqNo;
    int           m_iIndex;
    Strng         m_sFldTag;
    Strng         m_sFldCnv;
    DataUnion     m_InitialValue;
  };

class DllImportExport CFieldMonitorTOCls
  {
  public:
    CFieldMonitorTOCls()
      {
      }

  public:
    CMap  <char*, char*, CFieldMonitor*, CFieldMonitor*> m_Map;
    CList <CFieldMonitor, CFieldMonitor&> m_List;
  };

class DllImportExport CFieldMonitorTO
  {
  public:
    CFieldMonitorTO(TaggedObject * pObj)
      {
      m_pObj=pObj;
      }
    ~CFieldMonitorTO()
      {
      for (int i=0; i<m_CurrentValue.GetSize(); i++)
        delete m_CurrentValue[i];
      }

  public:
    CArray <DataUnion*, DataUnion*> m_CurrentValue;
  protected:
    TaggedObject    * m_pObj;
  };

#endif

//===========================================================================
//
//
//
//===========================================================================

#if WITHEQUIPSPECS        

class DllImportExport CTOCustomData
  {
  public:
    class DllImportExport CDataSet
      {
      public:
        class DllImportExport CField
          {
          public:
            CField(TagObjClass * pClass, int Index);
            ~CField();

            bool          Load(CProfINIFile & SCO, LPCSTR FieldDefn);
            void          BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwner);

          public:
            TagObjClass * m_pClass;
            int           m_iIndex;

            Strng         m_sSym;
            Strng         m_sTag;
            byte          m_iType;
            Strng         m_sDefaultValue;
            DDEF_Flags    m_iOptions;
            CCnvIndex     m_iCnv;
            Strng         m_sCnvText;
          };

      public:
        CDataSet(TagObjClass * pClass);
        ~CDataSet();

        bool          Load(CProfINIFile & SCO, LPCSTR DataSetName);
        void          Clear();

        int           FieldCount()     { return m_Fields.GetCount(); };
        CField      & Field(int i)     { return *(m_Fields[i]); };

      public:
        TagObjClass * m_pClass;
        Strng         m_sName;
        Strng         m_sDspName;
        Strng         m_sDspNameTag;
        CArray<CField*, CField*> m_Fields;
      };

  public:
    CTOCustomData(TagObjClass * pClass);
    ~CTOCustomData();

    bool              Load(CProfINIFile & SCO);
    void              Clear();
    DDBValueLstMem  * DDBSetNames() { return &m_DDBDataSets; };

    int               DataSetCount()      { return m_DataSets.GetCount(); };
    CDataSet &        DataSet(int i)      { return *(m_DataSets[i]); };

    void              BuildDataDefnHead(DataDefnBlk & DDB, TaggedObject * pOwner);
    void              BuildDataDefnData(DataDefnBlk & DDB, TaggedObject * pOwner);
    flag              DataXchg(DataChangeBlk &DCB, TaggedObject * pOwner);

    void              DbgDump();

  protected:
    TagObjClass     * m_pClass;
    CArray<CDataSet*, CDataSet*> m_DataSets;
    DDBValueLstMem    m_DDBDataSets;

  };

//===========================================================================

class DllImportExport CTOSelectData
  {
  public:

    static const int MaxTOSelectDataSetDepth = 5;
    
    static const byte VS_None    = 0x00;
    static const byte VS_Set     = 0x01;
    static const byte VS_Changed = 0x02;
    static const byte VS_Reset   = 0x04; // ?? To Defaults

  public:
    class DllImportExport CDataSet
      {
      public:
        class DllImportExport CField
          {
          public:
            CField(TagObjClass * pClass, int Index);
            ~CField();

            bool          Load(CProfINIFile & SCO, LPCSTR FieldDefn);
            //void          BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwner);

          public:
            TagObjClass * m_pClass;
            int           m_iIndex;

            Strng         m_sName;
            byte          m_iType;
            Strng         m_sDstPartTag;
            CCnvIndex     m_iCnv;
            Strng         m_sCnvText;
            Strng         m_sDefaultValue;

          };

      public:
        class DllImportExport CRecord
          {
          public:
            CRecord(TagObjClass * pClass, int Index);
            ~CRecord();

            bool          Load(CProfINIFile & SCO, LPCSTR FieldDefn);
            //void          BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwner);

          public:
            TagObjClass * m_pClass;                     
            int           m_iIndex;

            CArray<Strng, Strng&> m_Values;
          };

      public:
        CDataSet(TagObjClass * m_pClass, CTOSelectData * p);
        ~CDataSet();

        bool          Load(CProfINIFile & SCO, LPCSTR DataSetName);
        void          Clear();

        int           FieldCount()        { return m_Fields.GetCount(); };
        CField      & Field(int i)        { return *(m_Fields[i]); };

        int           RecordCount()       { return m_Records.GetCount(); };
        CRecord     & Record(int i)       { return *(m_Records[i]); };

        DDBValueLstMem * DDBRecordSlct() { return &m_DDBRecords; };

        void          BuildDataDefnHead(DataDefnBlk & DDB, TaggedObject * pOwner, int Level);
        flag          DataXchg(DataChangeBlk &DCB, TaggedObject * pOwner, int Level);

        flag          ChangeSelection(bool ForFiling, int SelectedRecord, TaggedObject * pOwner);

      public:
        TagObjClass     * m_pClass;
        CTOSelectData   * m_pSelect;
        Strng             m_sName;
        Strng             m_sDspName;
        Strng             m_sDspNameTag;
        Strng             m_sDisplayFld;
        Strng             m_sDisplayCnvTxt;
        Strng             m_sOrderByFld;

        int               m_iDisplayFld;
        int               m_iOrderByFld;
        int               m_iChildTblFld;

        int               m_iSelectedRecord;

        CDataSet        * m_pChildDS;

        CArray<CField*, CField*> m_Fields;
        CArray<CRecord*, CRecord*> m_Records;
        DDBValueLstMem    m_DDBRecords;
      };
    
    class CTagStatus
      {
      public:
        Strng         m_Tag1;
        Strng         m_Tag2;
        byte          m_Status;
      };

  public:
    CTOSelectData(TagObjClass * pClass);

    bool              Load(CProfINIFile & SCO);
    void              Clear();
    
    CDataSet        * AddDataSet(CProfINIFile & SCO, LPSTR Table);
    CDataSet        * FindDataSet(LPSTR Table);

    int               DataSetCount()   { return m_DataSets.GetCount(); };
    CDataSet        & DataSet(int i)   { return *(m_DataSets[i]); };
    int               TagStatusListCount() { return m_TagStatusList.GetCount(); };

    void              BuildDataDefnHead(DataDefnBlk & DDB, TaggedObject * pOwner);
    //void              BuildDataDefnData(DataDefnBlk & DDB, TaggedObject * pOwner);
    flag              DataXchg(DataChangeBlk &DCB, TaggedObject * pOwner);

    void              SetTagValueStatus(LPSTR Tag1, LPSTR Tag2, CTagValueStatus TagStatus);
    CTagValueStatus   GetTagValueStatus(LPSTR Tag);

    flag              GetFirstClassVariableStatusEntry(POSITION & Pos);
    flag              GetNextClassVariableStatusEntry(POSITION & Pos, Strng &Tag1, Strng &Tag2, CTagValueStatus & Status);

    void              DbgDump();

  protected:
    TagObjClass     * m_pClass;
    CDataSet        * m_pRootDS;
    CArray<CDataSet*, CDataSet*> m_DataSets;

    CList<CTagStatus*, CTagStatus*> m_TagStatusList;
    CMap<LPSTR, LPSTR, CTagStatus*, CTagStatus*> m_TagStatusMap;
  
  };

//===========================================================================

class DllImportExport CTODefaultData
  {
  public:
     CTODefaultData();

  protected:
    ;

  };
#endif

//===========================================================================

//typedef TaggedObject *(*pTagObjConstruct)(LPSTR, LPSTR, short);

typedef flag (*fn_BuildDataDefnGlobal)(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB);
typedef flag (*fn_DataXchgGlobal)(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB);

//===========================================================================

enum eAssocGrfTypes
  {
    eAG_None,
    eAG_Spill, eAG_Vent, eAG_Leak,                   // Real IOs
    eAG_Makeup, eAG_Bleed,
    eAG_Src, eAG_Snk, eAG_Accum, eAG_Deplete,        // Direct
    eAG_Tear,
    eAG_RB,
    eAG_Last
  };

typedef byte eAssocGrfShows;
const byte eAGS_AsPerClass    = 0;
const byte eAGS_Hide          = 1;
const byte eAGS_Show          = 2;
const byte eAGS_ShowInUse     = 3;
const byte eAGS_ShowEverUsed  = 4;

// Connect Useage Status
const byte CUseMsk_InUse    = 0x01;
const byte CUseMsk_WasUsed  = 0x02;
const byte CUseMsk_Exists   = 0x80;
  
class CAssocGrfShowArray 
  {
  public:
    CAssocGrfShowArray()                                          { memset(m_Show, 0, eAG_Last); }
    CAssocGrfShowArray(const CAssocGrfShowArray & V)              { memcpy(m_Show, V.m_Show, eAG_Last); }
    CAssocGrfShowArray & operator=(const CAssocGrfShowArray & V)  { memcpy(m_Show, V.m_Show, eAG_Last); return *this; }
    byte & operator [](eAssocGrfTypes i)                          { return m_Show[i]; }

  public:
    byte     m_Show[eAG_Last];
  };

//===========================================================================
// MInitialiseTest moved to md_share3.h

class DllImportExport TagObjClass
  {
  friend class TaggedObject;
  friend class DataDefnBlk;
  friend class MSpModelDefBase;
  friend class MSpModelDefBaseEx;
  friend class MUnitDefBase;
  friend class MUnitDefBaseEx;
  friend class DDBDataInfo;
  friend class CClassLicenseOptions;

  public:
    TagObjClass(LPSTR pClassName_, LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId_, LPSTR pVersion_, LPSTR pDrwGroup_, LPSTR pTagInitialID_, dword dwCat_, LPSTR pShortDesc_, LPSTR pDesc_, DWORD dwSelectMask_);
    virtual ~TagObjClass();
    static void    DumpAll();
    virtual void   Clear();
    static void    ClearAll();

    static TagObjClass * AddCOMClass(char* pProgID, char* pClassName_, char* pGroup_, char* pClassId_, char* pVersion_, char* pDrwGroup_, char* pTagInitialID_, dword dwCat_, char* pShortDesc_, char* pDesc_, DWORD dwSelectMask_);

    static TagObjClass* FindClassName(LPSTR ClassName_);
    static TagObjClass* FindClassId(LPSTR ClassId_, flag fWithVersion=True, LPCTSTR SubClass=NULL);
    static TagObjClass* FindGroupId(LPSTR GroupId_);
    static TagObjClass* FindClassIndexed(LPSTR GroupId_, int iIndex);
    TagObjClass*   FindGrpClassId(LPSTR ClassId_);
    void           GetShortDesc(Strng &S);
    TagObjClass*   FindGrpShortDesc(LPSTR ShortDescd_);
    TagObjClass*   FindDuplicateClassId(TagObjClass* pStartAt);
    static TaggedObject * Construct(LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    static flag    EditorExists(LPSTR pGroup_, LPSTR pClassId_, LPSTR pTag);

    TaggedObject *  ConstructGrp(LPSTR pClassId_, LPSTR pSubClassId, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    TaggedObject *  ConstructGrpDesc(LPSTR pClassId_, LPSTR pSubClassId, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach);

    virtual TaggedObject * Construct(LPSTR pSubClassId, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach);

    virtual pFxdEdtBookRef ConstructEdit(pFxdEdtView pView_, TaggedObject * pObj) {return NULL;};
    virtual flag   EditorExists() {return 0;};

    LPSTR          ProgID()                                    { return m_sProgID(); };
    LPSTR          ClassName()                                 { return sClassName(); };
    LPSTR          ClassId()                                   { return sClassId(); };
    LPSTR          SubClassId() ;//                            { return strchr(sClassId(), '.'); };
    LPSTR          BaseClassId() ;//                           { return strchr(sClassId(), '.'); };
    LPSTR          TableId()                                   { return sTableId(); };
    LPSTR          MdlLibName()                                { return sMdlLibName(); };
    void           SetMdlLibName(LPCTSTR Name)                 { sMdlLibName=Name; };
    int            ClassIdLenNoVersion()                       { return iClassIdLenNoVersion; };
    LPSTR          ClassIdNoVersion()                          { return sClassIdNV(); };
    LPSTR          Group()                                     { return sGroup(); };
    LPSTR          Desc()                                      { return sDesc(); };
    LPSTR          ShortDesc()                                 { return sShortDesc(); };
    void           SetSubConstruct(MSubConstructBaseDef * P)   { m_pSubConstruct=P; };
    MSubConstructBaseDef * SubConstruct()                      { return m_pSubConstruct; };
#if WITHTAGMONITOR
    CFieldMonitorTOCls & Monitor()                             { return m_Monitor; }
#endif
    const type_info * TypeId()                                 { return m_pTypeId; }
    void SetTypeId(const type_info * id)                       { m_pTypeId=id; }
    fn_BuildDataDefnGlobal GlobalFnBuildDataDefn()             { return m_pGlobalFnBuildDataDefn; };
    fn_DataXchgGlobal GlobalFnDataXchg()                       { return m_pGlobalFnDataXchg; };
    void *         GlobalFnData1()                             { return m_pGlobalFnData1; };
    void *         GlobalFnData2()                             { return m_pGlobalFnData2; };
    void SetDataDefnGlobalFn(fn_BuildDataDefnGlobal pBuild, fn_DataXchgGlobal pXchg) { m_pGlobalFnBuildDataDefn = pBuild; m_pGlobalFnDataXchg=pXchg; };
    void SetDataDefnGlobalFn(fn_BuildDataDefnGlobal pBuild, fn_DataXchgGlobal pXchg, void * Data1, void * Data2)
       { m_pGlobalFnBuildDataDefn = pBuild; m_pGlobalFnDataXchg=pXchg; m_pGlobalFnData1 = Data1; m_pGlobalFnData2 = Data2; };
    void SetDataDefnGlobalFnData(void * Data1, void * Data2)   { m_pGlobalFnData1 = Data1; m_pGlobalFnData2 = Data2; };

    dword          Category()                                  { return dwCategory; };
    dword          CategoryMdl()                               { return (dwCategory & TOC_MDL_MASK); };
    dword          CategoryGrp()                               { return (dwCategory & TOC_GRP_MASK); };
    flag           CatIsDynamic()                              { return ((dwCategory & TOC_DYNAMICFLOW)==TOC_DYNAMICFLOW || (dwCategory & TOC_DYNAMICFULL)==TOC_DYNAMICFULL); };
    flag           CatIsDynamicFlow()                          { return ((dwCategory & TOC_DYNAMICFLOW)==TOC_DYNAMICFLOW); };
    flag           CatIsDynamicFull()                          { return ((dwCategory & TOC_DYNAMICFULL)==TOC_DYNAMICFULL); };
    flag           CatIsProBal()                               { return ((dwCategory & TOC_PROBAL)==TOC_PROBAL); };
    flag           CatMdlTest(dword dwCat)                     { return ((TOC_MDL_MASK & dwCategory & dwCat)==(TOC_MDL_MASK & dwCategory)); }; //all mdl test criteria met
    flag           CatModeTest(dword dwCat)                    { return ((TOC_SOLVE_MASK & dwCategory & dwCat)!=0); };
    flag           TestLicenseOK(dword dwCat, flag LicErrNotedOn)    { m_bLicenseOK=(CatMdlTest(dwCat)/* && CatModeTest(dwCat)*/); m_bLicErrNoted=LicErrNotedOn; return m_bLicenseOK; };
    flag           LicenseOK()                                 { return m_bLicenseOK; };
    flag           SetLicErrNoted(flag On)                     { m_bLicErrNoted = On; return m_bLicErrNoted; };
    flag           LicErrNoted()                               { return m_bLicErrNoted; };

    short          NIdNos()                                    { return iNIds; };
    short          IdNo()                                      { return iId; };
    short          GroupIdNo()                                 { return iGrpId; };

    LPSTR          DrwGroup()                                  { return sDrwGroup(); };
    LPSTR          TagInitialID()                              { return m_sTagInitialID(); };
    LPSTR          TagFormat()                                 { return m_sTagFormat.GetLength()>0 ? m_sTagFormat() : sm_sTagFormat(); };
    LPSTR          TagComment()                                { return sTagComment(); };

    CAssocGrfShowArray & AssocGrfShow()                    { return m_AssocGrfShow; }       
    bool           AssocGrfShowInited()                    { return m_AssocGrfShowInited; } 

    void           InitAssocGrfShowPB(flag PBMode, eAssocGrfShows Spill, eAssocGrfShows Vent, eAssocGrfShows Leak, eAssocGrfShows Makeup, eAssocGrfShows Bleed, eAssocGrfShows Tear, eAssocGrfShows RB)
      {
      if (PBMode && !m_AssocGrfShowInited)
        {
        m_AssocGrfShow[eAG_Spill] = Spill;
        m_AssocGrfShow[eAG_Vent] = Vent;
        m_AssocGrfShow[eAG_Leak] = Leak;
        m_AssocGrfShow[eAG_Makeup] = Makeup;
        m_AssocGrfShow[eAG_Bleed] = Bleed;
        m_AssocGrfShow[eAG_Tear] = Tear;
        m_AssocGrfShow[eAG_RB] = RB;
        m_AssocGrfShowInited=true;
        }
      }
    void           InitAssocGrfShowDyn(flag DynMode, eAssocGrfShows Spill, eAssocGrfShows Vent, eAssocGrfShows Leak, eAssocGrfShows Makeup, eAssocGrfShows Bleed, eAssocGrfShows Tear, eAssocGrfShows RB)
      {
      if (DynMode && !m_AssocGrfShowInited)
        {
        m_AssocGrfShow[eAG_Spill] = Spill;
        m_AssocGrfShow[eAG_Vent] = Vent;
        m_AssocGrfShow[eAG_Leak] = Leak;
        m_AssocGrfShow[eAG_Makeup] = Makeup;
        m_AssocGrfShow[eAG_Bleed] = Bleed;
        m_AssocGrfShow[eAG_Tear] = Tear;
        m_AssocGrfShow[eAG_RB] = RB;
        m_AssocGrfShowInited=true;
        }
      }

    flag           Selectable();
    void           SetSelectable(flag Select)                  { m_bSelectable=Select; };

    flag           Flagged()                                   { return bFlagged; };
    void           SetFlagged(flag Flagged)                    { bFlagged=Flagged; };

    static TagObjClass* FirstClass()                           { return pFirstClass; };
    TagObjClass*   NextClass()                                 { return pNxtClass; };
    TagObjClass*   FirstClassInGrp ()                          { return pFirstClassInGrp; };
    TagObjClass*   NextClassInGrp ()                           { return pNxtClassInGrp; };
    TagObjClass*   operator()()                                { return this; };

    flag           DuplicateExists()                           { return fDuplicateExists; };
    void           SetDuplicateExists(flag Exists)             { fDuplicateExists=Exists; };

    static void    GetClassValueLst(char * GroupName, DDBValueLstMem & DDB0, char * Value0=NULL, DWORD SelectMask=0xffffffff);
    static void    GetSDescValueLst(char * GroupName, DDBValueLstMem & DDB0, char * Value0=NULL, DWORD SelectMask=0xffffffff);

    void           IncInstances()                              { m_nInstances++; m_nInstancesMax=Max(m_nInstancesMax,m_nInstances); };
    void           DecInstances()                              { m_nInstances--; };
    DWORD          Instances()                                 { return m_nInstances; };
    DWORD          InstancesMax()                              { return m_nInstancesMax; };

#if WITHCLASSOPTIONS
    flag           CreateClassOptions(LPCSTR File);
    flag           ReadClassOptions(LPCSTR Path);
#endif

#if WITHEQUIPSPECS
    CTOCustomData *CustomData() { return m_CustomData.DataSetCount()>0 ? &m_CustomData:NULL; };
    CTOSelectData *SelectData() { return m_SelectData.DataSetCount()>0 ? &m_SelectData:NULL; };
#endif

  protected:
    static TagObjClass* pFirstClass;
    static short   iNIds;
    short          iId;
    bool           m_bIsCOMClass;
    bool           m_bMustDelete;
    Strng          m_sProgID;
    Strng          sClassName;
    Strng          sClassId;
    Strng          m_sSubClassId;
    Strng          m_sBaseClassId;
    Strng          sClassIdNV;
    Strng          sTableId;
    Strng          sMdlLibName;
    Strng          sShortDesc;
    Strng          sDesc;
    Strng          sTagComment;
    Strng          sDrwGroup;
    Strng          m_sTagInitialID;
    Strng          m_sTagFormat;
    static Strng   sm_sTagFormat;
    TagObjClass*   pNxtClass;

    dword          dwCategory; //flags for category/grouping/licensing

    dword          dwObjDataVerNo;

    MSubConstructBaseDef * m_pSubConstruct;
    int            iClassIdLenNoVersion;

    Strng          sGroup;
    short          iGrpId;
    TagObjClass*   pFirstClassInGrp;
    TagObjClass*   pNxtClassInGrp;

    flag           m_bSelectable,
                   m_bLicenseOK,
                   m_bLicErrNoted,
                   m_bCreateIfNotLicensed,
                   bFlagged, //temp flag
                   fDuplicateExists;

    DWORD          m_nInstances;
    DWORD          m_nInstancesMax;
    DWORD          dwSelectMask;

    const type_info * m_pTypeId;

    fn_BuildDataDefnGlobal  m_pGlobalFnBuildDataDefn;
    fn_DataXchgGlobal       m_pGlobalFnDataXchg;
    void         * m_pGlobalFnData1;
    void         * m_pGlobalFnData2;

#if WITHTAGMONITOR
    CFieldMonitorTOCls m_Monitor;
#endif

#if WITHEQUIPSPECS        
    static bool    m_bEquipSpecs;
    CTOCustomData  m_CustomData;
    CTOSelectData  m_SelectData;
#endif

    CAssocGrfShowArray  m_AssocGrfShow;//
    bool                m_AssocGrfShowInited;//
    //dword          m_AssocGrfOnMask;
  };

inline Strng GetClassIdNoVersion(LPCTSTR pClass)
  {
  Strng ClassNV(pClass);
  const char *pVer=strchr(pClass, '-');
  ClassNV.SetLength(pVer ? (pVer-pClass): strlen(pClass));
  return ClassNV;
  }

// ==========================================================================
//
//
//
// ==========================================================================

class CClassLicenseOptions 
  {
  public:
    CClassLicenseOptions (TagObjClass & TOClass, flag CreateIfNotLicensed)
      {
      TOClass.m_bCreateIfNotLicensed=CreateIfNotLicensed;
      };

  public:
  };

// ==========================================================================
//
//
//
// ==========================================================================

_FWDDEF(DataChangeBlk)
class DllImportExport DataChangeBlk
  {
  public:
    DataChangeBlk(DDBDataInfo *  DataInfo) { pDataInfo=DataInfo; };
    flag           Init(TABOptions Opts, char Type, long Handle, long Handle2, void *p, dword UserInfo)
      { dwRetFlags=0; dwOpts=Opts; iType=Type; lHandle=Handle; lHandle2=Handle2; rC=(char*)p; dwUserInfo=UserInfo; return lHandle>0;};

    flag           ForNDB()           { return (dwOpts&TABOpt_ForNDB)!=0; };
    flag           ForFiling()        { return (dwOpts&TABOpt_ForFile)!=0; };
    flag           ForView()          { return (dwOpts&TABOpt_ForView)!=0; };
    flag           ForViewConstruct() { return (dwOpts&TABOpt_ForViewConstruct)!=0; };
    flag           ForSnapShot()      { return (dwOpts&TABOpt_ForSnapShot)!=0; };
    flag           ForScenario()      { return (dwOpts&TABOpt_ForScenario)!=0; };
    flag           ForFileSnpScn()    { return (dwOpts&(TABOpt_ForFile|TABOpt_ForSnapShot|TABOpt_ForScenario))!=0; };
    double         NParms()           { return pDataInfo->m_nParms; };
    double         DoubleParm(int i)  { return i<pDataInfo->m_nParms ? pDataInfo->m_dParms[i] : 0.0; };

    //flag OperatePropertyMap(CPropertyMap & Map, long iBaseHandle, long iLastHandle, CPropertyBase * pPropBase, TaggedObject * pTgObj, MPropertyValue & WorkPropValue);

    //helpers...
    static void    DoBrowseBtn(char* ext, char* FileDesc, Strng& Path, Strng& File);
    static int     DoEditBtn(HANDLE& hProcess, DWORD& dwProcessId, char* Path, char* File, char* ext=NULL, Strng* pFullFilename=NULL, bool CheckExists=false, char* SubFolder=NULL);

  public:
    DDBDataInfo *  pDataInfo;
    DDEF_Flags     dwRetFlags;
    TABOptions     dwOpts;
    dword          dwUserInfo;
    byte           iType;
    long           lHandle;
    long           lHandle2;
    word           wBlobLen;
    bool           m_bWasNAN;
    double         m_dNANValue;
    union {
      char         C;
      byte         B;
      word         W;
      dword        DW;
      short        S;
      long         L;
      float        F;
      double       D;
      LPSTR        pC;
      void *       pBlob;
      };
    union {
      char        *rC;
      byte        *rB;
      word        *rW;
      dword       *rDW;
      short       *rS;
      long        *rL;
      float       *rF;
      double      *rD;
      char        *rpC;
      void        *rpBlob;
      };

  };


// --------------------------------------------------------------------------

typedef const long XID;
#define ModelXID(i)   ((i)+(XID)  00000000)
#define MdlBsXID(i)   ((i)+(XID)  10000000)
#define FlwXID(i)     ((i)+(XID)  20000000)
#define FlwBlkXID(i)  ((i)+(XID)  30000000)
#define SVXID(i)      ((i)+(XID)  40000000)
#define SpcXID(i)     ((i)+(XID)  50000000)
#define RctXID(i)     ((i)+(XID)  60000000)
#define VleXID(i)     ((i)+(XID)  70000000)
#define EvapXID(i)    ((i)+(XID)  71000000)
#define AdjustXID(i)  ((i)+(XID)  72000000)
#define EvalBlkXID(i) ((i)+(XID)  79000000)
#define XyXID(i)      ((i)+(XID)  80000000)
#define Qual0XID(i)   ((i)+(XID)  90000000)
#define Qual1XID(i)   ((i)+(XID)  91000000)
#define Qual2XID(i)   ((i)+(XID)  92000000)
#define Qual3XID(i)   ((i)+(XID)  93000000)
#define Qual4XID(i)   ((i)+(XID)  94000000)
#define Qual5XID(i)   ((i)+(XID)  95000000)
#define Qual6XID(i)   ((i)+(XID)  96000000)
#define Qual7XID(i)   ((i)+(XID)  97000000)
#define Qual8XID(i)   ((i)+(XID)  98000000)
#define Qual9XID(i)   ((i)+(XID)  99000000)

#define SysXID(i)     ((i)+(XID) 100000000)
#define PwrXID(i)     ((i)+(XID) 110000000)
#define SMDKXID(i)    ((i)+(XID) 200000000)

XID xidTag                 = SysXID(1);
XID xidParent              = SysXID(2);
XID xidEqDesc              = SysXID(3);
XID xidEqMemo              = SysXID(4);
XID xidEqIdStr             = SysXID(5);
XID xidEqLocation          = SysXID(6);
XID xidEqLink              = SysXID(7);
XID xidEqGUID              = SysXID(8);
XID xidEqRepKey            = SysXID(9);
XID xidPrimaryCfg          = SysXID(10);
XID xidTOCondition         = SysXID(11);
XID xidTOConditionCount    = SysXID(12);

XID xidTOSelectString0     = SysXID(100);
XID xidTOSelectString1     = SysXID(101);
XID xidTOSelectString2     = SysXID(102);
XID xidTOSelectString3     = SysXID(103);
XID xidTOSelectString4     = SysXID(104);

XID xidTOCustomDataSelect  = SysXID(110);

//const int  MaxFullObjTagTmp = 10;

const int TOData_OK          = 0;
const int TOData_NotFound    = 1;
const int TOData_InvalidData = 2;
const int TOData_CommsError  = 3;

//---------------------------------------------------------------------------

class ValidateDataBlk
  {
  public:
    flag           fValidateDataMessagesOn;

    ValidateDataBlk(flag MessagesOn=true)
      {
      fValidateDataMessagesOn=MessagesOn;
      };

  };

//---------------------------------------------------------------------------

class DllImportExport CComTOOwner
  {
  public:
#ifdef WITHSCDINTERFACES
    GIScdTaggedObject m_gpTO;
#else
    void * m_spVOID;
#endif
  };

//---------------------------------------------------------------------------

typedef CList<TaggedObject *, TaggedObject *> CTaggedObjectList;

class DllImportExport TaggedObject : public ConditionBlk
  {
  friend class TagObjClass;
  friend class TagAccessBlk;
  friend class DataDefnBlk;
  friend class CExecutive;
  friend class DDBDataInfo;

  public:
    TaggedObject(TagObjClass* pClass_, LPSTR Tag_, TaggedObject * pAttach, TagObjAttachment eAttach);
    virtual ~TaggedObject();

    void            AttachMeToObj(TaggedObject * pObj, TagObjAttachment eAttach);
    void            DetachMeFromOwner();
    virtual LPSTR   Tag();
    virtual LPSTR   SetTag(LPSTR ReqdTag, bool AllowEmptyTag=false);
    LPSTR           Prefix(LPSTR ReqdPrefix=NULL);
    Strng           PrefixTag();
    static int      TagCmp(LPSTR pTag, LPSTR pSrchTag, int MinTagLen=0);
    static int      TagCmpFn(LPSTR pTag, LPSTR pSrchTag, int MinTagLen=0);
    static void     ValidateTag(Strng & S);
    static void     ValidateTagDot(Strng & S);
    static flag     ValidateTagChanged(Strng & S);
    static flag     ValidateTagDotChanged(Strng & S);
#if WithNumericTags
    static int      TestValidTag(char* pTag, int NumStartBad, int NumBad);
    static int      TestValidTagDot(char* pTag, int NumStartBad, int NumBad);
    static int      TestValidTag(char* pTag) { return TestValidTag(pTag, NumericStartingTagsBad, NumericTagsBad); };
    static int      TestValidTagDot(char* pTag) { return TestValidTagDot(pTag, NumericStartingTagsBad, NumericTagsBad); };
#else
    static int      TestValidTag(char* pTag, int NumStartBad);
    static int      TestValidTagDot(char* pTag, int NumStartBad);
    static int      TestValidTag(char* pTag) { return TestValidTag(pTag, NumericStartingTagsBad); };
    static int      TestValidTagDot(char* pTag) { return TestValidTagDot(pTag, NumericStartingTagsBad); };
#endif
    static flag     CheckNonNumericTagChr(char c);
    static void     SetBadFieldNameChars(char* pBad, char * pRep);
    static int      TestValidFieldName(char* pTag);
    //static void     ValidateFieldName(Strng & S);
    static flag     TestIfReservedTag(char* pTag);

    // Overridables
    virtual void    ResetData(flag Complete) {};
    virtual void    PrepareDefinedData() {}; // Called Before a GetDefined Data - for the root object
    //static virtual flag BuildDataDefnGlobal(DataDefnBlk & DDB) { return false; };
    virtual void    BuildDataDefn(DataDefnBlk & DDB)=0;
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual LPTSTR  GetPrimaryCfg() { return ""; };
    virtual flag    PutPrimaryCfg(LPCTSTR Cfg) { return true; };

  protected:
    static int      SetHoldValidateData(flag On, flag MessagesOn=true);

  public:
    virtual flag    GetOtherData(FilingControlBlock &FCB) {return False;};
    virtual flag    PutOtherData(FilingControlBlock &FCB) {return False;};

    static void     SetGlblTopologyChg(GlblTopologyChgFun *ChgFun, dword ChgData);
    static void     CallGlblTopologyChg(DDBDataInfo * Info);

    static flag     SetGlblResultsAlwaysValid(flag On)
      {
      flag x=bGlblResultsAlwaysValid;
      bGlblResultsAlwaysValidChg=bGlblResultsAlwaysValidChg || bGlblResultsAlwaysValid!=On;
      bGlblResultsAlwaysValid=On;
      return x;
      };
    static flag     GlblResultsAlwaysValid() { return bGlblResultsAlwaysValid; };
    static flag     GlblResultsAlwaysValidChg(flag Clear)
      {
      flag x=bGlblResultsAlwaysValidChg;
      if (Clear)
        bGlblResultsAlwaysValidChg=false;
      return x;
      };

    static flag     SetGlblResultsValid(flag On)
      {
      flag x=bGlblResultsValid;
      bGlblResultsValidChg=bGlblResultsValidChg || bGlblResultsValid!=On;
      bGlblResultsValid=On;
      return x;
      };
    static flag     GlblResultsValid() { return bGlblResultsValid; };
    static flag     GlblResultsValidChg(flag Clear)
      {
      flag x=bGlblResultsValidChg;
      if (Clear)
        bGlblResultsValidChg=false;
      return x;
      };

    flag            FndDefinedData(char * FullItemTag, TABOptions Opts, pDDBShortcut pShortcut);
    flag            GetDefinedData(char * FullItemTag, CPkDataList &DataObj, TABOptions Opts, /*CPkDataItem* &pPItem,*/ pDDBShortcut pShortcut);
    flag            PutDefinedData(char * FullItemTag, CPkDataList &DataObj, CPkDataIter &Iter, TABOptions Opts, /*CPkDataItem* &pPItem,*/ pDDBShortcut pShortcut);

    double          ValidateRange(ValidateDataBlk & VDB, LPCTSTR What, double MinV=dNAN, double V=dNAN, double MaxV=dNAN, flag *pOK=NULL);
    flag            ValidateDefinedData(ValidateDataBlk &VDB);
    flag            UnValidatedData()        { return bUnValidatedData; };

    virtual TaggedObject * FindObjTag(LPSTR pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen);
    virtual void    SetFullObjTag();
    virtual char *  FullObjTag();
    virtual char *  FullObjTag(Strng &S, flag What/*=TOA_All*/);
    virtual LPSTR   TagOfParent()            { return "."; };
    static void     StructureChanged(TaggedObject * pObj);
    LPSTR           BaseTag()                { return pAttachedTo ? pAttachedTo->BaseTag() : Tag(); };
    virtual void    MyTagsHaveChanged(bool FirstOne=true);

    template <class T> T* FindObjOfType(T * X)
      {
      TaggedObject *p=pAttachedTo;
      T *pT=dynamic_cast<T*>(p);
      while (pT==NULL && p)
        {
        p=p->pAttachedTo;
        pT=dynamic_cast<T*>(p);
        }
      return pT;
      }

    dword CurrentDataVersion()               { return dwObjDataVerNo; };// | (wGlblDataVerNo<<16);};

    // Utilities
    virtual void    BuildCommonStartDefn(DataDefnBlk &DDB);
    virtual void    BuildCommonSEDefn(DataDefnBlk &DDB);
    virtual void    BuildCommonEndDefn(DataDefnBlk &DDB);
    virtual void    BuildSystemDefn(DataDefnBlk &DDB) {};
    short           IdNo()                   { return m_pClass->iId; };
    LPSTR           ClassId()                { return m_pClass->sClassId(); };
    LPSTR           SubClassId()             { return m_pClass->SubClassId(); };
    LPSTR           BaseClassId()            { return m_pClass->BaseClassId(); };
    LPSTR           Group()                  { return m_pClass->sGroup(); };
    TagObjClass*    Class()                  { return m_pClass; };
    void            SetClass(TagObjClass*pClass)
      {
      m_pClass=pClass;
      SetSubClass(SubClassId());
      };
    virtual void    SetSubClass(LPSTR pSubClass)  { };
    LPSTR           Desc()                   { return m_pClass->sDesc(); };
    LPSTR           ShortDesc()              { return m_pClass->sShortDesc(); };
    Strng &         TagRef()                 { return m_sTag; };
    Strng &         PrefixRef()              { return m_sPrefix; };

    static CString  CreateGuidStr();
    virtual LPSTR   Guid()                   { return m_sGuid(); };
    virtual LPSTR   SetGuid(LPSTR Guid);//      { if (Guid) m_sGuid = Guid; return m_sGuid(); };

    void            SetEqpDesc(char * Desc)  { m_sEqDesc=Desc; };
    char *          EqpDesc()                { return m_sEqDesc(); };
    void            SetEqpMemo(char * Memo)  { m_sEqMemo=Memo; };
    char *          EqpMemo()                { return m_sEqMemo(); };
    void            SetEqpIdStr(char * IdStr){ m_sEqIdStr=IdStr; };
    char *          EqpIdStr()               { return m_sEqIdStr(); };
    void            SetEqpLocation(char * Location){ m_sEqLocation=Location; };
    char *          EqpLocation()               { return m_sEqLocation(); };
    void            SetEqpLink(long Id)      { m_lEqLink=Id; };
    long            EqpLink()                { return m_lEqLink; };
    void            SetReportKey(long Key)   { m_lReportKey=Key; };
    long            ReportKey()              { return m_lReportKey; };

    pFxdEdtBookRef  ConstructEdit(pFxdEdtView pView_) { return Class()->ConstructEdit(pView_, this); };

#ifdef WITHSCDINTERFACES
    GIScdTaggedObject *m_pgpTO;
#else
    void * m_pgpTO;
#endif

    virtual void    MarkAsCOMOwned(void *pgpTO);
    virtual void    DestroyCOMOwned();

    void            Destroy()               { if (m_pgpTO) DestroyCOMOwned(); else delete this; };

    void            ClearDDBLists();

    void            SetAllowedModes(bool NodeSolve, long AllowedModes);
    void            ValidateModes();

    long            AllModes(long MdMsk=-1); // All Modes

    void            SetNetMethod(long Mode);
    void            SetNetMethod(LPCTSTR Mode);
    long            NetMethod();   
    LPTSTR          NetMethodStr();
    flag            NetProbalMethod()       { return NetMethod() == NM_Probal; };
    flag            NetDynamicMethod()      { return NetMethod() == NM_Dynamic; };

    void            SetSolveMethod(long Mode);
    void            SetSolveMethod(LPCTSTR Mode);
    long            SolveMethod();
    LPTSTR          SolveMethodStr();
    flag            SolveDirectMethod()     { return SolveMethod() == SM_Direct; }; //ProBal or Dynamic
    flag            SolveInlineMethod()     { return SolveMethod() == SM_Inline; }; //Dynamic
    flag            SolveBufferedMethod()   { return SolveMethod() == SM_Buffered; }; //Dynamic
    flag            SolveSurgeMethod()      { return (SolveMethod() & (SM_Inline|SM_Buffered))!=0; }; //Dynamic

    void            SetHeatMethod(long Mode);
    void            SetHeatMethod(LPCTSTR Mode);
    long            HeatMethod();
    LPTSTR          HeatMethodStr();
    flag            HeatSkipMethod()        { return (HeatMethod() & HM_All)==HM_None; };
    flag            HeatSimpleMethod()      { return (HeatMethod() & HM_All)==HM_Reduced; };
    flag            HeatFullMethod()        { return (HeatMethod() & HM_All)==HM_Full; };
    long            FlowModeRqd();
    LPTSTR          FlowModeRqdStr();
    void            SetFlowModeRqd(long FlowMode, long FlowFlags, TaggedObject * pRuleObj);
    void            SetFlowModeRqd(LPCTSTR FlowMode, long FlowFlags, TaggedObject * pRuleObj);

    static void     SetRevert2DefaultRqd(bool On) { gs_Revert2DefaultRqd=On; };
    static bool     Revert2DefaultRqd() { return gs_Revert2DefaultRqd; };
    void            Revert2Default();
    void            SetFlowModeLcl(long Mode);
    void            SetFlowModeLcl(LPCTSTR Mode);
    long            FlowMode();
    long            FlowModePg();
    LPTSTR          FlowModeStr();
    TaggedObject *  FlowModeObj();

    flag            TransferFlowMode()      { return (FlowMode() & LFM_All)==LFM_Xfer; };
    flag            SimpleFlowMode()        { return (FlowMode() & LFM_All)==LFM_Simple; };
    flag            LinearFlowMode()        { return (FlowMode() & LFM_All)==LFM_Linear; };
    flag            FullFlowMode()          { return (FlowMode() & LFM_All)==LFM_Full; };
    

    void            SetActiveOptions(flag HoldPB_OK=true, flag HoldDyn_OK=false, flag HoldIfSS=false, flag MustBeActive=false)
      {
      m_fActiveHoldOK_PB  = HoldPB_OK;
      m_fActiveHoldOK_Dyn = HoldDyn_OK;
      m_fActiveHoldIfSS   = HoldIfSS;
      m_fMustBeActive     = MustBeActive;
      };
    flag            GetActive();
    virtual void    SetActive(flag Active);

    flag            GetActiveHold();
    flag            GetActiveHoldIsAuto();
    flag            GetActiveHoldOK();
    virtual void    SetActiveHold(flag Hold);
    virtual void    SetActiveHoldAuto(flag Hold);

    static flag     GetRunning()            { return fRunning;};
    static flag     GetBuildingAccess()     { return fBuildingAccess; };
    static void     SetBuildingAccess(flag On) { fBuildingAccess=On; };
    static void     SetRunning(flag Running)  { fRunning=Running;};

    flag            DoDbgBrk();

    static void     SplitTagCnv(char *pTag, Strng & Tag, Strng& Cnv);

    // ConditionBlk Overides
    DEFINE_CI(TaggedObject, ConditionBlk, FirstRngChkCI);
    char *          CBTag()                 { return FullObjTag(); };
    void            CBChanged()             { /*StructureChanged(this);*/ };
    flag            GetAllCIs(Strng_List & L, int MaxNo=100);
    void            ClrAllCIs(bool ClrPermanent);
    int             CountAllCIs(int MaxNo=1, dword RqdFlags=0);

    virtual dword   ModelStatus() { return 0; };

#if WITHTAGMONITOR
    CFieldMonitorTO & Monitor()             { return m_Monitor; }
    CFieldMonitorTOCls & ClassMonitor()     { return Class()->m_Monitor; }
#endif

#if WITHEQUIPSPECS
    void           SetCustomDataIndex(int i);
    int            GetCustomDataIndex();
    CTOCustomData::CDataSet* CustomDataSelected();
    CTOCustomData *CustomData()             { return TagObjClass::m_bEquipSpecs && m_pClass->m_CustomData.DataSetCount()>0 ? &m_pClass->m_CustomData:NULL; };
    CTOSelectData *SelectData()             { return TagObjClass::m_bEquipSpecs && m_pClass->m_SelectData.DataSetCount()>0 ? &m_pClass->m_SelectData:NULL; };
#endif

    void           InitAssocGrfShowPB(eAssocGrfShows Spill=eAGS_Hide, eAssocGrfShows Vent=eAGS_Hide, eAssocGrfShows Leak=eAGS_Hide, eAssocGrfShows Makeup=eAGS_Show, eAssocGrfShows Bleed=eAGS_Show, eAssocGrfShows Tear=eAGS_Show, eAssocGrfShows RB=eAGS_Hide)
      { Class()->InitAssocGrfShowPB(NetProbalMethod(), Spill, Vent, Leak, Makeup, Bleed, Tear, RB); }
    void           InitAssocGrfShowDyn(eAssocGrfShows Spill=eAGS_Hide, eAssocGrfShows Vent=eAGS_Hide, eAssocGrfShows Leak=eAGS_Hide, eAssocGrfShows Makeup=eAGS_Show, eAssocGrfShows Bleed=eAGS_Show, eAssocGrfShows Tear=eAGS_Show, eAssocGrfShows RB=eAGS_Hide)
      { Class()->InitAssocGrfShowDyn(!NetProbalMethod(), Spill, Vent, Leak, Makeup, Bleed, Tear, RB); }
    
    bool           AssocGrfOn(eAssocGrfTypes AGType, byte UseStatus);                       
    CAssocGrfShowArray & AssocGrfShowClass()                                 { return Class()->AssocGrfShow();      }       
    CAssocGrfShowArray & AssocGrfShow()                                      { return m_AssocGrfShow;               }       

  protected:
    Strng           m_sTag;
    Strng           m_sGuid;
    Strng           m_sPrefix;
    Strng           m_sSubClass;
    Strng           m_sFullObjTag;
    Strng           m_sEqDesc;
    Strng           m_sEqMemo;
    Strng           m_sEqIdStr;
    Strng           m_sEqLocation;
    long            m_lEqLink;
    long            m_lReportKey;

    static flag     fRunning;
    static flag     fBuildingAccess;

public:
    static long     gs_DefNetMode;
    static long     gs_DefLinkMode;
    static long     gs_DefNodeMode;
    static long     gs_DefHeatMode;
    static long     gs_DefFlowMode;

    static long     gs_MaxLinkMode;
    static long     gs_MaxNodeMode;
    static long     gs_MaxHeatMode;
    static long     gs_MaxFlowMode;

    static long     gs_AllowedLinkMode;
    static long     gs_AllowedNodeMode;
    static long     gs_AllowedHeatMode;
    static long     gs_AllowedFlowMode;

    static long     gs_AllowedNodeModes;
    static long     gs_AllowedLinkModes;

    static bool     gs_Revert2DefaultRqd;

    bool            m_bNodeSolveMode;
    long            m_AllowedModes;
    long            m_FallBackModes;
    long            m_lRunMds;
    long          * m_pRunMds;

    long            m_FlowModeRqd;
    TaggedObject  * m_FlowModeObj;
    int             m_FlowModePass;

protected:
    flag            m_fActive;
    flag            m_fActiveHold;
    flag            m_fActiveHoldIfSS;
    flag            m_fActiveHold_Auto;
    flag            m_fActiveHoldOK_PB;
    flag            m_fActiveHoldOK_Dyn;
    flag            m_fMustBeActive;

    static flag     bGlblResultsValid;
    static flag     bGlblResultsValidChg;
    static flag     bGlblResultsAlwaysValid;
    static flag     bGlblResultsAlwaysValidChg;

    static GlblTopologyChg GlblTopoChg;

  public:
    flag            fDoDbgBrk; // For debugging purposes

    TagObjAttachment eAttachment;
    TaggedObject *  pAttachedTo;
    TaggedObject *  pAttachments;
    TaggedObject *  pNxtAttachment;

#if WITHTAGMONITOR
    CFieldMonitorTO m_Monitor;
#endif
    
    int             m_iCustomData;
    CArray<void*, void*> m_CustomData;

//  byte           iSolveMode;
    flag            bObjModified;
    flag            bUnValidatedData;
    flag            bDoSystemDefn;
    flag            bSkipSnapLoad;
    dword           dwObjDataVerNo;
    static dword    dwGlblDataVerNo;
#if WithNumericTags
    static int      NumericTagsBad;         //if true, tags that only contain numeric characters are illegal
#endif
    static int      NumericStartingTagsBad; //if true, tags starting with a numeric character are illegal
    static char     NonNumericTagChr;       //character to use to make illegal numeric tags good
    static int      m_DisplayTagsOnly;
    static int      m_HideZeros;
    static int      bHoldValidateData;
    static CTaggedObjectList m_List2Validate;
    static Strng    sBadFieldNameChars; // Fields may not contain these
    static Strng    sRepFieldNameChars; // Fields may contain these but they will be replaced
    //static CSArray sRepFieldNameChars;

    flag            m_bFileIOBusy;

    CAssocGrfShowArray  m_AssocGrfShow;//

    Strng           m_sCndStrng;

  protected:
    TagObjClass*   m_pClass;

  private:
  };

//===========================================================================

inline void  TaggedObject::CallGlblTopologyChg(DDBDataInfo * Info)
  {
  if ((Info->iFlags & DDEF_PARAMSTOPPED) && GlblTopoChg.Fun)
    {
    //dbgpln("Call ChgFun:%s", Info->pTag);
    (*GlblTopoChg.Fun)(GlblTopoChg.Data);
    }
  };

//===========================================================================

inline void  StripClassIdVersion(Strng &ID)
  {
  char * p=ID.XStrRChr('-');
  if (p)
    ID.SetLength(p-ID());
  }

//===========================================================================
//
//
//
//===========================================================================

inline int GetDisplayTagsOnly() { return TaggedObject::m_DisplayTagsOnly; };
inline void SetDisplayTagsOnly(int On) { TaggedObject::m_DisplayTagsOnly=On; };
inline int GetHideZeros() { return TaggedObject::m_HideZeros; };
inline void SetHideZeros(int On) { TaggedObject::m_HideZeros=On; };
inline char * GetDisplayTag(char * Tag, char *Sym)
  {
  return (TaggedObject::m_DisplayTagsOnly && (strlen(Tag)>0)) ? Tag : Sym;
  };
inline char * GetDisplayTag(Strng & Tag, Strng & Sym)
  {
  return (TaggedObject::m_DisplayTagsOnly && (Tag.Length()>0)) ? Tag() : Sym();
  };

//===========================================================================
//
//
//
//===========================================================================

#define XHEAP_SPARES 0
//#define XHEAP_SPARES HEAP_SPARES

#if XHEAP_SPARES
class DllImportExport DDBPkListMapSpares : public CList <DDBPkListMap*, DDBPkListMap*> {};
#endif

class DllImportExport DDBPkListMap : public CArray <pDDBPkListItem, pDDBPkListItem>
  {
  public:
#if XHEAP_SPARES
    static DDBPkListMapSpares Spares;
    static DDBPkListMap *Get()
      {
      if (Spares.IsEmpty())
        return new DDBPkListMap;
      return Spares.RemoveHead();
      };
    void      Free() { Spares.AddHead(this); };
#else
    static DDBPkListMap *Get() { return new DDBPkListMap; };
    void      Free() { delete this; };
#endif
  };

//---------------------------------------------------------------------------

class DllImportExport DDBPkListItem
  {
  public:
    CPkDataItem   *pItem;
    byte           iType;
    char          *pTag;
    DDEF_Flags     iFlags;
    UINT           uHash;
    pDDBPkListItem pNextInMap;
    pDDBPkListMap  pMap;

    DDBPkListItem() { pMap=NULL; pNextInMap=NULL;};
    ~DDBPkListItem() { Clear(); };
    void Clear() { if (pMap) pMap->Free(); pMap=NULL; pNextInMap=NULL; };
  };

//---------------------------------------------------------------------------

#if XHEAP_SPARES
class DllImportExport DDBPkListArraySpares : public CList <DDBPkListArray*, DDBPkListArray*> {};
#endif

class DllImportExport DDBPkListArray : public CArray <DDBPkListItem, DDBPkListItem&>
  {
  public:
#if XHEAP_SPARES
    static DDBPkListArraySpares Spares;
    static DDBPkListArray *Get()
      {
      if (Spares.IsEmpty())
        return new DDBPkListArray;
      return Spares.RemoveHead();
      };
    void      Free() { Spares.AddHead(this); };
#else
    static DDBPkListArray *Get() { return new DDBPkListArray; };
    void      Free() { delete this; };
#endif
  };

//---------------------------------------------------------------------------

#if XHEAP_SPARES
class DllImportExport DDBTagMapSpares : public CList <DDBTagMap*, DDBTagMap*> {};
#endif

class DllImportExport DDBTagMap : public CMap <char *, char *, DDBShortcut*, DDBShortcut*>
  {
  public:
#if XHEAP_SPARES
    static DDBTagMapSpares Spares;
    static DDBTagMap *Get()
      {
      if (Spares.IsEmpty())
        return new DDBTagMap;
      return Spares.RemoveHead();
      };
    void      Free() { Spares.AddHead(this); };
#else
    static DDBTagMap *Get() { return new DDBTagMap; };
    void      Free() { delete this; };
#endif
    void AddShortcut(LPSTR pFullItemTag, byte iType, dword dwDataVer, LPSTR pTag, CCnvIndex iCnv, LPSTR pCnv, void * pData, TaggedObject * pThis, DDEF_Flags Flags);
    void AddShortcut(LPSTR pFullItemTag, byte iType, dword dwDataVer, LPSTR pTag, CCnvIndex iCnv, LPSTR pCnv, long lHandle, long lHandle2, TaggedObject * pThis, DDEF_Flags Flags);
  };

//---------------------------------------------------------------------------

struct DDBFnParms
  {
  byte         iType;
  CCnvIndex    iCnv;
  char *       pCnvTxt;
  double       dDefault;
  double       dMin;
  double       dMax;
  };

//---------------------------------------------------------------------------

#if (DoDDBChecks && !(0 && SYSCAD10))

class DllImportExport DDBCheckItem
  {
  public:
    Strng         m_Tag;
    DWORD         m_Add;
    int           m_FileVer;
    flag          m_AltName;
    flag          m_Reported;
    DDEF_Flags    m_iFlags;
    dword         m_UserInfo;
    long          m_lOpenCnt;
  };

typedef CMap <char *, char *, DDBCheckItem*, DDBCheckItem*> DDBCheckMap;
typedef CArray <DDBCheckItem*, DDBCheckItem*> DDBCheckArray;

class DllImportExport DDBCheckClass
  {
  public:
    Strng          m_ClassName;
    DDBCheckMap    m_Map;
    DDBCheckArray  m_Vars;
  };

typedef CArray <DDBCheckClass*, DDBCheckClass*> DDBCheckClassArray;
typedef CMap <char *, char *, DDBCheckClass*, DDBCheckClass*> DDBCheckClassMap;
//typedef CMap <char *, char *, CString, CString &> DDBCheckDuplicateMap;

#define asmGetRetAdd(a)    \
    _asm mov eax, [ebp+4]  \
    _asm mov a, eax;

#define CHKSTUFF0() DWORD asmRetAddress; asmGetRetAdd(asmRetAddress);
#define CHKSTUFF1(pDDB) if (bContinueChecking && gs_DevelopementChecks) DoChecks(asmRetAddress, 0);

#else
#define CHKSTUFF0()  DWORD asmRetAddress=0;
#define CHKSTUFF1(pDDB);
#endif

#ifdef _MANAGED
#undef CHKSTUFF0
#undef CHKSTUFF1
#define CHKSTUFF0()          /* nothing */
#define CHKSTUFF1(pDDB)      /* nothing */
#endif

//---------------------------------------------------------------------------

class DllImportExport DataDefnBlk  : public DDBDataInfo
  {
  friend class TagAccessBlk;
  friend class TaggedObject;
  friend class TagDBaseRec;
  friend class TagDBaseTab;
  friend class TagDBase;

  public:
    DataDefnBlk(CPkDataList * DataObj);//, CPkDataItem ** pPItem);
    ~DataDefnBlk();

    void           Open(TaggedObject *  pRootObj_, char * FullItemTag, DDBTasks Task, TABOptions Opts, pDDBShortcut Shortcut);
    void           Close();
    flag           Analyse(CPkDataIter &Iter);
    void           AnalyseItem(DDBPkListItem *Parent, long & iItemNo, long &MsgNesting);
    UINT           HashValue(char* Key);
    void           AddToParentMap(DDBPkListItem &Parent, DDBPkListItem &Item);
    DDBPkListItem *  FindChildItem(int MsgItemLevel, char *pTag, char *pSym);
    void           dbgItem(DDBPkListItem &Item);

    void           Button(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Button(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Button(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Button(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Button(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Button(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Button(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Button(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }

    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);

    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { CheckBox(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, bool* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL)
      { CheckBox(Tag, Sym, Cnv, CnvTxt, (flag*)Data, This, Flags, ValueLst, pDesc); };
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, bool* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL)
      { CheckBox(Tag, Sym, Cnv, CnvTxt, (flag*)Data, This, Flags, ValueLstMem, pDesc); }
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { CheckBox(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           CheckBox(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { CheckBox(Tag, Sym, Cnv, CnvTxt, Handle, Handle2, This, Flags, (DDBValueLst *)NULL, pDesc); }

    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);

    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, bool* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL)
      { CheckBoxBtn(Tag, Sym, Cnv, CnvTxt, (flag*)Data, This, Flags, ValueLst, pDesc); }
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, bool* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL)
      { CheckBoxBtn(Tag, Sym, Cnv, CnvTxt, (flag*)Data, This, Flags, ValueLstMem, pDesc); }
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { CheckBoxBtn(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { CheckBoxBtn(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           CheckBoxBtn(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { CheckBoxBtn(Tag, Sym, Cnv, CnvTxt, Handle, Handle2, This, Flags, (DDBValueLst *)NULL, pDesc); }

    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);

    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, bool* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL)
      { Bool(Tag, Sym, Cnv, CnvTxt, (flag*)Data, This, Flags, ValueLst, pDesc); }
    void           Bool  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, bool* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL)
      { Bool(Tag, Sym, Cnv, CnvTxt, (flag*)Data, This, Flags, ValueLstMem, pDesc); }
    void           Bool(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, flag* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Bool(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Bool(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Bool(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }

    void           Char  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, char* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Char  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, char* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Char  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Char  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Char(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, char* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Char(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Char(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Char(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           Range (char Min, char Max);

    void           Byte  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, byte* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Byte  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, byte* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Byte  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Byte  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Byte(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, byte* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Byte(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Byte(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Byte(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           Range (byte Min, byte Max);

    void           Word  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, word* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Word  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, word* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Word  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Word  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Word(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, word* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Word(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Word(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Word(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           Range (word Min, word Max);

    void           DWord (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, dword*Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           DWord (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, dword*Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           DWord (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           DWord (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           DWord(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, dword* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { DWord(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           DWord(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { DWord(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           Range (dword Min, dword Max);

    void           Int   (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, int* Data,  TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Int   (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, int* Data,  TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Int   (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Int   (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Int(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, int* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Int(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Int(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Int(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           Range (int Min, int Max);

    void           Short (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, short* Data,TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Short (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, short* Data,TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Short (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Short (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Short (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Short (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);

    void           Short(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, short* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Short(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Short(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Short(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }
    void           Range (short Min, short Max);

    void           Long  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Long  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Long  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Long  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           Long  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           Long  (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);

    void           Long(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Long(Tag, Sym, Cnv, CnvTxt, Data, This, Flags, (DDBValueLst*)NULL, pDesc); }
    void           Long(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR pDesc)
      { Long(Tag, Sym, Cnv, CnvTxt, Handle, This, Flags, (DDBValueLst *)NULL, pDesc); }

    void           Range (long Min, long Max);

    void           Float (LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, float* Data,TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Float (LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Float (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, float* Data,TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Float (LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Range (float Min, float Max);

    //void           Double(LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, double* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR NANStr=NULL, LPCTSTR pDesc=NULL);
    //void           Double(LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR NANStr=NULL, LPCTSTR pDesc=NULL);
    //void           Double(LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, LPCTSTR NANStr=NULL, LPCTSTR pDesc=NULL);
    //void           Double(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, double* Data, TaggedObject * This, DDEF_Flags Flags, LPCTSTR NANStr=NULL, LPCTSTR pDesc=NULL);
    //void           Double(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, LPCTSTR NANStr=NULL, LPCTSTR pDesc=NULL);
    //void           Double(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, LPCTSTR NANStr=NULL, LPCTSTR pDesc=NULL);
    void           Double(LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, double* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Double(LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Double(LPSTR Tag, LPSTR Sym, CCnvFamily * CnvFam, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Double(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, double* Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Double(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Double(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * NANStrLst=NULL, LPCTSTR pDesc=NULL);
    void           Range (double Min, double Max);

    void           FnDouble(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBFnParms * ParmDefs, LPCTSTR pDesc=NULL);

    void           String(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, pStrng Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           String(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, pStrng Data, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           String(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           String(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);
    void           String(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLst * ValueLst=NULL, LPCTSTR pDesc=NULL);
    void           String(LPSTR Tag, LPSTR Sym, CCnvIndex Cnv, LPSTR CnvTxt, long Handle, long Handle2, TaggedObject * This, DDEF_Flags Flags, DDBValueLstMem * ValueLstMem, LPCTSTR pDesc=NULL);

    void           Blob(LPSTR Tag, void *Data, word Size, TaggedObject * This);
    void           Blob(LPSTR Tag, long Handle, TaggedObject * This);

    //void           StrValue(LPSTR pStr);
    //void           IndexedStrValue(int iVal, LPSTR pStr);
    void           Description(LPCTSTR pStr);
    void           TagComment(LPSTR pStr);

    void           SuppressNextBlock() {bBlkSuppressed[iBlkDepth]=1;};
    flag           BeginStruct(TaggedObject * Obj, LPSTR Tag=NULL, LPSTR TagComment=NULL, DDBPages PageIs = DDB_OptPage, dword UserInfo=-1, DDEF_Flags BlkFlags=0);
    void           EndStruct();
    flag           BeginObject(TaggedObject * Obj, LPSTR Tag=NULL, LPSTR ObjectId=NULL, LPSTR TagComment=NULL, DDBPages PageIs = DDB_OptPage, dword UserInfo=-1, DDEF_Flags BlkFlags=0);
    void           EndObject();
    flag           BeginArray(TaggedObject * Obj, LPSTR Tag, LPSTR ClassName, int Len_, long Handle=0, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    flag           BeginElement(TaggedObject * Obj, int iElementIndex, LPSTR TagComment=NULL, dword UserInfo=-1, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    flag           BeginElement(TaggedObject * Obj, LPCTSTR pElementTag, LPSTR TagComment=NULL, dword UserInfo=-1, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    void           EndElement(DDEF_Flags BlkFlags = 0);
    void           EndArray(DDEF_Flags BlkFlags = 0);
    void           PushUserInfo(dword UserInfo=0, TaggedObject * This=NULL);
    void           PopUserInfo();
    void           Object(TaggedObject * Obj, TaggedObject * This, LPSTR Tag=NULL, LPSTR TagComment=NULL, DDBPages PageIs = DDB_OptPage, dword UserInfo=-1, DDEF_Flags BlkFlags=0);
    char *         BlockTag() { return Nest[lCallLevel-1].pTag; }
    DDEF_Flags     Visibility(DDEF_Flags ModeFlags=NSHM_All, flag ViewVisible=TRUE, flag FileVisible=TRUE, flag SnapVisible=TRUE, flag ScenVisible=TRUE);
    DDEF_Flags     GetVisibility();
    DDEF_Flags     SetVisibility(DDEF_Flags VisFlags);

    void           Add_PropertyMap(CPropertyMap & Map, ULONG WhichProps, flag AsParms, long iBaseHandle, CPropertyBase * pPropBase, TaggedObject * pTgObj);

    void           Page(LPSTR Nm, DDBPages PageIs = DDB_OptPage, DDEF_Flags Flags=0);
    void           Text(LPSTR Txt, DDEF_Flags Flags=0);

    flag           BeginMatrix(TaggedObject * Obj, LPSTR Tag, LPSTR ClassName, int MaxCols, int MaxRows, int ColWidth, int Gap, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    flag           BeginMatrix(TaggedObject * Obj, LPSTR Tag, LPSTR ClassName, int MaxCols, int MaxRows, int ColWidth, int Gap, LPSTR ColHdr, LPSTR RowHdr, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    flag           BeginMatrixElement(TaggedObject * Obj, int iColIndex, int iRowIndex, LPSTR TagComment=NULL, dword UserInfo=-1, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    //flag           BeginMatrixElement(TaggedObject * Obj, LPCTSTR pColTag, LPCTSTR pRowTag, LPSTR TagComment=NULL, dword UserInfo=-1, DDBPages PageIs=DDB_NoPage, DDEF_Flags BlkFlags=0);
    void           EndMatrixElement(DDEF_Flags Flags=0);
    void           EndMatrix(DDEF_Flags Flags=0);
 
    flag           BeginGrid(LPSTR Text, int MaxCols, int MaxRows, int Width, int Gap);
    void           ColumnHeader(LPSTR Text, int Width, int Gap, int Justification);
    void           RowHeader(LPSTR Text);
    void           RowStart(LPSTR Text="");
    void           EndGrid(LPSTR Text="");

    // Helper Functions
    void           Add_OnOff(LPSTR pTag, pflag pOn, /*flag bOffisBlank,*/ TaggedObject * pOwn, dword Flags=isParm);
    void           Add_YesNo(LPSTR pTag, pflag pYes, /*flag bNoisBlank,*/ TaggedObject * pOwn, dword Flags=isParm);

    flag           ForNDB()         { return (dwOptions&TABOpt_ForNDB)!=0; };
    flag           ForFiling()      { return (dwOptions&TABOpt_ForFile)!=0; };
    flag           ForView()        { return (dwOptions&TABOpt_ForView)!=0; };
    flag           ForSnapShot()    { return (dwOptions&TABOpt_ForSnapShot)!=0; };
    flag           ForScenario()    { return (dwOptions&TABOpt_ForScenario)!=0; };
    flag           ForFileSnpScn()  { return (dwOptions&(TABOpt_ForFile|TABOpt_ForSnapShot|TABOpt_ForScenario))!=0; };
    flag           DoingGetData()   { return (eTask==DDBTask_GetData); };
    flag           DoingPutData()   { return (eTask==DDBTask_PutData); };

    flag           SetDisplayOnly(flag On)
      {
      flag Ret=(dwOptions&TABOPT_DisplayOnly)!=0;
      dwOptions=On ? (dwOptions|TABOPT_DisplayOnly) : (dwOptions&~TABOPT_DisplayOnly);
      return Ret;
      };
    flag           ForDisplayOnly() { return (dwOptions&TABOPT_DisplayOnly)!=0; };

    flag           SearchingAtLevel() { return  Nest[lCallLevel].fSearching; };
    long           CallLevel() { return  lCallLevel; };
    TABOptions     Options() { return dwOptions; };

  private:
    flag           BeginBlock(byte iType_,  TaggedObject * pObj,
                              LPSTR pTag, LPSTR pClassName, LPSTR pSubClassName, LPSTR pPrimaryCfg,
                              LPSTR pTagOfParent, LPSTR pTagComment, dword UserInfo, long NElems, DDEF_Flags BlkFlags, dword RetAddress);
    void           EndBlock(DDEF_Flags BlkFlags = 0);

    DDEF_Flags     CurrentFlags();

    flag           GetReqdFlags();
    flag           GetReqdItem();
    flag           GetReqdItemFn();
    flag           GetReqdLayout();
    flag           GetReqdBlock(flag & SearchingTag, LPSTR &pSrchTag);
    flag           PutReqdItem();
    flag           PutReqdBlock(flag & SearchingTag, LPSTR &pSrchTag);

    void           DoBool();
    void           DoChar();
    void           DoByte();
    void           DoWord();
    void           DoDWord();
    void           DoInt();
    void           DoShort();
    void           DoLong();
    void           DoFloat();
    void           DoDouble();
    //void           DopChar();
    //void           DoppChar();
    void           DoString();
    void           DoBlob();

    void           DoNANName(LPCTSTR pStr);
    void           DoNANName(DDBValueLst * NANStrLst);

    void           ParseParms(DDBFnParms * ParmDefs);

  private:
    DDBTasks          eTask;
    TABOptions        dwOptions;
    CPkDataList       * m_pList;
    CPkDataItem       * m_pItem;
    //CPkDataItem      *pPItem;
    //CPkDataItem      **ppPItem;
    flag              fAllInfo;
    flag              fValCnvs;
    flag              fStrList;
    char *            m_pFullItemTag;

    flag              fAllDone;
    flag              fWriting;
    flag              fSetShortcut;
    DDBShortcut       * pShortcut;

    char              cElementTag[256]; // Space to remember tag of current array element

    DDBPkListArray    * pItemList;
    long              lNMsgItems;
    long              lMsgItemLevel;
    DDBPkListItem     * pParentItem[MAXSTRUCTNESTING];

    long              lCallLevel;
    struct
      {
      char              iTyp;
      flag              fBlockReqd;
      flag              fItemReqd;
      flag              fSearching;
      flag              fWantAll;
      TaggedObject      * pDataObj;
      DDEF_Flags        dwDefaultFlags;
      dword             dwUserInfo;
      char              * pTag;
      char              * pSrchTag;
      TaggedObject      * pThis;
      //char              * m_CurrentTagEnd;
      long              m_CurrentTagLen;
      }               Nest[MAXSTRUCTNESTING];

    byte              iBlkDepth;
    flag              bBlkSuppressed[MAXSTRUCTNESTING];

    byte              iADepth;
    struct
      {
      LPSTR             pClass;
      long              nElem;
      long              iIndex;
      bool              m_ElementClosed;
      long              m_MaxCols;
      long              m_PrevRow;
      }               AInfo[MAXSTRUCTNESTING];

#if DoDDBChecks
    struct
      {
      DDBCheckClass   * pClass;
      int               iTagEnd;
      char *            pTagStart;
      long              lOpenCnts;
      }               Chk[MAXSTRUCTNESTING];
    char              cChkTag[1024];
    static long       lOpenCnt;

  public:
    void           DoChecks(DWORD RetAddress, dword UserInfo);

    static DDBCheckClassArray   ClassList;
    static DDBCheckClassMap     ClassMap;
    //static DDBCheckDuplicateMap DuplicateMap;

    static DDBCheckClass * FindClass(char * ClassName);
    static void    ClearAllChecks();
    static void    DumpAllChecks();
#endif

  public:
    static flag    debugging;

    static void StartUp()
      {
      }
    static void ShutDown()
      {
#if XHEAP_SPARES
      while (!DDBPkListArray::Spares.IsEmpty())
        {
        DDBPkListArray *p=DDBPkListArray::Spares.RemoveHead();
        for (int i=0; i<p->GetSize(); i++)
          p->ElementAt(i).Clear();
        delete p;
        }
      while (!DDBPkListMap::Spares.IsEmpty())
        delete DDBPkListMap::Spares.RemoveHead();
#endif
      }
  };

// ==========================================================================

enum eGlblModeWhich {eGM_Name, eGM_Default, eGM_Remote, eGM_Propagate, eGM_Propagated, eGM_FallBack };

extern DllImportExport long     GetPermissableModes(long MdMask, TaggedObject * pObject);
extern DllImportExport Strng    GlblMode2String(long Md, long MdMsk);
extern DllImportExport LPTSTR   GlblMode2LPTSTR(long Md, long MdMsk, eGlblModeWhich Which);
extern DllImportExport long     String2GlblMode(LPCTSTR MdStr, long MdMsk);
extern DllImportExport void     GetGlblModeValueLst(DDBValueLstMem & Lst, long MdMask, long DefMd, bool PropMds);

    //static long     GlblRunModes()          { return gs_RunMds; };
extern DllImportExport void     SetDefNetMode(long Mode);
inline                 long     DefNetMode()            { return TaggedObject::gs_DefNetMode & NM_All; };
inline                 LPTSTR   DefNetModeStr()         { return GlblMode2LPTSTR(DefNetMode(), NM_All, eGM_Name); };
inline                 flag     DefNetProbalMode()      { return (TaggedObject::gs_DefNetMode & NM_All) == NM_Probal; };
inline                 flag     DefNetDynamicMode()     { return (TaggedObject::gs_DefNetMode & NM_All) == NM_Dynamic; }

extern DllImportExport void     SetDefNodeMode(long Mode);
inline                 long     DefNodeSolveMode()      { return TaggedObject::gs_DefNodeMode & SM_All; };
inline                 LPTSTR   DefNodeSolveModeStr()   { return GlblMode2LPTSTR(DefNodeSolveMode(), SM_All, eGM_Name); };
extern DllImportExport void     SetDefLinkMode(long Mode);
inline                 long     DefLinkSolveMode()      { return TaggedObject::gs_DefLinkMode & SM_All; };
inline                 LPTSTR   DefLinkSolveModeStr()   { return GlblMode2LPTSTR(DefLinkSolveMode(), SM_All, eGM_Name); };

extern DllImportExport void     SetDefHeatMode(long Mode);
inline                 long     DefHeatMode()           { return TaggedObject::gs_DefHeatMode & HM_All; };
inline                 LPTSTR   DefHeatModeStr()        { return GlblMode2LPTSTR(DefHeatMode(), HM_All, eGM_Name); };
inline                 flag     DefSkipHeat()           { return (TaggedObject::gs_DefHeatMode & HM_All)==HM_None; };
inline                 flag     DefReducedHeat()        { return (TaggedObject::gs_DefHeatMode & HM_All)==HM_Reduced; };
inline                 flag     DefFullHeat()           { return (TaggedObject::gs_DefHeatMode & HM_All)==HM_Full; };

extern DllImportExport void     SetDefFlowMode(long Mode);
inline                 long     DefFlowMode()           { return TaggedObject::gs_DefFlowMode & LFM_All; };
inline                 LPTSTR   DefFlowModeStr()        { return GlblMode2LPTSTR(DefFlowMode(), LFM_All, eGM_Name); };
 
extern DllImportExport void     SetMaxNodeMode(long Mode);
extern DllImportExport void     SetMaxLinkMode(long Mode);
extern DllImportExport void     SetMaxHeatMode(long Mode);
extern DllImportExport void     SetMaxFlowMode(long Mode);

extern DllImportExport long     MaxModes2Mask(long MaxMode);

extern DllImportExport long     MaxNodeMode();
extern DllImportExport long     MaxLinkMode();
extern DllImportExport long     MaxHeatMode();
extern DllImportExport long     MaxFlowMode();

extern DllImportExport long     AllowedNodeModes();
extern DllImportExport long     AllowedLinkModes();
//extern DllImportExport long     AllowedHeatModes();
//extern DllImportExport long     AllowedFlowModes();

inline                 void     SetDefNetMode(LPCTSTR RunMds)  { SetDefNetMode(String2GlblMode(RunMds, NM_All)); }
inline                 void     SetDefLinkMode(LPCTSTR RunMds) { SetDefLinkMode(String2GlblMode(RunMds, SM_All)); }
inline                 void     SetDefNodeMode(LPCTSTR RunMds) { SetDefNodeMode(String2GlblMode(RunMds, SM_All)); }
inline                 void     SetDefHeatMode(LPCTSTR RunMds) { SetDefHeatMode(String2GlblMode(RunMds, HM_All)); }
inline                 void     SetDefFlowMode(LPCTSTR RunMds) { SetDefFlowMode(String2GlblMode(RunMds, LFM_All)); }

//void CExecutive::SetMaxNetMode(LPCTSTR RunMds)  { SetMaxNetMode(TaggedObject::String2GlblMode(RunMds, NM_All)); }
inline                 void     SetMaxLinkMode(LPCTSTR RunMds) { SetMaxLinkMode(String2GlblMode(RunMds, SM_All)); }
inline                 void     SetMaxNodeMode(LPCTSTR RunMds) { SetMaxNodeMode(String2GlblMode(RunMds, SM_All)); }
inline                 void     SetMaxHeatMode(LPCTSTR RunMds) { SetMaxHeatMode(String2GlblMode(RunMds, HM_All)); }
inline                 void     SetMaxFlowMode(LPCTSTR RunMds) { SetMaxFlowMode(String2GlblMode(RunMds, LFM_All)); }

inline                 long     GetDefNetMode(LPCTSTR RunMds)  { return String2GlblMode(RunMds, NM_All); }
inline                 long     GetDefLinkMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, SM_All); }
inline                 long     GetDefNodeMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, SM_All); }
inline                 long     GetDefHeatMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, HM_All); }
inline                 long     GetDefFlowMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, LFM_All); }

//void CExecutive::SetMaxNetMode(LPCTSTR RunMds)  { SetMaxNetMode(TaggedObject::String2GlblMode(RunMds, NM_All)); }
inline                 long     GetMaxLinkMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, SM_All); }
inline                 long     GetMaxNodeMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, SM_All); }
inline                 long     GetMaxHeatMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, HM_All); }
inline                 long     GetMaxFlowMode(LPCTSTR RunMds) { return String2GlblMode(RunMds, LFM_All); }

inline                 LPCTSTR  GetNetModeStr (long RunMds) { return GlblMode2LPTSTR(RunMds, NM_All, eGM_Name); }
inline                 LPCTSTR  GetLinkModeStr(long RunMds) { return GlblMode2LPTSTR(RunMds, SM_All, eGM_Name); }
inline                 LPCTSTR  GetNodeModeStr(long RunMds) { return GlblMode2LPTSTR(RunMds, SM_All, eGM_Name); }
inline                 LPCTSTR  GetHeatModeStr(long RunMds) { return GlblMode2LPTSTR(RunMds, HM_All, eGM_Name); }
inline                 LPCTSTR  GetFlowModeStr(long RunMds) { return GlblMode2LPTSTR(RunMds, LFM_All, eGM_Name); }

// ==========================================================================

class DllImportExport TagAccessBlk
  {
  public :
    TagAccessBlk();
    TagAccessBlk(TaggedObject * pOwner, TaggedObject * pSrchRoot_, LPSTR Tag_, TABOptions Opts, TagUniqueness TU, flag AllowChangeTest=False);
    ~TagAccessBlk();

    TagAccessBlk& operator=(const TagAccessBlk&X);

    void           Init(TaggedObject * pOwner, TaggedObject * pSrchRoot_, LPSTR Tag_, TABOptions Opts, TagUniqueness TU, flag AllowChangeTest=False);
    flag           LoadAddress(flag FndShortcut=True);

    flag           GetDefinedData(CPkDataList &DataObj, TABOptions Opts);
    int            PutDefinedData(CPkDataList &DataObj);
    flag           ValidateDefinedData();
    void           ClearDDBLists();

    void           GotAllDataFromObject();

    byte           Type()           { return m_Shortcut.fOK ? m_Shortcut.iType : tt_NULL; };
    flag           IsNumData()      { return m_Shortcut.fOK ? ::IsNumData(m_Shortcut.iType) : False; };
    flag           IsStrng()        { return m_Shortcut.fOK ? ::IsStrng(m_Shortcut.iType) : False; };
    CCnvIndex      CnvIndex()       { return m_Shortcut.fOK ? m_Shortcut.iCnv : 0; };
    DDEF_Flags     Flags()          { return m_Shortcut.fOK ? m_Shortcut.iFlags : 0; };

    MTagIOValue    GetTagValue(LPSTR pRqdCnvText=NULL);
    int            PutTagValue(MTagIOValue V, LPSTR pRqdCnvText=NULL);
    double         GetDouble(char * pRqdCnvText=NULL);
    long           GetLong();
    int            PutDouble(double D, char * pRqdCnvText=NULL);
    int            PutLong(long L);
    char*          GetString();
    int            PutString(char* p);

    flag           Changed()        {return bChanged; };
    static void    ResetTimers();

  public:
    byte                Ok:1, bAllowChangeTest:1, bChanged:1;
    TagUniqueness       eUnique;
    LPSTR               m_pTagFound;
    Strng               sTag, sCnvTxt, sCnvFam;
    TaggedObject      * pSrchRoot;
    TaggedObject      * pAccObj;
    TABOptions          dwOptions;
    char              * m_pPrevValues;
    dword               m_lPrevValues;
    CTagRefStatus       m_PrevXRefStatus;
    DDBShortcut         m_Shortcut;

    TaggedObject      * m_pOwner;


  public:
#ifndef _MANAGED
    struct CTimes
      {
      long        m_Calls;
      double      m_Time;
      void Reset();
      void Add(CStopWatch & SW);
      } ;
    static CTimes   m_LoadAddress;
    static CTimes   m_GetDefined;
    static CTimes   m_PutDefined;
    static CTimes   m_ValidateDefined;
#endif
  };

//===========================================================================
//
//
//
//===========================================================================

#if DOINLINE
#include "TagObj.inl"
#endif
//===========================================================================
//
//  COM
//
//===========================================================================

class DllImportExport CCOMObjManagerRoot
  {
  public :
    static CCOMObjManagerRoot * gs_Manager;
    //AddClassCCOMObjManagerRoot * gs_Manager;

#if 0
    virtual TaggedObject * Construct(LPCTSTR ProgID, LPSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach)=0;
#endif
    virtual TaggedObject * Construct(TagObjClass *pClass, LPCTSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach)=0;
    virtual void Destroy(TaggedObject * pObj)=0;

  };

// ==========================================================================
//
//
//
// ==========================================================================

#define DEFINE_TAGOBJDERIVED(Obj, Base) \
  _FWDDEF(Obj); \
  class DllImportExport Obj##ClassDef : public Base \
    { \
    public: \
      TaggedObject * ConstructGroup(LPSTR pClass, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach); \
      TaggedObject * ConstructGroupDesc(LPSTR pShortDesc, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach); \
      virtual TaggedObject * Construct(LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach); \
      Obj##ClassDef(LPSTR pClassName_, LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId_, LPSTR pVersion_, LPSTR pDrwGroup_, LPSTR pTagInitialID_, dword dwCat, LPSTR pShortDesc_, LPSTR pDesc_, DWORD dwSelectMask); \
      Obj##ClassDef(); \
    }; \
  extern DllImportExport Obj##ClassDef Obj##Class;

#define DEFINE_TAGOBJ(Obj) \
  DEFINE_TAGOBJDERIVED(Obj, TagObjClass)


#define IMPLEMENT_TAGOBJSLCTDERIVED(Obj, ObjName, Grp, ClassId, SubClass, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, SelectMask, Base) \
TaggedObject * Obj##ClassDef::ConstructGroup(LPSTR pClass, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach) \
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); }; \
TaggedObject * Obj##ClassDef::ConstructGroupDesc(LPSTR pShortDesc, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach) \
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); }; \
TaggedObject * Obj##ClassDef::Construct(LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach) \
  { \
  p##Obj pn = new Obj(this, pTag, pAttach, eAttach); \
  SetTypeId(&typeid(*pn));                           \
  pn->SetSubClass(pSubClass);                        \
  return pn;                                         \
  }; \
Obj##ClassDef::Obj##ClassDef(LPSTR pClassName_, LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId_, LPSTR pVersion_, LPSTR pDrwGroup_, LPSTR pTagInitialID_, dword dwCat, LPSTR pShortDesc_, LPSTR pDesc_, DWORD dwSelectMask) \
  : Base(pClassName_, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup_, pTagInitialID_, dwCat, pShortDesc_, pDesc_, dwSelectMask) { sMdlLibName=MDLLIBNAME; }; \
  Obj##ClassDef::Obj##ClassDef() : Base(ObjName, Grp, ClassId, SubClass, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, SelectMask){ sMdlLibName=MDLLIBNAME; }; \
Obj##ClassDef Obj##Class;

#define IMPLEMENT_TAGOBJ(Obj, Grp, ClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJSLCTDERIVED(Obj, #Obj, Grp, ClassId, NULL, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, 0xffffffff, TagObjClass)

#define IMPLEMENT_TAGOBJEX(Obj, ObjName, Grp, ClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJSLCTDERIVED(Obj, ObjName, Grp, ClassId, NULL, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, 0xffffffff, TagObjClass)

#define IMPLEMENT_TAGOBJSUB(Obj, Grp, ClassId, SubClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJSLCTDERIVED(Obj, #Obj, Grp, ClassId, SubClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, 0xffffffff, TagObjClass)

#define IMPLEMENT_TAGOBJDERIVED(Obj, Grp, ClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, Base) \
  IMPLEMENT_TAGOBJSLCTDERIVED(Obj, #Obj, Grp, ClassId, NULL, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, 0xffffffff, Base)

#define IMPLEMENT_TAGOBJSLCT(Obj, Grp, ClassId, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, SelectMask) \
  IMPLEMENT_TAGOBJSLCTDERIVED(Obj, #Obj, Grp, ClassId, NULL, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, SelectMask, TagObjClass)

// --------------------------------------------------------------------------

#define DEFINE_TAGOBJEDTDERIVED(Obj, Base) \
  _FWDDEF(Obj); \
  class DllImportExport Obj##ClassDef : public Base \
    { \
    public: \
      TaggedObject * ConstructGroup(LPSTR pGroup, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach); \
      TaggedObject * ConstructGroupDesc(LPSTR pShortDesc, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach); \
      virtual TaggedObject * Construct(LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach); \
      Obj##ClassDef(LPSTR pClassName_, LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId_, LPSTR pVersion_, LPSTR pDrwGroup_, LPSTR pTagInitialID_, dword dwCat, LPSTR pShortDesc, LPSTR pDesc, DWORD dwSelectMask); \
      Obj##ClassDef(); \
      virtual flag EditorExists() {return 1;}; \
      virtual pFxdEdtBookRef ConstructEdit(pFxdEdtView pView_, TaggedObject * pObj); \
    }; \
  extern DllImportExport Obj##ClassDef Obj##Class;

#define DEFINE_TAGOBJEDT(Obj) \
  DEFINE_TAGOBJEDTDERIVED(Obj, TagObjClass)


#define IMPLEMENT_TAGOBJEDTSLCTDERIVED(Obj, Grp, ClassId, SubClass, Version, DrwGroup, TagInitialID, Cat, Edt, ShortDesc, Desc, SelectMask, Base) \
TaggedObject * Obj##ClassDef::ConstructGroup(LPSTR pGroup, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach) \
  { return TagObjClass::ConstructGrp(pGroup, pSubClass, pTag, pAttach, eAttach); }; \
TaggedObject * Obj##ClassDef::ConstructGroupDesc(LPSTR pShortDesc, LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach) \
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); }; \
TaggedObject * Obj##ClassDef::Construct(LPSTR pSubClass, LPSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach) \
  { \
  p##Obj pn = new Obj(this, pTag, pAttach, eAttach); \
  SetTypeId(&typeid(*pn));                           \
  pn->SetSubClass(pSubClass);                        \
  return pn;                                         \
  };  \
pFxdEdtBookRef Obj##ClassDef::ConstructEdit(pFxdEdtView pView_, TaggedObject * pObj) \
  { return ::new Edt(pView_, (Obj*)pObj); }; \
Obj##ClassDef::Obj##ClassDef(LPSTR pClassName_, LPSTR pGroup_, LPSTR pClassId_, LPSTR pSubClassId_, LPSTR pVersion_, LPSTR pDrwGroup_, LPSTR pTagInitialID_, dword dwCat, LPSTR pShortDesc, LPSTR pDesc, DWORD dwSelectMask) \
  : Base(pClassName_, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup_, pTagInitialID_, dwCat, pShortDesc, pDesc, dwSelectMask) { sMdlLibName=MDLLIBNAME; }; \
Obj##ClassDef::Obj##ClassDef() : Base(#Obj, Grp, ClassId, SubClass, Version, DrwGroup, TagInitialID, Cat, ShortDesc, Desc, SelectMask) { sMdlLibName=MDLLIBNAME; }; \
Obj##ClassDef Obj##Class;

#define IMPLEMENT_TAGOBJEDT(Obj, Grp, ClassId, Version, TagInitialID, Cat, Edt, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJEDTSLCTDERIVED(Obj, Grp, ClassId, NULL, Version, "Edit", TagInitialID, Cat, Edt, ShortDesc, Desc, 0xffffffff, TagObjClass)

#define IMPLEMENT_TAGOBJEDTDERIVED(Obj, Grp, ClassId, Version, TagInitialID, Cat, Edt, ShortDesc, Desc, Base) \
  IMPLEMENT_TAGOBJEDTSLCTDERIVED(Obj, Grp, ClassId, NULL, Version, "Edit", TagInitialID, Cat, Edt, ShortDesc, Desc, 0xffffffff, Base)

#define IMPLEMENT_TAGOBJEDTSLCT(Obj, Grp, ClassId, Version, TagInitialID, Cat, Edt, ShortDesc, Desc, SelectMask) \
  IMPLEMENT_TAGOBJEDTSLCTDERIVED(Obj, Grp, ClassId, NULL, Version, "Edit", TagInitialID, Cat, Edt, ShortDesc, Desc, SelectMask, TagObjClass)


#define IMPLEMENT_BUILDDATADEFNGLOBAL(Obj)                                            \
static class C##Obj##BuildDataDefnGlobalAttach                                        \
  {                                                                                   \
  public:                                                                             \
    C##Obj##BuildDataDefnGlobalAttach()                                               \
      {                                                                               \
      Obj##Class.SetDataDefnGlobalFn(Obj::BuildDataDefnGlobal, Obj::DataXchgGlobal);  \
      }                                                                               \
  } gs_##Obj##BuildDataDefnGlobalAttach;


//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

//========================================================================
//
//
//
//===========================================================================

#include "MathlibTO.h"

//===========================================================================
//
//
//
//===========================================================================

#endif
