using System;
using System.Collections.Generic;
using System.Windows.Forms;
using SysCAD.Protocol;
using System.Drawing;
using MindFusion.FlowChartX;

namespace TestApp
{
  static class Program
  {
    static private ElementTemplate Element(object element)
    {
      SysCAD.Protocol.Line line = element as SysCAD.Protocol.Line;

      if (line != null) return new LineTemplate(line.x1, line.y1,
                                                line.x2, line.y2);

      Arc arc = element as Arc;

      if (arc != null) return new ArcTemplate(arc.x, arc.y,
                                              arc.w, arc.h, arc.a, arc.s);

      Bezier bezier = element as Bezier;

      if (bezier != null) return new BezierTemplate(bezier.x1, bezier.y1,
                                                    bezier.x2, bezier.y2,
                                                    bezier.x3, bezier.y3,
                                                    bezier.x4, bezier.y4);

      return null;
    }

    static public ShapeTemplate GetShapeTemplate(ModelStencil stencil)
    {
      int i;

      if (stencil != null)
      {
        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        i = 0;

        foreach (Element element in stencil.Elements)
        {
          elementTemplate[i] = Element(element);
          i++;
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        i = 0;

        foreach (Element decoration in stencil.Decorations)
        {
          decorationTemplate[i] = Element(decoration);
          i++;
        }

        return (new ShapeTemplate(elementTemplate, decorationTemplate, null, stencil.FillMode, stencil.Tag));
      }

      else
        return null;
    }

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      FlowChart flowchart16 = new FlowChart();
      FlowChart flowchart32 = new FlowChart();

      Config config = new Config();
      if (config.TestUrl(new System.Uri("ipc://SysCAD.Service/Global")))
      {
        config.Syncxxx();

        Dictionary<String, Bitmap> thumbnails16 = new Dictionary<String, Bitmap>();
        Dictionary<String, Bitmap> thumbnails32 = new Dictionary<String, Bitmap>();

        foreach (String key in config.ModelStencils.Keys)
        {
          ModelStencil stencil = config.ModelStencils[key];
          {
            flowchart16.DocExtents = flowchart16.ClientToDoc(new Rectangle(0, 0, 17, 17));
            flowchart16.ShadowsStyle = ShadowsStyle.None;
            flowchart16.BackColor = Color.White;
            flowchart16.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            RectangleF boxRect = flowchart16.ClientToDoc(new Rectangle(0, 0, 15, 15));
            Box box16 = flowchart16.CreateBox(boxRect.X, boxRect.Y, boxRect.Width, boxRect.Height);
            box16.Style = BoxStyle.Shape;
            box16.Shape = GetShapeTemplate(stencil);
            box16.Locked = true;
            thumbnails16.Add(key, flowchart16.CreateImage());
            flowchart16.DeleteObject(box16);
          }

          {
            flowchart32.DocExtents = flowchart32.ClientToDoc(new Rectangle(0, 0, 33, 33));
            flowchart32.ShadowsStyle = ShadowsStyle.None;
            flowchart32.BackColor = Color.White;
            flowchart32.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            RectangleF boxRect = flowchart32.ClientToDoc(new Rectangle(0, 0, 31, 31));
            Box box32 = flowchart32.CreateBox(boxRect.X, boxRect.Y, boxRect.Width, boxRect.Height);
            box32.Style = BoxStyle.Shape;
            box32.Shape = GetShapeTemplate(stencil);
            box32.Locked = true;
            thumbnails32.Add(key, flowchart32.CreateImage());
            flowchart32.DeleteObject(box32);
          }
        }

        Application.Run(new TestAppForm(thumbnails16, thumbnails32));
      }
    }
  }
}