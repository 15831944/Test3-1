/// <summary>
/// Custom PropertyDescriptor
/// </summary>
using System.ComponentModel;
using System;

namespace SysCAD.Interface
{
  public class BranchAccessItemPropertyDescriptor : PropertyDescriptor
  {
    BranchAccessItemProperty m_Property;
    public BranchAccessItemPropertyDescriptor(ref BranchAccessItemProperty myProperty, Attribute[] attrs)
      : base(myProperty.Name, attrs)
    {
      m_Property = myProperty;
    }

    #region PropertyDescriptor specific

    public override bool CanResetValue(object component)
    {
      return false;
    }

    public override Type ComponentType
    {
      get
      {
        return null;
      }
    }

    public override object GetValue(object component)
    {
      return m_Property.Value;
    }

    public override string Description
    {
      get
      {
        return m_Property.Name;
      }
    }

    public override string Category
    {
      get
      {
        return string.Empty;
      }
    }

    public override string DisplayName
    {
      get
      {
        return m_Property.Name;
      }

    }



    public override bool IsReadOnly
    {
      get
      {
        return m_Property.ReadOnly;
      }
    }

    public override void ResetValue(object component)
    {
      //Have to implement
    }

    public override bool ShouldSerializeValue(object component)
    {
      return false;
    }

    public override void SetValue(object component, object value)
    {
      m_Property.Value = value;
    }

    public override Type PropertyType
    {
      get { return m_Property.Value.GetType(); }
    }

    #endregion


  }
}