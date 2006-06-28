using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Interface;
using System.Collections;
using System.Runtime.Serialization.Formatters.Soap;
using System.IO;

using MindFusion.FlowChartX;

/*
MDrw_Poly 10.0, 5.0, 5.0, 10.0
MDrw_Arc 10.0, 10.0, 0.5
MDrw_Arc2 0.0, 0.0, 10.0, 10.0, 20.0, 70.0
*/

namespace StencilEditor
{
  public partial class Form1 : Form
  {
    string tempText = "";

    string filename = "";

    private GraphicStencil graphicStencil = new GraphicStencil();

    private string[] stringSplitArray = { "MDrw_" };
    private char[] charSplitArray = { ',', ' ', '\r', '\n' };

    MindFusion.FlowChartX.Box box;

    float minX;
    float maxX;
    float minY;
    float maxY;

    float scale;
    
    double det(double x11, double x21, double x31,
              double x12, double x22, double x32,
              double x13, double x23, double x33)
    {
      return x11 * x22 * x33
           + x13 * x21 * x32 
           + x12 * x23 * x31 
           - x13 * x22 * x31 
           - x11 * x23 * x32 
           - x12 * x21 * x33;
    }

    public Form1()
    {
      InitializeComponent();

      newToolStripMenuItem_Click(null, null);
    }

    public static bool IsEven(int intValue)
    {
      return ((intValue & 1) == 0);
    }

    public static bool IsOdd(int intValue)
    {
      return ((intValue & 1) == 1);
    }
    
    private bool IsPrimitive(string atom)
    {
      if ((atom == "MDrw_Poly") || (atom == "MDrw_Arc") || (atom == "MDrw_Arc3") || (atom == "MDrw_End"))
        return true;

      return false;
    }

    private void textBox_TextChanged(object sender, EventArgs e)
    {
      toolStripStatusLabel1.Text = "";
      toolStripStatusLabel1.BackColor = DefaultBackColor;

      Parse(graphicStencil.elements, elementTextBox);
      Parse(graphicStencil.decorations, decorationTextBox);
      UpdateStencil(graphicStencil);
    }

    private void Parse(ArrayList arrayList, TextBox textBox)
    {
      arrayList.Clear();

      string[] atoms = textBox.Text.Split(stringSplitArray, System.StringSplitOptions.RemoveEmptyEntries);
      foreach (string atom in atoms)
      {
        if (atom.StartsWith("Poly"))
        {
          if (!ParsePoly(atom, arrayList))
            return;
        }
        else if (atom.StartsWith("Arc2"))
        {
          if (!ParseArc2(atom, arrayList))
            return;
        }
        else if (atom.StartsWith("Arc3"))
        {
          if (!ParseArc3(atom, arrayList))
            return;
        }
        else if (atom.StartsWith("Arc"))
        {
          if (!ParseArc(atom, arrayList))
            return;
        }
        else if (atom.StartsWith("Bezier"))
        {
          if (!ParseBezier(atom, arrayList))
            return;
        }
        else
        {
          toolStripStatusLabel1.Text = "Unknown atom: " + atom;
          toolStripStatusLabel1.BackColor = Color.Yellow;
          return;
        }
      }
    }

    private bool ParsePoly(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if ((IsOdd(atoms.Length)) || (atoms.Length < 4))
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x1 = float.Parse(atoms[0]);
        float y1 = float.Parse(atoms[1]);
        int i = 2;
        while (i < atoms.Length)
        {
          float x2 = float.Parse(atoms[i]);
          float y2 = float.Parse(atoms[i + 1]);
          i += 2;

          Line line = new Line(x1, y1, x2, y2);

          arrayList.Add(line);

          x1 = x2;
          y1 = y2;
        }
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool ParseArc(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 3);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if (atoms.Length != 3)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x = float.Parse(atoms[0]);
        float y = float.Parse(atoms[1]);
        float r = Math.Abs(float.Parse(atoms[2]));

        Arc arc = new Arc(x - r, y - r, 2.0F * r, 2.0F * r, 0.0F, 360.0F);

        arrayList.Add(arc);
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool ParseArc2(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if (atoms.Length != 6)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x = float.Parse(atoms[0]);
        float y = float.Parse(atoms[1]);
        float w = float.Parse(atoms[2]);
        float h = float.Parse(atoms[3]);
        float a = float.Parse(atoms[4]);
        while (a < 0.0F) a += 360.0F;
        while (a > 360.0F) a -= 360.0F;
        float s = float.Parse(atoms[5]);

        Arc arc = new Arc(x, y, w, h, a, s);

        arrayList.Add(arc);
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool ParseArc3(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if (atoms.Length != 6)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x1 = float.Parse(atoms[0]);
        float y1 = float.Parse(atoms[1]);
        float x2 = float.Parse(atoms[2]);
        float y2 = float.Parse(atoms[3]);
        float x3 = float.Parse(atoms[4]);
        float y3 = float.Parse(atoms[5]);

        //a = 2*det([1,1,1;x1,x2,x3;y1,y2,y3]);
        double a = 2.0 * det(1.0, 1.0, 1.0, x1, x2, x3, y1, y2, y3);

        //r = sqrt( ((x2-x3)^2+(y2-y3)^2) ...
        float r = (float)(Math.Sqrt((Math.Pow(x2 - x3, 2) + Math.Pow(y2 - y3, 2))
          // * ((x3-x1)^2+(y3-y1)^2)
                             * (Math.Pow(x3 - x1, 2) + Math.Pow(y3 - y1, 2))
          // * ((x1-x2)^2+(y1-y2)^2) )
                             * (Math.Pow(x1 - x2, 2) + Math.Pow(y1 - y2, 2)))
          // / abs(a);
                   / Math.Abs(a));

        //xc = - det([1,1,1;y1,y2,y3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;
        float xc = (float)(-det(1.0, 1.0, 1.0,
                                y1, y2, y3,
                                x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / a);

        //yc =   det([1,1,1;x1,x2,x3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;
        float yc = (float)(det(1.0, 1.0, 1.0,
                                x1, x2, x3,
                                x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / a);

        float a3 = (float)(Math.Atan2(yc - y3, xc - x3) * 180 / Math.PI) + 180.0F;
        float a1 = (float)(Math.Atan2(yc - y1, xc - x1) * 180 / Math.PI) + 180.0F;


        Arc arc = new Arc(xc - r, yc - r, 2.0F * r, 2.0F * r, a1, a3 - a1);

        arrayList.Add(arc);
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool ParseBezier(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 6);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if (atoms.Length != 8)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x1 = float.Parse(atoms[0]);
        float y1 = float.Parse(atoms[1]);
        float x2 = float.Parse(atoms[2]);
        float y2 = float.Parse(atoms[3]);
        float x3 = float.Parse(atoms[4]);
        float y3 = float.Parse(atoms[5]);
        float x4 = float.Parse(atoms[6]);
        float y4 = float.Parse(atoms[7]);

        Bezier bezier = new  Bezier(x1, y1, x2, y2, x3, y3, x4, y4);

        arrayList.Add(bezier);
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private void Transform(float sx, float sy, float dx, float dy)
    {
      toolStripStatusLabel1.Text = "";
      toolStripStatusLabel1.BackColor = DefaultBackColor;

      Transform(sx, sy, dx, dy, elementTextBox);
      Transform(sx, sy, dx, dy, decorationTextBox);
    }

    private void Transform(float sx, float sy, float dx, float dy, TextBox textBox)
    {
      tempText = "";

      string[] atoms = textBox.Text.Split(stringSplitArray, System.StringSplitOptions.RemoveEmptyEntries);
      foreach (string atom in atoms)
      {
        if (atom.StartsWith("Poly"))
        {
          if (!TransformPoly(atom, sx, sy, dx, dy))
            return;
        }
        else if (atom.StartsWith("Arc2"))
        {
          if (!TransformArc2(atom, sx, sy, dx, dy))
            return;
        }
        else if (atom.StartsWith("Arc3"))
        {
          if (!TransformArc3(atom, sx, sy, dx, dy))
            return;
        }
        else if (atom.StartsWith("Arc"))
        {
          if (!TransformArc(atom, sx, sy, dx, dy))
            return;
        }
        else if (atom.StartsWith("Bezier"))
        {
          if (!TransformBezier(atom, sx, sy, dx, dy))
            return;
        }
        else
        {
          toolStripStatusLabel1.Text = "Unknown atom: " + atom;
          toolStripStatusLabel1.BackColor = Color.Yellow;
          return;
        }
      }

      textBox.Text = tempText;
    }

    private bool TransformPoly(string atom, float sx, float sy, float dx, float dy)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      tempText += "MDrw_Poly ";

      if ((IsOdd(atoms.Length)) || (atoms.Length < 4))
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        int i = 0;

        while (i < atoms.Length)
        {
          if (i > 0)
            tempText += ", ";

          float x = float.Parse(atoms[i]);
          float y = float.Parse(atoms[i + 1]);
          i += 2;

          x *= sx;
          y *= sy;

          x += dx;
          y += dy;

          tempText += x.ToString() + ", " + y.ToString();
        }
        tempText += "\r\n";
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool TransformArc(string atom, float sx, float sy, float dx, float dy)
    {
      atom = atom.Remove(0, 3);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      tempText += "MDrw_Arc ";

      if (atoms.Length != 3)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x = float.Parse(atoms[0]);
        float y = float.Parse(atoms[1]);
        float r = Math.Abs(float.Parse(atoms[2]));

        x *= sx;
        y *= sy;
        r *= (float)(Math.Sqrt((double)(sx * sy)));

        x += dx;
        y += dy;

        tempText += x.ToString() + ", " + y.ToString() + ", " + r.ToString() + "\r\n";
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool TransformArc2(string atom, float sx, float sy, float dx, float dy)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      tempText += "MDrw_Arc2 ";

      if (atoms.Length != 6)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x = float.Parse(atoms[0]);
        float y = float.Parse(atoms[1]);
        float w = float.Parse(atoms[2]);
        float h = float.Parse(atoms[3]);
        float a = float.Parse(atoms[4]);
        float s = float.Parse(atoms[5]);

        x *= sx;
        y *= sy;
        w *= sx;
        h *= sy;

        x += dx;
        y += dy;

        tempText += x.ToString() + ", " + y.ToString() + ", " + w.ToString() + ", " + h.ToString() + ", " + a.ToString() + ", " + s.ToString() + "\r\n";
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool TransformArc3(string atom, float sx, float sy, float dx, float dy)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      tempText += "MDrw_Arc3 ";

      if (atoms.Length != 6)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x1 = float.Parse(atoms[0]);
        float y1 = float.Parse(atoms[1]);
        float x2 = float.Parse(atoms[2]);
        float y2 = float.Parse(atoms[3]);
        float x3 = float.Parse(atoms[4]);
        float y3 = float.Parse(atoms[5]);

        x1 *= sx;
        y1 *= sy;

        x1 += dx;
        y1 += dy;

        x2 *= sx;
        y2 *= sy;

        x2 += dx;
        y2 += dy;

        x3 *= sx;
        y3 *= sy;

        x3 += dx;
        y3 += dy;

        tempText += x1.ToString() + ", " + y1.ToString() + ", " + x2.ToString() + ", " + y2.ToString() + ", " + x3.ToString() + ", " + y3.ToString() + "\r\n";
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private bool TransformBezier(string atom, float sx, float sy, float dx, float dy)
    {
      atom = atom.Remove(0, 6);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      tempText += "MDrw_Bezier ";

      if (atoms.Length != 8)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        float x1 = float.Parse(atoms[0]);
        float y1 = float.Parse(atoms[1]);
        float x2 = float.Parse(atoms[2]);
        float y2 = float.Parse(atoms[3]);
        float x3 = float.Parse(atoms[4]);
        float y3 = float.Parse(atoms[5]);
        float x4 = float.Parse(atoms[6]);
        float y4 = float.Parse(atoms[7]);

        x1 *= sx;
        y1 *= sy;

        x1 += dx;
        y1 += dy;

        x2 *= sx;
        y2 *= sy;

        x2 += dx;
        y2 += dy;

        x3 *= sx;
        y3 *= sy;

        x3 += dx;
        y3 += dy;

        x4 *= sx;
        y4 *= sy;

        x4 += dx;
        y4 += dy;

        tempText += x1.ToString() + ", " + y1.ToString() + ", " +
                    x2.ToString() + ", " + y2.ToString() + ", " +
                    x3.ToString() + ", " + y3.ToString() + ", " +
                    x4.ToString() + ", " + y4.ToString() + "\r\n";
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
    }

    private void UpdateStencil(GraphicStencil graphicStencil)
    {
      minX = float.MaxValue;
      maxX = float.MinValue;
      minY = float.MaxValue;
      maxY = float.MinValue;

      UpdateStencil(graphicStencil.elements, ref minX, ref minY, ref maxX, ref maxY);
      UpdateStencil(graphicStencil.decorations, ref minX, ref minY, ref maxX, ref maxY);

      ScaleStencil(graphicStencil.elements, minX, minY, maxX, maxY);
      ScaleStencil(graphicStencil.decorations, minX, minY, maxX, maxY);

      scale = 1000.0F / Math.Max((maxX - minX), (maxY - minY));

      RectangleF rect = new RectangleF(minX * scale, minY * scale, (maxX - minX) * scale, (maxY - minY) * scale);

      graphicStencil.defaultSize = new SizeF((maxX - minX), (maxY - minY));

      box.BoundingRect = rect;
      box.Shape = graphicStencil.ShapeTemplate(false, false);

      rect.Inflate((maxX - minX) * scale / 10.0F, (maxY - minY) * scale / 10.0F);
      flowChart1.DocExtents = rect;
      flowChart1.ZoomToRect(rect);
    }

    private void UpdateStencil(ArrayList arrayList, ref float minX, ref float minY, ref float maxX, ref float maxY)
    {
      foreach (Element element in arrayList)
      {
        if (element is Line)
        {
          Line line = element as Line;
          if (line.x1 < minX) minX = line.x1;
          if (line.x1 > maxX) maxX = line.x1;
          if (line.y1 < minY) minY = line.y1;
          if (line.y1 > maxY) maxY = line.y1;
          if (line.x2 < minX) minX = line.x2;
          if (line.x2 > maxX) maxX = line.x2;
          if (line.y2 < minY) minY = line.y2;
          if (line.y2 > maxY) maxY = line.y2;
        }
        else if (element is Arc)
        {
          Arc arc = element as Arc;

          float a1;
          float a2;
          
          // Sides if they exist.

          a1 = arc.a;
          a2 = arc.a + arc.s;
          if ((a1 * a2 <= 0.0F) && (arc.x + arc.w > maxX)) maxX = arc.x + arc.w;

          a1 = arc.a - 90.0F;
          a2 = arc.a + arc.s - 90.0F;
          if ((a1 * a2 <= 0.0F) && (arc.y + arc.h > maxY)) maxY = arc.y + arc.h;

          a1 = arc.a - 180.0F;
          a2 = arc.a + arc.s - 180.0F;
          if ((a1 * a2 <= 0.0F) && (arc.x < minX)) minX = arc.x;

          a1 = arc.a - 270.0F;
          a2 = arc.a + arc.s - 270.0F;
          if ((a1 * a2 <= 0.0F) && (arc.y < minY)) minY = arc.y;

          // Endpoints.

          float x1 = (float)(Math.Cos(arc.a / 180.0F * Math.PI)) * arc.w / 2.0F + arc.x + arc.w / 2.0F;
          float y1 = (float)(Math.Sin(arc.a / 180.0F * Math.PI)) * arc.h / 2.0F + arc.y + arc.h / 2.0F;

          if (x1 < minX) minX = x1;
          if (x1 > maxX) maxX = x1;
          if (y1 < minY) minY = y1;
          if (y1 > maxY) maxY = y1;

          float x2 = (float)(Math.Cos((arc.a + arc.s) / 180.0F * Math.PI)) * arc.w / 2.0F + arc.x + arc.w / 2.0F;
          float y2 = (float)(Math.Sin((arc.a + arc.s) / 180.0F * Math.PI)) * arc.h / 2.0F + arc.y + arc.h / 2.0F;
                    
          if (x2 < minX) minX = x2;
          if (x2 > maxX) maxX = x2;
          if (y2 < minY) minY = y2;
          if (y2 > maxY) maxY = y2;
        }
        else if (element is Bezier)
        {
          Bezier bezier = element as Bezier;

          PointCollection pointCollection = Utilities.approxBezier(bezier.Points, 0, 100);

          foreach (PointF point in pointCollection)
          {
            if (point.X < minX) minX = point.X;
            if (point.Y < minY) minY = point.Y;

            if (point.X > maxX) maxX = point.X;
            if (point.Y > maxY) maxY = point.Y;
          }          
        }
      }

      if (maxX == minX) maxX += 0.01F;
      if (maxY == minY) maxY += 0.01F;
    }

    private void ScaleStencil(ArrayList arrayList, float minX, float minY, float maxX, float maxY)
    {
      foreach (Element element in arrayList)
      {
        if (element is Line)
        {
          Line line = element as Line;
          line.x1 = (line.x1 - minX) * 100.0F / (maxX - minX);
          line.y1 = (line.y1 - minY) * 100.0F / (maxY - minY);
          line.x2 = (line.x2 - minX) * 100.0F / (maxX - minX);
          line.y2 = (line.y2 - minY) * 100.0F / (maxY - minY);
        }
        else if (element is Arc)
        {
          Arc arc = element as Arc;
          arc.x = (arc.x - minX) * 100.0F / (maxX - minX);
          arc.y = (arc.y - minY) * 100.0F / (maxY - minY);
          arc.w *= 100.0F / (maxX - minX);
          arc.h *= 100.0F / (maxY - minY);
        }
        if (element is Bezier)
        {
          Bezier bezier = element as Bezier;
          bezier.x1 = (bezier.x1 - minX) * 100.0F / (maxX - minX);
          bezier.y1 = (bezier.y1 - minY) * 100.0F / (maxY - minY);
          bezier.x2 = (bezier.x2 - minX) * 100.0F / (maxX - minX);
          bezier.y2 = (bezier.y2 - minY) * 100.0F / (maxY - minY);
          bezier.x3 = (bezier.x3 - minX) * 100.0F / (maxX - minX);
          bezier.y3 = (bezier.y3 - minY) * 100.0F / (maxY - minY);
          bezier.x4 = (bezier.x4 - minX) * 100.0F / (maxX - minX);
          bezier.y4 = (bezier.y4 - minY) * 100.0F / (maxY - minY);
        }
      }
    }

    private void sxUpDown_ValueChanged(object sender, EventArgs e)
    {
      this.sxUpDown.ValueChanged -= new System.EventHandler(this.sxUpDown_ValueChanged);

      if (sxUpDown.Value < 0)
      {
        Transform(0.9F, 1.0F, 0.0F, 0.0F);
      }
      else if (sxUpDown.Value > 0)
      {
        Transform(1.1F, 1.0F, 0.0F, 0.0F);
      }

      sxUpDown.Value = 0;

      this.sxUpDown.ValueChanged += new System.EventHandler(this.sxUpDown_ValueChanged);
    }

    private void syUpDown_ValueChanged(object sender, EventArgs e)
    {
      this.syUpDown.ValueChanged -= new System.EventHandler(this.syUpDown_ValueChanged);

      if (syUpDown.Value < 0)
      {
        Transform(1.0F, 0.9F, 0.0F, 0.0F);
      }
      else if (syUpDown.Value > 0)
      {
        Transform(1.0F, 1.1F, 0.0F, 0.0F);
      }

      syUpDown.Value = 0;

      this.syUpDown.ValueChanged += new System.EventHandler(this.syUpDown_ValueChanged);
    }

    private void dxUpDown_ValueChanged(object sender, EventArgs e)
    {
      this.dxUpDown.ValueChanged -= new System.EventHandler(this.dxUpDown_ValueChanged);

      if (dxUpDown.Value < 0)
      {
        Transform(1.0F, 1.0F, -0.1F, 0.0F);
      }
      else if (dxUpDown.Value > 0)
      {
        Transform(1.0F, 1.0F, 0.1F, 0.0F);
      }

      dxUpDown.Value = 0;

      this.dxUpDown.ValueChanged += new System.EventHandler(this.dxUpDown_ValueChanged);
    }

    private void dyUpDown_ValueChanged(object sender, EventArgs e)
    {
      this.dyUpDown.ValueChanged -= new System.EventHandler(this.dyUpDown_ValueChanged);

      if (dyUpDown.Value < 0)
      {
        Transform(1.0F, 1.0F, 0.0F, -0.1F);
      }
      else if (dyUpDown.Value > 0)
      {
        Transform(1.0F, 1.0F, 0.0F, 0.1F);
      }

      dyUpDown.Value = 0;

      this.dyUpDown.ValueChanged += new System.EventHandler(this.dyUpDown_ValueChanged);
    }

    private void newToolStripMenuItem_Click(object sender, EventArgs e)
    {
      elementTextBox.Text = "";
      decorationTextBox.Text = "";

      graphicStencil.elements = new System.Collections.ArrayList();
      graphicStencil.decorations = new System.Collections.ArrayList();
      graphicStencil.textAreas = new System.Collections.ArrayList();

      flowChart1.DocExtents = new RectangleF(-1.0F, -1.0F, 2.0F, 2.0F);
      flowChart1.ZoomToRect(new RectangleF(-1.0F, -1.0F, 2.0F, 2.0F));
      box = flowChart1.CreateBox(-1.0F, -1.0F, 2.0F, 2.0F);
      box.Locked = true;
      box.Style = MindFusion.FlowChartX.BoxStyle.Shape;
      box.Shape = graphicStencil.ShapeTemplate(false, false);
    }

    private void openToolStripMenuItem_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();
      openFileDialog.AddExtension = true;
      openFileDialog.CheckFileExists = true;
      openFileDialog.CheckPathExists = true;
      openFileDialog.DefaultExt = "GraphicStencil";
      openFileDialog.DereferenceLinks = true;
      openFileDialog.Filter = "Graphic Stencil|*.GraphicStencil";
      openFileDialog.Multiselect = false;
      openFileDialog.ValidateNames = true;

      if (openFileDialog.ShowDialog() == DialogResult.OK)
      {
        filename = openFileDialog.FileName;
        SoapFormatter sf = new SoapFormatter();
        Stream stream = new StreamReader(openFileDialog.FileName).BaseStream;
        GraphicStencil graphicStencil = (GraphicStencil)sf.Deserialize(stream);
        stream.Close();

        Generate(graphicStencil.elements, graphicStencil.defaultSize, elementTextBox);
        Generate(graphicStencil.decorations, graphicStencil.defaultSize, decorationTextBox);

        Parse(graphicStencil.elements, elementTextBox);
        Parse(graphicStencil.decorations, decorationTextBox);
        UpdateStencil(graphicStencil);
      }
    }

    float xj = float.NaN;
    float yj = float.NaN;

    private void Generate(ArrayList arrayList, SizeF defaultSize, TextBox textBox)
    {
      bool firstLineInPoly = true;

      tempText = "";
      xj = float.NaN;
      yj = float.NaN;

      foreach (Element element in arrayList)
      {
        if (element is Line)
        {
          Line line = element as Line;
          GeneratePoly(firstLineInPoly, line, defaultSize);
          
          firstLineInPoly = false;
        }
        else if (element is Arc)
        {
          if (!firstLineInPoly)
            tempText += "\r\n";

          Arc arc = element as Arc;
          GenerateArc2(arc, defaultSize);
          
          firstLineInPoly = true;
        }
        else if (element is Bezier)
        {
          if (!firstLineInPoly)
            tempText += "\r\n";
          
          Bezier bezier = element as Bezier;
          GenerateBezier(bezier, defaultSize);
          
          firstLineInPoly = true;
        }
      }

      textBox.Text = tempText;
    }

    private void GeneratePoly(bool first, Line line, SizeF defaultSize)
    {
      if ((first)
         ||
         ((line.x1 != xj)||(line.y1 != yj)))
      {
        if (!first)
          tempText += "\r\n";

        tempText += "MDrw_Poly " +
                    (line.x1 / 100.0F * defaultSize.Width).ToString() + ", " + (line.y1 / 100.0F * defaultSize.Height).ToString() + ", " +
                    (line.x2 / 100.0F * defaultSize.Width).ToString() + ", " + (line.y2 / 100.0F * defaultSize.Height).ToString();
      }
      else 
      {
        tempText += ", " +
                    (line.x2 / 100.0F * defaultSize.Width).ToString() + ", " + (line.y2 / 100.0F * defaultSize.Height).ToString();
      }

      xj = line.x2;
      yj = line.y2;
    }

    private void GenerateArc2(Arc arc, SizeF defaultSize)
    {
      tempText += "MDrw_Arc2 " +
                  (arc.x / 100.0F * defaultSize.Width).ToString() + ", " + (arc.y / 100.0F * defaultSize.Height).ToString() + ", " +
                  (arc.w / 100.0F * defaultSize.Width).ToString() + ", " + (arc.h / 100.0F * defaultSize.Height).ToString() + ", " +
                  arc.a.ToString() + ", " + arc.s.ToString() + "\r\n";
    }

    private void GenerateBezier(Bezier bezier, SizeF defaultSize)
    {
      tempText += "MDrw_Bezier " +
                  (bezier.x1 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y1 / 100.0F * defaultSize.Height).ToString() + ", " +
                  (bezier.x2 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y2 / 100.0F * defaultSize.Height).ToString() + ", " +
                  (bezier.x3 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y3 / 100.0F * defaultSize.Height).ToString() + ", " +
                  (bezier.x4 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y4 / 100.0F * defaultSize.Height).ToString() + "\r\n";
    }

    private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog = new SaveFileDialog();
      saveFileDialog.AddExtension = true;
      saveFileDialog.CheckPathExists = true;
      saveFileDialog.DefaultExt = "GraphicStencil";
      saveFileDialog.DereferenceLinks = true;
      saveFileDialog.Filter = "Graphic Stencil|*.GraphicStencil";
      saveFileDialog.ValidateNames = true;

      if (saveFileDialog.ShowDialog() == DialogResult.OK)
      {
        filename = saveFileDialog.FileName;
        saveToolStripMenuItem_Click(sender, e);
      }
    }

    private void flowChart1_Resize(object sender, EventArgs e)
    {
      RectangleF rect = new RectangleF(minX * scale, minY * scale, (maxX - minX) * scale, (maxY - minY) * scale);

      box.BoundingRect = rect;
      box.Shape = graphicStencil.ShapeTemplate(false, false);

      rect.Inflate((maxX - minX) * scale / 10.0F, (maxY - minY) * scale / 10.0F);
      flowChart1.DocExtents = rect;
      flowChart1.ZoomToRect(rect);
    }

    private void saveToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SoapFormatter sf = new SoapFormatter();
      Stream stream = new StreamWriter(filename).BaseStream;
      sf.Serialize(stream, graphicStencil);
      stream.Close();
    }
  }
}