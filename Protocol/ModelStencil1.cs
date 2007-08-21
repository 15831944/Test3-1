
using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using System.Drawing;
using System.Drawing.Drawing2D;

//using MindFusion.FlowChartX;
using System.Collections;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class ModelStencil1
  {

    private ArrayList anchors;
    private ArrayList decorations;

    private ArrayList elements;
    private FillMode fillMode;
    private String groupName;
    private String tag;

    public ModelStencil1()
    {
    }

    public ArrayList Anchors
    {
      get { return anchors; }
      set { anchors = value; }
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

    public FillMode FillMode
    {
      get { return fillMode; }
      set { fillMode = value; }
    }

    public String GroupName
    {
      get { return groupName; }
      set { groupName = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }
  }
}
