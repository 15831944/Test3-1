//-------------------------------------------------------------------------
//used to generate accessable message while compiling, seen in build window with other compile errors etc
#define chSTR(x)     #x
#define chSTR2(x) chSTR(x)
#define chMSG(desc) message(__FILE__ "(" chSTR2(__LINE__) ") :" #desc)
#define chFIXIT(desc) message(__FILE__ "(" chSTR2(__LINE__) ") : fix : " #desc)
#define chCHECKIT(desc) message(__FILE__ "(" chSTR2(__LINE__) ") : check : " #desc)
#define chNOTE(desc) message(__FILE__ "(" chSTR2(__LINE__) ") : note : " #desc)
//#define DoFixIt(desc) #pragma message(__FILE__ "(" chSTR2(__LINE__) "):" #desc)

//egs of use:
//#pragma chFIXIT("Remember to blah blah....")
//#pragma chMSG(NOTE: This is temp code)
//-------------------------------------------------------------------------
// Library Searches


#ifndef LIBCOMMENTHD
  #define LIBCOMMENTHD  ""
#endif

#if defined(_DEBUG)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "DBG" b ".lib")
#elif defined(_RELEASE)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "RLS" b ".lib")
#else
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "RDB" b ".lib")
#endif
