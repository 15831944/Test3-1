// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Drawing;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a property value as well as a progress bar
    /// </summary>
    public class PropertyProgressBarLook : PropertyLook
    {
        private bool _drawFrame;

        private string _formatStr = "";

        public string Format
        {
            set { _formatStr = value; }
            get { return _formatStr; }
        }

        public PropertyProgressBarLook(bool drawFrame)
        {
            _drawFrame = drawFrame;
        }

        public PropertyProgressBarLook(bool drawFrame, string formatStr)
        {
            _drawFrame = drawFrame;
            _formatStr = formatStr;
        }

        public override void AttachProperty(PropertyEnumerator propEnum)
        {
            propEnum.Property.Value.Feel = null;
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            int margin = Value.Grid.GlobalTextMargin;
            valueRect.Height++;
            valueRect.X += margin;

            if (_formatStr.Length != 0)
            {
                Size size = Win32Calls.GetTextExtent(graphics, DisplayString, Value.Font);
                Rectangle strRect = valueRect;
                strRect.Width = Math.Min(size.Width, valueRect.Width - margin);
                Win32Calls.DrawText(graphics, DisplayString, ref strRect, Value.Font, textColor,
                    Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                    ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

                valueRect.Width = valueRect.Right - (strRect.Right + margin);
                valueRect.X = strRect.Right + margin;
            }

            if (_drawFrame)
                valueRect.Width -= 2 * margin;
            else
                valueRect.Width -= margin - 2;
            valueRect.Y++;
            valueRect.Height -= 3;

            if (valueRect.Width > 2)
            {
                PropertyValidatorMinMax validator = (propEnum.Property.Value.Validator as PropertyValidatorMinMax);
                int min = 0, max = 100;

                if (validator != null)
                {
                    min = (int)validator.Min;
                    max = (int)validator.Max;
                }

                propEnum.Property.ParentGrid.DrawManager.DrawProgressBar(graphics, propEnum.Property.ParentGrid.Grid,
                valueRect, _drawFrame, Int32.Parse(propEnum.Property.Value.GetStringValue()), min, max);
            }
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            valueRect.Inflate(-Value.Grid.GlobalTextMargin, 0);

            if (_formatStr.Length != 0)
            {
                Size size = Win32Calls.GetTextExtent(graphics, DisplayString, Value.Font);
                valueRect.Width = size.Width;

                return valueRect;
            }

            return base.GetDisplayStringRect(graphics, valueRect, point);
        }

        public override string DisplayString
        {
            get
            {
                return string.Format(_formatStr, base.DisplayString);
            }
        }
    }
}
