using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using MindFusion.FlowChartX;
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
  public class OldGraphicStencil
  {
    private string tag;

    public ArrayList elements;
    public ArrayList decorations;
    public ArrayList textAreas;

    public FillMode fillMode;
    public SizeF defaultSize;
    public String groupName;

    public OldGraphicStencil()
    {
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public ShapeTemplate ShapeTemplate(bool mirrorX, bool mirrorY)
    {
      ElementTemplate[] elementTemplate = new ElementTemplate[elements.Count];
      {
        int i = 0;
        foreach (Element element in this.elements)
        {
          if (element is Arc)
          {
            elementTemplate[i] = new ArcTemplate(
              (element as Arc).x,
              (element as Arc).y,
              (element as Arc).w,
              (element as Arc).h,
              (element as Arc).a,
              (element as Arc).s);
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
      }

      ElementTemplate[] decorationTemplate = new ElementTemplate[decorations.Count];
      {
        int i = 0;
        foreach (Element decoration in this.decorations)
        {
          if (decoration is Arc)
          {
            decorationTemplate[i] = new ArcTemplate((decoration as Arc).x,
              (decoration as Arc).y,
              (decoration as Arc).w,
              (decoration as Arc).h,
              (decoration as Arc).a,
              (decoration as Arc).s);
          }
          if (decoration is Line)
          {
            decorationTemplate[i] = new LineTemplate((decoration as Line).x1,
              (decoration as Line).y1,
              (decoration as Line).x2,
              (decoration as Line).y2);
          }
          if (decoration is Bezier)
          {
            decorationTemplate[i] = new BezierTemplate((decoration as Bezier).x1,
              (decoration as Bezier).y1,
              (decoration as Bezier).x2,
              (decoration as Bezier).y2,
              (decoration as Bezier).x3,
              (decoration as Bezier).y3,
              (decoration as Bezier).x4,
              (decoration as Bezier).y4);
          }
          i++;
        }
      }

      System.Drawing.Drawing2D.FillMode fillMode = this.fillMode;
      string id = this.tag;

      return (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], fillMode, id));
    }

    //public ShapeTemplate TextShapeTemplate(bool mirrorX, bool mirrorY)
    //{
    //  ElementTemplate[] textAreaTemplate = new ElementTemplate[textAreas.Count];
    //  {
    //    int i = 0;
    //    foreach (Element textArea in this.textAreas)
    //    {
    //      if (textArea is Arc)
    //      {
    //        textAreaTemplate[i] = new ArcTemplate((textArea as Arc).x,
    //          (textArea as Arc).y,
    //          (textArea as Arc).w,
    //          (textArea as Arc).h,
    //          (textArea as Arc).a,
    //          (textArea as Arc).s);
    //      }
    //      if (textArea is Line)
    //      {
    //        textAreaTemplate[i] = new LineTemplate((textArea as Line).x1,
    //          (textArea as Line).y1,
    //          (textArea as Line).x2,
    //          (textArea as Line).y2);
    //      }
    //      if (textArea is Bezier)
    //      {
    //        textAreaTemplate[i] = new BezierTemplate((textArea as Bezier).x1,
    //          (textArea as Bezier).y1,
    //          (textArea as Bezier).x2,
    //          (textArea as Bezier).y2,
    //          (textArea as Bezier).x3,
    //          (textArea as Bezier).y3,
    //          (textArea as Bezier).x4,
    //          (textArea as Bezier).y4);
    //      }
    //      i++;
    //    }
    //  }

    //  string id = this.tag+"_textArea";

    //  return (new ShapeTemplate(textAreaTemplate, new ElementTemplate[0], new ElementTemplate[0], fillMode, id));
    //}

  }
}
