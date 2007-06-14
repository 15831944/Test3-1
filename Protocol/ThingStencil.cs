
using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;

//using MindFusion.FlowChartX;
using System.Drawing;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for ThingStencil.
  /// </summary>
  [Serializable]
  public class ThingStencil
  {

    private SizeF defaultSize;

    private Model model;
    private String tag;

    private String xaml;

    public ThingStencil()
    {
    }

    public override string ToString()
    {
      return tag;
    }

    public SizeF DefaultSize
    {
      get { return defaultSize; }
    }

    public Model Model
    {
      get { return model; }
      set { model = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public String Xaml
    {
      get { return xaml; }
      set { xaml = value; }
    }

  }
}
