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
    /// Look class used to display a property value as a password with bullets.
    /// </summary>
    public class PropertyPasswordLook : PropertyLook
    {
        public override string DisplayString
        {
            get
            {
                string str = Value.GetStringValue();
                string dispStr = "";

                for (int i = 0; i < str.Length; i++)
                    dispStr += Value.Grid.DrawManager.PasswordChar;

                return dispStr;
            }
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            valueRect.Height++;

            Rectangle strRect = GetDisplayStringRect(graphics, valueRect, Point.Empty /* not used */);

            strRect.Y += (Value.Grid.PropertyVerticalMargin % 2 == 0 ? 1 : 0);
            strRect.X -= Value.Grid.GlobalTextMargin - 3;
            strRect.Width = valueRect.Right - strRect.Left;

            StringFormat stringFormat = (StringFormat)StringFormat.GenericDefault.Clone();
            stringFormat.FormatFlags = StringFormatFlags.NoWrap;
            stringFormat.LineAlignment = StringAlignment.Center;
            graphics.DrawString(DisplayString, Value.Font, new SolidBrush(textColor), strRect, stringFormat);
        }
        
        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            Size size = Size.Round(graphics.MeasureString(DisplayString, Value.Font));
            Rectangle rect = valueRect;
            rect.X += Value.Grid.GlobalTextMargin;
            rect.Width = size.Width;

            return rect;
        }
    }
}
