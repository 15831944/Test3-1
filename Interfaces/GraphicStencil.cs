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
  public class GraphicStencil
  {
    public ArrayList elements;
    public ArrayList decorations;
    public ArrayList textAreas;
    public FillMode fillMode;
    public SizeF defaultSize;
    public String groupName;
    public string id;

    public GraphicStencil()
    {
      elements = new ArrayList();
      decorations = new ArrayList();
      textAreas = new ArrayList();
    }

    public ShapeTemplate ShapeTemplate()
    {
      ElementTemplate[] elementTemplate = new ElementTemplate[elements.Count];
      {
        int i = 0;
        foreach (Element element in this.elements)
        {
          if (element is Arc)
          {
            elementTemplate[i] = new ArcTemplate((element as Arc).x,
              (element as Arc).y,
              (element as Arc).w,
              (element as Arc).h,
              (element as Arc).a,
              (element as Arc).x);
          }
          if (element is Line)
          {
            elementTemplate[i] = new LineTemplate((element as Line).x1,
              (element as Line).y1,
              (element as Line).x2,
              (element as Line).y2);
          }
          if (element is Bezier)
          {
            elementTemplate[i] = new BezierTemplate((element as Bezier).x1,
              (element as Bezier).y1,
              (element as Bezier).x2,
              (element as Bezier).y2,
              (element as Bezier).x3,
              (element as Bezier).y3,
              (element as Bezier).x4,
              (element as Bezier).y4);
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
              (decoration as Arc).x);
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

      ElementTemplate[] textAreaTemplate = new ElementTemplate[textAreas.Count];
      {
        int i = 0;
        foreach (Element textArea in this.textAreas)
        {
          if (textArea is Arc)
          {
            textAreaTemplate[i] = new ArcTemplate((textArea as Arc).x,
              (textArea as Arc).y,
              (textArea as Arc).w,
              (textArea as Arc).h,
              (textArea as Arc).a,
              (textArea as Arc).x);
          }
          if (textArea is Line)
          {
            textAreaTemplate[i] = new LineTemplate((textArea as Line).x1,
              (textArea as Line).y1,
              (textArea as Line).x2,
              (textArea as Line).y2);
          }
          if (textArea is Bezier)
          {
            textAreaTemplate[i] = new BezierTemplate((textArea as Bezier).x1,
              (textArea as Bezier).y1,
              (textArea as Bezier).x2,
              (textArea as Bezier).y2,
              (textArea as Bezier).x3,
              (textArea as Bezier).y3,
              (textArea as Bezier).x4,
              (textArea as Bezier).y4);
          }
        }
      }

      System.Drawing.Drawing2D.FillMode fillMode = this.fillMode;
      string id = this.id;

      return (new ShapeTemplate(elementTemplate, decorationTemplate, textAreaTemplate, fillMode, id));
    }

//    [STAThread]
//    static void Main() 
//    {
//      GraphicStencil graphicStencil = new GraphicStencil();
//      graphicStencil.elements.Add(new Arc(1, 1, 1, 1, 1, 1));
//      graphicStencil.elements.Add(new Line(1, 2, 3, 4));
//      graphicStencil.elements.Add(new Bezier(1, 2, 3, 4, 6, 7, 8, 9));
//
//      ShapeTemplate shapeTemplate = graphicStencil.ShapeTemplate();
//
//      System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(GraphicStencil));
//
//      TextWriter writer = new StreamWriter("test.stencil");
//      ser.Serialize(writer, graphicStencil);
//      writer.Close();
//    }
  }
}
