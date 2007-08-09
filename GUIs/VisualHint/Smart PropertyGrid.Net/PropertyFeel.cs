// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System.Drawing;

namespace VisualHint.SmartPropertyGrid
{
	/// <summary>
	/// By assigning a feel to a property, it defines the way this property will be edited.
	/// </summary>
    /// <remarks>
    /// There are several ways to associate a specific feel to a property:
    /// <list type="bullet">
    /// <item><term></term><description></description></item>
    /// <item><term></term><description></description></item>
    /// <item><term></term><description></description></item>
    /// <item><term></term><description></description></item>
    /// </list>
    /// </remarks>
	public class PropertyFeel
	{
        public static PropertyFeel Empty = new PropertyFeel(null);

        protected System.Windows.Forms.Control mInPlaceCtrl;

        internal System.Windows.Forms.Control InPlaceCtrl
        {
            get { return mInPlaceCtrl; }
        }

        // Pointer to the parent window
        protected System.Windows.Forms.Control mParentWnd;

        // Pointer to the parent properties control
        protected PropertyGrid mPropCtrl;

        // Identifier used by the properties control as a key to register this feel
        string _name;

        public string Name
        {
            set { _name = value; }
            get { return _name; }
        }

        public PropertyFeel(PropertyGrid grid)
        {
            if (grid != null)
            {
                mPropCtrl = grid;
                mParentWnd = mPropCtrl.Grid;
            }
        }

        public virtual bool DontShowInPlaceCtrl(PropertyEnumerator propEnum)
        {
            return false;
        }

        public virtual System.Windows.Forms.Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if (mInPlaceCtrl != null)
            {
                mInPlaceCtrl.Visible = true;
                mInPlaceCtrl.BackColor = propEnum.Property.Value.BackColor;

                if (propEnum.Property.Value.ReadOnly)
                {
                    if ((mPropCtrl.Enabled == false) &&
                        (mPropCtrl.DisableMode != PropertyGrid.DisableModes.None) &&
                        mPropCtrl.DisableModeGrayedOut)
                        mInPlaceCtrl.ForeColor = mPropCtrl.DisabledForeColor;
                    else
                        mInPlaceCtrl.ForeColor = propEnum.Property.Value.ReadOnlyForeColor;
                }
                else
                    mInPlaceCtrl.ForeColor = propEnum.Property.Value.ForeColor;
            }

            mPropCtrl.OnInPlaceCtrlVisible(new InPlaceCtrlVisibleEventArgs(propEnum, mInPlaceCtrl));

            return mInPlaceCtrl;
        }

        public void UpdateInPlaceControlFromValue(PropertyEnumerator propEnum)
        {
            if (mInPlaceCtrl != null)
            {
                (mInPlaceCtrl as IInPlaceControl).OwnerPropertyEnumerator = propEnum;
                mInPlaceCtrl.Refresh();
            }
        }

        public void UpdateFont()
        {
            if (mInPlaceCtrl != null)
                mInPlaceCtrl.Font = mPropCtrl.Font;
        }

        public virtual void MoveControl(Rectangle valueRect, PropertyEnumerator propEnum)
		{
            if (mInPlaceCtrl == null)
                return;

            valueRect.Height--;

            if (mInPlaceCtrl.Bounds != valueRect)
            {
                mInPlaceCtrl.Invalidate();
                mInPlaceCtrl.Bounds = valueRect;
            }

            (mInPlaceCtrl as IInPlaceControl).RepositionChildren();
        }

        protected void NotifyInPlaceControlCreated(PropertyEnumerator propEnum)
        {
            ((PropertyGrid)mParentWnd.Parent).NotifyInPlaceControlCreated(
                new InPlaceCtrlCreatedEventArgs(propEnum, mInPlaceCtrl));
        }
	}
}
