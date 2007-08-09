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
    internal class PropertyRadioButtonFeel : PropertyFeel
    {
        public PropertyRadioButtonFeel(PropertyGrid grid) : base(grid)
        {
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceRadioButton radioButton;

            if (mInPlaceCtrl == null)
            {
                radioButton = new PropInPlaceRadioButton();
                radioButton.Visible = false;
                radioButton.Parent = mParentWnd;
                mInPlaceCtrl = radioButton;
            }
            else
                radioButton = (PropInPlaceRadioButton)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

//            radioButton.Text = propEnum.Property.Value.DisplayString;
            radioButton.OwnerPropertyEnumerator = propEnum;
            radioButton.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
