using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using MindFusion.FlowChartX;

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
    public ArrayList textArea;
    public System.Drawing.Drawing2D.FillMode fillMode;
    public string id;

    public GraphicStencil()
    {
      elements = new ArrayList();
      decorations = new ArrayList();
      textArea = new ArrayList();
    }

    public ShapeTemplate ShapeTemplate()
    {
      ElementTemplate[] elements = new ElementTemplate[this.elements.Count];
      for (int i=0; i<this.elements.Count; i++)
      {
        if (this.elements[i] is Arc)
        {
          elements[i] = new ArcTemplate((this.elements[i] as Arc).x, 
            (this.elements[i] as Arc).y, 
            (this.elements[i] as Arc).w, 
            (this.elements[i] as Arc).h, 
            (this.elements[i] as Arc).a,
            (this.elements[i] as Arc).x);
        }
        if (this.elements[i] is Line)
        {
          elements[i] = new LineTemplate((this.elements[i] as Line).x1, 
            (this.elements[i] as Line).y1, 
            (this.elements[i] as Line).x2, 
            (this.elements[i] as Line).y2);
        }
        if (this.elements[i] is Bezier)
        {
          elements[i] = new BezierTemplate((this.elements[i] as Bezier).x1, 
            (this.elements[i] as Bezier).y1, 
            (this.elements[i] as Bezier).x2, 
            (this.elements[i] as Bezier).y2, 
            (this.elements[i] as Bezier).x3, 
            (this.elements[i] as Bezier).y3, 
            (this.elements[i] as Bezier).x4, 
            (this.elements[i] as Bezier).y4);
        }
      }

      ElementTemplate[] decorations = new ElementTemplate[this.decorations.Count];
      for (int i=0; i<this.decorations.Count; i++)
      {
        if (this.decorations[i] is Arc)
        {
          decorations[i] = new ArcTemplate((this.decorations[i] as Arc).x, 
            (this.decorations[i] as Arc).y, 
            (this.decorations[i] as Arc).w, 
            (this.decorations[i] as Arc).h, 
            (this.decorations[i] as Arc).a,
            (this.decorations[i] as Arc).x);
        }
        if (this.decorations[i] is Line)
        {
          decorations[i] = new LineTemplate((this.decorations[i] as Line).x1, 
            (this.decorations[i] as Line).y1, 
            (this.decorations[i] as Line).x2, 
            (this.decorations[i] as Line).y2);
        }
        if (this.decorations[i] is Bezier)
        {
          decorations[i] = new BezierTemplate((this.decorations[i] as Bezier).x1, 
            (this.decorations[i] as Bezier).y1, 
            (this.decorations[i] as Bezier).x2, 
            (this.decorations[i] as Bezier).y2, 
            (this.decorations[i] as Bezier).x3, 
            (this.decorations[i] as Bezier).y3, 
            (this.decorations[i] as Bezier).x4, 
            (this.decorations[i] as Bezier).y4);
        }
      }

      ElementTemplate[] textArea = new ElementTemplate[this.textArea.Count];
      for (int i=0; i<this.textArea.Count; i++)
      {
        if (this.textArea[i] is Arc)
        {
          textArea[i] = new ArcTemplate((this.textArea[i] as Arc).x, 
            (this.textArea[i] as Arc).y, 
            (this.textArea[i] as Arc).w, 
            (this.textArea[i] as Arc).h, 
            (this.textArea[i] as Arc).a,
            (this.textArea[i] as Arc).x);
        }
        if (this.textArea[i] is Line)
        {
          textArea[i] = new LineTemplate((this.textArea[i] as Line).x1, 
            (this.textArea[i] as Line).y1, 
            (this.textArea[i] as Line).x2, 
            (this.textArea[i] as Line).y2);
        }
        if (this.textArea[i] is Bezier)
        {
          textArea[i] = new BezierTemplate((this.textArea[i] as Bezier).x1, 
            (this.textArea[i] as Bezier).y1, 
            (this.textArea[i] as Bezier).x2, 
            (this.textArea[i] as Bezier).y2, 
            (this.textArea[i] as Bezier).x3, 
            (this.textArea[i] as Bezier).y3, 
            (this.textArea[i] as Bezier).x4, 
            (this.textArea[i] as Bezier).y4);
        }
      }

      System.Drawing.Drawing2D.FillMode fillMode = this.fillMode;
      string id = this.id;

      return (new ShapeTemplate(elements, decorations, textArea, fillMode, id));
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
