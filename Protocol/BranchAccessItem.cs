using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace SysCAD.Interface
{
  [TypeConverter(typeof(BranchAccessItemConverter))]
  public class BranchAccessItem : BaseAccessItem, ICustomTypeDescriptor
  {
    private BaseAccessItem[] properties;

    public BranchAccessItem(String name, String description, BaseAccessItem[] properties)
    {
      Name = name;
      Description = description;
      this.properties = properties;
    }

    #region "TypeDescriptor Implementation"
    /// <summary>
    /// Get Class Name
    /// </summary>
    /// <returns>String</returns>
    public String GetClassName()
    {
      return TypeDescriptor.GetClassName(this, true);
    }

    /// <summary>
    /// GetAttributes
    /// </summary>
    /// <returns>AttributeCollection</returns>
    public AttributeCollection GetAttributes()
    {
      return TypeDescriptor.GetAttributes(this, true);
    }

    /// <summary>
    /// GetComponentName
    /// </summary>
    /// <returns>String</returns>
    public String GetComponentName()
    {
      return TypeDescriptor.GetComponentName(this, true);
    }

    /// <summary>
    /// GetConverter
    /// </summary>
    /// <returns>TypeConverter</returns>
    public TypeConverter GetConverter()
    {
      return TypeDescriptor.GetConverter(this, true);
    }

    /// <summary>
    /// GetDefaultEvent
    /// </summary>
    /// <returns>EventDescriptor</returns>
    public EventDescriptor GetDefaultEvent()
    {
      return TypeDescriptor.GetDefaultEvent(this, true);
    }

    /// <summary>
    /// GetDefaultProperty
    /// </summary>
    /// <returns>PropertyDescriptor</returns>
    public PropertyDescriptor GetDefaultProperty()
    {
      return TypeDescriptor.GetDefaultProperty(this, true);
    }

    /// <summary>
    /// GetEditor
    /// </summary>
    /// <param name="editorBaseType">editorBaseType</param>
    /// <returns>object</returns>
    public object GetEditor(Type editorBaseType)
    {
      return TypeDescriptor.GetEditor(this, editorBaseType, true);
    }

    public EventDescriptorCollection GetEvents(Attribute[] attributes)
    {
      return TypeDescriptor.GetEvents(this, attributes, true);
    }

    public EventDescriptorCollection GetEvents()
    {
      return TypeDescriptor.GetEvents(this, true);
    }

    public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
    {
      PropertyDescriptor[] newProps = new PropertyDescriptor[properties.Length];
      for (int i = 0; i < properties.Length; i++)
      {
        BranchAccessItemProperty prop = new BranchAccessItemProperty(properties[i].Name, properties[i], false, true);
        newProps[i] = new BranchAccessItemPropertyDescriptor(ref prop, attributes);
      }

      return new PropertyDescriptorCollection(newProps);
    }

    public PropertyDescriptorCollection GetProperties()
    {

      return TypeDescriptor.GetProperties(this, true);

    }

    public object GetPropertyOwner(PropertyDescriptor pd)
    {
      return this;
    }
    #endregion
  }
}
