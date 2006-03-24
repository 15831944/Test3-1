// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Collections;
using System.Text.RegularExpressions;

namespace MindFusion.Diagramming.Export
{
	/// <summary>
	/// Defines styles that can be applied to styled text.
	/// </summary>
	[FlagsAttribute]
	internal enum Styles
	{
		Regular = 0,
		Bold = 1,
		Italic = 2,
		Underline = 4,
		Sup = 8,
		Sub = 16,
		Color = 32
	}


	/// <summary>
	/// Represents succession of either printable characters,
	/// whitespaces or linebreaks.
	/// </summary>
	internal class Word
	{
		public Word(string val)
		{
			_value = val;
			_width = 0;

			_sformat.Alignment = StringAlignment.Near;
		}

		/// <summary>
		/// Updates the width of the word as it would be
		/// when drawn with the specified font in the specified graphics.
		/// </summary>
		public virtual void UpdateMeasures(Graphics graphics, Font font)
		{
			string word = _value;
			int rangePos = 0;
			int rangeLen = word.Length;

			if (IsWhitespace)
			{
				// Enclose the word in printable characters for
				// Graphics.MeasureText to work correctly.
				// Furthermore tabs are not measured, so replace
				// tabs with, say, 4 spaces
				word = "W" + word + "W";
				word = word.Replace("\t", "    ");
				rangePos = 1;
				rangeLen = word.Length - 2;
			}

			_sformat.SetMeasurableCharacterRanges(new CharacterRange[]
				{
					new CharacterRange(rangePos, rangeLen)
				});
			Region[] r = graphics.MeasureCharacterRanges(
				word, font, _srect, _sformat);
			RectangleF bounds = r[0].GetBounds(graphics);
			r[0].Dispose();

			_width = bounds.Width;
		}


		/// <summary>
		/// Gets a value, indicating whether the word
		/// contains only whitespace characters.
		/// </summary>
		public bool IsWhitespace
		{
			get { return _value[0] == ' ' || _value[0] == '\t'; }
		}

		/// <summary>
		/// Gets a value, indicating whether the word
		/// contains linebreaking characters.
		/// </summary>
		public bool IsLineBreak
		{
			get { return _value[0] == '\r' || _value[0] == '\n'; }
		}

		/// <summary>
		/// Gets the text representation of the word.
		/// </summary>
		public string Value
		{
			get { return _value; }
		}

		/// <summary>
		/// Gets the width of the word.
		/// </summary>
		public float Width
		{
			get { return _width; }
		}


		private static StringFormat _sformat = new StringFormat();
		private static RectangleF _srect = new RectangleF(0, 0, 10000, 10000);


		private string _value;
		private float _width;
	}

	/// <summary>
	/// List of word objects.
	/// </summary>
	internal class WordList : ReadOnlyCollectionBase
	{
		public WordList(ICollection values)
		{
			base.InnerList.AddRange(values);
		}

		public bool Contains(Word word)
		{
			return base.InnerList.Contains(word);
		}

		public Word this[int index]
		{
			get { return base.InnerList[index] as Word; }
		}
	}

	/// <summary>
	/// Defines base implementation for immutable text objects.
	/// </summary>
	internal abstract class Text
	{
		public Text()
		{
			_text = "";
			_words = new ArrayList(256);
			_readOnlyWords = new WordList(_words);
			_height = 0;
		}

		/// <summary>
		/// Assigns new text to the object. The text is formatted
		/// according to the specified font and graphics objects.
		/// </summary>
		public void Setup(string text, Graphics graphics, Font font)
		{
			_text = text;

			// Process the text
			_words.Clear();
			ProcessText();

			// Update the read-only words collection
			_readOnlyWords = new WordList(_words);

			// Recalculates the width of all words within the text,
			// using the specified font and graphics objects
			foreach (Word word in _words)
				word.UpdateMeasures(graphics, font);

			// Calculate the height of the text when
			// drawn with the specified font in the specified graphics
			_height = font.GetHeight(graphics);
		}


		/// <summary>
		/// Gets a read-only list containing the words in the text.
		/// </summary>
		public WordList Words
		{
			get { return _readOnlyWords; }
		}

		/// <summary>
		/// Gets the raw text.
		/// </summary>
		public string RawText
		{
			get { return _text; }
		}

		/// <summary>
		/// Gets the text stripped of any formatting tags.
		/// </summary>
		public virtual string PlainText
		{
			get { return _text; }
		}

		/// <summary>
		/// Gets the list containing the words of the text.
		/// </summary>
		protected ArrayList InnerWords
		{
			get { return _words; }
		}

		/// <summary>
		/// Gets the height of the text as it is
		/// specified in the last Setup operation.
		/// </summary>
		public float Height
		{
			get { return _height; }
		}


		/// <summary>
		/// Invoked after new text has been assigned to the object to
		/// allow clients to perform custom text processing.
		/// </summary>
		protected abstract void ProcessText();


		/// <summary>
		/// Subdivides the specified string to whitespaces and text.
		/// </summary>
		public static string[] Split(string fragment)
		{
			ArrayList result = new ArrayList();
			if (fragment.Length == 0)
				return (string[])result.ToArray(Type.GetType("System.String"));

			int p = 0;
			string s = "" + fragment[p];
			while (++p < fragment.Length)
			{
				char c = fragment[p];

				if (c == '\n')
				{
					if (s.Length > 0)
					{
						if (s[0] != '\r')
						{
							result.Add(s);
							result.Add(c.ToString());
						}
						else
						{
							s += c;
							result.Add(s);
						}
					}
					else
					{
						result.Add(c.ToString());
					}

					s = "";
					continue;
				}

				if (c == '\r')
				{
					if (s.Length > 0)
						result.Add(s);

					s = c.ToString();
					continue;
				}

				if (c == ' ' || c == '\t')
				{
					if (s.Length > 0)
					{
						if (s[0] == ' ' || s[0] == '\t')
						{
							s += c;
						}
						else
						{
							result.Add(s);
							s = c.ToString();
						}
					}
					else
					{
						s += c;
					}

					continue;
				}

				// c is a printable character
				if (s.Length > 0)
				{
					if (s[0] == ' ' || s[0] == '\t' ||
						s[0] == '\r' || s[0] == '\n')
					{
						result.Add(s);
						s = "";
					}
				}

				s += c;
			}

			if (s.Length > 0)
				result.Add(s);

			return (string[])result.ToArray(Type.GetType("System.String"));
		}


		private string _text = "";
		private ArrayList _words = null;
		private WordList _readOnlyWords = null;
		private float _height = 0;
	}

	/// <summary>
	/// Represents text with no formatting.
	/// </summary>
	internal class PlainText : Text
	{
		/// <summary>
		/// Builds new plain text object.
		/// </summary>
		public PlainText()
		{
		}

		/// <summary>
		/// Performs simple parsing of the text.
		/// </summary>
		protected override void ProcessText()
		{
			string[] ss = Split(RawText);

			foreach (string s in ss)
				InnerWords.Add(new Word(s));
		}
	}

	/// <summary>
	/// Represents a text which can, but not necessarily would,
	/// contain formatting tags.
	/// </summary>
	internal class StyledText : Text
	{
		/// <summary>
		/// Represents single formatting tag.
		/// </summary>
		private class FormatToken
		{
			public FormatToken(Styles format, object param)
			{
				_format = format;
				_param = param;
			}


			public Styles Format
			{
				get { return _format; }
			}

			public object Param
			{
				get { return _param; }
			}


			private Styles _format;
			private object _param;
		}


		/// <summary>
		/// Represents word with uniform formatting.
		/// </summary>
		internal class StyledWord : Word
		{
			public StyledWord(string val, Styles format, Color color) : base(val)
			{
				_format = format;
				_color = color;
			}

			/// <summary>
			/// Updates the measures of the word as it would be
			/// when drawn with the specified font in the specified graphics,
			/// making sure word style is taken into account.
			/// </summary>
			public override void UpdateMeasures(Graphics graphics, Font font)
			{
				Font styledFont = CreateFont(font);

				base.UpdateMeasures(graphics, styledFont);

				// Update y-off
				if ((_format & Styles.Sub) > 0)
				{
					float wordHeight = styledFont.GetHeight(graphics);
					float textHeight = font.GetHeight(graphics);

					_yOff = textHeight - wordHeight;
				}

				else if ((_format & Styles.Sup) > 0)
				{
					_yOff = 0; // ?!
				}

				DisposeFont(styledFont);
			}


			/// <summary>
			/// Gets the word formatting.
			/// </summary>
			public Styles Format
			{
				get { return _format; }
			}

			/// <summary>
			/// Gets the word color or empty color if no
			/// color is associated with the word.
			/// </summary>
			public Color Color
			{
				get { return _color; }
			}

			/// <summary>
			/// Gets the y-offset of this word in graphics units.
			/// In most cases this is 0, except when
			/// the text is sub- or sup- ed.
			/// </summary>
			public float YOffset
			{
				get { return _yOff; }
			}

			/// <summary>
			/// Applies the word style to the given font.
			/// </summary>
			public Font CreateFont(Font font)
			{
				// Check if new font needs to be created
				if (_format == Styles.Regular ||
					_format == Styles.Color)
					return font;

				string name = font.Name;
				float size = font.Size;
				FontStyle style = font.Style;
				GraphicsUnit unit = font.Unit;

				if ((_format & Styles.Sub) > 0 || (_format & Styles.Sup) > 0)
					size -= size / 3;
				if ((_format & Styles.Bold) > 0)
					style |= FontStyle.Bold;
				if ((_format & Styles.Italic) > 0)
					style |= FontStyle.Italic;
				if ((_format & Styles.Underline) > 0)
					style |= FontStyle.Underline;

				return new Font(name, size, style, unit);
			}

			/// <summary>
			/// Disposes the specified font.
			/// </summary>
			public void DisposeFont(Font font)
			{
				// Check if new font needs to be disposed
				if (_format == Styles.Regular ||
					_format == Styles.Color)
					return;

				font.Dispose();
			}

			/// <summary>
			/// Applies the word style to the given brush.
			/// </summary>
			/// <returns></returns>
			public System.Drawing.Brush CreateBrush(System.Drawing.Brush brush)
			{
				if ((_format & Styles.Color) == 0)
					return brush;

				return new System.Drawing.SolidBrush(_color);
			}

			/// <summary>
			/// Disposes the specified brush.
			/// </summary>
			public void DisposeBrush(System.Drawing.Brush brush)
			{
				if ((_format & Styles.Color) == 0)
					return;

				brush.Dispose();
			}


			private Styles _format;
			private Color _color;
			private float _yOff;
		}


		/// <summary>
		/// Builds new styled text object.
		/// </summary>
		public StyledText()
		{
		}

		/// <summary>
		/// Processes formatting text.
		/// </summary>
		protected override void ProcessText()
		{
			Stack formatting = new Stack();
			int pos = 0;

			// Locate formatting tags
			MatchCollection matches = _recognizer.Matches(RawText);

			foreach (Match match in matches)
			{
				if (match.Index > pos)
				{
					string[] ss = Split(
						RawText.Substring(pos, match.Index - pos));

					// Collect formatting
					Styles f = Styles.Regular;
					object color = null;
					foreach (FormatToken token in formatting)
					{
						f |= token.Format;
						if (token.Format == Styles.Color)
							color = token.Param;
					}

					foreach (string s in ss)
					{
						Word word = null;
								
						if (color != null)
							word = new StyledWord(s, f, (Color)color);
						else
							word = new StyledWord(s, f, Color.Empty);

						InnerWords.Add(word);
					}

					// Advance pos
					pos = match.Index;
				}

				if (match.Index == pos)
				{
					// Modify formatting
					Styles f = Styles.Regular;
					object param = null;
					bool closing = match.Value.IndexOf("/") != -1;
					string mval = match.Value.ToLower();

					if (mval == "<b>" || mval == "</b>")
						f = Styles.Bold;

					else if (mval == "<i>" || mval == "</i>")
						f = Styles.Italic;

					else if (mval == "<u>" || mval == "</u>")
						f = Styles.Underline;

					else if (mval == "<sub>" || mval == "</sub>")
						f = Styles.Sub;

					else if (mval == "<sup>" || mval == "</sup>")
						f = Styles.Sup;

					else if (mval.IndexOf("color") != -1)
					{
						f = Styles.Color;
						if (!closing)
						{
							int spos = mval.IndexOf("#");
							string color = mval.Substring(spos + 1, 6);
							param = Color.FromArgb(
								int.Parse(color.Substring(0, 2), System.Globalization.NumberStyles.HexNumber),
								int.Parse(color.Substring(2, 2), System.Globalization.NumberStyles.HexNumber),
								int.Parse(color.Substring(4, 2), System.Globalization.NumberStyles.HexNumber));
						}
					}

					// It it is opening tag, add it to the stack,
					// otherwise - find the last corresponding opening
					// tag and remove it from the stack.
					if (!closing)
					{
						formatting.Push(new FormatToken(f, param));
					}
					else
					{
						Stack temp = new Stack();
						while (formatting.Count > 0)
						{
							if ((formatting.Peek() as FormatToken).Format == f)
								break;

							temp.Push(formatting.Pop());
						}

						// Remove if found
						if (formatting.Count > 0)
							formatting.Pop();

						// Return the remaining tokens
						while (temp.Count > 0)
							formatting.Push(temp.Pop());
					}

					// Advance pos
					pos += match.Length;
					continue;
				}

				throw new Exception("Styled text internal error.");
			}

			if (pos < RawText.Length)
			{
				string[] ss = Split(RawText.Substring(pos));

				// Collect formatting
				Styles f = Styles.Regular;
				object color = null;
				foreach (FormatToken token in formatting)
				{
					f |= token.Format;
					if (token.Format == Styles.Color)
						color = token.Param;
				}

				foreach (string s in ss)
				{
					Word word = null;
								
					if (color != null)
						word = new StyledWord(s, f, (Color)color);
					else
						word = new StyledWord(s, f, Color.Empty);

					InnerWords.Add(word);
				}
			}

			// Update the pure text
			_plainText = "";
			foreach (Word word in InnerWords)
				_plainText += word.Value;
		}


		/// <summary>
		/// Gets the text stripped of any formatting tags.
		/// </summary>
		public override string PlainText
		{
			get { return _plainText; }
		}


		/// <summary>
		/// Regular expression recognizer object used
		/// internally in the parsing process.
		/// </summary>
		private static Regex _recognizer = new Regex(
			"<b>|</b>|<i>|</i>|<u>|</u>|<sup>|</sup>|<sub>|</sub>|" +
			"<color=#[0-9,A-F][0-9,A-F][0-9,A-F][0-9,A-F][0-9,A-F][0-9,A-F]>|</color>", 
			RegexOptions.Compiled | RegexOptions.IgnoreCase);


		private string _plainText;
	}
}
