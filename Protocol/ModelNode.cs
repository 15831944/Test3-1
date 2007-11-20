using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
{
  [Serializable]
  public class ModelNode : ModelItem
  {
    private NodeClass modelClass;

    public ModelNode(Guid guid, String tag, NodeClass modelClass)
    {
      this.Guid = guid;
      this.Tag = tag;
      this.modelClass = modelClass;
    }

    public NodeClass NodeClass
    {
      get { return modelClass; }
      set { modelClass = value; }
    }
  }
}
