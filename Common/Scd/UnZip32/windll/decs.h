/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef __decs_h   /* prevent multiple inclusions */
#define __decs_h

#include <windows.h>
#ifndef __unzip_h
#  include "../unzip.h"
#endif
#ifndef __structs_h
#  include "../windll/structs.h"
#endif

#define Wiz_Match match

#ifdef ZIP32
#define DLX __declspec(dllimport) 
#else
#define DLX __declspec(dllexport) 
#endif

DLX void    WINAPI Wiz_NoPrinting(int f);
DLX int     WINAPI Wiz_Validate(LPSTR archive, int AllCodes);
DLX BOOL    WINAPI Wiz_Init(zvoid *, LPUSERFUNCTIONS);
DLX BOOL    WINAPI Wiz_SetOpts(zvoid *, LPDCL);
DLX int     WINAPI Wiz_Unzip(zvoid *, int, char **, int, char **);
DLX int     WINAPI Wiz_SingleEntryUnzip(int, char **, int, char **,
                     LPDCL, LPUSERFUNCTIONS);

DLX int     WINAPI Wiz_UnzipToMemory(LPSTR zip, LPSTR file,
                                 LPUSERFUNCTIONS lpUserFunctions,
                                 UzpBuffer *retstr);
DLX int     WINAPI Wiz_Grep(LPSTR archive, LPSTR file, LPSTR pattern,
                                int cmd, int SkipBin,
                                LPUSERFUNCTIONS lpUserFunctions);

#endif /* __decs_h */
