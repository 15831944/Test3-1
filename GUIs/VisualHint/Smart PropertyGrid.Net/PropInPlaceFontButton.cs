// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

#endregion

// TODO : implementer un FontLook qui fasse le display de la couleur, soit dans une box soit sur la couleur de la string
// TODO : si on a ajoute la couleur comme enfant de la fonte, il faut developper une listbox de couleurs avec seulement
// les couleurs permises pour une fonte.
namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Inplace control shown in the value column when a button (with textbox or not) is needed to display
    /// the Microsoft font dialog.
    /// </summary>
    [ToolboxItem(false)]
    public class PropInPlaceFontButton : PropInPlaceButton
    {
        public PropInPlaceFontButton() : base(true)
        {
        }

        protected override void RunButton()
        {
            Parent.Focus();

            FontDialog dlg = new FontDialog();
            dlg.Font = (Font)mOwnerPropertyEnum.Property.Value.GetValue();

            if (dlg.ShowDialog() != DialogResult.Cancel)
            {
                mOwnerPropertyEnum.Property.Value.PreviousValue = mOwnerPropertyEnum.Property.Value.GetValue();
                mOwnerPropertyEnum.Property.Value.SetValue(dlg.Font);

                if (mEdit != null)
                    mEdit.Text = mOwnerPropertyEnum.Property.Value.DisplayString;

                mOwnerPropertyEnum.Property.ParentGrid.NotifyPropertyChanged(new PropertyChangedEventArgs(mOwnerPropertyEnum));
            }

            mOwnerPropertyEnum.Property.ParentGrid.OnInPlaceCtrlFinishedEdition();
        }
    }
}
