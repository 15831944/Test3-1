// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Windows.Forms;
using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal class PropertyButtonFeel : PropertyFeel
    {
        private bool _editable;

        public PropertyButtonFeel(PropertyGrid grid, bool editable) : base(grid)
        {
            _editable = editable;
        }

        public override bool DontShowInPlaceCtrl(PropertyEnumerator propEnum)
        {
            return ((propEnum.Property.Value.IsReadOnly(propEnum)) && !_editable);
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if ((propEnum.Property.Value.IsReadOnly(propEnum)) && !_editable)
                return null;

            PropInPlaceButton button;

            if (mInPlaceCtrl == null)
            {
                button = new PropInPlaceButton(_editable);
                button.Visible = false;
                button.Parent = mParentWnd;
                mInPlaceCtrl = button;
            }
            else
                button = (PropInPlaceButton)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

//            button.Text = propEnum.Property.Value.DisplayString;

            button.OwnerPropertyEnumerator = propEnum;

            button.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
