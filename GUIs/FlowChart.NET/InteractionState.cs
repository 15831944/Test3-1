using System;
using System.Drawing;
using MindFusion.FlowChartX.Commands;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Encapsulates the current state of user interaction with the control.
	/// </summary>
	public class InteractionState
	{
		public InteractionState(
			ChartObject currentObject,
			int selectionHandle,
			Action action)
		{
			this.currentObject = currentObject;
			this.selectionHandle = selectionHandle;
			this.action = action;

			completing = false;
			itemDeleted = false;

			invalidRect = new RectangleF(0, 0, 0, 0);

			cycleRoots = new ChartObjectCollection();
			affectedArrows = new ArrowCollection();
			splitToChangeOrient = true;
		}

		public ChartObject CurrentObject
		{
			get { return currentObject; }
		}

		public int SelectionHandle
		{
			get { return selectionHandle; }
		}

		internal void setHandleIndex(int newIndex)
		{
			selectionHandle = newIndex;
		}

		public Action Action
		{
			get { return action; }
		}

		public PointF StartPoint
		{
			get { return startPoint; }
		}

		internal RectangleF InvalidRect
		{
			get { return invalidRect; }
		}

		internal void start(PointF pt, FlowChart fc)
		{
			cycleRoots.Clear();
			affectedArrows.Clear();
			startPoint = currentPoint = pt;

			if (action == Action.Split)
			{
				invalidRect = currentObject.getRepaintRect(true);
				return;
			}

			if (action == Action.Create)
				currentObject.startCreate(pt);
			if (action == Action.Modify)
				currentObject.startModify(pt, selectionHandle, this);

			invalidRect = currentObject.getRepaintRect(action == Action.Modify);
		}

		internal bool update(PointF pt, FlowChart fc)
		{
			currentPoint = pt;
			bool redrawBack = false;
			if (currentObject == null)
				return false;

			// split the arrow on the first mouse move, then modify
			if (action == Action.Split && fc.mouseMoved)
			{
				Arrow arrow = (Arrow)currentObject;
				action = Action.Modify;

				selectionHandle = arrow.splitSegment(selectionHandle, startPoint);
				currentObject.startModify(startPoint, selectionHandle, this);
				redrawBack = true;
			}

			if (action != Action.Split)
			{
				// set the mouse pointer
				ChartObject obj = fc.GetNodeAt(pt);
				if (action == Action.Create)
				{
					if (currentObject.allowCreate(pt))
						fc.Cursor = currentObject.getCanDropCursor();
					else
						fc.Cursor = currentObject.getCannotDropCursor();
				}
				else
				{
					if (currentObject.allowModify(pt))
						fc.Cursor = currentObject.getCanDropCursor();
					else
						fc.Cursor = currentObject.getCannotDropCursor();
				}

				// update the object positions
				if (action == Action.Create)
				{
					currentObject.updateCreate(pt);
				}
				else
				{
					currentObject.updateModify(pt, this);
					fc.fireModifyingEvent(currentObject, pt, selectionHandle);
				}
			}

			RectangleF rcNew = currentObject.getRepaintRect(
				action == Action.Modify);
			invalidRect = Utilities.unionRects(invalidRect, rcNew);

			return redrawBack;
		}

		internal void complete(PointF pt, FlowChart fc)
		{
			cycleRoots.Clear();
			affectedArrows.Clear();
			completing = true;

			// someone might have deleted the item from a selection- change event
			if (currentObject == null)
				return;

			if (action == Action.Create)
			{
				currentObject.completeCreate(pt);

				// someone might have deleted the item in itemCreated handler
				if (itemDeleted)
					return;

				invalidRect = Utilities.unionRects(
					invalidRect, currentObject.getRepaintRect(true));
				fc.Dirty = true;

				if (currentObject.getType() != ItemType.Selection)
				{
					// add to the diagram
					AddItemCmd cmd = new AddItemCmd(currentObject);
					cmd.Execute();
					fc.fireObjCreated(currentObject);

					// select the new item if SelectAfterCreate == true;
					// check for null because people just love to delete currObject
					// from within its xCreated or xActivated event handlers
					if (fc.SelectAfterCreate && currentObject != null && !itemDeleted)
					{
						invalidRect = Utilities.unionRects(
							invalidRect, fc.Selection.getRepaintRect(true));
						fc.Selection.Change(currentObject);
						if (currentObject != null)
						{
							invalidRect = Utilities.unionRects(
								invalidRect, currentObject.getRepaintRect(true));
						}
					}

					if (fc.UndoManager.UndoEnabled)
						cmd.saveSelState();
				}
			}
			else
			{
				currentObject.completeModify(pt, this);
				fc.UndoManager.onCompleteModify();
				if (!itemDeleted)
				{
					invalidRect = Utilities.unionRects(invalidRect,
						currentObject.getRepaintRect(true));
				}
				fc.Dirty = true;
			}
		}

		internal void cancel(FlowChart fc)
		{
			if (currentObject != null)
			{
				cycleRoots.Clear();
				affectedArrows.Clear();

				if (action == Action.Create && !completing)
				{
					if (currentObject == fc.Selection)
						fc.Selection.cancelCreate();
					else
					{
						currentObject.onRemove();
						currentObject.freeResources();
					}
					currentObject = null;
				}

				if (action == Action.Modify)
				{
					currentObject.cancelModify(this);
					fc.UndoManager.onCancelModify();

					RectangleF rcAdd = currentObject.getRepaintRect(true);
					invalidRect = Utilities.unionRects(invalidRect, rcAdd);
				}
			}
		}

		internal bool isAllowed(PointF pt, FlowChart fc)
		{
			if (action == Action.Create)
				return currentObject.allowCreate(pt) && fc.confirmCreate(currentObject);

			if (action == Action.Modify)
				return currentObject.allowModify(pt);

			if (action == Action.Split)
				return false;

			return true;
		}

		internal bool isCompleting()
		{
			return completing;
		}

		internal void setItemDeleted()
		{
			itemDeleted = true;
		}

		private ChartObject currentObject;
		private int selectionHandle;
		private Action action;

		// state
		private bool completing;
		private bool itemDeleted;

		private PointF startPoint;
		private PointF currentPoint;

		private RectangleF invalidRect;

		internal ChartObjectCollection cycleRoots;
		internal ArrowCollection affectedArrows;
		internal float savedAngle;

		// if a segment has been inserted in order to change the orientation of a cascading arrow,
		// do not add any more segments if the orientation must be changed again
		internal bool splitToChangeOrient;
	}
}