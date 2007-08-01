using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Reaction_Editor
{
    public partial class FrmAddSelector : Form
    {
        public FrmAddSelector()
        {
            InitializeComponent();
        }

        public FrmAddSelector(bool sourceAvailable, bool sinkAvailable, bool HXAvailable)
        {
            radioHX.Enabled = HXAvailable;
            radioSink.Enabled = sinkAvailable;
            radioSource.Enabled = sourceAvailable;
        }
    }
}