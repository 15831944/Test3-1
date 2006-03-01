using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using System.Drawing;
using System.Drawing.Drawing2D;
using MindFusion.FlowChartX;

namespace SysCAD.Interface
{
  /// <summary>
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class ModelStencil
  {
    private Dictionary<string, Element> elements = new Dictionary<string, Element>();
    private Dictionary<string, Element> decorations = new Dictionary<string, Element>();
    private Dictionary<string, Element> textAreas = new Dictionary<string, Element>();
    private Dictionary<string, Point> anchors = new Dictionary<string, Point>();
    private FillMode fillMode;
    private string id;

    public ModelStencil()
    {
      fillMode = FillMode.Alternate;
      id = "";
    }

    public Dictionary<string, Point> Anchors
    {
      get
      {
        return anchors;
      }
    }

    public ShapeTemplate ShapeTemplate()
    {
      int i;

      ElementTemplate[] elementTemplate = new ElementTemplate[elements.Count];
      i = 0;
      foreach (Element element in elements.Values)
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
      foreach (Element decoration in decorations.Values)
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

      ElementTemplate[] textAreaTemplate = new ElementTemplate[textAreas.Count];
      i = 0;
      foreach (Element textArea in textAreas.Values)
      {
        if (textArea is Arc)
        {
          Arc arc = textArea as Arc;
          textAreaTemplate[i] = new ArcTemplate(arc.x, arc.y, arc.w, arc.h, arc.a, arc.x);
        }
        if (textArea is Line)
        {
          Line line = textArea as Line;
          textAreaTemplate[i] = new LineTemplate(line.x1, line.y1, line.x2, line.y2);
        }
        if (textArea is Bezier)
        {
          Bezier bezier = textArea as Bezier;
          textAreaTemplate[i] = new BezierTemplate(bezier.x1, bezier.y1, bezier.x2, bezier.y2, bezier.x3, bezier.y3, bezier.x4, bezier.y4);
        }
        i++;
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, textAreaTemplate, fillMode, id));
    }
  }
}