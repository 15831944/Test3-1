// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Drawing;
using System.ComponentModel;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a masked text.
    /// </summary>
    /// <remarks>
    /// The underlying value does not contain the mask. This look class ensures that the value is displayed with its
    /// mask.
    /// It must be used in conjunction with <see cref="PropertyMaskedEditFeel"/>.
    /// </remarks>
    public class PropertyMaskedEditLook : PropertyLook
    {
        private MaskedTextProvider _maskedTextProvider;

        public PropertyMaskedEditLook(string mask)
        {
            _maskedTextProvider = new MaskedTextProvider(mask);
            _maskedTextProvider.IncludePrompt = true;
        }

        public PropertyMaskedEditLook(string mask, char promptChar)
        {
            _maskedTextProvider = new MaskedTextProvider(mask);
            _maskedTextProvider.IncludePrompt = true;
            _maskedTextProvider.PromptChar = promptChar;
        }

        public string Mask
        {
            set
            {
                _maskedTextProvider = new MaskedTextProvider(value);
                _maskedTextProvider.IncludePrompt = true;
            }

            get { return _maskedTextProvider.Mask; }
        }

        public char PromptChar
        {
            set { _maskedTextProvider.PromptChar = value; }
            get { return _maskedTextProvider.PromptChar; }
        }

        public override string DisplayString
        {
            get
            {
                _maskedTextProvider.Set(Value.GetStringValue());
                return _maskedTextProvider.ToString(true);
            }
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            valueRect.Height++;

            Rectangle strRect = GetDisplayStringRect(graphics, valueRect, Point.Empty /* not used */);
            strRect.Width = valueRect.Right - strRect.Left;

            Win32Calls.DrawText(graphics, (drawAnotherString == null ? DisplayString : drawAnotherString),
                ref strRect, Value.Font, textColor,
                Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            Size size = Win32Calls.GetTextExtent(graphics, DisplayString, Value.Font);

            Rectangle rect = valueRect;
            rect.X += Value.Grid.GlobalTextMargin;
            rect.Width = size.Width;

            return rect;
        }
    }
}
