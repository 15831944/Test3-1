// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.Collections.Specialized;
using System.IO;
using System.Drawing;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization.Formatters;
using System.Reflection;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	public class PersistContext
	{
		int saveFileFormat = 28;

		internal PersistContext(Stream stream, IPersistObjFactory factory)
		{
			objIds = new HybridDictionary();
			objRefs = new HybridDictionary();
			idObjs = new HybridDictionary();
			references = new HybridDictionary();
			this.stream = stream;
			this.factory = factory;
			fmt = new BinaryFormatter();
			fmt.Binder = new DeserializationHack();

			nextId = 1;
		}

		BinaryFormatter fmt;

		internal void writeHeader()
		{
			writer = new BinaryWriter(stream);

			// write header
			string magic = "FcX!";
			writer.Write(magic);
			writer.Write(saveFileFormat);
		}

		internal void readHeader()
		{
			reader = new BinaryReader(stream);

			if (reader.PeekChar() != 4)
				throw new System.IO.FileLoadException(
					"Unsupported file format", "", null);

			string magic = (string)reader.ReadString();
			if (magic != "FcX!")
			{
				// this is not our magic
				throw new System.IO.FileLoadException(
					"Unsupported file format", "", null);
			}

			fileVersion = (int)reader.ReadInt32();
			if (fileVersion > saveFileFormat)
			{
				// the file version is newer than the one supported by the current FC.NET version
				throw new System.IO.FileLoadException(
					"Unsupported file version", "", null);
			}
		}

		int nextId;
		int fileVersion = 0;
		internal int FileVersion { get { return fileVersion; } }

		private IDictionary objIds;
		private IDictionary objRefs;
		private IDictionary idObjs;

		private IDictionary references;
		class RefInfo
		{
			public RefInfo(int objId, int refId)
			{
				this.objId = objId;
				this.refId = refId;
			}
			public int refId;
			public int objId;
		}

		void setObjRef(IPersists referer, int objId, int refId)
		{
			if (!references.Contains(referer))
				references[referer] = new ArrayList();

			RefInfo ri = new RefInfo(objId, refId);
			ArrayList al = (ArrayList)references[referer];
			al.Add(ri);
		}

		private Stream stream;
		private IPersistObjFactory factory;

		internal BinaryWriter writer;
		internal BinaryReader reader;

		internal void saveObject(IPersists po)
		{
			if (po == null)
			{
				writer.Write((int)0);
				writer.Write((int)0);
				return;
			}

			writer.Write(po.getClassId());
			writer.Write(idForObject(po));
			po.saveTo(writer, this);

			objRefs[po] = true;
		}

		internal IPersists loadObject()
		{
			int clsId = reader.ReadInt32();
			int objId = reader.ReadInt32();

			if (clsId == 0 && objId == 0)
				return null;

			IPersists obj = factory.createObj(clsId);
			obj.loadFrom(reader, this);
			idObjs[objId] = obj;
			return obj;
		}

		private int idForObject(IPersists po)
		{
			if (po == null) return 0;

			if (objIds.Contains(po))
			{
				return (int)objIds[po];
			}
			else
			{
				int id = nextId++;
				objIds.Add(po, id);
				return id;
			}
		}

		internal void saveReference(IPersists referer, IPersists pobj, int refId)
		{
			if (pobj == null)
			{
				writer.Write((int)0);
				writer.Write(refId);
				return;
			}

			if (!objRefs.Contains(pobj))
			{
				objRefs.Add(pobj, false);
			}
			int refererId = idForObject(referer);
			int objId = idForObject(pobj);
			writer.Write(objId);
			writer.Write(refId);
		}

		internal void loadReference(IPersists referer)
		{
			int objId = reader.ReadInt32();
			int refId = reader.ReadInt32();
			setObjRef(referer, objId, refId);
		}

		internal void setReferences()
		{
			foreach (IPersists obj in references.Keys)
			{
				ArrayList arr = (ArrayList)references[obj];
				foreach (RefInfo inf in arr)
				{
					if (inf.objId == 0)
						obj.setReference(inf.refId, null);
					else
						obj.setReference(inf.refId, (IPersists)idObjs[inf.objId]);
				}
			}
		}

		internal void saveFont(Font font)
		{
			if (font == null)
			{
				writer.Write((int)0);
				return;
			}
			writer.Write((int)1);

			writer.Write(font.GdiCharSet);
			writer.Write(font.GdiVerticalFont);
			writer.Write(font.Name);
			writer.Write((double)font.Size);
			writer.Write((double)font.SizeInPoints);
			writer.Write((int)font.Style);
			writer.Write((int)font.Unit);
		}

		internal Font loadFont()
		{
			int nullMrk = reader.ReadInt32();
			if (nullMrk == 0)
				return null;

			byte cset = reader.ReadByte();
			bool vtc = reader.ReadBoolean();
			string name = reader.ReadString();
			float size = (float)reader.ReadDouble();
			float sizeInPts = (float)reader.ReadDouble();
			FontStyle style = (FontStyle)reader.ReadInt32();
			GraphicsUnit unit = (GraphicsUnit)reader.ReadInt32();

			return new Font(name, size, style, unit, cset, vtc);
		}

		internal void saveImage(Image image)
		{
			if (image == null)
			{
				writer.Write((int)0);
			}
			else
			{
				byte[] imageBytes = null;

				try
				{
					MemoryStream ms = new MemoryStream();
					image.Save(ms, image.RawFormat);
					imageBytes = ms.ToArray();
					ms.Close();
				}
				catch (Exception)
				{
					imageBytes = null;
				}

				if (imageBytes == null)
				{
					try
					{
						MemoryStream ms = new MemoryStream();
						image.Save(ms, System.Drawing.Imaging.ImageFormat.Png);
						imageBytes = ms.ToArray();
						ms.Close();
					}
					catch (Exception)
					{
						imageBytes = null;
					}
				}

				if (imageBytes == null)
				{
					writer.Write((int)0);
				}
				else
				{
					writer.Write((int)1);
					writer.Write(imageBytes.Length);
					writer.Write(imageBytes);
				}
			}
		}

		internal Image loadImage()
		{
			int ima = reader.ReadInt32();
			if (ima == 1)
			{
				int len = reader.ReadInt32();
				byte[] bts = reader.ReadBytes(len);

				MemoryStream ms = new MemoryStream(bts);
				Image image = Image.FromStream(ms);
				// dont close the stream because the image keeps a reference to it

				return image;
			}

			return null;
		}

		internal void saveColor(Color clr)
		{
			writer.Write(clr.A);
			writer.Write(clr.R);
			writer.Write(clr.G);
			writer.Write(clr.B);
		}

		internal Color loadColor()
		{
			byte a = reader.ReadByte();
			byte r = reader.ReadByte();
			byte g = reader.ReadByte();
			byte b = reader.ReadByte();

			return Color.FromArgb(a, r, g, b);
		}

		internal void writeReferencedObjects()
		{
			ArrayList toSave = new ArrayList();
			bool more;

			// While saving an item, some new items might appear in objRefs,
			// so we cannot use foreach to save all referenced items.
			// Just loop here until no unsaved items remain in objRefs.
			do
			{
				more = false;

				// find the items to be saved at this step
				toSave.Clear();
				foreach (IPersists po in objRefs.Keys)
				{
					bool alreadySaved = (bool)objRefs[po];
					if (!alreadySaved)
					{
						toSave.Add(po);
						more = true;
					}
				}

				// save the found items
				foreach (IPersists item in toSave)
					saveObject(item);
			}
			while (more);

			writer.Write((int)-1);
		}

		internal void loadRemainingObjects()
		{
			IPersists obj;

			while (true)
			{
				int clsId = reader.ReadInt32();
				if (clsId == -1) return;

				int objId = reader.ReadInt32();

				obj = null;
				if (!(clsId == 0 && objId == 0))
				{
					obj = factory.createObj(clsId);
					obj.loadFrom(reader, this);
				}
				idObjs[objId] = obj;
			}
		}

		internal void saveStringFormat(StringFormat sf)
		{
			if (sf == null)
			{
				writer.Write((int)0);
				return;
			}
			writer.Write((int)1);

			StringAlignment al;
			int dsl;
			StringDigitSubstitute dsm;
			StringFormatFlags sff;
			System.Drawing.Text.HotkeyPrefix hp;
			StringAlignment lal;
			StringTrimming tr;
			float fto;
			float[] ts;

			al = sf.Alignment;
			dsl = sf.DigitSubstitutionLanguage;
			dsm = sf.DigitSubstitutionMethod;
			sff = sf.FormatFlags;
			hp = sf.HotkeyPrefix;
			lal = sf.LineAlignment;
			tr = sf.Trimming;
			ts = sf.GetTabStops(out fto);

			writer.Write((int)al);
			writer.Write(dsl);
			writer.Write((int)dsm);
			writer.Write((int)sff);
			writer.Write((int)hp);
			writer.Write((int)lal);
			writer.Write((int)tr);
			writer.Write((double)fto);
			writer.Write(ts.GetLength(0));
			foreach (float f in ts)
				writer.Write((double)f);
		}

		internal StringFormat loadStringFormat()
		{
			int nullMrk = reader.ReadInt32();
			if (nullMrk == 0)
				return null;

			StringAlignment al;
			int dsl;
			StringDigitSubstitute dsm;
			StringFormatFlags sff;
			System.Drawing.Text.HotkeyPrefix hp;
			StringAlignment lal;
			StringTrimming tr;
			float fto;
			float[] ts;

			al = (StringAlignment)reader.ReadInt32();
			dsl = reader.ReadInt32();
			dsm = (StringDigitSubstitute)reader.ReadInt32();
			sff = (StringFormatFlags)reader.ReadInt32();
			hp = (System.Drawing.Text.HotkeyPrefix)reader.ReadInt32();
			lal = (StringAlignment)reader.ReadInt32();
			tr = (StringTrimming)reader.ReadInt32();
			fto = (float)reader.ReadDouble();
			int len = reader.ReadInt32();
			ts = new float[len];
			for (int i = 0; i < len; ++i)
				ts[i] = (float)reader.ReadDouble();

			StringFormat res = new StringFormat(sff);
			res.Alignment = al;
			res.LineAlignment = lal;
			res.SetDigitSubstitution(dsl, dsm);
			res.HotkeyPrefix = hp;
			res.Trimming = tr;
			res.SetTabStops(fto, ts);
			return res;
		}

		internal void saveRect(RectangleF rect)
		{
			writer.Write((double)rect.X);
			writer.Write((double)rect.Y);
			writer.Write((double)rect.Width);
			writer.Write((double)rect.Height);
		}

		internal RectangleF loadRectF()
		{
			float x, y, w, h;
			x = (float)reader.ReadDouble();
			y = (float)reader.ReadDouble();
			w = (float)reader.ReadDouble();
			h = (float)reader.ReadDouble();
			return new RectangleF(x, y, w, h);
		}

		internal Rectangle loadRect()
		{
			int x, y, w, h;
			x = reader.ReadInt32();
			y = reader.ReadInt32();
			w = reader.ReadInt32();
			h = reader.ReadInt32();
			return new Rectangle(x, y, w, h);
		}

		internal void saveRect(Rectangle rect)
		{
			writer.Write(rect.X);
			writer.Write(rect.Y);
			writer.Write(rect.Width);
			writer.Write(rect.Height);
		}

		internal void savePtfArray(PointF[] pts)
		{
			if (pts == null)
			{
				writer.Write((int)0);
				return;
			}
			writer.Write((int)1);

			writer.Write(pts.GetLength(0));
			for (int i = 0; i < pts.GetLength(0); ++i)
			{
				writer.Write((double)pts[i].X);
				writer.Write((double)pts[i].Y);
			}
		}

		internal PointF[] loadPtfArray()
		{
			int nullMrk = reader.ReadInt32();
			if (nullMrk == 0)
				return null;

			int count = reader.ReadInt32();
			PointF[] res = new PointF[count];
			for (int i = 0; i < count; ++i)
			{
				float x, y;
				x = (float)reader.ReadDouble();
				y = (float)reader.ReadDouble();
				res[i] = new PointF(x, y);
			}

			return res;
		}

		internal void saveFloatArray(float[] arr)
		{
			if (arr == null)
			{
				writer.Write((int)0);
				return;
			}
			writer.Write((int)1);

			writer.Write(arr.GetLength(0));
			for (int i = 0; i < arr.GetLength(0); ++i)
				writer.Write((double)arr[i]);
		}

		internal float[] loadFloatArray()
		{
			int nullMrk = reader.ReadInt32();
			if (nullMrk == 0)
				return null;

			int count = reader.ReadInt32();
			float[] res = new float[count];
			for (int i = 0; i < count; ++i)
				res[i] = (float)reader.ReadDouble();

			return res;
		}

		internal void saveColorArray(Color[] arr)
		{
			if (arr == null)
			{
				writer.Write((int)0);
				return;
			}
			writer.Write((int)1);

			writer.Write(arr.GetLength(0));
			for (int i = 0; i < arr.GetLength(0); ++i)
				saveColor(arr[i]);
		}

		internal Color[] loadColorArray()
		{
			int nullMrk = reader.ReadInt32();
			if (nullMrk == 0)
				return null;

			int count = reader.ReadInt32();
			Color[] res = new Color[count];
			for (int i = 0; i < count; ++i)
				res[i] = loadColor();

			return res;
		}

		internal void saveTag(object obj)
		{
			// save tags using BinaryWriter when possible, because
			// that runs fine under the default security settings
			// of the Internet zone (whereas formatter.Serialize does not)

			byte typeId = 0;

			if (obj is String)
			{
				typeId = 1;
				string strVal = (string)obj;
				writer.Write(typeId);
				writer.Write(strVal);
				return;
			}

			if (obj is Boolean)
			{
				typeId = 2;
				bool boolVal = (bool)obj;
				writer.Write(typeId);
				writer.Write(boolVal);
				return;
			}

			if (obj is Byte)
			{
				typeId = 3;
				Byte byteVal = (Byte)obj;
				writer.Write(typeId);
				writer.Write(byteVal);
				return;
			}

			if (obj is Char)
			{
				typeId = 4;
				Char charVal = (Char)obj;
				writer.Write(typeId);
				writer.Write(charVal);
				return;
			}

			if (obj is Decimal)
			{
				typeId = 5;
				Decimal decVal = (Decimal)obj;
				writer.Write(typeId);
				writer.Write(decVal);
				return;
			}

			if (obj is Double)
			{
				typeId = 6;
				Double dbVal = (Double)obj;
				writer.Write(typeId);
				writer.Write(dbVal);
				return;
			}

			if (obj is Int16)
			{
				typeId = 7;
				Int16 i16Val = (Int16)obj;
				writer.Write(typeId);
				writer.Write(i16Val);
				return;
			}

			if (obj is Int32)
			{
				typeId = 8;
				Int32 i32Val = (Int32)obj;
				writer.Write(typeId);
				writer.Write(i32Val);
				return;
			}

			if (obj is Int64)
			{
				typeId = 9;
				Int64 i64Val = (Int64)obj;
				writer.Write(typeId);
				writer.Write(i64Val);
				return;
			}

			if (obj is SByte)
			{
				typeId = 10;
				SByte sbVal = (SByte)obj;
				writer.Write(typeId);
				writer.Write(sbVal);
				return;
			}

			if (obj is Single)
			{
				typeId = 11;
				Single floatVal = (Single)obj;
				writer.Write(typeId);
				writer.Write(floatVal);
				return;
			}

			if (obj is UInt16)
			{
				typeId = 12;
				UInt16 ui16Val = (UInt16)obj;
				writer.Write(typeId);
				writer.Write(ui16Val);
				return;
			}

			if (obj is UInt32)
			{
				typeId = 13;
				UInt32 ui32Val = (UInt32)obj;
				writer.Write(typeId);
				writer.Write(ui32Val);
				return;
			}

			if (obj is UInt64)
			{
				typeId = 14;
				UInt64 ui64Val = (UInt64)obj;
				writer.Write(typeId);
				writer.Write(ui64Val);
				return;
			}

			writer.Write(typeId);
			fmt.Serialize(stream, obj);
		}

		internal object loadTag()
		{
			if (fileVersion > 23)
			{
				byte typeId = reader.ReadByte();

				if (typeId == 1)
					return reader.ReadString();

				if (typeId == 2)
					return reader.ReadBoolean();

				if (typeId == 3)
					return reader.ReadByte();

				if (typeId == 4)
					return reader.ReadChar();

				if (typeId == 5)
					return reader.ReadDecimal();

				if (typeId == 6)
					return reader.ReadDouble();

				if (typeId == 7)
					return reader.ReadInt16();

				if (typeId == 8)
					return reader.ReadInt32();

				if (typeId == 9)
					return reader.ReadInt64();

				if (typeId == 10)
					return reader.ReadSByte();

				if (typeId == 11)
					return reader.ReadSingle();

				if (typeId == 12)
					return reader.ReadUInt16();

				if (typeId == 13)
					return reader.ReadUInt32();

				if (typeId == 14)
					return reader.ReadUInt64();
			}

			return fmt.Deserialize(stream);
		}

		internal void saveAnchorPattern(AnchorPattern ap)
		{
			if (ap == null)
			{
				writer.Write(Constants.NoAnchorPattern);
			}
			else
			{
				int regIndex = ap.getRegIndex();
				writer.Write(regIndex);
				if (regIndex == -1)
					ap.saveTo(writer, this);
			}
		}

		internal AnchorPattern loadAnchorPattern()
		{
			AnchorPattern anchorPattern = null;

			int regIndex = reader.ReadInt32();
			if (regIndex == Constants.NoAnchorPattern)
			{
				anchorPattern = null;
			}
			else
			{
				if (regIndex != -1)
					anchorPattern = AnchorPattern.getRegisteredPattern(regIndex);
				else
					(anchorPattern = new AnchorPattern()).loadFrom(reader, this);
			}

			return anchorPattern;
		}

	}

	public interface IPersists
	{
		int getClassId();

		void saveTo(BinaryWriter writer, PersistContext ctx);
		void loadFrom(BinaryReader reader, PersistContext ctx);

		void setReference(int refId, IPersists obj);
	}

	public interface IPersistObjFactory
	{
		IPersists createObj(int clsId);
	}

	// .NET 1.1 framework cannot load .NET 1.0 serialized classes, and vice versa;
	// which is kind of stupid for classes such as Size and Rectangle;
	// so we just try to find any needed type in loaded assemblies here
	internal sealed class DeserializationHack : SerializationBinder 
	{
		public override Type BindToType(string assemblyName, string typeName) 
		{
			// first try returning a type from loaded assembly of
			// the same version, that should be the quickest
			Assembly asm = Utilities.getLoadedAssembly(assemblyName);
			if (asm != null)
				return asm.GetType(typeName, false);

			try
			{
				// or try to load the necessary assembly version
				asm = Assembly.Load(assemblyName);

				// it seems those guys never return null, but throw exceptions
				if (asm != null)
					return asm.GetType(typeName, false);
			}
			catch (Exception)
			{
				// probably the needed version of the assembly is not
				// installed on the local system; just continue
			}

			// or try with another loaded version of the assembly
			return Utilities.getLoadedType(typeName,
				assemblyName.Split(new char[] { ',' })[0]);
		}
	}

}
