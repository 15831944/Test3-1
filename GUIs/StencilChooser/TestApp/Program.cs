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

    static public ShapeTemplate GetShapeTemplate(GraphicStencil stencil)
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

        return (new ShapeTemplate(elementTemplate, decorationTemplate, null, stencil.fillMode, stencil.Tag));
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

      FlowChart flowchart = new FlowChart();

      Config config = new Config();
      if (config.TestUrl(new System.Uri("ipc://SysCAD.Service/Global")))
      {
        config.Syncxxx();

        Dictionary<String, Bitmap> modelThumbnails = new Dictionary<String, Bitmap>();
        Dictionary<String, Bitmap> graphicThumbnails = new Dictionary<String, Bitmap>();

        foreach (String key in config.ModelStencils.Keys)
        {
          ModelStencil stencil = config.ModelStencils[key];
          flowchart.DocExtents = flowchart.ClientToDoc(new Rectangle(0, 0, 17, 17));
          flowchart.ShadowsStyle = ShadowsStyle.None;
          flowchart.BackColor = System.Drawing.SystemColors.Window;
          flowchart.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
          RectangleF boxRect = flowchart.ClientToDoc(new Rectangle(1, 1, 13, 13));
          Box box = flowchart.CreateBox(boxRect.X, boxRect.Y, boxRect.Width, boxRect.Height);
          box.Style = BoxStyle.Shape;
          box.Shape = GetShapeTemplate(stencil);
          box.FillColor = System.Drawing.Color.FromArgb(150, System.Drawing.Color.BurlyWood);
          box.FrameColor = System.Drawing.Color.FromArgb(255, System.Drawing.Color.BurlyWood);
          box.Locked = true;
          modelThumbnails.Add(key, flowchart.CreateImage());
          flowchart.DeleteObject(box);
        }

        Application.Run(new TestAppForm(modelThumbnails, config.ModelStencils));
      }
    }
  }
}