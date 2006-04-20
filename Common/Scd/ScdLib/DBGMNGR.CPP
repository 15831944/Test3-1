//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
#include "stdafx.h"

#define __DBGMNGR_CPP

#include <sys\stat.h>             
//#include <afxmt.h>           
#include "dbgmngr.h"
#include "gpfuncs.h"
#include "crtdbg.h"
#include "errorlog.h"
#include "scdver.h"

#if DBGMANAGER

//#pragma warning (disable : 4073)   // initializers put in library initialization area
//#pragma init_seg(lib)              // Force Early Construction and Late Destruction
//
//CCriticalSection DbgMngrLock;
LONG DbgMngrLock=-1;

//#pragma init_seg(user)              // Restore Normal Construction and Destruction

// =========================================================================
// initialize static data members in cpp for only ONE copy ( if in header MULTIPLE copies )
pCDbgMngr CDbgMngr::pFirst = NULL;
pCGenSexn CGenSexn::pFirstGen = NULL;

long CDbgMngr::LastFileTime = 0;
long CDbgMngr::IndexCnt = 0;
flag CDbgMngr::bStarted = 0;

Strng CDbgMngr::DbgMngrFilename(" ");//("c:\\syscad71\\SCD_Dmf.Ini"); // specifies the DbgMngrFile to be used
//Strng CDbgMngr::DbgMngrFilename; // specifies the DbgMngrFile to be used

//--------------------------------------------------------------------------


CSubSexn::CSubSexn ( pchar subSection)
  { 
  pNxt         = NULL;
  subStrng     = subSection;
  fValue       = 0;
  fStrng       = "";
  };

//--------------------------------------------------------------------------


CSubSexn::~CSubSexn ()
  { 
  delete pNxt;
  };

//--------------------------------------------------------------------------

/*#F:If the sub section is already present in the list then nothing is added, otherwise
the sub section name is added to the list, which corresponds to the respective general section name.*/

flag CSubSexn::Add (pchar subSection )
 {
 pCSubSexn looking = this; // pFirstSub;
 flag flag = 1;
 pCSubSexn last = NULL;
 while ( looking) //->pNxt ) //!= NULL )
   {
   if ( looking->subStrng.XStrICmp ( subSection ) == 0 ) // if sub section exists EXIT
     {                                  // case insensitive comparison like file
     flag = 0;
     break;
     };
   last = looking;
   looking = looking->pNxt;
   }
 if (flag) // it exited because the last term was reached
   last->pNxt = new CSubSexn (subSection) ;
 return 1;
 };

//=======================================================================================


/*#f:this constructs the  CGenSexn instance, and at the same time it also 
constructs the included CSbSexn pointer*/
CGenSexn::CGenSexn(pchar general, pchar subSection )
  {
  pNxt       = NULL;
  pSubSexns  = new CSubSexn( subSection );
  genStrng   = general;
  };

//--------------------------------------------------------------------------

CGenSexn::~CGenSexn()
  {
  delete pSubSexns;
  delete pNxt;
  };

//--------------------------------------------------------------------------

/*#F: If the general section already exists then only a new sub section is added
to the list, otherwise a new general section is also added to the list */

flag CGenSexn::Add (pchar generalSection, pchar subSection)
  {
  pCGenSexn p = pFirstGen;
  flag flag = 1;
  pCGenSexn last = NULL;

  while(p) 
    {
    if ( p->genStrng.XStrICmp (generalSection) == 0)  // matches with existing
      {                                             // case insensitive
      p->pSubSexns->Add ( subSection );
      flag = 0;
      break;
      }
    last = p;
    p = p->pNxt; //get next
    }
  if (flag) // exited the loop without finding a like minded friend
    {
    CGenSexn*pn = new CGenSexn ( generalSection, subSection  );
    CGenSexn*p=pFirstGen, *p1=NULL;
    while (p && ((_stricmp(p->genStrng(), generalSection)<0)))
      {
      p1=p;
      p=p->pNxt;
      }
    if (p1)
      {
      pn->pNxt=p1->pNxt;
      p1->pNxt=pn;
      }
    else
      {
      pn->pNxt=pFirstGen;
      pFirstGen=pn;
      }

    //last->pNxt = new CGenSexn ( generalSection, subSection  );
    };
  return 1;
  };  

//--------------------------------------------------------------------------

/*#F: The index value returned by the list box can be used to retrieve
the required general section */

pCGenSexn CGenSexn::GetByIndex ( int index )
  {
  pCGenSexn p = pFirstGen;

  int count = 0;
  while(p && ( count < index ) ) 
    {
    count++;
    p = p->pNxt; //get next
    }
  return p;
  };  

//--------------------------------------------------------------------------

/*#F: This will initially determine whether the file actually exists, if so the
value and groups associated with the line will be put into the "fValue" and 
fStrng variables in the associated CSubSexn, if no such line 
exists or the file does not exist then the default values are retrieved. */

void CGenSexn::ReadFromFile ( pCSubSexn currentSubSexn )
  {
  struct _stat buffer;
  int r = _stat( CDbgMngr::DbgMngrFilename(), &buffer );
  if(!r)// file exists
    { 
    char Buff[1024]; 
    GetPrivateProfileString(genStrng(), currentSubSexn->subStrng(), "xXxX", &Buff[0], sizeof(Buff), CDbgMngr::DbgMngrFilename());
    if (strcmp(Buff, "xXxX")!=0) // line exists in file
      {
      Strng s = Buff;// used only to break out tokens from Buff
      s.Trim(" ");
      Strng entireLine = s;
      int decider = 0;
      decider = entireLine.Find(','); // line only consists of a value
      if ( decider == -1 )
        currentSubSexn->fStrng = "xXxX";
      else
        currentSubSexn->fStrng = entireLine.Right ( entireLine.Length() - (decider + 1) );// all except the value & comma
      Strng Tok; // used to manipulate token
      Tok = s.StrTok(",",True);
      Tok.Trim(" ");// isolate the value
      currentSubSexn->fValue = atol(Tok.Str());// returns 0 if this cannot be done
      }
    else //line does not exist in file
      {
      currentSubSexn->fValue = 0;
      currentSubSexn->fStrng = "xXxX";
      };
    } 
  else
    { // dbg file does not exist ( -1)
    currentSubSexn->fValue = 0;
    currentSubSexn->fStrng = "xXxX";
    } 
  };  

//--------------------------------------------------------------------------

/*#F: This will open the file anew and write the required value and groups 
associated with the line to the file. If the default value for groups are 
retrieved, then no groups are written to the file*/

void CGenSexn::WriteToFile ( pCSubSexn currentSubSexn )
  {
  Strng actuallyWritten;
  if (strcmp(currentSubSexn->fStrng(), "xXxX") !=0) // groups != default val
    {
    actuallyWritten.Set("%ld",currentSubSexn->fValue);
    actuallyWritten += ",";
    actuallyWritten += currentSubSexn->fStrng();
    }
  else
    actuallyWritten.Set("%ld",currentSubSexn->fValue);

  // to posn the equals and values in a column 30 characters from start of subSexnname
  int remainder = 30 - currentSubSexn->subStrng.Length ();
  currentSubSexn->subStrng.RPad(remainder); // pad the remainder with spaces
  WritePrivateProfileString (genStrng(), currentSubSexn->subStrng(), actuallyWritten(), CDbgMngr::DbgMngrFilename() );
  };  

//=======================================================================================

/*#F: The Constructor appends the CDbgMngr object to end of linked list, records
 the object's "Group Name" and loads any items, from the file, which are listed after
 this group name.
 #x: if pGroup does not correlate with one of the Group names in the DbgMngrFile -
 eg SYSCAD.DMF in syscad71 directory, it will not be ignored.*/

CDbgMngr::CDbgMngr(pchar pSection, pchar pGroup)
  {
  sSection = pSection;//assign sexn name to member function for later reference
  sGroup = pGroup;   //assign group name to member function for later reference
  ASSERT(pSection);  //cannot be empty string
  ASSERT(pGroup);    //cannot be empty string
  ASSERT(sSection.Length()>0);  //cannot be empty string
  ASSERT(sGroup.Length()>0);    //cannot be empty string
  Items.Clear();// ensure that linked list assigned to sGroup is clear of all items
  pNxt=NULL;

  bOn=0;
  bOnPgm=0;
  //add to CDbgMngr list...
  StartupLock.Lock();

  CDbgMngr*p=pFirst, *p1=NULL;
  while (p && 
         ((_stricmp(p->sSection(), pSection)<0) ||
         ((_stricmp(p->sSection(), pSection)==0) && (_stricmp(p->sGroup(), pGroup)<0))))
    {
    p1=p;
    p=p->pNxt;
    }
  if (p1)
    {
    pNxt=p1->pNxt;
    p1->pNxt=this;
    }
  else
    {
    pNxt=pFirst;
    pFirst=this;
    }
  StartupLock.Unlock();

  //if(pFirst)// if a list exists 
  //  {
  //  pCDbgMngr p = pFirst;
  //  while(p->pNxt) // cycle through list until p is linked to NULL
  //    p = p->pNxt;
  //  p->pNxt = this; // assign the previous CDbgMngr's pointer to the current CDbgMngr
  //  }
  //else
  //  pFirst = this;

  Load();

  //add to Sections/sub-section list...
  if (CGenSexn::pFirstGen)
    CGenSexn::pFirstGen->Add(pSection, pGroup);
  else
    CGenSexn::pFirstGen = new CGenSexn(pSection, pGroup);
  if (0)
    {
    dbgpln("DbgMngr @%8x Nxt %8x : %s:%s", this, pNxt,sSection(), sGroup());
    pCDbgMngr p = pFirst;
    while(p)
      {
      dbgpln("         @%8x %8x: %s:%s",p, p->pNxt, p->sSection(), p->sGroup());
      p = p->pNxt;
      }
    }
  };

//--------------------------------------------------------------------------
/*#F: The Destructor .*/
CDbgMngr::~CDbgMngr()
  {
  if (0)
    {
    dbgpln("~DbgMngr @%8x %8x: %s:%s",this, pNxt, sSection(), sGroup());
    pCDbgMngr p = pFirst;
    while(p)
      {
      dbgpln("         @%8x %8x: %s:%s",p, p->pNxt, p->sSection(), p->sGroup());
      p = p->pNxt;
      }
    }
  Items.Clear();// ensure that linked list assigned to sGroup is clear of all items
  // append dbgmngr in list or create list
  StartupLock.Lock();
  ASSERT(pFirst!=NULL);
  if (pFirst==this) // Unlink this Object
    pFirst=pNxt;
  else
    {
    pCDbgMngr p = pFirst;
    while(p && (p->pNxt!=this))
      p = p->pNxt;
    if (p)
      p->pNxt=pNxt;
    else 
      DoBreak();
    //while(p && _CrtIsValidPointer(p, sizeof(CDbgMngr), True) && (p->pNxt!=this))
    //  p = p->pNxt;
    //if (p && _CrtIsValidPointer(p, sizeof(CDbgMngr), True))
    //  p->pNxt=pNxt;
    }
  StartupLock.Unlock();
  }

//--------------------------------------------------------------------------

void CDbgMngr::StartUp()
  {
  bStarted=1;
  LoadAll();
  };

//--------------------------------------------------------------------------

void CDbgMngr::ShutDown()
  {
  bStarted=0;
  delete CGenSexn::pFirstGen;
  CGenSexn::pFirstGen=NULL;
  };

//--------------------------------------------------------------------------
/*#f:Places debugging-enabled Items in a linked list, provided that the file and group name
 exists and the group is enabled to produce debug information ( bOn != 0)*/
void CDbgMngr::Load()
  {
  if (!bStarted) 
    return; 
  struct _stat buffer;
  int r = _stat( DbgMngrFilename(), &buffer );
  if(!r)
    { 
    bOn=0;
    Items.Clear();
    char Buff[1024]; // preferred method but GetPrivateProfileString !convert char Buff(*)[1024] to char*
                     // thus needing &Buff[0] ie point to 1st char of limited array of char ie sim to a pointer
                     // note type casting (char *)&Buff would also have worke
                                                                  //thus independent of ctual size
    GetPrivateProfileString(sSection(), sGroup(), "xXxX", &Buff[0], sizeof(Buff), DbgMngrFilename());
    if (strcmp(Buff, "xXxX")!=0)
      {
      Strng s = Buff;// used only to break out tokens from Buff
      s.Trim(" ");
      Strng Tok; // used to manipulate token
      Tok = s.StrTok(",",True);
      Tok.Trim(" ");// trim any trailing spaces from token
      // should have isolated bOn
      bOn = atol(Tok.Length()> 0 ? Tok.Str() : "0");// 0 if this cannot be done
      Tok = s.StrTok(",");// break out next token up to nxt ','
      if (bOn)
        while( Tok.Length()>0)
          {
          Tok.Trim(" ");// trim off any leading/trailing spaces
          Strng inWaiting = Tok;
          pchar t = Tok();//.Buff;
          t[1]='\0';// separate the yes/no from rest of sentence
          if(!(strcmp(t,"0")))
            {
            Tok = s.StrTok(",");// break out next token up to nxt ','
            continue;// dont add to list of items if switched off
            }
          else
            Items.Append(inWaiting());// append whole token is not preceded by digit
            Tok = s.StrTok(",");// break out next token up to nxt ','
          }
      }
    //if (bOn)
    //  dbgpln("CDbgMngr %s:%s", sSection()!=NULL ? sSection() : "?", sGroup()!=NULL ? sGroup() : "?");
    } 
  else
    { // file does not exist ( -1)
    } // therefore do nothing
  }

//--------------------------------------------------------------------------
/*#F: Only when the DbgMngrFile is updated or modified, does it cycle through the linked
 list of CDbgMngrs, loading each in turn*/
void CDbgMngr::LoadAll()
  {
  if (!bStarted) 
    return; 
  //SetRootDirectory("c:\\syscad71");
  Strng NewDbgMngrFilename(TemporaryFiles());
#if BLDDEPENDENTFILES
  NewDbgMngrFilename.Set("%sScd_Dbg.%i.INI", NewDbgMngrFilename(), SCD_BUILDNO);
#else
  NewDbgMngrFilename+="Scd_Dbg.ini"; // specifies the DbgMngrFile to be used  
#endif
  flag ForceLoad=_stricmp(NewDbgMngrFilename(), DbgMngrFilename())!=0;
  struct _stat FileTime;            //if used an unallocated pointer memory would be unstable
  long CurrentFileTime = 0;
  if ((_stat( DbgMngrFilename(), &FileTime )) == 0) // if file info obtained ie file exists
    CurrentFileTime = FileTime.st_mtime;
  if (CurrentFileTime != LastFileTime || ForceLoad) // if file recently updated DO
    {
    if (ForceLoad)
      DbgMngrFilename=NewDbgMngrFilename;

    StartupLock.Lock();
    pCDbgMngr pArb;
    if(pFirst)
      {
      pFirst->Load();
      pArb=pFirst->pNxt;
      while(pArb)
        {
        pArb->Load();
        pArb= pArb->pNxt;
        };
      };
    StartupLock.Unlock();
    LastFileTime = CurrentFileTime;
    };
  };

//--------------------------------------------------------------------------

/*#F: When the size of the Dbg-File is greater than MaxFileSize( in KiloBytes )
then it is deleted. PercentRemaining - determines how much of the most
recent data sent to the debug file, remains after the deletion.
This an interger value specifing the percentage that is to be saved
from 0 - 50, the default is 0 ie a complete deletion. A number greater than 50 becomes 50 %.*/
void CDbgMngr::ChkFileSize(long MaxFileSize, long PercentRemaining)
  {
  if (!dbgtestfileopen())
    return;
  if (!bStarted) 
    return; 

  if (PercentRemaining < 0) 
    PercentRemaining = 0;
  if (PercentRemaining > 50)
    PercentRemaining = 50;   
  float percentage = (float)PercentRemaining / 100;
  long actualFileSize = MaxFileSize * 1024;
  long remainder = 0; 
  //SetRootDirectory("c:\\syscad71");
  //struct _stat dbg_stats;
//  Strng test = TemporaryFiles();
//  if (test.Length()>0)
//    if (test[test.Length()-1] != '\\')
//      test += "\\";
//  test += "Scd_Dbg.Txt";
//  //dbgopen(test());
//  int r = _stat( test(), &dbg_stats );
//  long qwerty = dbg_stats.st_size;
//  if (r==0 && dbg_stats.st_size >= actualFileSize)

  long filesize=GetFileSize(dbgfile(), NULL);
//  long qwerty = dbg_stats.st_size;
  if (filesize >= actualFileSize)
    {
    float rem = percentage * actualFileSize;
    remainder = (long)rem;
    dbgfiletrunc(remainder);
    LogNote("DbgMngr", 0, "Debug file truncated");
    }
  };
#endif

//--------------------------------------------------------------------------
