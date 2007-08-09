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
    /// <summary>
    /// Special property that contains only a text displayed as an hyperlink.
    /// </summary>
    public class PropertyHyperLink : Property
    {
        public PropertyHyperLink(int propertyId) : base(propertyId, "")
        {
            ForeColor = Color.Blue;
        }

        internal override void OnPropertyClicked(PropertyEnumerator enumSelf, Point point, Rectangle itemRect)
        {
            if ((ParentGrid.Enabled == false) && (ParentGrid.DisableMode != PropertyGrid.DisableModes.None))
                return;

            Rectangle labelRect = GetLabelTextRect(itemRect, enumSelf);

            if (labelRect.Contains(point))
            {
                PropertyHyperLinkClickedEventArgs ev = new PropertyHyperLinkClickedEventArgs(enumSelf, HyperLink);
                ParentGrid.OnHyperLinkPropertyClicked(ev);
            }
        }

        public override Rectangle GetLabelColumnRect(Rectangle itemRect, PropertyEnumerator enumerator)
        {
            Rectangle labelRect = itemRect;
            labelRect.X = ParentGrid.LeftColumnWidth + 1;
            labelRect.Width = itemRect.Right - labelRect.Left;

            return labelRect;
        }

        public override Rectangle GetLabelTextRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            Font underlinedFont = new Font(Font, FontStyle.Underline);
            Graphics graphics = ParentGrid.CreateGraphics();

            Size textSize = Win32Calls.GetTextExtent(graphics, DisplayName, underlinedFont);

            Rectangle labelColumnRect = GetLabelColumnRect(itemRect, enumSelf);
            labelColumnRect.Inflate(-ParentGrid.GlobalTextMargin, 0);
            Rectangle labelRect = labelColumnRect;

            if (textSize.Width <= labelColumnRect.Width)
            {
                labelRect.X = labelColumnRect.Right - textSize.Width;
                labelRect.Width = textSize.Width;
            }
            else
                labelRect.Width = labelColumnRect.Right - labelRect.Left;

            graphics.Dispose();
            underlinedFont.Dispose();

            return labelRect;
        }

        public override bool OverrideCursor(Point mouseLocation, Rectangle itemRect, PropertyEnumerator propEnum)
        {
            if ((ParentGrid.Enabled == false) && (ParentGrid.DisableMode != PropertyGrid.DisableModes.None))
                return false;

            Rectangle labelRect = GetLabelTextRect(itemRect, propEnum);

            if (labelRect.Contains(mouseLocation) && Enabled)
            {
                ParentGrid.Grid.Cursor = Cursors.Hand;
                return true;
	        }

	        return false;
        }

        public override void Draw(Graphics graphics, Rectangle itemRect, PropertyVisibleDeepEnumerator enumSelf)
        {
            // Fill the label background
            Rectangle labelColumnRect = GetLabelColumnRect(itemRect, enumSelf);
            ParentGrid.DrawManager.DrawPropertyLabelBackground(graphics, labelColumnRect, enumSelf);

            ParentGrid.DrawManager.DrawSeparationLines(graphics, itemRect, labelColumnRect, enumSelf);

	        // Draw label text
	        //----------------

            // Set the font to underline
            Font underlinedFont = new Font(Font, FontStyle.Underline);

            Color textColor;

            // Choose the colors of the background and text regarding the item selection
            if (Selected)
            {
                if (ParentGrid.GridContainsFocus)
                    textColor = SystemColors.HighlightText;
                else
                    textColor = SystemColors.WindowText;
            }
            else										// if not selected
            {
                if (!ParentGrid.Enabled && (ParentGrid.DisableMode != PropertyGrid.DisableModes.None) &&
                    ParentGrid.DisableModeGrayedOut)
                {
                    textColor = ParentGrid.DisabledForeColor;
                }
                else
                {
                    if (Enabled == false)
                        textColor = DisabledForeColor;
                    else
                        textColor = ForeColor;
                }
            }

            labelColumnRect.X += ParentGrid.GlobalTextMargin;
            labelColumnRect.Width -= 2 * ParentGrid.GlobalTextMargin;
            labelColumnRect.Height--;

        	// Draw the text, clipped by the given area
            Size textSize = Win32Calls.GetTextExtent(graphics, DisplayName, underlinedFont);

            if (textSize.Width <= labelColumnRect.Width)
                Win32Calls.DrawText(graphics, DisplayName, ref labelColumnRect, underlinedFont, textColor,
                    Win32Calls.DT_RIGHT | Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX);
            else
                Win32Calls.DrawText(graphics, DisplayName, ref labelColumnRect, underlinedFont, textColor,
                    Win32Calls.DT_LEFT | Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_END_ELLIPSIS |
                    Win32Calls.DT_NOPREFIX);

            underlinedFont.Dispose();
        }
    }
}
