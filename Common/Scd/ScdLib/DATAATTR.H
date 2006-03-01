//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DATAATTR_H
#define  __DATAATTR_H

#ifndef __SCDTEMPL_H__
  #include "scdtempl.h"
#endif
#ifndef __DATACNVS_H__
  #include "datacnvs.h"
#endif
#ifndef __DATATYPE_H__
  #include "datatype.h"
#endif

#ifdef __DATAATTR_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class DllImportExport FmtAttribute
  {
  //protected:
  public:
    static UINT iCmdIndex;
    Strng sNm;
    byte  iWide;
    byte  iPrec;
    char  cTyp;

  public:
    FmtAttribute() {iWide=0; iPrec=2; cTyp='f';};
    FmtAttribute(char * Nm, byte Wide, byte Prec, char Type)
      {sNm=Nm; iWide=Wide; iPrec=Prec; cTyp=Type;};
    FmtAttribute(const FmtAttribute & Other)
      {
      sNm  =Other.sNm  ;
      iWide=Other.iWide;
      iPrec=Other.iPrec;
      cTyp =Other.cTyp ;
      }
    FmtAttribute& operator=(const FmtAttribute & Other)
      {
      sNm  =Other.sNm  ;
      iWide=Other.iWide;
      iPrec=Other.iPrec;
      cTyp =Other.cTyp ;
      return *this;
      }

    void FormatFloat(double dVal, Strng& sValue, Strng_List * NANStrLst = NULL);//LPCTSTR NANName=NULL);
    void FormatFloat(double dVal, char * Str, short Len, Strng_List * NANStrLst = NULL);//LPCTSTR NANName=NULL);
    static int FindFmt(char* pNm);
    flag SetFmt(char* pNm);

    flag ParseStr(char* pFmtStr);
    char * BuildStr(Strng & S);

    static void SetCmdIndex(UINT CmdIndex) { iCmdIndex=CmdIndex; };
    static UINT CmdIndex(UINT CmdIndex) { return iCmdIndex; };

    void AddToMenu(CMenu& Menu, flag WithOverride=false);

    char* Name() { return sNm(); };
    int   NameLength() { return sNm.Length(); };
    byte  Wide() { return iWide; };
    byte  Prec() { return iPrec; };
    char  Type()  { return cTyp; };
    void  SetName(char* Nm) { sNm=Nm; };
    void  SetName(Strng &Nm) { sNm=Nm; };
    void  SetWide(byte Wide) { iWide=Wide; };
    void  SetPrec(byte Prec) { iPrec=Prec; };
    void  SetType(char Type)  { cTyp=Type; };
    void  Set    (char *Nm, byte Wide, byte Prec, char Type)
      { sNm=Nm; iWide=Wide; iPrec=Prec; cTyp=Type; };


  };

//===========================================================================

class DllImportExport FmtAttrArray : public CArray<FmtAttribute, FmtAttribute&>
  {
  public:
  void AddOne(char * Nm, byte Wide, byte Prec, char Type)
    {
    FmtAttribute F(Nm, Wide, Prec, Type);
    Add(F);
    }
  };

extern DllImportExport FmtAttrArray DefinedFmts;

//===========================================================================

class DllImportExport CnvAttribute
  {
  protected:
    static UINT iCmdIndex;
    CCnvIndex iIndex;
    Strng sTxt;

  public:
    CnvAttribute() { iIndex=0; };
    CnvAttribute(CCnvIndex Index, char * Txt) { iIndex=Index; sTxt=Txt; };
    CnvAttribute(const CnvAttribute & Other)
      {
      sTxt  =Other.sTxt  ;
      iIndex=Other.iIndex;
      }
    CnvAttribute& operator=(const CnvAttribute & Other)
      {
      sTxt  =Other.sTxt  ;
      iIndex=Other.iIndex;
      return *this;
      }

    static void SetCmdIndex(UINT CmdIndex) { iCmdIndex=CmdIndex; };
    static UINT CmdIndex(UINT CmdIndex) { return iCmdIndex; };

    void AddToMenu(CMenu& Menu, flag WithOverride=false);
    double Human(double D);
    double Normal(double D);
    double Cnv(double D, char* ReqdCnvTxt);

    inline char*  Text() { return sTxt(); };
    inline int    TextLength() { return sTxt.Length(); };
    inline CCnvIndex Index() { return iIndex; };
    inline void   SetText(char * Txt) { sTxt=Txt; };
    inline void   SetText(Strng &Txt) { sTxt=Txt; };
    inline void   SetIndex(CCnvIndex Index) { iIndex=Index; };
    inline int    SetIndex(char * FamName);
    inline void   Set(CCnvIndex Index, char * Txt) { iIndex=Index; sTxt=Txt; };
    inline void   Set(CCnvIndex Index, Strng &Txt) { iIndex=Index; sTxt=Txt; };
    inline char * FamName() { return Cnvs[(iIndex)]->Fam(); };

  };

//===========================================================================

_FWDDEF(DataAttribute);
_FWDDEF(ObjectAttribute);

class DllImportExport DataAttribute
  {
  public :
    Strng          sName;
    flag           fVis;
    CnvAttribute   Cnv;
    FmtAttribute   Fmt;
    double         dMin;
    double         dMax;

    DataAttribute(char * pFldName);
  };

typedef CSCDList <DataAttribute*, DataAttribute*> DataAttributeList;
typedef CSCDPtrListIter <DataAttributeList, DataAttribute*> DataAttributeIter;

//===========================================================================

class DllImportExport ObjectAttribute
  {
  public :
    Strng          Name;
    DataAttributeList DataAttributes;
    flag           Changed;

    ObjectAttribute() { Changed=0; };
    ObjectAttribute(char * pObjName) { Name=pObjName; Changed=0; };
    ~ObjectAttribute();

    pDataAttribute FieldInList(char * pFldName);
    DataAttribute * SetBase(char * pFldName);
    flag           ValidateCnv(char * pFldName, CCnvIndex Index);
    flag           FieldVisible(char * pFldName, DDEF_Flags dwFlags, flag fDeflt);
    void           SetFieldFmt(char * pFldName, FmtAttribute &Fmt);
    void           SetFieldCnvs(char * pFldName, CnvAttribute &Cnv);
    flag           FieldFmt(char * pFldName, FmtAttribute & Fmt);
    flag           FieldCnvs(char * pFldName, CnvAttribute &Cnv);
    flag           FieldFmtCnvs(char * pFldName, FmtAttribute & Fmt, CnvAttribute &Cnv);
    void           SetFieldMin(char * pFldName, double Min);
    void           SetFieldMax(char * pFldName, double Max);
    flag           FieldRange(char * pFldName, double &Min, double &Max);
    flag           SetFieldVisible(char * pFldName, DDEF_Flags dwFlags, flag fVisible);
    flag           Load();
    flag           Save();
  };

//===========================================================================

class DllImportExport ObjectAttributeList : public CSCDList <ObjectAttribute*, ObjectAttribute*>
  {
  public:
    ~ObjectAttributeList();
    ObjectAttribute *FindObject(char * pObjName);
    flag           Load();
    flag           Save();
  };

typedef CSCDPtrListIter <ObjectAttributeList, ObjectAttribute*> ObjectAttributeIter;

extern DllImportExport ObjectAttributeList ObjAttributes;

//===========================================================================

#undef DllImportExport

//===========================================================================

#endif

