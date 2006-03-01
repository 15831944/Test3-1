//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ZIPSTUFF_H
#define  __ZIPSTUFF_H

#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#if WITHZIP
#include "zipstuff.h"
#include "gpfuncs.h"
#include "errorlog.h"
//#include "optoff.h"
       
#define API 1
#define NO_MKTEMP

//---------------------------------------------------------------------------

extern "C"
  {
  #include "..\zip32\api.h"
  //#include "..\unzip\unzip.h"
  #include "..\unzip32\windll\structs.h"
  #include "..\unzip32\windll\decs.h"
  }

#pragma LIBCOMMENT("..\\..\\scd\\Zip32\\", "\\Zip32" )
#pragma LIBCOMMENT("..\\..\\scd\\UnZip32\\", "\\UnZip32" )

#undef NO_MKTEMP
#undef API

//---------------------------------------------------------------------------
/* Password entry routine - see password.c in the wiz directory for how
   this is actually implemented in Wiz. If you have an encrypted file,
   this will probably give you great pain. Note that none of the
   parameters are being used here, and this will give you warnings. */
static int WINAPI DummyPassword(LPSTR p, int n, LPCSTR m, LPCSTR name)
  {
  return 1;
  }

//---------------------------------------------------------------------------
/* Dummy "print" routine that simply outputs what is sent from the dll */
static int WINAPI DummyPrint(char far *buf, unsigned long size)
  {
  #ifndef _RELEASE
  if (buf && size>1)
    {
    static char* printbufflist[10];
    static int printbufflistlen=0;
    char bigbuff[16000];
    bigbuff[0]=0;
    if (strchr(buf, '\n') || strchr(buf, '\r'))
      {
      for (int i=0; i<printbufflistlen; i++)
        {
        strcat(bigbuff,printbufflist[i]);
        delete printbufflist[i];
        }
      strcat(bigbuff,buf);

      LogNote("Zip", 0, "%s", (char *)bigbuff);
      
      printbufflistlen=0;
      }
    else
      {
      printbufflist[printbufflistlen] = new char[strlen(buf)+1];
      strcpy(printbufflist[printbufflistlen], buf);
      printbufflistlen++;
      }
    }
  #endif
  return (unsigned int) size;
  }

//---------------------------------------------------------------------------
/* Dummy "comment" routine. See comment.c in the wiz directory for how
   this is actually implemented in Wiz. This will probably cause you
   great pain if you ever actually make a call into it. */
static int WINAPI DummyComment(char far *szBuf)
  {
  szBuf[0] = '\0';
  return TRUE;
  }

//---------------------------------------------------------------------------
/* This is a very stripped down version of what is done in Wiz. Essentially
   what this function is for is to do a listing of an archive contents. It
   is actually never called in this example, but a dummy procedure had to
   be put in, so this was used. */
void WINAPI ReceiveDllMessage(unsigned long ucsize, unsigned long csiz,
    unsigned cfactor,
    unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
    char c, LPSTR filename, LPSTR methbuf, unsigned long crc, char fCrypt)
  {
  char psLBEntry[_MAX_PATH];
  char LongHdrStats[] = "%7lu  %7lu %4s  %02u-%02u-%02u  %02u:%02u  %c%s";
  char CompFactorStr[] = "%c%d%%";
  char CompFactor100[] = "100%%";
  char szCompFactor[10];
  char sgn;

  if (csiz > ucsize)
    sgn = '-';
  else
    sgn = ' ';
  if (cfactor == 100)
    lstrcpy(szCompFactor, CompFactor100);
  else
    sprintf(szCompFactor, CompFactorStr, sgn, cfactor);
  wsprintf(psLBEntry, LongHdrStats, ucsize, csiz, szCompFactor, mo, dy, yr, hh, mm, c, filename);

  LogNote("UnZip", 0, "%s", psLBEntry);
  }

//---------------------------------------------------------------------------
/* Password entry routine - see password.c in the wiz directory for how
   this is actually implemented in WiZ. If you have an encrypted file,
   this will probably give you great pain. */
int WINAPI password(char *p, int n, const char *m, const char *name)
  {
  return 1;
  }

//---------------------------------------------------------------------------
/* Dummy "print" routine that simply outputs what is sent from the dll */
int WINAPI DisplayBuf(LPSTR buf, unsigned long size)
  {
  #ifndef _RELEASE
  if (buf && size>1)
    LogNote("UnZip", 0, "%s", (char *)buf);
  #endif
  return (unsigned int)size;
  }

//---------------------------------------------------------------------------
/* This is where you will decide if you want to replace, rename etc existing
   files. */
int WINAPI GetReplaceDlgRetVal(char *filename)
  {
  return 1;
  }

//---------------------------------------------------------------------------
//===========================================================================

int CZipFile::AddCmd(const char * Cmd)
  {
  //kga : 29/4/02 : increased MaxCmd and put in test
  const int MaxCmd = 256;
  if (m_nCmds>=MaxCmd)
    {
    LogError("Zip", 0, "Exceeded MaxCmd, '%s' not added!", (char *)Cmd);
    //TOO MANY COMMANDS!!!!!!!
    return m_nCmds;
    }

  //char *pBuff[100];
  char *pBuff[MaxCmd];
  int n=m_nCmds++;
  char *New;
  int l=0;
  for (int j=0; j<n; j++)
    pBuff[j]=m_Cmds[j];
  pBuff[j]=(char*)Cmd;

  int lt=0;
  for (int i=0; i<m_nCmds; i++)
    lt+=strlen(pBuff[i])+1;

  New=new char[m_nCmds*sizeof(char**)+lt]; //management of this list could improve!?!

  char **pp=(char**)New;
  char *p=New+m_nCmds*sizeof(char**);

  for (i=0; i<m_nCmds; i++)
    {
    strcpy(p, pBuff[i]);
    pp[i]=p;
    p+=strlen(p)+1;
    }

  if (n)
    delete m_Cmds;
  m_Cmds=(char**)New;
  return m_nCmds;
  }

//---------------------------------------------------------------------------

int CZipFile::ZipIt()
  {

  ZPOPT ZpOpt;
  ZpVer Zv;

  ZpVersion(&Zv);

  ZCL ZpZCL;

  memset(&ZpOpt, 0, sizeof(ZpOpt));

  /* Here is where the action starts */
  ZpOpt.fSuffix = FALSE;        /* include suffixes (not yet implemented) */
  ZpOpt.fEncrypt = FALSE;       /* true if encryption wanted */
  ZpOpt.fSystem = FALSE;        /* true to include system/hidden files */
  ZpOpt.fVolume = FALSE;        /* true if storing volume label */
  ZpOpt.fExtra = FALSE;         /* true if including extra attributes */
  ZpOpt.fNoDirEntries = FALSE;  /* true if ignoring directory entries */
  ZpOpt.fVerbose = FALSE;       /* true if full messages wanted */
  ZpOpt.fQuiet = FALSE;         /* true if minimum messages wanted */
  ZpOpt.fCRLF_LF = FALSE;       /* true if translate CR/LF to LF */
  ZpOpt.fLF_CRLF = FALSE;       /* true if translate LF to CR/LF */
  ZpOpt.fJunkDir = FALSE;       /* true if junking directory names */
  ZpOpt.fGrow = FALSE;          /* true if allow appending to zip file */
  ZpOpt.fForce = FALSE;         /* true if making entries using DOS names */
  ZpOpt.fMove = FALSE;          /* true if deleting files added or updated */
  ZpOpt.fDeleteEntries = FALSE; /* true if deleting files from archive */
  ZpOpt.fUpdate = FALSE;        /* true if updating zip file--overwrite only if newer */
  ZpOpt.fFreshen = FALSE;       /* true if freshening zip file--overwrite only */
  ZpOpt.fJunkSFX = FALSE;       /* true if junking sfx prefix*/
  ZpOpt.fLatestTime = FALSE;    /* true if setting zip file time to time of latest file in archive */
  ZpOpt.fComment = FALSE;       /* true if putting comment in zip file */
  ZpOpt.fOffsets = FALSE;       /* true if updating archive offsets for sfx files */
  //ZpOpt.fPrivilige = ;
  //ZpOpt.fEncryption = ;
  ZpOpt.fRecurse = 1;           /* subdir recursing mode: 1 = "-r", 2 = "-R" */
  ZpOpt.fRepair = 0;            /* archive repair mode: 1 = "-F", 2 = "-FF" */
  //ZpOpt.fLevel = ;
  ZpOpt.Date = NULL;            /* Not using, set to NULL pointer */
  //getcwd(szFullPath, PATH_MAX); /* Set directory to current directory */
  
  Strng Rt, Nm;
  Rt.FnDrivePath(m_Name());
  Nm.FnNameExt(m_Name());
  
  ZpOpt.szRootDir = Rt();
  ZpZCL.lpszZipFN = Nm();       /* archive to be created/updated */
  ZpZCL.argc = m_nCmds;         /* number of files to archive - adjust for the*/
  ZpZCL.FNV  = m_Cmds;

//  //AddZipCmd(ZpZCL, "-r");
//  AddZipCmd(ZpZCL, "-q");
//  AddZipCmd(ZpZCL, "D:\\Temp\\*.*");

  ZIPUSERFUNCTIONS Zuf;
  memset(&Zuf, 0, sizeof(Zuf));

  Zuf.print = DummyPrint;
  Zuf.password = DummyPassword;
  Zuf.comment = DummyComment;
  Zuf.ServiceApplication = NULL;

  ZpInit(&Zuf);

  ZpSetOptions(&ZpOpt);

  /* Go zip 'em up */
  int ret=ZpArchive(ZpZCL);

  delete m_Cmds;
  m_nCmds=0;
  m_Cmds=NULL;

  return ret;
  };

//---------------------------------------------------------------------------

int CZipFile::UnZipIt()
  {
  USERFUNCTIONS uf;
  memset(&uf, 0, sizeof(uf));

  uf.password = password;
  uf.print = DisplayBuf;
  uf.sound = NULL;
  uf.replace = GetReplaceDlgRetVal;
  uf.SendApplicationMessage = ReceiveDllMessage;

  DCL  dcl; 
  memset(&dcl, 0, sizeof(dcl));
  /*
     Here is where the actual extraction process begins. First we set up the
     flags to be passed into the dll.
   */
  dcl.ncflag = 0; /* Write to stdout if true */
  dcl.fQuiet = 0; /* We want all messages.
                        1 = fewer messages,
                        2 = no messages */
  dcl.ntflag = 0; /* test zip file if true */
  dcl.nvflag = 0; /* give a verbose listing if true */
  dcl.nzflag = 0; /* display a zip file comment if true */
  dcl.ndflag = 1; /* Recreate directories if true */
  dcl.naflag = 0; /* Do not convert CR to CRLF */
  dcl.nfflag = 0; /* Do not freshen existing files only */
  dcl.noflag = 1; /* Over-write all files if true */
  dcl.ExtractOnlyNewer = 0; /* Do not extract only newer */
  dcl.PromptToOverwrite = 0; /* "Overwrite all" selected -> no query mode */
  dcl.lpszZipFN = m_Name();//argv[1]; /* The archive name */
  dcl.lpszExtractDir = m_Dest(); /* The directory to extract to. This is set
                                   to NULL if you are extracting to the
                                   current directory.
                                 */
  /*
     As this is a quite short example, intended primarily to show how to
     load and call in to the dll, the command-line parameters are only
     parsed in a very simplistic way:
     We assume that the command-line parameters after the zip archive
     make up a list of file patterns:
     " [file_i1] [file_i2] ... [file_iN] [-x file_x1 [file_x2] ...]".
     We scan for an argument "-x"; all arguments in front are
     "include file patterns", all arguments after are "exclude file patterns".
     If no more arguments are given, we extract ALL files.

     In summary, the example program should be run like:
     example <archive.name> [files to include] [-x files to exclude]
     ("<...> denotes mandatory arguments, "[...]" optional arguments)
   */
  //x_opt = NULL;
  //if (argc > 2) {
  //  infv = &argv[2];
  //  for (infc = 0; infc < argc-2; infc++)
  //    if (!strcmp("-x", infv[infc])) {
  //        x_opt = infv[infc];
  //        infv[infc] = NULL;
  //        break;
  //    }
  //  exfc = argc - infc - 3;
  //  if (exfc > 0)
  //    exfv = &argv[infc+3];
  //  else {
  //    exfc = 0;
  //    exfv = NULL;
  //  }
  //} else {
  //  infc = exfc = 0;
  //  infv = exfv = NULL;
  //}
  int infc=0; 
  int exfc=0;
  char **infv=m_Cmds;
  char **exfv=NULL;

  for (int i=0; i<m_nCmds; i++)
    {
    if (stricmp(m_Cmds[i], "-x")==0)
      {
      i++;
      break;
      }
    else
      infc++;
    }
  if (i<m_nCmds)
    {
    exfc=i-m_nCmds;
    exfv=&m_Cmds[i];
    }

  int retcode = Wiz_SingleEntryUnzip(infc, infv, exfc, exfv, &dcl, &uf);
  //if (x_opt) {
  //  infv[infc] = x_opt;
  //  x_opt = NULL;
  //}

  //if (retcode != 0)
  //   printf("Error unzipping...\n");

  //FreeUpMemory();
  //FreeLibrary(hUnzipDll);
  return retcode;
  }

//---------------------------------------------------------------------------

int CZipFile::ZipOne(LPCTSTR FileName, bool DeleteOriginal)
  {
  if (FileName==NULL)
    return 901;
  if (!FileExists((char*)FileName))
    return 902;

  Strng ZipFile(FileName);
  ZipFile += ".zip";
  CZipFile zf(ZipFile());
  Strng ToZip;
  ToZip.FnNameExt((char*)FileName);
  zf.AddCmd(ToZip());
  int retcode = zf.ZipIt();
  if (retcode)
    return retcode;
  if (!FileExists(ZipFile()))
    return 903;
  if (DeleteOriginal && !DeleteFile(FileName))
    {
    Strng E;
    E.GetSystemErrorStr(GetLastError());
    return 904;
    }
  return 0;
  }

//---------------------------------------------------------------------------

int CZipFile::UnZipOne(LPCTSTR FileName)
  {
  if (FileName==NULL)
    return 901;
  if (!FileExists((char*)FileName))
    return 902;
  Strng Dest;
  Dest.FnDrivePath((char*)FileName);
  CZipFile zf(FileName, Dest());
  int retcode = zf.UnZipIt();
  return retcode;
  }

//---------------------------------------------------------------------------

#endif