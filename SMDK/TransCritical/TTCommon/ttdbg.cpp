#include "ttdbg.h"


bool TTDbg::tkinited = false;
int TTDbg::nInsts = 0;
Tcl_Interp *TTDbg::the_interp;
Tk_Window TTDbg::tkmain;






void CALLBACK TclTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
  int ok=TRUE;
  while (ok) {
    ok = Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT);
  }
}


TTDbg::TTDbg() {
  nInsts++; 
}


TTDbg::~TTDbg() {
  nInsts--;
  if (nInsts<=0) {
    KillTimer(NULL, 42);
    Tcl_Finalize();
  }
}



int TTDbg::tcltk_init(void)
{
  if (tkinited)  {      // Already initialized
      return 0;
  }
  

    int code;

    /* Absence of the following line is said to be an error with
     * tcl 8.4 on all platforms, and is known to cause crashes under
     * Windows */

    /* Unfortunately, *presence* of the line appears to cause crashes
     * with tcl 8.0... */

#ifndef TCL80
    Tcl_FindExecutable(NULL);
#endif

    the_interp = Tcl_CreateInterp();
    code = Tcl_Init(the_interp); /* Undocumented... If omitted, you
				    get the windows but no event
				    handling. (Update: the
				    documentation is in the Tcl
				    sources, just not shipped
				    w/RedHat) */
    if (code != TCL_OK)
      return 1;
    code = Tk_Init(the_interp);  /* Load Tk into interpreter */
    if (code != TCL_OK)
      return 2;
    Tcl_StaticPackage(the_interp, "Tk", Tk_Init, Tk_SafeInit);

#ifdef Win32
    Tcl_SetServiceMode(TCL_SERVICE_ALL);
#endif

    tkmain = Tk_MainWindow(the_interp);


    code = Tcl_EvalFile(the_interp,"c:/devel/TTCommon/command.tcl");
    if (code != TCL_OK)
      Tcl_Eval(the_interp, "pack [label .l -text NoStart]");
    
    code = Tcl_Eval(the_interp,"update idletasks");
    SetTimer(NULL,42,10,TclTimerProc);
    setVar("nInsts", nInsts);
    tkinited = true;
    return 0;
}




void TTDbg::log(char*fmt,...) 
{
  char msgstr[80];
  char buf[80];
  va_list argptr;
  int cnt;
  
  va_start(argptr, fmt);
  cnt = vsprintf(msgstr, fmt, argptr);
  va_end(argptr);

  sprintf(buf,  "$twin insert end \"%s\n\"", msgstr);
  Tcl_Eval(the_interp, buf);  
}
  




void TTDbg::newVar(char *s) {
  char buf[80];
  sprintf(buf, "newVar %s\n", s);
  Tcl_Eval(the_interp, buf);  
}

void TTDbg::setVar(char *s, double d) {
  char buf[80];
  sprintf(buf, "setVar %s %12.5g\n", s, d);
  Tcl_Eval(the_interp, buf);
}

void TTDbg::setVar(char *s, int i) {
  char buf[80];
  sprintf(buf, "setVar %s %6d\n", s, (int)i);
  Tcl_Eval(the_interp, buf);
}


