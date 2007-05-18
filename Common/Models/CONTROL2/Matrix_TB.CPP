//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>
#define __Matrix_TB_CPP
#include "sc_defs.h"
#include "Matrix_TB.h"
#include "Matrix_TBedt.h"
//#include "optoff.h"

// =========================================================================

IMPLEMENT_TAGOBJEDT(CMatrixTB, "MatrixTB", "MatrixTB", "", "", TOC_SYSTEM, CMatrixTBEdt, "", ""); 
//IMPLEMENT_TAGOBJ(CMatrixTB, "MatrixTB", "MatrixTB", "", "", "" ,TOC_SYSTEM,  "", ""); 


CMatrixTB::CMatrixTB(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FxdEdtBookRef(NULL),
  TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  Clear();
  m_bViewOnly = true;
  }

// -------------------------------------------------------------------------

CMatrixTB::CMatrixTB(  pchar Tag_,  pTaggedObject pAttach, TagObjAttachment eAttach , bool bviewonly ) :
  FxdEdtBookRef(NULL),
  TaggedObject(&CMatrixTBClass, Tag_, pAttach, eAttach)
  {
  Clear();
  m_bViewOnly = bviewonly;
  }

// -------------------------------------------------------------------------

CMatrixTB::CMatrixTB(CMatrixTB& Fn) :
  FxdEdtBookRef(NULL),
  TaggedObject(Fn.m_pClass, "", Fn.pAttachedTo, Fn.eAttachment)
  {
  Strng s = Fn.TagRef();
  TaggedObject::SetTag(s());

  }

// -------------------------------------------------------------------------
 
CMatrixTB::~CMatrixTB()
  {

  }
 
// -------------------------------------------------------------------------
 
void CMatrixTB::Clear()
  {
 
  }

// -------------------------------------------------------------------------

void CMatrixTB::SetPrefix(pchar rdesc, pchar cdesc )

  {

    for (int i=0;  i < RowCount(); i++)
      {
          Strng s;
         char temp[5];
         sprintf(temp,"%d",i);
         s = rdesc;
         s+=temp;
        if (i > (RowNames.GetCount()-1))
          {
            // Grow Row Names
            RowNames.SetSize(RowCount());
          }
        RowNames[i] = s; 
      }

    for (int j=0;  j < ColCount(); j++)
      {
        Strng s;
        s = cdesc;
        char temp[5];
        sprintf(temp,"%d",j);
        s+=temp;
       if (j > (ColNames.GetCount()-1))
          {
            // Grow Row Names
            ColNames.SetSize(ColCount());
          }

        ColNames[j] = s; 
      }

  }

// -------------------------------------------------------------------------

void CMatrixTB::SetRowPrefix(pchar rdesc  )

  {

    for (int i=0;  i < RowCount(); i++)
      {
         Strng s;
         char temp[5];
         sprintf(temp,"%d",i);
         s = rdesc;
         s+=temp;
        if (i > (RowNames.GetCount()-1))
          {
            // Grow Row Names
            RowNames.SetSize(RowCount());
          }
        RowNames[i] = s; 
      }

  }

// -------------------------------------------------------------------------

void CMatrixTB::SetColPrefix( pchar cdesc )

  {

    for (int j=0;  j < ColCount(); j++)
      {
        Strng s;
        s = cdesc;
        char temp[5];
        sprintf(temp,"%d",j);
        s+=temp;
       if (j > (ColNames.GetCount()-1))
          {
            // Grow Row Names
            ColNames.SetSize(ColCount());
          }

        ColNames[j] = s; 
      }

  }

// -------------------------------------------------------------------------

void CMatrixTB::SetRowName(long row , pchar name )

  {
  if (row > (RowNames.GetCount()-1))
    {
      // Grow Row Names
      RowNames.SetSize(RowCount());
    }
    RowNames[row] = name;
  }

// -------------------------------------------------------------------------

void CMatrixTB::SetColName(long col , pchar name )

  {
    if (col > (ColNames.GetCount()-1))
        {
          // Grow Row Names
          ColNames.SetSize(ColCount());
        }

    ColNames[col] = name;
  }

// -------------------------------------------------------------------------

void CMatrixTB::SetDim(int RowCount,int ColCount)

  {

    //
    // Change Size of Matrix
    //
    ((ZMat*)this)->SetDim(RowCount,ColCount);

    //
    // Change Size of Row and Coumn Description Strings
    //
    RowNames.SetSize(RowCount);
    ColNames.SetSize(ColCount);
  }

// -------------------------------------------------------------------------

const int Id_MatrixCol     =   500; // Reserved Through to 600 
                                    // i.e. Id_MatrixCol + Id_MatrixMaxCols
const int MatrixMaxCols    =   100; // Maximum columns that can be displayed
const int MatrixMaxRows    =   100; // Maximum rows that can be displayed

const int xID_Rows         = 1;
const int xID_Cols         = 2;
const int xID_Matrix       = 100;

void CMatrixTB::BuildDataDefn(DataDefnBlk & DDB)
  {

 
  DDB.BeginStruct(this);
  Strng S;

// if (!DDB.ForView() && !DDB.ForFiling() && !DDB.ForSnapShot())
    DDEF_Flags lflags = isParm;
    bool bBuild = false;

    if (m_bViewOnly && DDB.ForView())
      bBuild = true; // Matrix View only - can not edit. Only Build Data Defn conditionally
    else if (m_bViewOnly == false)
      bBuild = true; // Matrix is Editable. Build all the time
    else
      bBuild = false;


    if ( bBuild )
    { 

  DDB.Bool("ViewOnly", "",     DC_,    "", &m_bViewOnly  , this, lflags); 
  DDB.Long("Rows",     "",     DC_,    "", xID_Rows      , this, lflags); 
  DDB.Long("Cols",     "",     DC_,    "", xID_Cols      , this, lflags); 
  //DDB.Long("Rows",     "",     DC_,    "", xID_Rows  , this, DDEF_WRITEPROTECT|noFile|noSnap|DupHandlesOk); 
  //DDB.Long("Cols",     "",     DC_,    "", xID_Cols  , this, DDEF_WRITEPROTECT|noFile|noSnap|DupHandlesOk); 


  // Matrix Data Limit to 100x100 corner

  for (int j=0; j<min(ColCount(),MatrixMaxCols); j++)
  {

      // Column Description
      char temps[5];
      sprintf(temps,"CS%d",j);

      if ( j < ColNames.GetCount() )
        DDB.String(temps,  "", DC_, "",   &(ColNames[j]) ,  this, lflags );

  }

  DDB.BeginArray(this, "X", "XData", RowCount(), 0);
  for (int i=0; i<min(RowCount(),MatrixMaxRows); i++)
    {

    DDB.BeginElement(this, i); 

    // Row Description
    char temps[5];
    sprintf(temps,"RS%d",i);

    if ( i < RowNames.GetCount() )
      DDB.String(temps,  "", DC_, "",   &(RowNames[i]) ,  this, lflags );


    for (int j=0; j<min(ColCount(),MatrixMaxCols); j++)
      {

      // Row Data

      char temp[5];
      sprintf(temp,"C%d",j);
      ZMat* pm_X = this;
      double* daddr = &((*pm_X)[i][j]);
      //double* daddr = &(m_X[i][j]);
      //DDB.Double(temp,     "",     DC_,    "", daddr  ,      this, /*isParm*/isResult); 
      //DDB.Double(temp,     "",     DC_,    "", daddr  ,  this, DDEF_WRITEPROTECT|noFile|noSnap|DupHandlesOk); 
      //DDB.Double(temp,     "",     DC_,    "", xID_Matrix +i*MatrixMaxCols + j ,  this, isParm   ); 
      DDB.Double(temp,     "",     DC_,    "", daddr ,  this, lflags   ); 

      }

    }
  DDB.EndArray();

  }

  DDB.EndStruct();


  }

// -------------------------------------------------------------------------

flag CMatrixTB::DataXchg(DataChangeBlk & DCB)
  {

//
// Requests to set and read data via DataXchg
//

  if (TaggedObject::DataXchg(DCB)) 
    return 1;

  //
  // XChng Matrix Rows and Columns
  //
  switch(DCB.lHandle)
    {
    case xID_Rows:
      if (DCB.rL)
        SetDim(*DCB.rL,ColCount());
      DCB.L = RowCount();
     return(1);
    case xID_Cols:
     if (DCB.rL)
        SetDim(RowCount(),*DCB.rL);
      DCB.L = ColCount();
      return(1);
    }
 
    //
    // XChng Matrix Elements
    //

    if ((DCB.lHandle >= xID_Matrix && 
         DCB.lHandle <= (xID_Matrix+MatrixMaxCols*MatrixMaxRows)))
      {


 
          long offset = DCB.lHandle - xID_Matrix;
          long row = offset/MatrixMaxCols;
          long col = offset % MatrixMaxCols;



          ZMat* pm_X = this;

           if (DCB.rD)
             {

         if (row == 0 && col == 0 )
           {
              int testx = 1;
           }


                 (*pm_X)[row][col] = *DCB.rD; 
             }
           else
             {
         if (row == 0 && col == 0 )
           {
              int testy = 1;
           }

             }
              DCB.D = (*pm_X)[row][col]; 
           return(1);
      }

  return False;
  }

// -------------------------------------------------------------------------

void CMatrixTB::Build()
  {
#ifdef OLD_FORMAT
  pchar p=Tag();
 
  StartPage((strlen(p)>0) ? p : "Matrix");

  //
  // Matrix Example
  //

  // Start a New block with 2 + RowCount Entries - Row indexes are offset by 2 when referencing????
  StartBlk(2+RowCount(), 2, NULL);

  int L=0;
  //SetDesc(L, "Matrix Data--------------------------", -1, 24, 2, "");
  SetDesc(L, p , -1, 24, 2, "");
  L++;

  // Column Descriptions

  SetSpace(L,11);
  for (long col=0; col<ColCount() ; col++)
    {
    char tmp[80];
    if ( col < ColNames.GetCount() )
      sprintf(tmp, "%s", ColNames[col].Str() ,col);
    else
      sprintf(tmp, "C%d", col);

    SetDesc(L, tmp, -1 , 6, 1, "");
    }

  for (long row=0; row<RowCount(); row++)
    {

    // Row Description
    L++;
    char tmp[80];
    
    if ( row < RowNames.GetCount() )
      sprintf(tmp, "%s", RowNames[row].Str() , row);
    else
      sprintf(tmp , "R%d" , row );
    SetDesc(L, tmp, -1 , 9, 2, "");
    SetSpace(L,1);

    // Matrix Entries

    for (long col=0; col<ColCount() ; col++)
      {
      //SetParm(L,"", Id_MatrixCol+col, 6 , 2, "");
      if (m_bViewOnly)
        // Non Editable
        SetData(L,"", Id_MatrixCol+col, 6 , 2, "", false );
      else
        // Editable
        SetData(L,"", Id_MatrixCol+col, 6 , 2, "", true );
      }
    }
#else
  pchar p=Tag();
 
  StartPage((strlen(p)>0) ? p : "Matrix");

  //
  // Matrix Example
  //

  // Start a New block with 2 + RowCount Entries - Row indexes are offset by 2 when referencing????
  StartBlk(2+RowCount(), 2, NULL);

  int L=0;
  //SetDesc(L, "Matrix Data--------------------------", -1, 24, 2, "");
  SetDesc(L, p , -1, 24, 2, "");
  L++;

  // Column Descriptions

//RowNameWidth = 9;
//ColumnWidth = 6;
#define RowNameWidth 14
#define ColumnWidth  12

  SetSpace(L,RowNameWidth+2);
  for (long col=0; col<ColCount() ; col++)
    {
    char tmp[80];
    if ( col < ColNames.GetCount() )
      sprintf(tmp, "%s", ColNames[col].Str() ,col);
    else
      sprintf(tmp, "C%d", col);

    SetDesc(L, tmp, -1 , ColumnWidth , 1, "");
    }

  for (long row=0; row<RowCount(); row++)
    {

    // Row Description
    L++;
    char tmp[80];
    
    if ( row < RowNames.GetCount() )
      sprintf(tmp, "%s", RowNames[row].Str() , row);
    else
      sprintf(tmp , "R%d" , row );
    SetDesc(L, tmp, -1 , RowNameWidth , 2, "");
    SetSpace(L,1);

    // Matrix Entries

    for (long col=0; col<ColCount() ; col++)
      {

      if (m_bViewOnly)
        // Non Editable
        SetData(L,"", Id_MatrixCol+col, ColumnWidth , 2, "", false );
      else
        // Editable
        SetData(L,"", Id_MatrixCol+col, ColumnWidth , 2, "", true );
      }
    }
#endif
  }

//---------------------------------------------------------------------------

void CMatrixTB::Load(FxdEdtInfo &EI, Strng & Str)
  {

  if (CurrentBlk(EI))
    {
    int i=(int)(EI.BlkRowNo-EI.Index);

    if ((EI.FieldId >= Id_MatrixCol && EI.FieldId <= (Id_MatrixCol+MatrixMaxCols) ))
      {
        ZMat* pm_X = this;
        double dTemp = ((*pm_X)[i][EI.FieldId-Id_MatrixCol]);
        //double dTemp = m_X[i][EI.FieldId-Id_MatrixCol];
        //Str.Set("%2.1g", dTemp);
		Str.Set("%g", dTemp);
      }

    }

  }

//---------------------------------------------------------------------------

long CMatrixTB::Parse(FxdEdtInfo &EI, Strng & Str)
  {

  long Fix=0; //set Fix=1 to redraw graph

 if (CurrentBlk(EI))
    {
    int i=(int)(EI.BlkRowNo-EI.Index);

    if ((EI.FieldId >= Id_MatrixCol && EI.FieldId <= (Id_MatrixCol+MatrixMaxCols) ))
      {
        ZMat* pm_X = this;

        //m_X[i][EI.FieldId-Id_MatrixCol] = Str.SafeAtoF();
        (*pm_X)[i][EI.FieldId-Id_MatrixCol] = Str.SafeAtoF();
      }

    }

  return Fix;
  }

//---------------------------------------------------------------------------

long CMatrixTB::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  return 1;
  }

//==========================================================================
