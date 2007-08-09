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
    internal class PropertyUITypeEditorFeel : PropertyFeel
    {
        private System.Drawing.Design.UITypeEditor _editor;

        public PropertyUITypeEditorFeel(System.Drawing.Design.UITypeEditor editor, PropertyGrid grid)
            : base(grid)
        {
            _editor = editor;
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
//            if (propEnum.Property.Value.IsReadOnly(propEnum))
  //              return null;

            PropInPlaceUITypeEditor inPlaceCtrl;

            if (mInPlaceCtrl == null)
            {
                inPlaceCtrl = new PropInPlaceUITypeEditor(_editor, true);
                inPlaceCtrl.Visible = false;
                inPlaceCtrl.Parent = mParentWnd;
                mInPlaceCtrl = inPlaceCtrl;
            }
            else
                inPlaceCtrl = (PropInPlaceUITypeEditor)mInPlaceCtrl;

            NotifyInPlaceControlCreated(propEnum);
/*
            if (propEnum.Property.Value.HasMultipleValues)
                inPlaceCtrl.Text = "";
            else
                inPlaceCtrl.Text = propEnum.Property.Value.DisplayString;
*/
            inPlaceCtrl.OwnerPropertyEnumerator = propEnum;
            inPlaceCtrl.Font = propEnum.Property.Value.Font;

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }
    }
}
