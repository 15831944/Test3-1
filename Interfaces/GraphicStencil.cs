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
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class GraphicStencil
  {
    private string tag;

    public ArrayList elements;
    public ArrayList decorations;
    public RectangleF textArea;

    public FillMode fillMode;
    public SizeF defaultSize;
    public String groupName;

    public GraphicStencil()
    {
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }
  }
}
