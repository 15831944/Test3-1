#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <sstream>


//---------------------------------------------------------------------------

const static int maxItems = 20;
const static int maxData = 5;



class CQALPipe : public MBaseMethod
{
  public:
    CQALPipe(MUnitDefBase *pUnitDef, TaggedObject * pNd);
    virtual ~CQALPipe(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    //    virtual bool    ExchangeDataFields();
    virtual bool    ConfigureJoins();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void    ClosureInfo(MClosureInfo & CI);
    virtual bool    ValidateDataFields();
  protected:
    
    long m_plItemArray[maxItems];
    
    
    double m_dMassFlow;
    long m_lItems;     // Number of items
    double m_dQv;      // Volume Flow
    long m_lPipeMode;   
    double findDP();
    double m_dP;


    double m_dEntryHead;
    double mItemData[maxItems][maxData];
    
};
