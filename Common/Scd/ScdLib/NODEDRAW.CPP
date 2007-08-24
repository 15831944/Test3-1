//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"

#define  __NODEDRAW_CPP
#include "sc_defs.h"
#include "nodedraw.h"
//#include "tagobj.h"

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
  if (!pNGI)
    return;

  Strng DefSymbols;
  DefSymbols = PrjFiles();
  DefSymbols += "DefaultSymbols9.txt";
  Strng DefSymbols2;
  DefSymbols2 = PrjFiles();
  DefSymbols2 += "DefaultSymbols10.txt";

  FILE* f = fopen(DefSymbols(), "wt");
  if (!f)
    return;
  FILE* f2 = fopen(DefSymbols2(), "wt");
  if (!f2)
    return;

  NodeGrfInfo* pNGI2 = pNGI;

  char *pIOPt,*pVar,*pType;
  double XMn, XMx, YMn, YMx, X, Y, XRng, YRng, Xoffset, Yoffset, YMax2;
  #define DORANGE(X,Y) { XMn=Min(XMn,X);XMx=Max(XMx,X);YMn=Min(YMn,Y);YMx=Max(YMx,Y); };
  #define INITRANGE() { XMn=1e10; XMx=-XMn; YMn=XMn; YMx=XMx; XRng=YRng=0.0; };
  #define XFerX(X_) (X_+Xoffset)
  #define XFerY(Y_) (YMax2 - (Y_+Yoffset))
  long Pos1,Pos2;
  long Code2=-99;
  CString Symbol;
  //TagObjClass* pClass;

  bool FoundEnd = false;
  char Buff[4096];
  for (long Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        {
        INITRANGE();
        Symbol = pNGI->GetPChar();
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
        XRng = XMx-XMn;
        YRng = YMx-YMn;
        sprintf(Buff, "MDrw_End\n");
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        sprintf(Buff, "XMin,%g\nXMax,%g\nYMin,%g\nYMax,%g\n", XMn, XMx, YMn, YMx);
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        sprintf(Buff, "Width,%g\nHeight,%g\n\n", XRng, YRng);
        fwrite(Buff, sizeof(char), strlen(Buff), f);
        Pos1 = pNGI->Pos;

        //output file in alternate format
        Xoffset = 0.0 - XMn;
        Yoffset = 0.0 - YMn;
        YMax2 = YMx+Yoffset;
        FoundEnd = false;
        if (Code2==-99)
          Code2=pNGI2->GetLVal(TRUE);
        else
          pNGI2->Pos = Pos2;
        for (; Code2 != DD_EndList && !FoundEnd; Code2=pNGI2->GetLVal())
          {
          switch (Code2)
            {
            case DD_Name:
              {
              Symbol = pNGI2->GetPChar();
              CString DrwGrp;
              int iColon=Symbol.Find(':');
              if (iColon>0)
                {
                DrwGrp=Symbol.Left(iColon);
                Symbol=Symbol.Right(Symbol.GetLength()-iColon-1);
                }
              sprintf(Buff, "Symbol,%s\n", Symbol);
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              sprintf(Buff, "Group,%s\n", DrwGrp.GetLength()>0 ? DrwGrp : "");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
              }
            case DD_Poly:
              {
              strcpy(Buff, "MDrw_Poly, ");
              while (pNGI2->GetCoOrd(X,Y))
                {
                sprintf(Buff, "%s%g,%g, ", Buff, XFerX(X), XFerY(Y));
                }
              strcat(Buff, "\n");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
              }
            case DD_Arc:
              {
              strcpy(Buff, "MDrw_Arc, ");
              if (pNGI2->GetCoOrd(X,Y))
                {
                sprintf(Buff, "%s %g,%g, ", Buff, XFerX(X), XFerY(Y));
                double Rad=pNGI2->GetLength();
                sprintf(Buff, "%s%g, ", Buff, Rad);
                double S=0,E=360;
                pNGI2->GetPair(S,E); //???
                }
              strcat(Buff, "\n");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
              }
            case DD_Arc3:
              {
              strcpy(Buff, "MDrw_Arc3, ");
              while (pNGI2->GetCoOrd(X,Y))
                {
                sprintf(Buff, "%s%g,%g, ", Buff, XFerX(X), XFerY(Y));
                }
              strcat(Buff, "\n");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
              }
            case DD_TagPos:
              {
              strcpy(Buff, "MDrw_TagPos, ");
              if (pNGI2->GetCoOrd(X,Y))
                {
                sprintf(Buff, "%s%g,%g, ", Buff, XFerX(X), XFerY(Y));
                }
              strcat(Buff, "\n");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
              }
            case DD_ValuePos:
              {
              strcpy(Buff, "MDrw_ValuePos, ");
              if (pNGI2->GetCoOrd(X,Y))
                {
                sprintf(Buff, "%s%g,%g, ", Buff, XFerX(X), XFerY(Y));
                }
              strcat(Buff, "\n");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
              }
            case DD_Scale:
              sprintf(Buff, "Scale,%g\n", pNGI2->GetDVal()/1000.0);
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              break;
            case DD_In:
            case DD_Out:
              pIOPt=pNGI2->GetPChar();
              while (pNGI2->GetCoOrd(X,Y))
                {
                }
              break;
            case DD_Dynm:
              pVar  = pNGI2->GetAChar();
              pType = pNGI2->GetAChar();
              while (pNGI2->GetCoOrd(X,Y))
                {
                }
              break;
            case DD_End:
              {
              const double XMn2 = XFerX(XMn);
              const double XMx2 = XFerX(XMx);
              const double YMn2 = XFerY(YMx);
              const double YMx2 = XFerY(YMn);
              XRng = XMx2-XMn2;
              YRng = YMx2-YMn2;
              sprintf(Buff, "MDrw_End\n");
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              //todo: get the class and list all the available ports...
              /*TagObjClass* pClass = TagObjClass::FindClassName((LPSTR)(LPCSTR)Symbol);
              if (pClass)
                {
                RequestModelIOInfoArray Info;
                CFlwSolver::RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info);
                for each...
                }*/
              sprintf(Buff, "TextArea,%g,%g,%g,%g\n", XMn2, YMx2+0.5, XRng, 3.0);
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              sprintf(Buff, "XMin,%g\nXMax,%g\nYMin,%g\nYMax,%g\n", XMn2, XMx2, YMn2, YMx2);
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              sprintf(Buff, "Width,%g\nHeight,%g\n", XRng, YRng);
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              sprintf(Buff, "InsertX,%g\nInsertY,%g\n\n", XFerX(0), XFerY(0));
              fwrite(Buff, sizeof(char), strlen(Buff), f2);
              pNGI2->SetScale(1.0);
              FoundEnd = true;
              break;
              }
            default: ;
            }
          }
        Pos2 = pNGI2->Pos;

        pNGI->Pos = Pos1;
        INITRANGE();
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }
  fclose(f);
  fclose(f2);
  }

//===========================================================================
