// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

using MindFusion.FlowChartX.Commands;
using MindFusion.FlowChartX.Visitors;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Abstract class representing diagram nodes. Implemented by Box and Table classes.
	/// </summary>
	public abstract class Node : ChartObject
	{
		public Node(FlowChart parent) : base(parent)
		{
			incomingArrows = new ArrowCollection();
			outgoingArrows = new ArrowCollection();
			anchorPattern = null;
			obstacle = true;
			constraints = new NodeConstraints();
			enabledHandles = Handles.All;
			boolValues = 0xFFFF;
		}

		public Node(Node prototype) : base(prototype)
		{
			incomingArrows = new ArrowCollection();
			outgoingArrows = new ArrowCollection();
			AnchorPattern = prototype.AnchorPattern;
			obstacle = prototype.obstacle;
			constraints = (NodeConstraints)prototype.constraints.Clone();
			enabledHandles = prototype.enabledHandles;
			boolValues = prototype.boolValues;
		}

		/// <summary>
		/// This method is used internally by the Ruler control to align nodes.
		/// </summary>
		/// <param name="rect">The new location of the node.</param>
		/// <param name="raiseModfEvent">Raise the xModified event?</param>
		public void SetRect(RectangleF rect, bool raiseModfEvent)
		{
			if (rect != this.rect)
				setRect(rect);

			if (raiseModfEvent)
			{
				flowChart.fireObjModified(
					this, Utilities.getCenter(BoundingRect), 8);
			}
		}

		internal void setRect(RectangleF rc)
		{
			RectangleF rcOld = rect;
			rect = Utilities.normalizeRect(rc);
			onRectUpdate();

			if (subordinateGroup == null)
			{
				visitArrows(
					new MethodCallVisitor(new VisitOperation(AV_UpdPosOutgoing)),
					new MethodCallVisitor(new VisitOperation(AV_UpdPosIncoming)));
			}
			else
			{
				modifyDX = rect.X - rcOld.X;
				modifyDY = rect.Y - rcOld.Y;
				subordinateGroup.beginModification(
					new InteractionState(this, 8, Action.Modify));
				visitArrows(
					new MethodCallVisitor(new VisitOperation(AV_UpdPosOutgoing)),
					new MethodCallVisitor(new VisitOperation(AV_UpdPosIncoming)),
					getExcludedArrows());
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));
				subordinateGroup.endModification();
			}
		}

		internal void moveWith(float dx, float dy)
		{
			RectangleF newRect = rect;
			newRect.Offset(dx, dy);
			setRect(newRect);
		}

		/// <summary>
		/// Defines the node's bounding rectangle
		/// </summary>
		public RectangleF BoundingRect
		{
			get { return getBoundingRect(); }
			set
			{
				if (value.Width == 0) return;
				if (value.Height == 0) return;

				flowChart.UndoManager.onStartPlacementChange(this);

				flowChart.invalidate(getRepaintRect(true));
				setRect(value);
				flowChart.setDirty();
				flowChart.invalidate(getRepaintRect(true));

				flowChart.UndoManager.onEndPlacementChange();
			}
		}

		public void Move(float x, float y)
		{
			RectangleF rcOld = BoundingRect;
			BoundingRect = new RectangleF(x, y, rcOld.Width, rcOld.Height);
		}

		internal virtual float rotation()
		{
			return 0;
		}

		internal void move(float x, float y)
		{
			RectangleF rcOld = BoundingRect;
			setRect(new RectangleF(x, y, rcOld.Width, rcOld.Height));
		}

		public void Resize(float width, float height)
		{
			RectangleF rcOld = BoundingRect;
			BoundingRect = new RectangleF(rcOld.X, rcOld.Y, width, height);
		}

		internal PointF getCenter()
		{
			PointF center = new PointF(
				(rect.X + rect.Right) / 2,
				(rect.Y + rect.Bottom) / 2);

			return center;
		}

		[System.ComponentModel.TypeConverter(
			 typeof(System.ComponentModel.ExpandableObjectConverter))]
		public NodeConstraints Constraints
		{
			get { return constraints; }
			set
			{
				constraints = value;
				if (constraints == null)
					constraints = new NodeConstraints();
			}
		}

		private NodeConstraints constraints;

		protected void updPosArrows()
		{
			visitArrows(
				new MethodCallVisitor(new VisitOperation(AV_UpdPosOutgoing)),
				new MethodCallVisitor(new VisitOperation(AV_UpdPosIncoming)));
		}

		internal override RectangleF getRepaintRect(bool includeConnected)
		{
			RectangleF repaintRect = base.getRepaintRect(true);
			RectangleF arrowRect;

			addIntrRepaintRect(ref repaintRect);

			// include anchor points
			float markSize = Constants.getMarkSize(flowChart.MeasureUnit);
			if (flowChart.RenderOptions.EnableAnchors && anchorPattern != null &&
				markSize > flowChart.SelHandleSize)
			{
				float markInfl = markSize - flowChart.SelHandleSize;
				repaintRect.Inflate(markInfl, markInfl);
			}

			// include arrows rectangles in the update area
			if (includeConnected)
			{
				foreach (Arrow arrow in outgoingArrows)
				{
					arrowRect = arrow.getRepaintRect(true);
					repaintRect = Utilities.unionRects(repaintRect, arrowRect);
          repaintRect = Utilities.unionRects(repaintRect, arrow.Origin.getBoundingRect());
          repaintRect = Utilities.unionRects(repaintRect, arrow.Destination.getBoundingRect());
        }
				foreach (Arrow arrow in incomingArrows)
				{
					arrowRect = arrow.getRepaintRect(true);
					repaintRect = Utilities.unionRects(repaintRect, arrowRect);
          repaintRect = Utilities.unionRects(repaintRect, arrow.Origin.getBoundingRect());
          repaintRect = Utilities.unionRects(repaintRect, arrow.Destination.getBoundingRect());
        }
			}

			// include the group objects rectangles in the update area
			if (subordinateGroup != null && includeConnected)
			{
				RectangleF rcGroup = subordinateGroup.getRepaintRect();
				repaintRect = Utilities.unionRects(rcGroup, repaintRect);
			}

			return repaintRect;
		}

		private ArrowCollection getExcludedArrows()
		{
			if (subordinateGroup == null) return null;
			if (subordinateGroup.ArrowsToMove == null) return null;
			if (subordinateGroup.ArrowsToMove.Count == 0) return null;

			return subordinateGroup.ArrowsToMove;
		}

		internal override void startCreate(PointF org)
		{
			org = flowChart.AlignPointToGrid(org);
			base.startCreate(org);
			rcSaved = rect;
		}

		internal override void updateCreate(PointF current)
		{
			current = flowChart.AlignPointToGrid(current);
			base.updateCreate(current);

			if (constraints.isNodeConstrained())
				rect = updateRect(rcSaved, current, 2);

			// call node-class specific code
			onRectUpdate();
		}

		internal override bool allowCreate(PointF current)
		{
			current = flowChart.AlignPointToGrid(current);

			RectangleF rect = constraints.isNodeConstrained() ?
				updateRect(rcSaved, current, 2) :
				RectangleF.FromLTRB(ptOrg.X, ptOrg.Y, current.X, current.Y);

			if (!allowRect(rect))
				return false;

			return flowChart.confirmCreate(this);
		}

		internal override void completeCreate(PointF end)
		{
			end = flowChart.AlignPointToGrid(end);
			base.completeCreate(end);

			if (constraints.isNodeConstrained())
			{
				rect = updateRect(rect, end, 2);
				rect = Utilities.normalizeRect(rect);
			}

			// call node-class specific code
			onRectUpdate();
		}

		internal override void startModify(PointF org, int handle, InteractionState ist)
		{
			if (cycleProtect)
			{
				cycleDetected = true;
				ist.cycleRoots.Add(this);
				return;
			}
			else
				cycleDetected = false;

			cycleProtect = true;

			base.startModify(org, handle, ist);

			visitArrows(
				new MethodCallVisitor(new VisitOperationIntr(AV_StartModOutgoing), ist),
				new MethodCallVisitor(new VisitOperationIntr(AV_StartModIncoming), ist));

			if (subordinateGroup != null)
				subordinateGroup.beginModification(ist);

			cycleProtect = false;
		}

		internal override void updateModify(PointF current, InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.updateModify(current, ist);
			if (modifyHandle != 9)
				rect = updateRect(rect, current, modifyHandle);

			onRectUpdate();

			visitArrows(
				new MethodCallVisitor(new VisitOperation(AV_UpdateOutgoing)),
				new MethodCallVisitor(new VisitOperation(AV_UpdateIncoming)),
				getExcludedArrows());

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(ist);

			cycleProtect = false;
		}

		internal override void modifyTranslate(float x, float y, bool arrows)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			// apply movement constraints
			if (constraints.MoveDirection == DirectionConstraint.Horizontal)
				y = 0;
			if (constraints.MoveDirection == DirectionConstraint.Vertical)
				x = 0;

			base.modifyTranslate(x, y, arrows);
			rect.Offset(modifyDX, modifyDY);

			// apply "stay inside parent" constraints
			if (constraints.KeepInsideParent &&
				masterGroup != null && masterGroup.MainObject is Node)
			{
				Node parent = masterGroup.MainObject as Node;
				RectangleF bounds = parent.BoundingRect;

				if (!bounds.Contains(Utilities.normalizeRect(rect)))
				{
					if (rect.Left < bounds.Left)
						rect.X = bounds.Left;
					if (rect.Top < bounds.Top)
						rect.Y = bounds.Top;
					if (rect.Right > bounds.Right)
						rect.X = bounds.Right - rect.Width;
					if (rect.Bottom > bounds.Bottom)
						rect.Y = bounds.Bottom - rect.Height;
				}
			}

			onRectUpdate();

			if (arrows)
			{
				visitArrows(
					new MethodCallVisitor(new VisitOperation(AV_UpdateOutgoing)),
					new MethodCallVisitor(new VisitOperation(AV_UpdateIncoming)));
			}

			if (subordinateGroup != null)
				subordinateGroup.updateObjects(new InteractionState(this, -1, Action.Modify));

			cycleProtect = false;
		}

		internal override bool allowModify(PointF current)
		{
			if (notInteractive())
				return false;

			RectangleF rc = modifyHandle != 9 ?
				Utilities.normalizeRect(updateRect(rect, current, modifyHandle)) :
				getBoundingRect();

			if (!allowRect(rc))
				return false;

			return flowChart.confirmModify(this);
		}

		private bool allowRect(RectangleF rect)
		{
			// don't allow too small boxes, it will be hard for the user to see them
			if (Math.Abs(rect.Width) < Constants.getMinObjSize(flowChart.MeasureUnit))
				return false;
			if (Math.Abs(rect.Height) < Constants.getMinObjSize(flowChart.MeasureUnit))
				return false;

			if (flowChart.rectRestrict(ref rect))
				return false;

			return true;
		}

		internal override void completeModify(PointF end, InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.completeModify(end, ist);
			if (modifyHandle != 9)
				rect = updateRect(rect, end, modifyHandle);
			rect = Utilities.normalizeRect(rect);
			onRectUpdate();

			modifying = true;
			visitArrows(
				new MethodCallVisitor(new VisitOperation(AV_EndModOutgoing)),
				new MethodCallVisitor(new VisitOperation(AV_EndModIncoming)));
			if (subordinateGroup != null)
			{
				subordinateGroup.updateObjects(ist);
				subordinateGroup.endModification();
			}
			modifying = false;

			flowChart.routeAllArrows(this);

			flowChart.fireObjModified(this, end, modifyHandle);
			cycleProtect = false;
		}

		RectangleF makeRect(float x1, float y1, float x2, float y2)
		{
			return RectangleF.FromLTRB(x1, y1, x2, y2);
		}

		RectangleF updateRect(RectangleF rc, PointF point, int modifyHandle)
		{
			// if a user is currently moving the node ...
			if (modifyHandle == 8)
			{
				rc = rcSaved;
				PointF startPt = flowChart.InteractionStartPoint;
				rc.Offset(point.X - startPt.X, point.Y - startPt.Y);
				float w = rc.Width;
				float h = rc.Height;

				PointF ptTopLeft = new PointF(rc.X, rc.Y);
				ptTopLeft = flowChart.AlignPointToGrid(ptTopLeft);

				RectangleF newPos = RectangleF.FromLTRB(
					ptTopLeft.X, ptTopLeft.Y,
					ptTopLeft.X + w, ptTopLeft.Y + h);

				// if constrained to move only horizontally ...
				if (constraints.MoveDirection == DirectionConstraint.Horizontal)
					newPos.Y = rcSaved.Y;

				// if constrained to move only vertically ...
				if (constraints.MoveDirection == DirectionConstraint.Vertical)
					newPos.X = rcSaved.X;

				// apply "stay inside parent" constraints
				if (constraints.KeepInsideParent &&
					masterGroup != null && masterGroup.MainObject is Node)
				{
					Node parent = masterGroup.MainObject as Node;
					RectangleF bounds = parent.BoundingRect;

					if (!bounds.Contains(Utilities.normalizeRect(newPos)))
					{
						if (newPos.Left < bounds.Left)
							newPos.X = bounds.Left;
						if (newPos.Top < bounds.Top)
							newPos.Y = bounds.Top;
						if (newPos.Right > bounds.Right)
							newPos.X = bounds.Right - newPos.Width;
						if (newPos.Bottom > bounds.Bottom)
							newPos.Y = bounds.Bottom - newPos.Height;
					}
				}

				return newPos;
			}

			PointF pivot = Utilities.getCenter(rc);

			// the node is being resized
			if (rotation() == 0)
				point = flowChart.AlignPointToGrid(point);
			switch (modifyHandle)
			{
				case 0:
					rc = makeRect(point.X, point.Y, rc.Right, rc.Bottom);
					break;
				case 1:
					rc = makeRect(rc.Left, point.Y, point.X, rc.Bottom);
					break;
				case 2:
					rc = makeRect(rc.Left, rc.Top, point.X, point.Y);
					break;
				case 3:
					rc = makeRect(point.X, rc.Top, rc.Right, point.Y);
					break;
				case 4:
					rc = makeRect(rc.Left, point.Y, rc.Right, rc.Bottom);
					break;
				case 5:
					rc = makeRect(rc.Left, rc.Top, point.X, rc.Bottom);
					break;
				case 6:
					rc = makeRect(rc.Left, rc.Top, rc.Right, point.Y);
					break;
				case 7:
					rc = makeRect(point.X, rc.Top, rc.Right, rc.Bottom);
					break;
			}

			// apply width constraints
			if (constraints.MinWidth > 0 || constraints.MaxWidth > 0)
			{
				switch (modifyHandle)
				{
				case 0:	// top left
				case 3:	// bottom left
				case 7: // middle left

					// left side being moved
					if (constraints.MinWidth > 0 && Math.Abs(rc.Width) < constraints.MinWidth)
						rc = RectangleF.FromLTRB(rc.Right - Math.Abs(constraints.MinWidth) * (rc.Left > rc.Right ? -1 : 1), rc.Top, rc.Right, rc.Bottom);
					if (constraints.MaxWidth > 0 && Math.Abs(rc.Width) > constraints.MaxWidth)
						rc = RectangleF.FromLTRB(rc.Right - Math.Abs(constraints.MaxWidth) * (rc.Left > rc.Right ? -1 : 1), rc.Top, rc.Right, rc.Bottom);
					break;

				case 1:	// top right
				case 2:	// bottom right
				case 5:	// middle right

					// right side being moved
					if (constraints.MinWidth > 0 && Math.Abs(rc.Width) < constraints.MinWidth)
						rc = RectangleF.FromLTRB(rc.Left, rc.Top, rc.Left + Math.Abs(constraints.MinWidth) * (rc.Left > rc.Right ? -1 : 1), rc.Bottom);
					if (constraints.MaxWidth > 0 && Math.Abs(rc.Width) > constraints.MaxWidth)
						rc = RectangleF.FromLTRB(rc.Left, rc.Top, rc.Left + Math.Abs(constraints.MaxWidth) * (rc.Left > rc.Right ? -1 : 1), rc.Bottom);
					break;
				}
			}

			// apply ratio constraints
			float ratio = 1;
			if (constraints.KeepRatio)
			{
				if (rcSaved.Width > 0 && rcSaved.Height > 0)
					ratio = rcSaved.Width / rcSaved.Height;
				if (!(modifyHandle == 4 || modifyHandle == 6))
					rc.Height = rc.Width / ratio;
			}

			// apply height constraints
			if (!constraints.KeepRatio)
			{
				if (constraints.MinHeight > 0 || constraints.MaxHeight > 0)
				{				
					switch (modifyHandle)
					{
					case 0:	// top left
					case 1:	// top right
					case 4:	// top center

						// top side being moved
						if (constraints.MinHeight > 0 && Math.Abs(rc.Height) < constraints.MinHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Bottom - Math.Abs(constraints.MinHeight) * (rc.Top > rc.Bottom ? -1 : 1), rc.Right, rc.Bottom);
						if (constraints.MaxHeight > 0 && Math.Abs(rc.Height) > constraints.MaxHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Bottom - Math.Abs(constraints.MaxHeight) * (rc.Top > rc.Bottom ? -1 : 1), rc.Right, rc.Bottom);
						break;
	
					case 2:	// bottom right
					case 3:	// bottom left
					case 6:	// bottom center

						// bottom side being moved
						if (constraints.MinHeight > 0 && Math.Abs(rc.Height) < constraints.MinHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Top, rc.Right, rc.Top + Math.Abs(constraints.MinHeight) * (rc.Top > rc.Bottom ? -1 : 1));
						if (constraints.MaxHeight > 0 && Math.Abs(rc.Height) > constraints.MaxHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Top, rc.Right, rc.Top + Math.Abs(constraints.MaxHeight) * (rc.Top > rc.Bottom ? -1 : 1));
						break;
					}
				}
			}
			else
			{
				if (constraints.MinWidth > 0 || constraints.MaxWidth > 0)
				{
					float minHeight = constraints.MinWidth / ratio;
					float maxHeight = constraints.MaxWidth / ratio;

					switch (modifyHandle)
					{
					case 4:	// top center
						if (minHeight > 0 && Math.Abs(rc.Height) < minHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Bottom - Math.Abs(minHeight) * (rc.Top > rc.Bottom ? -1 : 1), rc.Right, rc.Bottom);
						if (maxHeight > 0 && Math.Abs(rc.Height) > maxHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Bottom - Math.Abs(maxHeight) * (rc.Top > rc.Bottom ? -1 : 1), rc.Right, rc.Bottom);
						break;
		
					case 6:	// bottom center
						if (minHeight > 0 && Math.Abs(rc.Height) < minHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Top, rc.Right, rc.Top + Math.Abs(minHeight) * (rc.Top > rc.Bottom ? -1 : 1));
						if (maxHeight > 0 && Math.Abs(rc.Height) > maxHeight)
							rc = RectangleF.FromLTRB(rc.Left, rc.Top, rc.Right, rc.Top + Math.Abs(maxHeight) * (rc.Top > rc.Bottom ? -1 : 1));
						break;
					}
				}
			}

			// apply ratio constraints
			if (constraints.KeepRatio)
			{
				if (modifyHandle == 4 || modifyHandle == 6)
					rc.Width = rc.Height * ratio;
			}

			// apply "stay inside parent" constraints
			bool insideParentApplied = false;
			if (constraints.KeepInsideParent &&
				masterGroup != null && masterGroup.MainObject is Node)
			{
				Node parent = masterGroup.MainObject as Node;
				RectangleF bounds = parent.BoundingRect;

				if (!bounds.Contains(Utilities.normalizeRect(rc)))
				{
					bool revx = rc.Width < 0;
					bool revy = rc.Height < 0;
					rc = Utilities.normalizeRect(rc);
					rc = RectangleF.FromLTRB(
						Math.Min(Math.Max(bounds.Left, rc.Left), bounds.Right),
						Math.Min(Math.Max(bounds.Top, rc.Top), bounds.Bottom),
						Math.Max(Math.Min(bounds.Right, rc.Right), bounds.Left),
						Math.Max(Math.Min(bounds.Bottom, rc.Bottom), bounds.Top));
					rc = Utilities.normalizeRect(rc);
					if (revx)
					{
						rc.X = rc.Right;
						rc.Width = -rc.Width;
					}
					if (revy)
					{
						rc.Y = rc.Bottom;
						rc.Height = -rc.Height;
					}
					insideParentApplied = true;
				}
			}

			// apply ratio constraints, again
			if (constraints.KeepRatio && insideParentApplied)
			{
				float newRatio = 1;
				if (rc.Width != 0 && rc.Height != 0)
				{
					newRatio = rc.Width / rc.Height;

					if (Math.Abs(newRatio) < ratio)
						rc.Height = rc.Width / ratio;
					else if (Math.Abs(newRatio) > ratio)
						rc.Width = rc.Height * ratio;
				}
			}

			if (rotation() != 0)
			{
				PointF pt1 = new PointF(rc.Left, rc.Top);
				PointF pt2 = new PointF(rc.Right, rc.Bottom);
				pt1 = Utilities.rotatePointAt(pt1, pivot, rotation());
				pt2 = Utilities.rotatePointAt(pt2, pivot, rotation());
				pivot = new PointF((pt1.X + pt2.X) / 2, (pt1.Y + pt2.Y) / 2);
				pt1 = Utilities.rotatePointAt(pt1, pivot, -rotation());
				pt2 = Utilities.rotatePointAt(pt2, pivot, -rotation());
				rc = RectangleF.FromLTRB(pt1.X, pt1.Y, pt2.X, pt2.Y);
			}

			return rc;
		}

		internal override void completeModify()
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.completeModify();

			EndModifyVisitor emv = new EndModifyVisitor();
			visitArrows(emv);

			if (subordinateGroup != null) subordinateGroup.endModification();

			cycleProtect = false;
		}

		internal override void cancelModify(InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			base.cancelModify(ist);

			visitArrows(
				new MethodCallVisitor(new VisitOperationIntr(AV_Cancel), ist));

			onRectUpdate();

			if (subordinateGroup != null)
				subordinateGroup.cancelModification(ist);

			cycleProtect = false;
		}

		internal virtual bool canHaveArrows(bool outgoing)
		{
			if (outgoing && !AllowOutgoingArrows)
				return false;
			if (!outgoing && !AllowIncomingArrows)
				return false;

			if (flowChart.AllowUnanchoredArrows)
				return true;

			if (anchorPattern == null) return false;
			return anchorPattern.allowArrowsDir(outgoing, this);
		}

		internal void updateArrowsIntsc()
		{
			visitArrows(
				new MethodCallVisitor(new VisitOperation(AV_UpdIntscOutgoing)),
				new MethodCallVisitor(new VisitOperation(AV_UpdIntscIncoming)));
		}

		internal void setPos(float x, float y, float w, float h)
		{
			ptOrg.X = x;
			ptOrg.Y = y;
			ptEnd.X = x + w;
			ptEnd.Y = y + h;

			rectFromPoints();
		}

		protected abstract void onRectUpdate();

		internal override Cursor getCannotDropCursor()
		{
			return flowChart.CurCannotCreate;
		}

		internal override Cursor getCanDropCursor()
		{
			if (!constructed)
				return flowChart.CurPointer;

			return null;
		}

		internal virtual void getAllOutgoingArrows(ArrowCollection ac)
		{
			foreach (Arrow a in outgoingArrows)
				if (!ac.Contains(a))
					ac.Add(a);
		}

		internal virtual void getAllIncomingArrows(ArrowCollection ac)
		{
			foreach (Arrow a in incomingArrows)
				if (!ac.Contains(a))
					ac.Add(a);
		}

		public ArrowCollection GetAllArrows()
		{
			ArrowCollection ac = new ArrowCollection();
			getAllIncomingArrows(ac);
			getAllOutgoingArrows(ac);
			return ac;
		}

		internal virtual void visitArrows(CollectionVisitor visitor)
		{
			foreach (Arrow arrow in outgoingArrows)
				visitor.accept(arrow);
			foreach (Arrow arrow in incomingArrows)
				visitor.accept(arrow);
		}

		internal virtual void visitArrows(CollectionVisitor outgVstr, CollectionVisitor incmVstr)
		{
			foreach (Arrow arrow in outgoingArrows)
				outgVstr.accept(arrow);
			foreach (Arrow arrow in incomingArrows)
				incmVstr.accept(arrow);
		}

		internal virtual void visitArrows(
			CollectionVisitor outgVstr,
			CollectionVisitor incmVstr,
			ArrowCollection excluded)
		{
			if (excluded == null)
			{
				visitArrows(outgVstr, incmVstr);
				return;
			}

			foreach (Arrow arrow in outgoingArrows)
				if (!excluded.Contains(arrow))
					outgVstr.accept(arrow);
			foreach (Arrow arrow in incomingArrows)
				if (!excluded.Contains(arrow))
					incmVstr.accept(arrow);
		}

		internal override void visitHierarchy(CollectionVisitor visitor)
		{
			base.visitHierarchy(visitor);
	
			foreach (Arrow arrow in outgoingArrows)
				arrow.visitHierarchy(visitor);
			foreach (Arrow arrow in incomingArrows)
				arrow.visitHierarchy(visitor);
		}

		protected void AV_UpdateOutgoing(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			arrow.updatePosFromOrgAndDest(true);
		}

		protected void AV_UpdateIncoming(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			if (!arrow.isReflexive())
			{
				arrow.updatePosFromOrgAndDest(true);
			}
		}

		protected void AV_UpdPosOutgoing(ChartObject obj)
		{
			flowChart.UndoManager.onPlacementChange(obj);

			Arrow arrow = (Arrow)obj;
			arrow.updatePosFromOrgAndDest(false);
		}

		protected void AV_UpdPosIncoming(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			if (!arrow.isReflexive())
			{
				flowChart.UndoManager.onPlacementChange(obj);
				arrow.updatePosFromOrgAndDest(false);
			}
		}

		protected void AV_UpdIntscOutgoing(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			arrow.updateIntersections();
		}

		protected void AV_UpdIntscIncoming(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			if (!arrow.isReflexive())
				arrow.updateIntersections();
		}

		protected void AV_StartModOutgoing(ChartObject obj, InteractionState ist)
		{
			Arrow arrow = (Arrow)obj;
			arrow.startModifyOrg(ist);
		}

		protected void AV_StartModIncoming(ChartObject obj, InteractionState ist)
		{
			Arrow arrow = (Arrow)obj;
			arrow.startModifyEnd(ist);
		}

		protected void AV_EndModOutgoing(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			arrow.updatePosFromOrgAndDest(true);
			arrow.completeModify();
		}

		protected void AV_EndModIncoming(ChartObject obj)
		{
			Arrow arrow = (Arrow)obj;
			if (!arrow.isReflexive())
			{
				arrow.updatePosFromOrgAndDest(true);
				arrow.completeModify();
			}
		}

		protected void AV_Cancel(ChartObject obj, InteractionState ist)
		{
			Arrow arrow = (Arrow)obj;
			arrow.cancelModify(ist);
		}

		public ArrowCollection IncomingArrows
		{
			get { return incomingArrows; }
		}

		public ArrowCollection OutgoingArrows
		{
			get { return outgoingArrows; }
		}

		internal void addIncomingArrow(Arrow arrow)
		{
			if (!incomingArrows.Contains(arrow))
				incomingArrows.Add(arrow);
		}

		internal void addOutgoingArrow(Arrow arrow)
		{
			if (!outgoingArrows.Contains(arrow))
				outgoingArrows.Add(arrow);
		}

		internal virtual void deleteArrows()
		{
			ArrowCollection toDelete = new ArrowCollection();
			getAllIncomingArrows(toDelete);
			getAllOutgoingArrows(toDelete);

			foreach (Arrow arrow in toDelete)
				flowChart.deleteItem(arrow);
		}

		internal virtual void removeIncomingArrow(Arrow arrow)
		{
			incomingArrows.Remove(arrow);
		}

		internal virtual void removeOutgoingArrow(Arrow arrow)
		{
			outgoingArrows.Remove(arrow);
		}

		protected internal ArrowCollection incomingArrows;
		protected internal ArrowCollection outgoingArrows;

		internal virtual bool alreadyLinked(Node node)
		{
			foreach (Arrow arrow in outgoingArrows)
			{
				if (arrow.getDestLink().getNode() == node)
					return true;
			}

			return false;
		}

		#region anchor points

		public AnchorPattern AnchorPattern
		{
			get { return anchorPattern; }
			set
			{
				if (value == null || value.isRegistered)
					anchorPattern = value;
				else
					anchorPattern = (AnchorPattern)value.Clone();
			}
		}

		protected internal AnchorPattern anchorPattern;
		
		internal virtual PointF getAnchor(PointF pt,
			Arrow arrow, bool incm, ref int idx)
		{
			return getNearestAnchor(pt, arrow, incm, ref idx);
		}

		internal PointF getAnchor(ArrowAnchor anchor)
		{
			return getAnchor(anchor, true);
		}

		internal PointF getAnchor(ArrowAnchor anchor, bool useRotation)
		{
			PointF center = getCenter();
			PointF intrs = center;

			// find intersection with node outline
			float width = Math.Abs(rect.Width);
			float height = Math.Abs(rect.Height);
			switch(anchor)
			{
				case ArrowAnchor.LeftCenter:
					intrs.X -= width + 10;
					break;
				case ArrowAnchor.TopCenter:
					intrs.Y -= height + 10;
					break;
				case ArrowAnchor.RightCenter:
					intrs.X += width + 10;
					break;
				case ArrowAnchor.BottomCenter:
					intrs.Y += height + 10;
					break;
			}

			if (rotation() != 0 && useRotation)
				intrs = Utilities.rotatePointAt(intrs, center, rotation());

			return getIntersection(intrs, center);
		}

		internal PointF getAnchorPos(int anchorIdx)
		{
			RectangleF nodeRect = getBoundingRect();
			AnchorPoint ap = anchorPattern.Points[anchorIdx];

			PointF point = ap.getPos(nodeRect);
			if (rotation() != 0)
				point = Utilities.rotatePointAt(point, getCenter(), rotation());

			return point;
		}

		internal virtual PointF getNearestAnchor(PointF pt,
			Arrow arrow, bool incoming, ref int anchorIdx)
		{
			anchorIdx = -1;
			if (anchorPattern == null) return pt;

			if (rotation() != 0)
				pt = Utilities.rotatePointAt(pt, getCenter(), -rotation());
			PointF nearestPt = pt;

			RectangleF nodeRect = getBoundingRect();
			float nearestDist = 2 * Math.Max(nodeRect.Width, nodeRect.Height);

			for (int i = 0; i < anchorPattern.Points.Count; i++)
			{
				AnchorPoint ap = anchorPattern.Points[i];
				if (incoming && !ap.AllowIncoming) continue;
				if (!incoming && !ap.AllowOutgoing) continue;
				if (!flowChart.validateAnchor(arrow, !incoming, this, i)) continue;

				PointF pos = ap.getPos(nodeRect);
				float dx = pos.X - pt.X;
				float dy = pos.Y - pt.Y;
				float ptDist = (float)Math.Sqrt(dx*dx + dy*dy);
				if (ptDist < nearestDist)
				{
					nearestDist = ptDist;
					nearestPt = pos;
					anchorIdx = i;
				}
			}

			if (rotation() != 0)
				nearestPt = Utilities.rotatePointAt(nearestPt, getCenter(), rotation());

			return nearestPt;
		}

		internal virtual void drawAnchors(Graphics g)
		{
			if (anchorPattern == null) return;
			for (int i = 0; i < anchorPattern.Points.Count; ++i)
			{
				AnchorPoint ap = anchorPattern.Points[i];
				MarkDrawArgs args = null;
				if (ap.MarkStyle == MarkStyle.Custom)
					args = new MarkDrawArgs(g, new PointF(0, 0), this, anchorPattern, i);
				ap.draw(g, getBoundingRect(), rotation(),
					args, Constants.getMarkSize(flowChart.MeasureUnit));
			}
		}

		#endregion

		internal abstract PointF getIntersection(PointF ptOrg, PointF ptEnd);

		internal PointF getTopMiddleIntr()
		{
			PointF center = getCenter();
			PointF upper = new PointF(center.X, center.Y - 3*Math.Abs(rect.Height));
			PointF intr = getIntersection(upper, center);
			RectangleF bounds = getRotatedBounds();
			if (!bounds.Contains(intr))
				intr = new PointF((bounds.Left + bounds.Right) / 2, bounds.Top);
			return intr;
		}

		internal PointF getTopIntr(float xfactor)
		{
			RectangleF bounds = getRotatedBounds();
			PointF center = new PointF(bounds.Left, (bounds.Top + bounds.Bottom) / 2);
			center.X += xfactor * bounds.Width;
			PointF upper = new PointF(center.X, center.Y - 3*Math.Abs(rect.Height));
			PointF intr = getIntersection(upper, center);
			if (!bounds.Contains(intr))
				intr = new PointF(bounds.Left + bounds.Width * xfactor, bounds.Top);
			return intr;
		}

		internal abstract Link createLink(Arrow arrow, PointF pt, bool incoming);


		// inplace editing
		internal abstract string getTextToEdit();
		internal abstract void setEditedText(string newText);
		internal abstract RectangleF getEditRect();

		internal bool Frozen
		{
			get { return IgnoreLayout | Invisible; }
		}

		public Handles EnabledHandles
		{
			get { return enabledHandles; }
			set
			{
				if (enabledHandles != value)
				{
					enabledHandles = value;
					flowChart.setDirty();
          if (!flowChart.LayoutSuspended)
            flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		protected internal Handles enabledHandles;

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			// new in save format 15
			writer.Write(obstacle);
			writer.Write(hyperLink);

			// new in save format 17
			constraints.saveTo(writer, ctx);

			// new in save format 27
			writer.Write(boolValues);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			if (ctx.FileVersion > 14)
			{
				// new in save format 15
				obstacle = reader.ReadBoolean();
				hyperLink = reader.ReadString();

				if (ctx.FileVersion > 16)
				{
					// new in save format 17
					constraints.loadFrom(reader, ctx);

					if (ctx.FileVersion > 26)
					{
						// new in save format 27
						boolValues = reader.ReadUInt16();
					}
				}
			}
		}

		internal override void restoreState(ItemState state)
		{
			base.restoreState(state);

			onRectUpdate();

			if (subordinateGroup != null)
				subordinateGroup.onRestoreState();
		}

		internal override void saveProperties(ItemProperties props)
		{
			base.saveProperties(props);

			NodeProperties nprops = (NodeProperties)props;
			nprops.anchorPattern = anchorPattern;
			nprops.expanded = expanded;
			nprops.expandable = expandable;
			nprops.obstacle = obstacle;
			nprops.contraints = (NodeConstraints)constraints.Clone();
			nprops.enabledHandles = enabledHandles;
			nprops.boolValues = boolValues;
		}

		internal override void restoreProperties(ItemProperties props)
		{
			NodeProperties nprops = (NodeProperties)props;
			anchorPattern = nprops.anchorPattern;
			expanded = nprops.expanded;
			expandable = nprops.expandable;
			obstacle = nprops.obstacle;
			Constraints = nprops.contraints == null ?
				null : (NodeConstraints)nprops.contraints.Clone();
			enabledHandles = nprops.enabledHandles;
			boolValues = nprops.boolValues;

			base.restoreProperties(props);
		}

		private bool expanded;
		private bool expandable;
		private Expander expander = null;

		internal Expander getExpander() { return expander; }

		internal virtual void expand()
		{
			if (!expanded)
			{
				flowChart.UndoManager.onExpandItem(this);

				expanded = true;
				if (flowChart.ExpandOnIncoming)
				{
					for (int i = 0; i < IncomingArrows.Count; ++i)
						IncomingArrows[i].expand(true);
				}
				else
				{
					for (int i = 0; i < OutgoingArrows.Count; ++i)
						OutgoingArrows[i].expand(false);
				}

				flowChart.Invalidate();
			}

			// propagate the expansion through the attached items
			if (SubordinateGroup != null)
				SubordinateGroup.expand(flowChart.ExpandOnIncoming);
		}

		internal virtual void collapse()
		{
			if (expanded)
			{
				flowChart.UndoManager.onExpandItem(this);

				expanded = false;
				if (flowChart.ExpandOnIncoming)
				{
					for (int i = 0; i < IncomingArrows.Count; ++i)
						IncomingArrows[i].collapse(true);
				}
				else
				{
					for (int i = 0; i < OutgoingArrows.Count; ++i)
						OutgoingArrows[i].collapse(false);
				}

				flowChart.Invalidate();
			}

			// propagate the collapse through the attached items
			if (SubordinateGroup != null)
				SubordinateGroup.collapse(flowChart.ExpandOnIncoming);
		}

		internal override void expand(bool incm)
		{
			if (!invisible) return;

			flowChart.UndoManager.onExpandItem(this);

			Visible = true;
			if (flowChart.RecursiveExpand)
			{
				expand();
			}
			else
			{
				// propagate the expansion through the attached items
				if (SubordinateGroup != null)
					SubordinateGroup.expand(incm);
			}
		}

		internal override void collapse(bool incm)
		{
			if (invisible) return;

			flowChart.UndoManager.onExpandItem(this);

			Visible = false;
			flowChart.Selection.RemoveObject(this);
			collapse();
		}

		internal override bool isExpanded() { return expanded; }
		internal override void setExpanded(bool e) { expanded = e; }

		public void SetExpandedFlag(bool expanded)
		{
			setExpanded(expanded);
			flowChart.invalidate(getRepaintRect(false));
		}

		internal protected void setExpandable(bool exp)
		{
			if (expander != null)
			{
				removeManipulator(expander);
				expander = null;
			}

			expandable = exp;

			if (expandable)
				addManipulator(expander = new Expander(this));
		}

		public void Expand()
		{
			Expanded = true;
		}

		public void Collapse()
		{
			Expanded = false;
		}

		public bool Expandable
		{
			get { return expandable; }
			set
			{
				if (expandable != value)
				{
					setExpandable(value);
					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		public virtual bool Expanded
		{
			get { return expanded; }
			set
			{
				if (expanded != value)
				{
					flowChart.UndoManager.onStartExpand();

					if (value)
						expand();
					else
						collapse();

					flowChart.UndoManager.onEndExpand();

					flowChart.setDirty();
					flowChart.Invalidate();
				}
			}
		}

		public bool Obstacle
		{
			get { return obstacle; }
			set
			{
				obstacle = value;
				flowChart.setDirty();
			}
		}

		private bool obstacle;

		public bool Intersects(Node node)
		{
			return Node.nodesIntersect(this, node);
		}

		internal static bool nodesIntersect(Node n1, Node n2)
		{
			RectangleF bounds1 = n1.BoundingRect;
			RectangleF bounds2 = n2.BoundingRect;
			if (!bounds1.IntersectsWith(bounds2))
				return false;

			bool res = true;

			Region r1 = n1.getRegion();
			Region r2 = n2.getRegion();

			if (r1 != null && r2 != null)
			{
				Graphics g = n1.Parent.CreateGraphics();
				n1.Parent.setTransforms(g);

				r1.Intersect(r2);
				if (r1.IsEmpty(g)) res = false;

				g.Dispose();
			}
			else
			{
				res = false;
			}

			if (r2 != null)	r2.Dispose();
			if (r1 != null) r1.Dispose();

			return res;
		}

		internal override PointCollection getOutlinePoly()
		{
			RectangleF br = BoundingRect;

			PointCollection points = new PointCollection(0);
			points.Add(new PointF(br.Left, br.Top));
			points.Add(new PointF(br.Right, br.Top));
			points.Add(new PointF(br.Right, br.Bottom));
			points.Add(new PointF(br.Left, br.Bottom));
			return points;
		}

		internal PointF getNearestOutlinePt(PointF pt)
		{
			PointCollection outline = getOutlinePoly();
			outline.Add(outline[0]);
			int segment = 0;
			Utilities.distToPolyline(pt, outline, outline.Count, ref segment);
			return Utilities.getNearestPt(pt, outline[segment], outline[segment+1]);
		}

		#region grouping

		public void AttachTo(Arrow arrow,
			AttachToArrow attType, int index)
		{
			// that returns the active composite if somebody has already created one
			CompositeCmd composite = flowChart.UndoManager.StartComposite("_fcnet_");

			Detach();
			Group masterGroup = getSubordinateGroup(arrow);

			switch (attType)
			{
				case AttachToArrow.Point:
					masterGroup.AttachToArrowPoint(this, index);
					break;
				case AttachToArrow.Segment:
					masterGroup.AttachToArrowSegment(this, index);
					break;
				case AttachToArrow.LongestHSegment:
					masterGroup.AttachToLongestHSegment(this);
					break;
			}

			if (composite != null && composite.Title == "_fcnet_")
			{
				// this is our own composite cmd
				composite.Title = "Attach";
				composite.Execute();
			}
		}

		public void AttachTo(Node node,
			AttachToNode attType)
		{
			// that returns the active composite if somebody has already created one
			CompositeCmd composite = flowChart.UndoManager.StartComposite("_fcnet_");

			Detach();
			Group masterGroup = getSubordinateGroup(node);

			switch (attType)
			{
				case AttachToNode.TopLeft:
					masterGroup.AttachToCorner(this, 0);
					break;
				case AttachToNode.TopRight:
					masterGroup.AttachToCorner(this, 1);
					break;
				case AttachToNode.BottomLeft:
					masterGroup.AttachToCorner(this, 3);
					break;
				case AttachToNode.BottomRight:
					masterGroup.AttachToCorner(this, 2);
					break;
				case AttachToNode.TopCenter:
					masterGroup.AttachToSideMiddle(this, 0);
					break;
				case AttachToNode.MiddleRight:
					masterGroup.AttachToSideMiddle(this, 1);
					break;
				case AttachToNode.BottomCenter:
					masterGroup.AttachToSideMiddle(this, 2);
					break;
				case AttachToNode.MiddleLeft:
					masterGroup.AttachToSideMiddle(this, 3);
					break;
			}

			if (composite != null && composite.Title == "_fcnet_")
			{
				// this is our own composite cmd
				composite.Title = "Attach";
				composite.Execute();
			}
		}

		public void AttachTo(Node node,
			float percentX1, float percentY1, float percentX2, float percentY2)
		{
			// that returns the active composite if somebody has already created one
			CompositeCmd composite = flowChart.UndoManager.StartComposite("_fcnet_");

			Detach();
			Group masterGroup = getSubordinateGroup(node);

			masterGroup.AttachProportional(this,
				percentX1, percentY1, percentX2, percentY2);

			if (composite != null && composite.Title == "_fcnet_")
			{
				// this is our own composite cmd
				composite.Title = "Attach";
				composite.Execute();
			}
		}

		public void Detach()
		{
			if (masterGroup != null)
				masterGroup.Detach(this);
		}

		private Group getSubordinateGroup(ChartObject item)
		{
			if (item.SubordinateGroup == null)
				flowChart.CreateGroup(item);
			return item.SubordinateGroup;
		}

		#endregion

		internal override string getToolTip(PointF point)
		{
			if (anchorPattern != null)
			{
				float hitDist = 1.5f * Constants.getMarkSize(flowChart.MeasureUnit);
				RectangleF nodeRect = getBoundingRect();
				for (int i = 0; i < anchorPattern.Points.Count; i++)
				{
					AnchorPoint ap = anchorPattern.Points[i];

					PointF pos = ap.getPos(nodeRect);
					float dx = pos.X - point.X;
					float dy = pos.Y - point.Y;
					float ptDist = (float)Math.Sqrt(dx * dx + dy * dy);
					if (ptDist < hitDist && ap.ToolTip != "")
						return ap.ToolTip;
				}
			}

			return base.getToolTip(point);
		}

		public bool AllowIncomingArrows
		{
			get { return (boolValues & fAllowIncomingArrows) != 0; }
			set
			{
				if (value)
					boolValues |= fAllowIncomingArrows;
				else
					if (AllowIncomingArrows)
						boolValues -= fAllowIncomingArrows;
			}
		}

		public bool AllowOutgoingArrows
		{
			get { return (boolValues & fAllowOutgoingArrows) != 0; }
			set
			{
				if (value)
					boolValues |= fAllowOutgoingArrows;
				else
					if (AllowOutgoingArrows)
						boolValues -= fAllowOutgoingArrows;
			}
		}

		ushort boolValues;

		private const ushort fAllowIncomingArrows = 1;
		private const ushort fAllowOutgoingArrows = 2;

	}	// class Node

}	// namespace MindFusion.FlowChartX