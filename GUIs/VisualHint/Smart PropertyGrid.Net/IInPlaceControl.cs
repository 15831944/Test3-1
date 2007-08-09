// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Windows.Forms;
using System;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    public enum ScrollValueOnDoubleClickSource
    {
        FromLabel,
        FromTextBox,
        FromInPlaceCtrl
    }

    /// <summary>
    /// Interface for classes that act as inplace controls in the value column of a property.
    /// </summary>
    public interface IInPlaceControl
    {
        void OnF4();

        bool RealtimeChange
        {
            get;
            set;
        }

        bool OnParentKeyDown(KeyEventArgs e);

        void RepositionChildren();

        PropertyEnumerator OwnerPropertyEnumerator
        {
            get;
            set;
        }

        bool GoToNextPropertyOnTab(bool forward);

        bool ScrollValueOnDoubleClick(bool forward, ScrollValueOnDoubleClickSource source, bool force);

        TextBox TextBox
        {
            get;
        }

        PropertyValue.ValueValidationResult CommitChanges(bool final);

        Control GetDropDownContainer();
    }
}
