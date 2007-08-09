// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

using System;
using System.ComponentModel;

namespace VisualHint.SmartPropertyGrid
{
    public class PropertyPreFilterOutEventArgs : EventArgs
    {
        public enum FilterModes
        {
            FilterDefault,
            FilterOut,
            DontFilter
        }

        private FilterModes _filterOut = FilterModes.FilterDefault;

        /// <summary>
        /// Set this property to true to filter out the property and all its descendants.
        /// </summary>
        public FilterModes FilterOut
        {
            set { _filterOut = value; }
        }

        internal FilterModes GetFilterOutInternal()
        {
            return _filterOut;
        }

        private PropertyDescriptor _propertyDescriptor;

        public PropertyDescriptor PropertyDescriptor
        {
            get { return _propertyDescriptor; }
        }

        private object _targetInstance;

        public object TargetInstance
        {
            get { return _targetInstance; }
        }

        public PropertyPreFilterOutEventArgs(PropertyDescriptor propertyDescriptor, object targetInstance)
        {
            _propertyDescriptor = propertyDescriptor;
            _targetInstance = targetInstance;
        }
    }
}
