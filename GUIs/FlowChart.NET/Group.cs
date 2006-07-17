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
using System.IO;
using MindFusion.FlowChartX.Commands;
using MindFusion.FlowChartX.Visitors;


namespace MindFusion.FlowChartX
{
	internal class Attachment : IPersists
	{
		internal Attachment()
		{
			node = null;
			percents = new RectangleF(0, 0, 0, 0);
			attData = 0;
			type = AttachTo.ArrowPoint;
		}

		internal Attachment Clone()
		{
			Attachment clone = new Attachment();
			clone.node = node;
			clone.percents = percents;
			clone.attData = attData;
			clone.type = type;
			return clone;
		}

		internal Node node;
		internal AttachTo type;
		internal int attData;
		internal RectangleF percents;

		public virtual int getClassId()
		{
			return 11;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveReference(this, node, 1);
			writer.Write((int)type);
			writer.Write(attData);

			// in format revision 28 'percents' changed from Rectangle to RectangleF
			ctx.saveRect(percents);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			ctx.loadReference(this);
			type = (AttachTo)reader.ReadInt32();
			attData = reader.ReadInt32();

			// in format revision 28 'percents' changed from Rectangle to RectangleF
			if (ctx.FileVersion < 28)
			{
				Rectangle r = ctx.loadRect();
				percents = RectangleF.FromLTRB(r.Left, r.Top, r.Right, r.Bottom);
			}
			else
			{
				percents = ctx.loadRectF();
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			this.node = (Node)obj;
		}
	}	// class Attachment

	/// <summary>
	/// a mediator to handle hierarchical grouping of items
	/// </summary>
	public class Group : IPersists
	{
		public Group(FlowChart parent)
		{
			fcParent = parent;
			mainObj = null;
			cycleProtect = false;
			attachments = new ArrayList();
			autoDeleteItems = false;
			tag = null;
			visible = true;

			attachedObjects = new ChartObjectCollection();
			arrowsToMove = new ArrowCollection();

			expandable = false;
			followMasterRotation = false;
		}

		public Group(Group prototype)
		{
			fcParent = prototype.fcParent;
			mainObj = prototype.mainObj;
			cycleProtect = false;
			attachments = (ArrayList)prototype.attachments.Clone();
			for (int i = 0; i < attachments.Count; ++i)
				attachments[i] = (attachments[i] as Attachment).Clone();

			autoDeleteItems = prototype.autoDeleteItems;
			tag = prototype.tag;
			visible = prototype.visible;

			attachedObjects = new ChartObjectCollection();
			updateObjCol();
			arrowsToMove = new ArrowCollection();

			expandable = prototype.expandable;
			followMasterRotation = prototype.followMasterRotation;
		}

		/// <summary>
		/// group's master item - either a node or an arrow;
		/// modifying the master modifies or moves subordinate items too
		/// </summary>
		public ChartObject MainObject
		{
			get
			{
				return mainObj;
			}
			set
			{
				setMainObject(value);
			}
		}

		private ChartObject mainObj;

		/// <summary>
		/// Adds an item to the group, attaching it to a control point of an arrow
		/// </summary>
		/// <param name="obj">The object to attach to the current group.</param>
		/// <param name="point">The point to attach to.</param>
		public bool AttachToArrowPoint(Node node, int point)
		{
			if (!objAttachable(node)) return false;
			if (mainObj.getType() != ItemType.Arrow) return false;
			if (((Arrow)mainObj).Points.Count <= point) return false;

			Attachment a = new Attachment();
			a.node = node;
			a.type = AttachTo.ArrowPoint;
			a.attData = point;

			new GroupAttachCmd(this, a).Execute();

			return true;
		}

		/// <summary>
		/// Adds an item to the group, attaching it to a segment of the master arrow
		/// </summary>
		/// <param name="node">The object to attach to the current group.</param>
		/// <param name="segment">The segment to attach to.</param>
		public bool AttachToArrowSegment(Node node, int segment)
		{
			if (!objAttachable(node)) return false;
			if (mainObj.getType() != ItemType.Arrow) return false;
			if (((Arrow)mainObj).SegmentCount <= segment) return false;

			Attachment a = new Attachment();
			a.node = node;
			a.type = AttachTo.ArrowSegment;
			a.attData = segment;

			new GroupAttachCmd(this, a).Execute();

			return true;
		}

		/// <summary>
		/// Adds an item to the group, maintaining its relative positions
		/// to master arrow's longest horizontal segment
		/// </summary>
		/// <param name="node">The object to attach to the current group.</param>
		public bool AttachToLongestHSegment(Node node)
		{
			if (!objAttachable(node)) return false;
			if (mainObj.getType() != ItemType.Arrow) return false;

			Attachment a = new Attachment();
			a.node = node;
			a.type = AttachTo.LongestHSegment;

			new GroupAttachCmd(this, a).Execute();

			return true;
		}

		/// <summary>
		/// Adds an item to the group, attaching it to a corner of the master node
		/// </summary>
		/// <param name="node">The object to attach to the current group.</param>
		/// <param name="corner">The corner to attach to.</param>
		public bool AttachToCorner(Node node, int corner)
		{
			if (!objAttachable(node)) return false;
			if (mainObj.getType() == ItemType.Arrow) return false;
			if (corner < 0 || corner > 3) return false;

			Attachment a = new Attachment();
			a.node = node;
			a.type = AttachTo.FixedCorner;
			a.attData = corner;

			new GroupAttachCmd(this, a).Execute();

			return true;
		}

		/// <summary>
		/// Adds an item to the group, attaching it to the middle point of a side of the master node
		/// </summary>
		/// <param name="node">The object to attach to the current group.</param>
		/// <param name="side">The corner to attach to.</param>
		public bool AttachToSideMiddle(Node node, int side)
		{
			if (!objAttachable(node)) return false;
			if (mainObj.getType() == ItemType.Arrow) return false;
			if (side < 0 || side > 3) return false;

			Attachment a = new Attachment();
			a.node = node;
			a.type = AttachTo.SideMiddle;
			a.attData = side;

			new GroupAttachCmd(this, a).Execute();

			return true;
		}

		/// <summary>
		/// Attaches an item so its size maintains a constant ratio to master's size
		/// </summary>
		/// <param name="obj">The object to attach to the current group.</param>
		/// <param name="left"></param>
		/// <param name="top"></param>
		/// <param name="right"></param>
		/// <param name="bottom"></param>
		public bool AttachProportional(Node node,
			float left, float top, float right, float bottom)
		{
			if (!objAttachable(node)) return false;
			if (mainObj.getType() == ItemType.Arrow) return false;

			Attachment a = new Attachment();
			a.node = node;
			a.type = AttachTo.Proportional;
			a.percents = RectangleF.FromLTRB(
				left, top, right, bottom);

			new GroupAttachCmd(this, a).Execute();

			return true;
		}

		/// <summary>
		/// Removes a subordinate item from the group.
		/// </summary>
		/// <param name="obj">The object to remove.</param>
		public bool Detach(ChartObject obj)
		{
			for (int i = 0; i < attachments.Count; ++i)
			{
				Attachment att = (Attachment)attachments[i];
				if (att.node == obj)
				{
					new GroupDetachCmd(this, att).Execute();
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// gets a collection of group's subordinate items
		/// </summary>
		public ChartObjectCollection AttachedObjects
		{
			get { return attachedObjects; }
		}

		private ChartObjectCollection attachedObjects;

		/// <summary>
		/// shows or hides all items in the group
		/// </summary>
		public bool Visible
		{
			get
			{
				return visible;
			}
			set
			{
				if(visible != value)
				{
					visible = value;

					Attachment att;
					for (int i = 0; i < attachments.Count; i++)
					{
						att = (Attachment)attachments[i];
						att.node.Visible = visible;
					}
					mainObj.Visible = visible;

					fcParent.setDirty();
				}
			}
		}

		private bool visible;

		/// <summary>
		/// specifies whether subordinate items are deleted
		/// automatically when the group is destroyed
		/// </summary>
		public bool AutoDeleteItems
		{
			get { return autoDeleteItems; }
			set { autoDeleteItems = value; }
		}

		private bool autoDeleteItems;

		/// <summary>
		/// Gets or sets a value specifying whether expanding or collapsing a 
		/// tree branch should propagate through the group's subordinate items.
		/// </summary>
		public bool Expandable
		{
			get { return expandable; }
			set { expandable = value; }
		}

		/// <summary>
		/// Specifies whether the positoions of subordinated items change
		/// when the group master item is rotated.
		/// </summary>
		public bool FollowMasterRotation
		{
			get { return followMasterRotation; }
			set { followMasterRotation = value; }
		}

		/// <summary>
		/// User-defined data
		/// </summary>
		public object Tag
		{
			get
			{
				return tag;
			}
			set
			{
				if (tag == null || !tag.Equals(value))
				{
					tag = value;
					fcParent.setDirty();
				}
			}
		}

		private object tag;

		internal virtual void onDelete()
		{
			if(mainObj != null)
				mainObj.setGroup(null);
			mainObj = null;

			foreach (Attachment attc in attachments)
				attc.node.putInGroup(null);
			attachments.Clear();

			attachedObjects.Clear();
		}

		internal bool setMainObject(ChartObject main)
		{
			if (main == null) return false;

			if (main.getType() != ItemType.Arrow &&
				main.getType() != ItemType.Box &&
				main.getType() != ItemType.Table &&
				main.getType() != ItemType.ControlHost)
				return false;

			if (!main.setGroup(this)) return false;

			mainObj = main;

			if (mainObj.getType() == ItemType.Arrow)
			{
				prevPoints = ((Arrow)mainObj).Points.Clone();
			}
			else
			{
				prevRect = mainObj.getBoundingRect();
				prevRotation = (mainObj as Node).rotation();
			}

			return true;
		}

		internal void updateObjects(InteractionState ist)
		{
			if(cycleProtect) return;
			cycleProtect = true;

			if (!fcParent.DisabledGroups)
			{
				foreach (Arrow arrow in arrowsToMove)
					arrow.translatePoints(
						mainObj.getBoundingRect().X - prevRect.X,
						mainObj.getBoundingRect().Y - prevRect.Y);
			}

			for (int i = 0; i < attachments.Count; i++)
			{
				Attachment att = (Attachment)attachments[i];
				switch(att.type)
				{
					case AttachTo.FixedCorner:
						updateToCornerAttachment(att, ist);
						break;
					case AttachTo.SideMiddle:
						updateSideMiddleAttachment(att, ist);
						break;
					case AttachTo.Proportional:
						updatePropAttachment(att, ist);
						break;
					case AttachTo.ArrowPoint:
						if (!updateToPointAttachment(att, ist))
							return;
						break;
					case AttachTo.ArrowSegment:
						if (!updateToSegmentAttachment(att, ist))
							return;
						break;
					case AttachTo.LongestHSegment:
						updateLongestSegmAttachment(att, ist);
						break;
				}
			}

			if (mainObj.getType() == ItemType.Arrow)
			{
				prevPoints = ((Arrow)mainObj).Points.Clone();
			}
			else
			{
				prevRect = mainObj.getBoundingRect();
				prevRotation = (mainObj as Node).rotation();
			}

			cycleProtect = false;
		}

		private void updateToCornerAttachment(Attachment att, InteractionState ist)
		{
			int corner;
			PointF ptPrev = new PointF(0, 0);
			PointF ptCurr = new PointF(0, 0);
			RectangleF rc;

			corner = att.attData;
			switch(corner)
			{
				case 0:
					ptPrev.X = prevRect.Left;
					ptPrev.Y = prevRect.Top;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Left;
					ptCurr.Y = rc.Top;
					break;
				case 1:
					ptPrev.X = prevRect.Right;
					ptPrev.Y = prevRect.Top;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Right;
					ptCurr.Y = rc.Top;
					break;
				case 2:
					ptPrev.X = prevRect.Right;
					ptPrev.Y = prevRect.Bottom;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Right;
					ptCurr.Y = rc.Bottom;
					break;
				case 3:
					ptPrev.X = prevRect.Left;
					ptPrev.Y = prevRect.Bottom;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Left;
					ptCurr.Y = rc.Bottom;
					break;
			}

			if (followMasterRotation && ist.SelectionHandle == 9 && mainObj is Node)
			{
				Node mainNode = mainObj as Node;
				float newAngle = mainNode.rotation();
				if (newAngle != prevRotation)
				{
					PointF pivot = mainNode.getCenter();
					PointF center = att.node.getCenter();
					float a = 0, r = 0;
					Geometry.Geometry2D.Convert.DekartToPolar(
						pivot, center, ref a, ref r);
					a -= newAngle - prevRotation;

					ptPrev = ptCurr = center;
					Geometry.Geometry2D.Convert.PolarToDekart(
						pivot, a,r, ref ptCurr);
				}
			}

			if (fcParent.DisabledGroups)
				ptCurr = ptPrev;
			att.node.modifyTranslate(
				ptCurr.X - ptPrev.X, ptCurr.Y - ptPrev.Y, true);
		}

		private void updateSideMiddleAttachment(Attachment att, InteractionState ist)
		{
			int side;
			PointF ptPrev = new PointF(0, 0);
			PointF ptCurr = new PointF(0, 0);
			RectangleF rc;

			side = att.attData;
			switch(side)
			{
				case 0:
					ptPrev.X = prevRect.Left + prevRect.Width / 2;
					ptPrev.Y = prevRect.Top;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Left + rc.Width / 2;
					ptCurr.Y = rc.Top;
					break;
				case 1:
					ptPrev.X = prevRect.Right;
					ptPrev.Y = prevRect.Top + prevRect.Height / 2;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Right;
					ptCurr.Y = rc.Top + rc.Height / 2;
					break;
				case 2:
					ptPrev.X = prevRect.Left + prevRect.Width / 2;
					ptPrev.Y = prevRect.Bottom;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Left + rc.Width / 2;
					ptCurr.Y = rc.Bottom;
					break;
				case 3:
					ptPrev.X = prevRect.Left;
					ptPrev.Y = prevRect.Top + prevRect.Height / 2;
					rc = mainObj.getBoundingRect();
					ptCurr.X = rc.Left;
					ptCurr.Y = rc.Top + rc.Height / 2;
					break;
			}

			if (followMasterRotation && ist.SelectionHandle == 9 && mainObj is Node)
			{
				Node mainNode = mainObj as Node;
				float newAngle = mainNode.rotation();
				if (newAngle != prevRotation)
				{
					PointF pivot = mainNode.getCenter();
					PointF center = att.node.getCenter();
					float a = 0, r = 0;
					Geometry.Geometry2D.Convert.DekartToPolar(
						pivot, center, ref a, ref r);
					a -= newAngle - prevRotation;

					ptPrev = ptCurr = center;
					Geometry.Geometry2D.Convert.PolarToDekart(
						pivot, a,r, ref ptCurr);
				}
			}

			if (fcParent.DisabledGroups)
				ptCurr = ptPrev;
			att.node.modifyTranslate(
				ptCurr.X - ptPrev.X, ptCurr.Y - ptPrev.Y, true);
		}

		private void updatePropAttachment(Attachment att, InteractionState ist)
		{
			if (!(mainObj is Node))
				return;

			Node mainNode = mainObj as Node;
			RectangleF rc = followMasterRotation ?
				mainNode.getRotatedBounds() : mainNode.getBoundingRect();
			float w = rc.Right - rc.Left;
			float h = rc.Bottom - rc.Top;

			RectangleF perc = att.percents;
			RectangleF rcNew = RectangleF.FromLTRB(
				rc.Left + w * perc.Left / 100,
				rc.Top + h * perc.Top / 100,
				rc.Left + w * perc.Right / 100,
				rc.Top + h * perc.Bottom / 100);

			if (!fcParent.DisabledGroups)
				att.node.setRect(rcNew);
		}

		private bool updateToPointAttachment(Attachment att, InteractionState ist)
		{
			int point = att.attData;
			if (((Arrow)mainObj).Points.Count <= point)
			{
				cycleProtect = false;
				new RemoveGroupCmd(MainObject, this).Execute();
				return false;
			}

			PointF ptPrev = prevPoints[point];
			PointF ptCurr = ((Arrow)mainObj).Points[point];

			if(fcParent.DisabledGroups)
				ptCurr = ptPrev;
			att.node.modifyTranslate(
				ptCurr.X - ptPrev.X, ptCurr.Y - ptPrev.Y, true);

			return true;
		}

		private bool updateToSegmentAttachment(Attachment att, InteractionState ist)
		{
			int segment = att.attData;
			int n1 = 0, n2 = 0;
			if (((Arrow)mainObj).SegmentCount <= segment)
			{
				cycleProtect = false;
				new RemoveGroupCmd(MainObject, this).Execute();
				return false;
			}

			switch (((Arrow)mainObj).Style)
			{
				case ArrowStyle.Bezier:
					n1 = segment * 3 + 1;
					n2 = n1 + 1;
					break;
				case ArrowStyle.Polyline:
				case ArrowStyle.Cascading:
					n1 = segment;
					n2 = segment + 1;
					break;
			}

			PointF ptPrev = Utilities.midPoint(
				prevPoints[n1], prevPoints[n2]);
			PointF ptCurr = Utilities.midPoint(
				((Arrow)mainObj).Points[n1],
				((Arrow)mainObj).Points[n2]);

			if (fcParent.DisabledGroups)
				ptCurr = ptPrev;
			att.node.modifyTranslate(
				ptCurr.X - ptPrev.X, ptCurr.Y - ptPrev.Y, true);

			return true;
		}

		private void updateLongestSegmAttachment(Attachment att, InteractionState ist)
		{
			int segment = ((Arrow)mainObj).getLongestHorzSegment();
			int n1 = 0, n2 = 0;

			switch (((Arrow)mainObj).Style)
			{
				case ArrowStyle.Bezier:
					n1 = segment * 3 + 1;
					n2 = n1 + 1;
					break;
				case ArrowStyle.Polyline:
				case ArrowStyle.Cascading:
					n1 = segment;
					n2 = segment + 1;
					break;
			}

			PointF ptCurr = Utilities.midPoint(
				((Arrow)mainObj).Points[n1],
				((Arrow)mainObj).Points[n2]);
			if (!fcParent.DisabledGroups)
			{
				float dx = ptCurr.X - att.node.getCenter().X;
				float dy = ptCurr.Y - att.node.getCenter().Y;
				att.node.modifyTranslate(dx, dy, true);
			}
		}

		internal void beginModification(InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			Point pt = new Point(0, 0);
			for (int i = 0; i < attachments.Count; ++i)
				((Attachment)attachments[i]).node.startModify(pt, 0, ist);

			getArrowsToMove(ist);

			cycleProtect = false;
		}

		internal void endModification()
		{
			if (cycleProtect) return;
			cycleProtect = true;

			for (int i = 0; i < attachments.Count; ++i)
				((Attachment)attachments[i]).node.completeModify();
			arrowsToMove.Clear();

			cycleProtect = false;
		}

		internal void cancelModification(InteractionState ist)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			for (int i = 0; i < attachments.Count; ++i)
				((Attachment)attachments[i]).node.cancelModify(ist);

			if (mainObj.getType() == ItemType.Arrow)
			{
				prevPoints = ((Arrow)mainObj).Points.Clone();
			}
			else
			{
				prevRect = mainObj.getBoundingRect();
				prevRotation = (mainObj as Node).rotation();
			}

			cycleProtect = false;
		}

		private void getArrowsToMove(InteractionState ist)
		{
			arrowsToMove.Clear();

			// translate arrows if attachment type is 'to corner'
			if (mainObj is Node)
			{
				// get arrows of nodes attached 'to corner'
				ArrowCollection arrowsToCheck = new ArrowCollection();
				foreach (Attachment att in Attachments)
				{
					if (att.type == AttachTo.FixedCorner ||
						att.type == AttachTo.SideMiddle)
					{
						Node node = att.node;
						node.getAllIncomingArrows(arrowsToCheck);
						node.getAllOutgoingArrows(arrowsToCheck);
					}
				}

				// check if arrow points must be translated by this group
				foreach (Arrow a in arrowsToCheck)
				{
					if (mustTranslateArrow(a) && !arrowsToMove.Contains(a) &&
						!ist.affectedArrows.Contains(a))
					{
						arrowsToMove.Add(a);
						ist.affectedArrows.Add(a);
					}
				}
			}
		}

		internal ArrowCollection ArrowsToMove
		{
			get { return arrowsToMove; }
		}

		private bool mustTranslateArrow(Arrow arrow)
		{
			// following two cases are handled elsewhere
			if (arrow.isReflexive()) return false;
			if (arrow.bothEndsSelected()) return false;
			if (arrow.RetainForm) return false;

			if (arrow.Origin == mainObj) return true;
			if (arrow.Destination == mainObj) return true;

			return false;
		}

		private bool objAttachable(ChartObject obj)
		{
			if (obj.isAttached())
				return false;
			if (mainObj == obj)
				return false;
			if (obj is Node)
				return true;

			return false;
		}

		internal RectangleF getRepaintRect()
		{
			RectangleF rc = new RectangleF( 0, 0, 0, 0 );

			if (cycleProtect) return rc;
			cycleProtect = true;

			for (int i = 0; i < attachments.Count; ++i)
			{
				RectangleF rcInv = rc;
				RectangleF rcObj = ((Attachment)attachments[i]).node.getRepaintRect(true);
				rc = Utilities.unionNonEmptyRects(rcInv, rcObj);
			}

			cycleProtect= false;
			return rc;
		}

		internal void notifyObjDeleted(ChartObject obj)
		{
			// destroy a group if its master object is deleted
			if (obj == mainObj)
			{
				mainObj = null;
				new RemoveGroupCmd(obj, this).Execute();
				return;
			}

			// remove the object from the attachments list
			foreach (Attachment att in attachments)
			{
				if (att.node == obj)
				{
					new GroupDetachCmd(this, att).Execute();
					return;
				}
			}
		}

		internal void destroySelf(bool redo)
		{
			fcParent.fireGroupDestroyed(this);

			// let the master object know that it no longer has a subordinate group
			if (mainObj != null)
				mainObj.setGroup(null);
			mainObj = null;

			// if called for redo, there are already GroupDetachCmd
			// commands attached to this one, so do not create them again
			if (!redo)
			{
				ChartObjectCollection delItems = autoDeleteItems ?
					attachedObjects.Clone() : null;

				// GroupDetachCmd always removes an attachment from
				// the array, so that cycle will end for sure
				while (attachments.Count > 0)
				{
					GroupDetachCmd cmd = new GroupDetachCmd(
						this, (Attachment)attachments[attachments.Count - 1]);
					cmd.Execute();
				}

				// if autoDeleteItems is enabled, delete all subordinate items
				if (delItems != null)
				{
					foreach (ChartObject item in delItems)
						fcParent.deleteItem(item);
					delItems.Clear();
					delItems = null;
				}
			}

			// remove from flowchart's list of groups
			fcParent.deleteGroup(this);
		}

		internal void visitHierarchy(CollectionVisitor visitor)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			for (int i = 0; i < attachments.Count; ++i)
				((Attachment)attachments[i]).node.visitHierarchy(visitor);

			cycleProtect = false;
		}

		internal void visitAttachedItems(CollectionVisitor visitor)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			for (int i = 0; i < attachments.Count; ++i)
				((Attachment)attachments[i]).node.visitAttachedItems(visitor);

			cycleProtect = false;
		}

		public virtual int getClassId()
		{
			return 9;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveReference(this, prevPoints, 1);
			ctx.saveRect(prevRect);
			writer.Write(attachments.Count);
			for (int i = 0; i < attachments.Count; ++i)
			{
				Attachment atch = (Attachment)attachments[i];
				ctx.saveObject(atch);
			}
			ctx.saveReference(this, mainObj, 2);

			writer.Write(visible);

			bool tagSerializable = false;
			if (tag != null)
				tagSerializable = tag.GetType().IsSerializable;
			writer.Write(tagSerializable);

			if (tagSerializable)
				ctx.saveTag(tag);

			// new in save format 17
			writer.Write(autoDeleteItems);

			// new in save format 21
			writer.Write(expandable);

			// new in save format 22
			writer.Write(followMasterRotation);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			ctx.loadReference(this);
			prevRect = ctx.loadRectF();
			int c = reader.ReadInt32();
			for (int i = 0; i < c; ++i)
			{
				Attachment atch = (Attachment)ctx.loadObject();
				attachments.Add(atch);
			}
			ctx.loadReference(this);
			visible = reader.ReadBoolean();

			tag = null;
			bool tagAvailable = reader.ReadBoolean();
			if (tagAvailable)
				tag = ctx.loadTag();

			if (ctx.FileVersion > 16)
			{
				// new in save format 17
				autoDeleteItems = reader.ReadBoolean();

				if (ctx.FileVersion > 20)
				{
					// new in save format 21
					expandable = reader.ReadBoolean();

					if (ctx.FileVersion > 21)
					{
						// new in save format 22
						followMasterRotation = reader.ReadBoolean();
					}
				}
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			switch (refId)
			{
			case 1:
				prevPoints = (PointCollection)obj;
				break;
			case 2:
				mainObj = (ChartObject)obj;
				break;
			}
		}

		internal void updateObjCol()
		{
			attachedObjects.Clear();
			foreach (Attachment att in attachments)
				attachedObjects.Add(att.node);
		}

		internal ArrayList Attachments
		{
			get { return attachments; }
		}

		private ArrayList attachments;

		internal FlowChart fcParent;
		private bool cycleProtect;

		private PointCollection prevPoints;
		private RectangleF prevRect;
		private float prevRotation;

		internal void onArrowSplit(bool added, int idx, int cnt)
		{
			if (added)
			{
				foreach (Attachment attc in attachments)
				{
					switch (attc.type)
					{
					case AttachTo.ArrowPoint:
						{
							int point = attc.attData;
							if (point >= idx)
								attc.attData += cnt;
						}
						break;
					case AttachTo.ArrowSegment:
						{
							int segment = attc.attData;
							if (segment >= idx)
								attc.attData += cnt;
						}
						break;
					}

				}	// for
			}
			else
			{
				foreach (Attachment attc in attachments)
				{
					switch (attc.type)
					{
					case AttachTo.ArrowPoint:
						{
							int point = attc.attData;
							if (point >= idx)
							{
								if (point < idx + cnt)
									attc.attData = idx;
								else
									attc.attData -= cnt;
							}
						}
						break;
					case AttachTo.ArrowSegment:
						{
							int segment = attc.attData;
							if (segment >= idx)
							{
								if (segment < idx + cnt)
									attc.attData = idx - 1;
								else
									attc.attData -= cnt;
								if (attc.attData < 0)
									attc.attData = 0;
							}
						}
						break;
					}

				}	// for
			}

			prevPoints = ((Arrow)mainObj).Points.Clone();
		}

		/// <summary>
		/// Has to be called whenever the arrow's segment count changes.
		/// </summary>
		internal void onSegmentsChanged()
		{
			Arrow main = mainObj as Arrow;
			int increase = main.Points.Count - prevPoints.Count;

			if (increase > 0)
			{
				for (int i = 0; i < increase; i++)
					prevPoints.Add(main.Points[prevPoints.Count]);
			}
			else if (increase < 0)
			{
				for (int i = 0; i < -increase; i++)
					prevPoints.RemoveAt(prevPoints.Count - 1);
			}
		}

		private ArrowCollection arrowsToMove;

		/// <summary>
		/// Used for undo/redo. Adds an existing Attachment instance to the internal list of attachments.
		/// </summary>
		internal void addAttachment(Attachment attachment, bool undoOp)
		{
			if (attachments == null)
				attachments = new ArrayList();

			attachments.Add(attachment);
			attachment.node.putInGroup(this);
			if (!undoOp)
				attachment.node.Visible = visible;
			attachedObjects.Add(attachment.node);
		}

		/// <summary>
		/// Used for undo/redo. Removes an Attachment instance from the internal list of attachments.
		/// </summary>
		internal void removeAttachment(Attachment attachment)
		{
			attachments.Remove(attachment);
			attachment.node.putInGroup(null);
			attachedObjects.Remove(attachment.node);
		}

		/// <summary>
		/// Has to be called by the main object when its state is restored due to undo/redo.
		/// </summary>
		internal void onRestoreState()
		{
			if (mainObj.getType() == ItemType.Arrow)
			{
				prevPoints = ((Arrow)mainObj).Points.Clone();
			}
			else
			{
				prevRect = mainObj.getBoundingRect();
				prevRotation = (mainObj as Node).rotation();
			}
		}

		/// <summary>
		/// Propagates a tree branch expansion through the groups' items.
		/// </summary>
		/// <param name="incoming">Specifies whether the tree is expanded in the direction of incoming arrows.</param>
		internal void expand(bool incoming)
		{
			if (!expandable)
				return;

			foreach (ChartObject item in attachedObjects)
				item.expand(incoming);
		}

		/// <summary>
		/// Propagates a tree branch collapse through the groups' items.
		/// </summary>
		/// <param name="incoming">Specifies whether the tree is collapsed in the direction of incoming arrows.</param>
		internal void collapse(bool incoming)
		{
			if (!expandable)
				return;

			foreach (ChartObject item in attachedObjects)
				item.collapse(incoming);
		}

		/// <summary>
		/// Specifies whether expanding or collapsing a tree branch should
		/// propagate through the group's subordinate items.
		/// </summary>
		private bool expandable;

		/// <summary>
		/// Specifies whether the positoions of subordinated items change
		/// when the group master item is rotated.
		/// </summary>
		private bool followMasterRotation;

	}	// class Group

}	// namespace MindFusion.FlowChartX