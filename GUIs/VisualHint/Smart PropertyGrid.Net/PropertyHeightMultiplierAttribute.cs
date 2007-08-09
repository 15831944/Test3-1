// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.Text;

namespace VisualHint.SmartPropertyGrid
{
    public class PropertyHeightMultiplierAttribute : Attribute
    {
        private int _heightMultiplier;

        public int HeightMultiplier
        {
            get { return _heightMultiplier; }
        }

        public PropertyHeightMultiplierAttribute(int heightMultiplier)
        {
            _heightMultiplier = heightMultiplier;
        }

        public override bool Equals(object obj)
        {
            PropertyHeightMultiplierAttribute attribute = obj as PropertyHeightMultiplierAttribute;
            if (attribute == null)
                return false;

            return (attribute.HeightMultiplier == _heightMultiplier);
        }

        private int _hashCode = 0;

        public override int GetHashCode()
        {
            if (_hashCode == 0)
                _hashCode = _heightMultiplier.GetHashCode();

            return _heightMultiplier;
        }
    }
}
