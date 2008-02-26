using System.IO;
using System;
using SysCAD.Protocol;
using System.Runtime.Serialization.Formatters.Soap;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;

namespace DXF2GS
{
  class Program
  {
    private static double det(double x11, double x21, double x31,
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

    static void Main(string[] args)
    {
      foreach (String path in Directory.GetFiles(args[0], "*.TxtSym"))
      {
        GraphicStencil graphicStencil = new GraphicStencil();
        String tag = Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1].Replace('_', ' ');
        graphicStencil.Tags = new ArrayList();          
        graphicStencil.Elements = new ArrayList();
        graphicStencil.Decorations = new ArrayList();
        graphicStencil.DefaultSize = new Size(0.0, 0.0);
        Double xMin = 0.0;
        Double xMax = 0.0;
        Double yMin = 0.0;
        Double yMax = 0.0;

        // Extract min/max's for scaling on-the-fly.
        foreach (String line in (new StreamReader(path)).ReadToEnd().Split("\n\r".ToCharArray(), StringSplitOptions.RemoveEmptyEntries))
        {
          string[] words = line.Split(" ,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
          if (words.Length > 0)
          {
            switch (words[0])
            {
              case "XMin":
                xMin = Double.Parse(words[1]);
                break;

              case "XMax":
                xMax = Double.Parse(words[1]);
                break;

              case "YMin":
                yMax = -Double.Parse(words[1]);
                break;

              case "YMax":
                yMin = -Double.Parse(words[1]);
                break;

              case "Group":
                graphicStencil.Tags.Add(words[1] + "/" + tag);
                break;
            }
          }
        }

        // Populate graphicStencil.
        Point oldPt = new Point(Double.NaN, Double.NaN);
        foreach (String line in (new StreamReader(path)).ReadToEnd().Split("\n\r".ToCharArray(), StringSplitOptions.RemoveEmptyEntries))
        {
          string[] words = line.Split(" ,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
          if (words.Length > 0)
          {
            ArrayList pCurvePts = new ArrayList();
            switch (words[0])
            {
              case "MDrw_Poly":
                int i = 1;
                while (i + 3 < words.Length)
                {
                  graphicStencil.Decorations.Add(
                    new Line(
                      (Double.Parse(words[i]) - xMin) / (xMax - xMin) * 100.0,
                      (-Double.Parse(words[i + 1]) - yMin) / (yMax - yMin) * 100.0,
                      (Double.Parse(words[i + 2]) - xMin) / (xMax - xMin) * 100.0,
                      (-Double.Parse(words[i + 3]) - yMin) / (yMax - yMin) * 100.0
                    )
                  );
                  i += 2;
                }
                break;

              case "MDrw_Arc":
                {
                  Double xc = Double.Parse(words[1]);
                  Double yc = Double.Parse(words[2]);
                  Double r = Double.Parse(words[3]);
                  Double s = Double.Parse(words[4]);
                  Double x1 = Double.Parse(words[5]);
                  Double y1 = Double.Parse(words[6]);
                  Double a = Math.Atan2(y1 - yc, x1 - xc);


                  if (s > 1.95 * Math.PI)
                  {
                    graphicStencil.Decorations.Add(
                      new Arc(
                        ((xc - r) - xMin) / (xMax - xMin) * 100.0,
                        ((-yc - r) - yMin) / (yMax - yMin) * 100.0,
                        (2.0 * r) / (xMax - xMin) * 100.0,
                        (2.0 * r) / (yMax - yMin) * 100.0,
                        0.0,
                        360.0
                      )
                    );
                  }
                  else
                  {
                    Bezier bezier = new Bezier();
                    bezier.x1 = x1;
                    bezier.y1 = y1;
                    bezier.x2 = xc + r * Math.Cos(a + s / 3.0);
                    bezier.y2 = yc + r * Math.Sin(a + s / 3.0);
                    bezier.x3 = xc + r * Math.Cos(a + 2.0 * s / 3.0);
                    bezier.y3 = yc + r * Math.Sin(a + 2.0 * s / 3.0);
                    bezier.x4 = xc + r * Math.Cos(a + s);
                    bezier.y4 = yc + r * Math.Sin(a + s);

                    bezier.x1 = (bezier.x1 - xMin) / (xMax - xMin) * 100.0;
                    bezier.y1 = (-bezier.y1 - yMin) / (yMax - yMin) * 100.0;
                    bezier.x2 = (bezier.x2 - xMin) / (xMax - xMin) * 100.0;
                    bezier.y2 = (-bezier.y2 - yMin) / (yMax - yMin) * 100.0;
                    bezier.x3 = (bezier.x3 - xMin) / (xMax - xMin) * 100.0;
                    bezier.y3 = (-bezier.y3 - yMin) / (yMax - yMin) * 100.0;
                    bezier.x4 = (bezier.x4 - xMin) / (xMax - xMin) * 100.0;
                    bezier.y4 = (-bezier.y4 - yMin) / (yMax - yMin) * 100.0;

                    graphicStencil.Decorations.Add(bezier);
                  }
                }
                break;

              case "MDrw_Arc3":
                graphicStencil.Decorations.Add(
                  Arc3(
                  Double.Parse(words[1]), -Double.Parse(words[2]),
                  Double.Parse(words[3]), -Double.Parse(words[4]),
                  Double.Parse(words[5]), -Double.Parse(words[6]),
                  xMin, xMax, yMin, yMax)
                );
                break;

              case "MDrw_PCurve":
                switch (words[1])
                {
                  case "S":
                    pCurvePts.Clear();
                    oldPt = new Point(Double.Parse(words[3]), -Double.Parse(words[4]));
                    break;

                  case "P":
                    graphicStencil.Decorations.Add(
                      new Line(
                        (oldPt.X - xMin) / (xMax - xMin) * 100.0,
                        (oldPt.Y - yMin) / (yMax - yMin) * 100.0,
                        (Double.Parse(words[3]) - xMin) / (xMax - xMin) * 100.0,
                        (-Double.Parse(words[4]) - yMin) / (yMax - yMin) * 100.0
                      )
                    );
                    oldPt = new Point(Double.Parse(words[3]), -Double.Parse(words[4]));
                    break;

                  case "A":
                    Double x2 = Double.Parse(words[3]);
                    Double y2 = -Double.Parse(words[4]);
                    Double xc = Double.Parse(words[5]);
                    Double yc = -Double.Parse(words[6]);
                    Double r = Double.Parse(words[7]);


                    graphicStencil.Decorations.Add(
                      Arc3(
                      oldPt.X, oldPt.Y,
                      x2, y2,
                      xc, yc, r,
                      xMin, xMax, yMin, yMax)
                    );
                    break;

                  default:
                    Message("Unknown MDrw_PCurve option", args[1], path + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1] + ".GraphicStencil", line);
                    break;
                }
                break;

              case "Width":
                graphicStencil.DefaultSize.Width = Double.Parse(words[1]);
                break;

              case "Height":
                graphicStencil.DefaultSize.Height = Double.Parse(words[1]);
                break;

              case "TextArea":
                graphicStencil.TagArea =
                  new Rectangle(
                    (Double.Parse(words[1]) - xMin) / (xMax - xMin) * 100.0,
                    ((-Double.Parse(words[2]) - yMin) - (Double.Parse(words[4]) - yMin)) / (yMax - yMin) * 100.0,
                    (Double.Parse(words[3])) / (xMax - xMin) * 100.0,
                    (Double.Parse(words[4])) / (yMax - yMin) * 100.0
                  );
                break;

              // Already handled.
              case "XMin":
              case "XMax":
              case "YMin":
              case "YMax":
              case "Group":
                break;

              // Ignore.
              case "Insert":
              case "Symbol":
              case "MDrw_TagPos":
              case "MDrw_End":
              case "InsertX":
              case "InsertY":
                break;

              default:
                Message("Unknown command", args[1], path + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1] + ".GraphicStencil", line);
                break;
            }
          }
        }

        GraphicStencil.Serialize(args[1] + "\\" + ((String)(graphicStencil.Tags[0])).Replace('/', '_').ToLower().Replace(' ', '_') + ".GraphicStencil", graphicStencil);

        //Console.WriteLine(args[1] + "\\" + ((String)(graphicStencil.Tags[0])).Replace('/', '_').ToLower().Replace(' ', '_') + ".GraphicStencil");

        if (xMin == xMax)
          Message("Zero width", args[1], path + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1] + ".GraphicStencil", "");

        if (yMin == yMax)
          Message("Zero height", args[1], path + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1] + ".GraphicStencil", "");
      }

      Dictionary<String, ModelStencil> modelStencils = new Dictionary<string, ModelStencil>();

      foreach (String path in Directory.GetFiles(args[1], "*.modelstencil"))
      {
        ModelStencil modelStencil = ModelStencil.Deserialize(path);
        modelStencil = ModelStencil.Deserialize(path);
        modelStencils.Add(modelStencil.Tag, modelStencil);
      }

      foreach (String path in Directory.GetFiles(args[0], "*.TxtMdl"))
      {
        ModelStencil modelStencil = null;
        string classID = "";
        int id = -1;
        string name = "";
        int direction = -1;
        int type = -1;
        int ioRequired = -1;
        int ioMax = -1;
        double height = -1.0;
        string description = "";

        foreach (String line in (new StreamReader(path)).ReadToEnd().Split("\n\r".ToCharArray(), StringSplitOptions.RemoveEmptyEntries))
        {
          string[] words = line.Split(" ,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
          if (words.Length > 0)
          {
            switch (words[0])
            {
              case "ClassId":
                classID = words[1];
                if (!modelStencils.TryGetValue(classID, out modelStencil))
                {
                  Console.WriteLine("ModelStencil '" + classID + "' missing.");
                }
                break;

              case "Port0":  case "Port1":  case "Port2":  case "Port3":  case "Port4":  case "Port5":  case "Port6":  case "Port7":  case "Port8":  case "Port9":
              case "Port10": case "Port11": case "Port12": case "Port13": case "Port14": case "Port15": case "Port16": case "Port17": case "Port18": case "Port19":
              case "Port20": case "Port21": case "Port22": case "Port23": case "Port24": case "Port25": case "Port26": case "Port27": case "Port28": case "Port29":
              case "Port30": case "Port31": case "Port32": case "Port33": case "Port34": case "Port35": case "Port36": case "Port37": case "Port38": case "Port39":
              case "Port40": case "Port41": case "Port42": case "Port43": case "Port44": case "Port45": case "Port46": case "Port47": case "Port48": case "Port49":
              case "Port50": case "Port51": case "Port52": case "Port53": case "Port54": case "Port55": case "Port56": case "Port57": case "Port58": case "Port59":
                {
                  //;Port,  Id, Name, Dirn, Type, IORqd, IOMax, Hgt,  Desc
                  // Port0, 0,  In,   0x30, 0x80, 1,     1,     0.00, Input
                  id = int.Parse(words[1]);
                  name = words[2];
                  direction = int.Parse(words[3].Remove(0, 2), NumberStyles.HexNumber);
                  type = int.Parse(words[4].Remove(0,2), NumberStyles.HexNumber);
                  ioRequired = int.Parse(words[5]);
                  ioMax = int.Parse(words[6]);
                  height = double.Parse(words[7]);
                  description = "";
                  if (words.Length > 8)
                    description = words[8];

                  if (modelStencil != null)
                  {
                    bool found = false;
                    foreach (Anchor anchor in modelStencil.Anchors)
                    {
                      if (anchor.Tag == name)
                        found = true;
                    }
                    if (!found)
                    {
                      if (type == 128)
                      {
                        modelStencil.Anchors.Add(new Anchor(name, AnchorType.Process, 0, 50.0, 50.0));
                      }
                      else if (type == 8192)
                      {
                        modelStencil.Anchors.Add(new Anchor(name, AnchorType.Control, 0, 50.0, 50.0));
                      }
                    }

                    ModelStencil.Serialize(args[1] + "\\" + classID + ".modelstencil", modelStencil);
                  }
                }
                break;

              // Ignore.
              case "ClassName":
              case "CategoryGrp":
              case "CategoryMdl":
              case "ProgID":
              case "ShortDesc":
              case "Desc":
              case ";Port":
                //case "
                break;

              default:
                break;
            }
          }
        }
      }

      Console.WriteLine();
      Console.WriteLine("Done.");
      Console.ReadKey(false);
    }

    private static void Message(String message, String oldPath, String newPath, String line)
    {
      Console.WriteLine();
      Console.WriteLine("!!!!!!!!!!! " + message + ": " + line);
      Console.WriteLine(oldPath + " ---> " + newPath);
      Console.ReadKey(false);
    }

    private static Bezier Arc3(Double x1, Double y1, Double x2, Double y2, Double xc, Double yc, Double r, Double xMin, Double xMax, Double yMin, Double yMax)
    {
      Double a1 = Math.Atan2(y1 - yc, x1 - xc);
      Double a2 = Math.Atan2(y2 - yc, x2 - xc);

      while (a2 <= a1)
        a2 += 2.0 * Math.PI;

      Double a3 = (a1 + a2) / 2.0;
      Double xm = xc + r * Math.Cos(a3);
      Double ym = yc + r * Math.Sin(a3);

      return Arc3(x1, y1, xm, ym, x2, y2, xMin, xMax, yMin, yMax);
    }

    private static Bezier Arc3(Double x1, Double y1, Double x2, Double y2, Double x3, Double y3, Double xMin, Double xMax, Double yMin, Double yMax)
    {
      //a = 2*det([1,1,1;x1,x2,x3;y1,y2,y3]);
      Double aDet = 2.0 * det(1.0, 1.0, 1.0, x1, x2, x3, y1, y2, y3);

      //r = sqrt( ((x2-x3)^2+(y2-y3)^2) ...
      Double r = (Math.Sqrt((Math.Pow(x2 - x3, 2) + Math.Pow(y2 - y3, 2))
        // * ((x3-x1)^2+(y3-y1)^2)
               * (Math.Pow(x3 - x1, 2) + Math.Pow(y3 - y1, 2))
        // * ((x1-x2)^2+(y1-y2)^2) )
               * (Math.Pow(x1 - x2, 2) + Math.Pow(y1 - y2, 2)))
        // / abs(a);
               / Math.Abs(aDet));

      //xc = - det([1,1,1;y1,y2,y3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;
      Double xc = (-det(1.0, 1.0, 1.0,
                        y1, y2, y3,
                        x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / aDet);

      //yc =   det([1,1,1;x1,x2,x3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;
      Double yc = (det(1.0, 1.0, 1.0,
                       x1, x2, x3,
                       x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / aDet);

      Double a1 = (Math.Atan2(yc - y1, xc - x1) * 180.0 / Math.PI) + 180.0;
      Double a2 = (Math.Atan2(yc - y2, xc - x2) * 180.0 / Math.PI) + 180.0;
      Double a3 = (Math.Atan2(yc - y3, xc - x3) * 180.0 / Math.PI) + 180.0;

      Double a;
      Double s;

      if (Math.Sign(a2 - a1) < Math.Sign(a3 - a2))
      {
        // a1>a2 but a2<a3 -- reduce a3...
        a1 -= 360.0;
      }
      else if (Math.Sign(a1 - a2) < Math.Sign(a2 - a3))
      {
        // a1<a2 but a2>a3 -- increase a3...
        a3 += 360.0;
      }

      // they are now going in a single direction, all is ok.
      if (a1 < a3)
      {
        a = a1 / 180.0 * Math.PI;
        s = (a3 - a1) / 180.0 * Math.PI;
      }
      else
      {
        a = a3 / 180.0 * Math.PI;
        s = (a1 - a3) / 180.0 * Math.PI;
      }

      Bezier bezier = new Bezier();
      bezier.x1 = xc + r * Math.Cos(a);
      bezier.y1 = yc + r * Math.Sin(a);
      bezier.x2 = xc + r * Math.Cos(a + s / 3.0);
      bezier.y2 = yc + r * Math.Sin(a + s / 3.0);
      bezier.x3 = xc + r * Math.Cos(a + 2.0 * s / 3.0);
      bezier.y3 = yc + r * Math.Sin(a + 2.0 * s / 3.0);
      bezier.x4 = xc + r * Math.Cos(a + s);
      bezier.y4 = yc + r * Math.Sin(a + s);

      bezier.x1 = (bezier.x1 - xMin) / (xMax - xMin) * 100.0;
      bezier.y1 = (-bezier.y1 - yMin) / (yMax - yMin) * 100.0;
      bezier.x2 = (bezier.x2 - xMin) / (xMax - xMin) * 100.0;
      bezier.y2 = (-bezier.y2 - yMin) / (yMax - yMin) * 100.0;
      bezier.x3 = (bezier.x3 - xMin) / (xMax - xMin) * 100.0;
      bezier.y3 = (-bezier.y3 - yMin) / (yMax - yMin) * 100.0;
      bezier.x4 = (bezier.x4 - xMin) / (xMax - xMin) * 100.0;
      bezier.y4 = (-bezier.y4 - yMin) / (yMax - yMin) * 100.0;

      return bezier;
      //return
      //  new Arc(
      //    ((xc - r) - xMin) / (xMax - xMin) * 100.0,
      //    ((yc - r) - yMin) / (yMax - yMin) * 100.0,
      //    (2.0 * r) / (xMax - xMin) * 100.0,
      //    (2.0 * r) / (yMax - yMin) * 100.0,
      //    a1,
      //    a3 - a1
      //  );
    }
  }
}
