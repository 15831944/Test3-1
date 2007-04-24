//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#pragma once

#ifndef  __MD_SPDATA_H
#define  __MD_SPDATA_H

#include "md_defn.h"
#include "md_share1.h"
#include "md_share2.h"
#include "md_share3.h"
#include "md_vector.h"

#ifdef __MD_SPDATA_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


// ======================================================================
//
//
//
// ======================================================================

class DllImportExport MBaseDataCommon 
  {
  public:
    MBaseDataCommon (TaggedObject * pTagObject = NULL) 
      {
      m_pTO  = pTagObject;
      m_pDDB = NULL;
      m_pDCB = NULL;
      m_pVDB = NULL;
      };

    void Init(TaggedObject * pTagObject)
      {
      m_pTO  = pTagObject;
      }

  public:
    //MUnitDefBase     *m_pUnitDef;
    //MBaseMethodImpl  *m_pImpl;
    TaggedObject     *m_pTO;
      //MdlNode          *m_pNd;
    DataDefnBlk      *m_pDDB;
    DataChangeBlk    *m_pDCB;
    ValidateDataBlk  *m_pVDB;

    //MUtilArray        m_Utils;
    //MInOutDefStruct  *m_pIODefs;
    //long              m_nJoins;
  };


//---------------------------------------------------------------------------

class DllImportExport MBaseDataCommonRef
  {
  public:
    MBaseDataCommonRef(MBaseDataCommon *pCom) :
        //m_pImpl(pCom->m_pImpl),
        m_pCommon(pCom)
          //,
        //m_pTO(pCom->m_pTO)
        //,
        //m_pDDB(pCom->m_pDDB),
        //m_pDCB(pCom->m_pDCB),
        //m_pVDB(pCom->m_pVDB)
        {
        }

  public:
      MBaseDataCommon * m_pCommon;
      //MBaseDataImpl   * &m_pImpl;
      //TaggedObject    * &m_pTO;
      //DataDefnBlk       * &m_pDDB;
      //DataChangeBlk     * &m_pDCB;
      //ValidateDataBlk   * &m_pVDB;
  };


// remote forward's
class DataDefnBlk;
class DataChangeBlk;
class ValidateDataBlk;
class TagObjClass;

//===========================================================================
/* Class MDataDefn: Helper class used for Data Definition of fields or tags in
a model. This is used by the BuildDataFields method which is used to define the
arrangement of the Access properties window.*/

enum MDDPages {MDD_NoPage, MDD_RqdPage, MDD_OptPage};

class DllImportExport MDataDefn : public MBaseDataCommonRef
  {
  public:
    MDataDefn(MBaseDataCommon *pCom) : MBaseDataCommonRef(pCom) { };

  public:
    void    Double  (LPCSTR Tag, LPCSTR Sym, double* Data, MD_Flags Flags, MCnv & Cnv = MC_);              //add a double (direct memory reference)
    void    Float   (LPCSTR Tag, LPCSTR Sym, float*  Data, MD_Flags Flags, MCnv & Cnv = MC_);              //add a float (direct memory reference)
    void    Long    (LPCSTR Tag, LPCSTR Sym, long*   Data, MD_Flags Flags, MDDValueLst * Values=NULL);     //add a long (direct memory reference)
    void    Short   (LPCSTR Tag, LPCSTR Sym, short*  Data, MD_Flags Flags, MDDValueLst * Values=NULL);     //add a short (direct memory reference)
    void    Bool    (LPCSTR Tag, LPCSTR Sym, bool*   Data, MD_Flags Flags, MDDValueLst * Values=NULL);     //add a bool (direct memory reference)
    void    CheckBox(LPCSTR Tag, LPCSTR Sym, bool*   Data, MD_Flags Flags);                                //add a bool as a checkbox (direct memory reference)
    //void    String  (LPCSTR Tag, LPCSTR Sym, ????? * Data, MD_Flags Flags);                              //specifically omitted, use DataHandle option below (What 'standard' string class to use is difficult to agree on)

    void    Double  (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MCnv & Cnv = MC_);           //add a double (use DataHandle ID to set/retrieve tags)
    void    Float   (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MCnv & Cnv = MC_);           //add a float (use DataHandle ID to set/retrieve tags)
    void    Long    (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MDDValueLst * Values=NULL);  //add a long (use DataHandle ID to set/retrieve tags)
    void    Short   (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MDDValueLst * Values=NULL);  //add a short (use DataHandle ID to set/retrieve tags)
    void    Bool    (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MDDValueLst * Values=NULL);  //add a bool (use DataHandle ID to set/retrieve tags)
    void    CheckBox(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags);                             //add a bool as a checkbox (use DataHandle ID to set/retrieve tags)
    void    String  (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags);                             //add a String (use DataHandle ID to set/retrieve tags)
    void    Button  (LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags = MF_PARAMETER);              //add a Button (use DataHandle ID for press button action)

    //add a text line
    void    Text(LPCSTR pName, MD_Flags Flags=0);
    //start a new tab page with specified name. If optional is true a new page is started based on the length of the current page thus far.
    void    Page(LPCSTR pName, bool Optional=false, MD_Flags Flags=0);
    //set the visibilty of tags on or off.
    void    Show(bool ViewVisible=true, bool FileVisible=true, bool SnapVisible=true);

    //begin structure for a group of tags using pName as part of tag for next 'dot' level.
    void    StructBegin(LPCSTR pName, MD_Flags Flags=0);
    //end structure marker
    void    StructEnd();
    //begin object (ie structure) for a group of tags using pName as part of tag for next 'dot' level. pClassName is name of table in database for this structure.
    void    ObjectBegin(LPCSTR pClassName, LPCSTR pName, MD_Flags Flags=0);
    //end object marker
    void    ObjectEnd();
    //Include defined Object
    void    Object(LPTAGGEDOBJECT Object, MDDPages Pg=MDD_OptPage, MD_Flags Flags=0);
    //begin array for a group of tags using pName as part of tag for next 'dot' level.
    void    ArrayBegin(LPCSTR pClassName, LPCSTR pName, long ElementCount, long LengthHandle=0, MD_Flags Flags=0);
    //element markers
    void    ArrayElementStart(LPCSTR sIndex, MD_Flags Flags=0);
    void    ArrayElementStart(long   iIndex, MD_Flags Flags=0);
    void    ArrayElementEnd(MD_Flags Flags=0);
    //end structure marker
    void    ArrayEnd(MD_Flags Flags=0);

    //begin matrix for a group of tags using pName as part of tag for next 'dot' level. - Includes gridding of the Display
    void    MatrixBegin(LPCSTR pClassName, LPCSTR pName, long ColCount, long RowCount, long ColWidth=10, long Gap=0, MD_Flags Flags=0);
    void    MatrixBegin(LPCSTR pClassName, LPCSTR pName, long ColCount, long RowCount, long ColWidth, long Gap, LPCSTR ColHdr, LPCSTR RowHdr, MD_Flags Flags=0);
    //element markers
    void    MatrixElementStart(long iColIndex, long iRowIndex, MD_Flags Flags=0);
    //void    MatrixElementStart(LPCSTR sColIndex, LPCSTR sRowIndex, MD_Flags Flags=0);
    void    MatrixElementEnd(MD_Flags Flags=0);
    //end structure marker
    void    MatrixEnd(MD_Flags Flags=0);

    // marks the start of the grid
    void    GridBegin(LPCSTR GridNameText, long MaxCols, long MaxRows, long ColWidth=10, long Gap=0);
    // column header  Width in chars, post gap chars, Justification <0:Left,0:Centre,>0:Right
    void    GridColumnHeader(LPCSTR ColHdrText, long ColWidth=10, long Gap=0, long Justification=1);
    // adds a row header at the bigging of the line 
    void    GridRowHeader(LPCSTR RowHdrText);
    //forces the next item to start a new row
    void    GridRowStart();
    // marks the end of the grid
    void    GridEnd();

    bool    getForNDB();           //retrieve flag
    bool    getForFiling();        //retrieve flag
    bool    getForView();          //retrieve flag
    bool    getForSnapShot();      //retrieve flag
    bool    getForScenario();      //retrieve flag
    bool    getForFileSnpScn();    //retrieve flag

    __declspec(property(get=getForNDB))                 bool    ForNDB;
    __declspec(property(get=getForFiling))              bool    ForFiling;
    __declspec(property(get=getForView))                bool    ForView;
    __declspec(property(get=getForSnapShot))            bool    ForSnapShot;
    __declspec(property(get=getForScenario))            bool    ForScenario;
    __declspec(property(get=getForFileSnpScn))          bool    ForFileSnpScn;

  };

/* Class MDataChange: Helper class used for setting and/or retrieving tags as
defined in BuildDataFields method for those tags using the DataHandle option.*/
class DllImportExport MDataChange : public MBaseDataCommonRef
  {
  public:
    MDataChange(MBaseDataCommon *pCom) : MBaseDataCommonRef(pCom)/*, m_pDCB(pCom->m_pDCB)*/ {};

    long    getHandle();       //return data handle
    bool    getHasReqdValue(); //return true when data is being set

    bool    getForNDB();           //retrieve flag
    bool    getForFiling();        //retrieve flag
    bool    getForView();          //retrieve flag
    bool    getForSnapShot();      //retrieve flag
    bool    getForScenario();      //retrieve flag
    bool    getForFileSnpScn();    //retrieve flag

    double  getDouble();   //retrieve value
    float   getFloat();    //retrieve value
    long    getLong();     //retrieve value
    short   getShort();    //retrieve value
    bool    getBool();     //retrieve value
    LPCTSTR getString();   //retrieve value

    void    putDouble(double V);  //set value
    void    putFloat(float V);    //set value
    void    putLong(long V);      //set value
    void    putShort(short V);    //set value
    void    putBool(bool V);      //set value
    void    putString(LPCTSTR V); //set value

  public:
    __declspec(property(get=getHandle))                 long    Handle;
    __declspec(property(get=getHasReqdValue))           bool    HasReqdValue;

    __declspec(property(get=getForNDB))                 bool    ForNDB;
    __declspec(property(get=getForFiling))              bool    ForFiling;
    __declspec(property(get=getForView))                bool    ForView;
    __declspec(property(get=getForSnapShot))            bool    ForSnapShot;
    __declspec(property(get=getForScenario))            bool    ForScenario;
    __declspec(property(get=getForFileSnpScn))          bool    ForFileSnpScn;

    __declspec(property(get=getDouble,put=putDouble))   double  Double;
    __declspec(property(get=getFloat, put=putFloat))    float   Float;
    __declspec(property(get=getLong,  put=putLong))     long    Long;
    __declspec(property(get=getShort, put=putShort))    short   Short;
    __declspec(property(get=getBool,  put=putBool))     bool    Bool;
    __declspec(property(get=getString,put=putString))   LPCTSTR String;
  
  //protected:
  //  DataChangeBlk     * &m_pDCB;
  };

/* Class MDataChange: Helper class used for setting and/or retrieving tags as
defined in BuildDataFields method for those tags using the DataHandle option.*/
class DllImportExport MDataValidate : public MBaseDataCommonRef
  {
  public:
    MDataValidate(MBaseDataCommon *pCom) : MBaseDataCommonRef(pCom)/*, m_pVDB(pCom->m_pVDB)*/ {};

  //  long    getHandle();       //return data handle
  //  bool    getHasReqdValue(); //return true when data is being set

  //  double  getDouble();   //retrieve value
  //  float   getFloat();    //retrieve value
  //  long    getLong();     //retrieve value
  //  short   getShort();    //retrieve value
  //  bool    getBool();     //retrieve value
  //  LPCTSTR getString();   //retrieve value

  //  void    putDouble(double V);  //set value
  //  void    putFloat(float V);    //set value
  //  void    putLong(long V);      //set value
  //  void    putShort(short V);    //set value
  //  void    putBool(bool V);      //set value
  //  void    putString(LPCTSTR V); //set value

  //public:
  //  __declspec(property(get=getHandle))                 long    Handle;
  //  __declspec(property(get=getHasReqdValue))           bool    HasReqdValue;
  //  __declspec(property(get=getDouble,put=putDouble))   double  Double;
  //  __declspec(property(get=getFloat, put=putFloat))    float   Float;
  //  __declspec(property(get=getLong,  put=putLong))     long    Long;
  //  __declspec(property(get=getShort, put=putShort))    short   Short;
  //  __declspec(property(get=getBool,  put=putBool))     bool    Bool;
  //  __declspec(property(get=getString,put=putString))   LPCTSTR String;

    double  ValidateRange(LPCTSTR What, double MinV=dNAN, double V=dNAN, double MaxV=dNAN, bool *pOK=NULL);

  //protected:
  //  ValidateDataBlk  * &m_pVDB;
  };


#undef DllImportExport

#endif
