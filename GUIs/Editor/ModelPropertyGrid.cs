using System;
using System.Collections.Generic;
using System.Text;
using VisualHint.SmartPropertyGrid;

namespace SysCAD.Editor
{
  class dummyType {} // type to use to distinguish a dummy node.

  public class ModelPropertyGrid : PropertyGrid
  {
    private int _id = 1;

    private SysCAD.Interface.ModelItem modelItem;

    internal void SetModel(SysCAD.Interface.ModelItem modelItem)
    {
      this.modelItem = modelItem;
      SelectedObject = modelItem;

      PropertyEnumerator testRootProperty = AppendRootCategory(_id++, "Main");
      ExpandProperty(testRootProperty, false);

      //PropertyEnumerator dummyProperty = AppendManagedProperty(testRootProperty, _id++, "", typeof(dummyType), null, "");

      //modelItem.Properties = new BaseAccessItem[6];
      //modelItem.Properties[0] = new FloatAccessItem("zero", "description zero", 074.6F);
      //modelItem.Properties[1] = new FloatAccessItem("one", "description one", 174.6F);
      //modelItem.Properties[2] = new FloatAccessItem("two", "description two", 274.6F);
      //modelItem.Properties[3] = new FloatAccessItem("three", "description three", 374.6F);
      //modelItem.Properties[4] = new FloatAccessItem("four", "description four", 474.6F);

      //BaseAccessItem[] properties = new BaseAccessItem[5];
      //properties[0] = new FloatAccessItem("zero branch", "description zero branch", 074.6F);
      //properties[1] = new FloatAccessItem("one branch", "description one branch", 174.6F);
      //properties[2] = new FloatAccessItem("two branch", "description two branch", 274.6F);
      //properties[3] = new FloatAccessItem("three branch", "description three branch", 374.6F);
      //properties[4] = new FloatAccessItem("four branch", "description four branch", 474.6F);

      //modelItem.Properties[5] = new BranchAccessItem("branch zero", "description branch zero", properties);
    }

    protected void removeDummy(PropertyEnumerator property)
    {
      PropertyEnumerator child = property.Children.MoveFirst();

      while (child != null)
      {

          if (child.GetType() == typeof(dummyType))
        {
          DeleteProperty(child);
        }
        child = child.MoveNext();
      }
    }

    protected override void OnPropertyExpanded(PropertyExpandedEventArgs e)
    {
      if (e.Expanded == true)
      {
        PropertyEnumerator property = e.PropertyEnum;
        
        removeDummy(property);

        PropertyEnumerator testProperty = AppendManagedProperty(property, _id++, "Tag", typeof(string), "Test Expanded Member", "Test Comment");
      }
    }
  }


}
