//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SIMPLEMIXER_H
#define  __SIMPLEMIXER_H

//---------------------------------------------------------------------------

class CSimpleMixer : public MBaseMethod
  {
  public:
    CSimpleMixer(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual void    EvalProducts();
    virtual bool    GetModelAction(CMdlActionArray & Acts);
    virtual bool    SetModelAction(CMdlAction & Act);
    virtual bool    GetModelGraphic(CMdlGraphicArray & Grfs);
    virtual bool    OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);

  protected:

    C_ModelParameters_DiamondWizard_SimpleMixer m_DWParms;

    bool          m_LBtnDn;
    bool          m_RBtnDn;
    CPoint        m_MousePt;
  };



#endif