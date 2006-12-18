using System;
using System.Collections.Generic;
using System.Text;

using VisualHint.SmartPropertyGrid;
using System.ComponentModel;
using SysCAD.Interface;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace SysCAD.Editor
{
  public class GraphicPropertyGrid : VisualHint.SmartPropertyGrid.PropertyGrid
  {
    int id = 0;
    SysCAD.Interface.GraphicItem graphicItem;
    SysCAD.Interface.ClientInterface clientInterface;

    internal void SetSelectedObject(SysCAD.Interface.GraphicItem graphicItem, SysCAD.Interface.ClientInterface clientInterface)
    {
      Clear();

      this.graphicItem = graphicItem;
      this.clientInterface = clientInterface;

      id = 0;
      PropertyEnumerator graphic = AppendRootCategory(id++, "Graphic");
        PropertyEnumerator graphicPath = AppendProperty(graphic, id++, "Path", graphicItem, "Path", "", new ReadOnlyAttribute(true));
        PropertyEnumerator graphicBoungingRect = AppendProperty(graphic, id++, "Bounding Rectangle", graphicItem, "BoundingRect", "");
          PropertyEnumerator graphicBoungingRectX = AppendProperty(graphicBoungingRect, id++, "Left", graphicItem, "X", "");
          PropertyEnumerator graphicBoungingRectY = AppendProperty(graphicBoungingRect, id++, "Top", graphicItem, "Y", "");
          PropertyEnumerator graphicBoungingRectWidth = AppendProperty(graphicBoungingRect, id++, "Width", graphicItem, "Width", "");
          PropertyEnumerator graphicBoungingRectHeight = AppendProperty(graphicBoungingRect, id++, "Height", graphicItem, "Height", "");
          ExpandProperty(graphicBoungingRect, false);
        PropertyEnumerator graphicAngle = AppendProperty(graphic, id++, "Angle", graphicItem, "Angle", "");
        PropertyEnumerator graphicShape = AppendProperty(graphic, id++, "Shape", graphicItem, "Shape", "");
        PropertyEnumerator graphicFill = AppendSubCategory(graphic, id++, "Fill");
          PropertyEnumerator graphicFillColor = AppendProperty(graphicFill, id++, "Color", graphicItem, "FillColor", "");
          PropertyEnumerator graphicFillMode = AppendProperty(graphicFill, id++, "Mode", graphicItem, "FillMode", "");
          ExpandProperty(graphicFill, false);
        PropertyEnumerator graphicMirror = AppendSubCategory(graphic, id++, "Mirroring");
          PropertyEnumerator graphicMirrorX = AppendProperty(graphicMirror, id++, "X", graphicItem, "MirrorX", "");
          PropertyEnumerator graphicMirrorY = AppendProperty(graphicMirror, id++, "Y", graphicItem, "MirrorY", "");
          ExpandProperty(graphicMirror, false);

      PropertyEnumerator model = AppendRootCategory(id++, "Model");
        PropertyEnumerator modelGuid = AppendProperty(model, id++, "Guid", graphicItem, "Guid", "", new ReadOnlyAttribute(true));
        PropertyEnumerator modelModel = AppendProperty(model, id++, "Model", graphicItem, "Model", "", new ReadOnlyAttribute(true));
        PropertyEnumerator modelTag = AppendProperty(model, id++, "Tag", graphicItem, "Tag", "");
    }

    internal void Initialize()
    {
      BorderStyle = System.Windows.Forms.BorderStyle.None;
      CommentsHeight = 70;
      CommentsVisibility = true;
      Dock = System.Windows.Forms.DockStyle.Fill;
      Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
      Location = new System.Drawing.Point(0, 0);
      Margin = new System.Windows.Forms.Padding(2);
    }
  }
}
