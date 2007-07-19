//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __SCD_WM_H
#define __SCD_WM_H

#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif

#ifdef __SCD_WM_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//SysCAD user defined messages...
#define WMU_ACCREFRESHCANCEL     (WM_USER + 500)
#define WMU_ACCREFRESHSAVE       (WM_USER + 501)
#define WMU_ACCCOMPLETE          (WM_USER + 502)
#define WMU_ADDTAG2TREND         (WM_USER + 503)
#define WMU_ADDTOQUICKVIEW       (WM_USER + 504)

#define WMU_CMD                  (WM_USER + 506)	
#define WMU_CMDDONE              (WM_USER + 507)
#define WMU_CMDNEXT              (WM_USER + 508)
#define WMU_CMDSTATE             (WM_USER + 509)
#define WMU_COMBOBOXMSG          (WM_USER + 510)	
#define WMU_COM_CMD              (WM_USER + 511)	
#define WMU_COM_APP              (WM_USER + 512)	
#define WMU_COM_SLV              (WM_USER + 513)	
#define WMU_COMEVT               (WM_USER + 514)	
#define WMU_COMEVTRELAY          (WM_USER + 515)	

#define WMU_DDEUPDATESTATS       (WM_USER + 520)	
#define WMU_DLGBUSYOPENCLOSE     (WM_USER + 521)	
#define WMU_DLGBUSYSET           (WM_USER + 522)	
#define WMU_DLGBUSYTESTCAN       (WM_USER + 523)	
#define WMU_DOGRFCMD             (WM_USER + 524)	
#define WMU_DOSETFOCUS           (WM_USER + 525)	
#define WMU_DOUPDATESTATUSBAR    (WM_USER + 526)	
#define WMU_DRVUPDATEBTNS        (WM_USER + 527)	
#define WMU_DRVUPDATESTATS       (WM_USER + 528)	
#define WMU_ENDCYCLES            (WM_USER + 529)	
//#define WMU_EXECACTION           (WM_USER + 530)	
#define WMU_EXECCOMMAND          (WM_USER + 531)	
#define WMU_EXECUPDATEDISPLAY    (WM_USER + 532)	
#define WMU_FLUSHMSGQ            (WM_USER + 533)	
#define WMU_FLWSOLVECMD          (WM_USER + 534)	
#define WMU_FXDEDTEDITDONE       (WM_USER + 535)	
#define WMU_FXDEDTFLDWRITE       (WM_USER + 536)	
#define WMU_GRFWNDDSPUPDATE      (WM_USER + 537)	
#define WMU_GRFWNDWRITESUBS      (WM_USER + 538)	
#define WMU_HSTQRYUPDATE         (WM_USER + 539)	 /* NBNBNB THIIS IS ALSO DEFINED IN SCDHist\STDAFX.H */
#define WMU_HSTUPDATESTATS       (WM_USER + 540)	
#define WMU_INFOVIEWMSG          (WM_USER + 541)	
#define WMU_LOGMSG               (WM_USER + 543)	
#define WMU_OLEINFOUPDATE        (WM_USER + 544)	
#define WMU_ONUPDATE             (WM_USER + 545)	
#define WMU_OPCSRVRUPDATESTATS   (WM_USER + 546)	
#define WMU_PB_UPDATEDATA        (WM_USER + 547)	
#define WMU_REPORT               (WM_USER + 548)	
#define WMU_SCROLL               (WM_USER + 549)	
#define WMU_SETHEADERTABS        (WM_USER + 550)	
#define WMU_SOLVECOMMAND         (WM_USER + 551)	
#define WMU_STARTCYCLES          (WM_USER + 552)	
#define WMU_TAGACTION            (WM_USER + 553)	

#define WMU_UPDATEALLVIEWS       (WM_USER + 560)	
#define WMU_UPDATEINDICATOR      (WM_USER + 561)	
#define WMU_UPDATEMAINWND        (WM_USER + 562)	
#define WMU_UPDATEMSGMENUBTN     (WM_USER + 563)	
#define WMU_UPDATEPGMDEBUG       (WM_USER + 564)	
#define WMU_UPDATESHOWWND        (WM_USER + 565)	
#define WMU_UPDATESTATUSBAR      (WM_USER + 566)	
#define WMU_XSENDMESSAGE         (WM_USER + 567)	
#define WMU_XSEQMANUALCHG        (WM_USER + 568)	
#define WMU_XSEQMANUALREPLAY     (WM_USER + 569)	
#define WMU_APPEVT               (WM_USER + 570)	
#define WMU_DRIVERMANAGER        (WM_USER + 571)
#define WMU_RELOADDRIVER         (WM_USER + 572)
#define WMU_CLOSEORDINFO         (WM_USER + 573)
#define WMU_GRFTAGGROUPS         (WM_USER + 574)
#define WMU_NODEAUDIT            (WM_USER + 575)
#define WMU_EDITRCTDLG           (WM_USER + 576)
#define WMU_EXCEPTION            (WM_USER + 577)
#define WMU_SETLICENSE           (WM_USER + 578)
#define WMU_CHKLICENSE           (WM_USER + 579)

//WPARAM sub-messages used by WMU_TAGACTION...
#define SUB_TAG_ACCESS                       0x0001
#define SUB_TAG_MARK                         0x0002
#define SUB_TAG_GOTO                         0x0004
#define SUB_TAG_NOERRDLG                     0x0008
#define SUB_TAG_KWIK                         0x0010
#define SUB_TAG_CHANGE                       0x0020
#define SUB_TAG_MDLHELP                      0x0040
#define SUB_TAG_COPYBLK                      0x0080
#define SUB_TAG_ACCREFRESHSAVE               0x0100
#define SUB_TAG_NOGRAPHICS                   0x0200
#define SUB_TAG_ACCNOUPDHIST                 0x0400

//#define SUB_TAG_FIND_NOERRDLG                 (SUB_TAG_FIND                     |SUB_TAG_NOERRDLG                   )
//#define SUB_TAG_FIND_KWIK                     (SUB_TAG_FIND                                            |SUB_TAG_KWIK)
//#define SUB_TAG_FIND_NOERRDLG_KWIK            (SUB_TAG_FIND                     |SUB_TAG_NOERRDLG|SUB_TAG_KWIK)
//#define SUB_TAG_FINDANDACCESS                 (SUB_TAG_FIND|SUB_TAG_ACCESS                                          )
//#define SUB_TAG_FINDANDACCESS_NOERRDLG        (SUB_TAG_FIND|SUB_TAG_ACCESS|SUB_TAG_NOERRDLG                   )
//#define SUB_TAG_FINDANDACCESS_KWIK            (SUB_TAG_FIND|SUB_TAG_ACCESS                       |SUB_TAG_KWIK)
//#define SUB_TAG_FINDANDACCESS_NOERRDLG_KWIK   (SUB_TAG_FIND|SUB_TAG_ACCESS|SUB_TAG_NOERRDLG|SUB_TAG_KWIK)

//WPARAM sub-messages used by WMU_INFOVIEWMSG...
#define SUB_INFO_STRING          1
#define SUB_INFO_FORCESTRING     2

//WPARAM sub-messages used by WMU_GENERATEOLEREPORT
#define SUB_REPORT_GETVALUES     0
#define SUB_REPORT_TREND         1
#define SUB_REPORT_SETVALUES     2
#define SUB_REPORT_OPEN          3
#define SUB_REPORT_SAVEANDCLOSE  4
#define SUB_REPORT_MACRO         5
#define SUB_REPORT_GENERALINFO   6

//WPARAM sub-messages used by WMU_REPORT...
#define SUB_REPACTION_GENERATE   0
#define SUB_REPACTION_START      1
#define SUB_REPACTION_PROCESS    2
#define SUB_REPACTION_END        3

////WPARAM sub-messages used by WMU_EXECACTION...
//#define SUB_EXECACTION_START     1
//#define SUB_EXECACTION_IDLE      2
//#define SUB_EXECACTION_STOP      3
//#define SUB_EXECACTION_STEP      4

//WPARAM sub-messages used by WMU_DDEUPDATESTATS and WMU_OPCSRVRUPDATESTATS...
#define SUB_UPDATE_SENDMSG       0x0001
#define SUB_UPDATE_SUBSCHANGED   0x0002
#define SUB_UPDATE_OPENWND       0x0004
#define SUB_UPDATE_UNUSED1       0x0008
#define SUB_UPDATE_EOSTART       0x0010
#define SUB_UPDATE_EOSTOP        0x0020
#define SUB_UPDATE_EOWRITE       0x0040
#define SUB_UPDATE_EOEXEC        0x0080
#define SUB_UPDATE_REDRAW        0x0100

//WPARAM sub-messages used by WMU_CMD...
#define SUB_CMD_RUNSCRIPT             1
#define SUB_CMD_GENERATEOLEREPORT     2
#define SUB_CMD_CMDLINE               3
#define SUB_CMD_ADDTOARCHIVE          4
#define SUB_CMD_ARCHIVEEXECUTE        5
#define SUB_CMD_ARCHIVESTEPDONE       6
#define SUB_CMD_ARCHIVESTART          7
#define SUB_CMD_ARCHIVESTOP           8
#define SUB_CMD_OPCSRVRSTATS          9 
#define SUB_CMD_PRJWNDUPDATE          10
#define SUB_CMD_HISTORYON             11
#define SUB_CMD_HISTORYOFF            12
#define SUB_CMD_ALLHISTORYON          13
#define SUB_CMD_ALLHISTORYOFF         14
#define SUB_CMD_ALLMDLHISTORYON       15
#define SUB_CMD_ALLMDLHISTORYOFF      16

//WPARAM sub-messages used by WMU_UPDATEMAINWND...
#define SUB_UPDMAIN_UPDATE            1 //update main window and bring window specified by lParam (if any) to top
#define SUB_UPDMAIN_PRJLOADED         2 //project just loaded
#define SUB_UPDMAIN_ACTIVATECMDS      3 //ensure top window commands are active
#define SUB_UPDMAIN_BACKGROUND        4 //refresh main window frame background
#define SUB_UPDMAIN_ACTIVATEAPP       5 //same as SUB_UPDMAIN_UPDATE but also activate syscad
#define SUB_UPDMAIN_STARTED           6 //SysCAD startup completed

//WPARAM sub-messages used by WMU_GRFTAGGROUPS...
#define SUB_GTG_BUILD                 0 //
#define SUB_GTG_ACTIVATE              1 //
#define SUB_GTG_DEACTIVATE            2 //

//WPARAM sub-messages used by WMU_NODEAUDIT...
#define SUB_NA_CLS_COMPACT      0x0001
#define SUB_NA_CLS_SUMMARY      0x0002
#define SUB_NA_ADT_MASS         0x0004
#define SUB_NA_ADT_MASSTOTAL    0x0008
#define SUB_NA_ADT_HEAT         0x0010
#define SUB_NA_ADT_HEATTOTAL    0x0020


//===========================================================================
#undef DllImportExport

#endif

