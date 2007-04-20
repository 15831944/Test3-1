using System;
using System.Collections.Generic;
using System.Text;

using VisualHint.SmartPropertyGrid;
using SysCAD.Protocol;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;

namespace SysCAD.Editor
{
  class dummyType {} // type to use to distinguish a dummy node.

  public class ModelPropertyGrid : PropertyGrid
  {
    private int id = 0;
    private Int64 requestId;

    private ModelItem modelItem = null;
    private State state = null;

    new internal void Clear()
    {
      modelItem = null;
      state = null;

      base.Clear();
    }

    internal void SetSelectedObject(ModelItem modelItem, State state)
    {
      this.modelItem = modelItem;
      this.state = state;

      PropertyEnumerator rootProperty = AppendRootCategory(id++, "Main");

      GetSubProperties(rootProperty, "What should be the propertyPath -- need to include this in modelitem definition... -- for now: " + modelItem.Guid.ToString());
    }

    private void GetSubProperties(PropertyEnumerator rootProperty, String propertyPath)
    {
      ArrayList propertyList = new ArrayList();
      state.GetSubTags(out requestId, propertyPath, out propertyList);

      foreach (ModelProperty modelProperty in propertyList)
      {
        PropertyEnumerator propertyEnum = AppendManagedProperty(rootProperty, id++, modelProperty.Path, modelProperty.State.GetType(), modelProperty.State, "");
        AppendManagedProperty(propertyEnum, -1, "", typeof(dummyType), null, "");
        ExpandProperty(propertyEnum, false);
      }

    }

    protected bool RemoveDummy(PropertyEnumerator property)
    {
      bool dummyDeleted = false;

      PropertyEnumerator child = null;

      if (property != null)
      {
        child = property.Children.MoveFirst();

        while (child.Property != null)
        {
          if ((child.Parent == property)&&(child.Property.Id == -1))
          {
            DeleteProperty(child);
            dummyDeleted = true;
          }
          child = child.MoveNext();
        }
      }
      return dummyDeleted; 
    }

    protected override void OnPropertyExpanded(PropertyExpandedEventArgs e)
    {
      if (e.Expanded == true)
      {
        PropertyEnumerator property = e.PropertyEnum;

        if (RemoveDummy(property))
        {
          GetSubProperties(property, "What should be the propertyPath -- need to include this in modelitem definition... -- for now: " + modelItem.Guid.ToString());
        }
      }
    }
  }
}
