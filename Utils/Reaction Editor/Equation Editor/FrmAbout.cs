using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace Reaction_Editor
{
    public partial class FrmAbout : Form
    {
        public FrmAbout()
        {
            InitializeComponent();

            label1.Text = string.Format(Reaction_Editor.Properties.Resources.About, Assembly.GetExecutingAssembly().GetName().Version.ToString(3));
        }
    }
}