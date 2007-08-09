// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System.Drawing;
using System.Windows.Forms;

namespace VisualHint.SmartPropertyGrid
{
    internal class PropertyFontFeel : PropertyFeel
    {
        public PropertyFontFeel(PropertyGrid grid)
            : base(grid)
        {
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceFontButton button;

            if (mInPlaceCtrl == null)
            {
                button = new PropInPlaceFontButton();
                button.Visible = false;
                button.Parent = mParentWnd;
                mInPlaceCtrl = button;
            }
            else
                button = (PropInPlaceFontButton)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

            button.Text = propEnum.Property.Value.DisplayString;
            button.OwnerPropertyEnumerator = propEnum;
            button.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
