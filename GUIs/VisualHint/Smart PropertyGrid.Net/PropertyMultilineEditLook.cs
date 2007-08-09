// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a property value on multiple lines.
    /// </summary>
    public class PropertyMultilineEditLook : PropertyLook
    {
        public PropertyMultilineEditLook()
        {
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            Rectangle strRect = GetDisplayStringRect(graphics, valueRect, Point.Empty /* not used */);
            strRect.Y += 2;
            strRect.Height = valueRect.Bottom - 2 - strRect.Y;

            Win32Calls.DrawText(graphics, DisplayString, ref strRect, Value.Font, textColor,
                Win32Calls.DT_WORDBREAK | Win32Calls.DT_EDITCONTROL | Win32Calls.DT_NOPREFIX);
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
	        Rectangle calcRect = valueRect;
            calcRect.Inflate(-Value.Grid.GlobalTextMargin, 0);

            Win32Calls.DrawText(graphics, DisplayString, ref calcRect, Value.Font, Color.Black,
                Win32Calls.DT_EDITCONTROL | Win32Calls.DT_WORDBREAK | Win32Calls.DT_CALCRECT |
                Win32Calls.DT_LEFT | Win32Calls.DT_TOP | Win32Calls.DT_NOPREFIX);

            return calcRect;
        }

        public override Size GetTooltipStringSize(Graphics graphics, Rectangle valueRect, Point point, int line)
        {
	        Rectangle calcRect = valueRect;
            int margin = Value.Grid.GlobalTextMargin;
            calcRect.X += margin;
            calcRect.Width = valueRect.Right - margin - calcRect.X;

            Win32Calls.DrawText(graphics, DisplayString, ref calcRect, Value.Font, Color.Black,
                Win32Calls.DT_EDITCONTROL | Win32Calls.DT_WORDBREAK | Win32Calls.DT_CALCRECT |
                Win32Calls.DT_LEFT | Win32Calls.DT_TOP | Win32Calls.DT_NOPREFIX);

	        return new Size(calcRect.Width, calcRect.Height - 4);
        }
    }
}
