//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"

#define  __NODEDRAW_CPP
#include "sc_defs.h"
#include "nodedraw.h"

//===========================================================================

#define SpaceForName(Nm) ((strlen((char *)(Nm))/sizeof(double))+1)
    
//---------------------------------------------------------------------------

NodeGrfInfo::NodeGrfInfo()
  {
  Len=0;
  Pos=0;
  Buff=NULL;
  Scl=1.0;
  }

//---------------------------------------------------------------------------

NodeGrfInfo::~NodeGrfInfo()
  {
  if (Buff)
    delete []Buff;
  }

//---------------------------------------------------------------------------

/**
void NodeGrfInfo::TstScl()
  {
  if (Buff[Pos]<= DD_CodeHi)
    return ;
  if (Buff[Pos]==DD_Scale)
    {
    Pos++;
    Scl=Buff[Pos++]/1000.0;
    }
  };
**/

//---------------------------------------------------------------------------

long NodeGrfInfo::GetLVal(flag Start)
  {
  if (Start)
    Pos=0;
  //TstScl();
  return (long)Buff[Pos++];
  };

//---------------------------------------------------------------------------

double NodeGrfInfo::GetDVal(flag Start)
  {
  if (Start)
    Pos=0;
  //TstScl();
  return Buff[Pos++];
  };

//---------------------------------------------------------------------------

double NodeGrfInfo::GetLength()
  {
  //TstScl();
  return GetDVal()*Scl;
  };

//---------------------------------------------------------------------------

flag NodeGrfInfo::GetPair(double &I, double &J)
  {
  if (Buff[Pos]<= DD_CodeHi)
    return False;
  //TstScl();
  I=GetDVal();
  J=GetDVal();
  return True;
  };

//---------------------------------------------------------------------------

flag NodeGrfInfo::GetCoOrd(double &X, double &Y)
  {
  if (Buff[Pos]<= DD_CodeHi)
    return False;
  //TstScl();
  X=GetDVal()*Scl;
  Y=GetDVal()*Scl;
  return True;
  };

//---------------------------------------------------------------------------

char * NodeGrfInfo::GetPChar(flag Start)
  {
  if (Start)
    Pos=0;
  //char * p=(char *)Buff[Pos];
  //Pos += 1;
  //TstScl();
  char * p=(char *)&Buff[Pos];
  Pos+=SpaceForName(p);
  return p;
  };


char * NodeGrfInfo::GetAChar(flag Start)
  {
  if (Start)
    Pos=0;
  //TstScl();
  char * p=(char *)(long)Buff[Pos];
  Pos += 1;
  return p;
  };



//---------------------------------------------------------------------------

void NodeGrfInfo::PutValue(double l)
  {
  if (Pos+1>Len)
    {
    double *b=new double [Len+1024];
    if (Buff)
      {
      for (int i=0; i<Pos; i++)
        b[i]=Buff[i];
      delete []Buff;
      }
    Len+=1024;
    Buff=b;
    }
  Buff[Pos]=l;
  Pos+=1;
  };

//---------------------------------------------------------------------------

void NodeGrfInfo::PutPChar(char * p)
  {
  long l=SpaceForName(p);
  if (Pos+l>Len)
    {
    double * b=new double[Len+1024];
    if (Buff)
      {
      for (int i=0; i<Pos; i++)
        b[i]=Buff[i];
      delete []Buff;
      }
    Len+=1024;
    Buff=b;
    }
  strcpy((char*)&Buff[Pos], p);
  Pos+=l;
  };

//===========================================================================
