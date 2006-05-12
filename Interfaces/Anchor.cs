using System;
using System.Collections;
using System.Drawing;

namespace SysCAD.Interface
{
  [Serializable]
  public class Anchor
	{
    public string tag;
    public string type; // Electrical, Control, or Process
    public PointF position;
    public uint minIn = 0;
    public uint maxIn = 0;
    public uint minOut = 0;
    public uint maxOut = 0;

    public Anchor()
    {
    }

    public Anchor(string tag, string type, PointF position, uint minIn, uint maxIn, uint minOut, uint maxOut)
    {
      this.position = position;
      this.minIn = minIn;
      this.maxIn = maxIn;
      this.minOut = minOut;
      this.maxOut = maxOut;
    }

    public Anchor(string tag, string type, float X, float Y, uint minIn, uint maxIn, uint minOut, uint maxOut)
    {
      this.position = new PointF(X, Y);
      this.minIn = minIn;
      this.maxIn = maxIn;
      this.minOut = minOut;
      this.maxOut = maxOut;
    }
  }
}
