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
    internal class PropertyUpDownFeel : PropertyFeel
    {
        private bool _editable;

        public PropertyUpDownFeel(PropertyGrid grid, bool editable) : base(grid)
        {
            _editable = editable;
        }

        public override bool DontShowInPlaceCtrl(PropertyEnumerator propEnum)
        {
            return ((propEnum.Property.Value.IsReadOnly(propEnum)) && !_editable);
        }

        public override System.Windows.Forms.Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if ((propEnum.Property.Value.IsReadOnly(propEnum)) && !_editable)
                return null;

            PropInPlaceUpDown upDown;

            if (mInPlaceCtrl == null)
            {
                upDown = new PropInPlaceUpDown(_editable);
                upDown.Visible = false;
                upDown.Parent = mParentWnd;
                mInPlaceCtrl = upDown;
            }
            else
                upDown = (PropInPlaceUpDown)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

//            upDown.Text = propEnum.Property.Value.DisplayString;
            upDown.OwnerPropertyEnumerator = propEnum;
            upDown.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
