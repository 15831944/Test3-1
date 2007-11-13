//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//---------------------------------------------------------------------------

INLINE void DDBDataInfo::GetDefinedData(TABOptions Opts, CPkDataList & List/*PkDataListInfo * pInfo, CPkDataItem* &pPItem*/)
  {
  PkDataUnion DataValue;
  DDEF_Flags RetFlags=GetValue(Opts, DataValue);
  VERIFY(DataValue.iType!=0);
  List.Item.Clear();
  List.Item.AppendDataValue(/*pInfo,*/ Opts, pTag, pSym, iFlags|RetFlags, DataValue, iCnv, pCnvTxt, pCnvFam); 
  if ((Opts & TABOpt_StrList) && pValueLst)
    List.Item.AppendValueLst(/*pInfo,*/ pValueLst, True);
  if ((Opts & TABOpt_XRefStatus) && m_psFullItemTag)
    {
    CTagRefStatus W=gs_TagRefGlblFn->GetTagRefStatus((*m_psFullItemTag)());
#if WITHEQUIPSPECS
    List.Item.AppendItemStatus(/*pInfo,*/ W, 0);
#else
    List.Item.AppendItemStatus(/*pInfo,*/ W);
#endif
    }
  List.AdvanceWrite();
  };


//---------------------------------------------------------------------------

INLINE void DDBDataInfo::PutDefinedData(TABOptions Opts, CPkDataList & List /*PkDataListInfo * pInfo*/, CPkDataIter &Item)
  {
  PutValue(Opts, *(List.CurrentItem(Item)->Value()));//*(m_pItem->Value()));
  };

//===========================================================================
//
//
//
//===========================================================================

INLINE UINT DataDefnBlk::HashValue(char* Key)
  { 
	// CNM - Borrowed from MFC CMapStringtoPtr
  UINT nHash = 0;
	while (*Key)
		nHash = (nHash<<5) + nHash + toupper(*Key++);
	return nHash;
  };

INLINE flag DataDefnBlk::GetReqdFlags()//DDEF_Flags iFlags) 
  {
  ASSERT((NSHM_All & dwOptions)!=0);
  if ((NSHM_All & iFlags & dwOptions)==0)
    return False;
  dword TABOpt_Msk = TABOpt_ForFile | TABOpt_ForView | TABOpt_ForSnapShot | TABOpt_ForScenario ;
  if ((TABOpt_Msk & iFlags)!=0)
    return ((iFlags & dwOptions & TABOpt_Msk)==0);
  else
    return True;
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Button(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                flag * Data, pTaggedObject This, DDEF_Flags Flags, 
                                DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_BUTTON|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Button(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                flag * Data, pTaggedObject This, DDEF_Flags Flags, 
                                DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_BUTTON|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Button(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                long Handle, pTaggedObject This, DDEF_Flags Flags, 
                                DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_BUTTON|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Button(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                long Handle, pTaggedObject This, DDEF_Flags Flags, 
                                DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_BUTTON|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBox(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                  flag * Data, pTaggedObject This, DDEF_Flags Flags, 
                                  DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBox(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                  flag * Data, pTaggedObject This, DDEF_Flags Flags, 
                                  DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBox(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                  long Handle, pTaggedObject This, DDEF_Flags Flags,
                                  DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX;
  pTag   = Tag;
  pSym   = Sym;     
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBox(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                  long Handle, pTaggedObject This, DDEF_Flags Flags,
                                  DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBox(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                  long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags,
                                  DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX;
  pTag   = Tag;
  pSym   = Sym;     
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBox(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                  long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags,
                                  DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBoxBtn(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                     flag * Data, pTaggedObject This, DDEF_Flags Flags, 
                                     DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX|DDEF_BUTTON;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBoxBtn(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                     flag * Data, pTaggedObject This, DDEF_Flags Flags, 
                                     DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX|DDEF_BUTTON;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBoxBtn(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                     long Handle, pTaggedObject This, DDEF_Flags Flags, 
                                     DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX|DDEF_BUTTON;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBoxBtn(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                     long Handle, pTaggedObject This, DDEF_Flags Flags, 
                                     DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX|DDEF_BUTTON;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBoxBtn(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                     long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                                     DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX|DDEF_BUTTON;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::CheckBoxBtn(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                     long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                                     DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags|DDEF_CHECKBOX|DDEF_BUTTON;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Bool(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              flag * Data, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Bool(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              flag * Data, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Bool(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Bool(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Bool(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Bool(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Bool;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoBool();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Char(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              char * Data, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Char;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoChar();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Char(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              char * Data, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Char;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoChar();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Char(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Char;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoChar();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Char(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Char;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoChar();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(char Min, char Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Byte(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              byte * Data, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Byte;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoByte();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Byte(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              byte * Data, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Byte;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoByte();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Byte(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              long Handle, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Byte;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoByte();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Byte(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              long Handle, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Byte;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoByte();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(byte Min, byte Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Word(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              word * Data, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Word;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Word(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              word * Data, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Word;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Word(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Word;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Word(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Word;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(word Min, word Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::DWord(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               dword * Data, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_DWord;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoDWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::DWord(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               dword * Data, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_DWord;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoDWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::DWord(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_DWord;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoDWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::DWord(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_DWord;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoDWord();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(dword Min, dword Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Int(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                             int * Data, pTaggedObject This, DDEF_Flags Flags,
                             DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Int;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoInt();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Int(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                             int * Data, pTaggedObject This, DDEF_Flags Flags,
                             DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Int;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoInt();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Int(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                             long Handle, pTaggedObject This, DDEF_Flags Flags,
                             DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Int;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoInt();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Int(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                             long Handle, pTaggedObject This, DDEF_Flags Flags,
                             DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Int;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoInt();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(int Min, int Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Short(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               short * Data, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Short;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoShort();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Short(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               short * Data, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Short;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoShort();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Short(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Short;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoShort();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Short(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Short;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoShort();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Short(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Short;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoShort();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Short(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                               DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Short;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoShort();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(short Min, short Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Long(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long * Data, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Long;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoLong();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Long(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long * Data, pTaggedObject This, DDEF_Flags Flags,
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Long;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoLong();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Long(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Long;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoLong();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Long(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Long;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoLong();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Long(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Long;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoLong();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Long(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                              long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, 
                              DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Long;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoLong();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(long Min, long Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Float(pchar Tag, pchar Sym, CCnvFamily * CnvFam,
                               float * Data, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Float;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = CnvFam->iCnv;
    pCnvTxt  = CnvFam->pCnvTxt;
    pCnvFam  = CnvFam->pFamily;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoFloat();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Float(pchar Tag, pchar Sym, CCnvFamily * CnvFam,
                               long Handle, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Float;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = CnvFam->iCnv;
    pCnvTxt  = CnvFam->pCnvTxt;
    pCnvFam  = CnvFam->pFamily;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoFloat();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Float(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                               float * Data, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Float;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoFloat();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Float(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                               long Handle, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Float;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoFloat();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(float Min, float Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      DMin.Set(Cnvs[iCnv]->Normal(Min, pCnvTxt));
      DMax.Set(Cnvs[iCnv]->Normal(Max, pCnvTxt));
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvFamily * CnvFam, 
//                                double * Data, pTaggedObject This, DDEF_Flags Flags, LPCTSTR NANName, LPCTSTR pDesc)
//  {
//  CHKSTUFF0();
//  iType  = tt_Double;
//  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
//  pTag   = Tag;
//  pSym   = Sym;
//  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
//    {
//    Nest[lCallLevel].fItemReqd = True;
//    iCnv     = CnvFam->iCnv;
//    pCnvTxt  = CnvFam->pCnvTxt;
//    pCnvFam  = CnvFam->pFamily;
//    pData    = Data;
//    lHandle  = 0;
//    lHandle2 = 0;
//    pThis    = This ? This : Nest[lCallLevel].pThis;
//    CHKSTUFF1(this);
//    DoDouble();
//    if (NANName)
//      DoNANName(NANName);
//    if (pDesc)
//      Description(pDesc);
//    }
//  else
//    Nest[lCallLevel].fItemReqd = False;
//  };
//
////---------------------------------------------------------------------------
//
//INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvFamily * CnvFam, 
//                                long Handle, pTaggedObject This, DDEF_Flags Flags, LPCTSTR NANName, LPCTSTR pDesc)
//  {
//  CHKSTUFF0();
//  iType  = tt_Double;
//  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
//  pTag   = Tag;
//  pSym   = Sym;
//  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
//    {
//    Nest[lCallLevel].fItemReqd = True;
//    iCnv     = CnvFam->iCnv;
//    pCnvTxt  = CnvFam->pCnvTxt;
//    pCnvFam  = CnvFam->pFamily;
//    pData    = NULL;
//    lHandle  = Handle;
//    lHandle2 = 0;
//    pThis    = This ? This : Nest[lCallLevel].pThis;
//    CHKSTUFF1(this);
//    DoDouble();
//    if (NANName)
//      DoNANName(NANName);
//    if (pDesc)
//      Description(pDesc);
//    }
//  else
//    Nest[lCallLevel].fItemReqd = False;
//  };
//
////---------------------------------------------------------------------------
//
//INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvFamily * CnvFam, 
//                                long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, LPCTSTR NANName, LPCTSTR pDesc)
//  {
//  CHKSTUFF0();
//  iType  = tt_Double;
//  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
//  pTag   = Tag;
//  pSym   = Sym;
//  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
//    {
//    Nest[lCallLevel].fItemReqd = True;
//    iCnv     = CnvFam->iCnv;
//    pCnvTxt  = CnvFam->pCnvTxt;
//    pCnvFam  = CnvFam->pFamily;
//    pData    = NULL;
//    lHandle  = Handle;
//    lHandle2 = Handle2;
//    pThis    = This ? This : Nest[lCallLevel].pThis;
//    CHKSTUFF1(this);
//    DoDouble();
//    if (NANName)
//      DoNANName(NANName);
//    if (pDesc)
//      Description(pDesc);
//    }
//  else
//    Nest[lCallLevel].fItemReqd = False;
//  };
//
////---------------------------------------------------------------------------
//
//INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
//                                double * Data, pTaggedObject This, DDEF_Flags Flags, LPCTSTR NANName, LPCTSTR pDesc)
//  {
//  CHKSTUFF0();
//  iType  = tt_Double;
//  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
//  pTag   = Tag;
//  pSym   = Sym;
//  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
//    {
//    Nest[lCallLevel].fItemReqd = True;
//    iCnv     = (Cnv);
//    pCnvTxt  = CnvTxt;
//    pCnvFam  = NULL;
//    pData    = Data;
//    lHandle  = 0;
//    lHandle2 = 0;
//    pThis    = This ? This : Nest[lCallLevel].pThis;
//    CHKSTUFF1(this);
//    DoDouble();
//    if (NANName)
//      DoNANName(NANName);
//    if (pDesc)
//      Description(pDesc);
//    }
//  else
//    Nest[lCallLevel].fItemReqd = False;
//  };
//
////---------------------------------------------------------------------------
//
//INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
//                                long Handle, pTaggedObject This, DDEF_Flags Flags, LPCTSTR NANName, LPCTSTR pDesc)
//  {
//  CHKSTUFF0();
//  iType  = tt_Double;
//  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
//  pTag   = Tag;
//  pSym   = Sym;
//  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
//    {
//    Nest[lCallLevel].fItemReqd = True;
//    iCnv     = (Cnv);
//    pCnvTxt  = CnvTxt;
//    pCnvFam  = NULL;
//    pData    = NULL;
//    lHandle  = Handle;
//    lHandle2 = 0;
//    pThis    = This ? This : Nest[lCallLevel].pThis;
//    CHKSTUFF1(this);
//    DoDouble();
//    if (NANName)
//      DoNANName(NANName);
//    if (pDesc)
//      Description(pDesc);
//    }
//  else
//    Nest[lCallLevel].fItemReqd = False;
//  };
//
////---------------------------------------------------------------------------
//
//INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
//                                long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, LPCTSTR NANName, LPCTSTR pDesc)
//  {
//  CHKSTUFF0();
//  iType  = tt_Double;
//  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
//  pTag   = Tag;
//  pSym   = Sym;
//  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
//    {
//    Nest[lCallLevel].fItemReqd = True;
//    iCnv     = (Cnv);
//    pCnvTxt  = CnvTxt;
//    pCnvFam  = NULL;
//    pData    = NULL;
//    lHandle  = Handle;       
//    lHandle2 = Handle2;
//    pThis    = This ? This : Nest[lCallLevel].pThis;
//    CHKSTUFF1(this);
//    DoDouble();
//    if (NANName)
//      DoNANName(NANName);
//    if (pDesc)
//      Description(pDesc);
//    }
//  else
//    Nest[lCallLevel].fItemReqd = False;
//  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvFamily * CnvFam, 
                                double * Data, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = CnvFam->iCnv;
    pCnvTxt  = CnvFam->pCnvTxt;
    pCnvFam  = CnvFam->pFamily;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoDouble();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvFamily * CnvFam, 
                                long Handle, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = CnvFam->iCnv;
    pCnvTxt  = CnvFam->pCnvTxt;
    pCnvFam  = CnvFam->pFamily;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoDouble();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvFamily * CnvFam, 
                                long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = CnvFam->iCnv;
    pCnvTxt  = CnvFam->pCnvTxt;
    pCnvFam  = CnvFam->pFamily;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoDouble();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                                double * Data, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = Data;
    lHandle  = 0;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoDouble();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                                long Handle, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoDouble();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Double(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                                long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags, DDBValueLst * NANStrLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;       
    lHandle2 = Handle2;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    CHKSTUFF1(this);
    DoDouble();
    if (NANStrLst)
      DoNANName(NANStrLst);
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Range(double Min, double Max)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      {
      DataUnion DMin(Min);
      DataUnion DMax(Max);
      DMin.Set(Cnvs[iCnv]->Normal(Min, pCnvTxt));
      DMax.Set(Cnvs[iCnv]->Normal(Max, pCnvTxt));
      m_pItem->AppendRange(/*pDataObj->GetInfo(),*/ DMin, DMax);
      }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::FnDouble(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                                  long Handle, pTaggedObject This, DDEF_Flags Flags, 
                                  DDBFnParms * ParmDefs, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Double;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItemFn()))// ||
//      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv     = (Cnv);
    pCnvTxt  = CnvTxt;
    pCnvFam  = NULL;
    pData    = NULL;
    lHandle  = Handle;
    lHandle2 = 0;
    pThis    = This ? This : Nest[lCallLevel].pThis;
    if (eTask&(DDBTask_GetData|DDBTask_FndData))
      ParseParms(ParmDefs);
    CHKSTUFF1(this);
    DoDouble();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::String(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                                pStrng Data, pTaggedObject This, DDEF_Flags Flags,
                                DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Strng;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = (Cnv);
    pCnvTxt = CnvTxt;
    pCnvFam  = NULL;
    pData   = Data;
    lHandle = 0;
    lHandle2 = 0;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    //iFlags  = Flags;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoString();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::String(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt,
                                pStrng Data, pTaggedObject This, DDEF_Flags Flags,
                                DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Strng;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = (Cnv);
    pCnvTxt = CnvTxt;
    pCnvFam  = NULL;
    pData   = Data;
    lHandle = 0;
    lHandle2 = 0;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    //iFlags  = Flags;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoString();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::String(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                long Handle, pTaggedObject This, DDEF_Flags Flags,
                                DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Strng;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = (Cnv);
    pCnvTxt = CnvTxt;
    pCnvFam  = NULL;
    pData   = NULL;
    lHandle = Handle;
    lHandle2 = 0;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    //iFlags  = Flags;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoString();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::String(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                long Handle, pTaggedObject This, DDEF_Flags Flags,
                                DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Strng;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = (Cnv);
    pCnvTxt = CnvTxt;
    pCnvFam  = NULL;
    pData   = NULL;
    lHandle = Handle;
    lHandle2 = 0;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    //iFlags  = Flags;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoString();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::String(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags,
                                DDBValueLst * ValueLst, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Strng;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = (Cnv);
    pCnvTxt = CnvTxt;
    pCnvFam  = NULL;
    pData   = NULL;
    lHandle = Handle;
    lHandle2 = Handle2;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    //iFlags  = Flags;
    pValueLst    = ValueLst;
    pValueLstMem = NULL;
    CHKSTUFF1(this);
    DoString();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::String(pchar Tag, pchar Sym, CCnvIndex Cnv, pchar CnvTxt, 
                                long Handle, long Handle2, pTaggedObject This, DDEF_Flags Flags,
                                DDBValueLstMem * ValueLstMem, LPCTSTR pDesc)
  {
  CHKSTUFF0();
  iType  = tt_Strng;
  iFlags = Nest[lCallLevel].dwDefaultFlags|Flags;
  pTag   = Tag;
  pSym   = Sym;
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = (Cnv);
    pCnvTxt = CnvTxt;
    pCnvFam  = NULL;
    pData   = NULL;
    lHandle = Handle;
    lHandle2 = Handle2;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    //iFlags  = Flags;
    pValueLst    = ValueLstMem->List();
    pValueLstMem = ValueLstMem;
    CHKSTUFF1(this);
    DoString();
    if (pDesc)
      Description(pDesc);
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Blob(pchar Tag, void *Data, word Size, pTaggedObject This)
  {
  iType  = tt_Blob;
  iFlags = Nest[lCallLevel].dwDefaultFlags|DDEF_PARAM|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_HIDDEN, NULL,pThis;
  pTag   = Tag;
  pSym   = "";
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv    = 0;
    pCnvTxt = NULL;
    pCnvFam  = NULL;
    pData   = Data;
    lHandle = 0;
    lHandle2 = 0;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    wBlobSize = Size;
    DoBlob();
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//---------------------------------------------------------------------------

INLINE void DataDefnBlk::Blob(pchar Tag, long Handle, pTaggedObject This)
  {
  iType  = tt_Blob;
  iFlags = Nest[lCallLevel].dwDefaultFlags|DDEF_PARAM|DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_HIDDEN;
  pTag   = Tag;
  pSym   = "";
  if (((eTask&(DDBTask_FndData|DDBTask_GetData)) && GetReqdItem()) ||
      (eTask==DDBTask_PutData && PutReqdItem()))
    {
    Nest[lCallLevel].fItemReqd = True;
    iCnv=0;
    pCnvTxt = NULL;
    pCnvFam  = NULL;
    pData   = NULL;
    lHandle = Handle;
    lHandle2 = 0;
    pThis   = This ? This : Nest[lCallLevel].pThis;
    wBlobSize = 0;
    DoBlob();
    }
  else
    Nest[lCallLevel].fItemReqd = False;
  }

//------------------------------------------------------------------------
//------------------------------------------------------------------------

INLINE void DataDefnBlk::DoNANName(LPCTSTR pStr)
  {
  if (pStr && eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    {
    //if (fAllInfo)
    word *pStrLstLen=NULL;
    m_pItem->AppendStrListItem((char*)pStr, 0, false, pStrLstLen);
    }
  }
INLINE void DataDefnBlk::DoNANName(DDBValueLst * NANStrLst)
  {
  if (NANStrLst && eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    m_pItem->AppendValueLst(NANStrLst, true);
  }

INLINE void DataDefnBlk::Description(LPCTSTR pStr)
  {
  if (pStr && eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      m_pItem->AppendDescription((LPTSTR)pStr);
  }

//------------------------------------------------------------------------

INLINE void DataDefnBlk::TagComment(pchar pStr)
  {
  if (eTask==DDBTask_GetData && Nest[lCallLevel].fItemReqd)
    if (fAllInfo)
      m_pItem->AppendTagComment(pStr);
  }
//------------------------------------------------------------------------

INLINE void DataDefnBlk::Page(pchar Nm, DDBPages PageIs, DDEF_Flags Flags)
  {
  char iType=tt_RqdPage;
  switch (PageIs)
    {
    case DDB_NoPage:  return;
    //case DDB_NoPage: iType=tt_NoPage; break;
    case DDB_RqdPage: iType=tt_RqdPage; break;
    case DDB_OptPage: iType=tt_OptPage; break;//Nm, NULL, 0, NULL, Nm, Flags, NULL, NULL); break;
    }

  iFlags=Nest[lCallLevel].dwDefaultFlags|Flags;
  if (eTask==DDBTask_GetData)
    {
    if (fAllInfo && GetReqdLayout())//iFlags))
      {
      if (fWriting)
        m_pList->AdvanceWrite();//pPItem);
      fWriting=True;
      m_pItem->AppendLayout(/*pDataObj->GetInfo(),*/ iType, Nm, CurrentFlags());
      }
    }
  };

//------------------------------------------------------------------------

INLINE void DataDefnBlk::Text(pchar Txt, DDEF_Flags Flags)
  {
  iFlags=Nest[lCallLevel].dwDefaultFlags|Flags;
  switch (eTask)
    {
    case DDBTask_GetData:
      {
      if (fAllInfo && GetReqdLayout())//iFlags))
        {
        if (fWriting)
          m_pList->AdvanceWrite();//pPItem);
        fWriting=True;
        //m_pItem->Clear();
        m_pItem->AppendLayout(/*pDataObj->GetInfo(),*/ tt_Text, Txt, CurrentFlags());
        }
      break;
      }
    case DDBTask_PutData:
      {
      //if (PutReqdItem(pTag, pSym))
      //  {
      //  // ???????????????????????????????????????
      //          
      //  pDataObj->AdvanceRead(rpPItem);
      //  }
      break;
      }
    };
  };

//========================================================================
//
//
//
//========================================================================

INLINE void TaggedObject::ValidateTag(rStrng S)
  {
  S.Trim(" \t\n\r\v\f");
  const int len = S.Len();
  if (len)
    {
    int NumCharCnt = 0;
    for (int i=0; i<len; i++)
      {
      if (S[i]==' ' || S[i]==',' || S[i]=='%' || S[i]=='\t')
        S[i] = '_';
      if (isdigit(S[i]))
        NumCharCnt++;
      }
    if (S[0]=='\'' || S[0]=='=')
      {
      S.InsertChar(0, NonNumericTagChr);
      NumCharCnt--;
      }
#if WithNumericTags
    if ((NumericStartingTagsBad && isdigit(S[0])) || (NumericTagsBad && len==NumCharCnt))
#else
    if ((len==NumCharCnt) || (NumericStartingTagsBad && isdigit(S[0])))
#endif
      {
      S.InsertChar(0, NonNumericTagChr);
      }
    }
  }

//---------------------------------------------------------------------------

INLINE void TaggedObject::ValidateTagDot(rStrng S)
  {
  S.Trim(" \t\n\r\v\f");
  const int len = S.Len();
  if (len)
    {
    int NumCharCnt = 0;
    for (int i=0; i<len; i++)
      {
      if (S[i]==' ' || S[i]=='.' || S[i]==',' || S[i]=='%' || S[i]=='\t') 
        S[i] = '_';
      if (isdigit(S[i]))
        NumCharCnt++;
      }
    if (S[0]=='\'' || S[0]=='=')
      {
      S.InsertChar(0, NonNumericTagChr);
      NumCharCnt--;
      }
#if WithNumericTags
    if ((NumericStartingTagsBad && isdigit(S[0])) || (NumericTagsBad && len==NumCharCnt))
#else
    if ((len==NumCharCnt) || (NumericStartingTagsBad && isdigit(S[0])))
#endif
      {
      S.InsertChar(0, NonNumericTagChr);
      }
    }
  }

//---------------------------------------------------------------------------

INLINE flag TaggedObject::ValidateTagChanged(rStrng S)
  {//return true if the tag is altered in any way
  const int PrevLen = S.Len();
  S.Trim(" \t\n\r\v\f"); 
  const int len = S.Len();
  flag Chngd = (PrevLen!=len);
  if (len)
    {
    int NumCharCnt = 0;
    for (int i=0; i<len; i++)
      {
      if (S[i]==' ' || S[i]==',' || S[i]=='%' || S[i]=='\t') 
        {
        S[i] = '_';
        Chngd = 1;
        }
      if (isdigit(S[i]))
        NumCharCnt++;
      }
    if (S[0]=='\'' || S[0]=='=')
      {
      S.InsertChar(0, NonNumericTagChr);
      NumCharCnt--;
      Chngd = 1;
      }
#if WithNumericTags
    if ((NumericStartingTagsBad && isdigit(S[0])) || (NumericTagsBad && len==NumCharCnt))
#else
    if ((len==NumCharCnt) || (NumericStartingTagsBad && isdigit(S[0])))
#endif
      {
      S.InsertChar(0, NonNumericTagChr);
      Chngd = 1;
      }
    }
  return Chngd;
  }

//---------------------------------------------------------------------------

INLINE flag TaggedObject::ValidateTagDotChanged(rStrng S)
  {//return true if the tag is altered in any way
  const int PrevLen = S.Len();
  S.Trim(" \t\n\r\v\f"); 
  const int len = S.Len();
  flag Chngd = (PrevLen!=len);
  if (len)
    {
    int NumCharCnt = 0;
    for (int i=0; i<len; i++)
      {
      if (S[i]==' ' || S[i]=='.' || S[i]==',' || S[i]=='%' || S[i]=='\t')
        {
        S[i] = '_';
        Chngd = 1;
        }
      if (isdigit(S[i]))
        NumCharCnt++;
      }
    if (S[0]=='\'' || S[0]=='=')
      {
      S.InsertChar(0, NonNumericTagChr);
      NumCharCnt--;
      Chngd = 1;
      }
#if WithNumericTags
    if ((NumericStartingTagsBad && isdigit(S[0])) || (NumericTagsBad && len==NumCharCnt))
#else
    if ((len==NumCharCnt) || (NumericStartingTagsBad && isdigit(S[0])))
#endif
      {
      S.InsertChar(0, NonNumericTagChr);
      Chngd = 1;
      }
    }
  return Chngd;
  }

//---------------------------------------------------------------------------

#if WithNumericTags
INLINE int TaggedObject::TestValidTag(char* pTag, int NumStartBad, int NumBad)
  {
  if (pTag==NULL || pTag[0]==0)
    return 1;
  if (NumStartBad && isdigit(pTag[0]))
    return 2;
  if (pTag[0]=='\'' || pTag[0]=='=')
    return 3;
  if (strpbrk(pTag, " ,%\t\n\v\f\r"))
    return 4;
//  if (strchr(pTag, ' ') || 
//      strchr(pTag, ',') || 
//      strchr(pTag, '%') || 
//      strchr(pTag, '\t') ||
//      strchr(pTag, '\n') ||
//      strchr(pTag, '\v') ||
//      strchr(pTag, '\f') ||
//      strchr(pTag, '\r'))
//    return False;
  if (NumBad)
    {
    int Cnt = 0;
    while (pTag[Cnt])
      {
      if (!isdigit(pTag[Cnt++]))
        return 0;
      }
    return 5;
    }
  return 0;
  }

//---------------------------------------------------------------------------

INLINE int TaggedObject::TestValidTagDot(char* pTag, int NumStartBad, int NumBad)
  { 
  if (pTag==NULL || pTag[0]==0)
    return 1;
  if (NumStartBad && isdigit(pTag[0]))
    return 2;
  if (pTag[0]=='\'' || pTag[0]=='=')
    return 3;
  if (strpbrk(pTag, " .,%\t\n\v\f\r"))
    return 4;
  //if (strchr(pTag, ' ') || 
  //    strchr(pTag, '.') || 
  //    strchr(pTag, ',') || 
  //    strchr(pTag, '%') || 
  //    strchr(pTag, '\t') ||
  //    strchr(pTag, '\n') ||
  //    strchr(pTag, '\v') ||
  //    strchr(pTag, '\f') ||
  //    strchr(pTag, '\r'))
  //  return False;
  if (NumBad)
    {
    int Cnt = 0;
    while (pTag[Cnt])
      {
      if (!isdigit(pTag[Cnt++]))
        return 0;
      }
    return 5;
    }
  return 0;
  }
#else
INLINE int TaggedObject::TestValidTag(char* pTag, int NumStartBad)
  {
  if (pTag==NULL || pTag[0]==0)
    return 1;
  if (NumStartBad && isdigit(pTag[0]))
    return 2;
  if (pTag[0]=='\'' || pTag[0]=='=')
    return 3;
  if (strpbrk(pTag, " ,%\t\n\v\f\r{"))
    return 4;
//  if (strchr(pTag, ' ') || 
//      strchr(pTag, ',') || 
//      strchr(pTag, '%') || 
//      strchr(pTag, '\t') ||
//      strchr(pTag, '\n') ||
//      strchr(pTag, '\v') ||
//      strchr(pTag, '\f') ||
//      strchr(pTag, '\r'))
//    return False;
  int Cnt = 0;
  while (pTag[Cnt])
    {
    if (!isdigit(pTag[Cnt++]))
      return 0;
    }
  return 5;
  }

//---------------------------------------------------------------------------

INLINE int TaggedObject::TestValidTagDot(char* pTag, int NumStartBad)
  { 
  if (pTag==NULL || pTag[0]==0)
    return 1;
  if (NumStartBad && isdigit(pTag[0]))
    return 2;
  if (pTag[0]=='\'' || pTag[0]=='=')
    return 3;
  if (strpbrk(pTag, " .,%\t\n\v\f\r{"))
    return 4;
  //if (strchr(pTag, ' ') || 
  //    strchr(pTag, '.') || 
  //    strchr(pTag, ',') || 
  //    strchr(pTag, '%') || 
  //    strchr(pTag, '\t') ||
  //    strchr(pTag, '\n') ||
  //    strchr(pTag, '\v') ||
  //    strchr(pTag, '\f') ||
  //    strchr(pTag, '\r'))
  //  return False;
  int Cnt = 0;
  while (pTag[Cnt])
    {
    if (!isdigit(pTag[Cnt++]))
      return 0;
    }
  return 5;
  }
#endif
//========================================================================
//
//
//
//========================================================================
