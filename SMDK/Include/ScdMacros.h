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

#if _MSC_VER>=1500

#if defined(DISTRIBUTION)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Distribution9" b ".lib")
#elif defined(_DEBUG)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Dbg9" b ".lib")
#elif defined(_RELEASE)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Rls9" b ".lib")
#else
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "RDb9" b ".lib")
#endif

#elif _MSC_VER>=1400

#if defined(DISTRIBUTION)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Distribution8" b ".lib")
#elif defined(_DEBUG)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Dbg8" b ".lib")
#elif defined(_RELEASE)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Rls8" b ".lib")
#else
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "RDb8" b ".lib")
#endif

#else

#if defined(DISTRIBUTION)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Distribution" b ".lib")
#elif defined(_DEBUG)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Dbg" b ".lib")
#elif defined(_RELEASE)
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "Rls" b ".lib")
#else
#define LIBCOMMENT(a,b) comment( lib, LIBCOMMENTHD a "RDb" b ".lib")
#endif

#endif