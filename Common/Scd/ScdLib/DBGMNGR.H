//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994,1995
#ifndef  __DBGMNGR_H
#define  __DBGMNGR_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  

#ifdef __DBGMNGR_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define DBGMANAGER  1

// -------------------------------------------------------------------------
//const pchar SectionName = "General"; // the section underwhich all the Key Names are searched for
//const pchar DefaultLine = "Default"; // the default line returned if no items are listed next to the specified Key Names e

_FWDDEF(CSubSexn);
_FWDDEF(CGenSexn);
_FWDDEF(CDbgMngr);

// -------------------------------------------------------------------------

/*#X:The form of the debug file :

[SECTION]
   GROUP = X , ItemName, ItemName , ...

SECTION : This is the name specified by the first string of the constructor,
          this section can contain many groups, and is used to categorise
          the varoius debug in the appropriate sections, for ease of referal 

GROUP : This is the name specified by the second string of the constructor, 
        it provides a reference to a long value, and also to a number of groups
        - although this is optional.

X  : is intepreted as bOn and determines whether the GROUP, as a whole, will be able to 
     supply debug information or not. This, is a 'long' integer and as long as this value is
     NOT '0',then the specified SECTION and any groups can provide debugging information.

ItemName : if this does NOT BEGIN with '0', then the WHOLE NAME specified, will be INCLUDED
           in the list of permissable groups that will permit debugging to occur, otherwise
           it is EXCLUDED from the list.
*/
//Each line in the file, under the GENERAL section, refers to a separate instance of the CDbgMngr and resembles the following :

//---------------------------------------------------------------------------------------
#if DBGMANAGER

/*#c: CSubSexn has been created as a sister class to CGenSexn to create a list 
of sub-sexn names corresponding each general section name*/
class DllImportExport CSubSexn
  {
  public:
    CSubSexn( pchar subSection ); 
    ~CSubSexn(); 
    pCSubSexn pNxt;
    Strng     subStrng;
    long      fValue;
    Strng     fStrng;
    flag Add ( pchar subSection );
  };     

/*#c: CGenSexn has been created to establish a unique list of general section names
 in the debug window - because the CDbgMngr did not provide all the existing options 
 at one time parts of could be deleted from the list before it was made*/
class DllImportExport CGenSexn
  {
  public:
    CGenSexn( pchar generalSection, pchar subSection );
    ~CGenSexn();
    pCGenSexn pNxt;
    pCSubSexn pSubSexns;
    Strng     genStrng;
    static pCGenSexn pFirstGen; //pointer to linked list
    flag Add (pchar generalSection, pchar subSection);
    static pCGenSexn GetByIndex ( int index );
    void WriteToFile ( pCSubSexn currentSubSexn );
    void ReadFromFile ( pCSubSexn currentSubSexn );
  };     
#endif

/*#C:This is used instead of the cumbersome '# define' statements, a file is read to 
determine which areas of a program are to have their debug lines enabled/disabled.*/
class DllImportExport CDbgMngr
  {
  public:
#if DBGMANAGER
    pCDbgMngr pNxt; //pointer to next CDbgMngr in list
    Strng sSection; //section name
    Strng sGroup; //group name
    Strng_List Items; //list of item tags
    long bOn, bOnPgm; //is the group to have its debugging enabled or not
    static pCDbgMngr pFirst; //pointer to linked list
    static long LastFileTime; //time of last modification to DbgMngrFile
    static Strng DbgMngrFilename; // specifies the DbgMngrFile to be used
    static long IndexCnt;
    static flag bStarted;

    CDbgMngr(pchar pSection, pchar pGroup);
    ~CDbgMngr();
    
    static void StartUp();
    static void ShutDown();

    void Load();
    //long Do(pchar pItem = NULL); - currently done by overloaded operator ()
    long operator () (pchar pItem = NULL) { long l=Max(bOn, bOnPgm); return (!bStarted || pItem==NULL) ? l : (l==0 ? 0 : (Items.Length()==0 ? 1 : (long)(Items.Find(pItem)!=NULL))); };
    static void LoadAll();
    long Value() { return bOn; };
    long PgmSet(long l) { long old=bOnPgm; bOnPgm=l; return old; };
    static void ChkFileSize(long MaxFileSize, long PercentRemaining=0);
  
   /*The folowing data members and functions are no longer required because the 
   list-making operation is now accomplished by  have beeen replaced by CGenSexn & CSubSexn
   Strng dbgStrng; // a copy of the current line of items in the dbg file 
   long dbgValue; //a copy of the value of the group in the dbg file 
   long Index;  
   void ListYourself(); 
   static CDbgMngr * CDbgMngr::GetByIndex(int index);*/ 
#else

    CDbgMngr(pchar pSection, pchar pGroup) {};
    ~CDbgMngr() {};
    
    static void StartUp() {};
    static void ShutDown() {};

    void Load() {};
    //long Do(pchar pItem = NULL); - currently done by overloaded operator ()
    long operator () (pchar pItem = NULL) { return 0; };
    static void LoadAll() {};
    long Value() { return 0; };
    long PgmSet(long l) { return 0; };
    static void ChkFileSize(long MaxFileSize, long PercentRemaining=0) {};
#endif
  };

// -------------------------------------------------------------------------

#undef DllImportExport 

#endif
