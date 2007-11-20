using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Size
  {
    private Double width;
    private Double height;

    public static Size Empty
    {
      get { return new Size(0.0, 0.0); }
    }

    //
    // Summary:
    //     Initializes a new instance of the SysCAD.Protocol.Size class from the specified
    //     SysCAD.Protocol.Point.
    //
    // Parameters:
    //   pt:
    //     The SysCAD.Protocol.Point from which to initialize this SysCAD.Protocol.Size.
    public Size(Point pt)
    {
      width = pt.X;
      height = pt.Y;
    }

    //
    // Summary:
    //     Initializes a new instance of the SysCAD.Protocol.Size class from the specified
    //     existing SysCAD.Protocol.Size.
    //
    // Parameters:
    //   size:
    //     The SysCAD.Protocol.Size from which to create the new SysCAD.Protocol.Size.
    public Size(Size size)
    {
      width = size.Width;
      height = size.Height;
    }

    public Size(SizeF sizeF)
    {
      width = sizeF.Width;
      height = sizeF.Height;
    }

    //
    // Summary:
    //     Initializes a new instance of the SysCAD.Protocol.Size class from the specified
    //     dimensions.
    //
    // Parameters:
    //   width:
    //     The width component of the new SysCAD.Protocol.Size.
    //
    //   height:
    //     The height component of the new SysCAD.Protocol.Size.
    public Size(Double width, Double height)
    {
      this.width = width;
      this.height = height;
    }

    // Summary:
    //     Subtracts the width and height of one SysCAD.Protocol.Size structure from
    //     the width and height of another SysCAD.Protocol.Size structure.
    //
    // Parameters:
    //   sz2:
    //     The SysCAD.Protocol.Size on the right side of the subtraction operator.
    //
    //   sz1:
    //     The SysCAD.Protocol.Size on the left side of the subtraction operator.
    //
    // Returns:
    //     A SysCAD.Protocol.Size that is the result of the subtraction operation.
    public static Size operator -(Size sz1, Size sz2)
    {
      return Subtract(sz1, sz2);
    }

    //
    // Summary:
    //     Tests whether two SysCAD.Protocol.Size structures are different.
    //
    // Parameters:
    //   sz2:
    //     The SysCAD.Protocol.Size structure on the right of the inequality operator.
    //
    //   sz1:
    //     The SysCAD.Protocol.Size structure on the left of the inequality operator.
    //
    // Returns:
    //     This operator returns true if sz1 and sz2 differ either in width or height;
    //     false if sz1 and sz2 are equal.
    public static bool operator !=(Size sz1, Size sz2)
    {
      return ((sz1.width != sz2.width) || (sz1.height != sz2.height));
    }

    //
    // Summary:
    //     Adds the width and height of one SysCAD.Protocol.Size structure to the width
    //     and height of another SysCAD.Protocol.Size structure.
    //
    // Parameters:
    //   sz2:
    //     The second SysCAD.Protocol.Size to add.
    //
    //   sz1:
    //     The first SysCAD.Protocol.Size to add.
    //
    // Returns:
    //     A System.Drawing.Size structure that is the result of the addition operation.
    public static Size operator +(Size sz1, Size sz2)
    {
      return Add(sz1, sz2);
    }

    //
    // Summary:
    //     Tests whether two SysCAD.Protocol.Size structures are equal.
    //
    // Parameters:
    //   sz2:
    //     The SysCAD.Protocol.Size structure on the right of the equality operator.
    //
    //   sz1:
    //     The SysCAD.Protocol.Size structure on the left side of the equality operator.
    //
    // Returns:
    //     This operator returns true if sz1 and sz2 have equal width and height; otherwise,
    //     false.
    public static bool operator ==(Size sz1, Size sz2)
    {
      return ((sz1.width == sz2.width) && (sz1.height == sz2.height));
    }

    //
    // Summary:
    //     Converts the specified SysCAD.Protocol.Size to a SysCAD.Protocol.Point.
    //
    // Parameters:
    //   size:
    //     The SysCAD.Protocol.Size structure to be converted
    //
    // Returns:
    //     The SysCAD.Protocol.Point structure to which this operator converts.
    public static explicit operator Point(Size size)
    {
      return new Point(size.Width, size.Height);
    }

    // Summary:
    //     Gets or sets the vertical component of this SysCAD.Protocol.Size.
    //
    // Returns:
    //     The vertical component of this SysCAD.Protocol.Size.
    public Double Height
    {
      get { return height;  }
      set { height = value; }
    }

    //
    // Summary:
    //     Gets or sets the horizontal component of this SysCAD.Protocol.Size.
    //
    // Returns:
    //     The horizontal component of this SysCAD.Protocol.Size.
    public Double Width
    {
      get { return width; }
      set { width = value; }
    }

    // Summary:
    //     Adds the width and height of one SysCAD.Protocol.Size structure to the width
    //     and height of another SysCAD.Protocol.Size structure.
    //
    // Parameters:
    //   sz2:
    //     The second SysCAD.Protocol.Size to add.
    //
    //   sz1:
    //     The first SysCAD.Protocol.Size to add.
    //
    // Returns:
    //     A SysCAD.Protocol.Size structure that is the result of the addition operation.
    public static Size Add(Size sz1, Size sz2)
    {
      return new Size(sz1.width + sz2.width, sz1.height + sz2.height);
    }

    //
    // Summary:
    //     Tests to see whether the specified object is a SysCAD.Protocol.Size with
    //     the same dimensions as this SysCAD.Protocol.Size.
    //
    // Parameters:
    //   obj:
    //     The System.Object to test.
    //
    // Returns:
    //     This method returns true if obj is a SysCAD.Protocol.Size and has the same
    //     width and height as this SysCAD.Protocol.Size; otherwise, false.
    public override bool Equals(object obj)
    {
      if (obj is Size)
      {
        Size objSize = obj as Size;
        return ((width == objSize.width) && (height == objSize.height));
      }

      return false;
    }

    public override int GetHashCode()
    {
      return base.GetHashCode() * width.GetHashCode() * height.GetHashCode();
    }

    //
    // Summary:
    //     Subtracts the width and height of one SysCAD.Protocol.Size structure from
    //     the width and height of another SysCAD.Protocol.Size structure.
    //
    // Parameters:
    //   sz2:
    //     The SysCAD.Protocol.Size structure on the right side of the subtraction operator.
    //
    //   sz1:
    //     The SysCAD.Protocol.Size structure on the left side of the subtraction operator.
    //
    // Returns:
    //     The SysCAD.Protocol.Size that is a result of the subtraction operation.
    public static Size Subtract(Size sz1, Size sz2)
    {
      return new Size(sz1.width + sz2.width, sz1.height + sz2.height);
    }

    //
    // Summary:
    //     Converts a SysCAD.Protocol.Size to a SysCAD.Protocol.Point.
    //
    // Returns:
    //     Returns a SysCAD.Protocol.Point structure.
    public Point ToPoint()
    {
      return new Point(width, height);
    }
  }
}
