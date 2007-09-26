using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SysCAD.Protocol;

namespace TestApp
{
  public partial class TestAppForm : Form
  {
    public TestAppForm(Dictionary<String, Bitmap> modelThumbnails, 
                       Dictionary<String, ModelStencil> modelStencils)
    {
      InitializeComponent(modelThumbnails, modelStencils);
    }
  }
}