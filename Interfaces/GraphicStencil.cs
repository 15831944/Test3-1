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
    private String tag;

    private ArrayList elements;
    private ArrayList decorations;
    private RectangleF textArea;

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

    public ArrayList Elements
    {
      get { return elements; }
      set { elements = value; }
    }

    public ArrayList Decorations
    {
      get { return decorations; }
      set { decorations = value; }
    }

    public RectangleF TextArea
    {
      get { return textArea; }
      set { textArea = value; }
    }

  }
}
