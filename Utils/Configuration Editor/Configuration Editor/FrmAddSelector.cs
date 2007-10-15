using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Configuration_Editor
{
    public partial class FrmAddSelector : Form
    {
        public FrmAddSelector()
        {
            InitializeComponent();
        }

        private void radioCalculation_CheckedChanged(object sender, EventArgs e)
        {
            btnOk.Enabled = true;
        }
    }
}