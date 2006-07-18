using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using System.Drawing;
using System.Drawing.Drawing2D;
//using MindFusion.FlowChartX;
using System.Collections;

namespace SysCAD.Interface
{
  /// <summary>
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class ModelStencil
  {
    private string tag;

    public ArrayList elements;
    public ArrayList decorations;
    public ArrayList anchors;
    public FillMode fillMode;
    public String groupName;

    public ModelStencil()
    {
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public ArrayList Anchors
    {
      get
      {
        return anchors;
      }
    }
  }
}