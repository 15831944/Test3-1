using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using System.Drawing;
using System.Drawing.Drawing2D;
using MindFusion.FlowChartX;
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
    public ArrayList elements = new ArrayList();
    public ArrayList decorations = new ArrayList();
    public ArrayList anchors = new ArrayList();
    public FillMode fillMode;
    public String groupName;
    public string id;

    public ModelStencil()
    {
      fillMode = FillMode.Alternate;
      id = "";
    }

    public ArrayList Anchors
    {
      get
      {
        return anchors;
      }
    }

    public ShapeTemplate ShapeTemplate(bool mirrorX, bool mirrorY)
    {
      float multX = 1.0F;
      float multY = 1.0F;

      if (mirrorX)
        multX = -1.0F;
      if (mirrorY)
        multY = -1.0F;

      int i;

      ElementTemplate[] elementTemplate = new ElementTemplate[elements.Count];
      i = 0;
      foreach (Element element in elements)
      {
        if (element is Arc)
        {
          Arc arc = element as Arc;
          elementTemplate[i] = new ArcTemplate(arc.x, arc.y, arc.w, arc.h, arc.a, arc.x);
        }
        if (element is Line)
        {
          Line line = element as Line;
          elementTemplate[i] = new LineTemplate(line.x1, line.y1, line.x2, line.y2);
        }
        if (element is Bezier)
        {
          Bezier bezier = element as Bezier;
          elementTemplate[i] = new BezierTemplate(bezier.x1, bezier.y1, bezier.x2, bezier.y2, bezier.x3, bezier.y3, bezier.x4, bezier.y4);
        }
        i++;
      }

      ElementTemplate[] decorationTemplate = new ElementTemplate[decorations.Count];
      i = 0;
      foreach (Element decoration in decorations)
      {
        if (decoration is Arc)
        {
          Arc arc = decoration as Arc;
          decorationTemplate[i] = new ArcTemplate(arc.x, arc.y, arc.w, arc.h, arc.a, arc.x);
        }
        if (decoration is Line)
        {
          Line line = decoration as Line;
          decorationTemplate[i] = new LineTemplate(line.x1, line.y1, line.x2, line.y2);
        }
        if (decoration is Bezier)
        {
          Bezier bezier = decoration as Bezier;
          decorationTemplate[i] = new BezierTemplate(bezier.x1, bezier.y1, bezier.x2, bezier.y2, bezier.x3, bezier.y3, bezier.x4, bezier.y4);
        }
        i++;
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, null, fillMode, id));
    }
  }
}