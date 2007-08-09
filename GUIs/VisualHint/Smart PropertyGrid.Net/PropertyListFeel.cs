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
    internal class PropertyListFeel : PropertyFeel
    {
        private bool _editable;

        public PropertyListFeel(PropertyGrid grid, bool editable) : base(grid)
        {
            _editable = editable;
        }

        public override bool DontShowInPlaceCtrl(PropertyEnumerator propEnum)
        {
            return ((propEnum.Property.Value.IsReadOnly(propEnum)) && !_editable);
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if (DontShowInPlaceCtrl(propEnum))
                return null;

            PropInPlaceList list;

            if (mInPlaceCtrl == null)
            {
                list = new PropInPlaceList(_editable);
                list.Visible = false;
                list.Parent = mParentWnd;
                mInPlaceCtrl = list;
            }
            else
                list = (PropInPlaceList)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

//            list.Text = propEnum.Property.Value.DisplayString;
            list.OwnerPropertyEnumerator = propEnum;
            list.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
