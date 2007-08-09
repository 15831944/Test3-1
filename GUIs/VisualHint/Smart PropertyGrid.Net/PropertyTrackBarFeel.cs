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
    internal class PropertyTrackBarFeel : PropertyFeel
    {
        private bool _editable;

        public PropertyTrackBarFeel(PropertyGrid grid, bool editable) : base(grid)
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

            PropInPlaceTrackbar trackbar;

            if (mInPlaceCtrl == null)
            {
                trackbar = new PropInPlaceTrackbar(_editable);
                trackbar.Visible = false;
                trackbar.Parent = mParentWnd;
                mInPlaceCtrl = trackbar;
            }
            else
                trackbar = (PropInPlaceTrackbar)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

            trackbar.OwnerPropertyEnumerator = propEnum;

            trackbar.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
