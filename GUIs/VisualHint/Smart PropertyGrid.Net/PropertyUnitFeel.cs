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
    internal class PropertyUnitFeel : PropertyFeel
    {
        public PropertyUnitFeel(PropertyGrid grid) : base(grid)
        {
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            PropInPlaceUnit unit;

            if (mInPlaceCtrl == null)
            {
                unit = new PropInPlaceUnit();
                unit.Visible = false;
                unit.Parent = mParentWnd;
                mInPlaceCtrl = unit;
            }
            else
                unit = (PropInPlaceUnit)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);

//            unit.Text = propEnum.Property.Value.DisplayString;
            unit.OwnerPropertyEnumerator = propEnum;
            unit.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
