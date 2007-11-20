using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Drawing;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Point
  {
    Double x;
    Double y;

    public static Point Empty
    {
      get { return new Point(0.0, 0.0); }
    }

    //
    // Summary:
    //     Initializes a new instance of the SysCAD.Protocol.Point class with the specified
    //     coordinates.
    //
    // Parameters:
    //   y:
    //     The vertical position of the point.
    //
    //   x:
    //     The horizontal position of the point.
    public Point()
    {
      this.x = 0.0;
      this.y = 0.0;
    }

    public Point(Double x, Double y)
    {
      this.x = x;
      this.y = y;
    }

    public Point(PointF ptF)
    {
      x = ptF.X;
      y = ptF.Y;
    }

    //
    // Summary:
    //     Translates a SysCAD.Protocol.Point by the negative of a specified SysCAD.Protocol.Size.
    //
    // Parameters:
    //   sz:
    //     The SysCAD.Protocol.Size that specifies the numbers to subtract from the
    //     coordinates of pt.
    //
    //   pt:
    //     The SysCAD.Protocol.Point to translate.
    //
    // Returns:
    //     The translated SysCAD.Protocol.Point.
    public static Point operator -(Point pt, Size sz)
    {
      return Subtract(pt, sz);
    }

    //
    // Summary:
    //     Determines whether the coordinates of the specified points are not equal.
    //
    // Parameters:
    //   right:
    //     A SysCAD.Protocol.Point to compare.
    //
    //   left:
    //     A SysCAD.Protocol.Point to compare.
    //
    // Returns:
    //     true to indicate the SysCAD.Protocol.Point.X and SysCAD.Protocol.Point.Y
    //     values of left and right are not equal; otherwise, false.
    public static bool operator !=(Point left, Point right)
    {
      return ((left.X != right.X) || (left.Y != right.Y));
    }

    //
    // Summary:
    //     Translates a SysCAD.Protocol.Point by a given System.Drawing.Size.
    //
    // Parameters:
    //   sz:
    //     A System.Drawing.Size that specifies the pair of numbers to add to the coordinates
    //     of pt.
    //
    //   pt:
    //     The SysCAD.Protocol.Point to translate.
    //
    // Returns:
    //     Returns the translated SysCAD.Protocol.Point.
    public static Point operator +(Point pt, Size sz)
    {
      return Add(pt, sz);
    }

    //
    // Summary:
    //     Compares two SysCAD.Protocol.Point structures. The result specifies whether
    //     the values of the SysCAD.Protocol.Point.X and SysCAD.Protocol.Point.Y properties
    //     of the two SysCAD.Protocol.Point structures are equal.
    //
    // Parameters:
    //   right:
    //     A SysCAD.Protocol.Point to compare.
    //
    //   left:
    //     A SysCAD.Protocol.Point to compare.
    //
    // Returns:
    //     true if the SysCAD.Protocol.Point.X and SysCAD.Protocol.Point.Y values of
    //     the left and right SysCAD.Protocol.Point structures are equal; otherwise,
    //     false.
    public static bool operator ==(Point left, Point right)
    {
      return Equals(left, right);
    }

    //
    // Summary:
    //     Gets or sets the x-coordinate of this SysCAD.Protocol.Point.
    //
    // Returns:
    //     The x-coordinate of this SysCAD.Protocol.Point.
    public Double X
    {
      get { return x; }
      set { x = value; }
    }

    //
    // Summary:
    //     Gets or sets the y-coordinate of this SysCAD.Protocol.Point.
    //
    // Returns:
    //     The y-coordinate of this SysCAD.Protocol.Point.
    public Double Y
    {
      get { return y; }
      set { y = value; }
    }

    // Summary:
    //     Translates a given SysCAD.Protocol.Point by the specified System.Drawing.Size.
    //
    // Parameters:
    //   sz:
    //     The System.Drawing.Size that specifies the numbers to add to the coordinates
    //     of pt.
    //
    //   pt:
    //     The SysCAD.Protocol.Point to translate.
    //
    // Returns:
    //     The translated SysCAD.Protocol.Point.
    public static Point Add(Point pt, Size sz)
    {
      return new Point(pt.X + sz.Width, pt.Y + sz.Height);
    }

    //
    // Summary:
    //     Specifies whether this SysCAD.Protocol.Point contains the same coordinates
    //     as the specified System.Object.
    //
    // Parameters:
    //   obj:
    //     The System.Object to test.
    //
    // Returns:
    //     This method returns true if obj is a SysCAD.Protocol.Point and has the same
    //     coordinates as this System.Drawing.Point.
    public override bool Equals(object obj)
    {
      if (obj is Point)
      {
        Point objPoint = obj as Point;
        return ((x == objPoint.X) && (y == objPoint.Y));
      }

      return false;
    }

    public override int GetHashCode()
    {
      return base.GetHashCode() * x.GetHashCode() * y.GetHashCode();
    }

    //
    // Summary:
    //     Translates a SysCAD.Protocol.Point by the negative of a specified size.
    //
    // Parameters:
    //   sz:
    //     The System.Drawing.Size that specifies the numbers to subtract from the coordinates
    //     of pt.
    //
    //   pt:
    //     The SysCAD.Protocol.Point to translate.
    //
    // Returns:
    //     The translated SysCAD.Protocol.Point.
    public static Point Subtract(Point pt, Size sz)
    {
      return new Point(pt.X - sz.Width, pt.Y - sz.Height);
    }

    public PointF ToPointF()
    {
      return new PointF((float)x, (float)y);
    }
  }
}
