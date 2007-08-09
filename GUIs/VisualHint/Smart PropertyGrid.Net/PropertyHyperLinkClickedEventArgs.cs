// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Passed to the client application when it is notified that the text of a HyperLink property has been clicked.
    /// </summary>
    public class PropertyHyperLinkClickedEventArgs : PropertyEventArgs
    {
        private string _hyperlink;

        public string HyperLink
        {
            get { return _hyperlink; }
        }

        public PropertyHyperLinkClickedEventArgs(PropertyEnumerator propEnum, string hyperlink) : base(propEnum)
        {
            _hyperlink = hyperlink;
        }
    }
}
