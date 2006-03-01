//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1995
#ifndef __PGM_I_H
#define __PGM_I_H

//===========================================================================
/* All the classes derived from  GCClassVar are used for encapsulating a C++
   class in a PGM class.  The derived class is instantiated once and is used 
   as a class reference by each PGM.  PGM instances of the class are always 
   of type GCClassVar (BAD, BAD boy this is not what one would expect and 
   hopefully next time I debug, I won't waste 2 hours working this out yet
   again!) which contain a pointer to the reference class.  The PGM class
   instances then call the virtual functions of the reference class to do
   the work. I know, this could have been done properly with an appropriate
   virtual constructor or something! */

//---------------------------------------------------------------------------

class GCQuadEqn : public GCClassVar
  {
  public:
    GCQuadEqn(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };

//---------------------------------------------------------------------------

class GCCircEqn : public GCClassVar
  {
  public:
    GCCircEqn(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };

//---------------------------------------------------------------------------

class GCPartEqn : public GCClassVar
  {
  public:
    GCPartEqn(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };

//---------------------------------------------------------------------------

class GCAveEqn : public GCClassVar
  {
  public:
    GCAveEqn(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };

//============================================================================

#endif
