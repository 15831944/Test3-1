// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System.Drawing;

namespace VisualHint.SmartPropertyGrid
{
	/// <summary>
    /// By assigning a look to a property, it defines the way this property will be displayed.
	/// </summary>
	public class PropertyLook
	{
        protected bool mNoFeelForReadOnly = false;

        public bool NoFeelForReadOnly
        {
            get { return mNoFeelForReadOnly; }
        }

        private PropertyValue _value;

        public PropertyValue Value
        {
            set { _value = value; }
            get { return _value; }
        }

        public virtual void AttachProperty(PropertyEnumerator propEnum)
        {
        }

        public virtual void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            Value.OnDraw(graphics, valueRect, textColor, propEnum, drawAnotherString);
        }

        public virtual Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            return new Rectangle(valueRect.Location, Size.Empty);
        }

        public virtual string DisplayString
        {
            get
            {
        	    // By default a look returns the string stored by the value object
                return _value.GetDisplayString(_value.GetStringValue());
            }
        }

        public virtual string TransformDisplayString(string str)
        {
	        return str;
        }

        public virtual string GetTooltipString(int line)
        {
            return DisplayString;
        }

        public virtual Size GetTooltipStringSize(Graphics graphics, Rectangle valueRect, Point point, int line)
        {
            return Win32Calls.GetTextExtent(graphics, DisplayString, _value.Font);
        }

        public virtual void ControlHeightMultiplier()
        {
        }
    }
}
