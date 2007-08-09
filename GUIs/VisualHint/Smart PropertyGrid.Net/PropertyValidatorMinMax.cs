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
    /// <summary>
    /// Sample validator class that checks a value against minimum and maximum integers.
    /// </summary>
/*
    public class PropertyValidatorMinMax<T> : PropertyValidatorBase
        where T : IComparable
    {
        private T _min;

        private T _max;

        public PropertyValidatorMinMax(PropertyEnumerator propEnum, T min, T max) : base(propEnum)
        {
            _min = min;
            _max = max;
        }

        public PropertyValidatorMinMax(T min, T max)
        {
            _min = min;
            _max = max;
        }

        public override bool Check(object value, bool modify)
        {
            if (value == null)
                return true;

            if (Enumerator == null)
                throw new NullReferenceException("The constructor of the validator should have been passed an enumerator on the corresponding property.");

            T valueToCheck;
            TypeConverter converter = TypeDescriptor.GetConverter(typeof(T));
            if (converter.CanConvertFrom(value.GetType()))
            {
                PropertyTypeDescriptorContext context = Enumerator.Property.Value.GetTypeDescriptorContext(Enumerator);
                valueToCheck = (T)converter.ConvertFrom(context, Enumerator.Property.Value.CultureInfo, value);
            }
            else
                valueToCheck = (T)value;

            bool result = true;

            if (valueToCheck.CompareTo(Min) == -1)
            {
                if (modify)
                    Enumerator.Property.Value.SetValue(Min);
                result = false;
            }
            else if (valueToCheck.CompareTo(Max) == 1)
            {
                if (modify)
                    Enumerator.Property.Value.SetValue(Max);
                result = false;
            }

            return result;
        }

        public T Min
        {
            set { _min = value; }
            get { return _min; }
        }

        public T Max
        {
            set { _max = value; }
            get { return _max; }
        }

        public override int GetHashCode()
        {
            return _min.GetHashCode() ^ _max.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            PropertyValidatorMinMax<T> validator = obj as PropertyValidatorMinMax<T>;
            if (validator == null)
                return false;

            return (_min.Equals(validator._min) && _max.Equals(validator._max));
        }
    }
 */
    public class PropertyValidatorMinMax : PropertyValidatorBase
    {
        private IComparable _min;

        private IComparable _max;

        public PropertyValidatorMinMax(PropertyEnumerator propEnum, IComparable min, IComparable max)
            : base(propEnum)
        {
            _min = min;
            _max = max;
        }

        public PropertyValidatorMinMax(IComparable min, IComparable max)
        {
            _min = min;
            _max = max;
        }

        public override bool Check(object value, bool modify)
        {
            if (value == null)
                return true;

            if (Enumerator == null)
                throw new NullReferenceException("The constructor of the validator should have been passed an enumerator on the corresponding property.");

            IComparable valueToCheck;
            TypeConverter converter = TypeDescriptor.GetConverter(value.GetType());
            if (converter.CanConvertFrom(value.GetType()))
            {
                PropertyTypeDescriptorContext context = Enumerator.Property.Value.GetTypeDescriptorContext(Enumerator);
                valueToCheck = (IComparable)converter.ConvertFrom(context, Enumerator.Property.Value.CultureInfo, value);
            }
            else
                valueToCheck = (IComparable)value;

            bool result = true;

            if (valueToCheck.CompareTo(Min) == -1)
            {
                if (modify)
                    Enumerator.Property.Value.SetValue(Min);
                result = false;
            }
            else if (valueToCheck.CompareTo(Max) == 1)
            {
                if (modify)
                    Enumerator.Property.Value.SetValue(Max);
                result = false;
            }

            if (result == false)
                Message = "" + valueToCheck + " is not between " + Min + " and " + Max + ".";
            else
                Message = "";

            return result;
        }

        public IComparable Min
        {
            set { _min = value; }
            get { return _min; }
        }

        public IComparable Max
        {
            set { _max = value; }
            get { return _max; }
        }
        /*
        public override int GetHashCode()
        {
            return _min.GetHashCode() ^ _max.GetHashCode();
        }
        
        public override bool Equals(object obj)
        {
            PropertyValidatorMinMax validator = obj as PropertyValidatorMinMax;
            if (validator == null)
                return false;

            return (_min.Equals(validator._min) && _max.Equals(validator._max));
        }*/
    }
}
