//============ Copyright Kenwalt (Pty) Ltd (1996, 1997, ...) ================
// $Nokeywords: $
//===========================================================================
#ifndef __KWDB_H
#define __KWDB_H

#include <afxtempl.h> // "xafxtempl.h"

#define DllExport __declspec( dllexport )
#define DllImport __declspec( dllimport )

#ifdef __KWDB_CPP
  #define DllImportExport DllExport
#elif !defined(KWDB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#pragma warning(disable:4995)

#define ALLOWTAGINDEX         1
#define USEMAPPEDRECORDSETS   01

///////////////////////////////////////////////////////////////////////////////////////

#if 0
    AttachTableAndImportAttributes();
    DeleteField()
    DeleteTable();
    DeleteObject();
    RenameObject()
#endif

class DllImportExport KWFieldDef : public CObject
{
public:
    CString strClassName;
    CString strFieldName;
    CString strEngUnits;
    ADOX::DataTypeEnum nType;
    short   nTypeSize;
    short   nOrdinalPosition;
    BOOL    bRequired;
    BOOL    bAllowZeroLength;
    CString strDefaultValue;
    CString strDispUnits;
    CString strDescription;
    long    lHelpID;

    BOOL    bInMap;

    KWFieldDef() { bInMap=FALSE; }
    KWFieldDef(LPCTSTR classname, LPCTSTR fieldname, LPCTSTR engunits,
      ADOX::DataTypeEnum type, short size =0, short pos =0,
               BOOL Required=TRUE, BOOL AllowZeroLength =FALSE,
               LPCTSTR defvalue =_T(""),
               LPCTSTR dispunits =_T(""), LPCTSTR desc =_T(""), long helpid =0) :
        strClassName(classname), strFieldName(fieldname), strEngUnits(engunits),
        nType(type), nTypeSize(size), nOrdinalPosition(pos),
        bRequired(Required), bAllowZeroLength(AllowZeroLength),
        strDefaultValue(defvalue),
        lHelpID(helpid), strDispUnits(dispunits), strDescription(desc) { bInMap=FALSE; }

protected:

};

///////////////////////////////////////////////////////////////////////////////////////

const DWORD CO_CheckForExistence = 0x00000001;
const DWORD CO_SetDefaultValues  = 0x00000002;

class DllImportExport KWDatabase 
{
protected:
  ADOX::_CatalogPtr m_pCat;
  ADODB::_ConnectionPtr m_pCnn;

public:
    KWDatabase();
    ~KWDatabase();

    virtual void DoKWDisplayException(LPCTSTR );

    BOOL OpenDB(long DBFmt, 
                LPCTSTR lpszName, 
                BOOL    bExclusive  = FALSE, 
                BOOL    bReadOnly   = FALSE/*, 
                LPCTSTR lpszConnect = _T("") */);

    BOOL CreateDB(long DBFmt, 
                  LPCTSTR lpszName, 
                  int     dwOptions  = 0 ); 

    void CloseDB(void);
    BOOL CompactDB(long DBFmt, 
                   LPCTSTR DBNameOld, 
                   LPCTSTR DBNameNew);

    BOOL VerifyDB(void); 

    BOOL IsExistentClass(LPCTSTR strClassName);
    BOOL IsExistentField(LPCTSTR strClassName, LPCTSTR strFieldName);
    BOOL IsExistentIndex(LPCTSTR strClassName, LPCTSTR strIndexName);

    BOOL CreateClass(LPCTSTR strNewClassName, 
                     LPCTSTR strDerivedClass = NULL);
    BOOL CreateFullTagIndex(LPCTSTR strClassName);
    BOOL CheckFullTagIndex(LPCTSTR strClassName);

    long GetRevisionNo();
    long SetRevisionNo(long RevNo);
    long FindRevisionNo(LPCTSTR strClassName, DWORD Options);
    void RemoveRevisions(int From, int To);

    ////////////////

    // Create an object instance using default field values for all fields.
    // strParentName may be NULL, in which case the Object will belong
    // to "PROJ". 
    BOOL CreateObject
    (
       LPCTSTR strObjName, 
       LPCTSTR strClassName,
       LPCTSTR strClassVersion,
       LPCTSTR strPrimaryCfg,
       LPCTSTR strParentName
    );


    // Create an object instance using a variable number of field name
    // and value pairs.
    // NOTE: (See also SizeofActualDBType())
    //
    // ADODB::adBoolean : ADODB::adUnsignedTinyInt :             1 == BYTE
    // ADODB::adSmallInt :                                       2 == short
    // ADODB::adInteger : ADODB::adSingle :                      4 == long, float
    // ADODB::adDouble : ADODB::adDate : ADODB::adCurrency :     8 == double
    //
    BOOL CreateObjectEx(LPCTSTR strObjName,
                        LPCTSTR strClassName,
                        LPCTSTR strClassVersion,
                        LPCTSTR strPrimaryCfg,
                        LPCTSTR strOwnerName,
                        ... );

    // Create an object instance using 2 arrays of flddefptrs and values

    BOOL CreateObjectExA(BOOL IsRootObject,
                         LPCTSTR strObjName,
                         LPCTSTR strClassName,
                         LPCTSTR strClassVersion,
                         LPCTSTR strPrimaryCfg,
                         LPCTSTR strOwnerName,
                         DWORD Options,
                         KWFieldDef ** fieldDefs, 
                         COleVariant * fieldData, 
                         int nDefs);

    ////////////////

    BOOL CreateField(LPCTSTR strClassName,
                     LPCTSTR strFieldName,
                     LPCTSTR strEngUnits,
                     ADOX::DataTypeEnum   nType,
                     short   nTypeSize        = 0,
                     BOOL    bRequired        = FALSE,
                     BOOL    bAllowZeroLength = FALSE,
                     LPCTSTR strDefaultValue  = _T(""),
                     LPCTSTR strDispUnits     = _T(""),
                     LPCTSTR strDescription   = _T(""),
                     long    nHelpID          = 0L,
                     short   nOrdinalPosition = 0 );

    int CreateFields(LPCTSTR strClassName, KWFieldDef ** fieldDefs, int nDefs);

    BOOL UpdateField(LPCTSTR strClassName,
                     LPCTSTR strFieldName,
                     LPCTSTR strDefaultValue  = _T(""),
                     LPCTSTR strDispUnits     = _T(""),
                     LPCTSTR strDescription   = _T(""),
                     long    nHelpID          = 0L);


    // MW ??? : TODO
    BOOL DeleteField(LPCTSTR strClassName,
                     LPCTSTR strFieldName);

    // MW ??? : TODO
    BOOL DeleteClass(LPCTSTR strClassName);

    BOOL DeleteAllClassObjects(LPCTSTR strClassName);
    
    BOOL GetKWFieldInfo(LPCTSTR      strClassName,
                        LPCTSTR      strFieldName,
                        KWFieldDef*& fieldInfo);

    BOOL GetFieldType(LPCTSTR strClassName,
                      LPCTSTR strFieldName,
                      short&  nType);

    BOOL GetObjectsClass(LPCTSTR strObjName,
                         LPTSTR  strClassName );


    BOOL GetAllFieldsInClass(LPCTSTR strClassName,
                             KWFieldDef& fieldInfo );

/////////////////////////
// Get individual records

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       short&   sValue );

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       long&    lValue );

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       float&   fValue );

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       double&  dValue );

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       LPTSTR   strValue );

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       CString& strValue );

    BOOL GetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       COleVariant& var );

/////////////////////////
// Set individual records

    BOOL SetFieldValue(LPCTSTR strObjName,
                       LPCTSTR strFieldName,
                       const COleVariant& var);

    BOOL SetFieldValue(LPCTSTR strObjName,
                       LPCTSTR strFieldName,
                       LPCTSTR strValue);
                                
    BOOL SetFieldValue(LPCTSTR  strObjName,
                       LPCTSTR  strFieldName,
                       CString& strValue);

    BOOL SetFieldValue(LPCTSTR strObjName,
                       LPCTSTR strFieldName,
                       double  dValue);

    BOOL SetFieldValue(LPCTSTR strObjName,
                       LPCTSTR strFieldName,
                       short   sValue);

    BOOL SetFieldValue(LPCTSTR strObjName,
                       LPCTSTR strFieldName,
                       long    iValue);

    BOOL SetFieldNull(LPCTSTR strObjName,
                      LPCTSTR strFieldName);

    BOOL SetFieldValues(LPCTSTR strObjName, ...);

    int SetFieldValuesA(LPCTSTR strObjName, 
                        KWFieldDef ** fieldDefs, 
                        COleVariant * fieldData, 
                        int nDefs);

/////////////////////

    BOOL GetUnits(LPCTSTR  strEngUnit,
                  CString& strUnits);

    BOOL GetFieldEngUnit(LPCTSTR  strClassName,
                         LPCTSTR  strFieldName,
                         CString& strFieldEngUnits);

    BOOL BuildKWFieldDefFromPropertiesRecord( ADODB::_RecordsetPtr pRS, KWFieldDef * fd );
    void BuildFieldDefMap( void);

    // These inline functions simply return table and column names
    inline LPCTSTR GetPropertiesTableName(void)          { return _T("$Properties"); }
    inline LPCTSTR GetPropertiesClassFieldName(void)     { return _T("Class"); }
    inline LPCTSTR GetPropertiesPropertyFieldName(void)  { return _T("Property"); }
    inline LPCTSTR GetPropertiesTypeFieldName(void)      { return _T("Type"); }
    inline LPCTSTR GetPropertiesSizeFieldName(void)      { return _T("Size"); }
    inline LPCTSTR GetPropertiesEngUnitFieldName(void)   { return _T("EngUnits"); }
    inline LPCTSTR GetPropertiesValueFieldName(void)     { return _T("DefaultValue"); }
    inline LPCTSTR GetPropertiesDispUnitsFieldName(void) { return _T("DispUnits"); }
    inline LPCTSTR GetPropertiesDescFieldName(void)      { return _T("Description"); }
    inline LPCTSTR GetPropertiesHelpIDFieldName(void)    { return _T("HelpID"); }

//    inline LPCTSTR GetObjectsTableName(void)          { return _T("$Objects"); }
    inline LPCTSTR GetObjectsTableName(void)          { return m_strObjects; }
    inline LPCTSTR GetObjectsOwnerFieldName(void)     { return _T("Owner"); }
    inline LPCTSTR GetObjectsTagFieldName(void)       { return _T("Tag"); }
    inline LPCTSTR GetObjectsClassFieldName(void)     { return _T("Class"); }
    inline LPCTSTR GetObjectsClassVerFieldName(void)  { return _T("ClassVersion"); }
    inline LPCTSTR GetObjectsPrimaryCfgName(void)     { return _T("PrimaryCfg"); }

    inline LPCTSTR GetClassesTableName(void)       { return _T("$Classes"); }
    inline LPCTSTR GetClassesClassName(void)       { return _T("Class"); }
    inline LPCTSTR GetClassesDerivedFieldName(void){ return _T("DerivedFrom"); }
    inline LPCTSTR GetClassesBaseFieldName(void)   { return _T("BaseClass"); }
    inline LPCTSTR GetClassesLocFieldName(void)    { return _T("MethodLocation"); }

//    inline LPCTSTR GetConnectTableName(void)       { return _T("$Connections"); };//"Connectivity"); }
    inline LPCTSTR GetConnectTableName(void)       { return m_strConnections; };//"Connectivity"); }
    inline LPCTSTR GetConnectTagFieldName(void)    { return m_bFullTagWithOwner ? _T("FullTag") : _T("Tag"); }
    inline LPCTSTR GetConnectSrcFieldName(void)    { return _T("Src"); }
    inline LPCTSTR GetConnectDstFieldName(void)    { return _T("Dst"); }
    inline LPCTSTR GetConnectSrcIOFieldName(void)  { return _T("SrcIONo"); }
    inline LPCTSTR GetConnectDstIOFieldName(void)  { return _T("DstIONo"); }
    inline LPCTSTR GetConnectDescFieldName(void)   { return _T("Description"); }

    inline LPCTSTR GetUnitsTableName(void)         { return _T("$UnitConversions"); }
    inline LPCTSTR GetUnitsFamilyFieldName(void)   { return _T("Family"); }
    inline LPCTSTR GetUnitsUnitFieldName(void)     { return _T("Unit"); }
    inline LPCTSTR GetUnitsScaleFieldName(void)    { return _T("Scale"); }
    inline LPCTSTR GetUnitsOffsetFieldName(void)   { return _T("Offset"); }
    inline LPCTSTR GetUnitsSIFlagFieldName(void)   { return _T("SI"); }
    inline LPCTSTR GetUnitsDescFieldName(void)     { return _T("Description"); }

    inline LPCTSTR GetFullTagFieldName(void)       { return m_bFullTagWithOwner ? _T("FullTag") : _T("Tag"); }
    inline LPCTSTR GetTagFieldName(void)           { return _T("Tag"); }
    inline LPCTSTR GetRevisionNoFieldName(void)    { return _T("RevNo"); }
    inline LPCTSTR GetSequenceNoFieldName(void)    { return _T("SeqNo"); }

    inline ADOX::_CatalogPtr GetCatalogPtr()        { return m_pCat; };
    inline ADODB::_ConnectionPtr GetConnectionPtr() { return m_pCnn; };

    // inline LPCTSTR GetBaseFullTagFieldName(void)       { return _T("FullTag"); }
    // inline LPCTSTR GetBaseTagFieldName(void)           { return _T("Tag"); }
    // inline LPCTSTR GetBaseParentFieldName(void)        { return _T("Parent"); }
    // inline LPCTSTR GetBaseClassFieldName(void)         { return _T("Class"); }

    inline short  SizeofDBType(ADOX::DataTypeEnum iDBType);
    inline short  IsValidDBType(ADOX::DataTypeEnum iDBType);

    inline BOOL ErrorOccured() { return m_bErrorOccured; };
    inline void ClearErrorOccured() { m_bErrorOccured = FALSE; };

protected:
    CString        m_strClassDBName;
    CString        m_strProjDBName;
    
    BOOL           m_bErrorOccured;
    
    BOOL           m_bWithSCDOptions;         // CNM
    BOOL           m_bFullTagWithOwner;       // CNM
    BOOL           m_bWithRevisionNo;         // CNM
    BOOL           m_bObjsWithSeqNo;          // CNM

    BOOL           m_bHoldLclTransactions;    // CNM
    BOOL           m_bGlblTransactionBusy;    // CNM


    CString        m_strObjects;              // CNM
    CString        m_strConnections;          // CNM

    long           m_lRevisionNo;             // CNM
    long           m_lSequenceNo;             // CNM

    CString        m_strDefaultOwner;         // CNM

    // Hash table of all field definitions; built when project opened.
    // CMap< CString, LPCTSTR, KWFieldDef*, KWFieldDef*& > m_mapFieldDefs;  <- didn't work!
    typedef CTypedPtrMap<CMapStringToOb,CString,KWFieldDef*> CMapStringToKWFieldDef;
    CMapStringToKWFieldDef m_mapFieldDefs;

#if UseMapRecSet
    class CMappedRecordset
      {
      public:
        ADODB::_RecordsetPtr m_pRS;
        KWDatabase * m_pDB;

        CMappedRecordset(KWDatabase * pDB) 
          { 
          m_pDB=pDB;
          m_pRS=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset)); 
          };
        ~CMappedRecordset()
          { 
          try
            {
            if (m_pRS) 
              m_pRS->Close(); 
            }
          catch(_com_error &e)
            {
            m_pDB->KWDisplayException(e, "Close Recordset");
            }
          m_pRS.Release(); 
          };
        ADODB::_RecordsetPtr operator->() { return m_pRS; }; 
      };
    typedef CMap<CString, LPCTSTR,CMappedRecordset*, CMappedRecordset*> CMapStringToRecordset;
    CMapStringToRecordset m_mapTblRecsets;
    CMappedRecordset * GetTblRecordset(LPCTSTR strTable);
#else
    //class CMappedRecordset : public ADODB::_RecordsetPtr {}; 
    typedef CMap<CString, LPCTSTR,ADODB::_RecordsetPtr, ADODB::_RecordsetPtr> CMapStringToRecordset;
    CMapStringToRecordset m_mapTblRecsets;
    ADODB::_RecordsetPtr GetTblRecordset(LPCTSTR strTable);
#endif

    void CloseTblRecordset(LPCTSTR strTable);
    void CloseAllTblRecordsets();

    void ResetSequenceNo() { m_lSequenceNo = 0; };
    long SequenceNo() { return m_lSequenceNo; };

private:
    void CreateClassesTable();
    void CreatePropertiesTable();
    void CreateObjectsTable();
    void CreateConnectivityTable();
    void SetFieldDefaultValueInAllRecords(KWFieldDef * fd);

    BOOL CreateField(KWFieldDef * fieldDef);
    void RemoveAllFieldDefs(void);

    BOOL LclTransactionRqd() { return !m_bHoldLclTransactions && !m_bGlblTransactionBusy; };

    void ConvertToType(LPCTSTR strValue, ADOX::DataTypeEnum nType, COleVariant& var);

    const enum { NUM_FIELDS_IN_MODELDEFS = 9 };

    CString MakeFullTag(LPCTSTR lpszOwner, LPCTSTR lpszTag);
    LPCTSTR OwnerName(LPCTSTR lpszOwner);

public:
    void          KWDisplayException(_com_error & e, LPCTSTR strMsg);
    CString       TagFilterString(long RevNo, LPCTSTR Tag, LPCTSTR Op);
    CString       TagFilterString(LPCTSTR Tag, LPCTSTR Op);
    _variant_t    MyKeyArray(_variant_t V1);
    _variant_t    MyKeyArray(_variant_t V1, _variant_t V2);

};

///////////////////////////////////////////////////////////////////////////////////////

extern DllImportExport KWDatabase* pTheDB;

#undef DllImportExport

#endif