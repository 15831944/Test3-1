// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System.ComponentModel;
using System;

namespace VisualHint.SmartPropertyGrid
{
    /// <exclude />
    public class PropertyTypeDescriptorContext : ITypeDescriptorContext
    {
        private PropertyDescriptor _descriptor;

        private object _targetInstance;

        private PropertyEnumerator _propEnum;

        public PropertyEnumerator OwnerPropertyEnumerator
        {
            get { return _propEnum; }
        }

        private PropertyGrid _propertyGrid;

        public PropertyGrid PropertyGrid
        {
            get { return _propertyGrid; }
        }

        public PropertyTypeDescriptorContext(PropertyDescriptor descriptor, object instance,
            PropertyEnumerator propEnum, PropertyGrid propGrid)
        {
            _descriptor = descriptor;
            _targetInstance = instance;
            _propEnum = propEnum;
            _propertyGrid = propGrid;
        }

        public void OnComponentChanged()
        {
        }

        public IContainer Container
        {
            get{return null;}
        }

        public bool OnComponentChanging()
        {
            return true;
        }

        public object Instance
        {
            get { return _targetInstance; }
        }

        public PropertyDescriptor PropertyDescriptor
        {
            get { return _descriptor; }
        }

        public object GetService(System.Type serviceType)
        {
            if ((_propertyGrid != null) && (_propertyGrid is IServiceProvider))
                return (_propertyGrid as IServiceProvider).GetService(serviceType);

            return null;
        }
    }
}
