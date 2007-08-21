
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
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class GraphicStencil1
  {

    public SizeF defaultSize;

    public FillMode fillMode;
    public String groupName;

    private ArrayList decorations;

    private ArrayList elements;
    private String tag;
    private RectangleF textArea;

    public GraphicStencil1()
    {
    }

    public ArrayList Decorations
    {
      get { return decorations; }

      set { decorations = value; }
    }

    public ArrayList Elements
    {
      get { return elements; }

      set { elements = value; }
    }

    public String Tag
    {
      get { return tag; }

      set { tag = value; }
    }

    public RectangleF TextArea
    {
      get { return textArea; }

      set { textArea = value; }
    }

  }
}
