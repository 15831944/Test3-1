// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Drawing;
using System.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal class PropertyCheckboxFeel : PropertyFeel
    {
        public PropertyCheckboxFeel(PropertyGrid grid) : base(grid)
        {
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceCheckbox checkboxes;

            if (mInPlaceCtrl == null)
            {
                checkboxes = new PropInPlaceCheckbox();
                checkboxes.Visible = false;
                checkboxes.Parent = mParentWnd;
                mInPlaceCtrl = checkboxes;
            }
            else
                checkboxes = (PropInPlaceCheckbox)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

//            checkboxes.Text = propEnum.Property.Value.DisplayString;
            checkboxes.OwnerPropertyEnumerator = propEnum;
            checkboxes.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
