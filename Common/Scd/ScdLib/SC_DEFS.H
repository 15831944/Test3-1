//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*  sc_defs.h

    Common definitions for pointer size and calling conventions.
*/

//#pragma  check_stack on

#ifndef  __SC_DEFS_H
#define  __SC_DEFS_H

#include <memory.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#pragma warning (default : 4701) // used before initial - disabled in AFX.h
#pragma warning (disable : 4355) // this used in base member initializer list
#pragma warning (disable : 4041) // compiler limit : terminating browser output
//#pragma warning (disable : 4237) // keyword reserved for future use : eg flag

#pragma warning (3 : 4700 4756)

#define DllExport __declspec( dllexport )
#define DllImport __declspec( dllimport )

// global debug
#ifdef _RELEASE
#define WITHTIMERS            0
#define WITHDEBUG             0
#else
#define WITHTIMERS            01
#define WITHDEBUG             01
#endif


//#include "mathlib.h"

// find out why this does not work properly
//#pragma pack(4) // default packing 4 byte boundaries

#define  ASSIGN_STRUCT(d, s) memcpy(&d, &s, sizeof(d))
#define  ASSIGN_ARRAY(d, s, l) memcpy(d, s, sizeof(l))

//#define TEMPLATES

#if defined( __cplusplus )
#  define _PDEF(name) typedef name * p##name;
#  define _RDEF(name) typedef name & r##name;
#  define _RPDEF(name) typedef name *& rp##name;
#  define _FWDDEF(name) class name; \
    _PDEF(name) \
    _RDEF(name) \
    _RPDEF(name)
#  define _HUGEFWDDEF(name) class huge name; \
    _PDEF(name) \
    _RDEF(name) \
    _RPDEF(name)
#endif

//-------------------------------------------------------------------------

#include "..\..\..\SMDK\Include\scdmacros.h"

//-------------------------------------------------------------------------

typedef unsigned short         word;
typedef unsigned int           uint;
typedef unsigned long          dword;
typedef unsigned char          byte;
//typedef unsigned char          byte;
typedef signed   char          sint;
typedef unsigned char          flag;
typedef void                  *pvoid;

_PDEF(char)
_PDEF(byte)
_PDEF(short)
_PDEF(int)
_PDEF(long)
_PDEF(word)
_PDEF(dword)
_PDEF(float)
_PDEF(double)
_PDEF(flag)

_RDEF(char)
_RDEF(byte)
_RDEF(short)
_RDEF(int)
_RDEF(long)
_RDEF(word)
_RDEF(dword)
_RDEF(float)
_RDEF(double)
_RDEF(flag)

typedef pchar* ppchar;

const flag False=0;
const flag True=1;

// ========================================================================
//
//
//
// ========================================================================

#define SgnStr(i) ((i)>=0?((i)>0?"+":"."):"-")

// ========================================================================

//#if !_MANAGED
#include "mathlib.h"
//#endif

// ========================================================================

//inline void ExchangePtrs(pvoid *a, pvoid *b) {pvoid c=*a; *a=*b; *b=c;};
//inline void Exchange(double &a, double &b) {double c=a; a=b; b=c;};
//inline void Exchange(float &a, float &b) {float c=a; a=b; b=c;};
//inline void Exchange(long &a, long &b) {long c=a; a=b; b=c;};
//inline void Exchange(int &a, int &b) {int c=a; a=b; b=c;};
//inline void Exchange(short &a, short &b) {short c=a; a=b; b=c;};
//inline void Exchange(char &a, char &b) {char c=a; a=b; b=c;};
//inline void Exchange(byte &a, byte &b) {byte c=a; a=b; b=c;};
//#define EXCHANGE(t,a,b) {t c=a; a=b; b=c;}
//template <class T> void Exchange(T&a, T&b) {T c=a; a=b; b=c;}; // moved to md_share1.h

// ========================================================================
//
//  Structured Exception Handling
//
// ========================================================================

#if !_MANAGED

#include <eh.h>
inline void XcpTranslateFunction( unsigned int u, EXCEPTION_POINTERS* pExp )
  {
  LPCTSTR pStrS=NULL, pStrF=NULL;
  switch (u)
    {
    case EXCEPTION_ACCESS_VIOLATION:         pStrS="ACCESS_VIOLATION";         break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    pStrS="ARRAY_BOUNDS_EXCEEDED";    break;
    case EXCEPTION_BREAKPOINT:               pStrS="BREAKPOINT";               break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:    pStrS="DATATYPE_MISALIGNMENT";    break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:     pStrF="FLT_DENORMAL_OPERAND";     break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       pStrF="FLT_DIVIDE_BY_ZERO";       break;
    case EXCEPTION_FLT_INEXACT_RESULT:       pStrF="FLT_INEXACT_RESULT";       break;
    case EXCEPTION_FLT_INVALID_OPERATION:    pStrF="FLT_INVALID_OPERATION";    break;
    case EXCEPTION_FLT_OVERFLOW:             pStrF="FLT_OVERFLOW";             break;
    case EXCEPTION_FLT_STACK_CHECK:          pStrF="FLT_STACK_CHECK";          break;
    case EXCEPTION_FLT_UNDERFLOW:            pStrF="FLT_UNDERFLOW";            break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:      pStrS="ILLEGAL_INSTRUCTION";      break;
    case EXCEPTION_IN_PAGE_ERROR:            pStrS="IN_PAGE_ERROR";            break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       pStrS="INT_DIVIDE_BY_ZERO";       break;
    case EXCEPTION_INT_OVERFLOW:             pStrS="INT_OVERFLOW";             break;
    case EXCEPTION_INVALID_DISPOSITION:      pStrS="INVALID_DISPOSITION";      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: pStrS="NONCONTINUABLE_EXCEPTION"; break;
    case EXCEPTION_PRIV_INSTRUCTION:         pStrS="PRIV_INSTRUCTION";         break;
    case EXCEPTION_SINGLE_STEP:              pStrS="SINGLE_STEP";              break;
    case EXCEPTION_STACK_OVERFLOW:           pStrS="STACK_OVERFLOW";           break;
    default: pStrS="Unknown"; break;
    }
  if (pStrS)
    throw MSysException(u, pExp->ExceptionRecord->ExceptionAddress, pStrS);
  else
    throw MFPPException(u, pExp->ExceptionRecord->ExceptionAddress, pStrF);
  }

#define SCDTRY                                                      \
  /* Get Old fpControl */                                           \
  _set_se_translator(XcpTranslateFunction);                         \
  DWORD FPP_State=_controlfp(0,0);                                  \
  try                                                               \
    {                                                               \
    /* clear any outstanding exceptions */                          \
    _clearfp();                                                     \
    /* Set New fpControl */                                         \
    _controlfp(_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);


#define SCDCATCH(Where)                                             \
    FPP_RestoreExceptions(FPP_State);                               \
    }                                                               \
  catch (MSysException e)                                           \
    {                                                               \
    /* clear any outstanding exceptions */                          \
    _clearfp();                                                     \
    /*XGotoEditMode();*/                                            \
                                                                    \
    Strng S;                                                        \
    S.Set(" %s \n\n"                                                \
          "Exception %s (%x) occurred\n\n"                          \
          "Address: %08x\n\n"                                       \
          "Thread terminating\n\n"                                  \
          "Push OK to exit SysCAD",                                 \
      Where, e.Name, e.Code, e.Address);                            \
                                                                    \
    Strng *pS=new Strng(S);                                         \
    CWnd *pWnd=AfxGetMainWnd();                                     \
    bool Done=false;                                                \
    pWnd->PostMessageA(WMU_EXCEPTION, (WPARAM)pS, (LPARAM)&Done);   \
                                                                    \
    /*gs_License.Exit(); NBNB we need this!!!*/                     \
                                                                    \
    while (!Done)                                                   \
      Sleep(100);                                                   \
                                                                    \
    throw;                                                          \
    }


#define SCDCATCH1(Where)                                            \
    FPP_RestoreExceptions(FPP_State);                               \
    }                                                               \
  catch (MSysException e)                                           \
    {

#define SCDCATCH2(Where)                                            \
    /* clear any outstanding exceptions */                          \
    _clearfp();                                                     \
    /*XGotoEditMode();*/                                            \
                                                                    \
    Strng S;                                                        \
    S.Set(" %s \n\n"                                                \
          "Exception %s (%x) occurred\n\n"                          \
          "Address: %08x\n\n"                                       \
          "Thread terminating\n\n"                                  \
          "Push OK to exit SysCAD",                                 \
      Where, e.Name, e.Code, e.Address);                            \
                                                                    \
    Strng *pS=new Strng(S);                                         \
    CWnd *pWnd=AfxGetMainWnd();                                     \
    bool Done=false;                                                \
    pWnd->PostMessageA(WMU_EXCEPTION, (WPARAM)pS, (LPARAM)&Done);   \
                                                                    \
    /*gs_License.Exit(); NBNB we need this!!!*/                     \
                                                                    \
    while (!Done)                                                   \
      Sleep(100);                                                   \
                                                                    \
    throw;                                                          \
    }

// ===========================================================================
//  . Net stuff
// ===========================================================================

//#pragma warning(disable : 4995) // .Net deprecated
//#pragma warning(disable : 4996) // .Net deprecated
//#pragma warning(disable : 4244) // Loss of data
#endif

#if _MSC_VER>= 1300
#define DotNet 1
#define DotNetAfxTempl 1
#pragma warning (disable : 4995) // deprecated
#else
#define DotNet 0
#define DotNetAfxTempl 0
#endif


//========================================
//      Smart Pointer template definition
//========================================

template <class TYPE> class CSmartPtr
  {
  public:
    CSmartPtr()
      {
      m_pI = NULL;
      }

    ~CSmartPtr()
      {
      Release();
      }

    void  Release()
      {
      if(NULL != m_pI)
        {
        //  Make sure to clear the local variable so it does not come
        //  here again on the destructor if explicitly released.
        //TYPE * pTemp = m_pI;
        //pTemp->~TYPE();
        m_pI = NULL;
        }
      }

    //    Operators
    TYPE * operator->() const { return m_pI; }
    TYPE & operator*()  const { return *m_pI; }
    TYPE * operator&()  const { return m_pI; }
    TYPE * operator=(TYPE* X) { m_pI=X; return m_pI; }
    bool operator==(TYPE* X) const { return m_pI==X; }
    bool IsNull() const { return m_pI==NULL; }

    operator CSmartPtr<TYPE>()  { return *this; }
    operator TYPE*()  { return m_pI; }
    //operator TYPE*[](int i)  { return this; }

  protected:
    TYPE *   m_pI;
  };
template <class TYPE> class CSmartPtrAllocate
  {
  public:
    CSmartPtrAllocate()
      {
      m_pI = NULL;
      Alloc();
      }

    ~CSmartPtrAllocate()
      {
      Release();
      }

    void  Alloc()
      {
      if(NULL == m_pI)
        {
        m_pI = new TYPE;
        }
      }
    void  Release()
      {
      if(NULL != m_pI)
        {
        //  Make sure to clear the local variable so it does not come
        //  here again on the destructor if explicitly released.
        TYPE * pTemp = m_pI;
        m_pI = NULL;
        delete pTemp;
        }
      }

    //    Operators
    TYPE * operator->() const { return m_pI; }
    TYPE & operator*()  const { return *m_pI; }
    TYPE * operator&()  const { return m_pI; }
    TYPE * operator=(const TYPE* X) { m_pI=X; return m_pI; }
    bool operator==(const TYPE* X) const { return m_pI==X; }
    bool IsNull() const { return m_pI==NULL; }

    operator CSmartPtrAllocate<TYPE>()  { return *this; }
    operator TYPE*()  { return m_pI; }
    operator TYPE&()  { return *m_pI; }

  protected:
    TYPE *   m_pI;

  };
//template <class TYPE> class CSmartPtrAllocFree
//  {
//  public:
//    CSmartPtrAllocFree()
//      {
//      m_pI = NULL;
//      Alloc();
//      }
//
//    ~CSmartPtrAllocFree()
//      {
//      Release();
//      }
//
//    void  Alloc()
//      {
//      if(NULL == m_pI)
//        {
//        m_pI = new TYPE;
//        }
//      }
//    void  Release()
//      {
//      if(NULL != m_pI)
//        {
//        //  Make sure to clear the local variable so it does not come
//        //  here again on the destructor if explicitly released.
//        TYPE * pTemp = m_pI;
//        m_pI = NULL;
//        delete pTemp;
//        }
//      }
//
//    //    Operators
//    TYPE * operator->() const { return m_pI; }
//    TYPE & operator*()  const { return *m_pI; }
//    TYPE * operator&()  const { return m_pI; }
//    TYPE * operator=(const TYPE* X) { m_pI=X; return m_pI; }
//    bool operator==(const TYPE* X) const { return m_pI==X; }
//    bool IsNull() const { return m_pI==NULL; }
//
//    operator CSmartPtrAllocFree<TYPE>()  { return *this; }
//    operator TYPE*()  { return m_pI; }
//    operator TYPE&()  { return *m_pI; }
//
//  protected:
//    TYPE *   m_pI;
//
//  };

template <class TYPE> class CSmartPtrReleaseOnly
  {
  public:
    CSmartPtrReleaseOnly()
      {
      m_pI = NULL;
      }

    ~CSmartPtrReleaseOnly()
      {
      Release();
      }

    void  Alloc()
      {
      if(NULL == m_pI)
        {
        m_pI = new TYPE;
        }
      }
    void  Release()
      {
      if(NULL != m_pI)
        {
        //  Make sure to clear the local variable so it does not come
        //  here again on the destructor if explicitly released.
        TYPE * pTemp = m_pI;
        m_pI = NULL;
        delete pTemp;
        }
      }

    //    Operators
    TYPE * operator->() const { return m_pI; }
    TYPE & operator*()  const { return *m_pI; }
    TYPE * operator&()  const { return m_pI; }
    TYPE * operator=(TYPE* X) { m_pI=X; return m_pI; }
    bool operator==(TYPE* X) const { return m_pI==X; }
    bool IsNull() const { return m_pI==NULL; }

    operator CSmartPtrReleaseOnly<TYPE>()  { return *this; }
    operator TYPE*()  { return m_pI; }
    operator TYPE&()  { return *m_pI; }

  template<class CC> operator CC*() { return dynamic_cast<CC*>(m_pI); };
  //template<class CC> operator CC&() { return *dynamic_cast<CC*>(m_pI); };

  protected:
    TYPE *   m_pI;

  };

// ===========================================================================

#include "dbgetc.h"
#include "gpfuncs.h"

// ===========================================================================

#endif  /* __SC_DEFS_H */

