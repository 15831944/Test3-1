// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a color with an alpha component in the value column of a property.
    /// </summary>
    public class PropertyAlphaColorLook : PropertyLook
    {
        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            if (propEnum.Property.Value.HasMultipleValues)
                return;

            Rectangle colorRect = valueRect;
            colorRect.Y += 4;
            colorRect.Height -= 9;
            int margin = propEnum.Property.ParentGrid.GlobalTextMargin;
            colorRect.X += margin;
            colorRect.Width -= 2 * margin;

            Brush whiteBrush = new SolidBrush(Color.White);
            Brush grayBrush = new SolidBrush(Color.Gray);
            for (int y = colorRect.Top + 1; y < colorRect.Bottom; y += 4)
            {
                int height = (y + 4 > colorRect.Bottom ? colorRect.Bottom - y : 4);
                for (int x = colorRect.Left + 1; x < colorRect.Right; x += 5)
                {
                    int width = (x + 5 > colorRect.Right ? colorRect.Right - x : 5);
                    if (((x - (colorRect.Left + 1)) % 10 == ((y - (colorRect.Top + 1)) % 8 == 0 ? 0 : 5)))
                        graphics.FillRectangle(whiteBrush, x, y, width, height);
                    else
                        graphics.FillRectangle(grayBrush, x, y, width, height);
                }
            }

            whiteBrush.Dispose();
            grayBrush.Dispose();

            // Draw the color with alpha
            Brush brush = new SolidBrush((Color)Value.GetActualValue(drawAnotherString));
            graphics.FillRectangle(brush, colorRect);
            brush.Dispose();

            // Draw a black frame around
            Pen blackPen = new Pen(Color.Black);
            graphics.DrawRectangle(blackPen, colorRect);
            blackPen.Dispose();
        }
    }
}
