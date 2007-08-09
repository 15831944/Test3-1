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
    /// Ensures at compile-time that a property will have a specific identifier.
    /// </summary>
    public class PropertyIdAttribute : Attribute
    {
        private int _id;

        public int Id
        {
            get { return _id; }
        }

        public PropertyIdAttribute(int id)
        {
            _id = id;
        }
    }
}
