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

//---------------------------------------------------------------------------

void NodeGrfInfo::ExportToFile(NodeGrfInfo* pNGI, char* TextFilename)
  {
  if (!pNGI || TextFilename==NULL)
    return;

  FILE* f = fopen(TextFilename, "wt");
  if (!f)
    return;

  char *pIOPt,*pVar,*pType;
  double XMn, XMx, YMn, YMx, X, Y;
  #define DORANGE(X,Y) { XMn=Min(XMn,X);XMx=Max(XMx,X);YMn=Min(YMn,Y);YMx=Max(YMx,Y); };
  #define INITRANGE() { XMn=1e10; XMx=-XMn; YMn=XMn; YMx=XMx; };

  char Buff[4096];
  for (long Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        {
        INITRANGE();
        CString Symbol(pNGI->GetPChar());
        CString DrwGrp;
        int iColon=Symbol.Find(':');
        if (iColon>0)
          {
          DrwGrp=Symbol.Left(iColon);
          Symbol=Symbol.Right(Symbol.GetLength()-iColon-1);
          }
        sprintf(Buff, "Symbol,%s\n", Symbol);
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        sprintf(Buff, "Group,%s\n", DrwGrp.GetLength()>0 ? DrwGrp : "");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
        }
      case DD_Poly:
        {
        strcpy(Buff, "MDrw_Poly, ");
        while (pNGI->GetCoOrd(X,Y))
          {
          sprintf(Buff, "%s%g,%g, ", Buff, X, Y);
          DORANGE(X,Y);
          }
        strcat(Buff, "\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
        }
      case DD_Arc:
        {
        strcpy(Buff, "MDrw_Arc, ");
        if (pNGI->GetCoOrd(X,Y))
          {
          sprintf(Buff, "%s %g,%g, ", Buff, X, Y);
          double Rad=pNGI->GetLength();
          sprintf(Buff, "%s%g, ", Buff, Rad);
          DORANGE(X+Rad,Y+Rad);
          DORANGE(X-Rad,Y-Rad);
          double S=0,E=360;
          pNGI->GetPair(S,E); //???
          }
        strcat(Buff, "\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
        }
      case DD_Arc3:
        {
        strcpy(Buff, "MDrw_Arc3, ");
        while (pNGI->GetCoOrd(X,Y))
          {
          sprintf(Buff, "%s%g,%g, ", Buff, X, Y);
          DORANGE(X,Y);
          }
        strcat(Buff, "\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
        }
      case DD_TagPos:
        {
        strcpy(Buff, "MDrw_TagPos, ");
        double X,Y;
        if (pNGI->GetCoOrd(X,Y))
          {
          sprintf(Buff, "%s%g,%g, ", Buff, X, Y);
          //int iJust=pNGI->GetLVal();
          //int iVisible=pNGI->GetLVal();
          //DORANGE(X,Y); //exclude tag position for preview
          }
        strcat(Buff, "\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
        }
      case DD_ValuePos:
        {
        strcpy(Buff, "MDrw_ValuePos, ");
        double X,Y;
        if (pNGI->GetCoOrd(X,Y))
          {
          sprintf(Buff, "%s%g,%g, ", Buff, X, Y);
          //int iJust=pNGI->GetLVal();
          //int iVisible=pNGI->GetLVal();
          //DORANGE(X,Y); //exclude tag position for preview
          }
        strcat(Buff, "\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
        }
      case DD_Scale:
        sprintf(Buff, "Scale,%g\n", pNGI->GetDVal()/1000.0);
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        break;
      case DD_In:
      case DD_Out:
        pIOPt=pNGI->GetPChar();
        while (pNGI->GetCoOrd(X,Y))
          DORANGE(X,Y);
        break;
      case DD_Dynm:
        pVar  = pNGI->GetAChar();
        pType = pNGI->GetAChar();
        while (pNGI->GetCoOrd(X,Y))
          DORANGE(X,Y);
        break;
      case DD_End:
        sprintf(Buff, "MDrw_End\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        sprintf(Buff, "XMin,%g\nXMax,%g\nYMin,%g\nYMax,%g\n\n", XMn, XMx, YMn, YMx);
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        INITRANGE();
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }
  fclose(f);
  }

//===========================================================================
