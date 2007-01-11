using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
//using MindFusion.FlowChartX;
using System.Drawing;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

namespace SysCAD.Interface
{
  /// <summary>
  /// Summary description for ThingStencil.
  /// </summary>
  [Serializable]
  public class ThingStencil
  {
    private String tag;

    private String xaml;
    private Model model;

    public SizeF defaultSize;

    public ThingStencil()
    {
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

    public Model Model
    {
      get { return model; }
      set { model = value; }
    }

    public override string ToString()
    {
      return tag;
    }

  }
}
