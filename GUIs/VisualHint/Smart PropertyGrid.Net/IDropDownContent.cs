// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Interface for classes that act as controls used in the dropdown area of a combobox inplace control.
    /// </summary>
    public interface IDropDownContent
    {
        void InitializeContent(PropertyEnumerator propEnum, string currentValue, object valueKey);
        bool HasToBeClosedOnClick();
        object GetValue();
        int GetSelectedIndex();
    }
}
