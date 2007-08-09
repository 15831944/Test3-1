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
    /// Passed to the client application when a property and all its descendants have been created.
    /// </summary>
    public class PropertyCreatedEventArgs : PropertyEventArgs
    {
        public PropertyCreatedEventArgs(PropertyEnumerator propEnum) : base(propEnum)
        {
        }
    }
}
