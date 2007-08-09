// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2005 VisualHint
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
    internal class PropertyColorChooserFeel : PropertyFeel
    {
        private bool _editable;

        public PropertyColorChooserFeel(PropertyGrid grid, bool editable) : base(grid)
        {
            _editable = editable;
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if ((propEnum.Property.Value.IsReadOnly(propEnum)) && !_editable)
                return null;

            PropInPlaceColorChooser colorChooser;

            if (mInPlaceCtrl == null)
            {
                colorChooser = new PropInPlaceColorChooser(_editable);
                colorChooser.Visible = false;
                colorChooser.Parent = mParentWnd;
                mInPlaceCtrl = colorChooser;
            }
            else
                colorChooser = (PropInPlaceColorChooser)mInPlaceCtrl;

            colorChooser.Font = propEnum.Property.Value.Font;
            colorChooser.Text = propEnum.Property.Value.DisplayString;

            MoveControl(valueRect, propEnum, false);

            colorChooser.OwnerPropertyEnumerator = propEnum;

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
