using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using System.Runtime.Serialization;
using System.IO;

using PureComponents;

namespace SysCAD.Edit
{
  public class NavigationTree : PureComponents.TreeView.TreeView
  {
    public NavigationTree()
      : base()
    {
      SelectionMode = PureComponents.TreeView.SelectionMode.MultipleExtended;
    }
  }  
}
