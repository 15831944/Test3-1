//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ERRORLOG_H
#define  __ERRORLOG_H

#include "..\..\..\SMDK\Include\MsgLog.h"

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifdef __ERRORLOG_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


//===========================================================================

DllImportExport const char * FindWinError(DWORD Code);

//===========================================================================

//pkh#if !_MANAGED

class ConditionArray  : public CArray < byte, byte &> {};
class ConditionSeqNos : public CArray < DWORD, DWORD &> {};
class ConditionInfo
  {
  friend class ConditionBlk;
  private:
    ConditionArray  EA;
    ConditionSeqNos SeqNo;
    Strng_List     ErrTexts;
  };

class DllImportExport ConditionBlk
  {
  private:
    ConditionInfo *pInfo;
    static long    m_lSeqNo;
    static long    m_lSeqNoAtRunStart;

    byte           ErrMsk(int i) { return 1<<(i%4); };
    byte           TxtMsk(int i) { return 1<<((i%4)+4); };
    int            Index(int i) { return i/4;};
    int            MaxIndex() { return pInfo->EA.GetSize()*4;};
    char *         GetTxtInfo(int No, flag SetIt, dword &Flag);
    void           DoLog(int No, flag SetIt, long SeqNo);
    char *         GetCITxt(int No);
    flag           DoSetCICntxt(int No, int Context);

  public:

    ConditionBlk();
    virtual ~ConditionBlk();

    void           ClrAllCIs();
    flag           SetCICntxt(int No, int Context, bool WithLog);
    flag           ClrCICntxt(int No, int Context, bool WithLog);
    flag           SetCICntxt(int No, int Context, bool WithLog, char * S);

    flag           TestCISetCntxt(int No, int Context);

    virtual char * CBTag();
    int            CBContext() { return 0;};
    virtual void   CBChanged() {};

    virtual flag   CIStrng(int No, pchar & pS);
    int            CICount()   { return 0;};

    flag           GetMyCIs(Strng_List & L, int MaxNo);
    int            CountMyCIs(int MaxNo=1, dword RqdFlags=0);
    virtual flag   GetAllCIs(Strng_List & L, int MaxNo=100);
    virtual int    CountAllCIs(int MaxNo=1, dword RqdFlags=0);

    static void    SetSeqNoAtRunStart() { m_lSeqNoAtRunStart=m_lSeqNo; };

    // These are used to Transfer to the top of a TaggedObject Class Heirarchy to get the context correct
    virtual flag   SetCIVirt(int No)                      { DoBreak(); return 0; };
    virtual flag   ClrCIVirt(int No)                      { DoBreak(); return 0; };
    virtual flag   SetCIVirt(int No, char * Fmt, ...)     { DoBreak(); return 0; };
    virtual flag   SetCIVirt(int No, flag State)          { DoBreak(); return 0; };
    virtual flag   ClrCIVirt(int No, flag State)          { DoBreak(); return 0; };
    virtual int    CBContextVirt()                        { DoBreak(); return 0; };
  };


#define DEFINE_CI(Class, BaseClass, MaxMsgs)                                         \
virtual flag   CIStrng(int No, pchar & S);                                           \
int            CICount()   { return MaxMsgs;};                                       \
int            CBContext() { return BaseClass::CBContext()+BaseClass::CICount();};   \
                                                                                     \
flag SetCI(int No)                                                                   \
  {                                                                                  \
  return ConditionBlk::SetCICntxt(No, Class::CBContext(), true);                     \
  };                                                                                 \
flag ClrCI(int No)                                                                   \
  {                                                                                  \
  return ConditionBlk::ClrCICntxt(No, Class::CBContext(), true);                     \
  };                                                                                 \
flag SetCI(int No, char * Fmt, ...)                                                  \
  {                                                                                  \
  char S[512];                                                                       \
  va_list argptr;                                                                    \
  va_start(argptr,Fmt);                                                              \
  vsprintf(S, Fmt, argptr);                                                          \
  va_end(argptr);                                                                    \
  return ConditionBlk::SetCICntxt(No, Class::CBContext(), true, S);                  \
  };                                                                                 \
flag SetCI(int No, flag State)                                                       \
  {                                                                                  \
  if (State)                                                                         \
    return ConditionBlk::SetCICntxt(No, Class::CBContext(), true);                   \
  else                                                                               \
    return ConditionBlk::ClrCICntxt(No, Class::CBContext(), true);                   \
  };                                                                                 \
flag ClrCI(int No, flag State)                                                       \
  {                                                                                  \
  if (!State)                                                                        \
    return ConditionBlk::SetCICntxt(No, Class::CBContext(), true);                   \
  else                                                                               \
    return ConditionBlk::ClrCICntxt(No, Class::CBContext(), true);                   \
  };                                                                                 \
flag SetCINoLog(int No)                                                              \
  {                                                                                  \
  return ConditionBlk::SetCICntxt(No, Class::CBContext(), false);                    \
  };                                                                                 \
flag ClrCINoLog(int No)                                                              \
  {                                                                                  \
  return ConditionBlk::ClrCICntxt(No, Class::CBContext(), false);                    \
  };                                                                                 \
flag SetCINoLog(int No, char * Fmt, ...)                                             \
  {                                                                                  \
  char S[512];                                                                       \
  va_list argptr;                                                                    \
  va_start(argptr,Fmt);                                                              \
  vsprintf(S, Fmt, argptr);                                                          \
  va_end(argptr);                                                                    \
  return ConditionBlk::SetCICntxt(No, Class::CBContext(), false, S);                 \
  };                                                                                 \
flag SetCINoLog(int No, flag State)                                                  \
  {                                                                                  \
  if (State)                                                                         \
    return ConditionBlk::SetCICntxt(No, Class::CBContext(), false);                  \
  else                                                                               \
    return ConditionBlk::ClrCICntxt(No, Class::CBContext(), false);                  \
  };                                                                                 \
flag ClrCINoLog(int No, flag State)                                                  \
  {                                                                                  \
  if (!State)                                                                        \
    return ConditionBlk::SetCICntxt(No, Class::CBContext(), false);                  \
  else                                                                               \
    return ConditionBlk::ClrCICntxt(No, Class::CBContext(), false);                  \
  };                                                                                 \
flag TestCISet(int No)                                                               \
  {                                                                                  \
  return ConditionBlk::TestCISetCntxt(No, Class::CBContext());                       \
  };                                                                                 \
flag TestCIClr(int No)                                                               \
  {                                                                                  \
  return !ConditionBlk::TestCISetCntxt(No, Class::CBContext());                      \
  };


#define DEFINE_CIVIRT(Class, BaseClass)                                             \
virtual flag SetCIVirt(int No)                      { return SetCI(No); };          \
virtual flag ClrCIVirt(int No)                      { return ClrCI(No); };          \
virtual flag SetCIVirt(int No, char * Fmt, ...)                                     \
  {                                                                                 \
  char S[512];                                                                      \
  va_list argptr;                                                                   \
  va_start(argptr,Fmt);                                                             \
  vsprintf(S, Fmt, argptr);                                                         \
  va_end(argptr);                                                                   \
  return SetCI(No, "%s", S);                                                        \
  };                                                                                \
virtual flag SetCIVirt(int No, flag State)          { return SetCI(No, State); };   \
virtual flag ClrCIVirt(int No, flag State)          { return ClrCI(No, State); };   \
virtual int  CBContextVirt()                        { return CBContext();      };

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CScdException
{
protected:
	long m_ErrNo;
  char* m_pszMessage;

  CScdException();
public:
	CScdException(long i_ErrNo,  char*i_pszMessage );
	virtual ~CScdException();

  long ErrNo() { return m_ErrNo;};
  char* ErrStr() { return m_pszMessage ? m_pszMessage : "";};
};

//pkh#endif

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif



