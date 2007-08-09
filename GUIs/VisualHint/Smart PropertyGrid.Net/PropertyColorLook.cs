// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System.Drawing;
using System;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a color box and the color name or value in the value column of a property.
    /// </summary>
    public class PropertyColorLook : PropertyLook
    {
        Rectangle GetColorRect(Rectangle valueRect)
        {
            Rectangle colorRect = valueRect;

            colorRect.X += Value.Grid.GlobalTextMargin;
            colorRect.Y += 2;
            colorRect.Height -= 6;
            colorRect.Width = 3 * colorRect.Height / 2;

            return colorRect;
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            valueRect.Height++;

            Rectangle colorRect = GetColorRect(valueRect);

            // Draw the color box
            Color color;
            try
            {
                color = (Color)Value.GetActualValue(drawAnotherString);
            }
            catch (Exception)
            {
                color = (Color)Value.GetValue();
            }
            Brush brush = new SolidBrush(color);
            graphics.FillRectangle(brush, colorRect);

            if (propEnum.Property.Value.HasMultipleValues)
                graphics.DrawLine(SystemPens.GrayText, colorRect.Left, colorRect.Bottom, colorRect.Right, colorRect.Top);

            // Draw a black frame around
            Pen pen = new Pen(Color.Black);
            graphics.DrawRectangle(pen, colorRect);
            pen.Dispose();

            Rectangle strRect = GetDisplayStringRect(graphics, valueRect, Point.Empty /* not used */);
            strRect.Width = valueRect.Right - strRect.Left;

            Win32Calls.DrawText(graphics, (drawAnotherString == null ? DisplayString : drawAnotherString),
                ref strRect, Value.Font, textColor,
                Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_END_ELLIPSIS | Win32Calls.DT_NOPREFIX);
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            Rectangle colorRect = GetColorRect(valueRect);
            Rectangle strRect = valueRect;
            strRect.X = colorRect.Right + 3 + Value.Grid.GlobalTextMargin;

            Size size = Win32Calls.GetTextExtent(graphics, DisplayString, Value.Font);

            strRect.Width = size.Width;

            return strRect;
        }
    }
}
