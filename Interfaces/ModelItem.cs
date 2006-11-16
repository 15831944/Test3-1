using System;
using System.Data;
using System.Data.OleDb;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections.Generic;

namespace SysCAD.Interface
{
  [Serializable]
  [ComVisible(true)]
  public class ModelItem
  {
    private Guid guid;

    [CategoryAttribute("Model"),
     DescriptionAttribute("Guid of the item."),
     ReadOnlyAttribute(true),
     DisplayName("Guid")]
    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    public ModelItem(Guid guid)
    {
      this.guid = guid;
    }

    public ModelItem()
    {
      this.guid = Guid.NewGuid();
    }
  }
}
