//
// System.Drawing.RectangleF.cs
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
  public struct ARectangleF
  {

    private Single x, y, width, height;

    /// <summary>
    ///	FromLTRB Shared Method
    /// </summary>
    ///
    /// <remarks>
    ///	Produces a RectangleF structure from left, top, right,
    ///	and bottom coordinates.
    /// </remarks>

    private static ARectangleF FromLTRB(Single left, Single top,
               Single right, Single bottom)
    {
      return new ARectangleF(left, top, right - left, bottom - top);
    }

    /// <summary>
    ///	Inflate Shared Method
    /// </summary>
    ///
    /// <remarks>
    ///	Produces a new RectangleF by inflating an existing 
    ///	RectangleF by the specified coordinate values.
    /// </remarks>

    internal static ARectangleF Inflate(ARectangleF rect,
              Single x, Single y)
    {
      ARectangleF ir = new ARectangleF(rect.X, rect.Y, rect.Width, rect.Height);
      ir.Inflate(x, y);
      return ir;
    }

    /// <summary>
    ///	Inflate Method
    /// </summary>
    ///
    /// <remarks>
    ///	Inflates the RectangleF by a specified width and height.
    /// </remarks>

    internal void Inflate(Single width, Single height)
    {
      x -= width;
      y -= height;
      width += width * 2;
      height += height * 2;
    }

    /// <summary>
    ///	Inflate Method
    /// </summary>
    ///
    /// <remarks>
    ///	Inflates the RectangleF by a specified Size.
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
    ///	Produces a new RectangleF by intersecting 2 existing 
    ///	RectangleFs. Returns null if there is no intersection.
    /// </remarks>

    public static ARectangleF Intersect(ARectangleF rect1, ARectangleF rect2)
    {
      ARectangleF rect = new ARectangleF(rect1.X, rect1.Y, rect1.Width, rect1.Height);
      rect.Intersect(rect2);
      return rect;
    }

    /// <summary>
    ///	Intersect Method
    /// </summary>
    ///
    /// <remarks>
    ///	Replaces the RectangleF with the intersection of itself
    ///	and another RectangleF.
    /// </remarks>

    public void Intersect(ARectangleF rect)
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
    ///	Produces a new RectangleF from the union of 2 existing 
    ///	RectangleFs.
    /// </remarks>

    public static ARectangleF Union(ARectangleF r1, ARectangleF r2)
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
    ///	Compares two RectangleF objects. The return value is
    ///	based on the equivalence of the Location and Size 
    ///	properties of the two RectangleFs.
    /// </remarks>

    public static bool operator ==(ARectangleF r1, ARectangleF r2)
    {
      return (r1.X == r2.X) && (r1.Y == r2.Y) &&
                                (r1.Width == r2.Width) && (r1.Height == r2.Height);
    }

    /// <summary>
    ///	Inequality Operator
    /// </summary>
    ///
    /// <remarks>
    ///	Compares two RectangleF objects. The return value is
    ///	based on the equivalence of the Location and Size 
    ///	properties of the two RectangleFs.
    /// </remarks>

    public static bool operator !=(ARectangleF r1, ARectangleF r2)
    {
      return (r1.X != r2.X) && (r1.Y != r2.Y) &&
                                (r1.Width != r2.Width) && (r1.Height != r2.Height);
    }

    /// <summary>
    ///	Rectangle to RectangleF Conversion
    /// </summary>
    ///
    /// <remarks>
    ///	Converts a Rectangle object to a RectangleF.
    /// </remarks>

    public static implicit operator ARectangleF(Rectangle rect)
    {
      return new ARectangleF(rect.X, rect.Y, rect.Width, rect.Height);
    }

    /// <summary>
    ///	System.Drawing.RectangleF to RectangleF Conversion
    /// </summary>
    ///
    /// <remarks>
    ///	Converts a Rectangle object to a RectangleF.
    /// </remarks>

    public static implicit operator ARectangleF(System.Drawing.RectangleF rect)
    {
      return new ARectangleF(rect.X, rect.Y, rect.Width, rect.Height);
    }

    public static implicit operator System.Drawing.RectangleF(ARectangleF rect)
    {
      return new System.Drawing.RectangleF(rect.X, rect.Y, rect.Width, rect.Height);
    }

    // -----------------------
    // Public Constructors
    // -----------------------

    /// <summary>
    ///	RectangleF Constructor
    /// </summary>
    ///
    /// <remarks>
    ///	Creates a RectangleF from PointF and SizeF values.
    /// </remarks>

    public ARectangleF(PointF loc, SizeF sz)
    {
      x = loc.X;
      y = loc.Y;
      width = sz.Width;
      height = sz.Height;
    }

    /// <summary>
    ///	RectangleF Constructor
    /// </summary>
    ///
    /// <remarks>
    ///	Creates a RectangleF from a specified x,y location and
    ///	width and height values.
    /// </remarks>

    public ARectangleF(Single x, Single y, Single width, Single height)
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
    ///	The Y coordinate of the bottom edge of the RectangleF.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Single Bottom
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
    ///	The Height of the RectangleF.
    /// </remarks>

    public Single Height
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
    // the Width, Height, X, and Y properties of this RectangleF all 
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
    ///	The X coordinate of the left edge of the RectangleF.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Single Left
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
    ///	The Location of the top-left corner of the RectangleF.
    /// </remarks>

    [Browsable(false)]
    public PointF Location
    {
      get
      {
        return new PointF(x, y);
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
    ///	The X coordinate of the right edge of the RectangleF.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Single Right
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
    ///	The Size of the RectangleF.
    /// </remarks>

    [Browsable(false)]
    public SizeF Size
    {
      get
      {
        return new SizeF(width, height);
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
    ///	The Y coordinate of the top edge of the RectangleF.
    ///	Read only.
    /// </remarks>

    [Browsable(false)]
    public Single Top
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
    ///	The Width of the RectangleF.
    /// </remarks>

    public Single Width
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
    ///	The X coordinate of the RectangleF.
    /// </remarks>

    public Single X
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
    ///	The Y coordinate of the RectangleF.
    /// </remarks>

    public Single Y
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
    ///	Checks if an x,y coordinate lies within this RectangleF.
    /// </remarks>

    public bool Contains(Single positionX, Single positionY)
    {
      return ((positionX >= Left) && (positionX <= Right) &&
        (positionY >= Top) && (positionY <= Bottom));
    }

    /// <summary>
    ///	Contains Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks if a Point lies within this RectangleF.
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
    ///	Checks if a RectangleF lies entirely within this 
    ///	RectangleF.
    /// </remarks>

    public bool Contains(ARectangleF rect)
    {
      return (rect == Intersect(this, rect));
    }

    /// <summary>
    ///	Equals Method
    /// </summary>
    ///
    /// <remarks>
    ///	Checks equivalence of this RectangleF and an object.
    /// </remarks>

    public override bool Equals(object obj)
    {

      if (!(obj is ARectangleF))
        return false;

      return (this == (ARectangleF)obj);
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
    ///	Checks if a RectangleF intersects with this one.
    /// </remarks>

    public bool IntersectsWith(ARectangleF rect)
    {
      return !((Left > rect.Right) || (Right < rect.Left) ||
          (Top > rect.Bottom) || (Bottom < rect.Top));
    }

    /// <summary>
    ///	Offset Method
    /// </summary>
    ///
    /// <remarks>
    ///	Moves the RectangleF a specified distance.
    /// </remarks>

    public void Offset(Single dx, Single dy)
    {
      X += dx;
      Y += dy;
    }

    /// <summary>
    ///	Offset Method
    /// </summary>
    ///
    /// <remarks>
    ///	Moves the RectangleF a specified distance.
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
    ///	Formats the RectangleF in (x,y,w,h) notation.
    /// </remarks>

    public override String ToString()
    {
      return String.Format(CultureInfo.InvariantCulture, "{{X={0},Y={1},Width={2},Height={3}}}",
             x, y, width, height);
    }

  }
}
