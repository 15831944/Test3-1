using System;
using System.Collections;

namespace SysCAD.Interface
{
	public class Anchor
	{
    public ArrayList locations = new ArrayList();
    public uint minIn = 0;
    public uint maxIn = 0;
    public uint minOut = 0;
    public uint maxOut = 0;
    
    public Anchor()
		{
      minIn = 0;
      maxIn = 0;
      minOut = 0;
      maxOut = 0;
		}
	}
}
