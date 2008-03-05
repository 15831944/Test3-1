//
// System.Drawing.Rectangle.cs
//
// Author:
//   Mike Kestner (mkestner@speakeasy.net)
//
// Copyright (C) 2001 Mike Kestner
// Copyright (C) 2004 Novell, Inc. http://www.novell.com
//

//
// Copyright (C) 2004 Novell, Inc (http://www.novell.com)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;

namespace SysCAD.Protocol
{

  [Serializable]
  [ComVisible(false)]
  public class Rectangle
  {

    private Double x, y, width, height;

    /// <summary>
    ///	FromLTRB Shared Method
    /// </summary>
    ///
    /// <remarks>
    ///	Produces a Rectangle structure from left, top, right,
    ///	and bottom coordinates.
    /// </remarks>

    public Rectangle(RectangleF rectF)
    {
      x = rectF.X;
      y = rectF.Y;
      width = rectF.Width;
      height = rectF.Height;
    }

    public Rectangle(Rectangle rect)
    {
      x = rect.X;
      y = rect.Y;
      width = rect.Width;
      height = rect.Height;
    }

    public Rectangle Clone()
    {
      return new Rectangle(this);
    }

    private static Rectangle FromLTRB(Double left, Double top,
               Double right, Double bottom)
    {
      return new Rectangle(left, top, right - left, bottom - top);
    }

    /// <summary>
    ///	Inflate Shared Method
    /// </summary>
    ///
    /// <remarks>
    ///	Produces a new Rectangle by inflating an existing 
    ///	Rectangle by the specified coordinate values.
    /// </remarks>

    internal static Rectangle Inflate(Rectangle rect,
              Double x, Double y)
    {
      Rectangle ir = new Rectangle(rect.X, rect.Y, rect.Width, rect.Height);
      ir.Inflate(x, y);
      return ir;
    }

    /// <summary>
    ///	Inflate Method
    /// </summary>
    ///
    /// <remarks>
    ///	Inflates the Rectangle by a specified width and height.
    /// </remarks>

    public void Inflate(Double width, Double height)
    {
      x -= width;
      y -= height;
      this.width += width * 2;
      this.height += height * 2;
    }

    /// <summary>
    ///	Inflate Method
    /// </summary>
    ///
    /// <remarks>
    ///	Inflates the Rectangle by a specified Size.
    /// </remarks>

    internal void Inflate(SizeF sz)
    {
      Inflate(sz.Width, sz.Height);
    }

    /// <summary>
    ///	Intersect Shared Method
    /// </summary>
    ///
    /// <remarks>
    ///	Produces a new Rectangle by intersecting 2 existing 
    ///	Rectangles. Returns null if there is no intersection.
    /// </remarks>

    public static Rectangle Intersect(Rectangle rect1, Rectangle rect2)
    {
      Rectangle rect = new Rectangle(rect1.X, rect1.Y, rect1.Width, rect1.Height);
      rect.Intersect(rect2);
      return rect;
    }

    /// <summary>
    ///	Intersect Method
    /// </summary>
    ///
    /// <remarks>
    ///	Replaces the Rectangle with the intersection of itself
    ///	and another Rectangle.
    /// </remarks>

    public void Intersect(Rectangle rect)
    {

      if (!IntersectsWith(rect))
      {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
      }

      x = Math.Max(Left, rect.Left);
      y = Math.Max(Top, rect.Top);
      width = Math.Min(Right, rect.Right) - X;
      height = Math.Min(Bottom, rect.Bottom) - Y;
    }

    /// <summary>
    ///	Union Shared Method
    /// </summary>
    ///
    /// <remarks>
    ///	Produces a new Rectangle from the union of 2 existing 
    ///	Rectangles.
    /// </remarks>

    public static Rectangle Union(Rectangle r1, Rectangle r2)
    {
      return FromLTRB(Math.Min(r1.Left, r2.Left),
           Math.Min(r1.Top, r2.Top),
           Math.Max(r1.Right, r2.Right),
           Math.Max(r1.Bottom, r2.Bottom));
    }

    /// <summary>
    ///	Equality Operator
    /// </summary>
    ///
    /// <remarks>
    ///	Compares two Rectangle objects. The return value is
    ///	based on the equivalence of the Location and Size 
    ///	properties of the two Rectangles.
    /// </remarks>

    public static bool operator ==(Rectangle r1, Rectangle r2)
    {
      if ((object)r2 == null)
        return ((object)r1 == null); 
      else
        return (r1.X == r2.X) && (r1.Y == r2.Y) && (r1.Width == r2.Width) && (r1.Height == r2.Height);
    }

    /// <summary>
    ///	Inequality Operator
    /// </summary>
    ///
    /// <remarks>
    ///	Compares two Rectangle objects. The return value is
    ///	based on the equivalence of the Location and Size 
    ///	properties of the two Rectangles.
    /// </remarks>

    public static bool operator !=(Rectangle r1, Rectangle r2)
    {
      return (r1.X != r2.X) && (r1.Y != r2.Y) &&
                                (r1.Width != r2.Width) && (r1.Height != r2.Height);
    }

    /// <summary>
    ///	Rectangle to Rectangle Conversion
    /// </summary>
    ///
    /// <remarks>
    ///	Converts a Rectangle object to a Rectangle.
    /// </remarks>

    public static implicit operator System.Drawing.RectangleF(Rectangle rect)
    {
      return new System.Drawing.RectangleF((float)rect.X, (float)rect.Y, (float)rect.Width, (float)rect.Height);
    }

    /// <summary>
    ///	System.Drawing.Rectangle to Rectangle Conversion
    /// </summary>
    ///
    /// <remarks>
    ///	Converts a Rectangle object to a Rectangle.
    /// </remarks>

    public static implicit operator Rectangle(System.Drawing.Rectangle rect)
    {
      return new Rectangle(rect.X, rect.Y, rect.Width, rect.Height);
    }

    // -----------------------
    // Public Constructors
    // -----------------------

    /// <summary>
    ///	Rectangle Constructor
    /// </summary>
    ///
    /// <remarks>
    ///	Creates a Rectangle from PointF and SizeF values.
    /// </remarks>

    public Rectangle(Point loc, Size sz)
    {
      x = loc.X;
      y = loc.Y;
      width = sz.Width;
      height = sz.Height;
    }

    public Rectangle(PointF loc, Size sz)
    {
      x = loc.X;
      y = loc.Y;
      width = sz.Width;
      height = sz.Height;
    }

    public Rectangle(Point loc, SizeF sz)
    {
      x = loc.X;
      y = loc.Y;
      width = sz.Width;
      height = sz.Height;
    }

    public Rectangle(PointF loc, SizeF sz)
    {
      x = loc.X;
      y = loc.Y;
      width = sz.Width;
      height = sz.Height;
    }

    /// <summary>
    ///	Rectangle Constructor
    /// </summary>
    ///
    /// <remarks>
    ///	Creates a Rectangle from a specified x,y location and
    ///	width and height values.
    /// </remarks>

    public Rectangle(Double x, Double y, Double width, Double height)
    {
      this.x = x;
      this.y = y;
      this.width = width;
      this.height = height;
    }

    /// <summary>
    ///	Bottom Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Y coordinate of the bottom edge of the Rectangle.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Double Bottom
    {
      get
      {
        return Y + Height;
      }
    }

    /// <summary>
    ///	Height Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Height of the Rectangle.
    /// </remarks>

    public Double Height
    {
      get
      {
        return height;
      }
      set
      {
        height = value;
      }
    }

    /// <summary>
    ///	IsEmpty Property
    /// </summary>
    ///
    /// <remarks>
    ///	Indicates if the width or height are zero. Read only.
    /// </remarks>
    //
    // LAMESPEC: Documentation says "This property returns true if 
    // the Width, Height, X, and Y properties of this Rectangle all 
    // have values of zero; otherwise, false.". Reality returns TRUE if
    // width or height are equal 0		

    [Browsable(false)]
    public bool IsEmpty
    {
      get
      {
        return ((width == 0) || (height == 0));
      }
    }

    /// <summary>
    ///	Left Property
    /// </summary>
    ///
    /// <remarks>
    ///	The X coordinate of the left edge of the Rectangle.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Double Left
    {
      get
      {
        return X;
      }
    }

    /// <summary>
    ///	Location Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Location of the top-left corner of the Rectangle.
    /// </remarks>

    [Browsable(false)]
    public Point Location
    {
      get
      {
        return new Point(x, y);
      }
      set
      {
        x = value.X;
        y = value.Y;
      }
    }

    /// <summary>
    ///	Right Property
    /// </summary>
    ///
    /// <remarks>
    ///	The X coordinate of the right edge of the Rectangle.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Double Right
    {
      get
      {
        return X + Width;
      }
    }

    /// <summary>
    ///	Size Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Size of the Rectangle.
    /// </remarks>

    [Browsable(false)]
    public Size Size
    {
      get
      {
        return new Size(width, height);
      }
      set
      {
        width = value.Width;
        height = value.Height;
      }
    }

    /// <summary>
    ///	Top Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Y coordinate of the top edge of the Rectangle.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Double Top
    {
      get
      {
        return Y;
      }
    }

    /// <summary>
    ///	Width Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Width of the Rectangle.
    /// </remarks>

    public Double Width
    {
      get
      {
        return width;
      }
      set
      {
        width = value;
      }
    }

    /// <summary>
    ///	X Property
    /// </summary>
    ///
    /// <remarks>
    ///	The X coordinate of the Rectangle.
    /// </remarks>

    public Double X
    {
      get
      {
        return x;
      }
      set
      {
        x = value;
      }
    }

    /// <summary>
    ///	Y Property
    /// </summary>
    ///
    /// <remarks>
    ///	The Y coordinate of the Rectangle.
    /// </remarks>

    public Double Y
    {
      get
      {
        return y;
      }
      set
      {
        y = value;
      }
    }

    /// <summary>
    ///	Contains Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks if an x,y coordinate lies within this Rectangle.
    /// </remarks>

    public bool Contains(Double positionX, Double positionY)
    {
      return ((positionX >= Left) && (positionX <= Right) &&
        (positionY >= Top) && (positionY <= Bottom));
    }

    /// <summary>
    ///	Contains Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks if a Point lies within this Rectangle.
    /// </remarks>

    public bool Contains(PointF point)
    {
      return Contains(point.X, point.Y);
    }

    /// <summary>
    ///	Contains Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks if a Rectangle lies entirely within this 
    ///	Rectangle.
    /// </remarks>

    public bool Contains(Rectangle rect)
    {
      return (rect == Intersect(this, rect));
    }

    /// <summary>
    ///	Equals Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks equivalence of this Rectangle and an object.
    /// </remarks>

    public override bool Equals(object obj)
    {

      if (!(obj is Rectangle))
        return false;

      return (this == (Rectangle)obj);
    }

    /// <summary>
    ///	GetHashCode Method
    /// </summary>
    ///
    /// <remarks>
    ///	Calculates a hashing value.
    /// </remarks>

    public override int GetHashCode()
    {
      return (int)(x + y + width + height);
    }

    /// <summary>
    ///	IntersectsWith Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks if a Rectangle intersects with this one.
    /// </remarks>

    public bool IntersectsWith(Rectangle rect)
    {
      return !((Left > rect.Right) || (Right < rect.Left) ||
          (Top > rect.Bottom) || (Bottom < rect.Top));
    }

    /// <summary>
    ///	Offset Method
    /// </summary>
    ///
    /// <remarks>
    ///	Moves the Rectangle a specified distance.
    /// </remarks>

    public void Offset(Double dx, Double dy)
    {
      X += dx;
      Y += dy;
    }

    /// <summary>
    ///	Offset Method
    /// </summary>
    ///
    /// <remarks>
    ///	Moves the Rectangle a specified distance.
    /// </remarks>

    public void Offset(PointF pt)
    {
      Offset(pt.X, pt.Y);
    }

    /// <summary>
    ///	ToString Method
    /// </summary>
    ///
    /// <remarks>
    ///	Formats the Rectangle in (x,y,w,h) notation.
    /// </remarks>

    public override String ToString()
    {
      return String.Format(CultureInfo.InvariantCulture, "{{X={0},Y={1},Width={2},Height={3}}}",
             x, y, width, height);
    }
  }
}
