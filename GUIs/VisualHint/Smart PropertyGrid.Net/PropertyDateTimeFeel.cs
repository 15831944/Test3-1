// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Windows.Forms;
using System.Drawing;

namespace VisualHint.SmartPropertyGrid
{
    internal class PropertyDateTimeFeel : PropertyFeel
    {
        public PropertyDateTimeFeel(PropertyGrid grid) : base(grid)
        {
        }

        public override Control ShowControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if (propEnum.Property.Value.ReadOnly)
            {
                PropInPlaceTextbox edit;

                if ((mInPlaceCtrl == null) || !(mInPlaceCtrl is PropInPlaceTextbox))
                {
                    edit = new PropInPlaceTextbox(false);
                    edit.Visible = false;
                    edit.Parent = mParentWnd;
                    mInPlaceCtrl = edit;
                }
                else
                    edit = (PropInPlaceTextbox)mInPlaceCtrl;

                NotifyInPlaceControlCreated(propEnum);

                edit.OwnerPropertyEnumerator = propEnum;
                edit.Font = propEnum.Property.Value.Font;
//                if (edit.Text != propEnum.Property.Value.DisplayString)
  //                  edit.Text = propEnum.Property.Value.DisplayString;
            }
            else
            {
                PropInPlaceDateTime dateTime;

                if ((mInPlaceCtrl == null) || !(mInPlaceCtrl is PropInPlaceDateTime))
                {
                    dateTime = new PropInPlaceDateTime();
                    dateTime.Visible = false;
                    dateTime.Parent = mParentWnd;
#if _DOTNET2
                    dateTime.AutoSize = false;
#endif
                    mInPlaceCtrl = dateTime;
                }
                else
                    dateTime = (PropInPlaceDateTime)mInPlaceCtrl;

                dateTime.OwnerPropertyEnumerator = propEnum;
                dateTime.Font = propEnum.Property.Value.Font;
//                dateTime.Value = (DateTime)propEnum.Property.Value.GetValue();
            }

            MoveControl(valueRect, propEnum);

            return base.ShowControl(valueRect, propEnum);
        }

        public override void MoveControl(Rectangle valueRect, PropertyEnumerator propEnum)
        {
            if (mInPlaceCtrl == null)
                return;

            if (mInPlaceCtrl is PropInPlaceDateTime)
            {
                valueRect.Height--;

                Win32Calls.RECT rect = new Win32Calls.RECT();
                Win32Calls.GetWindowRect(mInPlaceCtrl.Handle, ref rect);
                Rectangle bounds = Rectangle.FromLTRB(rect.Left, rect.Top, rect.Right, rect.Bottom);
                bounds = mInPlaceCtrl.Parent.RectangleToClient(bounds);

                if (bounds != valueRect)
                {
                    mInPlaceCtrl.Invalidate();

                    // Bounds doesn't work because of DateTimePicker.PreferredHeight
                    Win32Calls.SetWindowPos(mInPlaceCtrl.Handle, IntPtr.Zero, valueRect.Left, valueRect.Top,
                        valueRect.Width, valueRect.Height, Win32Calls.SWP_NOZORDER | Win32Calls.SWP_NOACTIVATE |
                        Win32Calls.SWP_NOREDRAW);

                    // Causes WM_NCPAINT
                    Win32Calls.RedrawWindow(mInPlaceCtrl.Handle, IntPtr.Zero, IntPtr.Zero, 0x0561);
                }
            }
            else
            {
                // Find where the string is actually displayed (a look instance could place it anywhere)
                Graphics graphics = mParentWnd.CreateGraphics();
                Rectangle strRect = propEnum.Property.Value.GetStringValueRect(graphics, valueRect, Point.Empty /* not used */);
                strRect.X -= (propEnum.Property.Value.EditboxLeftMargin == -1 ? propEnum.Property.ParentGrid.GlobalTextMargin :
                    propEnum.Property.Value.EditboxLeftMargin);
                strRect.Width = valueRect.Right - strRect.Left;

                // Calculate the height of the editbox, based on the font height

                Rectangle editRect = strRect;
                editRect.Inflate(0, -1);

                Win32Calls.TEXTMETRIC tm = new Win32Calls.TEXTMETRIC();
                Win32Calls.GetTextMetrics(graphics, mInPlaceCtrl.Font, ref tm);

                graphics.Dispose();

                int extraHeight = editRect.Height - tm.tmHeight;
                editRect.Y += extraHeight / 2;
                editRect.Height -= extraHeight;

                if (mInPlaceCtrl.Bounds != editRect)
                    mInPlaceCtrl.Bounds = editRect;
            }
        }
    }
}
