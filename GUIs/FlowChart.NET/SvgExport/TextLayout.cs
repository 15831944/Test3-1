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
using MindFusion.Geometry.Geometry2D;
using System.Xml;

namespace MindFusion.Diagramming.Export
{
	/// <summary>
	/// Structured data used in formatted text rendering.
	/// </summary>
	internal class DrawTextHint
	{
		public DrawTextHint(Graphics graphics, Font font, 
			System.Drawing.Brush brush, StringFormat format,
			bool addEllipsis)
		{
			Graphics = graphics;
			Font = font;
			Brush = brush;
			Format = format;
			AddEllipsis = addEllipsis;
		}

		public DrawTextHint(Graphics graphics, Font font, 
			System.Drawing.Brush brush, StringFormat format,
			bool addEllipsis, XmlNode pTextNode, RectangleF pRect )
		{
			Graphics = graphics;
			Font = font;
			Brush = brush;
			Format = format;
			AddEllipsis = addEllipsis;
			m_TextNode = pTextNode;
			rect = pRect;
		}

		public Graphics Graphics;
		public Font Font;
		public System.Drawing.Brush Brush;
		public StringFormat Format;
		public bool AddEllipsis;
		public XmlNode m_TextNode = null;
		public bool IsLastLine = false;
		public RectangleF rect = RectangleF.Empty;
		public Word CurrentWord = null;
	}

	/// <summary>
	/// Contains options for text layouting.
	/// </summary>
	internal struct LayoutOptions
	{
		/// <summary>
		/// Specifies the horizontal alignment of the text.
		/// </summary>
		public StringAlignment Alignment
		{
			get { return _alignment; }
			set { _alignment = value; }
		}

		/// <summary>
		/// Specifies the vertical alignment of the text.
		/// </summary>
		public StringAlignment LineAlignment
		{
			get { return _lineAlignment; }
			set { _lineAlignment = value; }
		}

		/// <summary>
		/// Specifies the distance between adjacent text lines.
		/// </summary>
		public float LineDistance
		{
			get { return _lineDistance; }
			set { _lineDistance = value; }
		}


		private StringAlignment _alignment;
		private StringAlignment _lineAlignment;
		private float _lineDistance;
	}

	/// <summary>
	/// Layouts text in the interior of various shapes.
	/// </summary>
	internal class Layout
	{
		private class DirPt : IComparable
		{
			public DirPt(PointF point, int direction)
			{
				_point = point;
				_direction = direction;
			}


			public PointF Point
			{
				get { return _point; }
				set { _point = value; }
			}

			public int Direction
			{
				get { return _direction; }
				set { _direction = value; }
			}

			public int CompareTo(object obj)
			{
				DirPt pt = (DirPt)obj;
				if (this < pt)
					return -1;
				else if (this > pt)
					return 1;
				return 0;
			}

			public static bool operator<(DirPt pt1, DirPt pt2)
			{
				if (pt1.Point.X < pt2.Point.X)
					return true;
				else if (pt1.Point == pt2.Point)
					return pt1.Direction > pt2.Direction;
				return false;
			}

			public static bool operator>(DirPt pt1, DirPt pt2)
			{
				if (pt1.Point.X > pt2.Point.X)
					return true;
				else if (pt1.Point == pt2.Point)
					return pt1.Direction < pt2.Direction;
				return false;
			}


			private PointF _point = PointF.Empty;
			private int _direction = 0;
		}


		private class XPointComparer : IComparer
		{
			public XPointComparer() {}

			public int Compare(object x, object y)
			{
				PointF px = (PointF)x;
				PointF py = (PointF)y;

				if(px.X < py.X)
					return -1;
				else if(px.X > py.X)
					return 1;
				return 0;
			}
		}



		/// <summary>
		/// Builds a new text layouter.
		/// </summary>
		public Layout()
		{
		}

		public bool LayoutInRectangle(Text text,
			RectangleF rectangle, LayoutOptions options)
		{
			return DoLayout(text, new Polygon(rectangle), options);
		}

		public bool LayoutInEllipse(Text text,
			RectangleF bounds, LayoutOptions options)
		{
			return DoLayout(text, new Polygon(bounds, 25), options);
		}

		/// <summary>
		/// Layouts text inside polygonal area.
		/// </summary>
		public bool LayoutInPolygon(Text text,
			PointF[] polygon, LayoutOptions options)
		{
			if (polygon == null)
				return false;
			if (polygon.Length < 3)
				return false;
			if (text == null || text.RawText.Length == 0)
				return true;

			return DoLayout(text, new Polygon(new PointList(polygon)), options);
		}

		private bool DoLayout(Text text, Polygon polygon, LayoutOptions options)
		{
			// Initialize internal variables
			_text = text;
			_polygon = polygon;
			_options = options;
			_bounds = _polygon.Bounds;

			_fits = false;

			// Build the h-lines according to the layout settings
			BuildLines();

			// Find out the starting line and the total number
			// of lines needed to layout the text.
			_totalLines = 1;
			_startLine = FirstLine(_totalLines);
			if (GetHLines(_totalLines).Count == 0)
				return false;

			do
			{
				// Check if the text fits within the h-lines
				// in the range [_startLine, _startLine + _totalLines)
				int iword = 0;
				PointList hLine = null;

				for (int i = _startLine; i < _startLine + _totalLines; i++)
				{
					hLine = GetHLines(_totalLines)[i] as PointList;
					for (int j = 0; j < hLine.Count; j += 2)
					{
						PointF pt1 = hLine[j];
						PointF pt2 = hLine[j + 1];
						RectangleF rc = new RectangleF(
							pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

						bool newLine = false;
						iword = FitTextInRect(iword, rc, ref newLine);

						if (newLine)
							break;
						if (iword >= _text.Words.Count)
						{
							_fits = true;
							return true;
						}
					}
				}

				// If the text does not fit, increase the total
				// number of lines and recalculate the starting
				// line depending on v-alignment.
				_totalLines++;
				if (_totalLines > GetHLines(_totalLines).Count)
				{
					_totalLines--;
					return false;
				}
				_startLine = FirstLine(_totalLines);

				ArrayList hLines = GetHLines(_totalLines);
				if (_startLine + _totalLines > hLines.Count)
				{
					ArrayList hLines2 = GetHLines(_totalLines + 1);
					if (_totalLines > hLines2.Count)
					{
						// The text would not fit in the
						// polygon, so use all of the available
						// space to layout as much text as possible
						_totalLines = Math.Max(hLines.Count, hLines2.Count);
						_startLine = 0;
						return false;
					}
				}
			}
			while ((_startLine = FirstLine(_totalLines)) != -1);

			return false;
		}

		/// <summary>
		/// Draws a previously laid-out text at a specified offset.
		/// </summary>
		public void Draw(float xOff, float yOff,
			RenderTextCallback renderCallback, DrawTextHint hint)
		{
			
			if (GetHLines(_totalLines) == null)
				throw new Exception("Draw invoked on a non-laid-out text.");
			if (GetHLines(_totalLines).Count == 0)
				return;

			bool styled = (_text is StyledText);

			bool is_LastWord = false;
			// Original brush and font
			System.Drawing.Brush brushOriginal = hint.Brush;
			Font fontOriginal = hint.Font;

			// We now have the starting line and the number
			// of total lines to layout the text in. Go for it
			int iword = 0;
			for (int i = _startLine; i < _startLine + _totalLines; i++)
			{
			   
				PointList hLine = GetHLines(_totalLines)[i] as PointList;

				for (int j = 0; j < hLine.Count; j += 2)
				{
					PointF pt1 = hLine[j];
					PointF pt2 = hLine[j + 1];
					RectangleF rc = new RectangleF(
						_bounds.X + xOff + pt1.X,
						_bounds.Y + yOff + pt1.Y,
						pt2.X - pt1.X, pt2.Y - pt1.Y);

					bool newLine = false;
					int newword = FitTextInRect(iword, rc, ref newLine);

					if (newword > iword)
					{
						// Calculate the total width of the words
						// which are about to be rendered, but skip
						// the whitespaces at the front and the rear
						int ifront = iword;
						int irear = newword - 1;
						/*DON'T SKIP WHITESPACES ( next 2 lines )
						 * 
						 */
						
						while (ifront < newword && _text.Words[ifront].IsWhitespace)
							ifront++;
						while (irear >= ifront && _text.Words[irear].IsWhitespace)
							irear--;
						
						 

						int w;
						float total = 0;
						for (w = ifront; w <= irear; w++)
							total += _text.Words[w].Width;

						// Adjust the left side of the destination
						// rectangle according to the specified alignment
						switch (_options.Alignment)
						{

							case StringAlignment.Near:
								// Do nothing
								break;

							case StringAlignment.Center:
								rc.X = rc.X + (rc.Width - total) / 2;
								break;

							case StringAlignment.Far:
								rc.X = rc.Right - total;
								break;

						}

						// Render the words in the range [ifront, irear]
						for (w = ifront; w <= irear; w++)
						{
							Word word = _text.Words[w];

							if ( w == _text.Words.Count -1  )
								is_LastWord = true;
							else
								is_LastWord = false;

							if (!_fits && hint.AddEllipsis && styled)
							{
								if (i == _startLine + _totalLines - 1)
								{
									if (j == hLine.Count - 2)
									{
										if (w == irear)
										{
											// Append the last word with ellipsis
											StyledText.StyledWord sword =
												word as StyledText.StyledWord;
											StyledText.StyledWord newWord = null;

											int chars = sword.Value.Length;
											float width = sword.Width;
											do
											{
												newWord = new StyledText.StyledWord(
													sword.Value.Substring(0, chars) + "...", sword.Format, sword.Color);
												newWord.UpdateMeasures(hint.Graphics, hint.Font);
												chars--;
											}
											while (chars > 0 && newWord.Width > width);

											word = newWord;
										}
									}
								}
							}

							/*DON'T SKIP WHITESPACES ( next line )
						 * 
						 */
						 							
							if (!word.IsLineBreak && !word.IsWhitespace)
						
						 
							{
								if (styled)
								{
									// In case of styled text formatting,
									// apply fonts and colors
									StyledText.StyledWord sword =
										word as StyledText.StyledWord;

									hint.Font = sword.CreateFont(hint.Font);
									hint.Brush = sword.CreateBrush(hint.Brush);

									rc.Y += sword.YOffset;
								}

								// Add 10 to width and height becaus GDI+ stupid
								// and clips the text

								hint.IsLastLine = is_LastWord;
								hint.CurrentWord = word;

								renderCallback(word.Value, new RectangleF(
									rc.X, rc.Y, word.Width + 10 , rc.Height + 10), hint);
								
								if (styled)
								{
									// Restore font and brush
									StyledText.StyledWord sword = 
										word as StyledText.StyledWord;

									sword.DisposeFont(hint.Font);
									sword.DisposeBrush(hint.Brush);
									hint.Font = fontOriginal;
									hint.Brush = brushOriginal;

									rc.Y -= sword.YOffset;
								}
							}
							rc.X += _text.Words[w].Width;
						}

						iword = newword;
					}

					if (newLine)
						break;
				}
			}


		}

		/// <summary>
		/// Returns the appropriate h-lines collection
		/// depending on the line alignment and the total
		/// number of lines.
		/// </summary>
		private ArrayList GetHLines(int total)
		{
			switch (_options.LineAlignment)
			{

				case StringAlignment.Near:
					return _ttbLines;

				case StringAlignment.Center:
				{
					if(total % 2 > 0) // odd
						return _oddLines;
					else
						return _evenLines;
				}

				case StringAlignment.Far:
					return _bttLines;

			}

			return _ttbLines;
		}

		/// <summary>
		/// Returns the index of the first h-line depending
		/// on the line alignment and the total number of lines.
		/// </summary>
		private int FirstLine(int total)
		{
			switch (_options.LineAlignment)
			{

				case StringAlignment.Near:
					return 0;

				case StringAlignment.Center:
				{
					if (total % 2 > 0) // odd
						return _oddLines.Count / 2 - total / 2;
					else
						return (_evenLines.Count - 1) / 2 - (total - 1) / 2;
				}

				case StringAlignment.Far:
					return _bttLines.Count - total;

			}

			return -1;
		}

		/// <summary>
		/// Tries to fits as many words from the source text
		/// as possible in the specified rectangle, starting
		/// from the word with the specified index.
		/// </summary>
		private int FitTextInRect(int iword, RectangleF rc, ref bool newLine)
		{
			newLine = false;

			if (iword >= _text.Words.Count)
				return iword;

			Word word;
			float total = 0; // total width
			int fromWord = iword;

			while (total < rc.Width && iword < _text.Words.Count)
			{
				word = _text.Words[iword++];

				if (word.IsLineBreak)
				{
					newLine = true;
					return iword;
				}

				total += word.Width;
				if (total > rc.Width)
					return iword - 1;
			}

			// All text is consumed
			return iword;
		}

		/// <summary>
		/// Creates single h-line at the specified vertical offset.
		/// </summary>
		private PointList BuildLine(float yOff)
		{
			PointF p11 = new PointF(_bounds.Left - 1, 0);
			PointF p12 = new PointF(_bounds.Right + 1, 0);
			PointF p21 = new PointF(_bounds.Left - 1, 0);
			PointF p22 = new PointF(_bounds.Right + 1, 0);

			p11.Y = p12.Y = yOff;
			yOff += _text.Height;
			if (yOff > _bounds.Bottom)
				return null;
			p21.Y = p22.Y = yOff;

			ArrayList merge = new ArrayList();
			PointList result = new PointList();
			PointList row1 = PolygonIntersect(p11, p12);
			PointList row2 = PolygonIntersect(p21, p22);

			int i;
			for (i = 0; i < row1.Count; i++)
				merge.Add(new DirPt(row1[i], i % 2 == 1 ? 2 : 0));
			for (i = 0; i < row2.Count; i++)
				merge.Add(new DirPt(row2[i], i % 2 == 1 ? 3 : 1));

			merge.Sort();

			PointF pt = PointF.Empty;
			int inter = -1; // 0 - for first line, 2 for second line, 4 - for both
			for (i = 0; i < merge.Count; i++)
			{
				DirPt temp = merge[i] as DirPt;

				if (temp.Direction == 2 || temp.Direction == 3) // out
				{
					if (inter != 4)
					{
						if (inter == 0 && temp.Direction == 2)
							inter = -1;
						else if (inter == 2 && temp.Direction == 3)
							inter = -1;
						continue;
					}

					pt.Y = yOff - _text.Height;
					temp.Point = new PointF(temp.Point.X, yOff);

					// Make points relative to the upper-left point of the polygon
					pt.X -= _bounds.Left;
					pt.Y -= _bounds.Top;
					temp.Point = new PointF(
						temp.Point.X - _bounds.Left,
						temp.Point.Y - _bounds.Top);

					result.Add(pt);
					result.Add(temp.Point);

					if (temp.Direction == 2)
						inter = 2;
					else
						inter = 0;
				}
				else
				{
					pt = temp.Point;

					if (temp.Direction == 0)
					{
						if (inter == -1)
							inter = 0;
						else if (inter == 2)
							inter = 4;
					}
					else
					{
						if (inter == -1)
							inter = 2;
						else if (inter == 0)
							inter = 4;
					}
				}
			}

			// Check if the center point of each
			// rectangle lies within the polygon
			for (i = 0; i < result.Count; )
			{
				PointF pt1 = result[i];
				PointF pt2 = result[i + 1];
				PointF ptc = PointF.Empty;
		
				ptc.X = (pt1.X + pt2.X) / 2 + _bounds.Left;
				ptc.Y = (pt1.Y + pt2.Y) / 2 + _bounds.Top;

				if (!_polygon.Contains(ptc))
				{
					result.RemoveAt(i);
					result.RemoveAt(i);
				}
				else
				{
					i += 2;
				}
			}

			return result;
		}

		/// <summary>
		/// Finds all h-lines which could fit in the specified
		/// area depending on the current layout settings.
		/// </summary>
		private void BuildLines()
		{
			_evenLines = new ArrayList();
			_oddLines = new ArrayList();
			_ttbLines = new ArrayList();
			_bttLines = new ArrayList();

			int i;
			float[] y = new float[2] { 0, 0 };
			ArrayList[] ptc = new ArrayList[2] { null, null };

			switch (_options.LineAlignment)
			{

				case StringAlignment.Near:
					y[0] = _bounds.Top + _options.LineDistance + 0.0001f;
					ptc[0] = _ttbLines;
					break;

				case StringAlignment.Center:
					y[0] = (_bounds.Top + _bounds.Bottom) / 2 - _text.Height / 2;
					ptc[0] = _oddLines;
					y[1] = (_bounds.Top + _bounds.Bottom) / 2 + _options.LineDistance / 2;
					ptc[1] = _evenLines;
					break;

				case StringAlignment.Far:
					y[0] = _bounds.Bottom - _options.LineDistance - 0.0001f;
					ptc[0] = _bttLines;
					break;

			}

			for (i = 0; i < 2; i++)
			{
				while (y[i] > _bounds.Top)
					y[i] -= (_options.LineDistance + _text.Height);

				y[i] += (_options.LineDistance + _text.Height);
			}

			// Build the lines
			PointList line;
			for (i = 0; i < 2; i++)
			{
				if (ptc[i] == null)
					continue;

				while (y[i] + _text.Height <= _bounds.Bottom)
				{
					line = BuildLine(y[i]);
					if (line == null)
						break;
					y[i] += _text.Height;
					y[i] += _options.LineDistance;

					ptc[i].Add(line);
				}
			}
		}

		/// <summary>
		/// Calculates all intersections between the internal
		/// polygon and the line segment defined by pt1 and pt2.
		/// </summary>
		private PointList PolygonIntersect(PointF a, PointF b)
		{
			PointList points = _polygon.IntersectSegment(a, b);

			// Sort the points by X coordinates
			points.Sort(new XPointComparer());

			if (points.Count > 1)
				if (Math.Abs(points[0].X - points[1].X) +
					Math.Abs(points[0].Y - points[1].Y) < 0.0001)
					points.RemoveAt(0);
			if (points.Count > 1)
				if (Math.Abs(points[points.Count - 1].X - points[points.Count - 2].X) +
					Math.Abs(points[points.Count - 1].Y - points[points.Count - 2].Y) < 0.0001)
					points.RemoveAt(points.Count - 1);

			if (points.Count == 1)
				points.Clear();
			if (points.Count % 2 != 0)
				points.RemoveAt(points.Count - 2);

			return points;
		}


		private Text _text = null;
		private Polygon _polygon = null;
		private RectangleF _bounds;
		private LayoutOptions _options;

		private ArrayList _evenLines = null;
		private ArrayList _oddLines = null;
		private ArrayList _ttbLines = null;
		private ArrayList _bttLines = null;

		private int _startLine = 0;
		private int _totalLines = 0;

		private bool _fits = false;
	}

	/// <summary>
	/// Delegate used by the layouting objects
	/// when the text needs to be drawn.
	/// </summary>
	internal delegate void RenderTextCallback(string text, RectangleF dest, DrawTextHint hint);
}
