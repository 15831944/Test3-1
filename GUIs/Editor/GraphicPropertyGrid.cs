
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

    GraphicNode graphicNode = null;
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
    PropertyEnumerator itemModelClass;
    PropertyEnumerator itemModelTag;

    PropertyEnumerator linkGraphic;
    PropertyEnumerator linkGraphicGuid;
    PropertyEnumerator linkGraphicOrigin;
    PropertyEnumerator linkGraphicDestination;
    PropertyEnumerator linkGraphicOriginPortID;
    PropertyEnumerator linkGraphicDestinationPortID;
    PropertyEnumerator linkGraphicTag;

    PropertyEnumerator linkModel;
    PropertyEnumerator linkModelClass;
    PropertyEnumerator linkModelOrigin;
    PropertyEnumerator linkModelDestination;
    PropertyEnumerator linkModelOriginPort;
    PropertyEnumerator linkModelDestinationPort;

    State state = null;

    protected override void OnPropertyButtonClicked(PropertyButtonClickedEventArgs e)
    {
      if (graphicNode != null)
      {
        Property a = e.PropertyEnum.Property;

        if (a.Name == "Shape")
        {
          GraphicStencilChangeForm gscf = new GraphicStencilChangeForm(state.GraphicUnselectedThumbnails, state.GraphicSelectedThumbnails, state.ModelStencils, state.GraphicStencils, a.Value.DisplayString);
          if (gscf.ShowDialog() == System.Windows.Forms.DialogResult.OK)
          {
            //state.ModifyGraphicItem(out requestId, graphicNode.Guid, graphicNode.Tag, 
            //  graphicNode.Path, graphicNode.Model, gscf.Value, graphicNode.BoundingRect, 
            //  graphicNode.Angle, graphicNode.TagArea, graphicNode.TagAngle, graphicNode.FillColor, 
            //  graphicNode.FillMode, graphicNode.MirrorX, graphicNode.MirrorY);
          }
        }
      }
    }

    protected override void OnPropertyChanged(PropertyChangedEventArgs e)
    {

      if (graphicNode != null)
      {
        //state.ModifyGraphicItem(out requestId, graphicNode.Guid, graphicNode.Tag, graphicNode.Path,
        //  graphicNode.Model, graphicNode.Shape, graphicNode.BoundingRect, graphicNode.Angle, graphicNode.TagArea,
        //  graphicNode.TagAngle, graphicNode.FillColor, graphicNode.FillMode, graphicNode.MirrorX, graphicNode.MirrorY);
      }

      if (graphicLink != null)
      {
        //state.ModifyGraphicLink(out requestId, graphicLink.Guid, graphicLink.Tag, graphicLink.ClassID,
        //  graphicLink.Origin, graphicLink.Destination,
        //  graphicLink.OriginPort, graphicLink.OriginPortID, graphicLink.DestinationPort, graphicLink.DestinationPortID,
        //  graphicLink.ControlPoints, graphicLink.TagArea, graphicLink.TagAngle);
      }
    }

    new internal void Clear()
    {
      graphicNode = null;
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

    internal void SetSelectedObject(GraphicNode graphicNode, ModelNode modelNode, State state)
    {
      Clear();

      this.graphicNode = graphicNode;
      this.state = state;

      id = 0;
      itemGraphic = AppendRootCategory(id++, "Graphic");
      itemGraphicPath = AppendProperty(itemGraphic, id++, "Path", graphicNode, "Path", "", new System.ComponentModel.ReadOnlyAttribute(true));
      itemGraphicBoungingRect = AppendProperty(itemGraphic, id++, "Bounding Rectangle", graphicNode, "BoundingRect", "");
      itemGraphicBoungingRectLeft = AppendProperty(itemGraphicBoungingRect, id++, "Left", graphicNode, "X", "");
      itemGraphicBoungingRectTop = AppendProperty(itemGraphicBoungingRect, id++, "Top", graphicNode, "Y", "");
      itemGraphicBoungingRectWidth = AppendProperty(itemGraphicBoungingRect, id++, "Width", graphicNode, "Width", "");
      itemGraphicBoungingRectHeight = AppendProperty(itemGraphicBoungingRect, id++, "Height", graphicNode, "Height", "");
      ExpandProperty(itemGraphicBoungingRect, false);
      itemGraphicAngle = AppendProperty(itemGraphic, id++, "Angle", graphicNode, "Angle", "");

      itemGraphicShape = AppendProperty(itemGraphic, id++, "Shape", graphicNode, "Shape", "");
      itemGraphicShape.Property.Feel = GetRegisteredFeel(PropertyGrid.FeelButton);

      itemGraphicFill = AppendSubCategory(itemGraphic, id++, "Fill");
      itemGraphicFillColor = AppendProperty(itemGraphicFill, id++, "Color", graphicNode, "FillColor", "");
      itemGraphicFillMode = AppendProperty(itemGraphicFill, id++, "Mode", graphicNode, "FillMode", "");
      ExpandProperty(itemGraphicFill, false);
      itemGraphicMirror = AppendSubCategory(itemGraphic, id++, "Mirroring");
      itemGraphicMirrorX = AppendProperty(itemGraphicMirror, id++, "X", graphicNode, "MirrorX", "");
      itemGraphicMirrorX.Property.Feel = GetRegisteredFeel(FeelCheckbox);
      itemGraphicMirrorX.Property.Value.Look = new PropertyCheckboxLook();
      itemGraphicMirrorY = AppendProperty(itemGraphicMirror, id++, "Y", graphicNode, "MirrorY", "");
      itemGraphicMirrorY.Property.Feel = GetRegisteredFeel(FeelCheckbox);
      itemGraphicMirrorY.Property.Value.Look = new PropertyCheckboxLook();
      ExpandProperty(itemGraphicMirror, false);

      itemModel = AppendRootCategory(id++, "Model");
      itemModelGuid = AppendProperty(itemModel, id++, "Guid", modelNode, "Guid", "", new System.ComponentModel.ReadOnlyAttribute(true));
      itemModelClass = AppendProperty(itemModel, id++, "Class", modelNode, "NodeClass", "", new System.ComponentModel.ReadOnlyAttribute(true));
      itemModelTag = AppendProperty(itemModel, id++, "Tag", modelNode, "Tag", "");
    }

    internal void SetSelectedObject(GraphicLink graphicLink, ModelLink modelLink, State state)
    {
      Clear();

      this.graphicLink = graphicLink;
      this.state = state;

      id = 0;
      linkGraphic = AppendRootCategory(id++, "Graphic");
      linkGraphicGuid = AppendProperty(linkGraphic, id++, "Guid", graphicLink, "Guid", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkGraphicOrigin = AppendProperty(linkGraphic, id++, "Origin", graphicLink, "Origin", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkGraphicDestination = AppendProperty(linkGraphic, id++, "Destination", graphicLink, "Destination", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkGraphicOriginPortID = AppendProperty(linkGraphic, id++, "Origin Port ID", graphicLink, "OriginPortID", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkGraphicDestinationPortID = AppendProperty(linkGraphic, id++, "Destination Port ID", graphicLink, "DestinationPortID", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkGraphicTag = AppendProperty(linkGraphic, id++, "Tag", graphicLink, "Tag", "");

      linkModel = AppendRootCategory(id++, "Model");
      linkModelClass = AppendProperty(linkModel, id++, "Class", modelLink, "LinkClass", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelOrigin = AppendProperty(linkModel, id++, "Origin", modelLink, "Origin", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelDestination = AppendProperty(linkModel, id++, "Destination", modelLink, "Destination", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelOriginPort = AppendProperty(linkModel, id++, "Origin Port", modelLink, "OriginPort", "", new System.ComponentModel.ReadOnlyAttribute(true));
      linkModelDestinationPort = AppendProperty(linkModel, id++, "Destination Port", modelLink, "DestinationPort", "", new System.ComponentModel.ReadOnlyAttribute(true));
    }
  }
}
