//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SP_DBEXTRA_H
#define  __SP_DBEXTRA_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SP_DB_H
  #include "sp_db.h"
#endif

#if defined(__SP_DBEXTRA_CPP)
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class DllImportExport SpeciePropCfgItem
  {
  public:
    Strng       Name;
    short       iWidth;
    byte        bOptional:1,
                bFound:1,
                bString:1,
                bDisplay:1;

    SpeciePropCfgItem();
    SpeciePropCfgItem(char* pName, short Width, byte String, byte Display=1, byte Optional=0);
    void Set(char* pName, short Width, byte String, byte Display=1, byte Optional=0);
  };

class DllImportExport SpeciePropCfgHelper
  {
  friend class SpecieProp;
  //friend class CSpeciePropDataBase;
  protected:
    int           iPropCnt;
    SpeciePropCfgItem * pCfgItems;
    char **       pPropNames;
    char **       pOptPropNames;
  public:
    SpeciePropCfgHelper();
    ~SpeciePropCfgHelper();
    void          Init(SpeciePropCfgItem * CfgItems, int Count);
    int           PropCount() { return iPropCnt; };
    char **       PropNames() { return pPropNames; };
    char **       OptPropNames() { return pOptPropNames; };
    SpeciePropCfgItem* CfgItem(int PropIndex) { return &pCfgItems[PropIndex]; };
    char*         Name(int PropIndex)     { return pCfgItems[PropIndex].Name(); };
    int           Width(int PropIndex)    { return pCfgItems[PropIndex].iWidth; };
    bool          Optional(int PropIndex) { return pCfgItems[PropIndex].bOptional; };
    bool          Found(int PropIndex)    { return pCfgItems[PropIndex].bFound; };
    bool          String(int PropIndex)   { return pCfgItems[PropIndex].bString; };
    bool          Display(int PropIndex)  { return pCfgItems[PropIndex].bDisplay; };
  };

//---------------------------------------------------------------------------

class DllImportExport SpecieProp
  {
  friend class CSpeciePropDataBase;
  protected:
    unsigned int  bOK:1;
    int           iSId;
    SpeciePropCfgHelper* pPropCfg;
    double*       dProp;
    Strng*        sProp;
  public:
    SpecieProp(SpeciePropCfgHelper* PropCfg, int SId);
    virtual ~SpecieProp();

    int           Load(CStringArray & Values);
    bool          OK() { return bOK; };
    SpeciePropCfgHelper* PropCfg() { return pPropCfg; };
    char*         Name(int PropIndex) { return pPropCfg->Name(PropIndex); };
    double        Prop(int PropIndex) { return dProp[PropIndex]; };
    char*         StrProp(int PropIndex) { return sProp[PropIndex](); };

    char*         Tag() { return SDB[iSId].Tag(); };
    CSpecie &     Specie() { return SDB[iSId]; };
  };

//---------------------------------------------------------------------------

class DllImportExport CSpeciePropDataBase
  {
  friend class OrePropDataBase;
  private:
    SpeciePropCfgHelper PropCfg;
    SpecieProp *  SP[MaxSpecies];
    int           iSpCnt;
  public:
    CSpeciePropDataBase();
    virtual ~CSpeciePropDataBase();

    int           Init(SpeciePropCfgItem * CfgItems, int Count);
    int           Load(char* DataFile, char* TableName);
    SpecieProp *  FindSolid(char* Tag);
    SpecieProp *  Find(int SpId);
    SpecieProp *  operator [](int index) { return index<iSpCnt ? SP[index] : NULL; };
    SpecieProp *  GetProp(int index) { return index<iSpCnt ? SP[index] : NULL; };
    int           GetCount() { return iSpCnt; };
    int           PropCount() { return PropCfg.PropCount(); };
    char*         PropName(int PropIndex) { return PropCfg.Name(PropIndex); };
    SpeciePropCfgHelper* GetPropCfg() { return &PropCfg; };

  };

//===========================================================================

#undef DllImportExport

#endif

