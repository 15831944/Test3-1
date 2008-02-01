//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __NODEDRAW_H
#define  __NODEDRAW_H

#include "..\..\..\smdk\include\md_share3.h"

#ifdef __NODEDRAW_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//const long DD_CodeLo    = 0x80000000;
//const long DD_Name      = 0x80000000; // Most Negetive
//const long DD_In        = 0x80000001; 
//const long DD_Out       = 0x80000002; 
//const long DD_Poly      = 0x80000003;
//const long DD_Arc       = 0x80000004;
//const long DD_Arc3      = 0x80000005;
//const long DD_Ellipse   = 0x80000006;
//const long DD_End       = 0x80000007;
//const long DD_Dynm      = 0x80000008;
//const long DD_Scale     = 0x80000009;
//
//const long DD_EndList   = 0x8000000e;
//const long DD_CodeHi    = 0x8000000f;

const long DD_CodeLo    = 0x80000000;
const long DD_Name      = 0x80000000; // Most Negetive
const long DD_Poly      = MDrw_Poly;
const long DD_Arc       = MDrw_Arc ;
const long DD_Arc3      = MDrw_Arc3;
const long DD_TagPos    = MDrw_TagPos;
const long DD_ValuePos  = MDrw_ValuePos;
const long DD_End       = MDrw_End ;
const long DD_Ellipse   = 0x80000007;
const long DD_In        = 0x80000008; 
const long DD_Out       = 0x80000009; 
const long DD_Dynm      = 0x8000000a;
const long DD_Scale     = 0x8000000b;
                               
const long DD_EndList   = 0x8000000e;
const long DD_CodeHi    = 0x8000000f;

_FWDDEF(NodeGrfInfo)
class DllImportExport NodeGrfInfo
  {
  public:
    NodeGrfInfo();
    ~NodeGrfInfo();

    void            SetScale(double Scl_) { Scl=Scl_; };
    long            GetLVal(flag Start=False);
    double          GetDVal(flag Start=False);
    double          GetLength();
    flag            GetPair(double &I, double &J);
    flag            GetCoOrd(double &X, double &Y);
    char          * GetPChar(flag Start=False);
    char          * GetAChar(flag Start=False);
    void            PutValue(double l);
    void            PutPChar(char * p);

    static void     ExportToFile(NodeGrfInfo* pNGI, char* TextFilename);
    static void     ExportToFile10(NodeGrfInfo* pNGI, char* Path);

  public:
    long            Len;
    double        * Buff;
    long            Pos;
    double          Scl;
   
  };

#undef DllImportExport 

#endif
