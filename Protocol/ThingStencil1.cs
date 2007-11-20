
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
  public class ThingStencil1
  {

    private SizeF defaultSize = new SizeF(10.0F, 10.0F);

    private String tag;

    private String xaml;

    public ThingStencil1()
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
