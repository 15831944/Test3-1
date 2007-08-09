// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

// TODO : il faudrait que ce look soit attache automatiquement a la property Name d'une fonte
// donc il faudrait un genre de register pour que des looks et des feels soient choisis a la creation des
// properties d'une property parente.

#region Using directives

using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a font name with a sample as the Microsoft PropertyGrid does.
    /// </summary>
    public class PropertyFontNameLook : PropertyLook
    {
        private static int fontSampleWidth = 20;

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            string str;
            if (drawAnotherString != null)
                str = drawAnotherString;
            else
                str = Value.DisplayString;

            int margin = Value.Grid.GlobalTextMargin;
            int marginDiv2 = margin / 2;

            float emSize = (float)(((double)valueRect.Height - 2) / 1.2);
            Font font = GetFont(str, emSize);
            if (font != null)
            {
                valueRect.Height++;
                valueRect.X += margin;
                valueRect.Width -= margin;

                string fontSampleStr = GetSampleText(graphics, font);

                Rectangle fontRect = valueRect;
                fontRect.Inflate(marginDiv2, -1);
                fontRect.Width = fontSampleWidth;
                fontRect.Height--;

                graphics.FillRectangle(SystemBrushes.Highlight, fontRect);

                Rectangle textRect = valueRect;
                textRect.X = fontRect.Right + Value.Grid.GlobalTextMargin;
                textRect.Width = valueRect.Right - textRect.Left;

                fontRect.Inflate(-marginDiv2, 0);
                Win32Calls.DrawText(graphics, fontSampleStr, ref fontRect, font, SystemColors.HighlightText,
                    Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE);

                Win32Calls.DrawText(graphics, str, ref textRect, Value.Font, textColor,
                    Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_END_ELLIPSIS);
            }
        }

        private string GetSampleText(Graphics graphics, Font font)
        {
            int marginDiv2 = Value.Grid.GlobalTextMargin / 2;
            Size fontTextSize = Win32Calls.GetTextExtent(graphics, "abcd", font);
            if (fontTextSize.Width <= fontSampleWidth - 2 * marginDiv2)
                return "abcd";

            fontTextSize = Win32Calls.GetTextExtent(graphics, "abc", font);
            if (fontTextSize.Width <= fontSampleWidth - 2 * marginDiv2)
                return "abc";

            fontTextSize = Win32Calls.GetTextExtent(graphics, "ab", font);
            if (fontTextSize.Width <= fontSampleWidth - 2 * marginDiv2)
                return "ab";

            fontTextSize = Win32Calls.GetTextExtent(graphics, "a", font);
            if (fontTextSize.Width <= fontSampleWidth - 2 * marginDiv2)
                return "a";

            return "";
        }

        private Font GetFont(string fontFamily, float emSize)
        {
            FontFamily family = null;
            Font font = null;

            try
            {
                family = new FontFamily(fontFamily);
            }
            catch { }

            if (family != null)
            {
                try
                {
                    font = new Font(family, emSize, FontStyle.Regular, GraphicsUnit.Pixel);
                }
                catch
                {
                    try
                    {
                        font = new Font(family, emSize, FontStyle.Italic, GraphicsUnit.Pixel);
                    }
                    catch
                    {
                        try
                        {
                            font = new Font(family, emSize, FontStyle.Bold, GraphicsUnit.Pixel);
                        }
                        catch
                        {
                            try
                            {
                                font = new Font(family, emSize, FontStyle.Italic | FontStyle.Bold, GraphicsUnit.Pixel);
                            }
                            catch
                            {
                            }
                        }
                    }
                }
            }

            return font;
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            float emSize = (float)(((double)valueRect.Height - 3) / 1.2);
            Font font = GetFont(Value.DisplayString, emSize);

            if (font != null)
            {
                Size textSize = Win32Calls.GetTextExtent(graphics, Value.DisplayString, Value.Font);

                int margin = Value.Grid.GlobalTextMargin;
                int marginDiv2 = margin / 2;
                int offset = 2 * margin - marginDiv2 + fontSampleWidth;
                valueRect.X += offset;
                valueRect.Width = textSize.Width;

                return valueRect;
            }
            else
                return Rectangle.Empty;
        }
    }
}
