using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Font
  {
    // Would normally not store there, but required for seialization.
    private string familyName;
    private float emSize;
    private FontStyle style;
    private GraphicsUnit unit;
    private byte gdiCharSet;
    private bool gdiVerticalFont;

    public Font()
    {
      System.Drawing.Font tempFont = new System.Drawing.Font("Arial", 10.0F, GraphicsUnit.Millimeter);
      familyName = tempFont.FontFamily.Name;
      emSize = tempFont.Size;
      style = tempFont.Style;
      unit = tempFont.Unit;
      gdiCharSet = tempFont.GdiCharSet;
      gdiVerticalFont = tempFont.GdiVerticalFont;
    }

    public Font(float sizeInMM)
    {
      System.Drawing.Font tempFont = new System.Drawing.Font("Arial", sizeInMM, GraphicsUnit.Millimeter);
      familyName = tempFont.FontFamily.Name;
      emSize = sizeInMM;
      style = tempFont.Style;
      unit = GraphicsUnit.Millimeter;
      gdiCharSet = tempFont.GdiCharSet;
      gdiVerticalFont = tempFont.GdiVerticalFont;
    }

    //public System.Drawing.Font BaseFont
    //{
    //  get
    //  {
    //    return new System.Drawing.Font(familyName, emSize, style, unit, gdiCharSet, gdiVerticalFont);
    //  }
    //  set
    //  {
    //    familyName = value.FontFamily.Name;
    //    emSize = value.Size;
    //    style = value.Style;
    //    unit = value.Unit;
    //    gdiCharSet = value.GdiCharSet;
    //    gdiVerticalFont = value.GdiVerticalFont;
    //  }
    //}

    public System.Drawing.Font ScaledFont(float zoomFactor)
    {
      return new System.Drawing.Font(familyName, emSize * zoomFactor / 75.0F, style, unit, gdiCharSet, gdiVerticalFont);
    }

    public string FamilyName
    {
      get { return familyName; }
      set { familyName = value; }
    }

    public float EmSize
    {
      get { return emSize; }
      set { emSize = value; }
    }
    
    public FontStyle Style
    {
      get { return style; }
      set { style = value; }
    }

    public GraphicsUnit Unit
    {
      get { return unit; }
      set { unit = value; }
    }

    public byte GdiCharSet
    {
      get { return gdiCharSet; }
      set { gdiCharSet = value; }
    }

    public bool GdiVerticalFont
    {
      get { return gdiVerticalFont; }
      set { gdiVerticalFont = value; }
    }
  }
}
