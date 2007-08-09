// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal class PropertyMultilineEditFeel : PropertyFeel
    {
        public PropertyMultilineEditFeel(PropertyGrid grid) : base(grid)
        {
        }

        public override System.Windows.Forms.Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceTextbox edit = null;

            if (mInPlaceCtrl == null)
            {
                edit = new PropInPlaceTextbox(false);
                edit.Visible = false;
                edit.Parent = mParentWnd;
                edit.Multiline = true;
                mInPlaceCtrl = edit;
            }
            else
                edit = (PropInPlaceTextbox)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

            edit.OwnerPropertyEnumerator = propEnum;
            edit.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            if (edit.Focused == false)		// are we already being editing ? If yes, don't touch the edit content
            {
                if (edit.Text != propEnum.Property.Value.DisplayString)
                    edit.Text = propEnum.Property.Value.DisplayString;
            }

            return base.ShowControl(valueRect, propEnum);
        }

        public override void MoveControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if (mInPlaceCtrl == null)
                return;

            Graphics graphics = mParentWnd.CreateGraphics();
            Rectangle strRect = propEnum.Property.Value.GetStringValueRect(graphics, valueRect, Point.Empty /* not used */);
            graphics.Dispose();
            int margin = propEnum.Property.ParentGrid.GlobalTextMargin;
            strRect.X -= (propEnum.Property.Value.EditboxLeftMargin == -1 ? margin :
                propEnum.Property.Value.EditboxLeftMargin);
            strRect.Width = valueRect.Right - strRect.Left - margin;
            strRect.Y += 2;
            strRect.Height = valueRect.Bottom - strRect.Y - 1;

            if (mInPlaceCtrl.Bounds != strRect)
                mInPlaceCtrl.Bounds = strRect;
        }
    }
}
