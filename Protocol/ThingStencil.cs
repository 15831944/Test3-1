
using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;

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

    private String tag;
    private Size defaultSize = new Size(10.0, 10.0);
    private String xaml;

    public ThingStencil()
    {
    }

    public override string ToString()
    {
      return tag;
    }

    public Size DefaultSize
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
