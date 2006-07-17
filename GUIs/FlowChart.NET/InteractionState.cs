using System;
using System.Drawing;
using System.Windows.Forms;

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

		internal PointF CurrentPoint
		{
			get { return currentPoint; }
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

		internal void start(PointF point, FlowChart fc)
		{
			cycleRoots.Clear();
			affectedArrows.Clear();
			startPoint = currentPoint = point;

			if (action == Action.Split)
			{
				invalidRect = currentObject.getRepaintRect(true);
				return;
			}

			if (action == Action.Create)
			{
				fc.raiseInitEvent(currentObject);
				currentObject.startCreate(point);
			}

			if (action == Action.Modify)
			{
				fc.fireBeginModifyEvent(currentObject, point, selectionHandle);
				currentObject.startModify(point, selectionHandle, this);
			}

			invalidRect = currentObject.getRepaintRect(action == Action.Modify);
		}

		internal bool update(PointF point, FlowChart fc)
		{
			currentPoint = point;
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
				// update the object positions
				if (action == Action.Create)
					currentObject.updateCreate(point);
				else
					currentObject.updateModify(point, this);
			}

			RectangleF rcNew = currentObject.getRepaintRect(
				action == Action.Modify);
			invalidRect = Utilities.unionRects(invalidRect, rcNew);

			return redrawBack;
		}

		internal void complete(PointF point, FlowChart fc)
		{
			cycleRoots.Clear();
			affectedArrows.Clear();
			completing = true;

			// someone might have deleted the item from a selection- change event
			if (currentObject == null)
				return;

			if (action == Action.Create)
			{
				currentObject.completeCreate(point);

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
				currentObject.completeModify(point, this);
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

		internal bool isAllowed(PointF point)
		{
			if (action == Action.Create)
				return currentObject.allowCreate(point);

			if (action == Action.Modify)
				return currentObject.allowModify(point);

			if (action == Action.Split)
				return false;

			return true;
		}

		internal void setCursor(PointF point, FlowChart fc)
		{
			if (action == Action.Create)
			{
				Cursor cursor = currentObject.allowCreate(point) ?
					currentObject.getCanDropCursor() : currentObject.getCannotDropCursor();

				if (cursor != null)
					fc.Cursor = cursor;
			}

			if (action == Action.Modify)
			{
				Cursor cursor = currentObject.allowModify(point) ?
					currentObject.getCanDropCursor() : currentObject.getCannotDropCursor();

				if (cursor != null)
					fc.Cursor = cursor;
				else
					fc.Cursor = fc.getCurrBehavior().getCurrentCursor();
			}
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