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

    public ShapeTemplate ShapeTemplate(bool mirrorX, bool mirrorY)
    {
      int i;

      ElementTemplate[] elementTemplate = new ElementTemplate[elements.Count];
      i = 0;
      foreach (Element element in elements)
      {
        if (element is Arc)
        {
          Arc arc = element as Arc;
          elementTemplate[i] = new ArcTemplate(arc.x, arc.y, arc.w, arc.h, arc.a, arc.s);
        }
        if (element is Line)
        {
          Line line = element as Line;
          float x1, y1, x2, y2;

          if (mirrorX)
            x1 = 100.0F - line.x1;
          else
            x1 = line.x1;

          if (mirrorY)
            y1 = 100.0F - line.y1;
          else
            y1 = line.y1;

          if (mirrorX)
            x2 = 100.0F - line.x2;
          else
            x2 = line.x2;

          if (mirrorY)
            y2 = 100.0F - line.y2;
          else
            y2 = line.y2;

          elementTemplate[i] = new LineTemplate(x1, y1, x2, y2);
        }
        if (element is Bezier)
        {
          Bezier bezier = element as Bezier;
          float x1, y1, x2, y2, x3, y3, x4, y4;

          if (mirrorX)
            x1 = 100.0F - bezier.x1;
          else
            x1 = bezier.x1;

          if (mirrorY)
            y1 = 100.0F - bezier.y1;
          else
            y1 = bezier.y1;

          if (mirrorX)
            x2 = 100.0F - bezier.x2;
          else
            x2 = bezier.x2;

          if (mirrorY)
            y2 = 100.0F - bezier.y2;
          else
            y2 = bezier.y2;

          if (mirrorX)
            x3 = 100.0F - bezier.x3;
          else
            x3 = bezier.x3;

          if (mirrorY)
            y3 = 100.0F - bezier.y3;
          else
            y3 = bezier.y3;

          if (mirrorX)
            x4 = 100.0F - bezier.x4;
          else
            x4 = bezier.x4;

          if (mirrorY)
            y4 = 100.0F - bezier.y4;
          else
            y4 = bezier.y4;

          elementTemplate[i] = new BezierTemplate(x1, y1, x2, y2, x3, y3, x4, y4);
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
          float x1, y1, x2, y2;

          if (mirrorX)
            x1 = 100.0F - line.x1;
          else
            x1 = line.x1;

          if (mirrorY)
            y1 = 100.0F - line.y1;
          else
            y1 = line.y1;

          if (mirrorX)
            x2 = 100.0F - line.x2;
          else
            x2 = line.x2;

          if (mirrorY)
            y2 = 100.0F - line.y2;
          else
            y2 = line.y2;

          decorationTemplate[i] = new LineTemplate(x1, y1, x2, y2);
        }
        if (decoration is Bezier)
        {
          Bezier bezier = decoration as Bezier;
          float x1, y1, x2, y2, x3, y3, x4, y4;

          if (mirrorX)
            x1 = 100.0F - bezier.x1;
          else
            x1 = bezier.x1;

          if (mirrorY)
            y1 = 100.0F - bezier.y1;
          else
            y1 = bezier.y1;

          if (mirrorX)
            x2 = 100.0F - bezier.x2;
          else
            x2 = bezier.x2;

          if (mirrorY)
            y2 = 100.0F - bezier.y2;
          else
            y2 = bezier.y2;

          if (mirrorX)
            x3 = 100.0F - bezier.x3;
          else
            x3 = bezier.x3;

          if (mirrorY)
            y3 = 100.0F - bezier.y3;
          else
            y3 = bezier.y3;

          if (mirrorX)
            x4 = 100.0F - bezier.x4;
          else
            x4 = bezier.x4;

          if (mirrorY)
            y4 = 100.0F - bezier.y4;
          else
            y4 = bezier.y4;

          decorationTemplate[i] = new BezierTemplate(x1, y1, x2, y2, x3, y3, x4, y4);
        }
        i++;
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, null, fillMode, tag));
    }
  }
}