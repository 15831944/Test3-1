// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.ComponentModel;
using System.Drawing;
using System.Security.Permissions;
using System.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Represents the default control that is displayed in the listbox part of a combobox.
    /// </summary>
    /// <remarks>
    /// It is used for enumeration, string and boolean types. If you want to use this class for another
    /// type, call <see cref="PropertyGrid.RegisterDropDownContent"/>.
    /// </remarks>
    /// <example>
    /// <code>
    /// RegisterDropDownContent(typeof(MyType), typeof(DropDownContentListBox));
    /// </code>
    /// </example>
    [ToolboxItem(false)]
    public class DropDownContentListBox : ListBox, IDropDownContent
    {
        private Control _wmFocusMissingArg = null;

        private PropertyEnumerator _ownerPropertyEnumerator;

        private PropertyValue _propValue;

        public DropDownContentListBox()
        {
            IntegralHeight = false;
            DrawMode = DrawMode.OwnerDrawFixed;
            DrawItem += new DrawItemEventHandler(OnDrawItem);
            DisplayMember = "Name";
        }

        void OnDrawItem(object sender, DrawItemEventArgs e)
        {
            // If the index is invalid then simply exit.
            if (e.Index == -1 || e.Index >= Items.Count)
                return;

            // Draw the background of the item.
            e.DrawBackground();

            // Should we draw the focus rectangle?
            if ((e.State & DrawItemState.Focus) != 0)
                e.DrawFocusRectangle();

            Color textColor = ((e.State & DrawItemState.Selected) != 0 ? SystemColors.HighlightText : ForeColor);
            int currentImageListIndex = _propValue.ImageIndex;
            _propValue.ImageIndex = e.Index;
            _propValue.DrawValue(e.Graphics, e.Bounds, textColor, _ownerPropertyEnumerator,
                Items[e.Index] as string);
            _propValue.ImageIndex = currentImageListIndex;
        }

        public int GetSelectedIndex()
        {
            return SelectedIndex;
        }

        public void InitializeContent(PropertyEnumerator propEnum, string currentValue, object valueKey)
        {
            _ownerPropertyEnumerator = propEnum;

            int width = 0;

            _propValue = (valueKey == null ? propEnum.Property.Value : propEnum.Property.GetValue(valueKey));

            Graphics graphics = CreateGraphics();

            object[] displayedValues = _propValue.GetDisplayedValues();
            foreach(object str in displayedValues)
            {
                Items.Add(str.ToString());

                Size size = Win32Calls.GetTextExtent(graphics, str.ToString(), propEnum.Property.ParentGrid.Font);
                if (width < size.Width)
                    width = size.Width;
            }

            Win32Calls.TEXTMETRIC tm = new Win32Calls.TEXTMETRIC();
            Win32Calls.GetTextMetrics(graphics, propEnum.Property.ParentGrid.Font, ref tm);

            graphics.Dispose();

            ItemHeight = tm.tmHeight + propEnum.Property.ParentGrid.PropertyVerticalMargin - 2;

            Height = Math.Max(ItemHeight, Math.Min(200, ItemHeight * displayedValues.Length));

            int margin = propEnum.Property.ParentGrid.GlobalTextMargin;
            Width = width + 2 * margin;
            if (_propValue.ImageList != null)
                Width += ItemHeight + margin;

            DrawMode = DrawMode.OwnerDrawFixed;

            SelectedItem = currentValue;
//            SelectedItem = _propValue.DisplayString;
        }

        public bool HasToBeClosedOnClick()
        {
            return true;
        }

        protected override CreateParams CreateParams
        {
            [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            get
            {
                CreateParams cp = base.CreateParams;
                cp.Style &= ~Win32Calls.WS_BORDER;
                cp.ExStyle &= ~Win32Calls.WS_EX_CLIENTEDGE;
                return cp;
            }
        }

        public object GetValue()
        {
            return SelectedItem;
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_SETFOCUS)
                _wmFocusMissingArg = FromHandle(m.WParam);
            else if (m.Msg == Win32Calls.WM_KILLFOCUS)
                _wmFocusMissingArg = FromHandle(m.WParam);

            base.WndProc(ref m);
        }
    }
}
