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
    /// Specific class for properties that have no value and act as a non first level container for other properties
    /// and subcategories.
    /// </summary>
    public class SubCategory : RootCategory
    {
        public SubCategory(int propertyId, string name) : base (propertyId, name)
        {
        }

        public override void Draw(Graphics graphics, Rectangle itemRect, PropertyVisibleDeepEnumerator enumSelf)
        {
	        Rectangle rect = itemRect;
            rect.X += ParentGrid.LeftColumnWidth + 1;
            rect.Width -= ParentGrid.LeftColumnWidth + 1;

            // Draw a background
	        //------------------

            Rectangle labelRect = GetLabelTextRect(itemRect, enumSelf);
            ParentGrid.DrawManager.DrawSubCategoryLabelBackground(graphics, labelRect.Left, rect, enumSelf);

	        // Draw the vertical separation between the columns and the bottom horizontal line
	        //--------------------------------------------------------------------------------

            Rectangle labelColumnRect = GetLabelColumnRect(itemRect, enumSelf);
            ParentGrid.DrawManager.DrawSeparationLines(graphics, itemRect, labelColumnRect, enumSelf);

	        // Choose the colors of the text regarding the item selection
   	        Color textColor;
	        if (Selected)
	        {
                if (ParentGrid.GridContainsFocus)
                    textColor = ParentGrid.HighlightedTextColor;
		        else
			        textColor = ForeColor;
	        }
	        else	// if not selected
		        textColor = ForeColor;

	        // Draw the image if any
	        //----------------------

            Win32Calls.SetClippingRect(graphics, labelColumnRect);

            Rectangle imgRect = GetImageRect(itemRect, enumSelf);
            if (imgRect != Rectangle.Empty)
                ParentGrid.DrawImage(ImageIndex, graphics, imgRect.Left, imgRect.Top);

            // Draw the category label
            //------------------------
            
            ParentGrid.DrawManager.DrawSubCategoryLabelText(graphics, labelRect, textColor, enumSelf);

	        // Draw the +/- sign
	        //------------------

            if (ParentGrid.HasOneVisibleChild(enumSelf))
                ParentGrid.DrawManager.DrawPlusMinusSign(graphics, ParentGrid.Grid, itemRect, enumSelf);

	        // Draw checkbox used to be able to manually disable/enable this category
	        //-----------------------------------------------------------------------

            if (CanBeDisabledManually)
            {
                Rectangle frameRect = GetManuallyDisableRect(itemRect, enumSelf);
                ParentGrid.DrawManager.DrawCheckBox(graphics, ParentGrid.Grid, frameRect, GetManuallyDisabledVariable(),
                    !ParentGrid.Grid.IsAncestorDisabled(enumSelf));
            }

            Win32Calls.ResetClippingRect(graphics);

            // Draw value text if any
            //-----------------------

            if (ValueText.Length > 0)
            {
                Rectangle valueRect = itemRect;
                int delta = ParentGrid.LeftColumnWidth + 1 + ParentGrid.LabelColumnWidth;
                valueRect.X += delta;
                valueRect.Width -= delta;
                valueRect.Inflate(-ParentGrid.GlobalTextMargin, 0);

                ParentGrid.DrawManager.DrawCategoryValue(graphics, valueRect, textColor, enumSelf);
            }
        }

        public override Rectangle GetImageRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            if (ImageIndex == -1)
                return Rectangle.Empty;

            Rectangle imgRect = itemRect;

            Rectangle checkboxRect = GetManuallyDisableRect(itemRect, enumSelf);
            if (checkboxRect != Rectangle.Empty)
                imgRect.X = checkboxRect.Right + ParentGrid.GlobalTextMargin;
            else
            {
                Rectangle signRect = GetSignRect(itemRect, enumSelf);
                if (signRect != Rectangle.Empty)
                    imgRect.X = signRect.Right + ParentGrid.GlobalTextMargin;
                else
                {
                    imgRect.X = ParentGrid.LeftColumnWidth + 1 + ParentGrid.GlobalTextMargin +
                        (enumSelf.Depth - 1) * (ParentGrid.GlobalTextMargin + TreeGlyphWidth);
                }
            }

            try
            {
                Image img = ParentGrid.ImageList.Images[ImageIndex];
                imgRect.Width = img.Width;
                imgRect.Height = img.Height;
            }
            catch (Exception)
            {
                return Rectangle.Empty;
            }

            imgRect.Y = itemRect.Top + itemRect.Height / 2 - 8;

            return imgRect;
        }

        public override Rectangle GetSignRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            if (ParentGrid.HasOneVisibleChild(enumSelf) == false)
                return Rectangle.Empty;

            Rectangle signRect = itemRect;

            // Default rectangle in the left most column for root categories
            signRect.Y += itemRect.Height / 2 - (TreeGlyphWidth / 2);
            signRect.Height = TreeGlyphWidth;

            signRect.X = ParentGrid.LeftColumnWidth + 1 + (enumSelf.Depth - 1) * (ParentGrid.GlobalTextMargin + TreeGlyphWidth) + ParentGrid.GlobalTextMargin;
            signRect.Width = TreeGlyphWidth;

            return signRect;
        }

        public override Rectangle GetManuallyDisableRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            if (CanBeDisabledManually == false)
                return Rectangle.Empty;

            Rectangle frameRect = itemRect;
            Rectangle signRect = GetSignRect(itemRect, enumSelf);
            if (signRect != Rectangle.Empty)
                frameRect.X = ParentGrid.GlobalTextMargin + signRect.Right;
            else
            {
                frameRect.X = ParentGrid.GlobalTextMargin + ParentGrid.LeftColumnWidth + 1 +
                    (enumSelf.Depth - 1) * (ParentGrid.GlobalTextMargin + TreeGlyphWidth);
            }

            // Size it based on the row height
            frameRect.Y += 2;
            frameRect.Height = ParentGrid.BasicPropertyHeight - 4;
            frameRect.Width = frameRect.Height;

            return frameRect;
        }

        public override Rectangle GetLabelTextRect(Rectangle itemRect, PropertyEnumerator enumSelf)
        {
            Rectangle labelRect = itemRect;

            Rectangle imgRect = GetImageRect(itemRect, enumSelf);
            if (imgRect != Rectangle.Empty)
            {
                labelRect.X = imgRect.Right;
            }
            else
            {
                Rectangle checkboxRect = GetManuallyDisableRect(itemRect, enumSelf);
                if (checkboxRect != Rectangle.Empty)
                    labelRect.X = checkboxRect.Right;
                else
                {
                    Rectangle signRect = GetSignRect(itemRect, enumSelf);
                    if (signRect != Rectangle.Empty)
                        labelRect.X = signRect.Right;
                    else
                    {
                        labelRect.X = ParentGrid.LeftColumnWidth + 1 + (enumSelf.Depth - 1) *
                            (ParentGrid.GlobalTextMargin + TreeGlyphWidth);
                    }
                }
            }

            if ((ValueText.Length > 0) || ParentGrid.DrawManager.ShowCategoryVerticalLine)
                labelRect.Width = ParentGrid.Grid.LeftColumnWidth + 1 + ParentGrid.LabelColumnWidth - labelRect.Left;

            if ((ParentGrid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0)
                labelRect.Inflate(-ParentGrid.GlobalTextMargin, 0);
            else
            {
                labelRect.X += ParentGrid.GlobalTextMargin;
                labelRect.Width -= ParentGrid.GlobalTextMargin + 1;
            }

            return labelRect;
        }
    }
}
