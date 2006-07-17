
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;


namespace MindFusion.Drawing
{
#if _EFFECTS
	/// <summary>
	/// Summary description for Effects.
	/// </summary>
	internal sealed class Effects
	{
		/// <summary>
		/// Prevent instantiation.
		/// </summary>
		private Effects()
		{
		}

		/// <summary>
		/// Generates a shadow for the specified shape.
		/// </summary>
		/// <param name="path">
		/// A GraphicsPath object, which defines the shape.
		/// </param>
		/// <param name="dispersion">
		/// The dispersion of the shadow.
		/// </param>
		/// <param name="color">
		/// The color of the shadow.
		/// </param>
		/// <returns>
		/// A bitmap, containing the generated shadow.
		/// </returns>
		public static Bitmap GenerateShadow(GraphicsPath path,
			int dispersion, Color color)
		{
			Matrix matrix = new Matrix();

			Brush brb = new SolidBrush(Color.FromArgb(0, color));
			Brush brf = new SolidBrush(color);

			RectangleF bounds = path.GetBounds();
			int w = (int)Math.Ceiling(bounds.Width) + 4 * dispersion;
			int h = (int)Math.Ceiling(bounds.Height) + 4 * dispersion;

			Rectangle rc = new Rectangle(0, 0, w, h);
			Bitmap bmpi = new Bitmap(w, h, PixelFormat.Format32bppArgb);
			Bitmap bmpo = new Bitmap(w, h, PixelFormat.Format32bppArgb);
			BitmapData bdi;
			BitmapData bdo;
			Graphics gr = Graphics.FromImage(bmpi);
			GraphicsPath gp = path.Clone() as GraphicsPath;

			matrix.Translate(2 * dispersion - bounds.X, 2 * dispersion - bounds.Y);
			gp.Transform(matrix);

			GraphicsContainer gt = gr.BeginContainer();
			gr.FillRectangle(brb, 0, 0, w, h);
			gr.SetClip(gp, CombineMode.Replace);
			gr.FillRectangle(brf, 0, 0, w, h);
			gr.EndContainer(gt);

			bdi = bmpi.LockBits(rc, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
			bdo = bmpo.LockBits(rc, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

			int d = dispersion;
			int lbi = bdi.Stride / 4;
			int lbo = bdo.Stride / 4;

			unsafe
			{
				uint* pxi = (uint*)bdi.Scan0;
				uint* pxo = (uint*)bdo.Scan0;

				uint c = (uint)(((2 * d + 1) * (2 * d + 1)));

				for (int i = d; i < w - d; i++)
				{
					for (int j = d; j < h - d; j++)
					{
						uint a = 0;
						uint r = 0;
						uint g = 0;
						uint b = 0;
						for (int ii = -d; ii <= d; ii++)
						{
							for (int jj = -d; jj <= d; jj++)
							{
								a += (byte)(pxi[(j + jj)* lbi + i + ii] >> 24);
								r += (byte)(pxi[(j + jj)* lbi + i + ii] >> 16);
								g += (byte)(pxi[(j + jj)* lbi + i + ii] >>  8);
								b += (byte)(pxi[(j + jj)* lbi + i + ii] >>  0);
							}
						}

						a /= c;
						r /= c;
						g /= c;
						b /= c;
						pxo[j * lbo + i] = (uint)((a << 24) | (r << 16) | (g << 8) | b);
					}
				}
			}

			bmpi.UnlockBits(bdi);
			bmpo.UnlockBits(bdo);

			bmpi.Dispose();

			brb.Dispose();
			brf.Dispose();
			gp.Dispose();
			gr.Dispose();

			return bmpo;
		}

		/// <summary>
		/// Generates an emboss mask for the specified shape.
		/// </summary>
		/// <param name="path">
		/// A GraphicsPath object which defines the shape.
		/// </param>
		/// <returns>
		/// A bitmap, containing the generated emboss mask.
		/// </returns>
		public static Bitmap GenerateEmboss(GraphicsPath path)
		{
			int off = 2;
			int div = 6;
			int[][] mx = 
			{
				new int[] {  1,  1,  1,  0,  0 },
				new int[] {  1,  1,  1,  0,  0 },
				new int[] {  1,  1,  5, -1, -1 },
				new int[] {  0,  0, -1, -1, -1 },
				new int[] {  0,  0, -1, -1, -1 }
			};
//			// The emboss mask
//			int off = 4;
//			int div = 16;
//			int[][] mx = 
//			{
//				new int[] {  1,  1,  1,  1,  1,  0,  0,  0,  0 },
//				new int[] {  1,  1,  1,  1,  1,  0,  0,  0,  0 },
//				new int[] {  1,  1,  1,  1,  1,  0,  0,  0,  0 },
//				new int[] {  1,  1,  1,  1,  1,  0,  0,  0,  0 },
//				new int[] {  1,  1,  1,  1, 16, -1, -1, -1, -1 },
//				new int[] {  0,  0,  0,  0, -1, -1, -1, -1, -1 },
//				new int[] {  0,  0,  0,  0, -1, -1, -1, -1, -1 },
//				new int[] {  0,  0,  0,  0, -1, -1, -1, -1, -1 },
//				new int[] {  0,  0,  0,  0, -1, -1, -1, -1, -1 }
//			};

			Matrix matrix = new Matrix();

			Brush brb = new SolidBrush(Color.FromArgb(255, 255, 255));
			Brush brf = new SolidBrush(Color.FromArgb(127, 127, 127));

			RectangleF bounds = path.GetBounds();
			int w = (int)Math.Ceiling(bounds.Width) + 2 * off;
			int h = (int)Math.Ceiling(bounds.Height) + 2 * off;

			Rectangle rc = new Rectangle(0, 0, w, h);
			Bitmap bmpi = new Bitmap(w, h, PixelFormat.Format32bppArgb);
			Bitmap bmpo = new Bitmap(w, h, PixelFormat.Format32bppArgb);
			BitmapData bdi;
			BitmapData bdo;
			Graphics g = Graphics.FromImage(bmpi);
			GraphicsPath gp = path.Clone() as GraphicsPath;

			// Rotation
			matrix.Translate(off - bounds.X, off - bounds.Y);
			gp.Transform(matrix);

			GraphicsContainer gt = g.BeginContainer();
			g.FillRectangle(brb, 0, 0, w, h);
			g.SetClip(gp, CombineMode.Replace);
			g.FillRectangle(brf, 0, 0, w, h);
			g.EndContainer(gt);

			// Perform the emboss
			bdi = bmpi.LockBits(rc, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
			bdo = bmpo.LockBits(rc, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

			int lbi = bdi.Stride / 4;
			int lbo = bdo.Stride / 4;

			unsafe
			{
				uint* pxi = (uint*)bdi.Scan0;
				uint* pxo = (uint*)bdo.Scan0;
				uint alpha = 0xff;

				for (int i = off; i < w - off; i++)
				{
					for (int j = off; j < h - off; j++)
					{
						int v = 0;
						for (int ii = -off; ii <= off; ii++)
						{
							for (int jj = -off; jj <= off; jj++)
								v += (int)(byte)pxi[(j + jj)* lbi + i + ii] * mx[ii + off][jj + off];
						}

						v = v / div;

						if (v < 0)
							v = 0;
						if (v > 255)
							v = 255;

						alpha = (byte)Math.Abs(v - 127);
						alpha = alpha << 24;
//						alpha = 0xff000000;

						pxo[j * lbo + i] = (alpha | ((uint)v << 16) | ((uint)v << 8) | ((uint)v << 0));
					}
				}
			}

			bmpi.UnlockBits(bdi);
			bmpo.UnlockBits(bdo);
			bmpi.Dispose();

			brb.Dispose();
			brf.Dispose();
			gp.Dispose();
			g.Dispose();

			return bmpo;
		}
	}
#endif
}
