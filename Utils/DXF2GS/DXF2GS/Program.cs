using System.IO;
using System;
using SysCAD.Protocol;
using System.Runtime.Serialization.Formatters.Soap;
using System.Collections;

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
        graphicStencil.Tag = Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1].Replace('_', ' ');
        graphicStencil.Elements = new ArrayList();
        graphicStencil.Decorations = new ArrayList();
        graphicStencil.defaultSize = new Size(0.0, 0.0);
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
                  Double yc = -Double.Parse(words[2]);
                  Double r = Double.Parse(words[3]);
                  Double x1 = Double.Parse(words[4]);
                  Double y1 = -Double.Parse(words[5]);
                  Double x2 = Double.Parse(words[6]);
                  Double y2 = -Double.Parse(words[7]);
                  
                  Double a1 = Math.Atan2(y1 - yc, x1 - xc);
                  Double a2 = Math.Atan2(y2 - yc, x2 - xc);

                  if (Math.Abs(a1 - a2) < 0.001)
                  {
                    graphicStencil.Decorations.Add(
                      new Arc(
                        ((xc - r) - xMin) / (xMax - xMin) * 100.0,
                        ((yc - r) - yMin) / (yMax - yMin) * 100.0,
                        (2.0 * r) / (xMax - xMin) * 100.0,
                        (2.0 * r) / (yMax - yMin) * 100.0,
                        0.0,
                        360.0
                      )
                    );
                  }
                  else
                  {
                    graphicStencil.Decorations.Add(
                      Arc3(
                      x1, y1,
                      x2, y2,
                      xc, yc, r,
                      xMin, xMax, yMin, yMax)
                    );
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
                graphicStencil.defaultSize.Width = Double.Parse(words[1]);
                break;

              case "Height":
                graphicStencil.defaultSize.Height = Double.Parse(words[1]);
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
                break;

              // Ignore.
              case "Insert":
              case "Symbol":
              case "Group":
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

        SoapFormatter sf = new SoapFormatter();
        StreamWriter streamWriter = new StreamWriter(args[1] + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1].ToLower().Replace(' ', '_') + ".GraphicStencil");
        Stream stream = streamWriter.BaseStream;
        sf.Serialize(stream, graphicStencil);
        stream.Close();

        Console.WriteLine(args[1] + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1].ToLower().Replace(' ', '_') + ".GraphicStencil");

        if (xMin == xMax)
          Message("Zero width", args[1], path + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1] + ".GraphicStencil", "");

        if (yMin == yMax)
          Message("Zero height", args[1], path + "\\" + Path.GetFileNameWithoutExtension(path).Split("().".ToCharArray(), StringSplitOptions.RemoveEmptyEntries)[1] + ".GraphicStencil", "");
      }

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

    private static Arc Arc3(Double x1, Double y1, Double x2, Double y2, Double xc, Double yc, Double r, Double xMin, Double xMax, Double yMin, Double yMax)
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

    private static Arc Arc3(Double x1, Double y1, Double x2, Double y2, Double x3, Double y3, Double xMin, Double xMax, Double yMin, Double yMax)
    {
      //a = 2*det([1,1,1;x1,x2,x3;y1,y2,y3]);
      Double a = 2.0 * det(1.0, 1.0, 1.0, x1, x2, x3, y1, y2, y3);

      //r = sqrt( ((x2-x3)^2+(y2-y3)^2) ...
      Double r = (Math.Sqrt((Math.Pow(x2 - x3, 2) + Math.Pow(y2 - y3, 2))
        // * ((x3-x1)^2+(y3-y1)^2)
               * (Math.Pow(x3 - x1, 2) + Math.Pow(y3 - y1, 2))
        // * ((x1-x2)^2+(y1-y2)^2) )
               * (Math.Pow(x1 - x2, 2) + Math.Pow(y1 - y2, 2)))
        // / abs(a);
               / Math.Abs(a));

      //xc = - det([1,1,1;y1,y2,y3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;
      Double xc = (-det(1.0, 1.0, 1.0,
                        y1, y2, y3,
                        x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / a);

      //yc =   det([1,1,1;x1,x2,x3;x1^2+y1^2,x2^2+y2^2,x3^2+y3^2]) / a;
      Double yc = (det(1.0, 1.0, 1.0,
                       x1, x2, x3,
                       x1 * x1 + y1 * y1, x2 * x2 + y2 * y2, x3 * x3 + y3 * y3) / a);

      Double a3 = (Math.Atan2(yc - y3, xc - x3) * 180.0 / Math.PI) + 180.0;
      Double a1 = (Math.Atan2(yc - y1, xc - x1) * 180.0 / Math.PI) + 180.0;

      return
        new Arc(
          ((xc - r) - xMin) / (xMax - xMin) * 100.0,
          ((yc - r) - yMin) / (yMax - yMin) * 100.0,
          (2.0 * r) / (xMax - xMin) * 100.0,
          (2.0 * r) / (yMax - yMin) * 100.0,
          a1,
          a3 - a1
        );
    }
  }
}
