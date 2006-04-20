//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>
#define __PSD_TB_CPP
#include "sc_defs.h"
#include "PSD_TB.h"
//#include "PSD_TBedt.h"
//#include "optoff.h"

// =========================================================================
//
// 
//

const int FieldWidth = 8;
const int FieldSpace = 2;

//
// Limits
//
const int MaxPSDTestSpecies = 64;
const int MaxTestData       = 32;

//
// Test Data
//
static double TestSizes[MaxPSDTestSpecies];
static double TestCum[MaxPSDTestSpecies];

//
// EditView IDs
//

const int Id_NumSpecies =   1;
const int Id_NumData    =   Id_NumSpecies + 1;
const int Id_Size       =   Id_NumData    + MaxPSDTestSpecies;
const int Id_Frac       =   Id_Size       + MaxPSDTestSpecies*MaxTestData;
const int Id_Cum        =   Id_Frac       + MaxPSDTestSpecies*MaxTestData;
const int Id_DataType   =   Id_Cum        + MaxPSDTestSpecies*MaxTestData;
const int Id_EntryType  =   Id_DataType   + MaxPSDTestSpecies;
const int Id_FuncType   =   Id_EntryType  + MaxPSDTestSpecies;
const int Id_TestSize   =   Id_FuncType   + MaxPSDTestSpecies;
const int Id_TestCum    =   Id_TestSize   + MaxPSDTestSpecies;
const int Id_FuncRR_a   =   Id_TestCum    + MaxPSDTestSpecies;
const int Id_FuncRR_b   =   Id_FuncRR_a    + MaxPSDTestSpecies;
const int Id_FuncGS_k   =   Id_FuncRR_b    + MaxPSDTestSpecies;
const int Id_FuncGS_m   =   Id_FuncGS_k   + MaxPSDTestSpecies;
//const int Id_SetButton  =   Id_TestCum + MaxPSDTestSpecies;

//
// Data XChng Ids
//

const int xID_NSpecies     = 1;
const int xID_Name         = xID_NSpecies   + 1;
const int xID_NData        = xID_Name       + MaxPSDTestSpecies;
const int xID_SzData       = xID_NData      + MaxPSDTestSpecies;
const int xID_FracData     = xID_SzData     + MaxPSDTestSpecies*MaxTestData;
const int xID_CumFracData  = xID_FracData   + MaxPSDTestSpecies*MaxTestData;
const int xID_DataType     = xID_CumFracData+ MaxPSDTestSpecies*MaxTestData;
const int xID_EntryType    = xID_DataType   + MaxPSDTestSpecies;
const int xID_FuncType     = xID_EntryType  + MaxPSDTestSpecies;
const int xID_FuncRR_a     = xID_FuncType   + MaxPSDTestSpecies;
const int xID_FuncRR_b     = xID_FuncRR_a   + MaxPSDTestSpecies;
const int xID_FuncGS_k     = xID_FuncRR_b   + MaxPSDTestSpecies;
const int xID_FuncGS_m     = xID_FuncGS_k   + MaxPSDTestSpecies;

// =========================================================================

//IMPLEMENT_TAGOBJEDT     (CPSDTB, "PSDTB", "PSDTB", "", "", TOC_SYSTEM, CPSDTBEdt, "", ""); 
/*
IMPLEMENT_TAGOBJEDTDERIVED(CPSDTB, "PSDTB", "PSDTB", "", "", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST, CPSDTBEdt, 
                 "Test Data",
                 "Test Data Entry Tab Page",
                 BaseClass);
*/

//IMPLEMENT_TAGOBJ(CPSDTB, "PSDTB", "PSDTB", "", "", "" ,TOC_SYSTEM,  "", ""); 


CPSDTB::CPSDTB(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) //:
  //FxdEdtBookRef(NULL),
  //TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
     Id_Offset = 0;
     XID_Offset = 0;
     Clear();
  }

// -------------------------------------------------------------------------

CPSDTB::CPSDTB(  pchar Tag_,  pTaggedObject pAttach, TagObjAttachment eAttach ) //:
  //FxdEdtBookRef(NULL),
  //TaggedObject(&CPSDTBClass, Tag_, pAttach, eAttach)
  {
    Id_Offset = 0;
    XID_Offset = 0;
    Clear();
  }

// -------------------------------------------------------------------------

CPSDTB::CPSDTB(CPSDTB& Fn) //:
  //FxdEdtBookRef(NULL),
  //TaggedObject(Fn.m_pClass, "", Fn.pAttachedTo, Fn.eAttachment)
  {
 // Strng s = Fn.sTag;
  //TaggedObject::Tag(s());
  Id_Offset = 0;
  XID_Offset = 0;
  Clear();
  }

// -------------------------------------------------------------------------
 
CPSDTB::CPSDTB()
  {
  Id_Offset = 0;
  XID_Offset = 0;
  Clear();
  }

// -------------------------------------------------------------------------
 
CPSDTB::~CPSDTB()
  {

  }
 
// -------------------------------------------------------------------------
 
void CPSDTB::Clear()
  {

  for (int i = 0; i < MaxPSDTestSpecies ; i++ )
    {
      TestSizes[i] = 0.0;
      TestCum[i] = 0.0; 
    }
  }

// -------------------------------------------------------------------------

void CPSDTB::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* TagObj )
  {

  DDB.BeginObject(TagObj, "TstData" , "PSDSpecies" , NULL, DDB_OptPage);

  DDB.Long("NSpecies",     "",     DC_,    "", XID_Offset + xID_NSpecies , TagObj, isParm);

   for (int i=0;i<m_PSDTestData.GetCount();i++)
    {
      char temp[80];
      sprintf(temp,"SP%d",i);
      DDB.BeginObject(TagObj, temp , "PSDInfo" , NULL, DDB_OptPage);

      // Species Name Description
      char temps[5];
      sprintf(temps,"Name");
      DDB.String(temps,  "", DC_, "", XID_Offset + xID_Name + i ,  TagObj, isParm );

      // Type of PSD Specification
      // Test Data or Function
      DDB.Long("EntryType"  , "", DC_, "", XID_Offset + xID_EntryType + i, TagObj , isParm, DDB_TestEntryType);

      // Type of PSD Function
      DDB.Long("FuncType"  , "", DC_, "", XID_Offset + xID_FuncType + i, TagObj , isParm, DDB_FunctionType);

      // Function Parameters
      DDB.Double("RR_a"  , "", DC_, "", XID_Offset + xID_FuncRR_a + i, TagObj , isParm);
      DDB.Double("RR_b"  , "", DC_, "", XID_Offset + xID_FuncRR_b + i, TagObj , isParm);
      DDB.Double("GS_m"  , "", DC_, "", XID_Offset + xID_FuncGS_m + i, TagObj , isParm);
      DDB.Double("GS_k"  , "", DC_, "", XID_Offset + xID_FuncGS_k + i, TagObj , isParm);


      // Number of Test Data for the Species
      DDB.Long("N", "", DC_, "",  XID_Offset + xID_NData + i , TagObj, isParm );

      // Data Entry Type of Test Data
      DDB.Long("DataType"  , "", DC_, "", XID_Offset + xID_DataType + i, TagObj , isParm, DDB_TestDataType);

      DDB.EndObject();


    }

  for (int i=0;i<m_PSDTestData.GetCount();i++)
  {

  char lClass[80];
  //sprintf(lClass,"PSDData%d",i);
  sprintf(lClass,"PSDData");
  char lTag[80];
  sprintf(lTag,"TstInts%d",i);
  DDB.BeginObject(TagObj, lTag , lClass , NULL, DDB_OptPage);

      for (int j=0;j<MaxTestData;j++)
      { 
        // For each size fraction - store in rows
        char lTagName[80];
        sprintf(lTagName,"I%d",j);
        char lClassName[80];
        sprintf(lClassName,"SP%d",i);
 
        // For each species - store in columns

        if (j < m_PSDTestData[i].GetTestDataCount())
          {

            DDB.BeginObject(TagObj, lTagName , lClassName , NULL, DDB_OptPage);

            char temp[80];
            sprintf(temp,"Size.SP%d",i);
            DDB.Double(temp, "", DC_L, "um",    XID_Offset +xID_SzData      + i*MaxTestData + j ,  TagObj, isParm );


            sprintf(temp,"Frac.SP%d",i);
            DDB.Double(temp, "", DC_, "",    XID_Offset +xID_FracData    + i*MaxTestData + j ,  TagObj, isParm );


            sprintf(temp,"Cum.SP%d",i);
            DDB.Double(temp, "", DC_, "",    XID_Offset +xID_CumFracData + i*MaxTestData + j ,  TagObj, isParm );

            DDB.EndObject();

          
          }

      }

  DDB.EndObject();


    }

  DDB.EndObject();


  }

// -------------------------------------------------------------------------

flag CPSDTB::DataXchg(DataChangeBlk & DCB)
  {

//
// Requests to set and read data via DataXchg
//

//  if (TaggedObject::DataXchg(DCB)) 
//    return 1;

  //
  // XChng PSD
  //

  long lHandle = DCB.lHandle - XID_Offset;
  //
  // Number of Species
  //
  switch(lHandle)
    {
    case xID_NSpecies:
      if (DCB.rL)
        m_PSDTestData.SetSize(max(0,*DCB.rL));
      DCB.L = m_PSDTestData.GetSize();
     return(1);

    }

   //
   // Names of Species
   //
   if ((lHandle >= xID_Name && 
        lHandle < (xID_Name+MaxPSDTestSpecies)))
      {


          long offset = lHandle - xID_Name;

          if (DCB.rpC)
               m_PSDTestData[offset].SetName(DCB.rpC);
          Strng s = m_PSDTestData[offset].GetName();
          DCB.pC = m_PSDTestData[offset].m_Name.Str();

           return(1);
      }

   //
   // Number of Test Data for Each Species
   //

   if ((lHandle >= xID_NData && 
        lHandle < (xID_NData+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_NData;

        if (DCB.rL)
          m_PSDTestData[offset].SetTestDataCount(min(max(0,*DCB.rL),MaxTestData));
        DCB.L = m_PSDTestData[offset].GetTestDataCount();
        return(1);
      }

   //
   // Type of Test Data for Each Species
   //

   if ((lHandle >= xID_DataType && 
        lHandle < (xID_DataType+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_DataType;

        if (DCB.rL)
          m_PSDTestData[offset].SetTestDataType((CPSDTestData::eTestDataType)*DCB.rL);
        DCB.L = (long)(m_PSDTestData[offset].GetTestDataType());
        return(1);
      }


  //
  // Size Elements
  //

   if ((lHandle >= xID_SzData && 
        lHandle < (xID_SzData+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lHandle - xID_SzData;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

        if (DCB.rD)
          m_PSDTestData[lSpeciesIndex].SetTestDataSz(lTestDataIndex,*DCB.rD);
        DCB.D = m_PSDTestData[lSpeciesIndex].GetTestDataSz(lTestDataIndex);
        return(1);
      }

  //
  // Frac Elements
  //

   if ((lHandle >= xID_FracData && 
        lHandle < (xID_FracData+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset =lHandle - xID_FracData;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

        if (DCB.rD)
          m_PSDTestData[lSpeciesIndex].SetTestDataFrac(lTestDataIndex,*DCB.rD);
        DCB.D = m_PSDTestData[lSpeciesIndex].GetTestDataFrac(lTestDataIndex);
        return(1);
      }

  //
  // Cum Elements
  //

   if ((lHandle >= xID_CumFracData && 
        lHandle < (xID_CumFracData+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lHandle - xID_CumFracData;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

        if (DCB.rD)
          m_PSDTestData[lSpeciesIndex].SetTestDataCumFrac(lTestDataIndex,*DCB.rD);
        DCB.D = m_PSDTestData[lSpeciesIndex].GetTestDataCumFrac(lTestDataIndex);
        return(1);
      }

   //
   // Type of Entry Data for Each Species
   //

   if ((lHandle >= xID_EntryType && 
        lHandle < (xID_EntryType+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_EntryType;

        if (DCB.rL)
          m_PSDTestData[offset].SetTestEntryType((CPSDTestData::eEntryType)*DCB.rL);
        DCB.L = (long)(m_PSDTestData[offset].GetTestEntryType());
        return(1);
      }

   //
   // Function Type of Entry Data for Each Species
   //

   if ((lHandle >= xID_FuncType && 
        lHandle < (xID_FuncType+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_FuncType;

        if (DCB.rL)
          m_PSDTestData[offset].SetFunctionType((CPSDTestData::eFunctionType)*DCB.rL);
        DCB.L = (long)(m_PSDTestData[offset].GetFunctionType());
        return(1);
      }

    //
    // Function Parameters
    //

   if ((lHandle >= xID_FuncRR_a && 
        lHandle < (xID_FuncRR_a+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_FuncRR_a;

        if (DCB.rD)
          m_PSDTestData[offset].setRR_a(*DCB.rD);
        DCB.D = (m_PSDTestData[offset].getRR_a());
        return(1);
      }
   if ((lHandle >= xID_FuncRR_b && 
        lHandle < (xID_FuncRR_b+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_FuncRR_b;

        if (DCB.rD)
          m_PSDTestData[offset].setRR_b(*DCB.rD);
        DCB.D = (m_PSDTestData[offset].getRR_b());
        return(1);
      }
   if ((lHandle >= xID_FuncGS_k && 
        lHandle < (xID_FuncGS_k+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_FuncGS_k;

        if (DCB.rD)
          m_PSDTestData[offset].setGS_k(*DCB.rD);
        DCB.D = (m_PSDTestData[offset].getGS_k());
        return(1);
      }
   if ((lHandle >= xID_FuncGS_m && 
        lHandle < (xID_FuncGS_m+MaxPSDTestSpecies)))
      {
         long offset = lHandle - xID_FuncGS_m;

        if (DCB.rD)
          m_PSDTestData[offset].setGS_m(*DCB.rD);
        DCB.D = (m_PSDTestData[offset].getGS_m());
        return(1);
      }

  return False;
  }

// -------------------------------------------------------------------------

void CPSDTB::Build(FxdEdtBookRef* pFxdEdt)
  {

  int L=0;

  pFxdEdt->StartPage("PSD");


  // Start a New block with 2 Description Entries
  pFxdEdt->StartBlk(2, 0, NULL);

  //
  // Species Names
  //
  pFxdEdt->SetSpace(L,FieldSpace);
  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {
      Strng Name = m_PSDTestData[i].GetName();
      pFxdEdt->SetDesc(L, Name.Str() , -1 , 2*FieldWidth, 1, "");
      pFxdEdt->SetSpace(L,FieldSpace);
    }
  L++;
  //
  // Type of Entry (Test Data or Function)
  //

  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {
          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "Entry" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_EntryType+i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.EntryType", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
     
          for (int k=0; DDB_TestEntryType[k].m_pStr !=NULL;k++)
          {
          pFxdEdt->FldHasFixedStrValue(k, DDB_TestEntryType[k].m_pStr );
          }

    }

  // Start a New Block with the Number of Test Data for each Test Data Series
  pFxdEdt->StartBlk(4, 0, NULL);
  L=0;

  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {

    if ( m_PSDTestData[i].GetTestEntryType() == CPSDTestData::eEntry_Test )
        {
         // Number of Test Data

          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "N" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_NumData+i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.N", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
      
        }
    else
      {

          // Type of Function

          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "Func" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_FuncType+i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.FuncType", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
     
          for (int k=0; DDB_FunctionType[k].m_pStr !=NULL;k++)
          {
          pFxdEdt->FldHasFixedStrValue(k, DDB_FunctionType[k].m_pStr );
          }

      }

    }
    L++;

  // Data Entry Type
  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {

      if ( m_PSDTestData[i].GetTestEntryType() == CPSDTestData::eEntry_Test )
        {
          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "Type" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_DataType+i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.DataType", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
     
          for (int k=0; DDB_TestDataType[k].m_pStr !=NULL;k++)
          {
          pFxdEdt->FldHasFixedStrValue(k, DDB_TestDataType[k].m_pStr );
          }
        }
    else
      {
        //pFxdEdt->SetSpace(L,FieldSpace+FieldWidth+FieldWidth);


        if ( m_PSDTestData[i].GetFunctionType() == CPSDTestData::eFunc_RR )
          {
          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "a" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_FuncRR_a +i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.RR_a", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
  
          }
        else if ( m_PSDTestData[i].GetFunctionType() == CPSDTestData::eFunc_GS )
          {
          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "k" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_FuncGS_k +i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.GS_k", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
 
          }
        else
          {
            pFxdEdt->SetSpace(L,FieldSpace+FieldWidth+FieldWidth);
          }

      }
    }
    L++;

  //
  // Species Sub Labels
  //
  pFxdEdt->SetSpace(L,FieldSpace);
  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {
      if ( m_PSDTestData[i].GetTestEntryType() == CPSDTestData::eEntry_Test )
        {

          pFxdEdt->SetDesc(L, "Size" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetDesc(L, "Frac" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetSpace(L,FieldSpace);

        }
    else
      {
        //pFxdEdt->SetSpace(L,FieldSpace+FieldWidth+FieldWidth);

        if ( m_PSDTestData[i].GetFunctionType() == CPSDTestData::eFunc_RR )
          {
          //pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "b" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_FuncRR_b +i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.RR_b", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
          pFxdEdt->SetSpace(L,FieldSpace);
          }
        else if ( m_PSDTestData[i].GetFunctionType() == CPSDTestData::eFunc_GS )
          {
          //pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetDesc(L, "m" , -1 , FieldWidth, 1, "");
          pFxdEdt->SetData(L,"", Id_Offset + Id_FuncGS_m +i , FieldWidth , 2, "", true );
          Strng Tg;
          Tg.Set("%s.SP%d.GS_m", pFxdEdt->FullObjTag(), i );
          pFxdEdt->SetTag(Tg());
          pFxdEdt->SetSpace(L,FieldSpace);
          }
        else
          {
            pFxdEdt->SetSpace(L,FieldSpace+FieldWidth+FieldWidth);
          }

      }
    }
  L++;
  pFxdEdt->SetSpace(L,FieldSpace);
  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {
      if ( m_PSDTestData[i].GetTestEntryType() == CPSDTestData::eEntry_Test )
        {

          pFxdEdt->SetDesc(L, m_PSDTestData[i].GetSizeCnvTxt().Str() , -1 , FieldWidth, 1, "");
          pFxdEdt->SetDesc(L, m_PSDTestData[i].GetFracCnvTxt().Str() , -1 , FieldWidth, 1, "");
          pFxdEdt->SetSpace(L,FieldSpace);

        }
    else
      {
        pFxdEdt->SetSpace(L,FieldSpace+FieldWidth+FieldWidth);
      }
    }


  // Start a New block with N Column Data Entries

  pFxdEdt->StartBlk(MaxTestData, 0, NULL);
  L=0;

  for (int j=0;j<MaxTestData;j++)
  {
 

  for (int i=0;i<m_PSDTestData.GetSize();i++)
    {
      if ( m_PSDTestData[i].GetTestEntryType() == CPSDTestData::eEntry_Test )
        {

        int lNData = m_PSDTestData[i].GetTestDataCount();
        CPSDTestData::eTestDataType lTestDataType = m_PSDTestData[i].GetTestDataType();
        if (j < lNData) 
        {
          pFxdEdt->SetSpace(L,FieldSpace);
          pFxdEdt->SetData(L,"", Id_Offset + Id_Size+i*MaxTestData + j , FieldWidth , 2, "", true );

          Strng Tg;
          Tg.Set("%s.I%d.Size.SP%d", pFxdEdt->FullObjTag(), j, i );
          pFxdEdt->SetTag(Tg(), m_PSDTestData[i].m_SizeCnvTxt.Str());
   

          if (lTestDataType == CPSDTestData::eTest_WeightPercent)
            {
            pFxdEdt->SetData(L,"", Id_Offset + Id_Frac+i*MaxTestData + j , FieldWidth , 2, "", true );

   
            Strng Tg;
            Tg.Set("%s.I%d.Frac.SP%d", pFxdEdt->FullObjTag(), j, i );
            pFxdEdt->SetTag(Tg(), m_PSDTestData[i].m_FracCnvTxt.Str());
   

            }
          else
            {
            pFxdEdt->SetData(L,"", Id_Offset + Id_Cum+i*MaxTestData + j , FieldWidth , 2, "", true );

            Strng Tg;
            Tg.Set("%s.I%d.Cum.SP%d", pFxdEdt->FullObjTag(), j, i );
            pFxdEdt->SetTag(Tg(), m_PSDTestData[i].m_FracCnvTxt.Str());
   

            }
   
        }
        else
        {
          pFxdEdt->SetSpace(L,2*FieldWidth+FieldSpace);
        }
      }
    else
      {
        pFxdEdt->SetSpace(L,FieldSpace+FieldWidth+FieldWidth);
      }
    }

    L++;
  }

  }

//---------------------------------------------------------------------------

void CPSDTB::Load(FxdEdtBookRef* pFxdEdt, FxdEdtInfo &EI, Strng & Str)
  {
  long lFieldId;
  lFieldId = EI.FieldId - Id_Offset;

  if (EI.FieldId > Id_Offset)
    {
    int testint = 0;
    }

  int offset;
  const int cnt = m_PSDTestData.GetSize();

  //if (pFxdEdt->CurrentBlk(EI))
    {
 
      if (lFieldId == Id_NumSpecies)
        {
        Str.Set("%d",m_PSDTestData.GetCount() );
        return;
        }

      if (cnt>0)
        {
        if ((lFieldId >= Id_EntryType) && (lFieldId < (Id_EntryType+MaxPSDTestSpecies) ))
          {
          offset=(int)(lFieldId-Id_EntryType);
          Str.Set("%s", (m_PSDTestData[offset].GetEntryString()).Str() );
          return;
          }

        if ((lFieldId >= Id_FuncType) && (lFieldId < (Id_FuncType+MaxPSDTestSpecies) ))
          {
          offset=(int)(lFieldId-Id_FuncType);
          Str.Set("%s", (m_PSDTestData[offset].GetFunctionString()).Str() );
          return;
          }

        if ((lFieldId >= Id_FuncRR_a) && (lFieldId < (Id_FuncRR_a+MaxPSDTestSpecies) ))
          {
          offset=(int)(lFieldId-Id_FuncRR_a);
          Str.Set("%g", m_PSDTestData[offset].getRR_a() );
          return;
          }

        if ((lFieldId >= Id_FuncRR_b) && (lFieldId < (Id_FuncRR_b+MaxPSDTestSpecies) ))
          {
          offset=(int)(lFieldId-Id_FuncRR_b);
          Str.Set("%g", m_PSDTestData[offset].getRR_b() );
          return;
          }

        if ((lFieldId >= Id_FuncGS_k) && (lFieldId < (Id_FuncGS_k+MaxPSDTestSpecies) ))
          {
          offset=(int)(lFieldId-Id_FuncGS_k);
          Str.Set("%g", m_PSDTestData[offset].getGS_k() );
          return;
          }

        if ((lFieldId >= Id_FuncGS_m) && (lFieldId < (Id_FuncGS_m+MaxPSDTestSpecies) ))
          {
          offset=(int)(lFieldId-Id_FuncGS_m);
          Str.Set("%g", m_PSDTestData[offset].getGS_m() );
          return;
          }
        }
    }

  //if (pFxdEdt->CurrentBlk(EI))
    {
    if (cnt>0)
      {
      if ((lFieldId >= Id_NumData) && (lFieldId < (Id_NumData+MaxPSDTestSpecies) ))
        {
        int offset=(int)(lFieldId-Id_NumData);
        Str.Set("%d",m_PSDTestData[offset].GetTestDataCount() );
        EI.Fld->fEditable=true; // Force this to see what happens
        return;
        }

      if ((lFieldId >= Id_DataType) && (lFieldId < (Id_DataType+MaxPSDTestSpecies) ))
        {
        offset=(int)(lFieldId-Id_DataType);
        Str.Set("%s", (m_PSDTestData[offset].GetTypeString()).Str() );
        return;
        }
      }
    }

   //if (pFxdEdt->CurrentBlk(EI))
    {
   if ((lFieldId >= Id_Size && 
        lFieldId < (Id_Size+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lFieldId - Id_Size;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

         double d = m_PSDTestData[lSpeciesIndex].GetTestDataSz(lTestDataIndex);
         d = m_PSDTestData[lSpeciesIndex].m_SizeCnv.Human(d);
         Str.Set("%g",d);
         return;

      }

   if ((lFieldId >= Id_Frac && 
        lFieldId < (Id_Frac+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lFieldId - Id_Frac;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

         double d = m_PSDTestData[lSpeciesIndex].GetTestDataFrac(lTestDataIndex);
         d = m_PSDTestData[lSpeciesIndex].m_FracCnv.Human(d);
         Str.Set("%g",d);
         return;

      }

  if ((lFieldId >= Id_Cum && 
       lFieldId < (Id_Cum+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lFieldId - Id_Cum;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

         double d = m_PSDTestData[lSpeciesIndex].GetTestDataCumFrac(lTestDataIndex);
         d = m_PSDTestData[lSpeciesIndex].m_FracCnv.Human(d);
         Str.Set("%g",d);
         return;

      }


    }

  }

//---------------------------------------------------------------------------

long CPSDTB::Parse(FxdEdtBookRef* pFxdEdt, FxdEdtInfo &EI, Strng & Str)
  {

  long Fix=0; //set Fix=1 to redraw graph
  long  lFieldId = EI.FieldId - Id_Offset;

 //if (pFxdEdt->CurrentBlk(EI))
 //   {

   int offset=(int)(lFieldId-Id_EntryType);

    if ((lFieldId >= Id_EntryType) && (lFieldId < (Id_EntryType+MaxPSDTestSpecies) ))
      {

        CPSDTestData::eEntryType ltype = m_PSDTestData[offset].GetEntryStringType(Str);
        m_PSDTestData[offset].SetTestEntryType(ltype);
        return Fix;

      }

   offset=(int)(lFieldId-Id_FuncType);

    if ((lFieldId >= Id_FuncType) && (lFieldId < (Id_FuncType+MaxPSDTestSpecies) ))
      {

        CPSDTestData::eFunctionType ltype = m_PSDTestData[offset].GetFunctionStringType(Str);
        m_PSDTestData[offset].SetFunctionType(ltype);
        return Fix;

      }


     offset=(int)(lFieldId-Id_FuncRR_a);
      if ((lFieldId >= Id_FuncRR_a) && (lFieldId < (Id_FuncRR_a+MaxPSDTestSpecies) ))
      {
        m_PSDTestData[offset].setRR_a(atof(Str.Str())) ;
        return Fix;
      }

      offset=(int)(lFieldId-Id_FuncRR_b);
      if ((lFieldId >= Id_FuncRR_b) && (lFieldId < (Id_FuncRR_b+MaxPSDTestSpecies) ))
      {
        m_PSDTestData[offset].setRR_b(atof(Str.Str())) ;
        return Fix;
      }

      offset=(int)(lFieldId-Id_FuncGS_k);
      if ((lFieldId >= Id_FuncGS_k) && (lFieldId < (Id_FuncGS_k+MaxPSDTestSpecies) ))
      {
        m_PSDTestData[offset].setGS_k(atof(Str.Str())) ;
        return Fix;
      }

     offset=(int)(lFieldId-Id_FuncGS_m);
      if ((lFieldId >= Id_FuncGS_m) && (lFieldId < (Id_FuncGS_m+MaxPSDTestSpecies) ))
      {
        m_PSDTestData[offset].setGS_m(atof(Str.Str())) ;
        return Fix;
      }


 //   }

  //if (pFxdEdt->CurrentBlk(EI))
    {
    
    offset=(int)(lFieldId-Id_NumData);

    if ((lFieldId >= Id_NumData) && (lFieldId < (Id_NumData+MaxPSDTestSpecies) ))
      {
        m_PSDTestData[offset].SetTestDataCount(min(max(0,Str.SafeAtoL()),MaxTestData));
        return Fix;
      }

    offset=(int)(lFieldId-Id_DataType);

    if ((lFieldId >= Id_DataType) && (lFieldId < (Id_DataType+MaxPSDTestSpecies) ))
      {
        //m_PSDTestData[offset].SetTestDataType((CPSDTestData::eTestDataType)Str.SafeAtoL());

        CPSDTestData::eTestDataType ltype = m_PSDTestData[offset].GetTypeStringType(Str);
        m_PSDTestData[offset].SetTestDataType(ltype);
        return Fix;

      }

    }

   //if (pFxdEdt->CurrentBlk(EI))
    {

      if ((lFieldId >= Id_Size && 
           lFieldId < (Id_Size+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lFieldId - Id_Size;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;


         double d = Str.SafeAtoF();
         d = m_PSDTestData[lSpeciesIndex].m_SizeCnv.Normal(d);
         m_PSDTestData[lSpeciesIndex].SetTestDataSz(lTestDataIndex,d);
         return Fix;
  
      }

         if ((lFieldId >= Id_Frac && 
              lFieldId < (Id_Frac+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lFieldId - Id_Frac;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

         double d = Str.SafeAtoF();
         d = m_PSDTestData[lSpeciesIndex].m_FracCnv.Normal(d);
         m_PSDTestData[lSpeciesIndex].SetTestDataFrac(lTestDataIndex,d);
         return Fix;
  
      }

        if ((lFieldId >= Id_Cum && 
             lFieldId < (Id_Cum+MaxPSDTestSpecies*MaxTestData)))
      {
         long offset = lFieldId - Id_Cum;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;

         double d = Str.SafeAtoF();
         d = m_PSDTestData[lSpeciesIndex].m_FracCnv.Normal(d);
         m_PSDTestData[lSpeciesIndex].SetTestDataCumFrac(lTestDataIndex,d);
         return Fix;
  
      }

    }

  return Fix;
  }

//---------------------------------------------------------------------------

long CPSDTB::ButtonPushed(FxdEdtBookRef* pFxdEdt, FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (pFxdEdt->CurrentBlk(EI))
    {
    }

  if (pFxdEdt->CurrentBlk(EI))
    {
    }

  if (pFxdEdt->CurrentBlk(EI))
    {
    }


  return 1;
  }

//==========================================================================

void CPSDTB::SetSpecies(long N, const Strng *Names)

  {
    m_PSDTestData.SetSize(N);
    for (int i = 0; i < N ; i++ )
      {
        m_PSDTestData[i].SetName(Names[i]);
      }

  }

//==========================================================================

void CPSDTB::SetDistribution(CSD_DistDefn   *pD)

  {

  int count = 0;

  m_pD = pD;
  if (m_pD)
    {

    const int NColmns = m_pD->Columns.GetSize();
    for (int c=0; c<NColmns; c++)
      {
      CSD_Column lC = m_pD->Columns[c];

      if ((lC.Editable() )&&(lC.iSpId >= 0))
        {

          //
          // Editable Data with Valid Species ID
          // Add to our list
          // 
          count++;
          if ( count > m_PSDTestData.GetSize() )
            // Increase the size of our Test Data
            // Only if count increases. If we do it on the fly
            // each time this routine is called we will lose the
            // data due to the fact that the size will be reset
            // back to 1 at the beginning of the loop
            m_PSDTestData.SetSize(count);

          m_PSDTestData[count-1].SetName(lC.sSpName);
          m_PSDTestData[count-1].SetSizeCnvTxt(SD_Defn.XCnv.Text());
          m_PSDTestData[count-1].SetFracCnvTxt(lC.pCnv->Text()) ;
          m_PSDTestData[count-1].SetSizeCnv(SD_Defn.XCnv);
          m_PSDTestData[count-1].SetFracCnv(*lC.pCnv);
          
        }

      }    

    }

  }

//==========================================================================

void CPSDTB::SetIDOffsets(int Id, int XID )

  {
    // Offset the EditView IDs and DataXChng IDs to what the owner class
    // requires so that there are no conflicts with the owner class IDs.
    // We do this so that the owner class can call our methods directly
    Id_Offset = Id;
    XID_Offset = XID;
  }

//==========================================================================

CnvAttribute* CPSDTB::GetCnv( long lFieldId )

  {
  CnvAttribute* cnv = NULL;

  lFieldId = lFieldId - Id_Offset;

  int offset=(int)(lFieldId-Id_NumData);

  if ((lFieldId >= Id_NumData) && (lFieldId < (Id_NumData+MaxPSDTestSpecies) ))
    {

    }

  offset=(int)(lFieldId-Id_DataType);

  if ((lFieldId >= Id_DataType) && (lFieldId < (Id_DataType+MaxPSDTestSpecies) ))
    {

    }


  if ((lFieldId >= Id_Size && 
    lFieldId < (Id_Size+MaxPSDTestSpecies*MaxTestData)))
    {
         long offset = lFieldId - Id_Size;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;
         cnv = m_PSDTestData[lSpeciesIndex].GetSizeCnvRef();

         cnv = &SD_Defn.XCnv; // Hack

    }

  if ((lFieldId >= Id_Frac && 
    lFieldId < (Id_Frac+MaxPSDTestSpecies*MaxTestData)))
    {
         long offset = lFieldId - Id_Frac;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;
         cnv = m_PSDTestData[lSpeciesIndex].GetFracCnvRef();

         cnv = &SD_Defn.YFCnv; // Hack

    }

  if ((lFieldId >= Id_Cum && 
    lFieldId < (Id_Cum+MaxPSDTestSpecies*MaxTestData)))
    {
         long offset = lFieldId - Id_Cum;
         long lSpeciesIndex  = offset/MaxTestData;
         long lTestDataIndex = offset%MaxTestData;
         cnv = m_PSDTestData[lSpeciesIndex].GetFracCnvRef();

         cnv = &SD_Defn.YFCnv; // Hack

    }

  return(cnv);
  }

//==========================================================================
