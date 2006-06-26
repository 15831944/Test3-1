using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Interface;
using System.Collections;

namespace StencilEditor
{
  public partial class Form1 : Form
  {
    private GraphicStencil graphicStencil = new GraphicStencil();

    private string[] stringSplitArray = { "MDrw_" };
    private char[] charSplitArray = { ',', ' ', '\r', '\n' };

    float height, width;

    MindFusion.FlowChartX.Box box;

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

      graphicStencil.elements = new System.Collections.ArrayList();
      graphicStencil.decorations = new System.Collections.ArrayList();
      graphicStencil.textAreas = new System.Collections.ArrayList();

      flowChart1.DocExtents = new RectangleF(-100.0F, -100.0F, 200.0F, 200.0F);
      flowChart1.ZoomToRect(new RectangleF(-100.0F, -100.0F, 200.0F, 200.0F));
      box = flowChart1.CreateBox(-50.0F, -50.0F, 100.0F, 100.0F);
      box.Locked = true;
      box.Style = MindFusion.FlowChartX.BoxStyle.Shape;
      box.Shape = graphicStencil.ShapeTemplate(false, false);

      height = 10.0F;
      width = 10.0F;

      toolStripTextBox1.Text = width.ToString();
      toolStripTextBox2.Text = height.ToString();

      double x1 = 0.0;
      double y1 = 0.0;

      double x2 = 1.0;
      double y2 = 1.0;

      double x3 = 2.0;
      double y3 = 0.0;

      //a = 2*det([1,1,1;x1,x2,x3;y1,y2,y3]);

      double a = 2.0 * det(1.0, 1.0, 1.0, x1, x2, x3, y1, y2, y3);

      //r = sqrt( ((x2-x3)^2+(y2-y3)^2) ...
      double r = Math.Sqrt(  (Math.Pow(x2 - x3, 2) + Math.Pow(y2 - y3, 2))
      //* ((x3-x1)^2+(y3-y1)^2)
                           * (Math.Pow(x3 - x1, 2) + Math.Pow(y3 - y1, 2))
      // * ((x1-x2)^2+(y1-y2)^2) )
                           * (Math.Pow(x1 - x2, 2) + Math.Pow(y1 - y2, 2)))
      //  / abs(a);
                 / Math.Abs(a);

      //xc = - det([1,1,1;y1,y2,y3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;

      double xc = -det(1.0, 1.0, 1.0,
                       y1, y2, y3,
                       x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / a;

      //yc =   det([1,1,1;x1,x2,x3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;

      double yc = det(1.0, 1.0, 1.0,
                      x1, x2, x3,
                      x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / a;

      double a3 = Math.Atan2(yc - y3, xc - x3) * 180 / Math.PI;
      double a1 = Math.Atan2(yc - y1, xc - x1) * 180 / Math.PI;

      double aa = a1;
      double ss = a3 - a1;

      int brk = 1;
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

    private void WhichArrayList(out ArrayList arrayList, out TextBox textBox)
    {
      arrayList = null;
      textBox = null;

      if (tabControl1.SelectedTab.Name == "elementTabPage")
      {
        arrayList = graphicStencil.elements;
        textBox = elementTextBox;
      }

      if (tabControl1.SelectedTab.Name == "decorationTabPage")
      {
        arrayList = graphicStencil.decorations;
        textBox = decorationTextBox;
      }

      //graphicStencil.textAreas.Clear();
    }

    bool ParsePoly(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 4);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if (IsOdd(atoms.Length))
        return false;

      if (atoms.Length < 4)
        return false;

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
        return false;
      }

      return true;
    }

    bool ParseArc(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 3);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if ((atoms.Length != 3) && (atoms.Length != 6))
        return false;

      if (atoms.Length == 3)
      {
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
          return false;
        }
      }
      else if (atoms.Length == 6)
      {
        try
        {
          float x = float.Parse(atoms[0]);
          float y = float.Parse(atoms[1]);
          float w = float.Parse(atoms[2]);
          float h = float.Parse(atoms[3]);
          float a = float.Parse(atoms[4]);
          float s = float.Parse(atoms[5]);

          Arc arc = new Arc(x, y, w, h, a, s);

          arrayList.Add(arc);
        }
        catch
        {
          return false;
        }
      }
      return true;
    }

    private void textBox_TextChanged(object sender, EventArgs e)
    {
      ArrayList arrayList;
      TextBox textBox;
      WhichArrayList(out arrayList, out textBox);

      arrayList.Clear();


      string[] atoms = textBox.Text.Split(stringSplitArray, System.StringSplitOptions.RemoveEmptyEntries);
      foreach (string atom in atoms)
      {
        if (atom.StartsWith("Poly"))
        {
          ParsePoly(atom, arrayList);
        }
        else if (atom.StartsWith("Arc"))
        {
          ParseArc(atom, arrayList);
        }
        else if (atom.StartsWith("Arc3"))
        {
        }
        else if (atom.StartsWith("End"))
        {
        }
        else
        {
        }
      }
      box.Shape = graphicStencil.ShapeTemplate(false, false);
    }

    private void toolStripButton1_Click(object sender, EventArgs e)
    {
      ArrayList arrayList;
      TextBox textBox;
      WhichArrayList(out arrayList, out textBox);

      float minX = float.MaxValue;
      float maxX = float.MinValue;
      float minY = float.MaxValue;
      float maxY = float.MinValue;

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
          if (arc.x < minX) minX = arc.x;
          if (arc.x + arc.w > maxX) maxX = arc.x + arc.w;
          if (arc.y < minY) minY = arc.y;
          if (arc.y + arc.h > maxY) maxY = arc.y + arc.h;
        }
      }

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
      }

      this.elementTextBox.TextChanged -= new System.EventHandler(this.textBox_TextChanged);
      elementTextBox.Text = "";

      Line prevLine = null;
      foreach (Element element in arrayList)
      {
        if (element is Line)
        {
          Line line = element as Line;
          if ((prevLine is Line) && (line.x1 == prevLine.x2) && (line.y1 == prevLine.y2))
          {
            elementTextBox.Text += ", ";
            elementTextBox.Text += line.x2.ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += line.y2.ToString();
          }
          else
          {
            elementTextBox.Text += "\r\nMDrw_Poly, ";
            elementTextBox.Text += line.x1.ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += line.y1.ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += line.x2.ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += line.y2.ToString();
          }

          prevLine = line;
        }
        else
        {
          if (element is Arc)
          {
            Arc arc = element as Arc;
            elementTextBox.Text += "\r\nMDrw_Arc, ";
            elementTextBox.Text += (arc.x).ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += (arc.y).ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += (arc.w).ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += (arc.h).ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += (arc.a).ToString();
            elementTextBox.Text += ", ";
            elementTextBox.Text += (arc.s).ToString();
          }
          prevLine = null;
        }
      }
      this.elementTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
      elementTextBox.Text = elementTextBox.Text.TrimStart(charSplitArray);
    }

    private void toolStripTextBox_TextChanged(object sender, EventArgs e)
    {
      float tempWidth = width;
      float tempHeight = height;

      try
      {
        tempWidth = float.Parse(toolStripTextBox1.Text);
        toolStripTextBox1.BackColor = DefaultBackColor;
      }
      catch
      {
        toolStripTextBox1.BackColor = Color.Yellow;
      }

      try
      {
        tempHeight = float.Parse(toolStripTextBox2.Text);
        toolStripTextBox2.BackColor = DefaultBackColor;
      }
      catch
      {
        toolStripTextBox2.BackColor = Color.Yellow;
      }

      if (tempWidth > 0.0)
      {
        width = tempWidth;
        toolStripTextBox1.BackColor = DefaultBackColor;
      }
      else
      {
        toolStripTextBox1.BackColor = Color.Yellow;
      }

      if (tempHeight > 0.0)
      {
        height = tempHeight;
        toolStripTextBox2.BackColor = DefaultBackColor;
      }
      else
      {
        toolStripTextBox2.BackColor = Color.Yellow;
      }

      box.BoundingRect = new RectangleF(-5.0F * width, -5.0F * height, 10.0F * width, 10.0F * height);
      flowChart1.DocExtents = new RectangleF(-10.0F * width, -10.0F * height, 20.0F * width, 20.0F * height);
      flowChart1.ZoomToRect(new RectangleF(-10.0F * width, -10.0F * height, 20.0F * width, 20.0F * height));
    }
  }
}