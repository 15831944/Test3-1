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
    internal class PropertyMaskedEditFeel : PropertyFeel
    {
        public PropertyMaskedEditFeel(PropertyGrid grid) : base(grid)
        {
        }

        public override System.Windows.Forms.Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceMaskedTextbox edit;

	        if (mInPlaceCtrl == null)
	        {
                edit = new PropInPlaceMaskedTextbox();
                edit.Visible = false;
                edit.Parent = mParentWnd;
                mInPlaceCtrl = edit;
            }
	        else
                edit = (PropInPlaceMaskedTextbox)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

            if (propEnum.Property.Value.Look is PropertyMaskedEditLook)
            {
                edit.Mask = (propEnum.Property.Value.Look as PropertyMaskedEditLook).Mask;
                edit.PromptChar = (propEnum.Property.Value.Look as PropertyMaskedEditLook).PromptChar;
            }

	        edit.OwnerPropertyEnumerator = propEnum;
            edit.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);
            /*
            if (edit.Focused == false)		// are we already being editing ? If yes, don't touch the edit content
	        {
                if (edit.Text != propEnum.Property.Value.DisplayString)
                    edit.Text = propEnum.Property.Value.DisplayString;
            }
            */
            base.ShowControl(valueRect, propEnum);

            // This one + the one in PropInPlaceMaskedTextbox.OnGotFocus are necessary so that when the
            // user clicks for the first time in the MaskedTextox, it doesn't scroll oddly to the right of the box.
            // I guess this is a bug of the .NET framework since this doesn't occur with a regular TextBox.
//            edit.Select(0, 0);

            return edit;
        }

        public override void MoveControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
	        if (mInPlaceCtrl == null)
		        return;

            mInPlaceCtrl.BackColor = propEnum.Property.Value.BackColor;

            // Find where the string is actually displayed (a look instance could place it anywhere)
            Graphics graphics = mParentWnd.CreateGraphics();
	        Rectangle strRect = propEnum.Property.Value.GetStringValueRect(graphics, valueRect, Point.Empty /* not used */);
            strRect.X -= (propEnum.Property.Value.EditboxLeftMargin == -1 ?
                propEnum.Property.ParentGrid.GlobalTextMargin :
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
