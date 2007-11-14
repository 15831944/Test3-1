using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Configuration_Editor
{
    public partial class FrmUnitSelection : Form
    {
        public FrmUnitSelection()
        {
            InitializeComponent();
        }

        public Dictionary<string, BindingSource> UnitBindings
        {
            set
            {
                if (value == null) return;
                foreach (KeyValuePair<string, BindingSource> kvp in value)
                {
                    Label lab = new Label();
                    lab.AutoSize = true;
                    if (kvp.Key.Length > 1)
                        lab.Text = char.ToUpper(kvp.Key[0]).ToString() + kvp.Key.Substring(1);
                    else
                        lab.Text = kvp.Key.ToUpper();

                    lab.Anchor = AnchorStyles.Right;
                    tlpMain.Controls.Add(lab);
                    lab.Dock = DockStyle.None;

                    ComboBox box = new ComboBox();
                    box.DataSource = kvp.Value;
                    box.DisplayMember = "Name";
                    box.DropDownStyle = ComboBoxStyle.DropDownList;
                    tlpMain.Controls.Add(box);
                    box.Anchor = AnchorStyles.Left;                    
                }
            }
        }
    }
}