// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Resources;
using System.Runtime.Serialization;
using System.Windows.Forms;

using MindFusion.FlowChartX.Text;
using MindFusion.FlowChartX.Visitors;
using MindFusion.Geometry.Geometry2D;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Represents diagram nodes whose shape can be changed. Boxes can optionally
	/// contain images and text and they expose various customization properties.
	/// </summary>
	public class Box : Node
	{
		#region initialization

		public Box(FlowChart parent) : base(parent)
		{
			pen = (Pen)fcParent.BoxPen.Clone();
			brush = fcParent.BoxBrush;
			brush.AddRef();

			style = parent.BoxStyle;

			fillColor = parent.BoxFillColor;
			frameColor = parent.BoxFrameColor;

			image = null;
			transparent = false;

			text = parent.BoxText;
			ftext = null;
			textFormat = new StringFormat(parent.TextFormat);
			textColor = parent.TextColor;

			customDraw = parent.BoxCustomDraw;

			shapeRotation = fcParent.ShapeOrientation;
			rotationAngle = 0;
			rotateContents = false;
			shapeTemplate = null;
			if (style == BoxStyle.Shape)
				Shape = fcParent.DefaultShape;

			setExpandable(fcParent.BoxesExpandable);
			setExpanded(true);

			picturePos = ImageAlign.Fit;

			selStyle = fcParent.BoxHandlesStyle;

			useTextLayout = fcParent.PolyTextLayout;
			useStyledText = fcParent.EnableStyledText;
			txOptions = new Text.LayoutOptions();
			txLayout = new Text.Layout();
			updateText();
			layoutText();
		}

		public Box(Box prototype) : base(prototype)
		{
			style = prototype.style;

			image = prototype.image;
			transparent = prototype.transparent;

			text = prototype.text;
			ftext = null;
			textFormat = (StringFormat)prototype.textFormat.Clone();
			textColor = prototype.textColor;

			customDraw = prototype.customDraw;

			shapeRotation = prototype.shapeRotation;
			rotationAngle = prototype.rotationAngle;
			rotateContents = prototype.rotateContents;
			Shape = prototype.Shape;

			setExpandable(prototype.Expandable);
			setExpanded(prototype.Expanded);

			picturePos = prototype.picturePos;

			selStyle = prototype.selStyle;

			useTextLayout = prototype.useTextLayout;
			useStyledText = prototype.useStyledText;
			txOptions = new Text.LayoutOptions();
			txLayout = new Text.Layout();

			updateText();
			layoutText();
		}

		/// <summary>
		/// Called when the box is added to the flowchart
		/// either by users, programmatically or via undo / redo.
		/// </summary>
		internal override void onAdd()
		{
			base.onAdd();
			updateShapePoints();

			// Listen for changes in FlowChart' unit of measure
			parentUnitChanged = new EventHandler(OnParentUnitChanged);
			fcParent.MeasureUnitChanged += parentUnitChanged;
		}

		#endregion

		#region cleanup

		/// <summary>
		/// Called when the box is removed from the flowchart
		/// either by users, programmatically or via undo / redo.
		/// </summary>
		internal override void onRemove()
		{
			base.onRemove();

			if (parentUnitChanged != null)
			{
				fcParent.MeasureUnitChanged -= parentUnitChanged;
				parentUnitChanged = null;
			}

			// fire a BoxDeleted event
			if (constructed)
				fcParent.fireBoxDeleted(this);
		}

		internal override void freeResources()
		{
			base.freeResources();

			// cleanup
			if (image != null)
			{
				//image.Dispose();
				image = null;
				System.GC.Collect();
			}
		}

		#endregion

		#region drawing

		internal override RectangleF getRotatedBounds()
		{
			if (rotation() != 0)
				return getRotatedBounds(rect, getCenter(), rotation());

			return Utilities.normalizeRect(rect);
		}

		private RectangleF getRotatedBounds(RectangleF rect, PointF center, float angle)
		{
			PointF[] p = new PointF[] {
				new PointF(rect.Left, rect.Top),
				new PointF(rect.Right, rect.Top),
				new PointF(rect.Right, rect.Bottom),
				new PointF(rect.Left, rect.Bottom)
			};

			Utilities.rotatePointsAt(p, center, angle);

			float minX = Math.Min(p[0].X, Math.Min(p[1].X, Math.Min(p[2].X, p[3].X)));
			float minY = Math.Min(p[0].Y, Math.Min(p[1].Y, Math.Min(p[2].Y, p[3].Y)));
			float maxX = Math.Max(p[0].X, Math.Max(p[1].X, Math.Max(p[2].X, p[3].X)));
			float maxY = Math.Max(p[0].Y, Math.Max(p[1].Y, Math.Max(p[2].Y, p[3].Y)));

			return RectangleF.FromLTRB(minX, minY, maxX, maxY);
		}

		internal override RectangleF getRepaintRect(bool includeConnected)
		{
			RectangleF repaintRect = base.getRepaintRect(true);

			// include the text area of custom shapes
			if (shapeTemplate != null && style == BoxStyle.Shape &&
				shapeTemplate.isTextOutside() && text != "")
			{
				RectangleF textRect = shapeTemplate.getTextRect(shapeData, 0);
				float angle = rotateContents ? rotation() : 0;
				if (angle != 0)
					textRect = getRotatedBounds(textRect, getCenter(), angle);
				repaintRect = RectangleF.Union(repaintRect, textRect);
			}

			// include the rotation handle
			if ((enabledHandles & Handles.Rotate) != 0)
			{
				// find the rotation handle position
				RectangleF rc = getBoundingRect();
				PointF pivot = getCenter();
				PointF point = new PointF(pivot.X,
					rc.Top - 6 * Constants.getMillimeter(fcParent.MeasureUnit) -
					2.8F * fcParent.SelHandleSize / 4);
				if (rotation() != 0)
					point = Utilities.rotatePointAt(point, pivot, rotation());

				// add it to the repaint rectangle
				if (!repaintRect.Contains(point))
					repaintRect = RectangleF.Union(repaintRect,
						new RectangleF(point, new SizeF(0.0001f, 0.0001f)));
			}

			return repaintRect;
		}

		#endregion

		#region interaction

		internal override void startModify(PointF org, int handle, InteractionState ist)
		{
			// save rotation angle so it can be restored if interaction is cancelled
			if (handle == 9 && ist.CurrentObject == this)
				ist.savedAngle = RotationAngle;

			if (rotation() != 0 && handle < 8)
			{
				org = Utilities.rotatePointAt(org,
					Utilities.getCenter(getBoundingRect()), -rotation());
			}

			base.startModify(org, handle, ist);
		}

		internal override void updateModify(PointF current, InteractionState ist)
		{
			// perform rotation
			if (modifyHandle == 9)
			{
				float a = 0;
				float r = 0;
				MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
					Utilities.getCenter(getBoundingRect()), current, ref a, ref r);
				setRotation(90 - a);
			}

			// handle resizing while the box is rotated
			if (rotation() != 0 && modifyHandle < 8)
			{
				current = Utilities.rotatePointAt(current,
					Utilities.getCenter(getBoundingRect()), -rotation());
			}

			base.updateModify(current, ist);
		}

		internal override bool allowModify(PointF current)
		{
			if (rotation() != 0 && modifyHandle < 8)
			{
				current = Utilities.rotatePointAt(current,
					Utilities.getCenter(getBoundingRect()), -rotation());
			}

			return base.allowModify(current);
		}

		internal override void completeModify(PointF end, InteractionState ist)
		{
			if (rotation() != 0 && modifyHandle < 8)
			{
				end = Utilities.rotatePointAt(end,
					Utilities.getCenter(getBoundingRect()), -rotation());
			}

			base.completeModify(end, ist);
		}

		internal override void cancelModify(InteractionState ist)
		{
			if (ist.SelectionHandle == 9 && ist.CurrentObject == this)
				setRotation(ist.savedAngle);

			base.cancelModify(ist);
		}

		#endregion

		#region serialization

		public override int getClassId()
		{
			return 3;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			writer.Write(Expanded);
			writer.Write(Expandable);
			writer.Write((long)enabledHandles);
			writer.Write((int)style);
			ctx.saveColor(fillColor);
			ctx.saveColor(frameColor);
			writer.Write((int)ImageAlign);
			writer.Write(text);
			ctx.saveColor(textColor);
			ctx.saveStringFormat(textFormat);
			writer.Write(transparent);

			ctx.saveReference(this, incomingArrows, 1);
			ctx.saveReference(this, outgoingArrows, 2);
			writer.Write((int)0/*anchorIncoming*/);
			writer.Write((int)0/*anchorOutgoing*/);
			writer.Write((int)customDraw);

			ctx.saveImage(image);

			ctx.saveObject(pen);
			ctx.saveReference(this, brush, 3);

			writer.Write((int)selStyle);

			// new in save format 5
			ctx.saveAnchorPattern(anchorPattern);

			// new in save format 6
			writer.Write(PolyTextLayout);
			writer.Write((double)shapeRotation);

			if (shapeTemplate != null)
			{
				writer.Write((int)0);
				writer.Write(shapeTemplate.Id);
				if (shapeTemplate.Id == "")
				{
					ctx.saveObject(shapeTemplate);
					ctx.saveObject(shapeData);
				}
			}
			else
			{
				writer.Write((int)-1);
			}

			// new in save format 13
			// now in Node: writer.Write(hyperLink);

			// new in save format 14
			writer.Write(useStyledText);

			// new in save format 22
			writer.Write(rotationAngle);
			writer.Write(rotateContents);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			int intStyle = 0;
			ShapeTemplate polyShape = null;

			setExpanded(reader.ReadBoolean());
			setExpandable(reader.ReadBoolean());
			enabledHandles = (Handles)reader.ReadInt64();
			intStyle = reader.ReadInt32();
			if (intStyle == 4) // Polygon
				style = BoxStyle.Shape;
			else
				style = (BoxStyle)intStyle;
			fillColor = ctx.loadColor();
			frameColor = ctx.loadColor();
			picturePos = (ImageAlign)reader.ReadInt32();
			text = reader.ReadString();
			textColor = ctx.loadColor();
			textFormat = ctx.loadStringFormat();
			transparent = reader.ReadBoolean();

			// this was used with the now obsolete bsPolygon boxes
			if (ctx.FileVersion < 22)
			{
				ctx.loadPtfArray();
				int c = reader.ReadInt32();
				byte[] b = reader.ReadBytes(c);

				if (intStyle == 4) // Polygon
					polyShape = new ShapeTemplate(b);
			}

			ctx.loadReference(this);
			ctx.loadReference(this);
			ArrowAnchor anchorIncoming =(ArrowAnchor)reader.ReadInt32();
			ArrowAnchor anchorOutgoing= (ArrowAnchor)reader.ReadInt32();
			customDraw = (CustomDraw)reader.ReadInt32();

			image = ctx.loadImage();

			pen.Width = PenWidth;
			pen.Color = frameColor;
			pen.DashStyle = PenDashStyle;
			brush.Release();
			brush = new SolidBrush(fillColor);
			brush.AddRef();

			if (ctx.FileVersion > 2)
			{
				if (ctx.FileVersion < 6)
				{
					shapeTemplate = (ShapeTemplate)ctx.loadObject();
					shapeData = (ShapeTemplate.PathData)ctx.loadObject();
				}

				if (ctx.FileVersion > 3)
				{
					pen = (Pen)ctx.loadObject();
					ctx.loadReference(this);

					selStyle = (HandlesStyle)reader.ReadInt32();

					if (ctx.FileVersion > 4)
					{
						// new in save format 5
						anchorPattern = ctx.loadAnchorPattern();

						if (ctx.FileVersion > 5)
						{
							// new in save format 6
							PolyTextLayout = reader.ReadBoolean();
							ShapeOrientation = (float)reader.ReadDouble();

							int hasShape = reader.ReadInt32();
							if (hasShape != -1)
							{
								string shpId = reader.ReadString();
								if (shpId == "")
								{
									shapeTemplate = (ShapeTemplate)ctx.loadObject();
									shapeData = (ShapeTemplate.PathData)ctx.loadObject();
								}
								else
								{
									ShapeTemplate stvalue = ShapeTemplate.FromId(shpId);
									ShapeTemplate newVal = fcParent.DefaultShape;
									if (stvalue != null)
										newVal = stvalue;
									if (newVal.Id == "")
										shapeTemplate = (ShapeTemplate)newVal.Clone();
									else
										shapeTemplate = newVal;
									shapeData = null;
								}
								updateShapePoints();
							}

							if (intStyle == 4) // Polygon
							{
								shapeTemplate = polyShape;
								shapeData = null;

								updateShapePoints();
							}

							if (ctx.FileVersion > 12)
							{
								// new in save format 13
								// now loaded in Node::loadFrom
								if (ctx.FileVersion < 15)
									hyperLink = reader.ReadString();

								if (ctx.FileVersion > 13)
								{
									// new in save format 14
									useStyledText = reader.ReadBoolean();

									if (ctx.FileVersion > 21)
									{
										// new in save format 22
										rotationAngle = reader.ReadSingle();
										rotateContents = reader.ReadBoolean();
									}
								}
							}
						}
					}
				}
			}

			updateText();
			layoutText();
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId, obj);

			switch (refId)
			{
				case 1:
					incomingArrows = (ArrowCollection)obj;
					break;
				case 2:
					outgoingArrows = (ArrowCollection)obj;
					break;
				case 3:
					brush.Release();
					brush = (Brush)obj;
					brush.AddRef();
					break;
			}
		}

		#endregion

		// ************ modification ************

		protected override void onRectUpdate()
		{
			updateShapePoints();
		}

		// ************ drawing ************

		public override void Draw(Graphics g, bool shadow)
		{
			if (invisible) return;
			if (fcParent.NowPrinting && !Printable) return;

			System.Drawing.Brush br = null;
			System.Drawing.Pen p = null;

			// select appropriate brush and pen
			if (shadow)
			{
				br = new System.Drawing.SolidBrush(ShadowColor);
				p = this.Pen.CreateGDIPen();
				p.Brush = br;
			}
			else
			{
				if (fcParent.RenderOptions.EnableInterior)
					br = this.Brush.CreateGDIBrush(getRotatedBounds());
				else
					br = new System.Drawing.SolidBrush(Color.FromArgb(0, 255, 255, 255));
				p = this.Pen.CreateGDIPen();
			}

			// draw with offset - used for drawing shadows
			RectangleF rc = Utilities.normalizeRect(rect);
			if (shadow)
				rc.Offset(ShadowOffsetX, ShadowOffsetY);

			// draw the appropriate shape
			if (!transparent)
			{
				if (customDraw == CustomDraw.Full)
				{
					// call the custom draw function
					fcParent.drawBox(g, this, shadow, rc);
				}
				else if (customDraw == CustomDraw.ShadowOnly && shadow)
				{
					Region oldClipReg = g.Clip;

					RectangleF rct = Utilities.normalizeRect(rect);
					rct.Offset(ShadowOffsetX, ShadowOffsetY);

					Region newClipReg = getRgnInRect(rct);
					if (newClipReg != null)
						g.SetClip(newClipReg, CombineMode.Intersect);

					fcParent.drawBox(g, this, true, rc);

					g.SetClip(oldClipReg, CombineMode.Replace);
					if (newClipReg != null) newClipReg.Dispose();
				}
				else
				{
					switch (style)
					{
					case BoxStyle.Rectangle:
						g.FillRectangle(br, rc);
						if (!shadow) drawInterior(g);
						g.DrawRectangle(p, rc.Left, rc.Top, rc.Width, rc.Height);
						break;
					case BoxStyle.Ellipse:
						g.FillEllipse(br, rc);
						if (!shadow) drawInterior(g);
						g.DrawEllipse(p, rc);
						break;
					case BoxStyle.RoundedRectangle:
						GraphicsPath gp = Utilities.getRoundRect(
							rc.X, rc.Y, rc.Width, rc.Height, Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));
						g.FillPath(br, gp);
						if (!shadow) drawInterior(g);
						g.DrawPath(p, gp);
						gp.Dispose();
						break;
					case BoxStyle.Delay:
						if (rc.Width > 0 && rc.Height > 0)
						{
							gp = Utilities.getDelaySymbol(rc);
							g.FillPath(br, gp);
							if (!shadow) drawInterior(g);
							g.DrawPath(p, gp);
							gp.Dispose();
						}
						break;
					case BoxStyle.Rhombus:
						// don't draw if width or height is zero
						if (rc.Width == 0 || rc.Height == 0) break;

						// calculate the vertex coordinates
						Utilities.normalizeRect(rc);
						PointF pt0 = new PointF(
							(rc.Left + rc.Right) / 2, rc.Top);
						PointF pt1 = new PointF(
							Math.Max(rc.Right, rc.Left), (rc.Top + rc.Bottom) / 2);
						PointF pt2 = new PointF(
                            pt0.X, Math.Max(rc.Bottom, rc.Top));
						PointF pt3 = new PointF(
                            rc.Left, pt1.Y);
						PointF[] pts = { pt0, pt1, pt2, pt3 };

						// draw the rhombus
						g.FillPolygon(br, pts);
						if (!shadow) drawInterior(g);
						g.DrawPolygon(p, pts);

						break;
					case BoxStyle.Shape:
						if (rc.Width > 0  && rc.Height > 0)
						{
							/*
							if (shadow && this.shadow != null)
							{
								RectangleF b = this.BoundingRect;
								g.DrawImage(this.shadow,
									b.X + ShadowOffsetX - dispersion,
									b.Y + ShadowOffsetY - dispersion,
									b.Width + 2 * dispersion, b.Height + 2 * dispersion);
							}
							else
							{*/
								GraphicsPath path = shapeTemplate.getPath(
									shapeData, rotation());

								if (shadow)
								{
									Matrix matrix = new Matrix();
									matrix.Translate(ShadowOffsetX, ShadowOffsetY);
									path.Transform(matrix);
								}

								g.FillPath(br, path);
								if (!shadow) drawInterior(g);
								g.DrawPath(p, path);
								/*
								{
									// Emboss
									RectangleF b = this.BoundingRect;
									Region oldClip = g.Clip;
									g.SetClip(path, CombineMode.Intersect);
									g.DrawImage(this.emboss, b.X - 1.5f, b.Y - 1.5f, emboss.Width, emboss.Height);
									g.Clip = oldClip;
								}
								*/
								path.Dispose();

								// Draw decorations
								path = shapeTemplate.getDecorationPath(
									shapeData, rotation());
								if (path != null)
								{
									g.DrawPath(p, path);
									path.Dispose();
								}
							//}
						}
						break;
					}
				}
			}
			else
			{
				if (!shadow && customDraw != CustomDraw.Full)
					drawInterior(g);
			}

			// additional2 custom draw type
			if (customDraw == CustomDraw.Additional2)
				fcParent.drawBox(g, this, shadow, rc);

			if (!shadow)
			{
				drawManipulators(g, false);
				if (fcParent.RenderOptions.EnableAnchors)
					drawAnchors(g);
			}

			p.Dispose();
			br.Dispose();
		}

		private void drawInterior(Graphics g)
		{
			Image image = getImage();
			if (image != null || customDraw == CustomDraw.Additional)
			{
				Region oldClipReg = g.Clip;

				RectangleF rct = Utilities.normalizeRect(rect);
				Region newClipReg = getRgnInRect(rct);
				if (newClipReg != null)
					g.SetClip(newClipReg, CombineMode.Intersect);

				if (image != null && fcParent.RenderOptions.EnableImages)
				{
					float angle = 0;
					if (rotateContents)
						angle = rotation();

					PointF pivot = new PointF(
						rect.X + rect.Width / 2,
						rect.Y + rect.Height / 2);

					Utilities.drawImage(g, image, getImageRect(), picturePos, angle, pivot);
				}

				// additional custom draw type
				if (customDraw == CustomDraw.Additional)
					fcParent.drawBox(g, this, false, rct);
					
				g.SetClip(oldClipReg, CombineMode.Replace);
			}

			// DrawString crashes with a too small PageScale
			if (g.PageScale > 0.01 && text.Length > 0 &&
				fcParent.RenderOptions.EnableText)
			{
				System.Drawing.Brush brText =
					new System.Drawing.SolidBrush(textColor);
				RectangleF rcf = Utilities.normalizeRect(rect);

				if (rcf.Width > 0 && rcf.Height > 0)
				{
					PointF center = new PointF(
						rcf.X + rcf.Width / 2,
						rcf.Y + rcf.Height / 2);

					if (rotateContents && rotation() != 0)
					{
						// Apply rotation transformation
						g.TranslateTransform(center.X, center.Y);
						g.RotateTransform(rotation());
						g.TranslateTransform(-center.X, -center.Y);
					}

					if (canLayoutText() && useTextLayout)
					{
						txLayout.Draw(rcf.Left, rcf.Top,
							new RenderTextCallback(DrawText),
							new DrawTextHint(g, Font, brText, nearFormat,
								Utilities.getTextEllipsis(textFormat)));
					}
					else
					{
						if (style == BoxStyle.Shape &&
							shapeTemplate != null && shapeTemplate.hasTextArea())
						{
							float angle = rotateContents ? 0 : rotation();
							rcf = shapeTemplate.getTextRect(shapeData, angle);
						}

						g.DrawString(text, Font, brText, rcf, textFormat);
					}

					if (rotateContents && rotation() != 0)
					{
						// Remove rotation
						g.TranslateTransform(center.X, center.Y);
						g.RotateTransform(-rotation());
						g.TranslateTransform(-center.X, -center.Y);
					}
				}

				brText.Dispose();
			}
		}

		internal override void drawSelHandles(Graphics g, Color color)
		{
			if (selStyle != HandlesStyle.Custom)
			{
				Handles mask = style == BoxStyle.Shape ?
					enabledHandles : enabledHandles & ~Handles.Rotate;
				RectangleF rc = Utilities.normalizeRect(rect);
				Utilities.drawSelHandles(g, color, fcParent.DisabledMnpColor,
					rc, rotation(), mask, fcParent.ShowDisabledHandles,
					selStyle, fcParent.SelHandleSize);
			}
			else
			{
				// let applications custom draw the selection handles
				fcParent.fireDrawSelHandles(g, this);
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

		private HandlesStyle selStyle;


		// ************ placement ************

		internal override bool containsPoint(PointF pt)
		{
			RectangleF bounds = getRotatedBounds();
			bounds.Inflate(0.05f, 0.05f);
			if (!bounds.Contains(pt))
				return false;

			//check if the shape contains pt
			switch (style)
			{
			case BoxStyle.Rectangle:
			case BoxStyle.RoundedRectangle:
				return Utilities.pointInRect(pt, rect);
			case BoxStyle.Delay:
				{
					float cx = (rect.Left + rect.Right) / 2;
					float cy = (rect.Top + rect.Bottom) / 2;
					float x = pt.X - cx;
					float y = pt.Y - cy;
					float a = (rect.Right - rect.Left) / 2;
					float b = (rect.Bottom - rect.Top) / 2;

					//ellipse equation
					if ((x*x)/(a*a) + (y*y)/(b*b) <= 1)
						return true;
					RectangleF rc = Utilities.normalizeRect(rect);
					rc.Width = rc.Width / 2;
					return Utilities.pointInRect(pt, rc);
				}
			case BoxStyle.Ellipse:
				{
					double cx = (double)(rect.Left + rect.Right) / 2;
					double cy = (double)(rect.Top + rect.Bottom) / 2;
					double x = pt.X - cx;
					double y = pt.Y - cy;
					double a = (double)(rect.Right - rect.Left) / 2;
					double b = (double)(rect.Bottom - rect.Top) / 2;

					//ellipse equation
					return ((x*x)/(a*a) + (y*y)/(b*b) <= 1);
				}
			case BoxStyle.Rhombus:
				{
					PointF pt0, pt1, pt2, pt3;
					RectangleF rc = rect;

					// don't draw if width or height is zero
					if (rc.Width == 0 || rc.Height == 0) return false;

					// calculate the vertex coordinates
					Utilities.normalizeRect(rc);
					pt0 = new PointF(
						(rc.Left + rc.Right) / 2, rc.Top);
					pt1 = new PointF(
						Math.Max(rc.Right, rc.Left), (rc.Top + rc.Bottom) / 2);
					pt2 = new PointF(pt0.X, Math.Max(rc.Bottom, rc.Top));
					pt3 = new PointF(rc.Left, pt1.Y);
					PointF[] pts = { pt0, pt1, pt2, pt3 };

					return Utilities.pointInPolygon(pts, pt);
				}
			case BoxStyle.Shape:
				{
					if (rect.Width == 0 || rect.Height == 0) return false;

					GraphicsPath path = shapeTemplate.getPath(
						shapeData, rotation());
					Graphics g = fcParent.CreateGraphics();
					fcParent.setTransforms(g);
					bool res = path.IsVisible(pt, g);
					g.Dispose();
					path.Dispose();
						
					return res;
				}
			}
			
			return false;
		}

		public override bool HitTestHandle(PointF pt, ref int handle)
		{
			if (selStyle != HandlesStyle.Custom)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				if (ptInManipulator(pt)) return false;

				Handles mask = style == BoxStyle.Shape ?
					enabledHandles : enabledHandles & ~Handles.Rotate;

				return Utilities.pointInHandle(pt,
					ref handle, rc, rotation(), mask, 0, selStyle,
					fcParent.SelHandleSize, fcParent.MeasureUnit);
			}
			else
			{
				// let the application do hit testing
				return fcParent.hitTestSelHandles(this, pt, ref handle);
			}
		}

		internal override int getHandleIdx(PointF pt)
		{
			int handle = -1;

			RectangleF rc = Utilities.normalizeRect(rect);
			if (Utilities.pointInHandle(pt, ref handle, rc, rotation(),
				Handles.All, 0, selStyle, fcParent.SelHandleSize, fcParent.MeasureUnit))
				return handle;

			return -1;
		}

		/// <summary>
		/// Finds the intersection of the box contour with the line
		/// segment defined by the points supplied as arguments
		/// to the function.
		/// </summary>
		internal override PointF getIntersection(PointF ptOrg, PointF ptEnd)
		{
			if (ptOrg == ptEnd) return ptOrg;
			PointF pt = getCenter();
			if (rect.Bottom == rect.Top || rect.Left == rect.Right)
				return pt;

			RectangleF rc = new RectangleF(ptOrg.X, ptOrg.Y,
				ptEnd.X - ptOrg.X, ptEnd.Y - ptOrg.Y);
			rc = Utilities.normalizeRect(rc);

			RectangleF rcBox = Utilities.normalizeRect(rect);

			float x1 = ptOrg.X;
			float y1 = ptOrg.Y;
			float x2 = ptEnd.X;
			float y2 = ptEnd.Y;

			switch (style)
			{
				case BoxStyle.Rectangle:
				case BoxStyle.RoundedRectangle:
					if (Utilities.pointInRect(ptOrg, rcBox) &&
						Utilities.pointInRect(ptEnd, rcBox)) return getCenter();

					Utilities.getRectIntr(rcBox, ptOrg, ptEnd, ref pt);

					break;
				case BoxStyle.Ellipse:
					if (Utilities.pointInEllipse(ptOrg, rect) &&
						Utilities.pointInEllipse(ptEnd, rect))
						return getCenter();

					Utilities.getEllipseIntr(rcBox, ptOrg, ptEnd, ref pt);

					break;
				case BoxStyle.Delay:
				{
					RectangleF rc1 = new RectangleF(
						rcBox.X, rcBox.Y, rcBox.Width / 2, rcBox.Height);

					if (Utilities.pointInRect(ptOrg, rc1) &&
						Utilities.pointInRect(ptEnd, rc1)) return getCenter();

					if (Utilities.pointInEllipse(ptOrg, rcBox) &&
						Utilities.pointInEllipse(ptEnd, rcBox))
						return getCenter();

					Utilities.getRectIntr(rc1, ptOrg, ptEnd, ref pt);
					if (pt.X >= rc1.Right - 0.3)
						Utilities.getEllipseIntr(rcBox, ptOrg, ptEnd, ref pt);

				}
					break;
				case BoxStyle.Rhombus:
				{
					PointF ptt, ptr, ptb, ptl;
		
					ptl = new PointF(
						rcBox.Left, (rcBox.Top + rcBox.Bottom) / 2);
					ptt = new PointF(
						(rcBox.Left + rcBox.Right) / 2, rcBox.Top);
					ptr = new PointF(rcBox.Right, ptl.Y);
					ptb = new PointF(ptt.X, rcBox.Bottom);

					PointF[] poly = { ptl, ptt, ptr, ptb };
					if (Utilities.pointInPolygon(poly, ptOrg) &&
						Utilities.pointInPolygon(poly, ptEnd))
						return getCenter();

					pt = Utilities.lineIntersect(ptl, ptt, ptOrg, ptEnd);
					if (pt.X >= ptl.X && pt.X <= ptt.X &&
						pt.Y >= ptt.Y && pt.Y <= ptl.Y &&
						pt.X >= rc.Left && pt.X <= rc.Right &&
						pt.Y >= rc.Top && pt.Y <= rc.Bottom) break;

					pt = Utilities.lineIntersect(ptt, ptr, ptOrg, ptEnd);
					if (pt.X <= ptr.X && pt.X >= ptt.X &&
						pt.Y >= ptt.Y && pt.Y <= ptr.Y &&
						pt.X >= rc.Left && pt.X <= rc.Right &&
						pt.Y >= rc.Top && pt.Y <= rc.Bottom) break;

					pt = Utilities.lineIntersect(ptb, ptr, ptOrg, ptEnd);
					if (pt.X <= ptr.X && pt.X >= ptb.X &&
						pt.Y <= ptb.Y && pt.Y >= ptr.Y &&
						pt.X >= rc.Left && pt.X <= rc.Right &&
						pt.Y >= rc.Top && pt.Y <= rc.Bottom) break;

					pt = Utilities.lineIntersect(ptl, ptb, ptOrg, ptEnd);
					if (pt.X >= ptl.X && pt.X <= ptb.X &&
						pt.Y <= ptb.Y && pt.Y >= ptl.Y &&
						pt.X >= rc.Left && pt.X <= rc.Right &&
						pt.Y >= rc.Top && pt.Y <= rc.Bottom) break;
				}
					break;
				case BoxStyle.Shape:
				{
					if (containsPoint(ptOrg) && containsPoint(ptEnd))
						return getCenter();
					if (rect.Width != 0 && rect.Height != 0)
					{
						GraphicsPath path = shapeTemplate.getPath(
							shapeData, rotation());
						path.Flatten();
						Utilities.getPathIntersection(path, ptOrg, ptEnd, ref pt);
						path.Dispose();
					}
				}
					break;
			}

			return pt;
		}

		internal PointF getBoxPtPercent(PointF point)
		{
			float angle = rotation();
			if (angle != 0)
			{
				PointF pivot = Utilities.getCenter(rect);
				point = Utilities.rotatePointAt(point, pivot, -angle);
			}

			return Utilities.getRectPtPercent(point, getBoundingRect());
		}

		internal PointF pointFromPercent(PointF ptPer)
		{
			PointF point = Utilities.rectPtFromPercent(ptPer, getBoundingRect());
			float angle = rotation();

			if (angle != 0)
			{
				PointF pivot = Utilities.getCenter(rect);
				point = Utilities.rotatePointAt(point, pivot, angle);
			}

			return point;
		}

		internal override Region getRegion()
		{
			Region rgn = null;
			RectangleF rc = Utilities.normalizeRect(rect);

			switch (style)
			{
			case BoxStyle.Rectangle:
				rgn = new Region(rc);
				break;
			case BoxStyle.Ellipse:
				rgn = Utilities.createEllipticRgn(rc);
				break;
			case BoxStyle.Delay:
				{
					rgn = Utilities.createEllipticRgn(rc);
					RectangleF rc1 = new RectangleF(rc.X, rc.Y, rc.Width / 2, rc.Height);
					Region rgn2 = new Region(rc1);
					rgn.Union(rgn2);
					rgn2.Dispose();
				}
				break;
			case BoxStyle.RoundedRectangle:
				rgn = Utilities.createRoundRectRgn(rc, Constants.getRoundRectArc(fcParent.MeasureUnit, fcParent.RoundRectFactor));
				break;
			case BoxStyle.Rhombus:
				{
					PointF pt0, pt1, pt2, pt3;

					// calculate the vertex coordinates
					pt0 = new PointF(
						(rc.Left + rc.Right) / 2, rc.Top);
					pt1 = new PointF(
						Math.Max(rc.Right, rc.Left), (rc.Top + rc.Bottom) / 2);
					pt2 = new PointF(
						pt0.X, Math.Max(rc.Bottom, rc.Top));
					pt3 = new PointF(rc.Left, pt1.Y);
					PointF[] pts = { pt0, pt1, pt2, pt3 };
					
					rgn = Utilities.createPolygonRgn(pts);
				}
				break;
			case BoxStyle.Shape:
				{
					GraphicsPath path = shapeTemplate.getPath(
						shapeData, rotation());
					rgn = new Region(path);
					path.Dispose();
				}
				break;
			}

			return rgn;
		}


		#region interface: appearance

		/// <summary>
		/// Defines the shape of the box
		/// </summary>
		public BoxStyle Style
		{
			get
			{
				return style;
			}
			set
			{
				if (style != value)
				{
					fcParent.invalidate(getRepaintRect(true));

					setStyle(value);
					updateArrowsIntsc();

					fcParent.invalidate(getRepaintRect(true));
					fcParent.setDirty();
				}
			}
		}

		/// <summary>
		/// Image painted inside the box
		/// </summary>
		public Image Image
		{
			get
			{
				return image;
			}
			set
			{
				image = value;
				fcParent.setDirty();
				fcParent.invalidate(getRepaintRect(false));
			}
		}

		/// <summary>
		/// Defines the placement of the box image
		/// </summary>
		public ImageAlign ImageAlign
		{
			get
			{
				return picturePos;
			}
			set
			{
				if (picturePos != value)
				{
					picturePos = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Only the box image and text are displayed if set
		/// </summary>
		public bool Transparent
		{
			get
			{
				return transparent;
			}
			set
			{
				if (transparent != value)
				{
					transparent = value;
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		#endregion

		#region data: appearance

		private Image getImage()
		{
			if (image != null)
				return image;

			if (style == BoxStyle.Shape && shapeTemplate != null)
				return shapeTemplate.Image;

			return null;
		}

		private RectangleF getImageRect()
		{
			RectangleF imageRect = Utilities.normalizeRect(rect);

			if (style == BoxStyle.Shape && shapeTemplate != null)
			{
				RectangleF percent = shapeTemplate.ImageRectangle;
				float w = imageRect.Width;
				float h = imageRect.Height;

				imageRect.X = imageRect.X + w * percent.X / 100;
				imageRect.Y = imageRect.Y + h * percent.Y / 100;
				imageRect.Width = w * percent.Width / 100;
				imageRect.Height = h * percent.Height / 100;
			}

			return imageRect;
		}

		private BoxStyle style;
		private Image image;
		private ImageAlign picturePos;
		private bool transparent;

		#endregion


		#region data: text

		private string text;
		private Text.Text ftext;
		private Color textColor;
		private StringFormat textFormat;
		private static StringFormat nearFormat = new StringFormat();
		private EventHandler parentUnitChanged = null;

		private bool useTextLayout;
		private bool useStyledText;
		private Text.LayoutOptions txOptions;
		private Text.Layout txLayout;

		#endregion


		#region interface: text

		/// <summary>
		/// Text displayed in the box.
		/// </summary>
		public string Text
		{
			get { return text; }
			set
			{
				string newText = value;
				if (newText == null)
					newText = "";

				if (!text.Equals(newText))
				{
					text = newText;

					updateText();
					layoutText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public string PlainText
		{
			get
			{
				if (ftext == null)
					return text;

				return ftext.PlainText;
			}
		}

		/// <summary>
		/// The box Text is painted in this color.
		/// </summary>
		public Color TextColor
		{
			get { return textColor; }
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

		/// <summary>
		/// Defines the text position and alignment.
		/// </summary>
		[Editor(typeof(MindFusion.FlowChartX.Design.StringFormatEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public StringFormat TextFormat
		{
			get { return textFormat; }
			set
			{
				textFormat = new StringFormat(value);

				layoutText();
				fcParent.setDirty();
				fcParent.invalidate(getRepaintRect(false));
			}
		}

		public override System.Drawing.Font Font
		{
			get { return base.Font; }
			set
			{
				base.Font = value;

				updateText();
				layoutText();
			}
		}

		public bool EnableStyledText
		{
			get { return useStyledText; }
			set
			{
				if (useStyledText != value)
				{
					useStyledText = value;

					updateText();
					layoutText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		public bool PolyTextLayout
		{
			get { return useTextLayout; }
			set
			{
				if (useTextLayout != value)
				{
					useTextLayout = value;

					updateText();
					layoutText();
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		#endregion


		#region implemetation: text

		private void OnParentUnitChanged(object sender, EventArgs e)
		{
			updateText();
			layoutText();
		}

		/// <summary>
		/// Updates the internal MindFusion.FlowChartX.Text.Text
		/// obejct when styled / laid-out characteristics of
		/// the box' text have changed.
		/// </summary>
		private void updateText()
		{
			Graphics graphics = fcParent.CreateGraphics();
			fcParent.setTransforms(graphics);

			if (useStyledText)
			{
				// Create and setup styled text
				ftext = new Text.StyledText();
				ftext.Setup(text, graphics, Font);
			}
			else
			{
				if (useTextLayout)
				{
					// Create and setup plain text
					ftext = new Text.PlainText();
					ftext.Setup(text, graphics, Font);
				}
				else
				{
					// Text won't be used
					ftext = null;
				}
			}

			graphics.Dispose();
		}

		/// <summary>
		/// Performs the actual text layouting so that
		/// the internal MindFusion.FlowChartX.Text.Layout object
		/// is prepared for subsequent drawing operations.
		/// Returns whether text fits entirely whithin the area.
		/// </summary>
		private bool layoutText()
		{
			// Check if text layouting is apporpiate
			// in the current context
			if (!canLayoutText())
				return false;

			// Check if text layouting is switched on for this box
			if (!useTextLayout)
				return false;

			// Ensure that the layout options are adequate
			txOptions.LineDistance = 0;
			txOptions.Alignment = textFormat.Alignment;
			txOptions.LineAlignment = textFormat.LineAlignment;

			// Perform the appropriate layout (depending on the box shape)
			RectangleF rc = Utilities.normalizeRect(rect);
			switch (style)
			{
				case BoxStyle.Shape:
					if (shapeTemplate != null)
					{
						// Apply rotation to the shape only if box contents are NOT rotated
						float angle = 0;
						if (!rotateContents)
							angle = rotation();

						return txLayout.LayoutInPolygon(ftext,
							docToLocal(shapeTemplate.getTextArea(shapeData, angle)), txOptions);
					}
					break;

				case BoxStyle.Rectangle:
				case BoxStyle.RoundedRectangle:
					return txLayout.LayoutInRectangle(ftext, docToLocal(rc), txOptions);

				case BoxStyle.Ellipse:
					return txLayout.LayoutInEllipse(ftext, docToLocal(rc), txOptions);

				case BoxStyle.Rhombus:
				{
					PointF[] pts = new PointF[]
						{
							new PointF((rc.Left + rc.Right) / 2, rc.Top),
							new PointF(rc.Right, (rc.Top + rc.Bottom) / 2),
							new PointF((rc.Left + rc.Right) / 2, rc.Bottom),
							new PointF(rc.Left, (rc.Top + rc.Bottom) / 2)
						};
					return txLayout.LayoutInPolygon(ftext, docToLocal(pts), txOptions);
				}

			}

			return false;
		}

		/// <summary>
		/// Transforms the specified points from document
		/// coordinates to coordinates relative to the left-top
		/// point of the box.
		/// </summary>
		private PointF[] docToLocal(PointF[] points)
		{
			PointF[] result = (PointF[])points.Clone();
			RectangleF nrect = Utilities.normalizeRect(this.rect);

			for (int i = 0; i < result.Length; i++)
			{
				result[i].X -= nrect.X;
				result[i].Y -= nrect.Y;
			}

			return result;
		}

		/// <summary>
		/// Transforms the specified rectangle from document
		/// coordinates to coordinates relative to the left-top
		/// point of the box.
		/// </summary>
		private RectangleF docToLocal(RectangleF rect)
		{
			RectangleF result = rect;
			RectangleF nrect = Utilities.normalizeRect(this.rect);

			result.X -= nrect.X;
			result.Y -= nrect.Y;

			return result;
		}

		/// <summary>
		/// Laid-out text rendering callback.
		/// </summary>
		private void DrawText(string text, RectangleF dest, DrawTextHint hint)
		{
			// Render formatted text
			hint.Graphics.DrawString(text, hint.Font, hint.Brush,
				dest, hint.Format);
		}

		/// <summary>
		/// Determines whether the text can be
		/// laid-out in the current context.
		/// </summary>
		private bool canLayoutText()
		{
			return (style != BoxStyle.Delay);
		}

		#endregion


		#region implementation: appearance

		private void setStyle(BoxStyle newStyle)
		{
			bool changed = style != newStyle;
			style = newStyle;

			if (style == BoxStyle.Shape && shapeTemplate == null)
				Shape = fcParent.DefaultShape;

			updateShapePoints();

			if (changed)
				updPosArrows();
		}

		#endregion


		#region interface: placement

		/// <summary>
		/// Makes the box the same size as its image
		/// </summary>
		public void FitSizeToImage()
		{
			fcParent.invalidate(getRepaintRect(true));

			Image image = getImage();
			if (image != null)
			{
				int w = image.Width;
				int h = image.Height;

				Graphics g = fcParent.CreateGraphics();
				fcParent.setTransforms(g);
				RectangleF sizeDev = RectangleF.FromLTRB(0, 0,
					(float)w / image.HorizontalResolution * g.DpiX * g.PageScale,
					(float)h / image.VerticalResolution * g.DpiY * g.PageScale);
				RectangleF newRect = Utilities.deviceToDoc(g, sizeDev);

				// consider the image rectangle set in the current shape template
				if (style == BoxStyle.Shape && shapeTemplate != null)
				{
					RectangleF imageRect = shapeTemplate.ImageRectangle;
					newRect.Width *= 100 / imageRect.Width;
					newRect.Height *= 100 / imageRect.Height;
				}

				newRect.X = rect.X;
				newRect.Y = rect.Y;
				rect = newRect;
				g.Dispose();
				
				updPosArrows();
				updateShapePoints();

				if (groupAttached != null)
					groupAttached.updateObjects(new InteractionState(this, -1, Action.Modify));
			}

			fcParent.invalidate(getRepaintRect(true));
			fcParent.setDirty();
		}

		/// <summary>
		/// Makes the box large enough to contain all the text
		/// </summary>
		public bool FitSizeToText(FitSize fit)
		{
			if (text.Length == 0)
				return false;

			float mm = Constants.getMillimeter(fcParent.MeasureUnit);
			RectangleF initial = rect;
			RectangleF initialRepaint = getRepaintRect(true);

			// increase only box height
			if (fit == FitSize.KeepWidth)
			{
				// check whether the box is wide enough to fit at least one letter
				SizeF size = fcParent.MeasureString("W",
					Font, int.MaxValue, textFormat);
				if (size.Width + mm > rect.Width)
					return false;

				if (canLayoutText() && useTextLayout)
				{
					// using FlowChart.NET polygonal text layout
					float stepy = Constants.getFitTextStep(fcParent.MeasureUnit);
					RectangleF rc = initial;

					// change height until the text fits
					float cy = 0;
					if (layoutText())
					{
						do
						{
							cy -= stepy;
							rc = initial;
							rc.Inflate(0, cy);

							setRect(rc);
						}
						while (layoutText());

						cy += stepy;
						rc = initial;
						rc.Inflate(0, cy);
						setRect(rc);
					}
					else
					{
						while (!layoutText())
						{
							cy += stepy;
							rc = initial;
							rc.Inflate(0, cy);

							setRect(rc);

							if (rect.Height > mm * 400)
							{
								setRect(initial);
								return false;
							}
						}
					}
				}
				else
				{
					// using standard .NET text layout
					size = fcParent.MeasureString(text,
						Font, (int)rect.Width, textFormat);
					rect.Height = size.Height;
				}
			}

			if (fit == FitSize.KeepHeight)
			{
				// check whether the box is big enough to fit at least one line
				SizeF size = fcParent.MeasureString(text,
					Font, int.MaxValue, textFormat);
				if (size.Height + mm > rect.Height)
					return false;

				float stepx = Constants.getFitTextStep(fcParent.MeasureUnit);
				RectangleF rc = rect;

				if (canLayoutText() && useTextLayout)
				{
					// using FlowChart.NET polygonal text layout
					float cx = 0;
					if (layoutText())
					{
						do
						{
							cx -= stepx;
							rc = initial;
							rc.Inflate(cx, 0);

							setRect(rc);
						}
						while (layoutText());

						cx += stepx;
						rc = initial;
						rc.Inflate(cx, 0);
						setRect(rc);
					}
					else
					{
						while (!layoutText())
						{
							cx += stepx;
							rc = initial;
							rc.Inflate(cx, 0);

							setRect(rc);

							if (rect.Width > mm * 1000)
								break;
						}
					}
				}
				else
				{
					// change width until the text fits
					float cx = 0;
					size = fcParent.MeasureString(text,
						Font, (int)rect.Width, textFormat);
					if (size.Height < rect.Height)
					{
						do
						{
							cx -= stepx;
							rc = initial;
							rc.Inflate(cx, 0);

							setRect(rc);
							size = fcParent.MeasureString(text,
								Font, (int)rect.Width, textFormat);
						}
						while (size.Height < rect.Height);

						cx += stepx;
						rc = initial;
						rc.Inflate(cx, 0);
						setRect(rc);
					}
					else
					{
						size = fcParent.MeasureString(text,
							Font, (int)rect.Width, textFormat);
						while (size.Height > rect.Height)
						{
							cx += stepx;
							rc = initial;
							rc.Inflate(cx, 0);

							setRect(rc);
							size = fcParent.MeasureString(text,
								Font, (int)rect.Width, textFormat);

							if (rect.Width > mm * 1000)
								break;
						}
					}
				}
			}

			if (fit == FitSize.KeepRatio)
			{
				if (rect.Width == 0 || rect.Height == 0)
					return false;

				float stepx = Constants.getFitTextStep(fcParent.MeasureUnit);
				float stepy = Constants.getFitTextStep(fcParent.MeasureUnit);
				RectangleF rc = initial;

				float hvratio = rc.Width / rc.Height;
				float vhratio = rc.Height / rc.Width;

				if (hvratio < vhratio)
				{
					vhratio = vhratio / hvratio;
					hvratio = 1;
				}
				else
				{
					hvratio = hvratio / vhratio;
					vhratio = 1;
				}

				stepx = hvratio * stepx;
				stepy = vhratio * stepy;

				if (canLayoutText() && useTextLayout)
				{
					// using FlowChart.NET polygonal text layout
					float cx = 0;
					float cy = 0;
					if (layoutText())
					{
						do
						{
							cx -= stepx;
							cy -= stepy;
							rc = initial;
							rc.Inflate(cx, cy);

							setRect(rc);
						}
						while (layoutText());

						cx += stepx;
						cy += stepy;
						rc = initial;
						rc.Inflate(cx, cy);
						setRect(rc);
					}
					else
					{
						while (!layoutText())
						{
							cx += stepx;
							cy += stepy;
							rc = initial;
							rc.Inflate(cx, cy);

							setRect(rc);

							if (rect.Width > mm * 1000)
								break;
						}
					}
				}
				else
				{
					// change width until the text fits
					float cx = 0;
					float cy = 0;

					SizeF size = fcParent.MeasureString(text,
						Font, (int)rect.Width, textFormat);
					if (size.Height < rect.Height)
					{
						do
						{
							cx -= stepx;
							cy -= stepy;
							rc = initial;
							rc.Inflate(cx, cy);

							setRect(rc);
							size = fcParent.MeasureString(text,
								Font, (int)rect.Width, textFormat);
						}
						while (size.Height < rect.Height);

						cx += stepx;
						cy += stepy;
						rc = initial;
						rc.Inflate(cx, cy);
						setRect(rc);
					}
					else
					{
						size = fcParent.MeasureString(text,
							Font, (int)rect.Width, textFormat);
						while (size.Height > rect.Height)
						{
							cx += stepx;
							cy += stepy;
							rc = initial;
							rc.Inflate(cx, cy);

							setRect(rc);
							size = fcParent.MeasureString(text,
								Font, (int)rect.Width, textFormat);

							if (rect.Width > mm * 1000)
								break;
						}
					}
				}
			}

			// update shape outline and dependent object positions
			updPosArrows();
			updateShapePoints();

			if (groupAttached != null)
				groupAttached.updateObjects(new InteractionState(this, -1, Action.Modify));

			fcParent.invalidate(
				RectangleF.Union(getRepaintRect(true), initialRepaint));
			fcParent.setDirty();

			return true;
		}

		#endregion


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

		// ************ item type ************

		public override ItemType getType()
		{
			return ItemType.Box;
		}

		// ************ mouse pointers ************

		internal override Cursor getCannotDropCursor()
		{
			return fcParent.CurCannotCreate;
		}

		internal override Cursor getCanDropCursor()
		{
			return fcParent.CurPointer;
		}


		// ************ box shape ************

		[Editor(typeof(MindFusion.FlowChartX.Design.ShapeEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public ShapeTemplate Shape
		{
			get { return shapeTemplate; }
			set
			{
				ShapeTemplate newVal = fcParent.DefaultShape;

				if (value != null)
					newVal = value;

				// If the shape is contained within
				// the static list with shape templates,
				// set only reference to it
				if (newVal.Id == "")
					shapeTemplate = (ShapeTemplate)newVal.Clone();
				else
					shapeTemplate = newVal;

				shapeData = null;
				updateShapePoints();
				updateArrowsIntsc();

				fcParent.invalidate(getRepaintRect(false));
				fcParent.setDirty();
			}
		}

		public float ShapeOrientation
		{
			get { return shapeRotation; }
			set
			{
				shapeRotation = value;
				if (shapeRotation != 0 && shapeRotation != 90 &&
					shapeRotation != 180 && shapeRotation != 270)
					shapeRotation = 0;

				if (shapeTemplate != null)
				{
					updateShapePoints();
					fcParent.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Gets or sets the rotation angle of this box.
		/// </summary>
		public float RotationAngle
		{
			get { return rotationAngle; }

			set
			{
				if (rotationAngle != value)
				{
					fcParent.UndoManager.onStartPlacementChange(this);
					fcParent.invalidate(getRepaintRect(true));

					rotationAngle = value;
					updateShapePoints();
					visitArrows(
						new MethodCallVisitor(new VisitOperation(AV_UpdPosOutgoing)),
						new MethodCallVisitor(new VisitOperation(AV_UpdPosIncoming)));
					
					fcParent.setDirty();
					fcParent.invalidate(getRepaintRect(true));

					fcParent.UndoManager.onEndPlacementChange();
				}
			}
		}

		internal void setRotation(float angle)
		{
			if (rotationAngle != angle)
			{
				rotationAngle = angle;
				updateShapePoints();
			}
		}

		/// <summary>
		/// Gets or sets whether box contents rotate along with it.
		/// </summary>
		public bool RotateContents
		{
			get { return rotateContents; }
			set
			{
				if (rotateContents == value)
					return;

				rotateContents = value;

				layoutText();

				fcParent.invalidate(getRepaintRect(false));
				fcParent.setDirty();
			}
		}

		/// <summary>
		/// Returns the box rotation angle.
		/// </summary>
		/// <returns></returns>
		internal override float rotation()
		{
			if (style == BoxStyle.Shape)
				return rotationAngle;

			return 0;
		}

		private ShapeTemplate shapeTemplate;
		private ShapeTemplate.PathData shapeData;
		private float shapeRotation;

		void updateShapePoints()
		{
			if (style == BoxStyle.Shape)
			{
				RectangleF rc = Utilities.normalizeRect(rect);
				if (shapeData == null)
					shapeData = shapeTemplate.initData(rc, shapeRotation);
				else
					shapeTemplate.updateData(rc, shapeData, shapeRotation);

/*
				if (shadow != null)
					shadow.Dispose();

				GraphicsPath path = Shape.getPath(shapeData, rotation());
				shadow = MindFusion.Drawing.Effects.GenerateShadow(
					path, dispersion, ShadowColor);

				if (emboss != null)
					emboss.Dispose();

				emboss = MindFusion.Drawing.Effects.GenerateEmboss(path);
				*/
			}

			layoutText();
		}

		/*
		private Bitmap shadow = null;
		private int dispersion = 2;
		private Bitmap emboss = null;
		*/

		internal override PointF getAnchor(PointF pt,
			Arrow arrow, bool incm, ref int idx)
		{
			return getNearestAnchor(pt, arrow, incm, ref idx);
		}

		// ************ connected arrows ************

		internal override Link createLink(Arrow arrow, PointF pt, bool incoming)
		{
			return new BoxLink(this, arrow, incoming);
		}


		Region getRgnInRect(RectangleF rc)
		{
			Region rgn = null;

			switch (style)
			{
			case BoxStyle.Rectangle:
				rgn = new Region(rc);
				break;
			case BoxStyle.Ellipse:
				rgn = Utilities.createEllipticRgn(rc);
				break;
			case BoxStyle.RoundedRectangle:
				rgn = Utilities.createRoundRectRgn(rc, 7);
				break;
			case BoxStyle.Delay:
			{
				float w = rc.Width;

				GraphicsPath path = new GraphicsPath();
				if (rc.Width > 0 && rc.Height > 0)
				{
					path.AddLine(rc.X + w/2, rc.Bottom, rc.X, rc.Bottom);
					path.AddLine(rc.X, rc.Bottom, rc.X, rc.Top);
					path.AddLine(rc.X, rc.Top, rc.X + w/2, rc.Top);
					path.AddArc(rc.Left, rc.Top, w, rc.Height, -90, 180);
					path.CloseFigure();
				}

				rgn = new Region(path);
			}
			break;
			case BoxStyle.Rhombus:
				{
					PointF pt0 = new PointF(
						(rc.Left + rc.Right) / 2, rc.Top);
					PointF pt1 = new PointF(
						Math.Max(rc.Right, rc.Left), (rc.Top + rc.Bottom) / 2);
					PointF pt2 = new PointF(
						pt0.X, Math.Max(rc.Bottom, rc.Top));
					PointF pt3 = new PointF(
						rc.Left, pt1.Y);
					PointF[] pts = { pt0, pt1, pt2, pt3 };

					rgn = Utilities.createPolygonRgn(pts);
				}
				break;
			case BoxStyle.Shape:
				{
					ShapeTemplate.PathData pd =
						(ShapeTemplate.PathData)shapeData.Clone();
					shapeTemplate.updateData(rc, pd, shapeRotation);
					GraphicsPath path = shapeTemplate.getPath(pd, rotation());
					rgn = new Region(path);
					path.Dispose();
				}
				break;
			}

			return rgn;
		}

		// In-place editing
		internal override string getTextToEdit()
		{
			return Text;
		}

		internal override void setEditedText(string newText)
		{
			Text = newText;
		}

		internal override RectangleF getEditRect()
		{
			return this.BoundingRect;
		}

		public void BeginInplaceEdit()
		{
			fcParent.startInplaceEdit(new NodeInplaceEditable(this), getEditRect());
		}

		public void EndInplaceEdit(bool accept)
		{
			fcParent.endInplaceEdit(accept);
		}

		internal override bool canModify(int handle)
		{
			return ((long)enabledHandles & (1 << handle)) != 0;
		}

		#region undo / redo

		internal override ItemState createState()
		{
			return new BoxState(this);
		}

		internal override void saveState(ItemState state)
		{
			base.saveState(state);

			BoxState bstate = (BoxState)state;
			bstate.rotationAngle = rotationAngle;
		}

		internal override void restoreState(ItemState state)
		{
			base.restoreState(state);

			BoxState bstate = (BoxState)state;
			setRotation(bstate.rotationAngle);
		}

		internal override ItemProperties createProperties()
		{
			return new BoxProperties();
		}

		internal override void saveProperties(ItemProperties props)
		{
			base.saveProperties(props);

			BoxProperties bprops = (BoxProperties)props;

			bprops.customDraw = customDraw;
			bprops.image = image;
			bprops.picturePos = picturePos;
			bprops.selStyle = selStyle;
			bprops.shapeRotation = shapeRotation;
			bprops.shapeTemplate = shapeTemplate;
			bprops.style = style;
			bprops.text = text;
			bprops.textColor = textColor;
			bprops.textFormat = textFormat;
			bprops.transparent = transparent;
			bprops.usePolyTextLt = useTextLayout;
			bprops.useStyledText = useStyledText;
		}

		internal override void restoreProperties(ItemProperties props)
		{
			BoxProperties bprops = (BoxProperties)props;

			customDraw = bprops.customDraw;
			image = bprops.image;
			picturePos = bprops.picturePos;
			selStyle = bprops.selStyle;
			text = bprops.text;
			textColor = bprops.textColor;
			textFormat = bprops.textFormat;
			transparent = bprops.transparent;

			Style = bprops.style;
			ShapeOrientation = bprops.shapeRotation;
			Shape = bprops.shapeTemplate;
			PolyTextLayout = bprops.usePolyTextLt;
			EnableStyledText = bprops.useStyledText;

			base.restoreProperties(props);
		}

		#endregion

		internal PointList getPolygon()
		{
			if (style == BoxStyle.Shape && shapeTemplate != null)
			{
				GraphicsPath path = shapeTemplate.getPath(shapeData, rotation());
				path.Flatten();
				PointList shapePoints = new PointList(path.PathPoints);
				path.Dispose();
				if (shapePoints[0] == shapePoints[shapePoints.Count-1])
					shapePoints.RemoveAt(shapePoints.Count-1);
				return shapePoints;
			}

			return null;
		}

		internal GraphicsPath getPath()
		{
			return this.Shape.getPath(shapeData, rotation());
		}

		internal override PointCollection getOutlinePoly()
		{
			RectangleF br = BoundingRect;

			PointCollection points = new PointCollection(0);
			GraphicsPath gp = null;

			switch (style)
			{
				case BoxStyle.Rectangle:
				case BoxStyle.RoundedRectangle:
					points.Add(new PointF(br.Left, br.Top));
					points.Add(new PointF(br.Right, br.Top));
					points.Add(new PointF(br.Right, br.Bottom));
					points.Add(new PointF(br.Left, br.Bottom));
					break;
				case BoxStyle.Shape:
					gp = Shape.getPath(shapeData, rotation());
					gp.Flatten();
					points.AddRange(gp.PathPoints);
					gp.Dispose();
					break;
				case BoxStyle.Ellipse:
					points = Utilities.approxEllipse(br, 40);
					break;
				case BoxStyle.Rhombus:
					// calculate the vertex coordinates
					points.Add(new PointF((br.Left + br.Right) / 2, br.Top));
					points.Add(new PointF(br.Right, (br.Top + br.Bottom) / 2));
					points.Add(new PointF((br.Left + br.Right) / 2, br.Bottom));
					points.Add(new PointF(br.Left, (br.Top + br.Bottom) / 2));
					break;
				case BoxStyle.Delay:
					gp = Utilities.getDelaySymbol(br);
					gp.Flatten();
					points.AddRange(gp.PathPoints);
					gp.Dispose();
					break;
			}

			return points;
		}

		/// <summary>
		/// The rotation angle of the box.
		/// </summary>
		private float rotationAngle;

		/// <summary>
		/// Specifies whether to rotate the contents, such
		/// as text and image, along with the box.
		/// </summary>
		private bool rotateContents;
	}
}
