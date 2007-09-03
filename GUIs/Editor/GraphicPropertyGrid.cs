
using System;
using System.Collections.Generic;
using System.Text;

using VisualHint.SmartPropertyGrid;
using SysCAD.Protocol;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace SysCAD.Editor
{

  internal class GraphicPropertyGrid : PropertyGrid
  {

    GraphicItem graphicItem = null;
    GraphicLink graphicLink = null;
    int id = 0;

    PropertyEnumerator itemGraphic;
    PropertyEnumerator itemGraphicAngle;
    PropertyEnumerator itemGraphicBoungingRect;
    PropertyEnumerator itemGraphicBoungingRectHeight;
    PropertyEnumerator itemGraphicBoungingRectLeft;
    PropertyEnumerator itemGraphicBoungingRectTop;
    PropertyEnumerator itemGraphicBoungingRectWidth;
    PropertyEnumerator itemGraphicFill;
    PropertyEnumerator itemGraphicFillColor;
    PropertyEnumerator itemGraphicFillMode;
    PropertyEnumerator itemGraphicMirror;
    PropertyEnumerator itemGraphicMirrorX;
    PropertyEnumerator itemGraphicMirrorY;
    PropertyEnumerator itemGraphicPath;
    PropertyEnumerator itemGraphicShape;

    PropertyEnumerator itemModel;
    PropertyEnumerator itemModelGuid;
    PropertyEnumerator itemModelModel;
    PropertyEnumerator itemModelTag;

    PropertyEnumerator linkModel;
    PropertyEnumerator linkModelClassID;
    PropertyEnumerator linkModelDestination;
    PropertyEnumerator linkModelDestinationPort;
    PropertyEnumerator linkModelGuid;
    PropertyEnumerator linkModelOrigin;
    PropertyEnumerator linkModelOriginPort;
    PropertyEnumerator linkModelTag;
    Int64 requestId;
    State state = null;

    protected override void OnPropertyButtonClicked(PropertyButtonClickedEventArgs e)
    {
      if (graphicItem != null)
      {
      Property a = e.PropertyEnum.Property;

      if (a.Name == "Shape")
      {
        GraphicStencilChangeForm gscf = new GraphicStencilChangeForm(state.GraphicUnselectedThumbnails, state.GraphicSelectedThumbnails, state.ModelStencils, state.GraphicStencils, a.Value.DisplayString);
        if (gscf.ShowDialog() == System.Windows.Forms.DialogResult.OK)
        {
          state.ModifyGraphicItem(out requestId, graphicItem.Guid, graphicItem.Tag, 
            graphicItem.Path, graphicItem.Model, gscf.Value, graphicItem.BoundingRect, 
            graphicItem.Angle, graphicItem.TextArea, graphicItem.TextAngle, graphicItem.FillColor, 
            graphicItem.FillMode, graphicItem.MirrorX, graphicItem.MirrorY);
        }
      }
      }
    }

    protected override void OnPropertyChanged(PropertyChangedEventArgs e)
    {

      if (graphicItem != null)
      {
        state.ModifyGraphicItem(out requestId, graphicItem.Guid, graphicItem.Tag, graphicItem.Path,
          graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.TextArea,
          graphicItem.TextAngle, graphicItem.FillColor, graphicItem.FillMode, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (graphicLink != null)
      {
        state.ModifyGraphicLink(out requestId, graphicLink.Guid, graphicLink.Tag, graphicLink.ClassID,
          graphicLink.Origin, graphicLink.Destination, graphicLink.OriginPort, graphicLink.DestinationPort,
          graphicLink.ControlPoints);
      }
    }

    new internal void Clear()
    {
      graphicItem = null;
      graphicLink = null;
      state = null;

      base.Clear();
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

    internal void SetSelectedObject(GraphicItem graphicItem, State state)
    {
      Clear();

      this.graphicItem = graphicItem;
      this.state = state;

      id = 0;
      itemGraphic = AppendRootCategory(id++, "Graphic");
      itemGraphicPath = AppendProperty(itemGraphic, id++, "Path", graphicItem, "Path", "", new System.ComponentModel.ReadOnlyAttribute(true));
      itemGraphicBoungingRect = AppendProperty(itemGraphic, id++, "Bounding Rectangle", graphicItem, "BoundingRect", "");
      itemGraphicBoungingRectLeft = AppendProperty(itemGraphicBoungingRect, id++, "Left", graphicItem, "X", "");
      itemGraphicBoungingRectTop = AppendProperty(itemGraphicBoungingRect, id++, "Top", graphicItem, "Y", "");
      itemGraphicBoungingRectWidth = AppendProperty(itemGraphicBoungingRect, id++, "Width", graphicItem, "Width", "");
      itemGraphicBoungingRectHeight = AppendProperty(itemGraphicBoungingRect, id++, "Height", graphicItem, "Height", "");
      ExpandProperty(itemGraphicBoungingRect, false);
      itemGraphicAngle = AppendProperty(itemGraphic, id++, "Angle", graphicItem, "Angle", "");
      
      itemGraphicShape = AppendProperty(itemGraphic, id++, "Shape", graphicItem, "Shape", "");
      itemGraphicShape.Property.Feel = GetRegisteredFeel(PropertyGrid.FeelButton);

      itemGraphicFill = AppendSubCategory(itemGraphic, id++, "Fill");
      itemGraphicFillColor = AppendProperty(itemGraphicFill, id++, "Color", graphicItem, "FillColor", "");
      itemGraphicFillMode = AppendProperty(itemGraphicFill, id++, "Mode", graphicItem, "FillMode", "");
      ExpandProperty(itemGraphicFill, false);
      itemGraphicMirror = AppendSubCategory(itemGraphic, id++, "Mirroring");
      itemGraphicMirrorX = AppendProperty(itemGraphicMirror, id++, "X", graphicItem, "MirrorX", "");
      itemGraphicMirrorX.Property.Feel = GetRegisteredFeel(FeelCheckbox);
      itemGraphicMirrorX.Property.Value.Look = new PropertyCheckboxLook();
      itemGraphicMirrorY = AppendProperty(itemGraphicMirror, id++, "Y", graphicItem, "MirrorY", "");
      itemGraphicMirrorY.Property.Feel = GetRegisteredFeel(FeelCheckbox);
      itemGraphicMirrorY.Property.Value.Look = new PropertyCheckboxLook();
      ExpandProperty(itemGraphicMirror, false);

      itemModel = AppendRootCategory(id++, "Model");
      itemModelGuid = AppendProperty(itemModel, id++, "Guid", graphicItem, "Guid", "", new System.ComponentModel.ReadOnlyAttribute(true));
      itemModelModel = AppendProperty(itemModel, id++, "Model", graphicItem, "Model", "", new System.ComponentModel.ReadOnlyAttribute(true));
      itemModelTag = AppendProperty(itemModel, id++, "Tag", graphicItem, "Tag", "");
    }

    internal void SetSelectedObject(GraphicLink graphicLink, State state)
    {
      Clear();

      this.graphicLink = graphicLink;
      this.state = state;

      id = 0;
      linkModel = AppendRootCategory(id++, "Model");
      linkModelGuid = AppendProperty(linkModel, id++, "Guid", graphicLink, "Guid", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelClassID = AppendProperty(linkModel, id++, "ClassID", graphicLink, "ClassID", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelOrigin = AppendProperty(linkModel, id++, "Origin", graphicLink, "Origin", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelDestination = AppendProperty(linkModel, id++, "Destination", graphicLink, "Destination", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelOriginPort = AppendProperty(linkModel, id++, "OriginPort", graphicLink, "OriginPort", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelDestinationPort = AppendProperty(linkModel, id++, "DestinationPort", graphicLink, "DestinationPort", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelTag = AppendProperty(linkModel, id++, "Tag", graphicLink, "Tag", "");
    }
  }
}
