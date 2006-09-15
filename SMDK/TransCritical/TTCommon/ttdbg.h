#include <cstdarg>
#include <windows.h>
#include <tcl.h>
#include <tk.h>


#define LOGf(X) dbg.log(" " #X "%12.5g", X)   
#define LOGi(X) dbg.log(" " #X "%6d", X)
#define WATCH(X) dbg.setVar(#X, X)
#define MSG(X) dbg.log(X);


class TTDbg 
{
  
  static bool tkinited;
  static Tcl_Interp *the_interp;
  static Tk_Window tkmain;
  static nInsts;

  char errmsg[80];
  

 public:
  TTDbg();
  ~TTDbg();
  

  
  int tcltk_init(void);
  void log(char*,...);
/*   void log(char*, double); */
/*   void log(char*, int); */
/*   void log(char*, int, int); */
/*   void log(char*, int, double); */
/*   void log(char*, int, int, double); */
  void newVar(char*);
  void setVar(char*, double);
  void setVar(char*, int);
};

