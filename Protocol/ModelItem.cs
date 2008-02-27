using System;
using System.Data;
using System.Data.OleDb;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections.Generic;

namespace SysCAD.Protocol
{
  [Serializable]
  [ComVisible(true)]
  public abstract class ModelItem : Item
  {
    private Guid parent;

    public Guid Parent
    {
      get { return parent; }
      set { parent = value; }
    }
  }
}
