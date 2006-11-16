using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Editor
{
    public class GraphicPropertyGrid : VisualHint.SmartPropertyGrid.PropertyGrid
    {
        internal void SetGraphic(SysCAD.Interface.GraphicItem graphicItem)
        {
            SelectedObject = graphicItem;
        }
    }
}
