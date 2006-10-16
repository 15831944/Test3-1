//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __WHITENCRUSHER_H
#define  __WHITENCRUSHER_H

//---------------------------------------------------------------------------

class CWhitenCrusher : public MBaseMethod
  {
  public:
    CWhitenCrusher(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual void    EvalProducts();
    virtual bool    GetModelAction(CMdlActionArray & Acts);
    virtual bool    SetModelAction(CMdlAction & Act);
    virtual bool    GetModelGraphic(CMdlGraphicArray & Grfs);
    virtual bool    OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);

  protected:

    C_ModelParameters_DiamondWizard_WhitenCrusher m_DWParms;
		C_ModelReturns_DiamondWizard_WhitenCrusher    m_DWReturns;	

    bool          m_LBtnDn;
    bool          m_RBtnDn;
    CPoint        m_MousePt;
  };



#endif