
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
  public class ModelItem
  {

    private Guid guid;

    public ModelItem()
    {
      this.guid = Guid.NewGuid();
    }

    public ModelItem(Guid guid)
    {
      this.guid = guid;
    }

    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }
  }
}
