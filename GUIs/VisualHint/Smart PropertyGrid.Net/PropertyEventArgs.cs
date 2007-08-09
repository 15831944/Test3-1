// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.Text;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    public class PropertyEventArgs : EventArgs
    {
        private PropertyEnumerator _propertyEnum;

        public PropertyEnumerator PropertyEnum
        {
            get { return _propertyEnum; }
        }

        internal PropertyEventArgs(PropertyEnumerator propEnum)
        {
            _propertyEnum = (PropertyEnumerator)propEnum.Clone();
        }
    }
}
