using System;
using System.Collections;
using System.Drawing;

namespace SysCAD.Interface
{
  [Serializable]
  public class Anchor
	{
    public PointF position;
    public uint minIn = 0;
    public uint maxIn = 0;
    public uint minOut = 0;
    public uint maxOut = 0;

    public Anchor(float X, float Y, uint minIn, uint maxIn, uint minOut, uint maxOut)
		{
      position = new PointF(X, Y);
      this.minIn = minIn;
      this.maxIn = maxIn;
      this.minOut = minOut;
      this.maxOut = maxOut;
		}
	}
}
