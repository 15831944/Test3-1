// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Windows.Forms;

using MindFusion.Geometry.Geometry2D;
using GeoConvert = MindFusion.Geometry.Geometry2D.Convert;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Represents a link between diagram nodes.
	/// </summary>
	public class Arrow : ChartObject
	{
		#region initialization

		internal static void initHeadTemplates()
		{
			if (headTemplates != null) return;

			headTemplates = new ArrowHeadTemplate[
				Enum.GetValues(typeof(ArrowHead)).Length];

			// ahNone
			ArrowHeadTemplate aht0 = new ArrowHeadTemplate();
			headTemplates[0] = aht0;

			// ahArrow
			ArrowHeadTemplate aht1 = new ArrowHeadTemplate();
			aht1.addLine(0.0f, 0.0f, 0.37f, 0.95f);
			aht1.addLine(0.0f, 0.0f, -0.37f, 0.95f);
			headTemplates[1] = aht1;

			// ahTriangle
			ArrowHeadTemplate aht2 = new ArrowHeadTemplate();
			float[] triangle = { 0.0f, 0.0f, 0.37f, 0.95f, -0.37f, 0.95f };
			aht2.addShape(triangle);
			headTemplates[2] = aht2;

			// ahCircle
			ArrowHeadTemplate aht3 = new ArrowHeadTemplate();
			aht3.addCircle(0.0f, 0.5f, 0.5f);
			headTemplates[3] = aht3;

			// ahTetragon
			ArrowHeadTemplate aht4 = new ArrowHeadTemplate();
			float[] tetragon = { 0.0f, 0.0f, 0.33f, 0.80f, 0.0f, 1.1f, -0.33f, 0.80f };
			aht4.addShape(tetragon);
			headTemplates[4] = aht4;

			// ahRhombus
			ArrowHeadTemplate aht5 = new ArrowHeadTemplate();
			float[] rhombus = { 0.0f, 0.0f, 0.33f, 0.55f, 0.0f, 1.1f, -0.33f, 0.55f };
			aht5.addShape(rhombus);
			headTemplates[5] = aht5;

			// ahBowArrow
			ArrowHeadTemplate aht6 = new ArrowHeadTemplate();
			float[] bowArrow = { 0.0f, 0.0f, 0.44f, 0.95f, 0.1f, 0.75f, 0.1f, 0.95f, -0.1f, 0.95f, -0.1f, 0.75f, -0.44f, 0.95f };
			aht6.addShape(bowArrow);
			headTemplates[6] = aht6;

			// ahPointerArrow
			ArrowHeadTemplate aht7 = new ArrowHeadTemplate();
			float[] pointerArrow = { 0.0f, 0.0f, 0.37f, 0.95f, 0.0f, 0.60f, -0.37f, 0.95f };
			aht7.addShape(pointerArrow);
			headTemplates[7] = aht7;

			// ahPentagon
			ArrowHeadTemplate aht8 = new ArrowHeadTemplate();
			float[] pentagon = { 0.0f, 0.0f, 0.37f, 0.37f, 0.37f, 0.95f, -0.37f, 0.95f, -0.37f, 0.37f };
			aht8.addShape(pentagon);
			headTemplates[8] = aht8;

			// ahDoubleArrow
			ArrowHeadTemplate aht9 = new ArrowHeadTemplate();
			float[] triangle1 = { 0.0f, 0.0f, 0.37f, 0.95f, -0.37f, 0.95f };
			float[] triangle2 = { 0.0f, 0.95f, 0.37f, 1.9f, -0.37f, 1.90f };
			aht9.addShape(triangle1);
			aht9.addShape(triangle2);
			headTemplates[9] = aht9;

			// ahPentagon
			ArrowHeadTemplate aht10 = new ArrowHeadTemplate();
			aht10.addLine(0.0f, 0.95f, 0.37f, 0.0f);
			aht10.addLine(0.0f, 0.95f, -0.37f, 0.0f);
			headTemplates[10] = aht10;

			// ahRevTriangle
			ArrowHeadTemplate aht11 = new ArrowHeadTemplate();
			float[] triangle3 = { 0.37f, 0.0f, 0.0f, 0.95f, -0.37f, 0.0f };
			aht11.addShape(triangle3);
			headTemplates[11] = aht11;

			// ahQuill
			ArrowHeadTemplate aht12 = new ArrowHeadTemplate();
			float[] arrow12 = { 0.0f, 0.0f, 0.37f, 0.37f, 0.37f, 0.95f, 0.0f, 0.58f, -0.37f, 0.95f, -0.37f, 0.37f };
			aht12.addShape(arrow12);
			headTemplates[12] = aht12;

			// ahRevWithLine
			ArrowHeadTemplate aht13 = new ArrowHeadTemplate();
			aht13.addLine(0.0f, 0.95f, 0.37f, 0.0f);
			aht13.addLine(0.0f, 0.95f, -0.37f, 0.0f);
			aht13.addLine(-0.37f, 1.1f, 0.37f, 1.1f);
			headTemplates[13] = aht13;

			// ahRevWithCirc
			ArrowHeadTemplate aht14 = new ArrowHeadTemplate();
			aht14.addLine(0.0f, 0.95f, 0.37f, 0.0f);
			aht14.addLine(0.0f, 0.95f, -0.37f, 0.0f);
			aht14.addCircle(0.0f, 1.47f, 0.37f);
			headTemplates[14] = aht14;

			// ahBackSlash
			ArrowHeadTemplate aht15 = new ArrowHeadTemplate();
			aht15.addLine(-0.37f, -0.37f, 0.37f, 0.37f);
			headTemplates[15] = aht15;

			// ahSlash
			ArrowHeadTemplate aht16 = new ArrowHeadTemplate();
			aht16.addLine(-0.37f, 0.37f, 0.37f, -0.37f);
			headTemplates[16] = aht16;

			// ahDefaultFlow
			ArrowHeadTemplate aht17 = new ArrowHeadTemplate();
			aht17.addLine(-0.37f, 1.11f, 0.37f, 0.37f);
			headTemplates[17] = aht17;
		}

		public Arrow(FlowChart parent) : base(parent)
		{
			pen = (Pen)fcParent.ArrowPen.Clone();
			headPen = (Pen)pen.Clone();
			brush = fcParent.ArrowBrush;
			brush.AddRef();

			penColor = parent.ArrowColor;
			frameColor = parent.ArrowColor;
			fillColor = parent.ArrowFillColor;

			style = parent.ArrowStyle;
			setSegments(parent.ArrowSegments);
			selStyle = parent.ArrowHandlesStyle;

			text = parent.ArrowText;
			textStyle = parent.ArrowTextStyle;
			textColor = parent.TextColor;
			updateText();

			reflexive = false;

			dynamic = parent.DynamicArrows;
			snapToNodeBorder = parent.ArrowsSnapToBorders;
			retainForm = parent.ArrowsRetainForm;

			cascadeOrientation = parent.ArrowCascadeOrientation;
			cascadeStartHorizontal = true;

			orgnLink = null;
			destLink = null;
			objNewDest = null;

			headSize = 5;

			arrowHead = fcParent.ArrowHead;
			arrowBase = fcParent.ArrowBase;
			arrowInterm = fcParent.IntermArrowHead;

			ahHead = new ArrowHeadShape(fcParent.ArrowHeadSize);
			ahBase = new ArrowHeadShape(fcParent.ArrowBaseSize);
			ahInterm = new ArrowHeadShape(fcParent.ArrowIntermSize);

			headTemplates[(int)arrowHead].initArrowHead(ahHead);
			headTemplates[(int)arrowBase].initArrowHead(ahBase);
			headTemplates[(int)arrowInterm].initArrowHead(ahInterm);

			headSize = fcParent.ArrowHeadSize;
			baseSize = fcParent.ArrowBaseSize;
			intermSize = fcParent.ArrowIntermSize;

			textSize = new SizeF(0, 0);

			allowMoveStart = fcParent.ArrowEndsMovable;
			allowMoveEnd = fcParent.ArrowEndsMovable;

			orgnAnchor = -1;
			destAnchor = -1;

			autoRoute = fcParent.RouteArrows;

			customDraw = parent.ArrowCustomDraw;
			savedSegments = null;
			drawCrossings = true;
		}

		public Arrow(FlowChart parent, Node src, Node dest) : this(parent)
		{
			setOrgAndDest(
				src.createLink(this, src.getCenter(), false),
				dest.createLink(this, dest.getCenter(), true));
		}

		public Arrow(FlowChart parent, Node src, PointF dest) : this(parent)
		{
			setOrgAndDest(
				src.createLink(this, src.getCenter(), false),
				new DummyLink(this, true, dest));
		}

		public Arrow(FlowChart parent, PointF src, Node dest) : this(parent)
		{
			setOrgAndDest(
				new DummyLink(this, false, src),
				dest.createLink(this, dest.getCenter(), true));
		}

		public Arrow(FlowChart parent, PointF src, PointF dest) : this(parent)
		{
			setOrgAndDest(
				new DummyLink(this, false, src),
				new DummyLink(this, true, dest));
		}

		public Arrow(Arrow prototype, Node src, Node dest) : base(prototype)
		{
			penColor = prototype.penColor;
			headPen = (Pen)prototype.headPen.Clone();

			style = prototype.style;
			setSegments(prototype.segmentCount);
			selStyle = prototype.selStyle;

			text = prototype.text;
			textStyle = prototype.textStyle;
			textColor = prototype.textColor;
			updateText();

			reflexive = prototype.reflexive;

			dynamic = prototype.dynamic;
			snapToNodeBorder = prototype.snapToNodeBorder;
			retainForm = prototype.retainForm;
			cascadeOrientation = prototype.cascadeOrientation;
			cascadeStartHorizontal = prototype.cascadeStartHorizontal;

			orgnLink = null;
			destLink = null;
			objNewDest = null;

			headSize = 5;

			arrowHead = prototype.arrowHead;
			arrowBase = prototype.arrowBase;
			arrowInterm = prototype.arrowInterm;

			headSize = prototype.headSize;
			baseSize = prototype.baseSize;
			intermSize = prototype.intermSize;

			ahHead = new ArrowHeadShape(headSize);
			ahBase = new ArrowHeadShape(baseSize);
			ahInterm = new ArrowHeadShape(intermSize);

			headTemplates[(int)arrowHead].initArrowHead(ahHead);
			headTemplates[(int)arrowBase].initArrowHead(ahBase);
			headTemplates[(int)arrowInterm].initArrowHead(ahInterm);

			textSize = prototype.textSize;

			allowMoveStart = prototype.allowMoveStart;
			allowMoveEnd = prototype.allowMoveEnd;

			orgnAnchor = prototype.orgnAnchor;
			destAnchor = prototype.destAnchor;

			autoRoute = false;
			setOrgAndDest(
				src.createLink(this, src.getCenter(), false),
				dest.createLink(this, dest.getCenter(), true));

			for (int i = 0; i < points.Count; ++i)
				points[i] = prototype.points[i];

			OrgnIndex = prototype.OrgnIndex;
			DestIndex = prototype.DestIndex;
			OrgnAnchor = prototype.OrgnAnchor;
			DestAnchor = prototype.DestAnchor;

			int l = points.Count - 1;
			if (Origin is DummyNode)
				points[0] = prototype.points[0];
			if (Destination is DummyNode)
				points[l] = prototype.points[l];

			customDraw = prototype.customDraw;

			autoRoute = prototype.autoRoute;
			savedSegments = null;
		}

		public Arrow(FlowChart parent, Node orgnNode) : this(parent)
		{
			this.orgnNode = orgnNode;
		}

		internal override void onAdd()
		{
			base.onAdd();

			// Listen for changes in FlowChart' unit of measure
			parentUnitChanged = new EventHandler(OnParentUnitChanged);
			fcParent.MeasureUnitChanged += parentUnitChanged;
		}

		#endregion

		#region cleanup

		internal override void onRemove()
		{
			base.onRemove();

			if (parentUnitChanged != null)
			{
				fcParent.MeasureUnitChanged -= parentUnitChanged;
				parentUnitChanged = null;
			}

			// fire an ArrowDeleted event
			if (constructed)
				fcParent.fireArrowDeleted(this);
		}

		internal override void freeResources()
		{
			base.freeResources();
			
			if (orgnLink != null)
			{
				orgnLink.freeResources();
				orgnLink = null;
			}

			if (destLink != null)
			{
				destLink.freeResources();
				destLink = null;
			}
		}

		#endregion

		#region serialization

		public override int getClassId()
		{
			return 2;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			writer.Write((int)style);
			ctx.saveColor(penColor);
			ctx.saveReference(this, orgnLink, 1);
			ctx.saveReference(this, destLink, 2);
			writer.Write(segmentCount);
			writer.Write(text);
			ctx.saveColor(textColor);
			writer.Write((int)arrowHead);
			writer.Write((int)arrowBase);
			writer.Write((int)arrowInterm);
			writer.Write((double)headSize);
			writer.Write((double)baseSize);
			writer.Write((double)intermSize);
			ctx.saveColor(fillColor);
			writer.Write(dynamic);
			writer.Write((int)cascadeOrientation);
			writer.Write(cascadeStartHorizontal);
			writer.Write(reflexive);
			ctx.saveReference(this, points, 3);
			ctx.saveReference(this, null, 4);

			ctx.saveObject(pen);
			ctx.saveReference(this, brush, 5);

			// new in file format 5
			writer.Write(allowMoveStart);
			writer.Write(allowMoveEnd);
			writer.Write(orgnAnchor);
			writer.Write(destAnchor);

			// new in file format 8
			writer.Write(autoRoute);
			writer.Write(retainForm);
			writer.Write((int)textStyle);

			// new in file format 10
			ctx.saveObject(headPen);

			// new in file format 17
			writer.Write((int)customDraw);

			// new in file format 20
			writer.Write(snapToNodeBorder);
			writer.Write((int)selStyle);

			// new in file format 26
			writer.Write(hyperLink);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			style = (ArrowStyle)reader.ReadInt32();
			penColor = ctx.loadColor();
			ctx.loadReference(this);
			ctx.loadReference(this);
			segmentCount = reader.ReadInt16();
			text = reader.ReadString();
			textColor = ctx.loadColor();
			arrowHead = (ArrowHead)reader.ReadInt32();
			arrowBase = (ArrowHead)reader.ReadInt32();
			arrowInterm = (ArrowHead)reader.ReadInt32();
			headSize = (float)reader.ReadDouble();
			baseSize = (float)reader.ReadDouble();
			intermSize = (float)reader.ReadDouble();
			fillColor = ctx.loadColor();
			dynamic = reader.ReadBoolean();
			cascadeOrientation = (Orientation)reader.ReadInt32();
			cascadeStartHorizontal = reader.ReadBoolean();
			reflexive = reader.ReadBoolean();
			ctx.loadReference(this);
			ctx.loadReference(this);

			ahHead = new ArrowHeadShape(headSize);
			ahBase = new ArrowHeadShape(baseSize);
			ahInterm = new ArrowHeadShape(intermSize);

			headTemplates[(int)arrowHead].initArrowHead(ahHead);
			headTemplates[(int)arrowBase].initArrowHead(ahBase);
			headTemplates[(int)arrowInterm].initArrowHead(ahInterm);

			pen.Width = PenWidth;
			pen.Color = frameColor;
			pen.DashStyle = PenDashStyle;
			brush.Release();
			brush = new SolidBrush(fillColor);
			brush.AddRef();
			headPen.Width = PenWidth;
			headPen.Color = frameColor;
			headPen.DashStyle = PenDashStyle;

			if (ctx.FileVersion > 3)
			{
				pen = (Pen)ctx.loadObject();
				ctx.loadReference(this);

				if (ctx.FileVersion > 4)
				{
					// new in file format 5
					allowMoveStart = reader.ReadBoolean();
					allowMoveEnd = reader.ReadBoolean();
					orgnAnchor = reader.ReadInt32();
					destAnchor = reader.ReadInt32();

					if (ctx.FileVersion > 7)
					{
						// new in file format 8
						autoRoute = reader.ReadBoolean();
						retainForm = reader.ReadBoolean();
						textStyle = (ArrowTextStyle)reader.ReadInt32();

						if (ctx.FileVersion > 9)
						{
							// new in file format 10
							headPen = (Pen)ctx.loadObject();

							if (ctx.FileVersion > 16)
							{
								// new in file format 17
								customDraw = (CustomDraw)reader.ReadInt32();

								if (ctx.FileVersion > 19)
								{
									// new in file format 20
									snapToNodeBorder = reader.ReadBoolean();
									selStyle = (HandlesStyle)ctx.reader.ReadInt32();

									if (ctx.FileVersion > 25)
									{
										// new in file format 26
										hyperLink = reader.ReadString();
									}
								}
							}
						}
					}
				}
			}

			updateText();
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId, obj);

			switch (refId)
			{
				case 1:
					orgnLink = (Link)obj;
					break;
				case 2:
					destLink = (Link)obj;
					break;
				case 3:
					points = (PointCollection)obj;
					break;
				case 4:
					// not used anymore: savedPoints = (PointCollection)obj;
					updateArrowHeads();
					break;
				case 5:
					brush.Release();
					brush = (Brush)obj;
					brush.AddRef();
					break;
			}
		}

		#endregion

		#region interaction

		// ************ creation ************

		internal override void startCreate(PointF org)
		{
			// get the object under mouse
			Node orgNode = orgnNode;

			org = orgNode.getAnchor(org, this, false, ref orgnAnchor);
			base.startCreate(org);

			orgnLink = orgNode.createLink(this, org, false);

			// initialize the control points
			for (int i = 0; i < points.Count; ++i)
				points[i] = org;
			
			headTemplates[(int)arrowHead].recalcArrowHead(ahHead, org, org);
			headTemplates[(int)arrowBase].recalcArrowHead(ahBase, org, org);
		}

		internal override void updateCreate(PointF current)
		{
			base.updateCreate(current);

			if (autoRoute &&
				(fcParent.RoutingOptions.TriggerRerouting & RerouteArrows.WhileCreating) != 0)
			{
				Link target = routeGetTarget(current);
				points[points.Count-1] = current;
				if (fcParent.RoutingOptions.Anchoring == Anchoring.Ignore)
				{
					setEndPoints(orgnLink, target, current);
					if (!target.sameNode(orgnLink))
					{
						PointF startPoint = points[0];
						PointF endPoint = current;
						int oNearest = 0, dNearest = 0;
						routeGetEndPoints(ref startPoint, ref endPoint,
							ref oNearest, ref dNearest, orgnLink, target, true);
						ptEnd = endPoint = target.getIntersection(target.getInitialPt(), endPoint);
					}
					points[points.Count-1] = ptEnd;
				}
				
				doRoute(false, orgnLink, target, true);
			}
			else
			{
				//compute the arrow points
				updatePoints(ptEnd);

				resetCrossings();
				updateText();
			}
		}

		internal override void completeCreate(PointF end)
		{
			bool routedWhileCreating = autoRoute &&
				(fcParent.RoutingOptions.TriggerRerouting & RerouteArrows.WhileCreating) != 0;

			// get the object under mouse cursor
			Node obj = fcParent.getCurrBehavior().GetTargetNode(end);
			if (obj == null)
				obj = fcParent.Dummy;

			if (routedWhileCreating)
			{
				base.completeCreate();
				destLink = obj.createLink(this, end, true);
				reflexive = destLink.sameNode(orgnLink);
			}
			else
			{
				end = obj.getAnchor(ptEnd, this, true, ref destAnchor);
				base.completeCreate(end);

				destLink = obj.createLink(this, end, true);
				setEndPoints(orgnLink, destLink, end);

				// set reflexive arrow position & form
				if (reflexive)
				{
					setReflexive();
				}
				else
				{
					if (dynamic)
						updatePosFromOrgAndDest(true);
					else
						// compute the arrow points
						updatePoints(ptEnd);
				}

				// route arrow
				doRoute();
				resetCrossings();
				updateText();
			}

			// relative position to nodes
			orgnLink.saveEndRelative();
			destLink.saveEndRelative();
		}

		private void setEndPoints(Link orgnLink, Link destLink, PointF end)
		{
			reflexive = destLink.sameNode(orgnLink);

			// align the end points of the arrow to the outlines of the connected nodes
			if (orgnLink.calcIntrs())
				ptOrg = points[0] = orgnLink.getInitialPt();
			if (destLink.calcIntrs())
				ptEnd = destLink.getInitialPt();
			else
				ptEnd = end;

			if (!orgnLink.objsIntersect(destLink) && !reflexive)
			{
				if (orgnLink.calcIntrs())
					ptOrg = points[0] = orgnLink.getIntersection(ptOrg, ptEnd);
				if (destLink.calcIntrs())
					ptEnd = destLink.getIntersection(ptEnd, ptOrg);
			}
			else
			{
				// if the arrow will be routed, choose the closest points on the
				// horizontal line intersections with the node outlines. These points
				// should not be contained within a node
				if (autoRoute && !dynamic && orgnAnchor == -1 && destAnchor == -1)
				{
					chooseRouteOutlinePoints(orgnLink, destLink, ref ptOrg, ref ptEnd);
					points[0] = ptOrg;
				}
			}
		}

		Node orgnNode = null;

		internal override bool allowCreate(PointF current)
		{
			objNewDest = null;

			// don't allow creation if the arrow doesn't point a object
			if (ptOrg == current) return false;
			Node node = fcParent.getCurrBehavior().GetTargetNode(current);
			if (node == null)
			{
				objNewDest = fcParent.Dummy;
				return fcParent.AllowUnconnectedArrows && fcParent.confirmCreate(this);
			}

			if (node.notInteractive())
				return false;
			if (!node.canHaveArrows(false))
				return false;

			objNewDest = node;

			// check for reflexive links
			if (!fcParent.AllowRefLinks && node == orgnLink.getNode())
				return false;

			// check for repeated links
			if (!fcParent.AllowLinksRepeat && orgnLink.getNode().alreadyLinked(node))
				return false;

			RectangleF rc = getBoundingRect();
			if (fcParent.rectRestrict(ref rc))
				return false;

			fcParent.setAutoAnchors(node);

			return fcParent.confirmCreate(this);
		}

		private Node objNewDest = null;
		internal Node NewDest { get { return objNewDest; }}

		internal override bool allowModify(PointF current)
		{
			if (!snapToNodeBorder)
				current = fcParent.AlignPointToGrid(current);

			if (modifyHandle == 0 && current.Equals(points[points.Count-1]))
				return false;
			if (modifyHandle == points.Count-1 && current.Equals(points[0]))
				return false;
			if (modifyHandle == 0 || modifyHandle == points.Count-1)
			{
				Node node = fcParent.GetNodeAt(current);
				objNewDest = node;

				if (node == null)
					return fcParent.AllowUnconnectedArrows && fcParent.confirmModify(this);

				if (node.notInteractive())
					return false;
				if (!node.canHaveArrows(modifyHandle == 0))
					return false;

				bool changingOrg = modifyHandle == 0;
				if (!fcParent.requestAttach(this, changingOrg, node))
					return false;

				// check for reflexive links
				if (!fcParent.AllowRefLinks)
				{
					if (modifyHandle == 0 && node == destLink.getNode())
						return false;
					if (modifyHandle == points.Count-1 && node == orgnLink.getNode())
						return false;
				}

				// check for repeated links
				bool linkedObjChanges =	node !=
					(changingOrg ? orgnLink.getNode() : destLink.getNode());
				if (!fcParent.AllowLinksRepeat && linkedObjChanges)
				{
					if (modifyHandle == 0 && node.alreadyLinked(destLink.getNode()))
						return false;
					if (modifyHandle == points.Count-1 &&
						orgnLink.getNode().alreadyLinked(node))
						return false;
				}

				fcParent.setAutoAnchors(node);
			}
			else 
				if (style == ArrowStyle.Cascading &&
				(modifyHandle == 1 || modifyHandle == points.Count - 2))
			{
				int h = modifyHandle;
				PointF ptp = points[h-1];
				PointF ptn = points[h+1];
				if ((cascadeStartHorizontal && h%2 > 0) || (!cascadeStartHorizontal && h%2 == 0))
				{
					ptp.Y = points[h].Y;
					ptn.X = points[h].X;
				}
				else
				{
					ptp.X = points[h].X;
					ptn.Y = points[h].Y;
				}
				if (modifyHandle == 1 && !ptp.Equals(points[0]))
					if (!orgnLink.canMoveLink(ptp))
						return false;
				if (modifyHandle == points.Count - 2 && !ptn.Equals(points[points.Count-1]))
					if (!destLink.canMoveLink(ptn))
						return false;
			}

			RectangleF rc = getBoundingRect();
			if (fcParent.rectRestrict(ref rc))
				return false;

			return fcParent.confirmModify(this);
		}

		internal void startModifyOrg(InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.startModify(points[0], 0, ist);

			// save the state of segments so it can be restored from cancelModify
			savedSegments = saveSegments();

			if (subordinateGroup != null)
				subordinateGroup.beginModification(ist);

			cycleProtect = false;
		}

		internal void startModifyEnd(InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.startModify(points[points.Count-1], points.Count-1, ist);

			// save the state of segments so it can be restored from cancelModify
			savedSegments = saveSegments();

			if (subordinateGroup != null)
				subordinateGroup.beginModification(ist);

			cycleProtect = false;
		}


		internal override void startModify(PointF org, int handle, InteractionState ist)
		{
			if (cycleProtect) { cycleDetected = true; return; } 
			else cycleDetected = false;
			cycleProtect = true;

			base.startModify(org, handle, ist);

			// save the state of segments so it can be restored from cancelModify
			savedSegments = saveSegments();

			if (subordinateGroup != null)
				subordinateGroup.beginModification(ist);

			cycleProtect = false;
		}

		private void updateRelatedPoints(PointF current, InteractionState ist)
		{
			// when the arrow style is asBezier it has two kinds of control points
			// 1. the points through which the curve go, when they are moved the adjacent
			// points of the second type are moved with the same translation vector
			// 2. the points controlling the bend of the curve, the line that goes through 
			// them and the point of type 1 between them is tangent in this type 1 point.
			// when any of the points in type 2 pair is modified, its corresponding
			// point is moved so they maintain there symmetry in relation to the type 1 point
			if (style == ArrowStyle.Bezier)
			{
				if ((modifyHandle+1) % 3 == 0 && modifyHandle < points.Count-3)
				{
					points[modifyHandle+2] =
						Utilities.symmetricPt(points[modifyHandle], points[modifyHandle+1]);
				}

				if ((modifyHandle-1) % 3 == 0 && modifyHandle > 3)
				{
					points[modifyHandle-2] =
						Utilities.symmetricPt(points[modifyHandle], points[modifyHandle-1]);
				}

				if ((modifyHandle%3 == 0) && modifyHandle > 0 && modifyHandle < points.Count-1)
				{
					points[modifyHandle-1] = new PointF(
						points[modifyHandle-1].X + modifyDX, points[modifyHandle-1].Y + modifyDY);
					points[modifyHandle+1] = new PointF(
						points[modifyHandle+1].X + modifyDX, points[modifyHandle+1].Y + modifyDY);
				}
			}

			// in case of a cascading arrow, when a point is moved the two
			// points adjacent to it are moved too, so the lines connecting them
			// maintain their horizontal / vertical orientation
			if (style == ArrowStyle.Cascading)
			{
				int h = modifyHandle;
				int ip = modifyHandle - 1;
				int ix = modifyHandle + 1;

				float dX = points[points.Count - 1].X - points[0].X;
				float dY = points[points.Count - 1].Y - points[0].Y;

				// when dragging an end point, change the start segment orientation
				// if the dx/dy ratio does not correspond to the current orientation
				if (cascadeOrientation == Orientation.Auto && (h == 0 || h == points.Count - 1) &&
					((cascadeStartHorizontal && Math.Abs(dX) < Math.Abs(dY)) ||
					(!cascadeStartHorizontal && Math.Abs(dX) > Math.Abs(dY))))
				{
					cascadeStartHorizontal = Math.Abs(dX) > Math.Abs(dY);
					arrangePrpSegments(dX, dY);
				}
				else
				{
					// let users change the segment orientation using a mouse gesture
					// while dragging a point next to the first or last one
					float mouseDX = current.X - ist.StartPoint.X;
					float mouseDY = current.Y - ist.StartPoint.Y;

					if (cascadeOrientation == Orientation.Auto)
					{
						bool transpose = false;

						if (h == 1)
						{
							float segmentDX = points[2].X - points[1].X;
							float segmentDY = points[2].Y - points[1].Y;
							float pointDX = current.X - points[0].X;
							float pointDY = current.Y - points[0].Y;

							transpose = CascadeStartHorizontal ?
								Math.Abs(pointDY) > Math.Abs(pointDX) && mouseDY * segmentDY > 0 :
								Math.Abs(pointDX) > Math.Abs(pointDY) && mouseDX * segmentDX > 0;
						}
						else

						if (h == points.Count - 2)
						{
							float segmentDX = points[points.Count - 2].X - points[points.Count - 1].X;
							float segmentDY = points[points.Count - 2].Y - points[points.Count - 1].Y;
							float pointDX = current.X - points[points.Count - 1].X;
							float pointDY = current.Y - points[points.Count - 1].Y;

							transpose = CascadeEndHorizontal ?
								Math.Abs(pointDY) > Math.Abs(pointDX) && mouseDY * segmentDY > 0 :
								Math.Abs(pointDX) > Math.Abs(pointDY) && mouseDX * segmentDX > 0;
						}

						if (transpose)
						{
							if (fcParent.AllowSplitArrows)
							{
								if (ist.splitToChangeOrient)
								{
									// insert a new segment and change only the orientation of the one at the end
									segmentCount++;
									if (h == 1)
									{
										cascadeStartHorizontal = !cascadeStartHorizontal;
										points[1] = points[2];
										points.Insert(1, current);
									}
									else
									if (h == points.Count - 2)
									{
										points[points.Count - 2] = points[points.Count - 3];
										points.Insert(points.Count - 1, current);
										h++; ip++; ix++;
										modifyHandle++;
										ist.setHandleIndex(ist.SelectionHandle + 1);
									}
								}
								else
								{
									// remove the segment that was added for the previous change of orientation
									segmentCount--;
									if (h == 1)
									{
										cascadeStartHorizontal = !cascadeStartHorizontal;
										points.RemoveAt(1);
									}
									else
									if (h == points.Count - 2)
									{
										points.RemoveAt(points.Count - 2);
										h--; ip--; ix--;
										modifyHandle--;
										ist.setHandleIndex(ist.SelectionHandle - 1);
									}
								}

								ist.splitToChangeOrient = !ist.splitToChangeOrient;
							}
							else
							{
								cascadeStartHorizontal = !cascadeStartHorizontal;

								// change the orientation of all segments
								PointF start = points[0];
								for (int i = 1; i < points.Count - 1; ++i)
								{
									PointF point = points[i];
									point.X = (point.X - start.X) / dX * 100;
									point.Y = (point.Y - start.Y) / dY * 100;
									points[i] = new PointF(
										start.X + point.Y * dX / 100, start.Y + point.X * dY / 100);
								}
								points[h] = current;
							}
						}
					}

					alignCascadingSegments(h);
				}
			}
		}

		private void alignCascadingSegments(int h)
		{
			// align the adjacent control points so the segments
			// keep their horizontal or vertical orientation
			int ip = h - 1;
			int ix = h + 1;

			if ((cascadeStartHorizontal && h%2 != 0) || (!cascadeStartHorizontal && !(h%2 !=0 )))
			{
				if (ip == 0)
					points[h] = new PointF(points[h].X, points[ip].Y);
				if (ix == points.Count-1)
					points[h] = new PointF(points[ix].X, points[h].Y);
				if (ip >= 0 && ip < points.Count)
					points[ip] = new PointF(points[ip].X, points[h].Y);
				if (ix >= 0 && ix < points.Count)
					points[ix] = new PointF(points[h].X, points[ix].Y);
			}
			else
			{
				if (ip == 0)
					points[h] = new PointF(points[ip].X, points[h].Y);
				if (ix == points.Count-1)
					points[h] = new PointF(points[h].X, points[ix].Y);
				if (ip >= 0 && ip < points.Count)
					points[ip] = new PointF(points[h].X, points[ip].Y);
				if (ix >= 0 && ix < points.Count)
					points[ix] = new PointF(points[ix].X, points[h].Y);
			}
		}

		internal override void updateModify(PointF current, InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			if (snapToNodeBorder && objNewDest != null &&
				objNewDest.getType() != ItemType.Dummy &&
				(modifyHandle == 0 || modifyHandle == points.Count - 1))
				current = objNewDest.getNearestOutlinePt(current);
			else
				current = fcParent.AlignPointToGrid(current);

			float dx = current.X - points[modifyHandle].X;
			float dy = current.Y - points[modifyHandle].Y;

			// update ctrl. point position
			base.updateModify(current, ist);

			if (modifyHandle == 0)
				retain(dx, dy, true);
			if (modifyHandle == points.Count - 1)
				retain(dx, dy, false);

			points[modifyHandle] = current;
			updateRelatedPoints(current, ist);

			if (dynamic && style != ArrowStyle.Cascading &&  points.Count > 2 &&
				(modifyHandle == 1 || modifyHandle == points.Count - 2))
				updatePosFromOrgAndDest(true);

			updateArrowHeads();
			resetCrossings();
			updateText();

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(ist);

			cycleProtect = false;
		}


		internal override void completeModify()
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.completeModify();

			if (autoRoute && fcParent.rerouteArrow(this)) doRoute();
			resetCrossings();
			updateText();

			if (subordinateGroup != null) subordinateGroup.endModification();

			cycleProtect = false;
		}


		internal override void completeModify(PointF end, InteractionState ist)
		{
			// make sure the modification does not propagate through endless recursion
			if (cycleProtect) return;
			cycleProtect = true;

			if (!(snapToNodeBorder && objNewDest != null))
				end = fcParent.AlignPointToGrid(end);

			// snap to anchor point 
			Node objNode = null;
			if (modifyHandle == 0 || modifyHandle == points.Count-1)
			{
				objNode = fcParent.GetNodeAt(end);

				// align to node border if that's enabled
				if (snapToNodeBorder && objNode != null)
					end = objNode.getNearestOutlinePt(end);

				// align to anchor point
				int idx = -1;
				if (objNode != null &&
					fcParent.SnapToAnchor == SnapToAnchor.OnCreateOrModify)
					end = objNode.getAnchor(end, this, modifyHandle != 0, ref idx);
					
				if (modifyHandle == 0) orgnAnchor = idx; else destAnchor = idx;
			}

			// Route only if either the first or the last point is modified
			bool shouldRoute = (modifyHandle == 0 || modifyHandle == points.Count - 1);

			// update ctrl. point position
			base.completeModify(end, ist);
			points[modifyHandle] = end;
			updateRelatedPoints(end, ist);

			// update the links if the origin object has changed
			if (modifyHandle == 0)
			{
				PointF pt = points[0];
				if (objNode == null)
					objNode = fcParent.GetNodeAt(pt);
				if (objNode == null)
					objNode = fcParent.Dummy;
				if (objNode != null && orgnLink.linkChanges(objNode, pt))
				{
					orgnLink.removeArrowFromObj();
					orgnLink = null;
					orgnLink = objNode.createLink(this, pt, false);
					orgnLink.addArrowToObj();
					reflexive = orgnLink.sameNode(destLink);
				}
				orgnLink.saveEndRelative();
			}
			else

			// update the links if the target object has changed
			if (modifyHandle == points.Count-1)
			{
				PointF pt = points[points.Count-1];
				if (objNode == null)
					objNode = fcParent.GetNodeAt(pt);
				if (objNode == null)
					objNode = fcParent.Dummy;
				if (objNode != null && destLink.linkChanges(objNode, pt))
				{
					destLink.removeArrowFromObj();
					destLink = null;
					destLink = objNode.createLink(this, pt, true);
					destLink.addArrowToObj();
					reflexive = destLink.sameNode(orgnLink);
				}
				destLink.saveEndRelative();
			}
			else

			// combine arrow segments ?
			{
				PointF ptPrev = points[modifyHandle - 1];
				PointF ptCurr = points[modifyHandle];
				PointF ptNext = points[modifyHandle + 1];

				// for polylines: if the angle between them is ~180
				if (style == ArrowStyle.Polyline && fcParent.AllowSplitArrows)
				{
					PointCollection pts = new PointCollection(2);
					pts.SetAt(0, ptPrev);
					pts.SetAt(1, ptNext);

					float dx = ptNext.X - ptPrev.X;
					float dy = ptNext.Y - ptPrev.Y;
					float len = (float)Math.Sqrt(dx*dx + dy*dy);

					float d = Utilities.distToPolyline(ptCurr, pts, 2);

					if (d*35 < len)
					{
						points.RemoveAt(modifyHandle);
						segmentCount--;
						if (subordinateGroup != null)
							subordinateGroup.onArrowSplit(false, modifyHandle, 1);
					}
				}

				// combine segments of cascading arrows if two adjacent control points overlap
				if (style == ArrowStyle.Cascading && segmentCount > 3 &&
					fcParent.AllowSplitArrows)
				{
					int toDel = -1;

					if (modifyHandle > 1 && fcParent.mergePoints(ptPrev, ptCurr))
						toDel = modifyHandle - 1;
					if (modifyHandle < points.Count - 2 && fcParent.mergePoints(ptNext, ptCurr))
						toDel = modifyHandle;

					if (toDel != -1)
					{
						points.RemoveAt(toDel);
						points.RemoveAt(toDel);
						segmentCount -= 2;
						alignCascadingSegments(toDel);
						updateEndPtsPrp();
						if (subordinateGroup != null)
							subordinateGroup.onArrowSplit(false, toDel, 2);
					}
				}
			}

			if (reflexive && (style == ArrowStyle.Polyline && segmentCount == 1 ||
				style == ArrowStyle.Cascading && segmentCount == 2))
			{
				setReflexive();
				if (subordinateGroup != null)
					subordinateGroup.endModification();
				cycleProtect = false;
				objNewDest = null;
				return;
			}

			if (shouldRoute && autoRoute && fcParent.rerouteArrow(this))
				doRoute();

			updateArrowHeads();
			if (subordinateGroup != null)
				subordinateGroup.endModification();
			resetCrossings();
			updateText();

			fcParent.fireObjModified(this, end, modifyHandle);
			cycleProtect = false;
			objNewDest = null;
		}

		internal override void modifyTranslate(float x, float y, bool arrows)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.modifyTranslate(x, y, arrows);

			for (int i = 0; i < points.Count; ++i)
				points[i] = new PointF(points[i].X+x, points[i].Y+y);

			headTemplates[(int)arrowBase].recalcArrowHead(ahBase, points[1], points[0]);
			headTemplates[(int)arrowHead].recalcArrowHead(ahHead, points[points.Count-2], points[points.Count-1]);
			resetCrossings();
			updateText();

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));

			cycleProtect = false;
		}

		internal override void cancelModify(InteractionState ist)
		{
			objNewDest = null;
			if (cycleProtect) return;
			cycleProtect = true;

			base.cancelModify(ist);

			if (savedSegments != null)
			{
				restoreSegments(savedSegments);
				savedSegments = null;
			}

			updateArrowHeads();

			if (subordinateGroup != null)
				subordinateGroup.cancelModification(ist);
			resetCrossings();
			updateText();

			cycleProtect = false;
		}

		#endregion

		#region drawing

		// ************ drawing ************

		internal void drawArrowSegments(Graphics g,
			System.Drawing.Pen p, System.Drawing.Brush b,
			System.Drawing.Pen pHeads, bool shadow, bool custom)
		{
			PointCollection pts = points;
			float xoff = shadow ? ShadowOffsetX : 0;
			float yoff = shadow ? ShadowOffsetY : 0;

			// offset if drawing shadow
			if (shadow)
			{
				pts = new PointCollection(points.Count);
				for (int i = 0; i < pts.Count; ++i)
				{
					pts[i] = new PointF(
						points[i].X + xoff, points[i].Y + yoff);
				}
			}

			if (custom && (customDraw == CustomDraw.Full ||
				(customDraw == CustomDraw.ShadowOnly && shadow)))
			{
				// call the custom draw function
				fcParent.drawArrow(g, this, shadow, pts);
			}
			else
			{
				// draw the arrow's line
				if (style == ArrowStyle.Bezier)
				{
					g.DrawBeziers(p, pts.getArray());
				}
				else
				{
					float mm = Constants.getMillimeter(fcParent.MeasureUnit);
					float pathThresh = mm / 3;
					if (!drawCrossings || fcParent.ArrowCrossings ==
						MindFusion.FlowChartX.ArrowCrossings.Straight)
					{
						if (fcParent.RoundedArrows)
						{
							DrawRoundedPolyline(g, p, pts.getArray(),
								fcParent.RoundedArrowsRadius, true, pathThresh, null);
						}
						else
						{
							g.DrawLines(p, pts.getArray());
						}
					}
					else
					{
						float crad = fcParent.CrossingRadius;

						ArrowCrossings crossings = getCrossings();
						object startPoint = null;

						GraphicsPath gpath = pen.Width > pathThresh ?
							new GraphicsPath() : null;

						for (int i = 0; i < segmentCount; ++i)
						{
							PointCollection pc = crossings.segmentCrossings[i] as PointCollection;
							if (pc.Count > 0)
							{
								for (int j = 0; j < pc.Count - 1; ++j)
								{
									PointF pt1 = pc[j];
									PointF pt2 = pc[j + 1];
									pt1.X += xoff; pt1.Y += yoff;
									pt2.X += xoff; pt2.Y += yoff;

									if (startPoint != null)
										pt1 = (PointF)startPoint;

									startPoint = null;

									if (j % 2 == 0)
									{
										// The subsegment between two crossings or
										// between a crossing and a segment end-point
										if (fcParent.RoundedArrows)
										{
											// Check if this is the last subsegment
											// in this segment. If that is the case,
											// and this is not the last segment of the
											// arrow, draw rounded arrow
											if (j == pc.Count - 2 && i != segmentCount - 1)
											{
												// The third point in the poly is
												// the second point of the next segment
												// if it does not have crossings, or
												// the second point of the crossings array
												int ni = i + 2;
												PointF next = pts[ni];
												while (Math.Abs(next.X - pt2.X) + Math.Abs(next.Y - pt2.Y) < 0.00001f)
												{
													ni++;
													if (ni == pts.Count)
														break;

													next = pts[ni];
												}

												if (ni == pts.Count)
												{
													if (gpath != null)
														gpath.AddLine(pt1, pt2);
													else
														g.DrawLine(p, pt1, pt2);
												}
												else
												{
													PointCollection nextPc = crossings.segmentCrossings[ni - 1] as PointCollection;
													if (nextPc.Count > 2)
													{
														next = nextPc[1];
														next.X += xoff;
														next.Y += yoff;
													}

													PointF[] triPoints = new PointF[]
												{
													pt1, pt2, next
												};

													startPoint = DrawRoundedPolyline(g, p, triPoints,
														fcParent.RoundedArrowsRadius, false, pathThresh, gpath);
												}
											}
											else
											{
												if (gpath != null)
													gpath.AddLine(pt1, pt2);
												else
													g.DrawLine(p, pt1, pt2);
											}
										}
										else
										{
											if (gpath != null)
												gpath.AddLine(pt1, pt2);
											else
												g.DrawLine(p, pt1, pt2);
										}
									}
									else
									{
										if (fcParent.ArrowCrossings ==
											MindFusion.FlowChartX.ArrowCrossings.Arcs)
										{
											float rad = Utilities.Distance(pt1, pt2) / 2;

											float aa = 0;
											float rr = 0;
											Geometry.Geometry2D.Convert.DekartToPolar(
												pt1, pt2, ref aa, ref rr);

											PointF[] centers = new PointF[] { PointF.Empty, PointF.Empty };
											Geometry.Geometry2D.Convert.PolarToDekart(
												pt1, aa, crad, ref centers[0]);
											Geometry.Geometry2D.Convert.PolarToDekart(
												pt1, aa, 2 * rad - crad, ref centers[1]);

											PointF[] startPts = new PointF[] { pt1, PointF.Empty };
											PointF[] endPts = new PointF[] { PointF.Empty, pt2 };
											Geometry.Geometry2D.Convert.PolarToDekart(
												pt1, aa, 2 * crad, ref endPts[0]);
											Geometry.Geometry2D.Convert.PolarToDekart(
												pt1, aa, 2 * rad - 2 * crad, ref startPts[1]);

											float angle = aa;
											if (angle < 90)
												angle += 180;

											RectangleF rc = RectangleF.FromLTRB(
												centers[0].X - crad,
												centers[0].Y - crad,
												centers[0].X + crad,
												centers[0].Y + crad);

											float ded = 0 * 90;
											if (aa < 90)
												ded = 90 - ded;

											float start = 180 - angle - ded;
											float sweep = -90;
											if (aa < 90)
											{
												start += sweep;
												sweep = -sweep;
											}

											if (gpath != null)
												gpath.AddArc(rc, start, sweep);
											else
												g.DrawArc(p, rc, start, sweep);

											PointF p1 = PointF.Empty;
											PointF p2 = PointF.Empty;
											Geometry.Geometry2D.Convert.PolarToDekart(
												centers[0], angle - 90, crad, ref p1);
											Geometry.Geometry2D.Convert.PolarToDekart(
												centers[1], angle - 90, crad, ref p2);

											if (gpath != null)
												gpath.AddLine(p1, p2);
											else
												g.DrawLine(p, p1, p2);

											rc = RectangleF.FromLTRB(
												centers[1].X - crad,
												centers[1].Y - crad,
												centers[1].X + crad,
												centers[1].Y + crad);

											ded = 1 * 90;
											if (aa < 90)
												ded = 90 - ded;

											start = 180 - angle - ded;
											sweep = -90;
											if (aa < 90)
											{
												start += sweep;
												sweep = -sweep;
											}

											if (gpath != null)
												gpath.AddArc(rc, start, sweep);
											else
												g.DrawArc(p, rc, start, sweep);

										}
										else
										{
											// Start new figure in the graph,
											// thus preventing the graph
											// to automatically connect broken
											// lines and losing break-offs
											if (gpath != null)
												gpath.StartFigure();
										}
									}
								}
							}
							else
							{
								if (gpath != null)
									gpath.AddLine(pts[i], pts[i+1]);
								else
									g.DrawLine(p, pts[i], pts[i+1]);
							}
						}

						if (gpath != null)
						{
							gpath.Flatten(new Matrix(), 0.05f);
							g.DrawPath(p, gpath);
							gpath.Dispose();
						}
					}
				}

				// draw arrowheads, intermediate are skipped for asBezier arrows
				ahBase.draw(g, pHeads, b, xoff, yoff);
				if (style != ArrowStyle.Bezier && arrowInterm != ArrowHead.None)
				{
					for (int i = 0; i < points.Count - 1; i++)
					{
						PointF pt1 = pts[i];
						PointF pt2 = new PointF((pt1.X + pts[i+1].X)/2, (pt1.Y + pts[i+1].Y)/2);
					
						headTemplates[(int)arrowInterm].recalcArrowHead(ahInterm, pt1, pt2);
						ahInterm.draw(g, pHeads, b, 0, 0);
					}
				}
				ahHead.draw(g, pHeads, b, xoff, yoff);

				// additional custom draw type
				if (custom && customDraw == CustomDraw.Additional)
					fcParent.drawArrow(g, this, false, pts);
			}
		}

		/// <summary>
		/// Draws a rounded polyline.
		/// 
		/// Note: The closeUp parameter indicates whether to draw the last segment.
		/// </summary>
		/// <returns>
		/// Returns the end point of the drawn curve.
		/// </returns>
		private static PointF DrawRoundedPolyline(Graphics g,
			System.Drawing.Pen p, PointF[] points, float radius, bool closeUp,
			float pathThreshold, GraphicsPath aPath)
		{
			// If the radius is too small, draw a normal polyline
			if (radius < 0.00001f)
			{
				g.DrawLines(p, points);
				return points[points.Length - 1];
			}

			// If the line consists of only two points, no rounding is performed
			if (points.Length <= 2)
			{
				g.DrawLines(p, points);
				return points[points.Length - 1];
			}

			GraphicsPath gpath = aPath != null ? aPath :
				(p.Width > pathThreshold ? new GraphicsPath() : null);

			PointF c, k1, k2, l1, l2;
			PointF prev = points[0];
			for (int i = 0; i < points.Length - 2; i++)
			{
				c = points[i + 1];

				k1 = points[i];
				k2 = points[i + 1];
				l1 = points[i + 1];
				l2 = points[i + 2];

				// If the two points coincide, get the next not-coinciding point
				bool continueLoop = false;
				while (Math.Abs(k2.X - l2.X) + Math.Abs(k2.Y - l2.Y) < 0.00001f)
				{
					i++;

					if (i >= points.Length - 2)
					{
						continueLoop = true;
						break;
					}

					l1 = points[i + 1];
					l2 = points[i + 2];
				}

				if (continueLoop)
					continue;

				// Find the angle between (a1, a2) and (b1, b2)
				float a1 = 0, r1 = 0;
				float a2 = 0, r2 = 0;
				Geometry.Geometry2D.Convert.DekartToPolar(c, k1, ref a1, ref r1);
				Geometry.Geometry2D.Convert.DekartToPolar(c, l2, ref a2, ref r2);

				// Ensure a1 and a2 are both positive,
				// and a2 is larger than a1
				while (a1 < 0)
					a1 += 360;
				while (a2 < 0)
					a2 += 360;
				if (a2 < a1)
					a2 += 360;

				float a = a2 - a1;

				// If the angle is 0 or 180, there is no need to draw an arc
				if (a == 0 || a == 180)
					continue;

				bool swap = false;
				if (a > 180)
				{
					// The angle between a1 and a2 is not keen, so
					// swap the two segments
					PointF temp1 = k1;
					PointF temp2 = k2;
					k1 = l1;
					k2 = l2;
					l1 = k1;
					l2 = k2;

					float temp = a1;
					a1 = a2;
					a2 = temp;

					temp = r1;
					r1 = r2;
					r2 = temp;

					a = a2 - a1;
					while (a < 0)
						a += 360;

					swap = true;
				}

				// Calculate the elongation of the tangent point from the intersection point
				float rr = radius;
				float rrSin = (float)Math.Sin(Radians(a / 2));
				float xxSin = (float)Math.Sin(Radians(90 - a / 2));
				float xx = xxSin * rr / rrSin;

				// Ensure that xx is less than the half the length of the corresponding segment
				float len = Math.Min(r1, r2);

				if (xx > Math.Min(len / 3, 10))
				{
					xx = Math.Min(len / 3, 10);
					rr = rrSin * xx / xxSin;
				}

				// Calculate the intersection points
				PointF iK = new PointF();
				PointF iL = new PointF();

				Geometry.Geometry2D.Convert.PolarToDekart(c, a1, xx, ref iK);
				Geometry.Geometry2D.Convert.PolarToDekart(c, a2, xx, ref iL);

				// Calculate the center of the inscribed circle
				float xc = (float)Math.Sqrt(Math.Pow(xx, 2) + Math.Pow(rr, 2));
				PointF center = new PointF();

				Geometry.Geometry2D.Convert.PolarToDekart(c, a1 + a / 2, xc, ref center);

				// Calculate the angles of the intersection points, relative to
				// the center of the inscribed circle
				float iKa = 0, iKr = 0;
				float iLa = 0, iLr = 0;

				Geometry.Geometry2D.Convert.DekartToPolar(center, iK, ref iKa, ref iKr);
				Geometry.Geometry2D.Convert.DekartToPolar(center, iL, ref iLa, ref iLr);

				while (Math.Abs(iLa - iKa) > 180)
				{
					if (iLa < iKa)
						iLa += 360;
					else
						iKa += 360;
				}

				if (iLa < iKa)
				{
					float temp = iLa;
					iLa = iKa;
					iKa = temp;
				}

				if (swap)
				{
					PointF temp = iK;
					iK = iL;
					iL = temp;
				}

				if (gpath != null)
					gpath.AddLine(prev, iK);
				else
					g.DrawLine(p, prev, iK);
				if (rr > 0)
				{
					float start = 360 - iKa;
					float sweep = -(iLa - iKa);
					if (!swap)
					{
						start = start + sweep;
						sweep = -sweep;
					}

					if (gpath != null)
						gpath.AddArc(new RectangleF(center.X - rr, center.Y - rr, rr * 2, rr * 2), start, sweep);
					else
						g.DrawArc(p, new RectangleF(center.X - rr, center.Y - rr, rr * 2, rr * 2), start, sweep);
				}

				prev = iL;
			}

			if (closeUp)
			{
				if (gpath != null)
					gpath.AddLine(prev, points[points.Length - 1]);
				else
					g.DrawLine(p, prev, points[points.Length - 1]);
			}

			if (gpath != null && aPath == null)
			{
				gpath.Flatten(new Matrix(), 0.05f);
				g.DrawPath(p, gpath);
				gpath.Dispose();
			}

			return prev;
		}

		/// <summary>
		/// Converts the values expressed in degrees to radians.
		/// </summary>
		private static float Radians(float degrees)
		{
			return (float)(degrees * Math.PI / 180);
		}

		public override void Draw(Graphics g, bool shadow)
		{
			if (invisible) return;
			if (fcParent.NowPrinting && !Printable) return;

			System.Drawing.Pen p = null;
			System.Drawing.Brush b = null;
			System.Drawing.Pen pHeads = null;
			
			if (shadow)
			{
				b = new System.Drawing.SolidBrush(ShadowColor);
				p = this.Pen.CreateGDIPen();
				p.Brush = b;
				pHeads = this.HeadPen.CreateGDIPen();
				pHeads.Brush = b;
			}
			else
			{
				p = this.Pen.CreateGDIPen();
				b = this.Brush.CreateGDIBrush(rect);
				pHeads = this.HeadPen.CreateGDIPen();
			}

			// draw segment lines and arrowheads
			drawArrowSegments(g, p, b, pHeads, shadow, true);

			if (!shadow && customDraw != CustomDraw.Full)
			{
				// draw arrow's text
				if (text.Length > 0 && fcParent.RenderOptions.EnableText)
					drawText(g);
			}

			pHeads.Dispose();
			p.Dispose();
			b.Dispose();
		}

		private void drawText(Graphics g)
		{
			// DrawString crashes with a too small PageScale
			if (g.PageScale < 0.01) return;

			System.Drawing.Brush brText = new System.Drawing.SolidBrush(textColor);
			StringFormat sf = new StringFormat(StringFormat.GenericDefault);
			sf.Alignment = StringAlignment.Center;
			sf.LineAlignment = StringAlignment.Far;

			float x, y;

			textSize = g.MeasureString(text, Font);

			if (textStyle == ArrowTextStyle.Rotate)
			{
				// Find the segment near which the text will be drawn
				int pt = points.Count / 2 - 1;
				int jump = 1;
				int ptBest = pt;
				float r = 0, a = 0, rBest = 0;
				if (style != ArrowStyle.Bezier)
				{
					if (style == ArrowStyle.Cascading ||
						(style == ArrowStyle.Polyline && segmentCount % 2 == 1))
					{
						do
						{
							MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
								points[pt], points[pt + 1], ref a, ref r);
							if (r > rBest)
							{
								rBest = r;
								ptBest = pt;
							}
							pt = pt + jump;
							jump = -Math.Sign(jump) * (Math.Abs(jump) + 1);
							if(pt < 0 || pt >= points.Count - 1)
								break;
						}
						while(textSize.Width > r);
						pt = ptBest;
						x = (points[pt].X + points[pt + 1].X) / 2;
						y = (points[pt].Y + points[pt + 1].Y) / 2;
					}
					else
					{
						pt = pt + 1;
						x = points[pt].X;
						y = points[pt].Y;
					}
				}
				else
				{
					if(segmentCount % 2 == 1)
					{
						pt = (pt + 1) / 4;
						PointF ptMid = Utilities.GetBezierPt(points, pt, 0.5f);
						x = ptMid.X;
						y = ptMid.Y;
					}
					else
					{
						pt = (pt + 1) / 4;
						PointF ptMid = points[(pt + 1) * 3];
						x = ptMid.X;
						y = ptMid.Y;
					}
				}

						
				// Find out the angle
				if (style != ArrowStyle.Bezier)
				{
					if( style == ArrowStyle.Cascading ||
						(style == ArrowStyle.Polyline && segmentCount % 2 == 1))
					{
						PointF pt1 = points[pt];
						PointF pt2 = points[pt + 1];
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
					else
					{
						float r1 = 0, a1 = 0;
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							points[pt - 1], points[pt], ref a, ref r);
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							points[pt], points[pt + 1], ref a1, ref r1);
						a = (a + a1) / 2;
					}
				}
				else
				{
					if (segmentCount % 2 == 1)
					{
						PointF pt1 = Utilities.GetBezierPt(points, pt, 0.45f);
						PointF pt2 = Utilities.GetBezierPt(points, pt, 0.55f);
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
					else
					{
						PointF pt1 = Utilities.GetBezierPt(points, pt, 0.95f);
						PointF pt2 = Utilities.GetBezierPt(points, pt + 1, 0.05f);
						MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
							pt1, pt2, ref a, ref r);
					}
				}

				a = 180 - a;
				if (a > 90 && a < 270)
					a -= 180;
					
				// Draw the text
				GraphicsState state = g.Save();

				g.TranslateTransform(x, y);
				g.RotateTransform(a);
				g.TranslateTransform(-x, -y);
				g.DrawString(text, Font, brText, new PointF(x, y), sf);

				g.Restore(state);
			}
			else if (textStyle == ArrowTextStyle.Center)
			{
				if(points.Count % 2 == 1)
				{
					x = points[points.Count / 2].X;
					y = points[points.Count / 2].Y;
				}
				else
				{
					x = points[points.Count / 2 - 1].X + points[points.Count / 2].X;
					y = points[points.Count / 2 - 1].Y + points[points.Count / 2].Y;
					x /= 2;
					y /= 2;
				}

				g.DrawString(text, Font, brText, new PointF(x, y), sf);
			}
			else if (textStyle == ArrowTextStyle.OverLongestSegment)
			{
				// find the center point of longest segment
				int longest = getLongestSegment();
				PointF center = getSegmentCenter(longest);

				// see how much space the text requires
				StringFormat cf = new StringFormat();
				cf.Alignment = StringAlignment.Center;
				cf.LineAlignment = StringAlignment.Center;
				SizeF size = g.MeasureString(text, Font);
				size.Width += Constants.getMillimeter(fcParent.MeasureUnit);
				size.Height += Constants.getMillimeter(fcParent.MeasureUnit);

				// fill using the background brush
				System.Drawing.Brush brush = fcParent.BackBrush.CreateGDIBrush(rect);
				g.FillRectangle(brush, center.X - size.Width / 2,
					center.Y - size.Height /2, size.Width, size.Height);
				brush.Dispose();

				// draw the text
				g.DrawString(text, Font, brText, center, cf);
			}
			else if (textStyle == ArrowTextStyle.Follow)
			{
				// Draw the text
				if (textLayout != null)
				{
					for (int i = 0; i < textLayout.Strings.Count; i++)
					{
						string part = textLayout.Strings[i] as string;
						RectangleF rect = (RectangleF)textLayout.Rectangles[i];
						float angle = (float)textLayout.Angles[i];

						if (part.Length == 0)
							continue;

						GraphicsState state = g.Save();

						g.TranslateTransform(rect.X, rect.Y);
						g.RotateTransform(angle);
						g.TranslateTransform(-rect.X, -rect.Y);

						g.DrawString(part, Font, brText, rect.X, rect.Y);

						g.Restore(state);
					}
				}
			}

			brText.Dispose();
		}

		internal override void drawSelHandles(Graphics g, Color color)
		{
			if (selStyle == HandlesStyle.Custom)
			{
				// let applications custom draw the selection handles
				fcParent.fireDrawSelHandles(g, this);
				return;
			}

			if (selStyle == HandlesStyle.DashFrame)
			{
				System.Drawing.Brush transp =
					new System.Drawing.SolidBrush(Color.Transparent);
				System.Drawing.Pen dash = new System.Drawing.Pen(color, 0);
				dash.DashStyle = DashStyle.Dash;

				// drawing with a dashed pen sometimes throws a generic exception
				// in GDIPLUS.DLL, so catch everything or everyone will blame us;
				try
				{
					drawArrowSegments(g, dash, transp, dash, false, false);
				}
				catch (Exception)
				{
					// sometimes the exception says 'out of memory'
					GC.Collect();
				}

				transp.Dispose();
				dash.Dispose();
			}

			// select pen and brush for drawing the selection handles
			System.Drawing.Pen pen = new System.Drawing.Pen(Color.Black, 0);
			System.Drawing.Brush brush = new System.Drawing.SolidBrush(color);

			// draw the handles
			for (int i = 0; i < points.Count; ++i)
			{
				RectangleF rc = RectangleF.FromLTRB(
					points[i].X - fcParent.SelHandleSize/2,
					points[i].Y - fcParent.SelHandleSize/2,
					points[i].X + fcParent.SelHandleSize/2,
					points[i].Y + fcParent.SelHandleSize/2);

				if (i%3 > 0 || style != ArrowStyle.Bezier)
				{
					g.FillRectangle(brush, rc.X, rc.Y, rc.Width, rc.Height);
					g.DrawRectangle(pen, rc.X, rc.Y, rc.Width, rc.Height);
				}
				else
				{
					g.FillEllipse(brush, rc.X, rc.Y, rc.Width, rc.Height);
					g.DrawEllipse(pen, rc.X, rc.Y, rc.Width, rc.Height);
				}
			}

			// cleanup
			brush.Dispose();
			pen.Dispose();
		}

		public override bool Visible
		{
			get { return base.Visible; }
			set
			{
				if (base.Visible != value)
				{
					base.Visible = value;
					resetCrossings();
				}
			}
		}

		public bool DrawCrossings
		{
			get { return drawCrossings; }
			set
			{
				if (drawCrossings != value)
				{
					drawCrossings = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private bool drawCrossings;

		#endregion

		#region structural

		public bool IsConnected
		{
			get
			{
				if (Origin == null || Origin is DummyNode)
					return false;
				if (Destination == null || Destination is DummyNode)
					return false;

				return true;
			}
		}

		/// <summary>
		/// The object from which the arrow is coming out
		/// </summary>
		public Node Origin
		{
			get
			{
				if (orgnLink == null) return null;
				return orgnLink.getNode();
			}
			set
			{
				if (orgnLink != null)
				{
					orgnLink.removeArrowFromObj();
				}
				orgnLink = value.createLink(this, value.getCenter(), false);
				orgnLink.addArrowToObj();

				// place the end points of the arrow at their respective object edge
				points[0] = orgnLink.getInitialPt();
				if (!orgnLink.objsIntersect(destLink))
				{
					points[0] = orgnLink.getIntersection(points[0], points[points.Count-1]);
					if (destAnchor == -1)
						points[points.Count-1] = destLink.getIntersection(points[points.Count-1], points[0]);
				}

				points[0] = orgnLink.getNode().getAnchor(points[0], this, false, ref orgnAnchor);
				if (destAnchor == -1)
					points[points.Count-1] = destLink.getNode().getAnchor(points[points.Count-1], this, true, ref destAnchor);

				// set reflexive arrow position & form
				reflexive = orgnLink.sameNode(destLink);
				if (reflexive)
				{
					setReflexive();
				}
				else
				{
					// compute the arrow points
					updatePoints(points[points.Count-1]);
					if (dynamic)
						updatePosFromOrgAndDest(true);
				}

				rectFromPoints();
				UpdateFromPoints();

				doRoute();
			}
		}

		private Link orgnLink;

		/// <summary>
		/// The destination the arrow is pointing to
		/// </summary>
		public Node Destination
		{
			get
			{
				if (destLink == null) return null;
				return destLink.getNode();
			}
			set
			{
				if (destLink != null)
				{
					destLink.removeArrowFromObj();
				}
				destLink = value.createLink(this, value.getCenter(), true);
				destLink.addArrowToObj();

				// place the end points of the arrow at their respective object edge
				points[points.Count-1] = destLink.getInitialPt();
				if (!orgnLink.objsIntersect(destLink))
				{
					if (orgnAnchor != -1)
						points[0] = orgnLink.getIntersection(points[0], points[points.Count-1]);
					points[points.Count-1] = destLink.getIntersection(points[points.Count-1], points[0]);
				}

				if (orgnAnchor != -1)
					points[0] = orgnLink.getNode().getAnchor(points[0], this, false, ref orgnAnchor);
				points[points.Count-1] = destLink.getNode().getAnchor(points[points.Count-1], this, true, ref destAnchor);

				// set reflexive arrow position & form
				reflexive = orgnLink.sameNode(destLink);
				if (reflexive)
				{
					setReflexive();
				}
				else
				{
					// compute the arrow points
					updatePoints(points[points.Count-1]);
					if (dynamic)
						updatePosFromOrgAndDest(true);
				}

				rectFromPoints();
				UpdateFromPoints();

				doRoute();
			}
		}

		private Link destLink;

		public int OrgnIndex
		{
			get
			{
				if (orgnLink is TableLink)
				{
					TableLink tlink = (TableLink)orgnLink;
					return tlink.getRow();
				}
				return orgnAnchor;
			}
			set
			{
				if (orgnLink is TableLink)
				{
					Table tbl = (Table)Origin;
					orgnLink.removeArrowFromObj();
					orgnLink = tbl.createLink(this, false, value);

					// place the end points of the arrow at their respective object edge
					points[0] = orgnLink.getInitialPt();
					if (!orgnLink.objsIntersect(destLink))
						points[0] = orgnLink.getIntersection(points[0], points[points.Count-1]);

					points[0] = orgnLink.getNode().getAnchor(points[0], this, false, ref orgnAnchor);

					// compute the arrow points
					if (dynamic)
						updatePosFromOrgAndDest(true);

					rectFromPoints();

					// add the arrow to the objects arrow lists
					orgnLink.addArrowToObj();
					orgnLink.saveEndRelative();

					doRoute();
					UpdateFromPoints();
					fcParent.Invalidate();

					return;
				}
				OrgnAnchor = value;
			}
		}

		public int DestIndex
		{
			get
			{
				if (destLink is TableLink)
				{
					TableLink tlink = (TableLink)destLink;
					return tlink.getRow();
				}
				return destAnchor;
			}
			set
			{
				if (destLink is TableLink)
				{
					Table tbl = (Table)Destination;
					destLink.removeArrowFromObj();
					destLink = tbl.createLink(this, true, value);

					// place the end points of the arrow at their respective object edge
					points[points.Count-1] = destLink.getInitialPt();
					if (!orgnLink.objsIntersect(destLink))
						points[points.Count-1] = destLink.getIntersection(points[points.Count-1], points[0]);

					points[points.Count-1] = destLink.getNode().getAnchor(points[points.Count-1], this, true, ref destAnchor);

					if (dynamic)
						updatePosFromOrgAndDest(true);

					rectFromPoints();

					// add the arrow to the objects arrow lists
					destLink.addArrowToObj();
					destLink.saveEndRelative();

					doRoute();
					UpdateFromPoints();
					fcParent.Invalidate();

					return;
				}
				DestAnchor = value;
			}
		}

		private int orgnAnchor;
		private int destAnchor;

		internal void setOrgnAnchor(int value)
		{
			orgnAnchor = value;
		}

		internal void setDestAnchor(int value)
		{
			destAnchor = value;
		}

		public int OrgnAnchor
		{
			get { return orgnAnchor; }
			set
			{
				if (value != orgnAnchor)
				{
					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
					orgnAnchor = -1;

					PointF anchorPos = new PointF(0, 0);
					if (orgnLink.getAnchorPos(value, ref anchorPos))
					{
						points[0] = anchorPos;
						orgnAnchor = value;
						UpdateFromPoints();
					}

					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
				}
			}
		}

		public int DestAnchor
		{
			get { return destAnchor; }
			set
			{
				if (value != destAnchor)
				{
					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
					destAnchor = -1;

					PointF anchorPos = new PointF(0, 0);
					if (destLink.getAnchorPos(value, ref anchorPos))
					{
						points[points.Count-1] = anchorPos;
						destAnchor = value;
						UpdateFromPoints();
					}

					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
				}
			}
		}

		/// <summary>
		/// The number of the arrow segments
		/// </summary>
		public short SegmentCount
		{
			get
			{
				return segmentCount;
			}
			set
			{
				if (segmentCount != value)
				{
					// Route arrows have automatic segmentation
					if (autoRoute)
						if (!reflexive) return;
					if (value <= 0) return;
					if (value == 1 && style == ArrowStyle.Cascading) return;

					if (reflexive && value < 3 &&
						style != ArrowStyle.Bezier) return;

					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
					setSegments(value);
					arrangePoints(Anchoring.Keep);
					resetCrossings();
					updateText();

					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Invoked internally to set the segment count
		/// unconditionally (i.e. when loading from XML).
		/// </summary>
		internal short InternalSegmentCount
		{
			set
			{
				segmentCount = value;
				setSegments(value);
			}
		}

		private short segmentCount;

		private PointCollection points;
		internal PointCollection Points
		{
			get { return points; }
		}

		public PointCollection ControlPoints
		{
			get { return points; }
		}

		private void onShapeChanged()
		{
			//compute the arrow points
			updateArrowHeads();

			orgnLink.saveEndRelative();
			destLink.saveEndRelative();

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));

			resetCrossings();
			updateText();
		}

		public void UpdateFromPoints()
		{
			onShapeChanged();
			fcParent.invalidate(getRepaintRect(false));

			if (style == ArrowStyle.Cascading)
			{
				if (cascadeOrientation == Orientation.Auto)
					cascadeStartHorizontal = points[0].X != points[1].X;
				else if (cascadeOrientation == Orientation.Horizontal)
					cascadeStartHorizontal = true;
				else
					cascadeStartHorizontal = false;
			}
		}

		internal PointF getStartPt() { return points[0]; }
		internal PointF getEndPt() { return points[points.Count-1]; }

		private bool reflexive;

		internal Link getDestLink()
		{
			return destLink;
		}

		internal Link getOrgnLink()
		{
			return orgnLink;
		}

		private void setReflexive()
		{
			Style = ArrowStyle.Bezier;
			SegmentCount = 1;

			PointF pt = orgnLink.getNode().getTopMiddleIntr();
			points[0] = points[3] = pt;

			points[1] = new PointF(
				pt.X - Constants.getReflWth2(fcParent.MeasureUnit),
				pt.Y - Constants.getReflHth(fcParent.MeasureUnit));

			points[2] = new PointF(
				pt.X + Constants.getReflWth2(fcParent.MeasureUnit),
				pt.Y - Constants.getReflHth(fcParent.MeasureUnit));

			updateArrowHeads();
			resetCrossings();
			updateText();
		}

		SizeF textSize;

		private void setSegments(short segments)
		{
			int ctrlPoints = 2;

			switch (style)
			{
				case ArrowStyle.Bezier:
					ctrlPoints = segments*3 + 1;
					break;
				case ArrowStyle.Polyline:
					ctrlPoints = segments + 1;
					break;
				case ArrowStyle.Cascading:
					ctrlPoints = segments + 1;
					break;
			}

			points = new PointCollection(ctrlPoints);
			segmentCount = segments;

			if (subordinateGroup != null)
				subordinateGroup.onSegmentsChanged();
		}

		internal bool isReflexive()
		{
			return reflexive;
		}

		internal bool bothEndsSelected()
		{
			if (Origin.Selected && Destination.Selected)
				return true;

			return false;
		}

		internal void translatePoints(float x, float y)
		{
			base.modifyTranslate(x, y, false);

			for (int i = 0; i < points.Count; ++i)
				points[i] = new PointF(points[i].X + x, points[i].Y + y);

			headTemplates[(int)arrowBase].recalcArrowHead(ahBase, points[1], points[0]);
			headTemplates[(int)arrowHead].recalcArrowHead(ahHead, points[points.Count-2], points[points.Count-1]);
			resetCrossings();
			updateText();

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));
		}

		#endregion

		#region behavior

		internal override void expand(bool incm)
		{
			if (!invisible) return;

			fcParent.UndoManager.onExpandItem(this);

			invisible = false;
			if (isReflexive()) return;

			if (incm)
				orgnLink.getNode().expand(incm);
			else
				destLink.getNode().expand(incm);

			// propagate the expansion through the attached items
			if (SubordinateGroup != null)
				SubordinateGroup.expand(incm);
		}

		internal override void collapse(bool incm)
		{
			if (invisible) return;

			fcParent.UndoManager.onExpandItem(this);

			invisible = true;
			fcParent.Selection.RemoveObject(this);
			if (isReflexive()) return;

			if (incm)
				orgnLink.getNode().collapse(incm);
			else
				destLink.getNode().collapse(incm);

			// propagate the collapse through the attached items
			if (SubordinateGroup != null)
				SubordinateGroup.collapse(incm);
		}

		/// <summary>
		/// Defines how the end-points move when modifying a connected box
		/// </summary>
		public bool Dynamic
		{
			get
			{
				return dynamic;
			}
			set
			{
				if (dynamic != value)
				{
					dynamic = value;
					fcParent.setDirty();
				}
			}
		}

		private bool dynamic;

		public bool RetainForm
		{
			get
			{
				return retainForm;
			}
			set
			{
				if(retainForm != value)
				{
					retainForm = value;
					fcParent.setDirty();
				}
			}
		}

		private bool retainForm;

		/// <summary>
		/// Defines the orientation of the first segment of perpendicular arrows
		/// </summary>
		public Orientation CascadeOrientation
		{
			get
			{
				return cascadeOrientation;
			}
			set
			{
				if (cascadeOrientation != value)
				{
					cascadeOrientation = value;
					fcParent.setDirty();
					if (style == ArrowStyle.Cascading)
					{
						fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
						arrangePoints(Anchoring.Keep);
						fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
					}
				}
			}
		}

		internal bool CascadeStartHorizontal
		{
			get { return cascadeStartHorizontal; }
			set { cascadeStartHorizontal = value; }
		}

		internal bool CascadeEndHorizontal
		{
			get
			{
				int l = points.Count - 1;
				if ((cascadeStartHorizontal && (l%2 != 0)) || (!cascadeStartHorizontal && (l%2 == 0)))
					return true;
				else
					return false;
			}
		}

		private Orientation cascadeOrientation;
		private bool cascadeStartHorizontal;

		private ArrowSegmentsState saveSegments()
		{
			ArrowSegmentsState state = new ArrowSegmentsState();
			state.controlPoints = points.Clone();
			state.segmentCount = segmentCount;
			state.cascadeStartHorizontal = cascadeStartHorizontal;
			return state;
		}

		private void restoreSegments(ArrowSegmentsState state)
		{
			points = state.controlPoints.Clone();
			segmentCount = state.segmentCount;
			cascadeStartHorizontal = state.cascadeStartHorizontal;
		}

		private ArrowSegmentsState savedSegments;

		public bool AllowMoveStart
		{
			get { return allowMoveStart; }
			set
			{
				if (allowMoveStart != value)
				{
					allowMoveStart = value;
					fcParent.setDirty();
				}
			}
		}

		public bool AllowMoveEnd
		{
			get { return allowMoveEnd; }
			set
			{
				if (allowMoveEnd != value)
				{
					allowMoveEnd = value;
					fcParent.setDirty();
				}
			}
		}

		private bool allowMoveStart;
		private bool allowMoveEnd;

		#endregion

		#region appearance

		// ************ mouse pointers ************

		internal override Cursor getCannotDropCursor()
		{
			return fcParent.CurArrowCannotCreate;
		}

		internal override Cursor getCanDropCursor()
		{
			return fcParent.CurArrowEnd;
		}

		/// <summary>
		/// Defines the shape of the arrow
		/// </summary>
		public ArrowStyle Style
		{
			get
			{
				return style;
			}
			set
			{
				if (style != value)
				{
					if (this.isReflexive() && this.segmentCount < 3 &&
						value != ArrowStyle.Bezier) return;

					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));

					style = value;

					if (style == ArrowStyle.Cascading && segmentCount == 1)
						setSegments(2);
					else
						setSegments(segmentCount);
					arrangePoints(Anchoring.Keep);

					doRoute();
					resetCrossings();
					updateText();

					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		internal bool SilentAutoRoute
		{
			set { autoRoute = value; }
		}

		private ArrowStyle style;

		/// <summary>
		/// The arrow is painted in this color
		/// </summary>
		public Color PenColor
		{
			get
			{
				if (pen != null)
					return pen.Color;
				return penColor;
			}
			set
			{
				if (!penColor.Equals(value))
				{
					penColor = value;
					pen.Color = penColor;

					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private Color penColor;

		public MindFusion.FlowChartX.Pen HeadPen
		{
			get
			{
				return headPen;
			}
			set
			{
				headPen = value;

				fcParent.setDirty();
				fcParent.invalidate(getRepaintRect(false));
			}
		}

		private MindFusion.FlowChartX.Pen headPen;

		public override float PenWidth
		{
			get
			{
				return base.PenWidth;
			}
			set
			{
				// Set heads width
				headPen.Width = value;

				base.PenWidth = value;
			}
		}

		public override Color FrameColor
		{
			get
			{
				return base.FrameColor;
			}
			set
			{
				// Set heads color
				headPen.Color = value;

				base.FrameColor = value;
			}
		}

		public override DashStyle PenDashStyle
		{
			get
			{
				return base.PenDashStyle;
			}
			set
			{
				// Set heads style
				headPen.DashStyle = value;

				base.PenDashStyle = value;
			}
		}




		/// <summary>
		/// This text is displayed near the center of the arrow
		/// </summary>
		public string Text
		{
			get
			{
				return text;
			}
			set
			{
				if (text != value)
				{
					if(value == null)
						text = "";
					else
						text = value;

					updateText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}

				if (fcParent.IsTrialVersion && ZIndex > 50)
					text = "FlowChart.NET\nDemo Version";
			}
		}

		private string text;

		public override Font Font
		{
			get { return base.Font; }
			set
			{
				base.Font = value;
				updateText();
			}
		}


		public ArrowTextStyle TextStyle
		{
			get
			{
				return textStyle;
			}
			set
			{
				if(textStyle != value)
				{
					textStyle = value;

					updateText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private ArrowTextStyle textStyle;

		/// <summary>
		/// The arrow text is painted in this color
		/// </summary>
		public Color TextColor
		{
			get
			{
				return textColor;
			}
			set
			{
				if (!textColor.Equals(value))
				{
					textColor = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private Color textColor;

		/// <summary>
		/// Defines the shape of the arrow head
		/// </summary>
		public ArrowHead ArrowHead
		{
			get
			{
				return arrowHead;
			}
			set
			{
				if (arrowHead != value)
				{
					arrowHead = value;
					headTemplates[(int)arrowHead].initArrowHead(ahHead);
					headTemplates[(int)arrowHead].recalcArrowHead(ahHead, points[points.Count-2], points[points.Count-1]);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public ArrowHead ArrowBase
		{
			get
			{
				return arrowBase;
			}
			set
			{
				if (arrowBase != value)
				{
					arrowBase = value;
					headTemplates[(int)arrowBase].initArrowHead(ahBase);
					headTemplates[(int)arrowBase].recalcArrowHead(ahBase, points[1], points[0]);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public ArrowHead IntermArrowHead
		{
			get
			{
				return arrowInterm;
			}
			set
			{
				if (arrowInterm != value)
				{
					arrowInterm = value;
					headTemplates[(int)arrowInterm].initArrowHead(ahInterm);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private ArrowHead arrowHead;
		private ArrowHead arrowBase;
		private ArrowHead arrowInterm;

		public float ArrowHeadSize
		{
			get
			{
				return headSize;
			}
			set
			{
				if (headSize != value)
				{
					headSize = value;
					ahHead.setSize(headSize);
					headTemplates[(int)arrowHead].recalcArrowHead(ahHead, points[points.Count-2], points[points.Count-1]);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public float ArrowBaseSize
		{
			get
			{
				return baseSize;
			}
			set
			{
				if (baseSize != value)
				{
					baseSize = value;
					ahBase.setSize(baseSize);
					headTemplates[(int)arrowBase].recalcArrowHead(ahBase, points[1], points[0]);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public float IntermHeadSize
		{
			get
			{
				return intermSize;
			}
			set
			{
				if (intermSize != value)
				{
					intermSize = value;
					ahInterm.setSize(intermSize);
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		float headSize;
		float baseSize;
		float intermSize;

		#endregion

		#region placement

		/// <summary>
		/// Returns the arrow bounding rectangle
		/// </summary>
		public RectangleF BoundingRect
		{
			get { return getBoundingRect(); }
		}

		// ************ placement ************

		internal override bool containsPoint(PointF pt)
		{
			RectangleF rc = getBoundingRect();

			float inflate = Constants.getLineHitTest(fcParent.MeasureUnit) / 2;
			float maxDist = Constants.getLineHitTest(fcParent.MeasureUnit) * 2 / 3;

			rc.Inflate(inflate,inflate);
			if (!Utilities.pointInRect(pt, rc)) return false; 

			if (style == ArrowStyle.Bezier)
				if (Utilities.distToBezier(pt, points, points.Count) < maxDist) return true;

			if (style == ArrowStyle.Polyline || style == ArrowStyle.Cascading)
				if (Utilities.distToPolyline(pt, points, points.Count) < maxDist) return true;

			return false;
		}

		internal bool pointInHandle(PointF pt)
		{
			int handle = 0;
			return HitTestHandle(pt, ref handle);
		}

		public override bool HitTestHandle(PointF pt, ref int handle)
		{
			for (int i = 0; i < points.Count; ++i)
			{
				PointF apt = points[i];
				if (Math.Abs(apt.X - pt.X) <= fcParent.SelHandleSize/2 &&
					Math.Abs(apt.Y - pt.Y) <= fcParent.SelHandleSize/2)
				{
					if (i == 0 && !allowMoveStart) continue;
					if (i == points.Count-1 && !allowMoveEnd) continue;
					handle = i;
					return true;
				}
			}

			return false;
		}

		internal override int getHandleIdx(PointF pt)
		{
			for (int i = 0; i < points.Count; ++i)
			{
				PointF apt = points[i];
				if (Math.Abs(apt.X - pt.X) <= fcParent.SelHandleSize/2 &&
					Math.Abs(apt.Y - pt.Y) <= fcParent.SelHandleSize/2)
					return i;
			}

			return -1;
		}

		/// <summary>
		/// Call this method to align the arrow ends to the contour of the nodes it connects.
		/// </summary>
		internal void updateIntersections()
		{
			// if an arrow is connected to anchor points,
			// do not align its ends to nodes' outlines
			if (orgnAnchor > -1 && destAnchor > -1)
				return;

			// make the arrow point to the centers of nodes
			if (orgnAnchor == -1)
				ptOrg = points[0] = orgnLink.getInitialPt();
			if (destAnchor == -1)
				ptEnd = points[points.Count - 1] = destLink.getInitialPt();

			// if the nodes do not intersect, place the arrow ends at their outlines
			if (!orgnLink.objsIntersect(destLink))
			{
				if (style == ArrowStyle.Cascading)
					updateEndPtsPrp();

				if (orgnAnchor == -1)
					ptOrg = points[0] = orgnLink.getIntersection(ptOrg, points[1]);
				if (destAnchor == -1)
					ptEnd = points[points.Count - 1] = destLink.getIntersection(ptEnd, points[points.Count - 2]);
			}

			if (style == ArrowStyle.Cascading)
				updateEndPtsPrp();

			onShapeChanged();
		}

		internal override RectangleF getBoundingRect()
		{
			PointF pt1 = points[0];
			PointF pt2 = points[points.Count-1];

			float l = Math.Min(pt1.X, pt2.X);
			float r = Math.Max(pt1.X, pt2.X);
			float t = Math.Min(pt1.Y, pt2.Y);
			float b = Math.Max(pt1.Y, pt2.Y);

			// calculate the bounds of the bounding rect
			for (int i = 1; i < points.Count - 1; ++i)
			{
				l = Math.Min(l, points[i].X);
				t = Math.Min(t, points[i].Y);
				r = Math.Max(r, points[i].X);
				b = Math.Max(b, points[i].Y);
			}

			if (PenWidth > 0)
			{
				l -= PenWidth; t -= PenWidth;
				r += PenWidth; b += PenWidth;
			}

			return new RectangleF(l, t, r-l, b-t);
		}

		internal override RectangleF getRotatedBounds()
		{
			return getBoundingRect();
		}

		internal override RectangleF getRepaintRect(bool includeConnected)
		{
			RectangleF rect = getBoundingRect();
			
			float inflate = Math.Max(headSize, baseSize);
			float inflX = Math.Max(textSize.Width, inflate) + PenWidth;
			float inflY = Math.Max(textSize.Height, inflate) + PenWidth;
			rect.Inflate(inflX, inflY);

			if (Math.Abs(ShadowOffsetX) > inflX ||
				Math.Abs(ShadowOffsetY) > inflY)
				Utilities.addToRect(ref rect, ShadowOffsetX, ShadowOffsetY);

			// include the group objects rectangles
			if (subordinateGroup != null && includeConnected)
				rect = Utilities.unionRects(rect, subordinateGroup.getRepaintRect());

			return rect;
		}

		internal float distToPt(PointF pt)
		{
			int segmNum = 0;
			return distToPt(pt, ref segmNum);
		}

		internal float distToPt(PointF pt, ref int segmNum)
		{
			if (style == ArrowStyle.Bezier)
				return Utilities.distToBezier(pt, points, points.Count);

			return Utilities.distToPolyline(pt, points, points.Count, ref segmNum);
		}


		public bool AutoRoute
		{
			get
			{
				return autoRoute;
			}
			set
			{
				if (autoRoute != value)
				{
					fcParent.invalidate(getRepaintRect(hasAttachedGroup()));
					autoRoute = value;
					if (autoRoute && fcParent.rerouteArrow(this)) doRoute();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private bool autoRoute;

		/// <summary>
		/// Routes the arrow ignoring whether the arrow has
		/// the AutoRoute flag set.
		/// </summary>
		public void Route()
		{
			doRoute(true);
		}

		public void ReassignAnchorPoints()
		{
			fcParent.invalidate(getRepaintRect(false));
			putEndPointsAtNodeBorders(autoRoute);
			onShapeChanged();
			fcParent.invalidate(getRepaintRect(false));
		}

		internal void doRoute()
		{
			if (reflexive)
				return;

			doRoute(false, orgnLink, destLink, false);
		}

		internal void doRoute(bool force)
		{
			if (reflexive)
				return;

			doRoute(force, orgnLink, destLink, false);
		}

		internal void doRoute(bool force, Link orgnLink, Link destLink, bool nowCreating)
		{
			if (!force)
				if (!autoRoute) return;

			if (fcParent.DontRouteForAwhile)
				return;

			int i;

			float gridSize = fcParent.RoutingOptions.GridSize;

			PointF startPoint = points[0];
			PointF endPoint = points[points.Count - 1];

			// get a rectangle bounding both the origin and the destination
			RectangleF bounds = orgnLink.getNodeRect(true);
			bounds = Utilities.unionRects(bounds, destLink.getNodeRect(true));
			bounds = RectangleF.Union(bounds, Utilities.normalizeRect(
				RectangleF.FromLTRB(startPoint.X, startPoint.Y, endPoint.X, endPoint.Y)));
			if (bounds.Width < gridSize * 4)
				bounds.Inflate(gridSize * 4, 0);
			if (bounds.Height < gridSize * 4)
				bounds.Inflate(0, gridSize * 4);
			bounds.Inflate(bounds.Width, bounds.Height);

			int oNearest = 0, dNearest = 0;
			routeGetEndPoints(ref startPoint, ref endPoint,
				ref oNearest, ref dNearest, orgnLink, destLink, nowCreating);

			// Get the starting and ending square
			Point ptStart = new Point((int)((startPoint.X - bounds.X) / gridSize),
				(int)((startPoint.Y - bounds.Y) / gridSize));
			Point ptEnd = new Point((int)((endPoint.X - bounds.X) / gridSize),
				(int)((endPoint.Y - bounds.Y) / gridSize));
			if (ptStart.X == ptEnd.X && ptStart.Y == ptEnd.Y)
				return;

			// init the route grid
			int gridCols = (int)(bounds.Width / gridSize);
			int gridRows = (int)(bounds.Height / gridSize);

			RoutingGrid routingGrid = fcParent.RoutingGrid;
			routingGrid.allocate(gridCols, gridRows, bounds, this);
			byte[,] grid = routingGrid.getCostGrid();
			PathNode[,] gridClosed = routingGrid.getClosedGrid();
			PathNode[,] gridOpen = routingGrid.getOpenGrid();

			bool hurry = (gridCols * gridRows > 90000) &&
				fcParent.RoutingOptions.DontOptimizeLongRoutes;
			RouteHeuristics calcRouteHeuristics = hurry ?
				RoutingOptions.DistSquare : fcParent.RoutingOptions.RouteHeuristics;

			routeFixEndRegions(grid, ref ptStart, oNearest, ref ptEnd, dNearest, gridCols, gridRows);
			grid[ptStart.X, ptStart.Y] = 0;
			grid[ptEnd.X, ptEnd.Y] = 0;

			//---------- A* algorithm initialization -----------
			SortedList open = new SortedList();
			ArrayList closed = new ArrayList();
			Stack stack = new Stack();

			PathNode temp = new PathNode(ptStart.X, ptStart.Y);
			temp.G = 0;
			temp.H = calcRouteHeuristics(ptStart, ptEnd);
			temp.F = temp.G + temp.H;
			open.Add(temp, temp);
			gridOpen[temp.X, temp.Y] = temp;

			// setup A* cost function
			int adjcCost = fcParent.RoutingOptions.LengthCost;
			int turnCost = fcParent.RoutingOptions.TurnCost;

			PathNode best = null;
			bool found = false;
			int iterations = 0;
			for ( ; ; )
			{
				iterations++;

				// Get the best node from the open list
				if (open.Count == 0) break;
				PathNode pstmp = open.GetByIndex(0) as PathNode;

				open.RemoveAt(0);
				gridOpen[pstmp.X, pstmp.Y] = null;

				closed.Add(pstmp);
				gridClosed[pstmp.X, pstmp.Y] = pstmp;

				if ((best = pstmp) == null) break;

				// If best == destination -> path found
				if (best.X == ptEnd.X && best.Y == ptEnd.Y)
				{
					found = true;
					break;
				}

				// Generate best's successors
				int x = best.X;
				int y = best.Y;

				int[,] off = new int[4, 2] { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
				for (i = 0; i < 4; i++)
				{
					byte localCost = grid[x + off[i, 0], y + off[i, 1]];
					if (localCost == 255)
						continue;

					int g = best.G + adjcCost + localCost;
					bool straight = best.Parent == null ||
						(best.Parent.Y == best.Y && off[i, 1] == 0) ||
						(best.Parent.X == best.X && off[i, 0] == 0);
					if (best.Parent == null && oNearest >= 0 && (
						oNearest < 2 && off[i, 1] == 0 || oNearest >= 2 && off[i, 1] == 1))
						straight = false;
					if (!straight) g += turnCost;

					PathNode check = null;

					// if the successor is an open node, add it to the path
					check = gridOpen[x + off[i, 0], y + off[i, 1]];
					if (check != null)
					{
						best.Children[best.ChildCount++] = check;

						// and update its cost if now it is reached via a better path
						if (g < check.G)
						{
							open.Remove(check);		// keep sorted
							check.Parent = best;
							check.G = g;
							check.F = g + check.H;
							open.Add(check, check);	// keep sorted
						}
					}
					else
					{
						// if the successor is a closed node, add it to the path
						check = gridClosed[x + off[i, 0], y + off[i, 1]];
						if (check != null)
						{
							best.Children[best.ChildCount++] = check;

							// and update its cost if now it is reached via a better path
							if (g < check.G)
							{
								check.Parent = best;
								check.G = g;
								check.F = g + check.H;

								// and update its child items
								int gg = check.G;
								int cc = check.ChildCount;
								PathNode kid = null;
								for (int j = 0; j < cc; j++)
								{
									kid = check.Children[j];

									int gi = adjcCost;
									straight = check.Parent == null ||
										(check.Parent.Y == check.Y && check.Y == kid.Y) ||
										(check.Parent.X == check.X && check.X == kid.X);
									if (!straight) gi += turnCost;

									if (g + gi < kid.G)
									{
										bool wasOpen = gridOpen[kid.X, kid.Y] != null;
										if (wasOpen) open.Remove(kid);	// keep sorted

										kid.G = g + gi;
										kid.F = kid.G + kid.H;
										kid.Parent = check;
										stack.Push(kid);

										if (wasOpen) open.Add(kid, kid);
									}
								}
								PathNode parent;
								while (stack.Count > 0)
								{
									parent = stack.Pop() as PathNode;
									cc = parent.ChildCount;
									for (int j = 0; j < cc; j++)
									{
										kid = parent.Children[j];

										int gi = adjcCost;
										straight = parent.Parent == null ||
											(parent.Parent.Y == parent.Y && parent.Y == kid.Y) ||
											(parent.Parent.X == parent.X && parent.X == kid.X);
										if (!straight) gi += turnCost;

										if (parent.G + gi < kid.G)
										{
											bool wasOpen = gridOpen[kid.X, kid.Y] != null;
											if (wasOpen) open.Remove(kid);	// keep sorted

											kid.G = parent.G + gi;
											kid.F = kid.G + kid.H;
											kid.Parent = parent;
											stack.Push(kid);

											if (wasOpen) open.Add(kid, kid);
										}
									}
								}
							}
						}
						else
						{
							// haven't considered this grid square by now
							// create and initialize a path node for it 
							Point current = new Point(x + off[i, 0], y + off[i, 1]);
							PathNode newNode = new PathNode(current.X, current.Y);
							newNode.Parent = best;
							newNode.G = g;
							newNode.H = calcRouteHeuristics(current, ptEnd);
							newNode.F = newNode.G + newNode.H;

							// add it to the list of open nodes to be evaluated later
							open.Add(newNode, newNode);
							gridOpen[newNode.X, newNode.Y] = newNode;

							// add to the path
							best.Children[best.ChildCount++] = newNode;
						}
					}
				}
			}

			if (found)
			{
				PtCollection current = new PtCollection(0);

				current.Add(new Point((int)((points[points.Count - 1].X - bounds.X) / gridSize),
					(int)((points[points.Count - 1].Y - bounds.Y) / gridSize)));
				while (best != null)
				{
					current.Add(new Point(best.X, best.Y));
					best = best.Parent;
				}
				current.Add(new Point((int)((points[0].X - bounds.X) / gridSize),
					(int)((points[0].Y - bounds.Y) / gridSize)));

				// Remove all unneeded points
				Point pt1, pt2, pt3;
				for (i = 1; i < current.Count - 1;)
				{
					pt1 = current[i - 1];
					pt2 = current[i];
					pt3 = current[i + 1];

					if (pt1.X == pt2.X && pt2.X == pt3.X)
						current.RemoveAt(i);
					else if(pt1.Y == pt2.Y && pt2.Y == pt3.Y)
						current.RemoveAt(i);
					else
						i++;
				}

				// Save the first and last points of the arrow
				PointF ptFirst = points[0];
				PointF ptLast = points[points.Count - 1];

				// no perp. arrows on a single line
				if (style == ArrowStyle.Cascading && current.Count == 2 &&
					ptFirst.X != ptLast.X && ptFirst.Y != ptLast.Y)
				{
					Point orgPt = current[0];
					Point trgPt = current[current.Count-1];
					if (orgPt.X == trgPt.X || orgPt.Y == trgPt.Y)
					{
						Point insPt = new Point(
							(orgPt.X + trgPt.X) / 2, (orgPt.Y + trgPt.Y) / 2);
						current.Insert(1, insPt);
						current.Insert(1, insPt);
					}
				}

				// Re-segment the arrow
				points = new PointCollection(current.Count);
				points[0] = ptFirst;
				points[points.Count - 1] = ptLast;

				// Assign the points from the path
				i = current.Count - 1;
				i--; // Skip the first point
				while (i > 0)
				{
					Point pt = current[i];
					PointF ptDoc = new PointF(0, 0);
					ptDoc.X = bounds.X + pt.X * gridSize + gridSize / 2;
					ptDoc.Y = bounds.Y + pt.Y * gridSize + gridSize / 2;

					if (i == 1)
					{
						// Align to the last point
						if (pt.Y == current[0].Y)
							ptDoc.Y = ptLast.Y;
						else
							ptDoc.X = ptLast.X;
					}
					if (i == current.Count - 2)
					{
						// Align to the first point
						if (pt.Y == current[current.Count - 1].Y)
							ptDoc.Y = ptFirst.Y;
						else
							ptDoc.X = ptFirst.X;

						if (style == ArrowStyle.Cascading)
							cascadeStartHorizontal = (ptDoc.X != ptFirst.X);
					}

					points[current.Count - i - 1] = ptDoc;
					i--;
				}

				PointF ptf, ptf1, ptf2, ptf3;

				// If the line is perpendicular make it at least 2 segments
				if(style == ArrowStyle.Cascading && points.Count == 2)
				{
					ptf1 = points[0];
					ptf2 = points[points.Count - 1];
					ptf = ptf1;
					if (cascadeStartHorizontal)
						ptf.X = ptf2.X;
					else
						ptf.Y = ptf2.Y;
					points.Insert(1, ptf);
				}

				// If the line is straight there might be more unneeded points
				if (style == ArrowStyle.Polyline)
				{
					i = 0;
					while(i < points.Count - 2)
					{
						ptf1 = points[i];
						ptf2 = points[i + 2];

						ChartObject obj = fcParent.objectIntersectedBy(ptf1, ptf2,
							orgnLink.getNode(), destLink.getNode());
						if(obj == null)
							points.RemoveAt(i + 1);
						else
							i++;
					}
				}

				// If the line is bezier, smooth it a bit
				if (style == ArrowStyle.Bezier)
				{
					PointCollection newPoints = new PointCollection(0);
					newPoints.Add(points[0]);
					i = 0;
					while(i < points.Count - 2)
					{
						ptf1 = points[i];
						ptf2 = points[i + 1];

						newPoints.Add(ptf2);
						newPoints.Add(ptf2);
						if(i != points.Count - 3)
						{
							ptf3 = points[i + 2];
							ptf = new PointF((ptf2.X + ptf3.X) / 2, (ptf2.Y + ptf3.Y) / 2);
							newPoints.Add(ptf);
						}
						else
						{
							newPoints.Add(points[i + 2]);
						}
						i += 1;
					}

					if (newPoints.Count == 1)
					{
						newPoints = new PointCollection(4);

						ptf1 = points[0];
						ptf2 = points[points.Count - 1];
						ptf = new PointF((ptf1.X + ptf2.X) / 2, (ptf1.Y + ptf2.Y) / 2);
						newPoints[0] = ptf1;
						newPoints[1] = ptf;
						newPoints[2] = ptf;
						newPoints[3] = ptf2;
					}

					points.Clear();
					points = newPoints;
				}

				// Update SegmentCount property value
				if (style == ArrowStyle.Bezier)
					segmentCount = (short)((points.Count - 1) / 3);
				else
					segmentCount = (short)(points.Count - 1);
			}
			else
			{
				// No path found -> reset the arrow, leaving as little points as possible
				int ptsToLeave = 2;
				if (style == ArrowStyle.Cascading)
					ptsToLeave = 4;
				else if (style == ArrowStyle.Bezier)
					ptsToLeave = 4;

				if (style == ArrowStyle.Cascading)
				{
					cascadeOrientation = Orientation.Auto;
					segmentCount = 3;
				}
				else
					segmentCount = 1;

				while (points.Count > ptsToLeave)
					points.RemoveAt(1);

				if (style == ArrowStyle.Cascading && points.Count == 3)
					segmentCount = 2;

				updatePoints(points[points.Count - 1]);
			}

			updateArrowHeads();

			if (subordinateGroup != null)
			{
				subordinateGroup.onSegmentsChanged();
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));
			}

			resetCrossings();
			updateText();

			fcParent.fireArrowRoutedEvent(this);
		}

		private void routeGetEndPoints(ref PointF startPoint, ref PointF endPoint,
			ref int oNearest, ref int dNearest, Link orgnLink, Link destLink, bool nowCreating)
		{
			if (fcParent.RoutingOptions.Anchoring == Anchoring.Ignore)
			{
				float gridSize = fcParent.RoutingOptions.GridSize;

				// find the intersection of the nodes outlines with the four rays
				// that go outwards from the start/end points
				PointF[] opts = new PointF[4];
				PointF ptoCenter = startPoint;
				RectangleF or = orgnLink.getNodeRect(true);
				Utilities.getProjections(startPoint, or, opts);

				PointF[] dpts = new PointF[4];
				PointF ptdCenter = endPoint;
				RectangleF dr = destLink.getNodeRect(true);
				Utilities.getProjections(endPoint, dr, dpts);

				// get the intersection point nearest to the origin and destination centers.
		
				int oLen = opts.Length;
				if (fcParent.RoutingOptions.StartOrientation == Orientation.Horizontal)
				{
					oNearest = 2;
				}
				else if (fcParent.RoutingOptions.StartOrientation == Orientation.Vertical)
				{
					oLen = 2;
				}

				for (int i = oNearest + 1; i < oLen; i++)
				{
					if (Math.Abs(opts[i].X - ptoCenter.X) + Math.Abs(opts[i].Y - ptoCenter.Y) <
						Math.Abs(opts[oNearest].X - ptoCenter.X) + Math.Abs(opts[oNearest].Y - ptoCenter.Y))
						oNearest = i;
				}

				int dLen = dpts.Length;
				if (fcParent.RoutingOptions.EndOrientation == Orientation.Horizontal)
				{
					dNearest = 2;
				}
				else if (fcParent.RoutingOptions.EndOrientation == Orientation.Vertical)
				{
					dLen = 2;
				}

				for (int i = dNearest + 1; i < dLen; i++)
				{
					if (Math.Abs(dpts[i].X - ptdCenter.X) + Math.Abs(dpts[i].Y - ptdCenter.Y) <
						Math.Abs(dpts[dNearest].X - ptdCenter.X) + Math.Abs(dpts[dNearest].Y - ptdCenter.Y))
						dNearest = i;
				}

				startPoint = opts[oNearest];
				endPoint = dpts[dNearest];

				// Offset the starting and ending point a little bit
				float[,] pull = new float[4, 2] { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 } };
				startPoint.X += pull[oNearest, 0] * gridSize;
				startPoint.Y += pull[oNearest, 1] * gridSize;
				endPoint.X += pull[dNearest, 0] * gridSize;
				endPoint.Y += pull[dNearest, 1] * gridSize;
			}
			else
			{
				oNearest = -1;
				dNearest = -1;

				if (nowCreating ||
					fcParent.RoutingOptions.Anchoring == Anchoring.Reassign)
				{
					putEndPointsAtNodeBorders(true, orgnLink, destLink);
					startPoint = points[0];
					endPoint = points[points.Count - 1];
				}
			}
		}

		private void routeFixEndRegions(byte[,] grid,
			ref Point ptStart, int oNearest, ref Point ptEnd, int dNearest, int gridCols, int gridRows)
		{
			if (fcParent.RoutingOptions.Anchoring == Anchoring.Ignore)
			{
				int dx = Math.Abs(ptStart.X - ptEnd.X);
				int dy = Math.Abs(ptStart.Y - ptEnd.Y);
				bool elongate = !(dx < 3 && dy < 3);

				switch (oNearest)
				{
				case 0:
					if (ptStart.Y + 1 < gridRows)
						grid[ptStart.X, ptStart.Y + 1] = 255;

					// Pull further if possible
					if (ptStart.Y - 1 > 0 && elongate)
					{
						if (grid[ptStart.X, ptStart.Y - 1] != 255)
						{
							grid[ptStart.X, ptStart.Y] = 255;
							ptStart.Y -= 1;
						}
					}
					break;

				case 1:
					if (ptStart.Y - 1 >= 0)
						grid[ptStart.X, ptStart.Y - 1] = 255;

					// Pull further if possible
					if (ptStart.Y + 1 < gridRows && elongate)
					{
						if (grid[ptStart.X, ptStart.Y + 1] != 255)
						{
							grid[ptStart.X, ptStart.Y] = 255;
							ptStart.Y += 1;
						}
					}
					break;

				case 2:
					if (ptStart.X - 1 >= 0)
						grid[ptStart.X - 1, ptStart.Y] = 255;

					// Pull further if possible
					if (ptStart.X + 1 < gridCols && elongate)
					{
						if (grid[ptStart.X + 1, ptStart.Y] != 255)
						{
							grid[ptStart.X, ptStart.Y] = 255;
							ptStart.X += 1;
						}
					}
					break;

				case 3:
					if (ptStart.X + 1 < gridCols)
						grid[ptStart.X + 1, ptStart.Y] = 255;

					// Pull further if possible
					if (ptStart.X - 1 > 0 && elongate)
					{
						if (grid[ptStart.X - 1, ptStart.Y] != 255)
						{
							grid[ptStart.X, ptStart.Y] = 255;
							ptStart.X -= 1;
						}
					}
					break;
				}

				switch (dNearest)
				{
				case 0:
					if (ptEnd.Y + 1 < gridRows)
						grid[ptEnd.X, ptEnd.Y + 1] = 255;

					// Pull further if possible
					if (ptEnd.Y - 1 > 0 && elongate)
					{
						if (grid[ptEnd.X, ptEnd.Y - 1] != 255)
						{
							grid[ptEnd.X, ptEnd.Y] = 255;
							ptEnd.Y -= 1;
						}
					}
					break;

				case 1:
					if (ptEnd.Y - 1 >= 0)
						grid[ptEnd.X, ptEnd.Y - 1] = 255;

					// Pull further if possible
					if (ptEnd.Y + 1 < gridRows && elongate)
					{
						if (grid[ptEnd.X, ptEnd.Y + 1] != 255)
						{
							grid[ptEnd.X, ptEnd.Y] = 255;
							ptEnd.Y += 1;
						}
					}
					break;

				case 2:
					if (ptEnd.X - 1 >= 0)
						grid[ptEnd.X - 1, ptEnd.Y] = 255;

					// Pull further if possible
					if (ptEnd.X + 1 < gridCols && elongate)
					{
						if (grid[ptEnd.X + 1, ptEnd.Y] != 255)
						{
							grid[ptEnd.X, ptEnd.Y] = 255;
							ptEnd.X += 1;
						}
					}
					break;

				case 3:
					if (ptEnd.X + 1 < gridCols)
						grid[ptEnd.X + 1, ptEnd.Y] = 255;

					// Pull further if possible
					if (ptEnd.X - 1 > 0 && elongate)
					{
						if (grid[ptEnd.X - 1, ptEnd.Y] != 255)
						{
							grid[ptEnd.X, ptEnd.Y] = 255;
							ptEnd.X -= 1;
						}
					}
					break;
				}
			}
			else
			{
				byte cost = fcParent.RoutingOptions.NodeVicinityCost;
				for (int i = -1; i <= 1; ++i)
				{
					for (int j = -1; j <= 1; ++j)
					{
						if (i == 0 && j == 0)
							continue;

						int sx = ptStart.X + i;
						int sy = ptStart.Y + j;
						int ex = ptEnd.X + i;
						int ey = ptEnd.Y + j;

						if (sx >= 0 && sx < gridCols && sy >= 0 && sy < gridRows)
							grid[sx, sy] = cost;
						if (ex >= 0 && ex < gridCols && ey >= 0 && ey < gridRows)
							grid[ex, ey] = cost;
					}
				}
			}
		}

		private Link routeGetTarget(PointF point)
		{
			if (destLink != null)
				return destLink;

			Node target = fcParent.getCurrBehavior().GetTargetNode(point);
			if (target == null)
				target = fcParent.Dummy;

			return target.createLink(this, point, true);
		}

		#endregion

		#region implementation

		public override ItemType getType()
		{
			return ItemType.Arrow;
		}

		internal void arrangePoints(Anchoring keepAnchorsPos)
		{
			if (reflexive)
			{
				if (style == ArrowStyle.Cascading)
				{
					if (points.Count % 2 == 0)
						setSegments(3);
					RectangleF rc = orgnLink.getNodeRect(true);
					float w = rc.Width;
					points[0] = orgnLink.getNode().getTopIntr(1f/3);
					points[points.Count-1] = orgnLink.getNode().getTopIntr(2f/3);
					for (int i = 0; i < points.Count / 2 - 1; ++i)
					{
						points[1+i] = new PointF(points[0].X, points[0].Y - w/3);
						points[points.Count - 2 - i] = new PointF(
							points[points.Count-1].X, points[0].Y - w/3);
					}
					cascadeStartHorizontal = false;
				}
				else
				{
					PointF ptOrg = orgnLink.getNode().getTopMiddleIntr();
					points[0] = points[points.Count-1] = ptOrg;
					for (int i = 0; i < points.Count / 2 - 1; ++i)
					{
						points[1+i] = new PointF(
							ptOrg.X - Constants.getReflWth2(fcParent.MeasureUnit),
							ptOrg.Y - Constants.getReflHth(fcParent.MeasureUnit));

						points[points.Count - 2 - i] = new PointF(
							ptOrg.X + Constants.getReflWth2(fcParent.MeasureUnit),
							ptOrg.Y - Constants.getReflHth(fcParent.MeasureUnit));
					}
					updateArrowHeads();
				}
			}
			else
			{
				int denom = points.Count - 1;

				// determine initial position of the first point
				bool keepOrg = keepAnchorsPos == Anchoring.Keep && orgnAnchor > -1;
				points[0] = keepOrg ?
					orgnLink.getAnchorPos(orgnAnchor) : orgnLink.getInitialPt();

				// determine initial position of the last point
				bool keepDest = keepAnchorsPos == Anchoring.Keep && destAnchor > -1;
				points[points.Count-1] = keepDest ?
					destLink.getAnchorPos(destAnchor) : destLink.getInitialPt();

				// if not using anchors, find closest points at node edge
				if (!orgnLink.objsIntersect(destLink))
				{
					if (!keepOrg) points[0] = orgnLink.getIntersection(
						points[0], points[points.Count-1]);
					if (!keepDest) points[points.Count-1] = destLink.getIntersection(
						points[points.Count-1], points[0]);
				}

				// if reassigning anchors...
				if (keepAnchorsPos == Anchoring.Reassign)
				{
					points[0] = orgnLink.getAnchor(points[0], this, false, ref orgnAnchor);
					points[points.Count-1] = destLink.getAnchor(points[points.Count-1], this, true, ref destAnchor);
				}

				// arrange the control points
				if (style == ArrowStyle.Cascading)
				{
					bool setOrg = keepAnchorsPos == Anchoring.Ignore || orgnAnchor == -1;
					if (setOrg) points[0] = orgnLink.getInitialPt();

					bool setDest = keepAnchorsPos == Anchoring.Ignore || destAnchor == -1;
					if (setDest) points[points.Count-1] = destLink.getInitialPt();

					if (cascadeOrientation == Orientation.Auto)
						cascadeStartHorizontal =
							Math.Abs(points[0].X - points[points.Count-1].X) >
							Math.Abs(points[0].Y - points[points.Count-1].Y);
					else if (cascadeOrientation == Orientation.Horizontal)
						cascadeStartHorizontal = true;
					else
						cascadeStartHorizontal = false;

					if (!orgnLink.objsIntersect(destLink))
					{
						// a number large enough for a point to go outside a node
						// when translation by that number is made
						RectangleF rc1 = orgnLink.getNode().getBoundingRect();
						RectangleF rc2 = destLink.getNode().getBoundingRect();
						float outside =
							(rc1.Width + rc1.Height + rc2.Width + rc2.Height) * 2;

						PointF ol = points[0]; ol.X -= outside;
						PointF ot = points[0]; ot.Y -= outside;
						PointF or = points[0]; or.X += outside;
						PointF ob = points[0]; ob.Y += outside;
						PointF el = points[points.Count-1]; el.X -= outside;
						PointF et = points[points.Count-1]; et.Y -= outside;
						PointF er = points[points.Count-1]; er.X += outside;
						PointF eb = points[points.Count-1]; eb.Y += outside;
						ol = orgnLink.getIntersection(points[0], ol);
						ot = orgnLink.getIntersection(points[0], ot);
						or = orgnLink.getIntersection(points[0], or);
						ob = orgnLink.getIntersection(points[0], ob);
						el = destLink.getIntersection(points[points.Count-1], el);
						et = destLink.getIntersection(points[points.Count-1], et);
						er = destLink.getIntersection(points[points.Count-1], er);
						eb = destLink.getIntersection(points[points.Count-1], eb);
						bool bEven = points.Count % 2 == 1;

						if (setOrg)
						{
							if (cascadeStartHorizontal && !bEven)
								points[0] = (or.X < el.X) ? or : ol;
							else
							if (cascadeStartHorizontal && bEven)
								points[0] = (ol.X > er.X) ? ol : or;
							else
							if (!cascadeStartHorizontal && !bEven)
								points[0] = (ob.Y < et.Y) ? ob : ot;
							else
								points[0] = (ot.Y > eb.Y) ? ot : ob;
						}
						if (setDest)
						{
							if (cascadeStartHorizontal && !bEven)
								points[points.Count-1] = (or.X < el.X) ? el : er;
							else
							if (cascadeStartHorizontal && bEven)
								points[points.Count-1] = (ol.Y < et.Y) ? et : eb;
							else
							if (!cascadeStartHorizontal && !bEven)
								points[points.Count-1] = (ob.Y < et.Y) ? et : eb;
							else
								points[points.Count-1] = (ot.X < el.X) ? el : er;
						}
					}

					float dx = points[points.Count-1].X - points[0].X;
					float dy = points[points.Count-1].Y - points[0].Y;

					arrangePrpSegments(dx, dy);
				}
				else
				{
					for (int i = 1; i < points.Count - 1; ++i)
					{
						points[i] = new PointF(
							points[0].X + i * (points[points.Count-1].X - points[0].X) / denom,
							points[0].Y + i * (points[points.Count-1].Y - points[0].Y) / denom);
					}
				}
			}

			//compute the arrow points
			updateArrowHeads();

			orgnLink.saveEndRelative();
			destLink.saveEndRelative();

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));
		}

		internal void setOrgAndDest(Link orgLink, Link trgLink)
		{
			orgnLink = orgLink;
			destLink = trgLink;

			// place the end points of the arrow at their respective object edge
			points[0] = orgnLink.getInitialPt();
			points[points.Count-1] = destLink.getInitialPt();
			if (!orgnLink.objsIntersect(destLink))
			{
				points[0] = orgnLink.getIntersection(points[0], points[points.Count-1]);
				points[points.Count-1] = destLink.getIntersection(points[points.Count-1], points[0]);
			}

			points[0] = orgnLink.getAnchor(points[0], this, false, ref orgnAnchor);
			points[points.Count-1] = destLink.getAnchor(points[points.Count-1], this, true, ref destAnchor);

			// set reflexive arrow position & form
			reflexive = orgnLink.sameNode(destLink);
			if (reflexive)
			{
				setReflexive();
			}
			else
			{
				// compute the arrow points
				updatePoints(points[points.Count-1]);
				if (dynamic)
					updatePosFromOrgAndDest(true);
			}

			rectFromPoints();

			// relative position to nodes
			orgnLink.saveEndRelative();
			destLink.saveEndRelative();

			doRoute();
		}

		internal void updateNodeCollections()
		{
			// add the arrow to its nodes' arrow lists
			orgnLink.addArrowToObj();
			destLink.addArrowToObj();
		}

		internal void adjustEnds(ArrowAnchor orgnAnchor, ArrowAnchor destAnchor)
		{
			points[0] = orgnLink.getAnchoredPt(orgnAnchor, false);
			points[points.Count-1] = destLink.getAnchoredPt(destAnchor, false);

			orgnLink.saveEndRelative();
			destLink.saveEndRelative();
			updateArrowHeads();
		}

		internal int splitSegment(int segment, PointF pt)
		{
			fcParent.UndoManager.onModifyItem(this);

			int ptPos = 0;
			PointF pt1, pt2;

			switch (style)
			{
			case ArrowStyle.Polyline:
				ptPos = segment + 1;
				points.Insert(ptPos, pt);
				segmentCount++;

				if (subordinateGroup != null)
					subordinateGroup.onArrowSplit(true, ptPos, 1);

				return ptPos;
			case ArrowStyle.Cascading:
				pt1 = points[segment];
				pt2 = points[segment + 1];
				pt.X = (pt1.X + pt2.X) / 2;
				pt.Y = (pt1.Y + pt2.Y) / 2;

				ptPos = segment + 1;
				points.Insert(ptPos, pt);
				ptPos++;

				points.Insert(ptPos, pt);
				segmentCount += 2;

				if (subordinateGroup != null)
					subordinateGroup.onArrowSplit(true, segment + 1, 2);

				return ptPos;
			}

			return 0;
		}

		static ArrowHeadTemplate[] headTemplates = null;

		public ArrowHeadShape HeadShape
		{
			get { return ahHead; }
		}

		public ArrowHeadShape BaseShape
		{
			get { return ahBase; }
		}

		private ArrowHeadShape ahHead;
		private ArrowHeadShape ahBase;
		private ArrowHeadShape ahInterm;

		internal void updatePoints(PointF p)
		{
			int denom = points.Count - 1;

			// update the control points
			points[points.Count-1] = p;
			if (style == ArrowStyle.Cascading)
			{
				float dx = p.X - points[0].X;
				float dy = p.Y - points[0].Y;

				if (cascadeOrientation == Orientation.Auto)
					cascadeStartHorizontal = Math.Abs(dx) > Math.Abs(dy);
				else if (cascadeOrientation == Orientation.Horizontal)
					cascadeStartHorizontal = true;
				else
					cascadeStartHorizontal = false;

				arrangePrpSegments(dx, dy);
			}
			else
			{
				for (int i = 1; i < points.Count - 1; ++i)
				{
					points.SetAt(i, new PointF(
						points[0].X + i * (p.X - points[0].X) / denom,
						points[0].Y + i * (p.Y - points[0].Y) / denom));
				}
			}

			updateArrowHeads();
		}

		internal void updateArrowHeads()
		{
			headTemplates[(int)arrowBase].recalcArrowHead(ahBase, points[1], points[0]);

			if (Points[Points.Count - 2] != Points[Points.Count - 1])
			{
				headTemplates[(int)arrowHead].recalcArrowHead(
					ahHead, Points[Points.Count-2], Points[Points.Count-1]);
			}
			else
			{
				if (Points.Count > 2)
					headTemplates[(int)arrowHead].recalcArrowHead(
						ahHead, Points[Points.Count - 3], Points[Points.Count - 1]);
			}
		}

		private void arrangePrpSegments(float dx, float dy)
		{
			float ax, ay;
			int pts = points.Count;
			int i;
			for (i = 1; i < pts - 1; ++i)
			{
				if (cascadeStartHorizontal)
				{
					if (i % 2 != 0)
					{
						ax = dx / (pts / 2);
						ay = 0;
					}
					else
					{
						ax = 0;
						ay = dy / ((pts - 1) / 2);
					}
				}
				else
				{
					if (i % 2 != 0)
					{
						ax = 0;
						ay = dy / (pts / 2);
					}
					else
					{
						ax = dx / ((pts - 1) / 2);
						ay = 0;
					}
				}
				points[i] = new PointF(points[i-1].X + ax, points[i-1].Y + ay);
			}
			i--;
			if ((cascadeStartHorizontal && (i%2 != 0)) || (!cascadeStartHorizontal && !(i%2 != 0)))
				points[pts-2] = new PointF(points[pts-1].X, points[pts-2].Y);
			else
				points[pts-2] = new PointF(points[pts-2].X, points[pts-1].Y);
		}

		private void updateEndPtsPrp()
		{
			int pts = points.Count;

			float fx = points[1].X;
			float fy = points[1].Y;
			float lx = points[pts-2].X;
			float ly = points[pts-2].Y;

			if (cascadeStartHorizontal) fy = points[0].Y;
			else fx = points[0].X;
			
			if (pts % 2 != 0)
			{
				if (cascadeStartHorizontal) lx= points[pts-1].X;
				else ly = points[pts-1].Y;
			}
			else
			{
				if (cascadeStartHorizontal) ly = points[pts-1].Y;
				else lx = points[pts-1].X;
			}

			if (segmentCount == 2)
			{
				// The second point and the point before the last
				// actually coincide, so pay heed to this special case
				float x = cascadeStartHorizontal ? lx : fx;
				float y = cascadeStartHorizontal ? fy : ly;
				points[1] = new PointF(x, y);
			}
			else
			if (segmentCount == 3)
			{
				points[1] = new PointF(fx, fy);
				if (cascadeStartHorizontal)
					points[pts-2] = new PointF(fx, ly);
				else
					points[pts-2] = new PointF(lx, fy);
			}
			else
			{
				points[1] = new PointF(fx, fy);
				points[pts-2] = new PointF(lx, ly);
			}
		}

		// This function retains the form of arrows with RetainForm flag.
		// The third parameter specifies if the destination or
		// origin has been moved; the first two parameters specify
		// the move distance.
		internal void retain(float dx, float dy, bool originChanged)
		{
			if (reflexive)
				return;
			if (autoRoute)
				return;
			if (!retainForm)
				return;

			PointF pt1, pt2;
			PointF center;

			if (originChanged)
			{
				center = points[points.Count - 1];
				pt1 = points[0];
			}
			else
			{
				center = points[0];
				pt1 = points[points.Count - 1];
			}
			pt2 = new PointF(pt1.X + dx, pt1.Y + dy);

			if(style != ArrowStyle.Cascading)
			{
				float a1 = 0, r1 = 0;
				float a2 = 0, r2 = 0;
				float da, dr;

				MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
					center, pt1, ref a1, ref r1);
				MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
					center, pt2, ref a2, ref r2);

				da = a2 - a1;
				if(r1 == 0)
					dr = 0;
				else
					dr = r2 / r1;

				for(int i = 1; i < points.Count - 1; i++)
				{
					PointF pt = points[i];
					MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
						center, pt, ref a1, ref r1);
					a1 += da;
					r1 *= dr;
					MindFusion.Geometry.Geometry2D.Convert.PolarToDekart(
						center, a1, r1, ref pt);
					points[i] = pt;
				}
			}
			else
			{
				PointF ptBegin1 = points[0];
				PointF ptBegin2;
				PointF ptEnd1 = points[points.Count - 1];
				PointF ptEnd2;

				if(originChanged)
				{
					ptBegin2 = new PointF(ptBegin1.X + dx, ptBegin1.Y + dy);
					ptEnd2 = ptEnd1;
				}
				else
				{
					ptBegin2 = ptBegin1;
					ptEnd2 = new PointF(ptEnd1.X + dx, ptEnd1.Y + dy);
				}

				float relx;
				float rely;

				if(Math.Abs(ptEnd1.X - ptBegin1.X) > 0.0001)
					relx = (ptEnd2.X - ptBegin2.X) / (ptEnd1.X - ptBegin1.X);
				else
					relx = 0;

				if(Math.Abs(ptEnd1.Y - ptBegin1.Y) > 0.0001)
					rely = (ptEnd2.Y - ptBegin2.Y) / (ptEnd1.Y - ptBegin1.Y);
				else
					rely = 0;

				for(int i = 1; i < points.Count - 1; i++)
				{
					PointF pt = points[i];

					pt.X = relx * (pt.X - ptBegin1.X) + ptBegin2.X;
					pt.Y = rely * (pt.Y - ptBegin1.Y) + ptBegin2.Y;

					points[i] = pt;
				}

				updateEndPtsPrp();
			}
		}

		private void chooseRouteOutlinePoints(ref PointF ptOrg, ref PointF ptDest)
		{
			chooseRouteOutlinePoints(orgnLink, destLink, ref ptOrg, ref ptDest);
		}

		private void chooseRouteOutlinePoints(Link orgnLink, Link destLink,
			ref PointF ptOrg, ref PointF ptDest)
		{
			RoutingOptions rop = fcParent.RoutingOptions;
			Node origin = orgnLink.getNode();
			Node destination = destLink.getNode();
			bool reflexive = orgnLink.sameNode(destLink);

			// how many points to consider ?
			int srcCount = rop.StartOrientation == Orientation.Auto ? 4 : 2;
			int dstCount = rop.EndOrientation == Orientation.Auto ? 4 : 2;
			PointF[] srcPoints = new PointF[srcCount];
			PointF[] dstPoints = new PointF[dstCount];

			// determine the coordinates of the points to be checked ...
			int srcIdx = 0, dstIdx = 0;

			// source points
			if (rop.StartOrientation != Orientation.Vertical)
			{
				srcPoints[srcIdx++] = orgnLink.getIntersection(ptOrg,
					new PointF(ptOrg.X - 2*origin.BoundingRect.Width, ptOrg.Y));
				srcPoints[srcIdx++] = orgnLink.getIntersection(ptOrg,
					new PointF(ptOrg.X + 2*origin.BoundingRect.Width, ptOrg.Y));
			}
			if (rop.StartOrientation != Orientation.Horizontal)
			{
				srcPoints[srcIdx++] = orgnLink.getIntersection(ptOrg,
					new PointF(ptOrg.X, ptOrg.Y - 2*origin.BoundingRect.Height));
				srcPoints[srcIdx++] = orgnLink.getIntersection(ptOrg,
					new PointF(ptOrg.X, ptOrg.Y + 2*origin.BoundingRect.Height));
			}

			// destination points
			if (rop.EndOrientation != Orientation.Vertical)
			{
				dstPoints[dstIdx++] = dstPoints[1] = destLink.getIntersection(ptDest,
					new PointF(ptDest.X - 2*destination.BoundingRect.Width, ptDest.Y));
				dstPoints[dstIdx++] = destLink.getIntersection(ptDest,
					new PointF(ptDest.X + 2*destination.BoundingRect.Width, ptDest.Y));
			}
			if (rop.EndOrientation != Orientation.Horizontal)
			{
				dstPoints[dstIdx++] = destLink.getIntersection(ptDest,
					new PointF(ptDest.X, ptDest.Y - 2*destination.BoundingRect.Height));
				dstPoints[dstIdx++] = destLink.getIntersection(ptDest,
					new PointF(ptDest.X, ptDest.Y + 2*destination.BoundingRect.Height));
			}

			// check which of these points are closest
			ptOrg = srcPoints[0];
			ptDest = dstPoints[0];
			float minDist = Single.MaxValue;
			for (int sidx = 0; sidx < srcCount; ++sidx)
			{
				if (!reflexive && destination.containsPoint(srcPoints[sidx]))
					continue;

				for (int didx = 0; didx < dstCount; ++didx)
				{
					if (!reflexive && origin.containsPoint(dstPoints[didx]))
						continue;

					float dist = Utilities.Distance(
						srcPoints[sidx], dstPoints[didx]);

					if (dist < 2 * rop.GridSize)
						continue;

					// we prefer these to be on the same side
					if (sidx != didx) dist *= 3;

					if (dist < minDist)
					{
						minDist = dist;
						ptOrg = srcPoints[sidx];
						ptDest = dstPoints[didx];
					}
				}
			}
		}

		private void putEndPointsAtNodeBorders(bool routing)
		{
			putEndPointsAtNodeBorders(routing, orgnLink, destLink);
		}

		private void putEndPointsAtNodeBorders(bool routing, Link orgnLink, Link destLink)
		{
			PointF ptOrg = orgnLink.getInitialPt();
			PointF ptDest = destLink.getInitialPt();

			if (!orgnLink.objsIntersect(destLink))
			{
				// if destination and source do not intersect, use the points
				// where the line connecting their centers crosses their outlines
				if (style == ArrowStyle.Cascading)
					updateEndPtsPrp();
				ptOrg = orgnLink.getIntersection(ptOrg,
					routing ? ptDest : points[1]);
				ptDest = destLink.getIntersection(ptDest,
					routing ? ptOrg : points[points.Count - 2]);
			}
			else
			{
				// if the arrow will be rerouted, choose the closest points on the
				// horizontal line intersections with the node outlines. These points
				// should not be contained within a node
				if (routing)
					chooseRouteOutlinePoints(orgnLink, destLink, ref ptOrg, ref ptDest);
			}

			// snap to nearest anchors
			ptOrg = orgnLink.getNode().getAnchor(
				ptOrg, this, false, ref orgnAnchor);
			ptDest = destLink.getNode().getAnchor(
				ptDest, this, true, ref destAnchor);

			if (style != ArrowStyle.Cascading)
				retain(ptOrg.X - points[0].X, ptOrg.Y - points[0].Y, true);
			points[0] = ptOrg;
			if (style != ArrowStyle.Cascading)
				retain(ptDest.X - points[points.Count - 1].X, ptDest.Y - points[points.Count - 1].Y, false);
			points[points.Count - 1] = ptDest;

			if (style == ArrowStyle.Cascading)
				updateEndPtsPrp();
		}

		internal void updatePosFromOrgAndDest(bool checkMdf)
		{
			PointF ptBefore = points[0];

			if (dynamic && !reflexive)
			{
				putEndPointsAtNodeBorders(autoRoute);
			}
			else
			{
				if (!checkMdf || orgnLink.getNode().getModifying())
				{
					PointF newPos = orgnLink.getEndPoint();
					float dx = newPos.X - points[0].X;
					float dy = newPos.Y - points[0].Y;
					retain(dx, dy, true);
					points[0] = newPos;

					if (style == ArrowStyle.Cascading)
					{
						float fx = points[1].X;
						float fy = points[1].Y;
						if (cascadeStartHorizontal) fy = points[0].Y;
						else  fx = points[0].X;
						points[1] = new PointF(fx, fy);
					}
				}
				if (!checkMdf || destLink.getNode().getModifying())
				{
					PointF newPos = destLink.getEndPoint();
					float dx = newPos.X - points[points.Count-1].X;
					float dy = newPos.Y - points[points.Count-1].Y;
					retain(dx, dy, false);
					points[points.Count-1] = newPos;

					if (style == ArrowStyle.Cascading)
					{
						int pts = points.Count;
						float lx = points[pts-2].X;
						float ly = points[pts-2].Y;
						if (pts % 2 != 0)
						{
							if (cascadeStartHorizontal) lx = points[pts-1].X;
							else  ly = points[pts-1].Y;
						}
						else
						{
							if (cascadeStartHorizontal) ly = points[pts-1].Y;
							else  lx = points[pts-1].X;
						}
						points[pts-2] = new PointF(lx, ly);
					}
				}
			}

			if (reflexive)
			{
				PointF ptAfter = points[0];

				float dx = ptAfter.X - ptBefore.X;
				float dy = ptAfter.Y - ptBefore.Y;

				for(int i = 1; i < points.Count - 1; i++)
				{
					PointF pt = points[i];
					pt.X += dx;
					pt.Y += dy;
					points[i] = pt;
				}
			}

			// compute the arrow points
			updateArrowHeads();

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));

			resetCrossings();
			updateText();
		}

		#endregion

		#region undo / redo

		internal override ItemState createState()
		{
			return new ArrowState(this);
		}

		internal override void saveState(ItemState state)
		{
			base.saveState(state);

			ArrowState astate = (ArrowState)state;
			astate.points = points.Clone();
			astate.segmentCount = segmentCount;
			astate.style = style;
			astate.reflexive = reflexive;
			astate.cascadeStartHorizontal = cascadeStartHorizontal;

			astate.orgnLink = orgnLink;
			astate.orgnAnchor = orgnAnchor;
			astate.destLink = destLink;
			astate.destAnchor = destAnchor;

			astate.orgnPoint = orgnLink.RelativePosition;
			astate.destPoint = destLink.RelativePosition;
		}

		internal override void restoreState(ItemState state)
		{
			base.restoreState(state);

			ArrowState astate = (ArrowState)state;
			style = astate.style;
			segmentCount = astate.segmentCount;
			points = astate.points.Clone();
			reflexive = astate.reflexive;
			cascadeStartHorizontal = astate.cascadeStartHorizontal;

			if (orgnLink != astate.orgnLink)
			{
				orgnLink.removeArrowFromObj();
				orgnLink = astate.orgnLink;
				orgnLink.addArrowToObj();
			}
			orgnAnchor = astate.orgnAnchor;

			if (destLink != astate.destLink)
			{
				destLink.removeArrowFromObj();
				destLink = astate.destLink;
				destLink.addArrowToObj();
			}
			destAnchor = astate.destAnchor;

			UpdateFromPoints();

			if (subordinateGroup != null)
				subordinateGroup.onRestoreState();

			orgnLink.RelativePosition = astate.orgnPoint;
			destLink.RelativePosition = astate.destPoint;

			resetCrossings();
			updateText();
		}

		internal override ItemProperties createProperties()
		{
			return new ArrowProperties();
		}

		internal override void saveProperties(ItemProperties props)
		{
			base.saveProperties(props);

			ArrowProperties aprops = (ArrowProperties)props;

			aprops.allowMoveStart = allowMoveStart;
			aprops.allowMoveEnd = allowMoveEnd;
			aprops.arrowHead = arrowHead;
			aprops.arrowBase = arrowBase;
			aprops.arrowInterm = arrowInterm;
			aprops.autoRoute = autoRoute;
			aprops.dynamic = dynamic;
			aprops.headSize = headSize;
			aprops.baseSize = baseSize;
			aprops.intermSize = intermSize;
			aprops.penColor = penColor;
			aprops.cascadeOrientation = cascadeOrientation;
			aprops.retainForm = retainForm;
			aprops.text = text;
			aprops.textColor = textColor;
			aprops.textStyle = textStyle;
			aprops.customDraw = customDraw;
			aprops.snapToNodeBorder = snapToNodeBorder;
			aprops.selStyle = selStyle;

			aprops.headPen = (MindFusion.FlowChartX.Pen)headPen.Clone();
		}

		internal override void restoreProperties(ItemProperties props)
		{
			ArrowProperties aprops = (ArrowProperties)props;

			allowMoveStart = aprops.allowMoveStart;
			allowMoveEnd = aprops.allowMoveEnd;
			ArrowHead = aprops.arrowHead;
			ArrowBase = aprops.arrowBase;
			IntermArrowHead = aprops.arrowInterm;
			autoRoute = aprops.autoRoute;
			dynamic = aprops.dynamic;
			ArrowHeadSize = aprops.headSize;
			ArrowBaseSize = aprops.baseSize;
			IntermHeadSize = aprops.intermSize;
			penColor = aprops.penColor;
			cascadeOrientation = aprops.cascadeOrientation;
			retainForm = aprops.retainForm;
			text = aprops.text;
			textColor = aprops.textColor;
			textStyle = aprops.textStyle;
			customDraw = aprops.customDraw;
			snapToNodeBorder = aprops.snapToNodeBorder;
			selStyle = aprops.selStyle;

			headPen = (MindFusion.FlowChartX.Pen)aprops.headPen.Clone();

			base.restoreProperties(props);
		}

		#endregion

		public float Length
		{
			get
			{
				PointCollection straightSegs = points;

				// use approximation if style is asBezier
				if (style == ArrowStyle.Bezier)
				{
					PointF[] ptArray = points.getArray();
					straightSegs = new PointCollection(0);

					for (int i = 0; i < SegmentCount; ++i)
					{
						straightSegs.AddRange(
							Utilities.approxBezier(ptArray, i * 3, 30));
					}
				}

				// sum up the length of straight segment lengths
				double total = 0;
				for (int s = 0; s < straightSegs.Count - 1; ++s)
				{
					PointF pt1 = straightSegs[s];
					PointF pt2 = straightSegs[s + 1];
					float dx = pt1.X - pt2.X;
					float dy = pt1.Y - pt2.Y;
					total += Math.Sqrt(dx*dx + dy*dy);
				}

				return (float)total;
			}
		}

		public bool Intersects(Node node)
		{
			PointCollection outline = node.getOutlinePoly();

			for (int i = 0; i < points.Count - 1; ++i)
			{
				for (int j = 0; j < outline.Count; ++j)
				{
					PointF a1 = points[i];
					PointF a2 = points[i + 1];
					PointF n1 = outline[j];
					PointF n2 = outline[(j + 1) % outline.Count];

					PointF ip = new PointF(0, 0);
					if (Utilities.segmentIntersect(a1, a2, n1, n2, ref ip))
						return true;
				}
			}

			return false;
		}

		#region crossings

		/// <summary>
		/// Stores a list of rectangles and a list of angles,
		/// which contain the destination of the arrow text.
		/// </summary>
		private class TextLayout
		{
			public TextLayout()
			{
				Rectangles = new ArrayList();
				Angles = new ArrayList();
				Strings = new ArrayList();
			}

			public ArrayList Rectangles;
			public ArrayList Angles;
			public ArrayList Strings;
		}

		/// <summary>
		/// stores a collection of PointCollection, each PointCollection
		/// holding the crossing points detected for correspondent arrow segment
		/// </summary>
		internal class ArrowCrossings
		{
			internal ArrowCrossings(int segments)
			{
				segmentCrossings = new ArrayList(segments);
				for (int i = 0; i < segments; ++i)
					segmentCrossings.Add(new PointCollection(0));
			}

			internal ArrayList segmentCrossings;
		};

		internal void resetCrossings()
		{
			if (fcParent.ArrowCrossings ==
				MindFusion.FlowChartX.ArrowCrossings.Straight) return;
			RectangleF rcArrow = getBoundingRect();

			freeData(Constants.ARROW_CROSSINGS);

			// z index where this arrow is or will be placed
			int z = ZIndex;
			if (!constructed)
				z = fcParent.Objects.Count;

			ArrowCollection arrows;
			if (fcParent.ArrowCrossings ==
				MindFusion.FlowChartX.ArrowCrossings.Arcs)
				arrows = fcParent.getArrowsFromZ(false, z);
			else
				arrows = fcParent.getArrowsFromZ(true, z);

			for (int i = 0; i < arrows.Count; ++i)
			{
				Arrow arrow = arrows[i];
				RectangleF rcTest = arrow.getBoundingRect();
				if (rcTest.IntersectsWith(rcArrow))
					arrow.freeData(Constants.ARROW_CROSSINGS);
			}

			if (modifying) fcParent.redrawNonModifiedItems();
		}

		private class CloserDistance : IComparer
		{
			public CloserDistance(PointF compareWith)
			{
				this.compareWith = compareWith;
			}

			public int Compare(object a, object b)
			{
				if (a == b)
					return 0;

				PointF _Left = (PointF)a;
				PointF _Right = (PointF)b;

				if (_Left.Equals(_Right))
					return 0;

				float dl = (compareWith.X - _Left.X)*(compareWith.X - _Left.X);
				dl += (compareWith.Y - _Left.Y)*(compareWith.Y - _Left.Y);

				float dr = (compareWith.X - _Right.X)*(compareWith.X - _Right.X);
				dr += (compareWith.Y - _Right.Y)*(compareWith.Y - _Right.Y);

				if (Math.Abs(dl - dr) < float.Epsilon) return 0;
				return (dl < dr) ? -1 : 1;
			}


			private PointF compareWith;
		}

		private ArrowCrossings getCrossings()
		{
			float crad = fcParent.CrossingRadius;
			RectangleF rcArrow = getBoundingRect();
			PointCollection intersections = new PointCollection(0);

			ArrowCrossings ac = (ArrowCrossings)getData(Constants.ARROW_CROSSINGS);
			if (ac == null)
			{
				ac = new ArrowCrossings(segmentCount);
				setData(Constants.ARROW_CROSSINGS, ac);

				// z index where this arrow is or will be placed
				int z = ZIndex;
				if (!constructed)
					z = fcParent.Objects.Count;

				// get the arrows which should be checked for intersections
				ArrowCollection arrows;
				if (fcParent.ArrowCrossings ==
					MindFusion.FlowChartX.ArrowCrossings.Arcs)
					arrows = fcParent.getArrowsFromZ(true, z);
				else
					arrows = fcParent.getArrowsFromZ(false, z);

				// check each segment
				for (int sgt = 0; sgt < segmentCount; ++sgt)
				{
					PointF pt1 = Points[sgt];
					PointF pt2 = Points[sgt+1];
					if (pt1 == pt2) continue;

					// find intersecting points with each arrow
					for (int i = 0; i < arrows.Count; ++i)
					{
						Arrow arrow = arrows[i];
						if (!arrow.Visible || arrow.Style == ArrowStyle.Bezier)
							continue;

						// dont check segments if arrow bounding rects dont intersect at all
						RectangleF rcTest = arrow.getBoundingRect();
						if (!rcTest.IntersectsWith(rcArrow))
							continue;

						for (int test = 0; test < arrow.SegmentCount; ++test)
						{
							PointF testPt1 = arrow.Points[test];
							PointF testPt2 = arrow.Points[test+1];
							if (testPt1 == testPt2) continue;

							PointF intersection = new PointF(0, 0);
							if (Utilities.segmentIntersect(pt1, pt2,
								testPt1, testPt2, ref intersection))
							{
								if (Utilities.Distance(pt1, intersection) > crad &&
									Utilities.Distance(pt2, intersection) > crad)
									intersections.Add(intersection);
							}
						}
					}

					// sort by distance to the first point
					intersections.Sort(new CloserDistance(pt1));

					// add radial intersections to the runtime intersection data
					CloserDistance closer = new CloserDistance(pt1);
					PointCollection rintr = ac.segmentCrossings[sgt] as PointCollection;
					rintr.Add(pt1);
					for (int ptc = 0; ptc < intersections.Count; ++ptc)
					{
						PointF ptRes1, ptRes2, pt = intersections[ptc];
						ptRes1 = ptRes2 = pt;
						RectangleF rc = RectangleF.FromLTRB(
							pt.X - crad, pt.Y - crad, pt.X + crad, pt.Y + crad);
						Utilities.getEllipseIntr(rc, pt1, pt, ref ptRes1);
						Utilities.getEllipseIntr(rc, pt2, pt, ref ptRes2);
						if (closer.Compare(ptRes1, ptRes2) < 0)
						{
							rintr.Add(ptRes1);
							rintr.Add(ptRes2);
						}
						else
						{
							rintr.Add(ptRes2);
							rintr.Add(ptRes1);
						}
					}
					rintr.Add(pt2);

					// Check if there are intersection that overlap
					for (int i = 1; i < rintr.Count - 2; )
					{
						PointF p1 = rintr[i];
						PointF p2 = rintr[i + 1];

						if (closer.Compare(p1, p2) > 0 ||
							Utilities.Distance(p1, p2) < Constants.getMillimeter(fcParent.MeasureUnit) / 2)
						{
							// Remove these points
							rintr.RemoveAt(i);
							rintr.RemoveAt(i);
						}
						else
						{
							i++;
						}
					}

					intersections.Clear();

				}	// for (int sgt = 0; sgt < GetSegments(); ++sgt)
			}

			return ac;
		}

		internal override void onChangeZ()
		{
			resetCrossings();
		}

		/// <summary>
		/// todo: move this someplace else.
		/// </summary>
		private double Rad(double degree)
		{
			return Math.PI * degree / 180;
		}

		/// <summary>
		/// Calculates how many characters from the specified
		/// text would fit in the specified space when drawn
		/// with the specified font.
		/// </summary>
		private int Fit(string text, float width, Font font, Graphics graphics)
		{
			string ctext = text;
			int chars = 0;

			if (ctext.Length == 0)
				return 0;

			StringFormat f = new StringFormat();
			f.SetMeasurableCharacterRanges(
				new CharacterRange[]
				{
					new CharacterRange(0, 1)
				});
			RectangleF rect = new RectangleF(0, 0, int.MaxValue, int.MaxValue);

			float totalWidth = 0;
			while (true)
			{
				if (ctext.Length == 0)
					return chars;

				totalWidth += graphics.MeasureCharacterRanges(
					ctext, font, rect, f)[0].GetBounds(graphics).Width;

				if (totalWidth > width)
					return Math.Max(0, chars);

				chars++;
				ctext = ctext.Substring(1);
			}
		}

		/// <summary>
		/// Updates the arrow's text.
		/// </summary>
		private void updateText()
		{
			if (textStyle == ArrowTextStyle.Follow)
			{
				textLayout = new TextLayout();
				PointF[] poly = points.getArray();

				if (style == ArrowStyle.Bezier)
				{
					GraphicsPath temp = new GraphicsPath();
					temp.AddBeziers(poly);
					temp.Flatten(new Matrix(), 1f);
					poly = temp.PathPoints;
					temp.Dispose();
				}

				// Check whether to reverse the arrow points
				if (poly[0].X > poly[poly.Length - 1].X)
				{
					for (int i = 0; i < poly.Length / 2; i++)
					{
						PointF t = poly[i];
						poly[i] = poly[poly.Length - i - 1];
						poly[poly.Length - i - 1] = t;
					}
				}

				Graphics graphics = fcParent.CreateGraphics();
				fcParent.setTransforms(graphics);

				float textHeight = Font.GetHeight(graphics);

				PointF startPoint = poly[0];
				for (int i = 0; i < poly.Length - 1; i++)
				{
					// The processed segment
					PointF p = poly[i];
					PointF t = poly[i + 1];

					float a = 0;
					float r = 0;
					GeoConvert.DekartToPolar(t, p, ref a, ref r);

					// Find the angle between the segments
					float angle = 0;

					// The angle for the last segment remains 0
					float a2 = 0;
					float r2 = 0;
					if (i < poly.Length - 2)
					{
						// The next segment
						PointF p2 = poly[i + 1];
						PointF t2 = poly[i + 2];

						GeoConvert.DekartToPolar(p2, t2, ref a2, ref r2);

						if (a2 < a)
							a2 += 360;

						angle = a2 - a;
					}

					// Calculate the real length (the start point might have been offset)
					float rr = 0;
					float aa = 0;
					GeoConvert.DekartToPolar(startPoint, t, ref aa, ref rr);

					PointF d1 = startPoint;
					PointF d2 = t;
					PointF d3 = PointF.Empty;
					PointF d4 = PointF.Empty;

					float width = 0;
					if (angle < 180)
					{
						// No restrictions
						GeoConvert.PolarToDekart(d1, a - 90, textHeight, ref d4);
						GeoConvert.PolarToDekart(d2, a - 90, textHeight, ref d3);

						width = rr;

						startPoint = t;
					}
					else
					{
						GeoConvert.PolarToDekart(d1, a - 90, textHeight, ref d4);

						float alpha2 = (360 - angle) / 2;
						float xx = textHeight / (float)Math.Sin(Rad(alpha2));
						float xxProj = (float)Math.Sqrt(Math.Pow(xx, 2) - Math.Pow(textHeight, 2));

						if (xxProj > rr)
						{
							xxProj = rr;
							xx = (float)Math.Sqrt(Math.Pow(xxProj, 2) + Math.Pow(textHeight, 2));
						}

						GeoConvert.PolarToDekart(d2, a - alpha2, xx, ref d3);
						GeoConvert.PolarToDekart(d2, a, xxProj, ref d2);

						width = rr - xxProj;

						if (xxProj > r2)
							xxProj = r2;
						GeoConvert.PolarToDekart(t, a2, xxProj, ref startPoint);
					}

					textLayout.Rectangles.Add(new RectangleF(d4.X, d4.Y, width, textHeight));
					textLayout.Angles.Add(180 - a);
				}

				string tempText = this.text;
				for (int i = 0; i < textLayout.Rectangles.Count; i++)
				{
					RectangleF rect = (RectangleF)textLayout.Rectangles[i];
					float angle = (float)textLayout.Angles[i];

					int chars = Fit(tempText, rect.Width, Font, graphics);
					if (chars == 0)
					{
						textLayout.Strings.Add("");
						continue;
					}

					string part = tempText.Substring(0, chars);
					tempText = tempText.Substring(chars);

					textLayout.Strings.Add(part);
				}

				graphics.Dispose();
			}
		}

		private TextLayout textLayout;

		/// <summary>
		/// Handles the FlowChart.MeasureUnitChanged event.
		/// </summary>
		private void OnParentUnitChanged(object sender, EventArgs e)
		{
			updateText();
		}

		private EventHandler parentUnitChanged = null;

		#endregion

		/// <summary>
		/// used when "attach items to longest H segment" grouping style is used
		/// </summary>
		/// <returns> longest non-vertical segment index </returns>
		public int getLongestHorzSegment()
		{
			int maxLenIdx = 0;
			float maxLen = 0;

			int segmentIncr = style != ArrowStyle.Bezier ? 1 : 3;
			int currSegment = 0;
			for (int i = 0; i < points.Count - 1; i += segmentIncr)
			{
				// get segment end points
				PointF pt1 = points[i];
				PointF pt2 = points[i + segmentIncr];

				if (pt1.X != pt2.X)
				{
					// compare the max distance found by now with the current one
					float currLen = Utilities.Distance(pt1, pt2);
					if (currLen >= maxLen)
					{
						// we have a new max distance
						maxLen = currLen;
						maxLenIdx = currSegment;
					}
				}

				currSegment++;
			}

			return maxLenIdx;
		}

		/// <summary>
		/// returns the length of a segment
		/// </summary>
		private float getSegmentLength(int index)
		{
			switch (style)
			{
				case ArrowStyle.Polyline:
				case ArrowStyle.Cascading:
					return Utilities.Distance(points[index], points[index + 1]);
				case ArrowStyle.Bezier:
					// approximate the bezier segment to polyline
					int counter = index * 3;
					PointF[] segmentPoints = new PointF[4];
					segmentPoints[0] = points[counter++];
					segmentPoints[1] = points[counter++];
					segmentPoints[2] = points[counter++];
					segmentPoints[3] = points[counter++];
					PointCollection bezierApprox = Utilities.approxBezier(
						segmentPoints, 0, 30);

					// sum up the length of straight segment lengths
					double total = 0;
					for (int s = 0; s < bezierApprox.Count - 1; ++s)
					{
						PointF pt1 = bezierApprox[s];
						PointF pt2 = bezierApprox[s + 1];
						float dx = pt1.X - pt2.X;
						float dy = pt1.Y - pt2.Y;
						total += Math.Sqrt(dx*dx + dy*dy);
					}

					return (float)total;
			}

			return 0;
		}

		/// <summary>
		/// returns the index of a segment that has the longest length
		/// </summary>
		private int getLongestSegment()
		{
			int longest = 0;
			float maxLength = 0;
			for (int i = 0; i < SegmentCount; ++i)
			{
				float length = getSegmentLength(i);
				if (length > maxLength)
				{
					maxLength = length;
					longest = i;
				}
			}

			return longest;
		}

		/// <summary>
		/// returns the center point of a segment
		/// </summary>
		private PointF getSegmentCenter(int index)
		{
			switch (style)
			{
				case ArrowStyle.Bezier:
					int startIdx = index * 3;

					// get the points defining the bezier curve
					float x0 = points[0 + startIdx].X;
					float y0 = points[0 + startIdx].Y;
					float x1 = points[1 + startIdx].X;
					float y1 = points[1 + startIdx].Y;
					float x2 = points[2 + startIdx].X;
					float y2 = points[2 + startIdx].Y;
					float x3 = points[3 + startIdx].X;
					float y3 = points[3 + startIdx].Y;

					// x(t) = (1-t)^3 x0 + 3t(1-t)^2 x1 + 3t^2 (1-t) x2 + t^3 x3
					// y(t) = (1-t)^3 y0 + 3t(1-t)^2 y1 + 3t^2 (1-t) y2 + t^3 y3
					double t = 0.5;
					double q0 = (1-t)*(1-t)*(1-t);
					double q1 = 3*t*(1-t)*(1-t);
					double q2 = 3*t*t*(1-t);
					double q3 = t*t*t;
					double xt = q0*x0 + q1*x1 + q2*x2 + q3*x3;
					double yt = q0*y0 + q1*y1 + q2*y2 + q3*y3;

					return new PointF((float)xt, (float)yt);

				default:
					PointF pt1 = points[index];
					PointF pt2 = points[index + 1];
					return new PointF((pt1.X + pt2.X) / 2, (pt1.Y + pt2.Y) / 2);
			}
		}

		/// <summary>
		/// Type of custom drawing
		/// </summary>
		public CustomDraw CustomDraw
		{
			get
			{
				return customDraw;
			}
			set
			{
				if (customDraw != value)
				{
					customDraw = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		private CustomDraw customDraw;

		/// <summary>
		/// Gets or sets whether arrow end points automatically align to node borders
		/// </summary>
		public bool SnapToNodeBorder
		{
			get
			{
				return snapToNodeBorder;
			}
			set
			{
				if (snapToNodeBorder != value)
				{
					snapToNodeBorder = value;
					fcParent.setDirty();
				}
			}
		}

		public HandlesStyle HandlesStyle
		{
			get	{ return selStyle; }

			set
			{
				if (value != selStyle)
				{
					selStyle = value;
					fcParent.invalidate(getRepaintRect(false));
					fcParent.setDirty();
				}
			}
		}

		private bool snapToNodeBorder;
		private HandlesStyle selStyle;

		internal override PointCollection getOutlinePoly()
		{
			PointCollection poly = new PointCollection(0);
			GraphicsPath arrowPath = new GraphicsPath(FillMode.Alternate);

			// draw the arrow's line
			if (style == ArrowStyle.Bezier)
			{
				arrowPath.AddBeziers(points.getArray());
			}
			else
			{
				arrowPath.AddLines(points.getArray());
			}

			System.Drawing.Pen widenPen = new System.Drawing.Pen(Color.Black,
				2 * Constants.getMillimeter(fcParent.MeasureUnit));
			arrowPath.Widen(widenPen);
			arrowPath.Flatten();
			poly.AddRange(arrowPath.PathPoints);
			widenPen.Dispose();
			arrowPath.Dispose();

			return poly;
		}

	}	// class Arrow

}	// namespace MindFusion.FlowChartX