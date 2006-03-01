//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __COPYBLK_H
#define __COPYBLK_H

#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif

#ifdef __COPYBLK_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

enum CopyBlkTypes { CB_SelectedTag, CB_Block, CB_TagList };

class DllImportExport CCopyBlock
  {
  public:
    CCopyBlock(char* ModelClass, CExecObj* EO);
    void SetSrcTag(char* Src);
    void SetSrcBlock(char* Src, BOOL FromDatalib=TRUE);
    void SetDstList();
    void SetDstList(CSVector& List);
    void SetDstBlock(char* Dst, BOOL FromDatalib=TRUE);
    int WriteTagValues(CSVector& Tags);
    static BOOL CheckCopyBlockVer(CProfINIFile& PF, BOOL TestReadOnly=FALSE);
    static void BuildBlockList(char* ModelClass, CSVector& List, CWordArray& FlagList, int& DefaultIndex);
    static void BuildBlockTagsList(char* ModelClass, char* BlockName, BOOL FromDatalib, CSVector& List);
    static void RemoveBlock(char* ModelClass, char* BlockName);
    static void MoveBlockLocation(char* ModelClass, char* BlockName, BOOL ToDatalib);
    int CopyTagBlocks();

    static const char* CopyBlkFileName;
    static const char* TempBlockName;
  protected:
    Strng sModelClass;   //model class/type for transfer
    CExecObj* pEO;       //Executive object to use for xread/xwrite
    CopyBlkTypes eSrc;   //source type
    CopyBlkTypes eDst;   //destination type
    Strng sSrc;          //source tag or default block name
    Strng sDst;          //destination tag or default block name
    BOOL bSrcDatalib;    //use ini file from datalib directory else from project directory
    BOOL bDstDatalib;    //use ini file from datalib directory else from project directory
    CSVector TagList;    //destination tag list
  };

//===========================================================================

#undef DllImportExport 

#endif
