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
    /// Look class used to display multiple choices as radio buttons.
    /// </summary>
    public class PropertyRadioButtonLook : PropertyLook
    {
        public PropertyRadioButtonLook()
        {
            mNoFeelForReadOnly = true;
        }

        public override void ControlHeightMultiplier()
        {
            Value.OwnerEnumerator.Property.HeightMultiplier = Value.GetDisplayedValues().Length;
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            valueRect.Height++;
            int margin = Value.Grid.GlobalTextMargin;
            valueRect.X += margin - 1;
            valueRect.Width -= margin - 1;

            string[] displayedStrings = Value.GetDisplayedValues();

            if (displayedStrings.Length > 0)
            {
                Rectangle oneLineRect = valueRect;
                oneLineRect.Height = valueRect.Height / displayedStrings.Length;
                for (int i = 0; i < displayedStrings.Length; i++)
                {
                    string str = displayedStrings[i];

                    Rectangle radioRect = oneLineRect;

                    radioRect.Height -= 4;
                    radioRect.Y += 2;
                    radioRect.Width = radioRect.Height;

                    // Draw radio button
                    propEnum.Property.ParentGrid.DrawManager.DrawRadioButton(graphics,
                        propEnum.Property.ParentGrid.Grid, radioRect, (propEnum.Property.Value.DisplayString == str),
                        propEnum.Property.Enabled);

                    Rectangle textRect = oneLineRect;
                    textRect.X = radioRect.Right + Value.Grid.GlobalTextMargin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    Win32Calls.DrawText(graphics, str, ref textRect, Value.Font, textColor,
                        Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                        ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

                    oneLineRect.Offset(0, oneLineRect.Height);
                }
            }
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            int margin = Value.Grid.GlobalTextMargin;
            valueRect.X += margin - 1;
            valueRect.Width -= margin - 1;

            Rectangle calcRect = Rectangle.Empty;
            string[] displayedStrings = Value.GetDisplayedValues();

            if (displayedStrings.Length > 0)
            {
                Rectangle oneLineRect = valueRect;
                oneLineRect.Height = valueRect.Height / displayedStrings.Length;
                for (int i = 0; i < displayedStrings.Length; i++)
                {
                    string str = displayedStrings[i];

                    // Radio button rectangle
                    Rectangle radioRect = oneLineRect;
                    radioRect.Height -= 4;
                    radioRect.Y += 2;
                    radioRect.Width = radioRect.Height;

                    // Text rectangle
                    Rectangle textRect = oneLineRect;
                    textRect.X = radioRect.Right + margin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    calcRect = textRect;
                    Win32Calls.DrawText(graphics, str, ref calcRect, Value.Font,
                        Color.Black, Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_CALCRECT |
                        Win32Calls.DT_NOPREFIX);

                    if (calcRect.Contains(point))
                        break;

                    oneLineRect.Offset(0, oneLineRect.Height);
                }

                calcRect.Y = oneLineRect.Top;
                calcRect.Height = oneLineRect.Bottom - calcRect.Y;
            }

            return calcRect;
        }

        public override string GetTooltipString(int line)
        {
            string[] displayedStrings = Value.GetDisplayedValues();
            if (line < displayedStrings.Length)
                return displayedStrings[line];

            return "";
        }

        public override Size GetTooltipStringSize(Graphics graphics, Rectangle valueRect, Point point, int line)
        {
            return Win32Calls.GetTextExtent(graphics, GetTooltipString(line), Value.Font);
        }
    }
}
