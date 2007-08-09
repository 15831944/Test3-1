// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal class PropertyEditFeel : PropertyFeel
    {
        private bool _password;

        public PropertyEditFeel(PropertyGrid grid, bool password) : base(grid)
        {
            _password = password;
        }

        public override System.Windows.Forms.Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceTextbox edit;

	        if (mInPlaceCtrl == null)
	        {
                edit = new PropInPlaceTextbox(_password);

                edit.Visible = false;
                if (_password)
                    edit.PasswordChar = propEnum.Property.ParentGrid.DrawManager.PasswordChar;

                edit.Parent = mParentWnd;
                mInPlaceCtrl = edit;
            }
	        else
                edit = (PropInPlaceTextbox)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

            edit.OwnerPropertyEnumerator = propEnum;
            edit.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);
/*
            if (edit.Focused == false)		// are we already being editing ? If yes, don't touch the edit content
	        {
                if (edit.PasswordChar != 0)
                {
                    if (edit.Text != propEnum.Property.Value.GetStringValue())
                        edit.Text = propEnum.Property.Value.GetStringValue();
                }
                else
                {
                    if (edit.Text != propEnum.Property.Value.DisplayString)
                        edit.Text = propEnum.Property.Value.DisplayString;
                }
            }
*/
            return base.ShowControl(valueRect, propEnum);
        }

        public override void MoveControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
	        if (mInPlaceCtrl == null)
		        return;

            // Find where the string is actually displayed (a look instance could place it anywhere)
            Graphics graphics = mParentWnd.CreateGraphics();
	        Rectangle strRect = propEnum.Property.Value.GetStringValueRect(graphics, valueRect, Point.Empty /* not used */);
            int margin = propEnum.Property.ParentGrid.GlobalTextMargin;
            strRect.X -= (propEnum.Property.Value.EditboxLeftMargin == -1 ? margin :
                propEnum.Property.Value.EditboxLeftMargin);
            strRect.Width = valueRect.Right - strRect.Left;

            // Calculate the height of the editbox, based on the font height

            Rectangle editRect = strRect;
            editRect.Inflate(0,-1);

            Win32Calls.TEXTMETRIC tm = new Win32Calls.TEXTMETRIC();
            Win32Calls.GetTextMetrics(graphics, mInPlaceCtrl.Font, ref tm);

            graphics.Dispose();

            int extraHeight = editRect.Height - tm.tmHeight;
            editRect.Y += extraHeight / 2;
            editRect.Height -= extraHeight;

            if (mInPlaceCtrl.Bounds != editRect)
                mInPlaceCtrl.Bounds = editRect;
        }
    }
}
