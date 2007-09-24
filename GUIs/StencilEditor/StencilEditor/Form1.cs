using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Protocol;
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
    private ModelStencil modelStencil = new ModelStencil();

    private string[] stringSplitArray = { "MDrw_" };
    private char[] charSplitArray = { ',', ' ', '\r', '\n' };

    private MindFusion.FlowChartX.Box box1,
                                      textBox1,
                                      box2;

    private ArrayList anchorPointBoxes = new ArrayList();

    private double det(double x11, double x21, double x31,
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
      graphicStencil.Elements = new System.Collections.ArrayList();
      graphicStencil.Decorations = new System.Collections.ArrayList();

      modelStencil.Elements = new System.Collections.ArrayList();
      modelStencil.Decorations = new System.Collections.ArrayList();
      modelStencil.Anchors = new System.Collections.ArrayList();

      InitializeComponent();

      flowChart1.Enabled = false;
      flowChart2.Enabled = false;

      newToolStripMenuItem_Click(null, null);
    }

    private static bool IsEven(int intValue)
    {
      return ((intValue & 1) == 0);
    }

    private static bool IsOdd(int intValue)
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
      if ((graphicStencil != null) && (modelStencil != null))
      {
        toolStripStatusLabel1.Text = "";
        toolStripStatusLabel1.BackColor = DefaultBackColor;

        Parse(graphicStencil.Elements, elementTextBox);
        Parse(graphicStencil.Decorations, decorationTextBox);

        SysCAD.Protocol.Rectangle textArea = graphicStencil.TextArea;
        ParseTextArea(ref textArea, textAreaTextBox);
        graphicStencil.TextArea = textArea;

        UpdateStencil(graphicStencil);

        Parse(modelStencil.Elements, elementTextBox);
        Parse(modelStencil.Decorations, decorationTextBox);
        ParseAnchor(modelStencil.Anchors, anchorTextBox);
        UpdateStencil(modelStencil);
      }
    }

    private bool ParseTextArea(ref SysCAD.Protocol.Rectangle rectangleF, TextBox textBox)
    {
      if (textBox.Text.Length > 0)
      {
        string[] atoms = textBox.Text.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

        if (atoms.Length != 4)
        {
          toolStripStatusLabel1.Text = "Incorrect number of parameters (x, y, w, h): " + textBox.Text;
          toolStripStatusLabel1.BackColor = Color.Yellow;
          return false;
        }

        try
        {
          rectangleF.X = float.Parse(atoms[0]);
          rectangleF.Y = float.Parse(atoms[1]);
          rectangleF.Width = float.Parse(atoms[2]);
          rectangleF.Height = float.Parse(atoms[3]);
        }
        catch
        {
          toolStripStatusLabel1.Text = "Error parsing parameters (x, y, w, h): " + textBox.Text;
          toolStripStatusLabel1.BackColor = Color.Yellow;
          return false;
        }
      }
      return true;
    }

    private void ParseAnchor(ArrayList arrayList, TextBox textBox)
    {
      arrayList.Clear();

      string[] atoms = textBox.Text.Split(stringSplitArray, System.StringSplitOptions.RemoveEmptyEntries);
      foreach (string atom in atoms)
      {
        if (atom.StartsWith("Anchor"))
        {
          if (!ParseAnchor(atom, arrayList))
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
    
    private bool ParseAnchor(string atom, ArrayList arrayList)
    {
      atom = atom.Remove(0, 6);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      if (atoms.Length != 3)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters (tag, x, y): " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        string tag = atoms[0];
        AnchorType type = AnchorType.Process;

        double x = double.Parse(atoms[1]);
        double y = double.Parse(atoms[2]);

        Anchor anchor = new Anchor(tag, type, 0, x, y);

        arrayList.Add(anchor);
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters (tag, x, y): " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      return true;
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
        toolStripStatusLabel1.Text = "Incorrect number of parameters (x1, y1, x2, y2, ...., xn, yn): " + atom;
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
        toolStripStatusLabel1.Text = "Error parsing parameters (x1, y1, x2, y2, ...., xn, yn): " + atom;
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
        toolStripStatusLabel1.Text = "Incorrect number of parameters (x, y, r): " + atom;
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
        toolStripStatusLabel1.Text = "Error parsing parameters (x, y, r): " + atom;
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
        toolStripStatusLabel1.Text = "Incorrect number of parameters (x, y, w, h, a, s): " + atom;
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
        toolStripStatusLabel1.Text = "Error parsing parameters (x, y, w, h, a, s): " + atom;
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
        toolStripStatusLabel1.Text = "Incorrect number of parameters (x1, y1, x2, y2, x3, y3): " + atom;
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
        toolStripStatusLabel1.Text = "Error parsing parameters (x1, y1, x2, y2, x3, y3): " + atom;
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
        toolStripStatusLabel1.Text = "Incorrect number of parameters (x1, y1, x2, y2, x3, y3, x4, y4): " + atom;
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

        Bezier bezier = new Bezier(x1, y1, x2, y2, x3, y3, x4, y4);

        arrayList.Add(bezier);
      }
      catch
      {
        toolStripStatusLabel1.Text = "Error parsing parameters (x1, y1, x2, y2, x3, y3, x4, y4): " + atom;
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
      Transform(sx, sy, dx, dy, textAreaTextBox);
      Transform(sx, sy, dx, dy, anchorTextBox);
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
        else if (atom.StartsWith("Anchor"))
        {
          if (!TransformAnchor(atom, sx, sy, dx, dy))
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

    private bool TransformAnchor(string atom, float sx, float sy, float dx, float dy)
    {
      atom = atom.Remove(0, 6);
      string[] atoms = atom.Split(charSplitArray, StringSplitOptions.RemoveEmptyEntries);

      tempText += "MDrw_Anchor ";

      if (atoms.Length != 6)
      {
        toolStripStatusLabel1.Text = "Incorrect number of parameters: " + atom;
        toolStripStatusLabel1.BackColor = Color.Yellow;
        return false;
      }

      try
      {
        string tag = atoms[0];
        float x = float.Parse(atoms[2]);
        float y = float.Parse(atoms[3]);

        x *= sx;
        y *= sy;

        x += dx;
        y += dy;

        uint min = uint.Parse(atoms[4]);
        uint max = uint.Parse(atoms[5]);

        tempText += tag + ", ";

        tempText += x.ToString() + ", " + y.ToString() + ", ";
        tempText += min.ToString() + ", " + max.ToString() + ", ";
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
      double minX = double.MaxValue;
      double maxX = double.MinValue;
      double minY = double.MaxValue;
      double maxY = double.MinValue;

      UpdateStencil(graphicStencil.Elements, ref minX, ref minY, ref maxX, ref maxY);
      UpdateStencil(graphicStencil.Decorations, ref minX, ref minY, ref maxX, ref maxY);

      double textMinX = double.MaxValue;
      double textMaxX = double.MinValue;
      double textMinY = double.MaxValue;
      double textMaxY = double.MinValue;

      UpdateStencil(graphicStencil.TextArea, ref textMinX, ref textMinY, ref textMaxX, ref textMaxY);

      ScaleStencil(graphicStencil.Elements, minX, minY, maxX, maxY);
      ScaleStencil(graphicStencil.Decorations, minX, minY, maxX, maxY);

      double scale = 1000.0 / Math.Max((maxX - minX), (maxY - minY));

      SysCAD.Protocol.Rectangle rect = new SysCAD.Protocol.Rectangle(minX * scale, minY * scale, (maxX - minX) * scale, (maxY - minY) * scale);

      graphicStencil.defaultSize = new SysCAD.Protocol.Size((maxX - minX), (maxY - minY));

      box1.BoundingRect = rect;
      

      {
        GraphicStencil stencil = graphicStencil;
        bool mirrorX = false;
        bool mirrorY = false;

        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        {
          int i = 0;
          foreach (Element element in stencil.Elements)
          {
            if (element is Arc)
            {
              elementTemplate[i] = new ArcTemplate(
                (float)(element as Arc).x,
                (float)(element as Arc).y,
                (float)(element as Arc).w,
                (float)(element as Arc).h,
                (float)(element as Arc).a,
                (float)(element as Arc).s);
            }
            if (element is Line)
            {
              Line line = element as Line;
              double x1, y1, x2, y2;

              if (mirrorX)
                x1 = 100.0 - line.x1;
              else
                x1 = line.x1;

              if (mirrorY)
                y1 = 100.0 - line.y1;
              else
                y1 = line.y1;

              if (mirrorX)
                x2 = 100.0 - line.x2;
              else
                x2 = line.x2;

              if (mirrorY)
                y2 = 100.0 - line.y2;
              else
                y2 = line.y2;

              elementTemplate[i] = new LineTemplate((float)x1, (float)y1, (float)x2, (float)y2);
            }
            if (element is Bezier)
            {
              Bezier bezier = element as Bezier;
              double x1, y1, x2, y2, x3, y3, x4, y4;

              if (mirrorX)
                x1 = 100.0 - bezier.x1;
              else
                x1 = bezier.x1;

              if (mirrorY)
                y1 = 100.0 - bezier.y1;
              else
                y1 = bezier.y1;

              if (mirrorX)
                x2 = 100.0 - bezier.x2;
              else
                x2 = bezier.x2;

              if (mirrorY)
                y2 = 100.0 - bezier.y2;
              else
                y2 = bezier.y2;

              if (mirrorX)
                x3 = 100.0 - bezier.x3;
              else
                x3 = bezier.x3;

              if (mirrorY)
                y3 = 100.0 - bezier.y3;
              else
                y3 = bezier.y3;

              if (mirrorX)
                x4 = 100.0 - bezier.x4;
              else
                x4 = bezier.x4;

              if (mirrorY)
                y4 = 100.0 - bezier.y4;
              else
                y4 = bezier.y4;

              elementTemplate[i] = new BezierTemplate((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
            }
            i++;
          }
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        {
          int i = 0;
          foreach (Element decoration in stencil.Decorations)
          {
            if (decoration is Arc)
            {
              decorationTemplate[i] = new ArcTemplate(
                (float)(decoration as Arc).x,
                (float)(decoration as Arc).y,
                (float)(decoration as Arc).w,
                (float)(decoration as Arc).h,
                (float)(decoration as Arc).a,
                (float)(decoration as Arc).s);
            }
            if (decoration is Line)
            {
              decorationTemplate[i] = new LineTemplate(
                (float)(decoration as Line).x1,
                (float)(decoration as Line).y1,
                (float)(decoration as Line).x2,
                (float)(decoration as Line).y2);
            }
            if (decoration is Bezier)
            {
              decorationTemplate[i] = new BezierTemplate(
                (float)(decoration as Bezier).x1,
                (float)(decoration as Bezier).y1,
                (float)(decoration as Bezier).x2,
                (float)(decoration as Bezier).y2,
                (float)(decoration as Bezier).x3,
                (float)(decoration as Bezier).y3,
                (float)(decoration as Bezier).x4,
                (float)(decoration as Bezier).y4);
            }
            i++;
          }
        }

        box1.Shape = (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], stencil.fillMode, stencil.Tag));
      }

      SysCAD.Protocol.Rectangle textRect = new SysCAD.Protocol.Rectangle(textMinX * scale, textMinY * scale, (textMaxX - textMinX) * scale, (textMaxY - textMinY) * scale);

      textBox1.BoundingRect = textRect;
      //textBox1.Shape = graphicStencil.TextShapeTemplate(false, false);
      textBox1.ZTop();
      textBox1.FillColor = Color.FromArgb(100, Color.HotPink);

      double bothMinX = Math.Min(minX, textMinX);
      double bothMaxX = Math.Max(maxX, textMaxX);
      double bothMinY = Math.Min(minY, textMinY);
      double bothMaxY = Math.Max(maxY, textMaxY);

      SysCAD.Protocol.Rectangle bothRect = new SysCAD.Protocol.Rectangle(bothMinX * scale, bothMinY * scale, (bothMaxX - bothMinX) * scale, (bothMaxY - bothMinY) * scale);

      bothRect.Inflate((maxX - minX) * scale / 10.0F, (maxY - minY) * scale / 10.0F);
      flowChart1.DocExtents = bothRect;
      flowChart1.ZoomToRect(bothRect);
    }

    private void UpdateStencil(ModelStencil modelStencil)
    {
      double minX = float.MaxValue;
      double maxX = float.MinValue;
      double minY = float.MaxValue;
      double maxY = float.MinValue;

      UpdateStencil(modelStencil.Elements, ref minX, ref minY, ref maxX, ref maxY);
      UpdateStencil(modelStencil.Decorations, ref minX, ref minY, ref maxX, ref maxY);

      double anchorMinX = minX;
      double anchorMaxX = maxX;
      double anchorMinY = minY;
      double anchorMaxY = maxY;

      UpdateStencil(modelStencil.Anchors, ref anchorMinX, ref anchorMinY, ref anchorMaxX, ref anchorMaxY);

      ScaleStencil(modelStencil.Elements, minX, minY, maxX, maxY);
      ScaleStencil(modelStencil.Decorations, minX, minY, maxX, maxY);
      
      ScaleStencil(modelStencil.Anchors, minX, minY, maxX, maxY);

      double scale = 1000.0F / Math.Max((maxX - minX), (maxY - minY));

      SysCAD.Protocol.Rectangle rect = new SysCAD.Protocol.Rectangle(minX * scale, minY * scale, (maxX - minX) * scale, (maxY - minY) * scale);

      box2.BoundingRect = rect;
      {
        int i;

        ModelStencil stencil = modelStencil;
        bool mirrorX = false;
        bool mirrorY = false;

        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        i = 0;
        foreach (Element element in stencil.Elements)
        {
          if (element is Arc)
          {
            Arc arc = element as Arc;
            elementTemplate[i] = new ArcTemplate((float)arc.x, (float)arc.y, (float)arc.w, (float)arc.h, (float)arc.a, (float)arc.s);
          }
          if (element is Line)
          {
            Line line = element as Line;
            double x1, y1, x2, y2;

            if (mirrorX)
              x1 = 100.0 - line.x1;
            else
              x1 = line.x1;

            if (mirrorY)
              y1 = 100.0 - line.y1;
            else
              y1 = line.y1;

            if (mirrorX)
              x2 = 100.0F - line.x2;
            else
              x2 = line.x2;

            if (mirrorY)
              y2 = 100.0 - line.y2;
            else
              y2 = line.y2;

            elementTemplate[i] = new LineTemplate((float)x1, (float)y1, (float)x2, (float)y2);
          }
          if (element is Bezier)
          {
            Bezier bezier = element as Bezier;
            double x1, y1, x2, y2, x3, y3, x4, y4;

            if (mirrorX)
              x1 = 100.0 - bezier.x1;
            else
              x1 = bezier.x1;

            if (mirrorY)
              y1 = 100.0 - bezier.y1;
            else
              y1 = bezier.y1;

            if (mirrorX)
              x2 = 100.0 - bezier.x2;
            else
              x2 = bezier.x2;

            if (mirrorY)
              y2 = 100.0 - bezier.y2;
            else
              y2 = bezier.y2;

            if (mirrorX)
              x3 = 100.0 - bezier.x3;
            else
              x3 = bezier.x3;

            if (mirrorY)
              y3 = 100.0 - bezier.y3;
            else
              y3 = bezier.y3;

            if (mirrorX)
              x4 = 100.0 - bezier.x4;
            else
              x4 = bezier.x4;

            if (mirrorY)
              y4 = 100.0 - bezier.y4;
            else
              y4 = bezier.y4;

            elementTemplate[i] = new BezierTemplate((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
          }
          i++;
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        i = 0;
        foreach (Element decoration in stencil.Decorations)
        {
          if (decoration is Arc)
          {
            Arc arc = decoration as Arc;
            decorationTemplate[i] = new ArcTemplate((float)arc.x, (float)arc.y, (float)arc.w, (float)arc.h, (float)arc.a, (float)arc.s);
          }
          if (decoration is Line)
          {
            Line line = decoration as Line;
            double x1, y1, x2, y2;

            if (mirrorX)
              x1 = 100.0 - line.x1;
            else
              x1 = line.x1;

            if (mirrorY)
              y1 = 100.0 - line.y1;
            else
              y1 = line.y1;

            if (mirrorX)
              x2 = 100.0 - line.x2;
            else
              x2 = line.x2;

            if (mirrorY)
              y2 = 100.0 - line.y2;
            else
              y2 = line.y2;

            decorationTemplate[i] = new LineTemplate((float)x1, (float)y1, (float)x2, (float)y2);
          }
          if (decoration is Bezier)
          {
            Bezier bezier = decoration as Bezier;
            double x1, y1, x2, y2, x3, y3, x4, y4;

            if (mirrorX)
              x1 = 100.0 - bezier.x1;
            else
              x1 = bezier.x1;

            if (mirrorY)
              y1 = 100.0 - bezier.y1;
            else
              y1 = bezier.y1;

            if (mirrorX)
              x2 = 100.0 - bezier.x2;
            else
              x2 = bezier.x2;

            if (mirrorY)
              y2 = 100.0 - bezier.y2;
            else
              y2 = bezier.y2;

            if (mirrorX)
              x3 = 100.0 - bezier.x3;
            else
              x3 = bezier.x3;

            if (mirrorY)
              y3 = 100.0 - bezier.y3;
            else
              y3 = bezier.y3;

            if (mirrorX)
              x4 = 100.0 - bezier.x4;
            else
              x4 = bezier.x4;

            if (mirrorY)
              y4 = 100.0 - bezier.y4;
            else
              y4 = bezier.y4;

            decorationTemplate[i] = new BezierTemplate((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
          }
          i++;
        }

        box2.Shape = (new ShapeTemplate(elementTemplate, decorationTemplate, null, stencil.FillMode, stencil.Tag));
      }


      SysCAD.Protocol.Rectangle anchorRect = new SysCAD.Protocol.Rectangle(anchorMinX * scale, anchorMinY * scale, (anchorMaxX - anchorMinX) * scale, (anchorMaxY - anchorMinY) * scale);

      foreach (Box box in anchorPointBoxes)
        flowChart2.DeleteObject(box);

      anchorPointBoxes.Clear();

      foreach (Anchor anchor in modelStencil.Anchors)
      {
        RectangleF displayRect = new RectangleF((float)(((anchor.Positions[0] as SysCAD.Protocol.Point).X / 100.0F * (maxX - minX) + minX) * scale),
                                                (float)(((anchor.Positions[0] as SysCAD.Protocol.Point).Y / 100.0F * (maxY - minY) + minY) * scale),
                                                0.0F, 0.0F);
        displayRect.Inflate(20.0F, 20.0F);
        Box box = flowChart2.CreateBox(displayRect.X, displayRect.Y, displayRect.Width, displayRect.Height);
        anchorPointBoxes.Add(box);
        box.Style = BoxStyle.Ellipse;
        box.ZTop();
        box.FillColor = Color.Red;
        box.FillColor = Color.FromArgb(100, box.FillColor);
      }

      anchorRect.Inflate((maxX - minX) * scale / 10.0F, (maxY - minY) * scale / 10.0F);
      flowChart2.DocExtents = anchorRect;
      flowChart2.ZoomToRect(anchorRect);
    }

    private void UpdateStencil(ArrayList arrayList, ref double minX, ref double minY, ref double maxX, ref double maxY)
    {
      foreach (object element in arrayList)
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

          double a1;
          double a2;

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

          double x1 = (float)(Math.Cos(arc.a / 180.0F * Math.PI)) * arc.w / 2.0F + arc.x + arc.w / 2.0F;
          double y1 = (float)(Math.Sin(arc.a / 180.0F * Math.PI)) * arc.h / 2.0F + arc.y + arc.h / 2.0F;

          if (x1 < minX) minX = x1;
          if (x1 > maxX) maxX = x1;
          if (y1 < minY) minY = y1;
          if (y1 > maxY) maxY = y1;

          double x2 = (float)(Math.Cos((arc.a + arc.s) / 180.0F * Math.PI)) * arc.w / 2.0F + arc.x + arc.w / 2.0F;
          double y2 = (float)(Math.Sin((arc.a + arc.s) / 180.0F * Math.PI)) * arc.h / 2.0F + arc.y + arc.h / 2.0F;

          if (x2 < minX) minX = x2;
          if (x2 > maxX) maxX = x2;
          if (y2 < minY) minY = y2;
          if (y2 > maxY) maxY = y2;
        }
        else if (element is Bezier)
        {
          Bezier bezier = element as Bezier;

          PointF[] bezierPoints = new PointF[4] {new PointF((float)bezier.x1, (float)bezier.y1), 
                                                 new PointF((float)bezier.x2, (float)bezier.y2), 
                                                 new PointF((float)bezier.x3, (float)bezier.y3),
                                                 new PointF((float)bezier.x4, (float)bezier.y4)};

          PointCollection pointCollection = Utilities.approxBezier(bezierPoints, 0, 100);

          foreach (PointF point in pointCollection)
          {
            if (point.X < minX) minX = point.X;
            if (point.Y < minY) minY = point.Y;

            if (point.X > maxX) maxX = point.X;
            if (point.Y > maxY) maxY = point.Y;
          }
        }
        if (element is Anchor)
        {
          Anchor anchor = element as Anchor;
          if ((anchor.Positions[0] as SysCAD.Protocol.Point).X < minX) minX = (anchor.Positions[0] as SysCAD.Protocol.Point).X;
          if ((anchor.Positions[0] as SysCAD.Protocol.Point).X > maxX) maxX = (anchor.Positions[0] as SysCAD.Protocol.Point).X;
          if ((anchor.Positions[0] as SysCAD.Protocol.Point).Y < minY) minY = (anchor.Positions[0] as SysCAD.Protocol.Point).Y;
          if ((anchor.Positions[0] as SysCAD.Protocol.Point).Y > maxY) maxY = (anchor.Positions[0] as SysCAD.Protocol.Point).Y;
        }
      }

      if (maxX == minX) maxX += 0.01F;
      if (maxY == minY) maxY += 0.01F;
    }

    private void UpdateStencil(RectangleF rectangleF, ref double minX, ref double minY, ref double maxX, ref double maxY)
    {
      if (rectangleF.Left < minX) minX = rectangleF.Left;
      if (rectangleF.Top  < minY) minY = rectangleF.Top;

      if (rectangleF.Right  > maxX) maxX = rectangleF.Right;
      if (rectangleF.Bottom > maxY) maxY = rectangleF.Bottom;

      if (maxX == minX) maxX += 0.01F;
      if (maxY == minY) maxY += 0.01F;
    }

    private void ScaleStencil(ArrayList arrayList, double minX, double minY, double maxX, double maxY)
    {
      foreach (object element in arrayList)
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
        if (element is Anchor)
        {
          Anchor anchor = element as Anchor;
          anchor.Positions[0] = new SysCAD.Protocol.Point(
            ((anchor.Positions[0] as SysCAD.Protocol.Point).X - minX) * 100.0F / (maxX - minX),
            ((anchor.Positions[0] as SysCAD.Protocol.Point).Y - minY) * 100.0F / (maxY - minY));
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
      textAreaTextBox.Text = "";

      Text = "*Untitled*";

      graphicStencil.Elements = new System.Collections.ArrayList();
      graphicStencil.Decorations = new System.Collections.ArrayList();

      modelStencil.Elements = new System.Collections.ArrayList();
      modelStencil.Decorations = new System.Collections.ArrayList();
      modelStencil.Anchors = new System.Collections.ArrayList();

      flowChart1.DocExtents = new RectangleF(-1.0F, -1.0F, 2.0F, 2.0F);
      flowChart1.ZoomToRect(new RectangleF(-1.0F, -1.0F, 2.0F, 2.0F));

      box1 = flowChart1.CreateBox(-1.0F, -1.0F, 2.0F, 2.0F);
      box1.Locked = true;
      box1.Style = MindFusion.FlowChartX.BoxStyle.Shape;

      {
        GraphicStencil stencil = graphicStencil;
        bool mirrorX = false;
        bool mirrorY = false;

        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        {
          int i = 0;
          foreach (Element element in stencil.Elements)
          {
            if (element is Arc)
            {
              elementTemplate[i] = new ArcTemplate(
                (float)(element as Arc).x,
                (float)(element as Arc).y,
                (float)(element as Arc).w,
                (float)(element as Arc).h,
                (float)(element as Arc).a,
                (float)(element as Arc).s);
            }
            if (element is Line)
            {
              Line line = element as Line;
              double x1, y1, x2, y2;

              if (mirrorX)
                x1 = 100.0 - line.x1;
              else
                x1 = line.x1;

              if (mirrorY)
                y1 = 100.0 - line.y1;
              else
                y1 = line.y1;

              if (mirrorX)
                x2 = 100.0 - line.x2;
              else
                x2 = line.x2;

              if (mirrorY)
                y2 = 100.0 - line.y2;
              else
                y2 = line.y2;

              elementTemplate[i] = new LineTemplate((float)x1, (float)y1, (float)x2, (float)y2);
            }
            if (element is Bezier)
            {
              Bezier bezier = element as Bezier;
              double x1, y1, x2, y2, x3, y3, x4, y4;

              if (mirrorX)
                x1 = 100.0 - bezier.x1;
              else
                x1 = bezier.x1;

              if (mirrorY)
                y1 = 100.0- bezier.y1;
              else
                y1 = bezier.y1;

              if (mirrorX)
                x2 = 100.0 - bezier.x2;
              else
                x2 = bezier.x2;

              if (mirrorY)
                y2 = 100.0 - bezier.y2;
              else
                y2 = bezier.y2;

              if (mirrorX)
                x3 = 100.0 - bezier.x3;
              else
                x3 = bezier.x3;

              if (mirrorY)
                y3 = 100.0 - bezier.y3;
              else
                y3 = bezier.y3;

              if (mirrorX)
                x4 = 100.0 - bezier.x4;
              else
                x4 = bezier.x4;

              if (mirrorY)
                y4 = 100.0 - bezier.y4;
              else
                y4 = bezier.y4;

              elementTemplate[i] = new BezierTemplate((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
            }
            i++;
          }
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        {
          int i = 0;
          foreach (Element decoration in stencil.Decorations)
          {
            if (decoration is Arc)
            {
              decorationTemplate[i] = new ArcTemplate(
                (float)(decoration as Arc).x,
                (float)(decoration as Arc).y,
                (float)(decoration as Arc).w,
                (float)(decoration as Arc).h,
                (float)(decoration as Arc).a,
                (float)(decoration as Arc).s);
            }
            if (decoration is Line)
            {
              decorationTemplate[i] = new LineTemplate(
                (float)(decoration as Line).x1,
                (float)(decoration as Line).y1,
                (float)(decoration as Line).x2,
                (float)(decoration as Line).y2);
            }
            if (decoration is Bezier)
            {
              decorationTemplate[i] = new BezierTemplate(
                (float)(decoration as Bezier).x1,
                (float)(decoration as Bezier).y1,
                (float)(decoration as Bezier).x2,
                (float)(decoration as Bezier).y2,
                (float)(decoration as Bezier).x3,
                (float)(decoration as Bezier).y3,
                (float)(decoration as Bezier).x4,
                (float)(decoration as Bezier).y4);
            }
            i++;
          }
        }

        box1.Shape = (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], stencil.fillMode, stencil.Tag));
      }


      textBox1 = flowChart1.CreateBox(-1.0F, -1.0F, 2.0F, 2.0F);
      textBox1.Locked = true;
      //textBox1.Style = MindFusion.FlowChartX.BoxStyle.Shape;
      //textBox1.Shape = graphicStencil.TextShapeTemplate(false, false);
      textBox1.Shape = MindFusion.FlowChartX.ShapeTemplate.Decision;

      flowChart2.DocExtents = new RectangleF(-1.0F, -1.0F, 2.0F, 2.0F);
      flowChart2.ZoomToRect(new RectangleF(-1.0F, -1.0F, 2.0F, 2.0F));

      box2 = flowChart2.CreateBox(-1.0F, -1.0F, 2.0F, 2.0F);
      box2.Locked = true;
      box2.Style = MindFusion.FlowChartX.BoxStyle.Shape;
      {
        int i;

        ModelStencil stencil = modelStencil;
        bool mirrorX = false;
        bool mirrorY = false;

        ElementTemplate[] elementTemplate = new ElementTemplate[stencil.Elements.Count];
        i = 0;
        foreach (Element element in stencil.Elements)
        {
          if (element is Arc)
          {
            Arc arc = element as Arc;
            elementTemplate[i] = new ArcTemplate((float)arc.x, (float)arc.y, (float)arc.w, (float)arc.h, (float)arc.a, (float)arc.s);
          }
          if (element is Line)
          {
            Line line = element as Line;
            double x1, y1, x2, y2;

            if (mirrorX)
              x1 = 100.0 - line.x1;
            else
              x1 = line.x1;

            if (mirrorY)
              y1 = 100.0 - line.y1;
            else
              y1 = line.y1;

            if (mirrorX)
              x2 = 100.0 - line.x2;
            else
              x2 = line.x2;

            if (mirrorY)
              y2 = 100.0 - line.y2;
            else
              y2 = line.y2;

            elementTemplate[i] = new LineTemplate((float)x1, (float)y1, (float)x2, (float)y2);
          }
          if (element is Bezier)
          {
            Bezier bezier = element as Bezier;
            double x1, y1, x2, y2, x3, y3, x4, y4;

            if (mirrorX)
              x1 = 100.0 - bezier.x1;
            else
              x1 = bezier.x1;

            if (mirrorY)
              y1 = 100.0 - bezier.y1;
            else
              y1 = bezier.y1;

            if (mirrorX)
              x2 = 100.0 - bezier.x2;
            else
              x2 = bezier.x2;

            if (mirrorY)
              y2 = 100.0 - bezier.y2;
            else
              y2 = bezier.y2;

            if (mirrorX)
              x3 = 100.0 - bezier.x3;
            else
              x3 = bezier.x3;

            if (mirrorY)
              y3 = 100.0 - bezier.y3;
            else
              y3 = bezier.y3;

            if (mirrorX)
              x4 = 100.0 - bezier.x4;
            else
              x4 = bezier.x4;

            if (mirrorY)
              y4 = 100.0 - bezier.y4;
            else
              y4 = bezier.y4;

            elementTemplate[i] = new BezierTemplate((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
          }
          i++;
        }

        ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.Decorations.Count];
        i = 0;
        foreach (Element decoration in stencil.Decorations)
        {
          if (decoration is Arc)
          {
            Arc arc = decoration as Arc;
            decorationTemplate[i] = new ArcTemplate((float)arc.x, (float)arc.y, (float)arc.w, (float)arc.h, (float)arc.a, (float)arc.s);
          }
          if (decoration is Line)
          {
            Line line = decoration as Line;
            double x1, y1, x2, y2;

            if (mirrorX)
              x1 = 100.0 - line.x1;
            else
              x1 = line.x1;

            if (mirrorY)
              y1 = 100.0 - line.y1;
            else
              y1 = line.y1;

            if (mirrorX)
              x2 = 100.0 - line.x2;
            else
              x2 = line.x2;

            if (mirrorY)
              y2 = 100.0 - line.y2;
            else
              y2 = line.y2;

            decorationTemplate[i] = new LineTemplate((float)x1, (float)y1, (float)x2, (float)y2);
          }
          if (decoration is Bezier)
          {
            Bezier bezier = decoration as Bezier;
            double x1, y1, x2, y2, x3, y3, x4, y4;

            if (mirrorX)
              x1 = 100.0 - bezier.x1;
            else
              x1 = bezier.x1;

            if (mirrorY)
              y1 = 100.0 - bezier.y1;
            else
              y1 = bezier.y1;

            if (mirrorX)
              x2 = 100.0 - bezier.x2;
            else
              x2 = bezier.x2;

            if (mirrorY)
              y2 = 100.0 - bezier.y2;
            else
              y2 = bezier.y2;

            if (mirrorX)
              x3 = 100.0 - bezier.x3;
            else
              x3 = bezier.x3;

            if (mirrorY)
              y3 = 100.0 - bezier.y3;
            else
              y3 = bezier.y3;

            if (mirrorX)
              x4 = 100.0 - bezier.x4;
            else
              x4 = bezier.x4;

            if (mirrorY)
              y4 = 100.0 - bezier.y4;
            else
              y4 = bezier.y4;

            decorationTemplate[i] = new BezierTemplate((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
          }
          i++;
        }

        box2.Shape = (new ShapeTemplate(elementTemplate, decorationTemplate, null, stencil.FillMode, stencil.Tag));
      }
    }

    private void openToolStripMenuItem_Click(object sender, EventArgs e)
    {
      OpenFileDialog openFileDialog = new OpenFileDialog();
      openFileDialog.AddExtension = true;
      openFileDialog.CheckFileExists = true;
      openFileDialog.CheckPathExists = true;
      openFileDialog.DefaultExt = "GraphicStencil";
      openFileDialog.DereferenceLinks = true;
      openFileDialog.Filter = "Graphic Stencil|*.GraphicStencil|Model Stencil|*.ModelStencil";
      openFileDialog.Multiselect = false;
      openFileDialog.ValidateNames = true;

      if (openFileDialog.ShowDialog() == DialogResult.OK)
      {
        string baseName = openFileDialog.FileName;
        baseName = baseName.Replace(".GraphicStencil", "");
        baseName = baseName.Replace(".ModelStencil", "");
        baseName = baseName.Replace(".graphicstencil", "");
        baseName = baseName.Replace(".modelstencil", "");

        Text = baseName;

        filename = baseName;

        this.elementTextBox.TextChanged -= new System.EventHandler(this.textBox_TextChanged);
        this.decorationTextBox.TextChanged -= new System.EventHandler(this.textBox_TextChanged);
        this.textAreaTextBox.TextChanged -= new System.EventHandler(this.textBox_TextChanged);
        this.anchorTextBox.TextChanged -= new System.EventHandler(this.textBox_TextChanged);

        toolStripComboBoxModelGroup.Text = "";

        elementTextBox.Text = "";
        decorationTextBox.Text = "";
        textAreaTextBox.Text = "";

        {
          SoapFormatter sf;
          Stream stream = null;

          try
          {
            sf = new SoapFormatter();
            stream = new StreamReader(baseName + ".GraphicStencil").BaseStream;

            graphicStencil = (GraphicStencil)sf.Deserialize(stream);
            stream.Close();

            Generate(graphicStencil.Elements, graphicStencil.defaultSize, elementTextBox);
            Generate(graphicStencil.Decorations, graphicStencil.defaultSize, decorationTextBox);
            Generate(graphicStencil.TextArea, graphicStencil.defaultSize, textAreaTextBox);
          }
          catch
          {
            if (stream != null) stream.Close();
            try
            {
              String stencilString;

              // This code creates an empty GraphicStencil1 file to help create the Replace strings below.
              {
                SoapFormatter sf1 = new SoapFormatter();
                Stream stream1 = new StreamWriter(filename + ".GraphicStencil1.new").BaseStream;
                GraphicStencil1 graphicStencil1 = new GraphicStencil1();
                graphicStencil1.Elements = new ArrayList();
                graphicStencil1.Elements.Add(new Arc(1.0F, 2.0F, 1.0F, 2.0F, 1.0F, 2.0F));

                sf1.Serialize(stream1, graphicStencil1);
                stream1.Close();
              }

              {
                sf = new SoapFormatter();
                StreamReader streamReader = new StreamReader(baseName + ".GraphicStencil");
                stencilString = streamReader.ReadToEnd();

                //stencilString = stencilString.Replace(
                //    "http://schemas.microsoft.com/clr/nsassem/SysCAD.Protocol/Protocol%2C%20Version%3D4.2.0.28260%2C%20Culture%3Dneutral%2C%20PublicKeyToken%3D8cb361a6244c44c8",
                //    "http://schemas.microsoft.com/clr/nsassem/SysCAD.Protocol/Stencils1%2C%20Version%3D0.0.0.0%2C%20Culture%3Dneutral%2C%20PublicKeyToken%3Dnull"
                //    );
                stencilString = stencilString.Replace("GraphicStencil", "GraphicStencil1");
                streamReader.Close();

                StreamWriter streamWriter = new StreamWriter(filename + ".GraphicStencil1");
                streamWriter.Write(stencilString);
                streamWriter.Close();
              }

              {
                sf = new SoapFormatter();
                stream = new StreamReader(baseName + ".GraphicStencil1").BaseStream;

                GraphicStencil1 graphicStencil1 = (GraphicStencil1)sf.Deserialize(stream);
                stream.Close();

                graphicStencil.Elements = graphicStencil1.Elements;
                graphicStencil.Decorations = graphicStencil1.Decorations;
                graphicStencil.defaultSize = new SysCAD.Protocol.Size(graphicStencil1.defaultSize);
                graphicStencil.fillMode = graphicStencil1.fillMode;
                graphicStencil.groupName = graphicStencil1.groupName;
                graphicStencil.TextArea = new SysCAD.Protocol.Rectangle(0.0, graphicStencil.defaultSize.Height * 1.1, graphicStencil.defaultSize.Width, 5.0);
                Generate(graphicStencil.Elements, graphicStencil.defaultSize, elementTextBox);
                Generate(graphicStencil.Decorations, graphicStencil.defaultSize, decorationTextBox);
                Generate(graphicStencil.TextArea, graphicStencil.defaultSize, textAreaTextBox);
              }
            }
            catch
            {
            }
          }
          if (stream != null) stream.Close();
        }

        {
          SoapFormatter sf;
          Stream stream = null;

          try
          {
            sf = new SoapFormatter();
            stream = new StreamReader(baseName + ".GraphicStencil").BaseStream;

            graphicStencil = (GraphicStencil)sf.Deserialize(stream);
            stream.Close();

            if (elementTextBox.Text == "")
              Generate(graphicStencil.Elements, graphicStencil.defaultSize, elementTextBox);
            if (decorationTextBox.Text == "")
              Generate(graphicStencil.Decorations, graphicStencil.defaultSize, decorationTextBox);
          }
          catch
          {
            if (stream != null) stream.Close();
            try
            {
              String stencilString;

              // This code creates an empty GraphicStencil1 file to help create the Replace strings below.
              {
                SoapFormatter sf1 = new SoapFormatter();
                Stream stream1 = new StreamWriter(filename + ".GraphicStencil1.new").BaseStream;
                GraphicStencil1 graphicStencil1 = new GraphicStencil1();
                graphicStencil1.Elements = new ArrayList();
                graphicStencil1.Elements.Add(new Arc(1.0F, 2.0F, 1.0F, 2.0F, 1.0F, 2.0F));

                sf1.Serialize(stream1, graphicStencil1);
                stream1.Close();
              }

              {
                sf = new SoapFormatter();
                StreamReader streamReader = new StreamReader(baseName + ".GraphicStencil");
                stencilString = streamReader.ReadToEnd();

                //stencilString = stencilString.Replace(
                //    "http://schemas.microsoft.com/clr/nsassem/SysCAD.Protocol/Protocol%2C%20Version%3D4.2.0.28260%2C%20Culture%3Dneutral%2C%20PublicKeyToken%3D8cb361a6244c44c8",
                //    "http://schemas.microsoft.com/clr/nsassem/SysCAD.Protocol/Stencils1%2C%20Version%3D0.0.0.0%2C%20Culture%3Dneutral%2C%20PublicKeyToken%3Dnull"
                //    );
                stencilString = stencilString.Replace("GraphicStencil", "GraphicStencil1");
                streamReader.Close();

                StreamWriter streamWriter = new StreamWriter(filename + ".GraphicStencil1");
                streamWriter.Write(stencilString);
                streamWriter.Close();
              }

              {
                sf = new SoapFormatter();
                stream = new StreamReader(baseName + ".GraphicStencil1").BaseStream;

                GraphicStencil1 graphicStencil1 = (GraphicStencil1)sf.Deserialize(stream);
                stream.Close();

                graphicStencil.Elements = graphicStencil1.Elements;
                graphicStencil.Decorations = graphicStencil1.Decorations;
                graphicStencil.defaultSize = new SysCAD.Protocol.Size(graphicStencil1.defaultSize);
                graphicStencil.fillMode = graphicStencil1.fillMode;
                graphicStencil.groupName = graphicStencil1.groupName;
                graphicStencil.TextArea = new SysCAD.Protocol.Rectangle(0.0, graphicStencil.defaultSize.Height * 1.1, graphicStencil.defaultSize.Width, 5.0);
                Generate(graphicStencil.Elements, graphicStencil.defaultSize, elementTextBox);
                Generate(graphicStencil.Decorations, graphicStencil.defaultSize, decorationTextBox);
                Generate(graphicStencil.TextArea, graphicStencil.defaultSize, textAreaTextBox);
              }
            }
            catch
            {
            }
        }
        }


        Parse(modelStencil.Elements, elementTextBox);
        Parse(modelStencil.Decorations, decorationTextBox);
        ParseAnchor(modelStencil.Anchors, anchorTextBox);
        UpdateStencil(modelStencil);

        Parse(graphicStencil.Elements, elementTextBox);
        Parse(graphicStencil.Decorations, decorationTextBox);

        SysCAD.Protocol.Rectangle textArea = graphicStencil.TextArea;
        ParseTextArea(ref textArea, textAreaTextBox);
        graphicStencil.TextArea = textArea;

        UpdateStencil(graphicStencil);

        this.elementTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
        this.decorationTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
        this.textAreaTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
        this.anchorTextBox.TextChanged += new System.EventHandler(this.textBox_TextChanged);
      }
    }

    double xj = double.NaN;
    double yj = double.NaN;

    private void Generate(ArrayList arrayList, SysCAD.Protocol.Size defaultSize, TextBox textBox)
    {
      bool firstLineInPoly = true;

      tempText = "";
      xj = float.NaN;
      yj = float.NaN;

      foreach (object element in arrayList)
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
        else if (element is Anchor)
        {
          if (!firstLineInPoly)
            tempText += "\r\n";

          Anchor anchor = element as Anchor;
          GenerateAnchor(anchor, defaultSize);

          firstLineInPoly = true;
        }
      }

      textBox.Text = tempText;
    }

    private void Generate(SysCAD.Protocol.Rectangle rectangle, SysCAD.Protocol.Size defaultSize, TextBox textBox)
    {
      textBox.Text = rectangle.X.ToString() + ", " +
                     rectangle.Y.ToString() + ", " +
                     rectangle.Width.ToString() + ", " +
                     rectangle.Height.ToString();
    }

    private void GeneratePoly(bool first, Line line, SysCAD.Protocol.Size defaultSize)
    {
      if ((first)
         ||
         ((line.x1 != xj) || (line.y1 != yj)))
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

    private void GenerateArc2(Arc arc, SysCAD.Protocol.Size defaultSize)
    {
      tempText += "MDrw_Arc2 " +
                  (arc.x / 100.0F * defaultSize.Width).ToString() + ", " + (arc.y / 100.0F * defaultSize.Height).ToString() + ", " +
                  (arc.w / 100.0F * defaultSize.Width).ToString() + ", " + (arc.h / 100.0F * defaultSize.Height).ToString() + ", " +
                  arc.a.ToString() + ", " + arc.s.ToString() + "\r\n";
    }

    private void GenerateBezier(Bezier bezier, SysCAD.Protocol.Size defaultSize)
    {
      tempText += "MDrw_Bezier " +
                  (bezier.x1 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y1 / 100.0F * defaultSize.Height).ToString() + ", " +
                  (bezier.x2 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y2 / 100.0F * defaultSize.Height).ToString() + ", " +
                  (bezier.x3 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y3 / 100.0F * defaultSize.Height).ToString() + ", " +
                  (bezier.x4 / 100.0F * defaultSize.Width).ToString() + ", " + (bezier.y4 / 100.0F * defaultSize.Height).ToString() + "\r\n";
    }

    private void GenerateAnchor(Anchor anchor, SysCAD.Protocol.Size defaultSize)
    {
      tempText += "MDrw_Anchor ";
      tempText += anchor.Tag + ", ";
      tempText += ((anchor.Positions[0] as SysCAD.Protocol.Point).X / 100.0F * defaultSize.Width).ToString() +
        ", "
        + ((anchor.Positions[0] as SysCAD.Protocol.Point).Y / 100.0F * defaultSize.Height).ToString() + "\r\n";
    }

    private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveFileDialog = new SaveFileDialog();
      saveFileDialog.AddExtension = true;
      saveFileDialog.CheckPathExists = true;
      saveFileDialog.DefaultExt = "GraphicStencil";
      saveFileDialog.DereferenceLinks = true;
      saveFileDialog.Filter = "Graphic Stencil|*.GraphicStencil|Model Stencil|*.ModelStencil";
      saveFileDialog.ValidateNames = true;
      saveFileDialog.AddExtension = false;

      if (saveFileDialog.ShowDialog() == DialogResult.OK)
      {
        String baseName = saveFileDialog.FileName;

        baseName = baseName.Replace(".GraphicStencil", "");
        baseName = baseName.Replace(".ModelStencil", "");
        baseName = baseName.Replace(".graphicstencil", "");
        baseName = baseName.Replace(".modelstencil", "");

        filename = baseName;

        saveToolStripMenuItem_Click(sender, e);
      }
    }

    private void saveToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (filename.Length > 0)
      {
        {
          SoapFormatter sf = new SoapFormatter();
          Stream stream = new StreamWriter(filename+".GraphicStencil").BaseStream;
          sf.Serialize(stream, graphicStencil);
          stream.Close();
        }
        {
          SoapFormatter sf = new SoapFormatter();
          Stream stream = new StreamWriter(filename+".ModelStencil").BaseStream;
          sf.Serialize(stream, modelStencil);
          stream.Close();
        }
      }
      else
        saveAsToolStripMenuItem_Click(sender, e);
    }

    private void exitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Application.Exit();
    }

    private void cutToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (tabControl1.SelectedTab == anchorTabPage)
        anchorTextBox.Cut();
      else if (tabControl1.SelectedTab == textAreaTabPage)
        textAreaTextBox.Cut();
      else if (tabControl1.SelectedTab == elementTabPage)
        elementTextBox.Cut();
      else if (tabControl1.SelectedTab == decorationTabPage)
        decorationTextBox.Cut();
    }

    private void copyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (tabControl1.SelectedTab == anchorTabPage)
        anchorTextBox.Copy();
      else if (tabControl1.SelectedTab == textAreaTabPage)
        textAreaTextBox.Copy();
      else if (tabControl1.SelectedTab == elementTabPage)
        elementTextBox.Copy();
      else if (tabControl1.SelectedTab == decorationTabPage)
        decorationTextBox.Copy();
    }

    private void pasteToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (tabControl1.SelectedTab == anchorTabPage)
        anchorTextBox.Paste();
      else if (tabControl1.SelectedTab == textAreaTabPage)
        textAreaTextBox.Paste();
      else if (tabControl1.SelectedTab == elementTabPage)
        elementTextBox.Paste();
      else if (tabControl1.SelectedTab == decorationTabPage)
        decorationTextBox.Paste();
    }

    private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (tabControl1.SelectedTab == anchorTabPage)
        anchorTextBox.SelectAll();
      else if (tabControl1.SelectedTab == textAreaTabPage)
        textAreaTextBox.SelectAll();
      else if (tabControl1.SelectedTab == elementTabPage)
        elementTextBox.SelectAll();
      else if (tabControl1.SelectedTab == decorationTabPage)
        decorationTextBox.SelectAll();
    }

    private void undoToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (tabControl1.SelectedTab == anchorTabPage)
        anchorTextBox.Undo();
      else if (tabControl1.SelectedTab == textAreaTabPage)
        textAreaTextBox.Undo();
      else if (tabControl1.SelectedTab == elementTabPage)
        elementTextBox.Undo();
      else if (tabControl1.SelectedTab == decorationTabPage)
        decorationTextBox.Undo();
    }

    private void copyAllToolStripMenuItem_Click(object sender, EventArgs e)
    {
      tempText = "";

      tempText += "Fill Elements:\r\n";

      tempText += elementTextBox.Text + "\r\n";

      tempText += "Decoration:\r\n";
      
      tempText += decorationTextBox.Text + "\r\n";

      tempText += "Text Area:\r\n";

      tempText += textAreaTextBox.Text + "\r\n";

      tempText += "Anchors:\r\n";

      tempText += anchorTextBox.Text + "\r\n";

      Clipboard.SetDataObject(tempText);
    }

    private void toolStripComboBoxModelGroup_Click(object sender, EventArgs e)
    {
      modelStencil.GroupName = toolStripComboBoxModelGroup.Text;
    }
  }
}