#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

enum GR_Methods { GRM_None, GRM_RB, GRM_Gibbs };

class GibbsReactor : public MBaseMethod
  {
  public:
    GibbsReactor(TaggedObject * pNd); //constructor
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();

  private:
    void GibbsReaction(MStream & Stream);

  protected:
    //inputs...
    GR_Methods      m_iReactionMethod;
    MReactionBlk    m_RB;
    CGibbsHelper    m_GH;

    //outputs...
    CString         m_sStatus1;
    long            m_iIterations;
    double          m_dGMin;
    double          m_dFeedT;
    double          m_dProdT;
  };

//---------------------------------------------------------------------------
