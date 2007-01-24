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
  public class ModelStencil
  {
    private String tag;

    private ArrayList elements;
    private ArrayList decorations;
    private ArrayList anchors;
    private FillMode fillMode;
    private String groupName;

    public ModelStencil()
    {
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

    public ArrayList Anchors
    {
      get { return anchors; }
      set { anchors = value; }
    }


    public FillMode FillMode
    {
      get { return fillMode; }
      set { fillMode = value; }
    }
  }
}