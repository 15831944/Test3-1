#include <iostream>
#include <windows.h>
#include "ttdbg.h"











int PASCAL
WinMain (HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine,
	 int CmdShow)
{

  TTDbg d;
  d.tcltk_init();
  MessageBox (NULL, "Quit", "TTDebug", MB_OK);  


	return 0;
}




    
    
