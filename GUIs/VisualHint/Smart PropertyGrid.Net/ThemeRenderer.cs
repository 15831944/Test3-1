#define THEME_ALL
#define THEME_CLASS_ALL
#region Copyright (C) 2004-2006 Skybound. All Rights Reserved.
/*
* Skybound Copyright Notice
* (C) 2004-2006 All Rights Reserved
* 
* This file and its contents are protected by Canadian and
* International copyright laws.  Reproduction and/or distribution
* of all or any portion of the code contained herein, without the
* express written consent of Skybound, is strictly prohibited and
* will result in severe civil and criminal penalties.  Any violations
* of this copyright will be prosecuted to the fullest extent possible
* under law.
* 
* THE SOURCE CODE CONTAINED IN THIS FILE AND IN RELATED FILES IS
* PROVIDED TO THE LICENSED DEVELOPER OF THIS PRODUCT FOR USAGE AS
* DESCRIBED BY THE END USER LICENSE AGREEMENT (EULA) ACCOMPANYING THE
* PRODUCT.
* 
* UNDER NO CIRCUMSTANCES MAY ANY PORTION OF THE SOURCE CODE BE
* DISTRIBUTED, DISCLOSED OR OTHERWISE MADE AVAILABLE TO ANY THIRD PARTY
* WITHOUT THE EXPRESS WRITTEN CONSENT OF SKYBOUND.
* 
* UNDER NO CIRCUMSTANCES MAY THE SOURCE CODE BE USED IN WHOLE OR IN
* PART, AS THE BASIS FOR CREATING A PRODUCT THAT PROVIDES THE SAME, OR
* SUBSTANTIALLY THE SAME, FUNCTIONALITY AS ANY SKYBOUND PRODUCT.
* 
* THE REGISTERED DEVELOPER ACKNOWLEDGES THAT THIS SOURCE CODE
* CONTAINS VALUABLE AND PROPRIETARY TRADE SECRETS OF SKYBOUND. THE
* REGISTERED DEVELOPER AGREES TO EXPEND EVERY EFFORT TO INSURE ITS
* CONFIDENTIALITY.
* 
* THE EULA PERMITS THE LICENSED DEVELOPER TO REDISTRIBUTE THE PRODUCT
* IN EXECUTABLE FORM ONLY IN SUPPORT OF APPLICATIONS WRITTEN USING
* THE PRODUCT.  IT DOES NOT PROVIDE ANY RIGHTS REGARDING THE
* SOURCE CODE CONTAINED HEREIN.
* 
* THIS COPYRIGHT NOTICE MAY NOT BE REMOVED FROM THIS FILE.
*/
#endregion Copyright (C) 2004-2006 Skybound. All Rights Reserved.

#region Imported Namespaces
using System;
using System.Collections;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;
#endregion

/*

#defines

THEME_ALL
	THEME_COLOR
	THEME_METRIC
	THEME_FILENAME
	THEME_MARGIN
	THEME_TEXT

THEME_CLASS_ALL
	THEME_CLASS_BUTTON
	THEME_CLASS_REBAR
	THEME_CLASS_WINDOW
	...

*/

namespace Skybound.Windows.Forms
{
	/// <summary>
	/// Draws theme parts using the Windows Theme API.
	/// </summary>
	struct ThemeRenderer
	{
		#region Detection & Support Methods
		
		/// <summary>
		/// Ensures that the Supported and Enabled properties have been initialized.
		/// </summary>
		static private void EnsureInitialized()
		{
			if (!InitializationComplete && Application.MessageLoop)
			{
				_CommonControlVersion = GetCommonControlVersion();
				//_Supported = (_CommonControlVersion.Major >= 6);
				_Supported = OSFeature.Feature.IsPresent(OSFeature.Themes);
				
				if (_CommonControlVersion.Major >= 6)
					_IsThemeActive = ThemeInternal.UxTheme.IsThemeActive();
				
				if (ThemeControl == null)
				{
					CachedThemeControl = new ThemeControlHelper();
					ThemeControl = new ThemeControlHelper();
				}
				
				InitializationComplete = true;
			}
			
			if (ThemeChangedControl == null)
			{
				ThemeChangedControl = new ThemeChangedWatcher();
			}
		}
		
		static bool InitializationComplete;
		
		/// <summary>
		/// Gets the version of comctl32.dll linked by the application.
		/// </summary>
		/// <returns></returns>
		static private Version GetCommonControlVersion()
		{
			ThemeInternal.DLLVERSIONINFO info = new ThemeInternal.DLLVERSIONINFO();
			info.cbSize = Marshal.SizeOf(typeof(ThemeInternal.DLLVERSIONINFO));
			ThemeInternal.DllGetVersion(ref info);
			
			return new Version(info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);
		}
		
		/// <summary>
		/// Gets a value that indicates whether the operating system supports visual styles.
		/// </summary>
		static public bool Supported
		{
			get { EnsureInitialized(); return _Supported; }
		}
		static bool _Supported;
		
		/// <summary>
		/// Gets a value that indicates whether visual styles are enabled for the current application.
		/// </summary>
		/// <value><see langword="true"/> if visual styles are enabled for the current application; otherwise, <see langword="false"/>.</value>
		/// <remarks>If visual styles are enabled or disabled in Control Panel/Display Properties while your application is running,
		/// the value of this property will change.</remarks>
		static public bool Enabled
		{
			get
			{
				EnsureInitialized();
				
				// can't be enabled if not linked to comctl 6
				if (_CommonControlVersion == null || _CommonControlVersion.Major < 6)
					return false;
				
				// check whether the application is themed
				return _IsThemeActive && InvokeIsAppThemed();
			}
		}
		static bool _IsThemeActive;

        static public void Reset()
        {
            // force re-initialization
            _CommonControlVersion = null;
            InitializationComplete = false;
        }

		/// <summary>
		/// Calls the IsThemeActive function.  This is in its own method so it isn't compiled by the JIT unless
		/// visual styles are supported (and thus, unmanaged code permissions are available).
		/// </summary>
		static private bool InvokeIsAppThemed()
		{
			return ThemeInternal.UxTheme.IsAppThemed();
		}
		
		/// <summary>
		/// Gets the version of the Windows Common Controls DLL to which this application is linked.
		/// </summary>
		/// <value>A <see cref="Version"/> object representing the version of the Windows Common Controls DLL
		/// to which this application is linked.</value>
		static public Version CommonControlVersion
		{
			get { return _CommonControlVersion; }
		}
		static Version _CommonControlVersion;
		
		static Control CachedThemeControl;
		static Control ThemeControl;
		static Control ThemeChangedControl;
		
		class ThemeControlHelper : Control
		{
			public ThemeControlHelper()
			{
				CreateControl();
			}
		}
		
		class ThemeChangedWatcher : ThemeControlHelper
		{
			protected override void WndProc(ref Message m)
			{
				if (m.Msg == 0x31A) // WM_THEMECHANGED
				{
					// force re-initialization
					InitializationComplete = false;
					_CommonControlVersion = null;
				}
				base.WndProc (ref m);
			}
		}
		
		#endregion
		
		#region Background Painting Methods
		/// <summary>
		/// Returns whether the specified control should be drawn using a transparent background.  A control is considered for
		/// transparency when it has no BackgroundImage and its background color is set to Control.
		/// </summary>
		/// <param name="control">The control to check.</param>
		/// <returns></returns>
		static public bool HasTransparentBackground(Control control)
		{
			if (control.BackgroundImage != null)
				return false;
			else if (!control.BackColor.Equals(SystemColors.Control))
				return false;
			
			return true;
		}
		
		/// <summary>
		/// Paints the background of a <see cref="Control"/>.
		/// </summary>
		/// <param name="control">The <see cref="Control"/> whose background is painted.</param>
		/// <param name="e">A <see cref="PaintEventArgs"/> which contains the <see cref="Graphics"/> surface and clipping
		/// rectangle in which the background is drawn.</param>
		static public void PaintBackground(Control control, PaintEventArgs e)
		{
			if (Supported)
			{
				IntPtr hdc = IntPtr.Zero;
				try
				{
					hdc = e.Graphics.GetHdc();
					PaintBackground(control, hdc, e.ClipRectangle);
				}
				finally
				{
					e.Graphics.ReleaseHdc(hdc);
				}
			}
		}
		
		/// <summary>
		/// Paints the background of a <see cref="Control"/> onto a GDI device context.
		/// </summary>
		/// <param name="control">The <see cref="Control"/> whose background is painted.</param>
		/// <param name="hdc">The GDI device context onto which the background is drawn.</param>
		/// <param name="clipBounds">The bounding rectangle in which the backgrounds is clipped.</param>
		static public void PaintBackground(Control control, IntPtr hdc, Rectangle clipBounds)
		{
			ThemeInternal.RECT rect = new ThemeInternal.RECT(clipBounds);
			ThemeInternal.UxTheme.DrawThemeParentBackground(control.Handle, hdc, ref rect);
		}
		#endregion
		
		#region Instance Construction
		/// <summary>
		/// Initializes a new instance of a <see cref="ThemeRenderer"/>.
		/// </summary>
		public ThemeRenderer(int partId)
		{
			this.Value = partId;
		}
		
		int Value;
		
		private string GetCachedString(ArrayList cache, int shift, int mask)
		{
			int index = (this.Value >> shift) & mask;
			if (index > 0 && index <= cache.Count)
			{
				return cache[index - 1] as string;
			}
			return null;
		}
		
		private void SetCachedString(ArrayList cache, int sortedItemCount, int shift, int mask, string value)
		{
			if (value == null)
			{
				this.Value &= ~(mask << shift);
			}
			else
			{
				// first, look for a predefined class name.  binary searching is faster
				int index = cache.BinarySearch(0, sortedItemCount, value, null);
				if (index < 0)
				{
					index = cache.IndexOf(value, sortedItemCount);
					if (index < 0)
					{
						index = cache.Add(value);
					}
				}
					
				this.Value = ((index + 1) << shift) | (this.Value & ~(mask << shift));
			}
		}
		
		/// <summary>
		/// Gets or sets the name of the theme application assigned to the renderer.
		/// </summary>
		public string ApplicationName
		{
			get
			{
				return GetCachedString(ApplicationNameCache, ApplicationShift, ApplicationMask);
			}
			set
			{
				SetCachedString(ApplicationNameCache, SortedApplicationNameCount, ApplicationShift, ApplicationMask, value);
			}
		}
		
		/// <summary>
		/// Gets or sets the name of the theme class assigned to the renderer.
		/// </summary>
		public string ClassName
		{
			get
			{
				return GetCachedString(ClassNameCache, ClassShift, ClassMask);
			}
			set
			{
				SetCachedString(ClassNameCache, SortedClassNameCount, ClassShift, ClassMask, value);
			}
		}
		
		/// <summary>
		/// Gets or sets the theme part ID assigned to the renderer.
		/// </summary>
		public int PartID
		{
			get { return Value & PartMask; }
			set { Value = (Value & ~PartMask) | (value & PartMask); }
		}
		
		const int PartMask = 0xFF;
		const int ClassShift = 8;
		const int ClassMask = 0xFFFF;
		const int ApplicationShift = 24;
		const int ApplicationMask = 0xFF;
		
		/// <summary>Static initializer.</summary>
		static ThemeRenderer()
		{
			ClassNameCache = new ArrayList();
			ClassNameCache.Add("Button");
			ClassNameCache.Add("Clock");
			ClassNameCache.Add("ComboBox");
			ClassNameCache.Add("Edit");
			ClassNameCache.Add("ExplorerBar");
			ClassNameCache.Add("Header");
			ClassNameCache.Add("ListView");
			ClassNameCache.Add("Menu");
			ClassNameCache.Add("MenuBand");
			ClassNameCache.Add("Page");
			ClassNameCache.Add("Progress");
			ClassNameCache.Add("Rebar");
			ClassNameCache.Add("Scrollbar");
			ClassNameCache.Add("Spin");
			ClassNameCache.Add("StartPanel");
			ClassNameCache.Add("Status");
			ClassNameCache.Add("Tab");
			ClassNameCache.Add("TaskBand");
			ClassNameCache.Add("TaskBar");
			ClassNameCache.Add("Toolbar");
			ClassNameCache.Add("Tooltip");
			ClassNameCache.Add("Trackbar");
			ClassNameCache.Add("TrayNotify");
			ClassNameCache.Add("Treeview");
			ClassNameCache.Add("Window");
			
			ApplicationNameCache = new ArrayList();
			ApplicationNameCache.Add("ExplorerBar");
			ApplicationNameCache.Add("PlacesBar");
			ApplicationNameCache.Add("Start");
			ApplicationNameCache.Add("StartMenu");
			ApplicationNameCache.Add("TaskBand");
			ApplicationNameCache.Add("TaskBandGroupMenu");
			ApplicationNameCache.Add("TaskBandVert");
			ApplicationNameCache.Add("TaskBar");
			ApplicationNameCache.Add("TaskBarVert");
			ApplicationNameCache.Add("TrayNotify");
			ApplicationNameCache.Add("TrayNotifyHoriz");
			ApplicationNameCache.Add("TrayNotifyHorizOpen");
			ApplicationNameCache.Add("TrayNotifyVert");
			ApplicationNameCache.Add("TrayNotifyVertOpen");
			ApplicationNameCache.Add("Shell");
			
			HthemeCache = new Htheme[ClassNameCache.Count];
		}
		
		static ArrayList ClassNameCache;
		static ArrayList ApplicationNameCache;
		
		static Htheme [] HthemeCache;
		
		const int SortedClassNameCount = 25;
		const int SortedApplicationNameCount = 15;
		
		/// <summary>
		/// Provides a disposable managed wrapper for an HTHEME handle.
		/// </summary>
		#region private struct Htheme : IDisposable
		private struct Htheme : IDisposable
		{
			/// <summary>
			/// Initializes a new instance of <see cref="Htheme"/>.
			/// </summary>
			public Htheme(ThemeRenderer theme)
			{
				// update current application name
				bool hasCustomAppName = (theme.Value >> ApplicationShift) != 0;
				if (hasCustomAppName)
				{
					string appName = theme.ApplicationName;
					if (appName != CurrentApplicationName)
					{
						ThemeInternal.UxTheme.SetWindowTheme(ThemeControl.Handle, CurrentApplicationName = appName, null);
					}
				}
				
				if (!hasCustomAppName)
				{
					int themeID = (theme.Value >> ClassShift) & ClassMask;
					if (themeID > 0 && themeID <= HthemeCache.Length)
					{
						themeID--;
						Htheme cached = HthemeCache[themeID];
						
						if (cached._Handle == IntPtr.Zero)
						{
							cached._Handle = ThemeInternal.UxTheme.OpenThemeData(CachedThemeControl.Handle, theme.ClassName);
							HthemeCache[themeID] = cached;
						}
						
						this._Handle = cached._Handle;
						this._CachedHandle = true;
						
						return;
					}
				}
				
				// default to non-cached
				this._Handle = ThemeInternal.UxTheme.OpenThemeData(ThemeControl.Handle, theme.ClassName);
				this._CachedHandle = false;
			}
			bool _CachedHandle;
			
			static string CurrentApplicationName;
			
			/// <summary>
			/// Gets whether the handle was created successfully.
			/// </summary>
			public bool IsValid
			{
				get { return _Handle != IntPtr.Zero; }
			}
			
			/// <summary>
			/// Gets the underlying HTHEME handle.
			/// </summary>
			public IntPtr Handle
			{
				get { return _Handle; }
			}
			IntPtr _Handle;
			
			/// <summary>
			/// Closes the HTHEME handle.
			/// </summary>
			public void Dispose()
			{
				if (!_CachedHandle)
					ThemeInternal.UxTheme.CloseThemeData(_Handle);
			}
		}
		#endregion
		
		#endregion
		
		#region Draw Methods
		/// <summary>
		/// Draws the part defined by the current renderer onto a specified <see cref="Graphics"/> surface.
		/// </summary>
		/// <param name="graphics">The <see cref="Graphics"/> surface on which the part is drawn.</param>
		/// <param name="state">A constant which determines the current state of the part.</param>
		/// <param name="bounds">The bounding rectangle in which the part is drawn.</param>
		public void Draw(System.Drawing.Graphics graphics, int state, Rectangle bounds)
		{
			Draw(graphics, state, bounds, bounds);
		}
		
		/// <summary>
		/// Draws the part defined by the current renderer onto a specified <see cref="Graphics"/> surface, within the
		/// given clipping bounds.
		/// </summary>
		/// <param name="graphics">The <see cref="Graphics"/> surface on which the part is drawn.</param>
		/// <param name="state">A constant which determines the current state of the part.</param>
		/// <param name="bounds">The bounding rectangle in which the part is drawn.</param>
		/// <param name="clipBounds">The bounding rectangle in which the part is clipped.</param>
		public void Draw(System.Drawing.Graphics graphics, int state, Rectangle bounds, Rectangle clipBounds)
		{
			if (Supported)
			{
				IntPtr hdc = IntPtr.Zero;
				try
				{
					DrawCore(hdc = graphics.GetHdc(), state, bounds, clipBounds);
				}
				finally
				{
					if (hdc != IntPtr.Zero)
						graphics.ReleaseHdc(hdc);
				}
			}
		}
		
		/// <summary>
		/// Draws the part defined by the current renderer onto a specified GDI device context, within the
		/// given clipping bounds.
		/// </summary>
		/// <param name="hdc">The GDI device context on which the part is drawn.</param>
		/// <param name="state">A constant which determines the current state of the part.</param>
		/// <param name="bounds">The bounding rectangle in which the part is drawn.</param>
		/// <param name="clipBounds">The bounding rectangle in which the part is clipped.</param>
		public void Draw(IntPtr hdc, int state, Rectangle bounds, Rectangle clipBounds)
		{
			if (Supported)
				DrawCore(hdc, state, bounds, clipBounds);
		}
		
		void DrawCore(IntPtr hdc, int state, Rectangle bounds, Rectangle clipBounds)
		{
			using (Htheme theme = new Htheme(this))
			{
				if (theme.IsValid)
				{
					ThemeInternal.RECT rect = new ThemeInternal.RECT(bounds);
					ThemeInternal.RECT clipRect = new ThemeInternal.RECT(clipBounds);
					
					ThemeInternal.UxTheme.DrawThemeBackground(
						theme.Handle, hdc, PartID, state, ref rect, ref clipRect);
				}
			}
		}
		
		/// <summary>
		/// Draws the part defined by the current renderer onto a specified device context, within the
		/// given clipping bounds, and using the specified <see cref="ThemeDrawFlags"/>.
		/// </summary>
		/// <param name="hdc">The GDI device context on which the part is drawn.</param>
		/// <param name="state">A constant which determines the current state of the part.</param>
		/// <param name="bounds">The bounding rectangle in which the part is drawn.</param>
		/// <param name="clipBounds">The bounding rectangle in which the part is clipped.</param>
		/// <param name="flags">A combination of <see cref="ThemeDrawFlags"/> which specify additional
		/// options used to draw the part.</param>
		public void Draw(IntPtr hdc, int state, Rectangle bounds, Rectangle clipBounds, ThemeDrawFlags flags)
		{
			using (Htheme theme = new Htheme(this))
			{
				if (theme.IsValid)
				{
					if ((flags & ThemeDrawFlags.UseDCOrigin) == ThemeDrawFlags.UseDCOrigin)
					{
						ThemeInternal.POINTAPI lpPoint;
						ThemeInternal.GetWindowOrgEx(hdc, out lpPoint);
						
						bounds.Offset(lpPoint.X*2, lpPoint.Y*2);
						clipBounds.Offset(lpPoint.X*2, lpPoint.Y*2);
						
						flags &= ~ThemeDrawFlags.UseDCOrigin;
					}
					
					ThemeInternal.RECT rect = new ThemeInternal.RECT(bounds);
					
					ThemeInternal.DTBGOPTS pOptions = new ThemeInternal.DTBGOPTS();
					pOptions.dwSize = Marshal.SizeOf(typeof(ThemeInternal.DTBGOPTS));
					pOptions.dwFlags = (int)flags;
					pOptions.rcClip = new ThemeInternal.RECT(clipBounds);
					
					ThemeInternal.UxTheme.DrawThemeBackgroundEx(
						theme.Handle, hdc, PartID, state, ref rect, ref pOptions);
				}
			}
		}
		
		#if THEME_ALL || THEME_TEXT
		/// <summary>
		/// Draws text onto a GDI device context within the specified bounding rectangle, with the option of displaying disabled text
		/// and applying other text formatting.
		/// </summary>
		/// <param name="hdc">The GDI device context on which the text is drawn.</param>
		/// <param name="state">The theme state constant for which the text is drawn.</param>
		/// <param name="bounds">The bounding rectangle in which the text is drawn.</param>
		/// <param name="text">The text to draw.</param>
		/// <param name="disabled">True if the text should be drawn disabled; otherwise, false, and the text is drawn normally.</param>
		/// <param name="flags">A bitwise combination of the <see cref="ThemeTextFormatFlags"/> values.</param>
		public void DrawText(IntPtr hdc, int state, Rectangle bounds, string text, bool disabled, ThemeTextFormatFlags flags)
		{
			if (Supported && text != null && text.Length > 0 && hdc != IntPtr.Zero)
			{
				using (Htheme theme = new Htheme(this))
				{
					if (theme.IsValid)
					{
						ThemeInternal.RECT prc = new ThemeInternal.RECT(bounds);
						ThemeInternal.UxTheme.DrawThemeText(theme.Handle, hdc, this.PartID, state, text, text.Length, (int)flags, disabled ? 1 : 0, ref prc);
					}
				}
			}
		}
		#endif
		
		#endregion
		
		#region Query Methods
		
		#if THEME_ALL || THEME_COLOR
		/// <summary>
		/// Returns a color defined on the part associated with this renderer.
		/// </summary>
		/// <param name="state">The theme state constant for which the color is defined.</param>
		/// <param name="color">One of the <see cref="ThemeColor"/> constants, which determines the color to return.</param>
		/// <returns></returns>
		public Color GetColor(int state, ThemeColor color)
		{
			return GetColorCore(state, (int)color);
		}
		#endif
		
		/// <summary>
		/// Returns the color of text drawn on the part associated with this renderer.
		/// </summary>
		/// <param name="state">The theme state constant for which the color is defined.</param>
		/// <returns></returns>
		public Color GetTextColor(int state)
		{
			return GetColorCore(state, 3803);
		}
		
		private Color GetColorCore(int state, int colorConstant)
		{
			int result;
			if (Supported)
			{
				using (Htheme theme = new Htheme(this))
				{
					if (theme.IsValid)
					{
						if (ThemeInternal.UxTheme.GetThemeColor(theme.Handle, this.PartID, state, colorConstant, out result) == 0)
							return ColorTranslator.FromWin32(result);
					}
				}
			}
			
			return Color.Red;
		}
		
		#if THEME_ALL || THEME_METRIC
		/// <summary>
		/// Returns an <see cref="Int32"/> metric value defined by the part associated with the renderer.
		/// </summary>
		/// <param name="state">The theme state constant for which the metric is defined.</param>
		/// <param name="metric">One of the <see cref="ThemeMetric"/> constants, which determines the metric to return.</param>
		/// <returns></returns>
		public int GetMetric(int state, ThemeMetric metric)
		{
			if (Supported)
			{
				using (Htheme theme = new Htheme(this))
				{
					if (theme.IsValid)
					{
						int result;
						if (ThemeInternal.UxTheme.GetThemeMetric(theme.Handle, IntPtr.Zero, this.PartID, state, (int)metric, out result) == 0)
							return result;
					}
				}
			}
			
			return 0;
		}
		#endif
		
		#if THEME_ALL || THEME_FILENAME
		/// <summary>
		/// Returns the value of a file name property defined by the part associated with the renderer.
		/// </summary>
		/// <param name="state">The theme state constant for which the filename is defined.</param>
		/// <param name="filename">One of the <see cref="ThemeFileName"/> constants, which determines the file name to return.</param>
		/// <returns></returns>
		public string GetFileName(int state, ThemeFileName filename)
		{
			if (Supported)
			{
				using (Htheme theme = new Htheme(this))
				{
					if (theme.IsValid)
					{
						int result = 0;
						string filenameString = new string((char)0, 128);
						
						if ((result = ThemeInternal.UxTheme.GetThemeFilename(theme.Handle, this.PartID, state, (int)filename, filenameString, 127)) == 0)
						{
							return filenameString.Substring(0, filenameString.IndexOf((char)0));
						}
					}
				}
			}
			
			return null;
		}
		#endif
		
		#if THEME_ALL || THEME_MARGIN
		/// <summary>
		/// Inflates or deflates a rectangle based on the margins for the current part.
		/// </summary>
		/// <param name="state">The theme state constant for which the margins are defined.</param>
		/// <param name="bounds">The rectangle to adjust.</param>
		/// <param name="margin">One of the <see cref="ThemeMargin"/> constants, which determines the margins to use.</param>
		/// <returns></returns>
		public Rectangle AdjustRectangle(int state, Rectangle bounds, ThemeMargin margin)
		{
			if (Supported)
			{
				using (Htheme theme = new Htheme(this))
				{
					if (theme.IsValid)
					{
						ThemeInternal.RECT prc = new ThemeInternal.RECT(bounds);
						int [] margins = new int[4];
						
						if (ThemeInternal.UxTheme.GetThemeMargins(theme.Handle, IntPtr.Zero, this.PartID, state, (int)margin, ref prc, margins) == 0)
							return Rectangle.FromLTRB(
								bounds.X + margins[0],
								bounds.Y + margins[2],
								bounds.Right - margins[1],
								bounds.Bottom - margins[3]
								);
					}
				}
			}
			
			return bounds;
		}
		
		/// <summary>
		/// Returns the content area for the background of the current part.
		/// </summary>
		/// <param name="hdc">The GDI device context to use.</param>
		/// <param name="state">The state of the part.</param>
		/// <param name="bounds">A <see cref="Rectangle"/> which contains the entire background area of the part.</param>
		/// <returns></returns>
		public Rectangle GetBackgroundContentRectangle(IntPtr hdc, int state, Rectangle bounds)
		{
			if (Supported)
			{
				using (Htheme theme = new Htheme(this))
				{
					if (theme.IsValid)
					{
						ThemeInternal.RECT rcBounds = new ThemeInternal.RECT(bounds);
						ThemeInternal.RECT rcContent;
						
						if (ThemeInternal.UxTheme.GetThemeBackgroundContentRect(theme.Handle, hdc, this.PartID, state, ref rcBounds, out rcContent) == 0)
							return rcContent.ToRectangle();
					}
				}
			}
			
			return bounds;
		}
		
		#endif
		
		#endregion
		
		#region class ThemeInternal
		private class ThemeInternal
		{
			/// <summary>
			/// Represents a Windows API RECT structure.
			/// </summary>
			#region struct RECT
			[
			StructLayout(LayoutKind.Sequential)
			]
			public struct RECT 
			{
				public int Left;
				public int Top;
				public int Right;
				public int Bottom;
			
				/// <summary>
				/// Initializes a new instance of <see cref="RECT"/>.
				/// </summary>
				public RECT(Rectangle r)
				{
					this.Left = r.Left;
					this.Top = r.Top;
					this.Right = r.Right;
					this.Bottom = r.Bottom;
				}
				
				/// <summary>
				/// Converts this RECT structure to a managed <see cref="Rectangle"/>.
				/// </summary>
				/// <returns></returns>
				public Rectangle ToRectangle()
				{
					return Rectangle.FromLTRB(Left, Top, Right, Bottom);
				}
				
				/// <summary>
				/// Overrides <see cref="ValueType.ToString"/>.
				/// </summary>
				public override string ToString() 
				{
					return ToRectangle().ToString();
				}
			}
			#endregion
			
			/// <summary>
			/// Summary description for UxTheme.
			/// </summary>
			#region sealed class UxTheme
			[
			System.Security.SuppressUnmanagedCodeSecurity
			]
			public sealed class UxTheme
			{
				private UxTheme() { }
			
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern System.IntPtr OpenThemeData(System.IntPtr hwnd, string pszClassList);
		
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern bool CloseThemeData(System.IntPtr hTheme);
		
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int DrawThemeBackground(System.IntPtr hTheme, System.IntPtr hdc, int iPartId, int iStateId, ref RECT pRect, ref RECT pClipRect);
		
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int DrawThemeBackgroundEx(System.IntPtr hTheme, System.IntPtr hdc, int iPartId, int iStateId, ref RECT pRect, ref DTBGOPTS pOptions);
		
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int DrawThemeParentBackground(IntPtr hwnd, IntPtr hdc, ref RECT prc);
		
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int GetThemeColor(IntPtr hTheme, int partId, int stateId, int propId, out int color);
		
//				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
//				public static extern int GetThemeBool(IntPtr hTheme, int partId, int stateId, int propId, out bool pfVal);
				
				#if THEME_ALL || THEME_MARGINS
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int GetThemeMargins(IntPtr hTheme, IntPtr hdc, int partId, int stateId, int propId, ref RECT pRect, int [] pMargins);
				#endif
				
				#if THEME_ALL || THEME_METRIC
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int GetThemeMetric(IntPtr hTheme, IntPtr hdc, int partId, int stateId, int propId, out int piVal);
				
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int GetThemeBackgroundContentRect(IntPtr hTheme, IntPtr hdc, int partId, int stateId, ref RECT pBoundingRect, out RECT pContentRect);

				#endif
				
//				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
//				public static extern int GetThemeEnumValue(IntPtr hTheme, int partId, int stateId, int propId, out int piVal);
		
				#if THEME_ALL || THEME_TEXT
				[DllImport("uxtheme", CharSet=CharSet.Unicode, SetLastError=false)]
				public static extern int DrawThemeText(System.IntPtr hTheme, System.IntPtr hdc, int iPartId, int iStateId, [MarshalAs(UnmanagedType.LPWStr)] string pszText, int iCharCount, int dwTextFlag, int dwTextFlags2, ref RECT pRect);
				#endif
				
				#if THEME_ALL || THEME_FILENAME
				[DllImport("uxtheme", CharSet=CharSet.Unicode)]
				public static extern int GetThemeFilename(System.IntPtr hTheme, int iPartId, int iStateId, int iPropId, [MarshalAs(UnmanagedType.LPWStr)] string pszThemeFilename, int cchMaxBuffChars);
				#endif
				
				[DllImport("uxtheme", ExactSpelling=true, CharSet=CharSet.Unicode)]
				public extern static int GetCurrentThemeName(string stringThemeName, int lengthThemeName, string stringColorName, int lengthColorName, string stringSizeName, int lengthSizeName);
		
				[DllImport("uxtheme", ExactSpelling=true)]
				public static extern bool IsAppThemed();
				
				[DllImport("uxtheme", CharSet=CharSet.Unicode)]
				public static extern int SetWindowTheme(IntPtr hwnd, string subAppName, string subIdList);
				
				[DllImport("uxtheme", CharSet=CharSet.Unicode)]
				public static extern bool IsThemeActive();
				
				[DllImport("uxtheme", CharSet=CharSet.Unicode)]
				public static extern int GetThemeAppProperties();
				
				/*
				public const int S_OK = 1;
			
				public const int DT_BOTTOM = 0x8;
				public const int DT_TOP = 0x0;
				public const int DT_CENTER = 0x1;
				public const int DT_LEFT = 0x0;
				public const int DT_RIGHT = 0x2;
				public const int DT_VCENTER = 0x4;
				public const int DT_HIDEPREFIX = 0x100000;
				public const int DT_NOPREFIX = 0x800;
			
				public const int TMT_BGTYPE = 4001;
				public const int TMT_BORDERTYPE = 4002;
				public const int TMT_FILLTYPE = 4003;
				public const int TMT_SIZINGTYPE = 4004;
				public const int TMT_HALIGN = 4005;
				public const int TMT_CONTENTALIGNMENT = 4006;
				public const int TMT_VALIGN = 4007;
				public const int TMT_OFFSETTYPE = 4008;
				public const int TMT_ICONEFFECT = 4009;
				public const int TMT_TEXTSHADOWTYPE = 4010;
				public const int TMT_IMAGELAYOUT = 4011;
				public const int TMT_GLYPHTYPE = 4012;
				public const int TMT_IMAGESELECTTYPE = 4013;
				public const int TMT_GLYPHFONTSIZINGTYPE = 4014;
				public const int TMT_TRUESIZESCALINGTYPE = 4015;
			
				public const int TMT_SIZINGMARGINS = 3601;
				public const int TMT_CONTENTMARGINS = 3602;
				public const int TMT_CAPTIONMARGINS = 3603;
			
				public const int TMT_IMAGECOUNT = 2401;
				public const int TMT_ALPHALEVEL = 2402;
				public const int TMT_BORDERSIZE = 2403;
				public const int TMT_ROUNDCORNERWIDTH = 2404;
				public const int TMT_ROUNDCORNERHEIGHT = 2405;
				public const int TMT_GRADIENTRATIO1 = 2406;
				public const int TMT_GRADIENTRATIO2 = 2407;
				public const int TMT_GRADIENTRATIO3 = 2408;
				public const int TMT_GRADIENTRATIO4 = 2409;
				public const int TMT_GRADIENTRATIO5 = 2410;
				public const int TMT_PROGRESSCHUNKSIZE = 2411;
				public const int TMT_PROGRESSSPACESIZE = 2412;
				public const int TMT_SATURATION = 2413;
				public const int TMT_TEXTBORDERSIZE = 2414;
				public const int TMT_ALPHATHRESHOLD = 2415;
				public const int TMT_WIDTH = 2416;
				public const int TMT_HEIGHT = 2417;
				public const int TMT_GLYPHINDEX = 2418;
				public const int TMT_TRUESIZESTRETCHMARK = 2419;
				public const int TMT_MINDPI1 = 2420;
				public const int TMT_MINDPI2 = 2421;
				public const int TMT_MINDPI3 = 2422;
				public const int TMT_MINDPI4 = 2423;
				public const int TMT_MINDPI5 = 2424;
				*/
			}
			#endregion
			
			[StructLayout(LayoutKind.Sequential)]
			public struct DTBGOPTS
			{
				public int dwSize;
				public int dwFlags;
				public RECT rcClip;
			};
			
			[StructLayout(LayoutKind.Sequential)]
			public struct DLLVERSIONINFO
			{
				public int cbSize;
				public int dwMajorVersion;
				public int dwMinorVersion;
				public int dwBuildNumber;
				public int dwPlatformID;
			}
			
			[DllImport("comctl32.dll")]
			public extern static int DllGetVersion(ref DLLVERSIONINFO dvi);
			
			[DllImport("gdi32.dll")]
			public static extern bool GetWindowOrgEx(IntPtr hdc, out POINTAPI lpPoint);
			
			[StructLayout(LayoutKind.Sequential)]
			public struct POINTAPI
			{
				public int X;
				public int Y;
			}
		}
		#endregion
	}
	
	#region ThemePart Types
	
	#region class ThemeButton (1)
	#if THEME_CLASS_ALL || THEME_CLASS_BUTTON
	/// <summary>
	/// Contains the parts and states which represent the Button theme class.
	/// </summary>
	class ThemeButton
	{
		private const int ClassID = 1 << 8;
		
		/// <summary>
		/// The default renderer for PushButton parts.
		/// </summary>
		public static readonly ThemeRenderer PushButton = new ThemeRenderer(ClassID | 0x1);
		
		/// <summary>Represents the state of a Button class PushButton part that can be drawn by a theme.</summary>
		public const int PushButtonNormal = 1;
		/// <summary>Represents the state of a Button class PushButton part that can be drawn by a theme.</summary>
		public const int PushButtonHot = 2;
		/// <summary>Represents the state of a Button class PushButton part that can be drawn by a theme.</summary>
		public const int PushButtonPressed = 3;
		/// <summary>Represents the state of a Button class PushButton part that can be drawn by a theme.</summary>
		public const int PushButtonDisabled = 4;
		/// <summary>Represents the state of a Button class PushButton part that can be drawn by a theme.</summary>
		public const int PushButtonDefaulted = 5;
		
		/// <summary>
		/// The default renderer for RadioButton parts.
		/// </summary>
		public static readonly ThemeRenderer RadioButton = new ThemeRenderer(ClassID | 0x2);
		
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonUncheckedNormal = 1;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonUncheckedHot = 2;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonUncheckedPressed = 3;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonUncheckedDisabled = 4;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonCheckedNormal = 5;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonCheckedHot = 6;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonCheckedPressed = 7;
		/// <summary>Represents the state of a Button class RadioButton part that can be drawn by a theme.</summary>
		public const int RadioButtonCheckedDisabled = 8;
		
		/// <summary>
		/// The default renderer for CheckBox parts.
		/// </summary>
		public static readonly ThemeRenderer CheckBox = new ThemeRenderer(ClassID | 0x3);
		
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxUncheckedNormal = 1;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxUncheckedHot = 2;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxUncheckedPressed = 3;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxUncheckedDisabled = 4;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxCheckedNormal = 5;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxCheckedHot = 6;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxCheckedPressed = 7;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxCheckedDisabled = 8;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxMixedNormal = 9;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxMixedHot = 10;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxMixedPressed = 11;
		/// <summary>Represents the state of a Button class CheckBox part that can be drawn by a theme.</summary>
		public const int CheckBoxMixedDisabled = 12;
		
		/// <summary>
		/// The default renderer for GroupBox parts.
		/// </summary>
		public static readonly ThemeRenderer GroupBox = new ThemeRenderer(ClassID | 0x4);
		
		/// <summary>Represents the state of a Button class GroupBox part that can be drawn by a theme.</summary>
		public const int GroupBoxNormal = 1;
		/// <summary>Represents the state of a Button class GroupBox part that can be drawn by a theme.</summary>
		public const int GroupBoxDisabled = 2;
		
		/// <summary>
		/// The default renderer for UserButton parts.
		/// </summary>
		public static readonly ThemeRenderer UserButton = new ThemeRenderer(ClassID | 0x5);
	}
	#endif
	#endregion
	
	#region class ThemeToolBar (2)
	#if THEME_CLASS_ALL || THEME_CLASS_CLOCK
	/// <summary>
	/// Contains the parts and states which represent the Clock theme class.
	/// </summary>
	class ThemeClock
	{
		private const int ClassID = 2 << 8;
		
		/// <summary>The default renderer for Time parts.</summary>
		public static readonly ThemeRenderer Time = new ThemeRenderer(ClassID | 1);
		
		/// <summary>Represents the state of a Time class part that can be drawn by a theme.</summary>
		public const int TimeNormal = 1;
	}
	#endif
	
	#endregion
	
	#region class ThemeComboBox (3)
	#if THEME_CLASS_ALL || THEME_CLASS_COMBOBOX
	/// <summary>
	/// Contains the parts and states which represent the ComboBox theme class.
	/// </summary>
	class ThemeComboBox
	{
		private const int ClassID = 3 << 8;
		
		/// <summary>The default renderer for DropDownButton parts.</summary>
		public static readonly ThemeRenderer DropDownButton = new ThemeRenderer(ClassID | 1);
		
		/// <summary>Represents the state of a DropDownButton class part that can be drawn by a theme.</summary>
		public const int DropDownButtonNormal = 1;
		/// <summary>Represents the state of a DropDownButton class part that can be drawn by a theme.</summary>
		public const int DropDownButtonHot = 2;
		/// <summary>Represents the state of a DropDownButton class part that can be drawn by a theme.</summary>
		public const int DropDownButtonPressed = 3;
		/// <summary>Represents the state of a DropDownButton class part that can be drawn by a theme.</summary>
		public const int DropDownButtonDisabled = 4;
	}
	#endif
	
	#endregion
	
	#region class ThemeEdit (4)
	#if THEME_CLASS_ALL || THEME_CLASS_EDIT
	/// <summary>
	/// Contains the parts and states which represent the Edit theme class.
	/// </summary>
	class ThemeEdit
	{
		private const int ClassID = 4 << 8;
		
		/// <summary>The default renderer for Text parts.</summary>
		public static readonly ThemeRenderer Text = new ThemeRenderer(ClassID | 1);
		
		/// <summary>The default renderer for Caret parts.</summary>
		public static readonly ThemeRenderer Caret = new ThemeRenderer(ClassID | 2);
		
		/// <summary>Represents the state of an Edit class Text part that can be drawn by a theme.</summary>
		public const int TextNormal = 1;
		/// <summary>Represents the state of an  class Text part that can be drawn by a theme.</summary>
		public const int TextHot = 2;
		/// <summary>Represents the state of an  class Text part that can be drawn by a theme.</summary>
		public const int TextSelected = 3;
		/// <summary>Represents the state of an  class Text part that can be drawn by a theme.</summary>
		public const int TextDisabled = 4;
		/// <summary>Represents the state of an  class Text part that can be drawn by a theme.</summary>
		public const int TextFocused = 5;
		/// <summary>Represents the state of an  class Text part that can be drawn by a theme.</summary>
		public const int TextReadOnly = 6;
		/// <summary>Represents the state of an  class Text part that can be drawn by a theme.</summary>
		public const int TextAssist = 7;
	}
	#endif
	
	#endregion
	
	#region class ThemeExplorerBar (5)
	#if THEME_CLASS_ALL || THEME_CLASS_EXPLORERBAR
	/// <summary>
	/// Contains the parts and states which represent the ExplorerBar theme class.
	/// </summary>
	class ThemeExplorerBar
	{
		private const int ClassID = 5 << 8;
		
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer HeaderBackground = new ThemeRenderer(ClassID | 1);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer HeaderClose = new ThemeRenderer(ClassID | 2);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer HeaderPin = new ThemeRenderer(ClassID | 3);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer IEBarMenu = new ThemeRenderer(ClassID | 4);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer NormalGroupBackground = new ThemeRenderer(ClassID | 5);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer NormalGroupCollapse = new ThemeRenderer(ClassID | 6);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer NormalGroupExpand = new ThemeRenderer(ClassID | 7);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer NormalGroupHead = new ThemeRenderer(ClassID | 8);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer SpecialGroupBackground = new ThemeRenderer(ClassID | 9);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer SpecialGroupCollapse = new ThemeRenderer(ClassID | 10);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer SpecialGroupExpand = new ThemeRenderer(ClassID | 11);
		/// <summary>Represents an ExplorerBar class part that can be drawn by a theme.</summary>
		public static readonly ThemeRenderer SpecialGroupHead = new ThemeRenderer(ClassID | 12);
		
		/// <summary>Represents the state of an Explorerbar class HeaderClose part that can be drawn by a theme.</summary>
		public const int HeaderCloseNormal = 1;
		/// <summary>Represents the state of an Explorerbar class HeaderClose part that can be drawn by a theme.</summary>
		public const int HeaderCloseHot = 2;
		/// <summary>Represents the state of an Explorerbar class HeaderClose part that can be drawn by a theme.</summary>
		public const int HeaderClosePressed = 3;

		/// <summary>Represents the state of an Explorerbar class HeaderPin part that can be drawn by a theme.</summary>
		public const int HeaderPinNormal = 1;
		/// <summary>Represents the state of an Explorerbar class HeaderPin part that can be drawn by a theme.</summary>
		public const int HeaderPinHot = 2;
		/// <summary>Represents the state of an Explorerbar class HeaderPin part that can be drawn by a theme.</summary>
		public const int HeaderPinPressed = 3;
		/// <summary>Represents the state of an Explorerbar class HeaderPin part that can be drawn by a theme.</summary>
		public const int HeaderPinSelectedNormal = 4;
		/// <summary>Represents the state of an Explorerbar class HeaderPin part that can be drawn by a theme.</summary>
		public const int HeaderPinSelectedHot = 5;
		/// <summary>Represents the state of an Explorerbar class HeaderPin part that can be drawn by a theme.</summary>
		public const int HeaderPinSelectedPressed = 6;

		/// <summary>Represents the state of an Explorerbar class IEBarMenu part that can be drawn by a theme.</summary>
		public const int IEBarMenuNormal = 1;
		/// <summary>Represents the state of an Explorerbar class IEBarMenu part that can be drawn by a theme.</summary>
		public const int IEBarMenuHot = 2;
		/// <summary>Represents the state of an Explorerbar class IEBarMenu part that can be drawn by a theme.</summary>
		public const int IEBarMenuPressed = 3;

		/// <summary>Represents the state of an Explorerbar class NormalGroupCollapse part that can be drawn by a theme.</summary>
		public const int NormalGroupCollapseNormal = 1;
		/// <summary>Represents the state of an Explorerbar class NormalGroupCollapse part that can be drawn by a theme.</summary>
		public const int NormalGroupCollapseHot = 2;
		/// <summary>Represents the state of an Explorerbar class NormalGroupCollapse part that can be drawn by a theme.</summary>
		public const int NormalGroupCollapsePressed = 3;

		/// <summary>Represents the state of an Explorerbar class NormalGroupExpand part that can be drawn by a theme.</summary>
		public const int NormalGroupExpandNormal = 1;
		/// <summary>Represents the state of an Explorerbar class NormalGroupExpand part that can be drawn by a theme.</summary>
		public const int NormalGroupExpandHot = 2;
		/// <summary>Represents the state of an Explorerbar class NormalGroupExpand part that can be drawn by a theme.</summary>
		public const int NormalGroupExpandPressed = 3;

		/// <summary>Represents the state of an Explorerbar class SpecialGroupCollapse part that can be drawn by a theme.</summary>
		public const int SpecialGroupCollapseNormal = 1;
		/// <summary>Represents the state of an Explorerbar class SpecialGroupCollapse part that can be drawn by a theme.</summary>
		public const int SpecialGroupCollapseHot = 2;
		/// <summary>Represents the state of an Explorerbar class SpecialGroupCollapse part that can be drawn by a theme.</summary>
		public const int SpecialGroupCollapsePressed = 3;

		/// <summary>Represents the state of an Explorerbar class SpecialGroupExpand part that can be drawn by a theme.</summary>
		public const int SpecialGroupExpandNormal = 1;
		/// <summary>Represents the state of an Explorerbar class SpecialGroupExpand part that can be drawn by a theme.</summary>
		public const int SpecialGroupExpandHot = 2;
		/// <summary>Represents the state of an Explorerbar class SpecialGroupExpand part that can be drawn by a theme.</summary>
		public const int SpecialGroupExpandPressed = 3;
	}
	#endif
	
	#endregion
	
	#region class ThemeHeader (6)
	#if THEME_CLASS_ALL || THEME_CLASS_HEADER
	/// <summary>
	/// Contains the parts and states which represent the Header theme class.
	/// </summary>
	class ThemeHeader
	{
		private const int ClassID = 6 << 8;
		
		/// <summary>The default renderer for Item parts.</summary>
		public static readonly ThemeRenderer Item = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for ItemLeft parts.</summary>
		public static readonly ThemeRenderer ItemLeft = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for ItemRight parts.</summary>
		public static readonly ThemeRenderer ItemRight = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for SortArrow parts.</summary>
		public static readonly ThemeRenderer SortArrow = new ThemeRenderer(ClassID | 4);
		
		/// <summary>Represents the state of a Header class HeaderItem part that can be drawn by a theme.</summary>
		public const int ItemNormal = 1;
		/// <summary>Represents the state of a Header class HeaderItem part that can be drawn by a theme.</summary>
		public const int ItemHot = 2;
		/// <summary>Represents the state of a Header class HeaderItem part that can be drawn by a theme.</summary>
		public const int ItemPressed = 3;
		
		/// <summary>Represents the state of a Header class HeaderItemLeft part that can be drawn by a theme.</summary>
		public const int ItemLeftNormal = 1;
		/// <summary>Represents the state of a Header class HeaderItemLeft part that can be drawn by a theme.</summary>
		public const int ItemLeftHot = 2;
		/// <summary>Represents the state of a Header class HeaderItemLeft part that can be drawn by a theme.</summary>
		public const int ItemLeftPressed = 3;
		
		/// <summary>Represents the state of a Header class HeaderItemRight part that can be drawn by a theme.</summary>
		public const int ItemRightNormal = 1;
		/// <summary>Represents the state of a Header class HeaderItemRight part that can be drawn by a theme.</summary>
		public const int ItemRightHot = 2;
		/// <summary>Represents the state of a Header class HeaderItemRight part that can be drawn by a theme.</summary>
		public const int ItemRightPressed = 3;
		
		/// <summary>Represents the state of a Header class HeaderSortArrow part that can be drawn by a theme.</summary>
		public const int SortArrowSortedUp = 1;
		/// <summary>Represents the state of a Header class HeaderSortArrow part that can be drawn by a theme.</summary>
		public const int SortArrowSortedDown = 2;
	}
	#endif
	#endregion
	
	#region class ThemeListView (7)
	#if THEME_CLASS_ALL || THEME_CLASS_LISTVIEW
	/// <summary>
	/// Contains the parts and states which represent the ListView theme class.
	/// </summary>
	class ThemeListView
	{
		private const int ClassID = 7 << 8;
		
		/// <summary>The default renderer for ListItem parts.</summary>
		public static readonly ThemeRenderer ListItem = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for ListGroup parts.</summary>
		public static readonly ThemeRenderer ListGroup = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for ListDetail parts.</summary>
		public static readonly ThemeRenderer ListDetail = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for ListSortedDetail parts.</summary>
		public static readonly ThemeRenderer ListSortedDetail = new ThemeRenderer(ClassID | 4);
		/// <summary>The default renderer for EmptyText parts.</summary>
		public static readonly ThemeRenderer EmptyText = new ThemeRenderer(ClassID | 5);
		
		/// <summary>Represents the state of a ListView class ListItem part that can be drawn by a theme.</summary>
		public const int ListItemNormal = 1;
		/// <summary>Represents the state of a ListView class ListItem part that can be drawn by a theme.</summary>
		public const int ListItemHot = 2;
		/// <summary>Represents the state of a ListView class ListItem part that can be drawn by a theme.</summary>
		public const int ListItemSelected = 3;
		/// <summary>Represents the state of a ListView class ListItem part that can be drawn by a theme.</summary>
		public const int ListItemDisabled = 4;
		/// <summary>Represents the state of a ListView class ListItem part that can be drawn by a theme.</summary>
		public const int ListItemSelectedNotFocus = 5;
	}
	#endif
	#endregion
	
	//internal const int Menu = 8 << ClassShift;
	//internal const int MenuBand = 9 << ClassShift;
	//internal const int Page = 10 << ClassShift;
	
	#region class ThemeProgress (11)
	#if THEME_CLASS_ALL || THEME_CLASS_LISTVIEW
	/// <summary>
	/// Contains the parts and states which represent the Progress theme class.
	/// </summary>
	class ThemeProgress
	{
		private const int ClassID = 11 << 8;
		
		/// <summary>The default renderer for Bar parts.</summary>
		public static readonly ThemeRenderer Bar = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for BarVert parts.</summary>
		public static readonly ThemeRenderer BarVert = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for Chunk parts.</summary>
		public static readonly ThemeRenderer Chunk = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for ChunkVert parts.</summary>
		public static readonly ThemeRenderer ChunkVert = new ThemeRenderer(ClassID | 4);
	}
	#endif
	#endregion
	
	#region class ThemeRebar (12)
	#if THEME_CLASS_ALL || THEME_CLASS_REBAR
	/// <summary>
	/// Contains the parts and states which represent the Rebar theme class.
	/// </summary>
	class ThemeRebar
	{
		private const int ClassID = 12 << 8;
		
		/// <summary>The default renderer for Gripper parts.</summary>
		public static readonly ThemeRenderer Gripper = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for GripperVert parts.</summary>
		public static readonly ThemeRenderer GripperVert = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for Band parts.</summary>
		public static readonly ThemeRenderer Band = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for Chevron parts.</summary>
		public static readonly ThemeRenderer Chevron = new ThemeRenderer(ClassID | 4);
		/// <summary>The default renderer for ChevronVert parts.</summary>
		public static readonly ThemeRenderer ChevronVert = new ThemeRenderer(ClassID | 5);
		
		/// <summary>Represents the state of a Rebar class Chevron part that can be drawn by a theme.</summary>
		public const int ChevronNormal = 1;
		/// <summary>Represents the state of a Rebar class Chevron part that can be drawn by a theme.</summary>
		public const int ChevronHot = 2;
		/// <summary>Represents the state of a Rebar class Chevron part that can be drawn by a theme.</summary>
		public const int ChevronPressed = 3;
	}
	#endif
	#endregion
	
	#region class ThemeScrollBar (13)
	#if THEME_CLASS_ALL || THEME_CLASS_REBAR
	/// <summary>
	/// Contains the parts and states which represent the ScrollBar theme class.
	/// </summary>
	class ThemeScrollBar
	{
		private const int ClassID = 13 << 8;
		
		/// <summary>The default renderer for ArrowBtn parts.</summary>
		public static readonly ThemeRenderer ArrowBtn = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for ThumbBtnHorz parts.</summary>
		public static readonly ThemeRenderer ThumbBtnHorz = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for ThumbBtnVert parts.</summary>
		public static readonly ThemeRenderer ThumbBtnVert = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for LowerTrackHorz parts.</summary>
		public static readonly ThemeRenderer LowerTrackHorz = new ThemeRenderer(ClassID | 4);
		/// <summary>The default renderer for UpperTrackHorz parts.</summary>
		public static readonly ThemeRenderer UpperTrackHorz = new ThemeRenderer(ClassID | 5);
		/// <summary>The default renderer for LowerTrackVert parts.</summary>
		public static readonly ThemeRenderer LowerTrackVert = new ThemeRenderer(ClassID | 6);
		/// <summary>The default renderer for UpperTrackVert parts.</summary>
		public static readonly ThemeRenderer UpperTrackVert = new ThemeRenderer(ClassID | 7);
		/// <summary>The default renderer for GripperHorz parts.</summary>
		public static readonly ThemeRenderer GripperHorz = new ThemeRenderer(ClassID | 8);
		/// <summary>The default renderer for GripperVert parts.</summary>
		public static readonly ThemeRenderer GripperVert = new ThemeRenderer(ClassID | 9);
		/// <summary>The default renderer for SizeBox parts.</summary>
		public static readonly ThemeRenderer SizeBox = new ThemeRenderer(ClassID | 10);
		
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnUpNormal = 1;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnUpHot = 2;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnUpPressed = 3;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnUpDisabled = 4;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnDownNormal = 5;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnDownHot = 6;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnDownPressed = 7;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnDownDisabled = 8;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnLeftNormal = 9;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnLeftHot = 10;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnLeftPressed = 11;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnLeftDisabled = 12;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnRightNormal = 13;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnRightHot = 14;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnRightPressed = 15;
		/// <summary>Represents the state of a Scrollbar class ArrowBtn part that can be drawn by a theme.</summary>
		public const int ArrowBtnRightDisabled = 16;

		/// <summary>Represents the state of a Scrollbar class Thumb or Track part that can be drawn by a theme.</summary>
		public const int Normal = 1;
		/// <summary>Represents the state of a Scrollbar class Thumb or Track part that can be drawn by a theme.</summary>
		public const int Hot = 2;
		/// <summary>Represents the state of a Scrollbar class Thumb or Track part that can be drawn by a theme.</summary>
		public const int Pressed = 3;
		/// <summary>Represents the state of a Scrollbar class Thumb or Track part that can be drawn by a theme.</summary>
		public const int Disabled = 4;
		
		/// <summary>Represents the state of a Scrollbar class SizeBox part that can be drawn by a theme.</summary>
		public const int SizeBoxRightAlign = 1;
		/// <summary>Represents the state of a Scrollbar class SizeBox part that can be drawn by a theme.</summary>
		public const int SizeBoxLeftAlign = 2;
	}
	#endif
	#endregion
	
	#region class ThemeSpin (14)
	#if THEME_CLASS_ALL || THEME_CLASS_SPIN
	/// <summary>
	/// Contains the parts and states which represent the Spin theme class.
	/// </summary>
	class ThemeSpin
	{
		private const int ClassID = 14 << 8;
		
		/// <summary>The default renderer for Up parts.</summary>
		public static readonly ThemeRenderer Up = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for Down parts.</summary>
		public static readonly ThemeRenderer Down = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for UpHorz parts.</summary>
		public static readonly ThemeRenderer UpHorz = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for DownHorz parts.</summary>
		public static readonly ThemeRenderer DownHorz = new ThemeRenderer(ClassID | 4);
		
		/// <summary>Represents the state of a Spin or Page class part that can be drawn by a theme.</summary>
		public const int Normal = 1;
		/// <summary>Represents the state of a Spin or Page class part that can be drawn by a theme.</summary>
		public const int Hot = 2;
		/// <summary>Represents the state of a Spin or Page class part that can be drawn by a theme.</summary>
		public const int Pressed = 3;
		/// <summary>Represents the state of a Spin or Page class part that can be drawn by a theme.</summary>
		public const int Disabled = 4;
	}
	#endif
	#endregion
	
	//internal const int StartPanel = 15 << ClassShift;
	
	#region class ThemeStatus (16)
	#if THEME_CLASS_ALL || THEME_CLASS_STATUS
	/// <summary>
	/// Contains the parts and states which represent the Status theme class.
	/// </summary>
	class ThemeStatus
	{
		private const int ClassID = 16 << 8;
		
		/// <summary>The default renderer for Pane parts.</summary>
		public static readonly ThemeRenderer Pane = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for GripperPane parts.</summary>
		public static readonly ThemeRenderer GripperPane = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for Gripper parts.</summary>
		public static readonly ThemeRenderer Gripper = new ThemeRenderer(ClassID | 3);
	}
	#endif
	#endregion
	
	#region class ThemeTab (17)
	#if THEME_CLASS_ALL || THEME_CLASS_TAB
	/// <summary>
	/// Contains the parts and states which represent the Tab theme class.
	/// </summary>
	class ThemeTab
	{
		private const int ClassID = 17 << 8;
		
		/// <summary>The default renderer for TabItem parts.</summary>
		public static readonly ThemeRenderer Item = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for TabItemLeftEdge parts.</summary>
		public static readonly ThemeRenderer ItemLeftEdge = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for TabItemRightEdge parts.</summary>
		public static readonly ThemeRenderer ItemRightEdge = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for TabItemBothEdge parts.</summary>
		public static readonly ThemeRenderer ItemBothEdge = new ThemeRenderer(ClassID | 4);
		/// <summary>The default renderer for TabTopTabItem parts.</summary>
		public static readonly ThemeRenderer TopTabItem = new ThemeRenderer(ClassID | 5);
		/// <summary>The default renderer for TabTopTabItemLeftEdge parts.</summary>
		public static readonly ThemeRenderer TopTabItemLeftEdge = new ThemeRenderer(ClassID | 6);
		/// <summary>The default renderer for TabTopTabItemRightEdge parts.</summary>
		public static readonly ThemeRenderer TopTabItemRightEdge = new ThemeRenderer(ClassID | 7);
		/// <summary>The default renderer for TabTopTabItemBothEdge parts.</summary>
		public static readonly ThemeRenderer TopTabItemBothEdge = new ThemeRenderer(ClassID | 8);
		/// <summary>The default renderer for TabPane parts.</summary>
		public static readonly ThemeRenderer Pane = new ThemeRenderer(ClassID | 9);
		/// <summary>The default renderer for TabBody parts.</summary>
		public static readonly ThemeRenderer Body = new ThemeRenderer(ClassID | 10);
		
		/// <summary>Represents the state of a Tab class Item part that can be drawn by a theme.</summary>
		public const int ItemNormal = 1;
		/// <summary>Represents the state of a Tab class Item part that can be drawn by a theme.</summary>
		public const int ItemHot = 2;
		/// <summary>Represents the state of a Tab class Item part that can be drawn by a theme.</summary>
		public const int ItemSelected = 3;
		/// <summary>Represents the state of a Tab class Item part that can be drawn by a theme.</summary>
		public const int ItemDisabled = 4;
		/// <summary>Represents the state of a Tab class Item part that can be drawn by a theme.</summary>
		public const int ItemFocused = 5;
	}
	#endif
	#endregion
	
	//internal const int TaskBand = 18 << ClassShift;
	
	#region class ThemeTaskBar (19)
	#if THEME_CLASS_ALL || THEME_CLASS_TASKBAR
	/// <summary>
	/// Contains the parts and states which represent the Globals theme class.
	/// </summary>
	class ThemeTaskBar
	{
		private const int ClassID = 19 << 8;
		
		/// <summary>The default renderer for BackgroundBottom parts.</summary>
		public static readonly ThemeRenderer BackgroundBottom = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for BackgroundRight parts.</summary>
		public static readonly ThemeRenderer BackgroundRight = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for BackgroundTop parts.</summary>
		public static readonly ThemeRenderer BackgroundTop = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for BackgroundLeft parts.</summary>
		public static readonly ThemeRenderer BackgroundLeft = new ThemeRenderer(ClassID | 4);
		/// <summary>The default renderer for SizingBarBottom parts.</summary>
		public static readonly ThemeRenderer SizingBarBottom = new ThemeRenderer(ClassID | 5);
		/// <summary>The default renderer for SizingBarRight parts.</summary>
		public static readonly ThemeRenderer SizingBarRight = new ThemeRenderer(ClassID | 6);
		/// <summary>The default renderer for SizingBarTop parts.</summary>
		public static readonly ThemeRenderer SizingBarTop = new ThemeRenderer(ClassID | 7);
		/// <summary>The default renderer for SizingBarLeft parts.</summary>
		public static readonly ThemeRenderer SizingBarLeft = new ThemeRenderer(ClassID | 8);
		
		/// <summary>The default renderer for Globals parts.</summary>
		public static readonly ThemeRenderer Empty = new ThemeRenderer(ClassID | 1);
	}
	#endif
	
	#endregion
	
	#region class ThemeToolBar (20)
	#if THEME_CLASS_ALL || THEME_CLASS_TOOLBAR
	/// <summary>
	/// Contains the parts and states which represent the ToolBar theme class.
	/// </summary>
	class ThemeToolBar
	{
		private const int ClassID = 20 << 8;
		
		/// <summary>The default renderer for Button parts.</summary>
		public static readonly ThemeRenderer Button = new ThemeRenderer(ClassID | 0x1);
		/// <summary>The default renderer for DropDownButton parts.</summary>
		public static readonly ThemeRenderer DropDownButton = new ThemeRenderer(ClassID | 0x2);
		/// <summary>The default renderer for SplitButton parts.</summary>
		public static readonly ThemeRenderer SplitButton = new ThemeRenderer(ClassID | 0x3);
		/// <summary>The default renderer for SplitButtonDropDown parts.</summary>
		public static readonly ThemeRenderer SplitButtonDropDown = new ThemeRenderer(ClassID | 0x4);
		/// <summary>The default renderer for Separator parts.</summary>
		public static readonly ThemeRenderer Separator = new ThemeRenderer(ClassID | 0x5);
		/// <summary>The default renderer for SeparatorVert parts.</summary>
		public static readonly ThemeRenderer SeparatorVert = new ThemeRenderer(ClassID | 0x6);
		
		/// <summary>Represents the state of a ToolBar class part that can be drawn by a theme.</summary>
		public const int Normal = 1;
		/// <summary>Represents the state of a ToolBar class part that can be drawn by a theme.</summary>
		public const int Hot = 2;
		/// <summary>Represents the state of a ToolBar class part that can be drawn by a theme.</summary>
		public const int Pressed = 3;
		/// <summary>Represents the state of a ToolBar class part that can be drawn by a theme.</summary>
		public const int Disabled = 4;
		/// <summary>Represents the state of a ToolBar class part that can be drawn by a theme.</summary>
		public const int Checked = 5;
		/// <summary>Represents the state of a ToolBar class part that can be drawn by a theme.</summary>
		public const int HotChecked = 6;
	}
	#endif
	
	#endregion
	
	//internal const int Tooltip = 21 << ClassShift;
	
	#region class ThemeTrackBar (22)
	#if THEME_CLASS_ALL || THEME_CLASS_TRACKBAR
	/// <summary>
	/// Contains the parts and states which represent the TrackBar theme class.
	/// </summary>
	class ThemeTrackBar
	{
		private const int ClassID = 22 << 8;
		
		/// <summary>The default renderer for TrackBarTrack parts.</summary>
		public static readonly ThemeRenderer Track = new ThemeRenderer(ClassID | 1);
		/// <summary>The default renderer for TrackBarTrackVert parts.</summary>
		public static readonly ThemeRenderer TrackVert = new ThemeRenderer(ClassID | 2);
		/// <summary>The default renderer for TrackBarThumb parts.</summary>
		public static readonly ThemeRenderer Thumb = new ThemeRenderer(ClassID | 3);
		/// <summary>The default renderer for TrackBarThumbBottom parts.</summary>
		public static readonly ThemeRenderer ThumbBottom = new ThemeRenderer(ClassID | 4);
		/// <summary>The default renderer for TrackBarThumbTop parts.</summary>
		public static readonly ThemeRenderer ThumbTop = new ThemeRenderer(ClassID | 5);
		/// <summary>The default renderer for TrackBarThumbVert parts.</summary>
		public static readonly ThemeRenderer ThumbVert = new ThemeRenderer(ClassID | 6);
		/// <summary>The default renderer for TrackBarThumbLeft parts.</summary>
		public static readonly ThemeRenderer ThumbLeft = new ThemeRenderer(ClassID | 7);
		/// <summary>The default renderer for TrackBarThumbRight parts.</summary>
		public static readonly ThemeRenderer ThumbRight = new ThemeRenderer(ClassID | 8);
		/// <summary>The default renderer for TrackBarTics parts.</summary>
		public static readonly ThemeRenderer Tics = new ThemeRenderer(ClassID | 9);
		/// <summary>The default renderer for TrackBarTicsVert parts.</summary>
		public static readonly ThemeRenderer TicsVert = new ThemeRenderer(ClassID | 10);
		
		/// <summary>Represents the state of a Trackbar class part that can be drawn by a theme.</summary>
		public const int TrackBarNormal = 1;

		/// <summary>Represents the state of a Trackbar class Track or TrackVert part that can be drawn by a theme.</summary>
		public const int TrackNormal = 1;

		/// <summary>Represents the state of a Trackbar class Thumb part that can be drawn by a theme.</summary>
		public const int ThumbNormal = 1;
		/// <summary>Represents the state of a Trackbar class Thumb part that can be drawn by a theme.</summary>
		public const int ThumbHot = 2;
		/// <summary>Represents the state of a Trackbar class Thumb part that can be drawn by a theme.</summary>
		public const int ThumbPressed = 3;
		/// <summary>Represents the state of a Trackbar class Thumb part that can be drawn by a theme.</summary>
		public const int ThumbFocused = 4;
		/// <summary>Represents the state of a Trackbar class Thumb part that can be drawn by a theme.</summary>
		public const int ThumbDisabled = 5;
		
		/// <summary>Represents the state of a Trackbar class Tics part that can be drawn by a theme.</summary>
		public const int TicsNormal = 1;
		
		/// <summary>Represents the state of a Trackbar class TicsVert part that can be drawn by a theme.</summary>
		public const int TicsVertNormal = 1;
	}
	#endif
	#endregion
	
	//internal const int TrayNotify = 23 << ClassShift;
	
	#region class ThemeTreeView (24)
#if THEME_CLASS_ALL || THEME_CLASS_SPIN
	/// <summary>
	/// Contains the parts and states which represent Treeview theme class.
	/// </summary>
	class ThemeTreeView
	{
		private const int ClassID = 24 << 8;
		
		/// <summary>The default renderer for Item parts.</summary>
		public static readonly ThemeRenderer Item = new ThemeRenderer(ClassID | 1);
		
		/// <summary>The default renderer for Glyph parts.</summary>
		public static readonly ThemeRenderer Glyph = new ThemeRenderer(ClassID | 2);
		
		/// <summary>The default renderer for Branch parts.</summary>
		public static readonly ThemeRenderer Branch = new ThemeRenderer(ClassID | 3);
		
		/// <summary>Represents the state of a Glyph class part that can be drawn by a theme.</summary>
		public const int GlyphClosed = 1;
		/// <summary>Represents the state of a Glyph class part that can be drawn by a theme.</summary>
		public const int GlyphOpened = 2;
	}
#endif
	#endregion
	
	#region class ThemeWindow (25)
	#if THEME_CLASS_ALL || THEME_CLASS_WINDOW
	/// <summary>
	/// Contains the parts and states which represent the Window theme class.
	/// </summary>
	class ThemeWindow
	{
		private const int ClassID = 25 << 8;
		
		/// <summary>The default renderer for Caption parts.</summary>
		public static readonly ThemeRenderer Caption = new ThemeRenderer(ClassID + 1);
		/// <summary>The default renderer for SmallCaption parts.</summary>
		public static readonly ThemeRenderer SmallCaption = new ThemeRenderer(ClassID + 2);
		/// <summary>The default renderer for MinCaption parts.</summary>
		public static readonly ThemeRenderer MinCaption = new ThemeRenderer(ClassID + 3);
		/// <summary>The default renderer for SmallMinCaption parts.</summary>
		public static readonly ThemeRenderer SmallMinCaption = new ThemeRenderer(ClassID + 4);
		/// <summary>The default renderer for MaxCaption parts.</summary>
		public static readonly ThemeRenderer MaxCaption = new ThemeRenderer(ClassID + 5);
		/// <summary>The default renderer for SmallMaxCaption parts.</summary>
		public static readonly ThemeRenderer SmallMaxCaption = new ThemeRenderer(ClassID + 6);
		/// <summary>The default renderer for FrameLeft parts.</summary>
		public static readonly ThemeRenderer FrameLeft = new ThemeRenderer(ClassID + 7);
		/// <summary>The default renderer for FrameRight parts.</summary>
		public static readonly ThemeRenderer FrameRight = new ThemeRenderer(ClassID + 8);
		/// <summary>The default renderer for FrameBottom parts.</summary>
		public static readonly ThemeRenderer FrameBottom = new ThemeRenderer(ClassID + 9);
		/// <summary>The default renderer for SmallFrameLeft parts.</summary>
		public static readonly ThemeRenderer SmallFrameLeft = new ThemeRenderer(ClassID + 10);
		/// <summary>The default renderer for SmallFrameRight parts.</summary>
		public static readonly ThemeRenderer SmallFrameRight = new ThemeRenderer(ClassID + 11);
		/// <summary>The default renderer for SmallFrameBottom parts.</summary>
		public static readonly ThemeRenderer SmallFrameBottom = new ThemeRenderer(ClassID + 12);
		/// <summary>The default renderer for SysButton parts.</summary>
		public static readonly ThemeRenderer SysButton = new ThemeRenderer(ClassID + 13);
		/// <summary>The default renderer for MdiSysButton parts.</summary>
		public static readonly ThemeRenderer MdiSysButton = new ThemeRenderer(ClassID + 14);
		/// <summary>The default renderer for MinButton parts.</summary>
		public static readonly ThemeRenderer MinButton = new ThemeRenderer(ClassID + 15);
		/// <summary>The default renderer for MdiMinButton parts.</summary>
		public static readonly ThemeRenderer MdiMinButton = new ThemeRenderer(ClassID + 16);
		/// <summary>The default renderer for MaxButton parts.</summary>
		public static readonly ThemeRenderer MaxButton = new ThemeRenderer(ClassID + 17);
		/// <summary>The default renderer for CloseButton parts.</summary>
		public static readonly ThemeRenderer CloseButton = new ThemeRenderer(ClassID + 18);
		/// <summary>The default renderer for SmallCloseButton parts.</summary>
		public static readonly ThemeRenderer SmallCloseButton = new ThemeRenderer(ClassID + 19);
		/// <summary>The default renderer for MdiCloseButton parts.</summary>
		public static readonly ThemeRenderer MdiCloseButton = new ThemeRenderer(ClassID + 20);
		/// <summary>The default renderer for RestoreButton parts.</summary>
		public static readonly ThemeRenderer RestoreButton = new ThemeRenderer(ClassID + 21);
		/// <summary>The default renderer for MdiRestoreButton parts.</summary>
		public static readonly ThemeRenderer MdiRestoreButton = new ThemeRenderer(ClassID + 22);
		/// <summary>The default renderer for HelpButton parts.</summary>
		public static readonly ThemeRenderer HelpButton = new ThemeRenderer(ClassID + 23);
		/// <summary>The default renderer for MdiHelpButton parts.</summary>
		public static readonly ThemeRenderer MdiHelpButton = new ThemeRenderer(ClassID + 24);
		/// <summary>The default renderer for HorzScroll parts.</summary>
		public static readonly ThemeRenderer HorzScroll = new ThemeRenderer(ClassID + 25);
		/// <summary>The default renderer for HorzThumb parts.</summary>
		public static readonly ThemeRenderer HorzThumb = new ThemeRenderer(ClassID + 26);
		/// <summary>The default renderer for VertScroll parts.</summary>
		public static readonly ThemeRenderer VertScroll = new ThemeRenderer(ClassID + 27);
		/// <summary>The default renderer for VertThumb parts.</summary>
		public static readonly ThemeRenderer VertThumb = new ThemeRenderer(ClassID + 28);
		/// <summary>The default renderer for Dialog parts.</summary>
		public static readonly ThemeRenderer Dialog = new ThemeRenderer(ClassID + 29);
		/// <summary>The default renderer for CaptionSizingTemplate parts.</summary>
		public static readonly ThemeRenderer CaptionSizingTemplate = new ThemeRenderer(ClassID + 30);
		/// <summary>The default renderer for SmallCaptionSizingTemplate parts.</summary>
		public static readonly ThemeRenderer SmallCaptionSizingTemplate = new ThemeRenderer(ClassID + 31);
		/// <summary>The default renderer for FrameLeftSizingTemplate parts.</summary>
		public static readonly ThemeRenderer FrameLeftSizingTemplate = new ThemeRenderer(ClassID + 32);
		/// <summary>The default renderer for SmallFrameLeftSizingTemplate parts.</summary>
		public static readonly ThemeRenderer SmallFrameLeftSizingTemplate = new ThemeRenderer(ClassID + 33);
		/// <summary>The default renderer for FrameRightSizingTemplate parts.</summary>
		public static readonly ThemeRenderer FrameRightSizingTemplate = new ThemeRenderer(ClassID + 34);
		/// <summary>The default renderer for SmallFrameRightSizingTemplate parts.</summary>
		public static readonly ThemeRenderer SmallFrameRightSizingTemplate = new ThemeRenderer(ClassID + 35);
		/// <summary>The default renderer for FrameBottomSizingTemplate parts.</summary>
		public static readonly ThemeRenderer FrameBottomSizingTemplate = new ThemeRenderer(ClassID + 36);
		/// <summary>The default renderer for SmallFrameBottomSizingTemplate parts.</summary>
		public static readonly ThemeRenderer SmallFrameBottomSizingTemplate = new ThemeRenderer(ClassID + 37);
		
		/// <summary>Represents the state of a Window class Frame part that can be drawn by a theme.</summary>
		public const int FrameActive = 1;
		/// <summary>Represents the state of a Window class Frame part that can be drawn by a theme.</summary>
		public const int FrameInactive = 2;

		/// <summary>Represents the state of a Window class Caption part that can be drawn by a theme.</summary>
		public const int CaptionActive = 1;
		/// <summary>Represents the state of a Window class Caption part that can be drawn by a theme.</summary>
		public const int CaptionInactive = 2;
		/// <summary>Represents the state of a Window class Caption part that can be drawn by a theme.</summary>
		public const int CaptionDisabled = 3;

		/// <summary>Represents the state of a Window class MaxCaption part that can be drawn by a theme.</summary>
		public const int MaxCaptionActive = 1;
		/// <summary>Represents the state of a Window class MaxCaption part that can be drawn by a theme.</summary>
		public const int MaxCaptionInactive = 2;
		/// <summary>Represents the state of a Window class MaxCaption part that can be drawn by a theme.</summary>
		public const int MaxCaptionDisabled = 3;

		/// <summary>Represents the state of a Window class MinCaption part that can be drawn by a theme.</summary>
		public const int MinCaptionActive = 1;
		/// <summary>Represents the state of a Window class MinCaption part that can be drawn by a theme.</summary>
		public const int MinCaptionInactive = 2;
		/// <summary>Represents the state of a Window class MinCaption part that can be drawn by a theme.</summary>
		public const int MinCaptionDisabled = 3;

		/// <summary>Represents the state of a Window class HorzScroll part that can be drawn by a theme.</summary>
		public const int HorzScrollNormal = 1;
		/// <summary>Represents the state of a Window class HorzScroll part that can be drawn by a theme.</summary>
		public const int HorzScrollHot = 2;
		/// <summary>Represents the state of a Window class HorzScroll part that can be drawn by a theme.</summary>
		public const int HorzScrollPushed = 3;
		/// <summary>Represents the state of a Window class HorzScroll part that can be drawn by a theme.</summary>
		public const int HorzScrollDisabled = 4;

		/// <summary>Represents the state of a Window class HorzThumb part that can be drawn by a theme.</summary>
		public const int HorzThumbNormal = 1;
		/// <summary>Represents the state of a Window class HorzThumb part that can be drawn by a theme.</summary>
		public const int HorzThumbHot = 2;
		/// <summary>Represents the state of a Window class HorzThumb part that can be drawn by a theme.</summary>
		public const int HorzThumbPushed = 3;
		/// <summary>Represents the state of a Window class HorzThumb part that can be drawn by a theme.</summary>
		public const int HorzThumbDisabled = 4;

		/// <summary>Represents the state of a Window class VertScroll part that can be drawn by a theme.</summary>
		public const int VertScrollNormal = 1;
		/// <summary>Represents the state of a Window class VertScroll part that can be drawn by a theme.</summary>
		public const int VertScrollHot = 2;
		/// <summary>Represents the state of a Window class VertScroll part that can be drawn by a theme.</summary>
		public const int VertScrollPushed = 3;
		/// <summary>Represents the state of a Window class VertScroll part that can be drawn by a theme.</summary>
		public const int VertScrollDisabled = 4;

		/// <summary>Represents the state of a Window class VertThumb part that can be drawn by a theme.</summary>
		public const int VertThumbNormal = 1;
		/// <summary>Represents the state of a Window class VertThumb part that can be drawn by a theme.</summary>
		public const int VertThumbHot = 2;
		/// <summary>Represents the state of a Window class VertThumb part that can be drawn by a theme.</summary>
		public const int VertThumbPushed = 3;
		/// <summary>Represents the state of a Window class VertThumb part that can be drawn by a theme.</summary>
		public const int VertThumbDisabled = 4;

		/// <summary>Represents the state of a Window class SysButton part that can be drawn by a theme.</summary>
		public const int SysButtonNormal = 1;
		/// <summary>Represents the state of a Window class SysButton part that can be drawn by a theme.</summary>
		public const int SysButtonHot = 2;
		/// <summary>Represents the state of a Window class SysButton part that can be drawn by a theme.</summary>
		public const int SysButtonPushed = 3;
		/// <summary>Represents the state of a Window class SysButton part that can be drawn by a theme.</summary>
		public const int SysButtonDisabled = 4;

		/// <summary>Represents the state of a Window class MinButton part that can be drawn by a theme.</summary>
		public const int MinButtonNormal = 1;
		/// <summary>Represents the state of a Window class MinButton part that can be drawn by a theme.</summary>
		public const int MinButtonHot = 2;
		/// <summary>Represents the state of a Window class MinButton part that can be drawn by a theme.</summary>
		public const int MinButtonPushed = 3;
		/// <summary>Represents the state of a Window class MinButton part that can be drawn by a theme.</summary>
		public const int MinButtonDisabled = 4;

		/// <summary>Represents the state of a Window class MaxButton part that can be drawn by a theme.</summary>
		public const int MaxButtonNormal = 1;
		/// <summary>Represents the state of a Window class MaxButton part that can be drawn by a theme.</summary>
		public const int MaxButtonHot = 2;
		/// <summary>Represents the state of a Window class MaxButton part that can be drawn by a theme.</summary>
		public const int MaxButtonPushed = 3;
		/// <summary>Represents the state of a Window class MaxButton part that can be drawn by a theme.</summary>
		public const int MaxButtonDisabled = 4;

		/// <summary>Represents the state of a Window class RestoreButton part that can be drawn by a theme.</summary>
		public const int RestoreButtonNormal = 1;
		/// <summary>Represents the state of a Window class RestoreButton part that can be drawn by a theme.</summary>
		public const int RestoreButtonHot = 2;
		/// <summary>Represents the state of a Window class RestoreButton part that can be drawn by a theme.</summary>
		public const int RestoreButtonPushed = 3;
		/// <summary>Represents the state of a Window class RestoreButton part that can be drawn by a theme.</summary>
		public const int RestoreButtonDisabled = 4;

		/// <summary>Represents the state of a Window class HelpButton part that can be drawn by a theme.</summary>
		public const int HelpButtonNormal = 1;
		/// <summary>Represents the state of a Window class HelpButton part that can be drawn by a theme.</summary>
		public const int HelpButtonHot = 2;
		/// <summary>Represents the state of a Window class HelpButton part that can be drawn by a theme.</summary>
		public const int HelpButtonPushed = 3;
		/// <summary>Represents the state of a Window class HelpButton part that can be drawn by a theme.</summary>
		public const int HelpButtonDisabled = 4;

		/// <summary>Represents the state of a Window class CloseButton part that can be drawn by a theme.</summary>
		public const int CloseButtonNormal = 1;
		/// <summary>Represents the state of a Window class CloseButton part that can be drawn by a theme.</summary>
		public const int CloseButtonHot = 2;
		/// <summary>Represents the state of a Window class CloseButton part that can be drawn by a theme.</summary>
		public const int CloseButtonPushed = 3;
		/// <summary>Represents the state of a Window class CloseButton part that can be drawn by a theme.</summary>
		public const int CloseButtonDisabled = 4;
	}
	#endif
	
	#endregion
	
	#endregion
	
	/// <summary>
	/// Enumerates the names of various applications defined in the Windows Luna theme.
	/// </summary>
	#region sealed class ThemeApplication
	sealed class ThemeApplication
	{
		public const string ExplorerBar = "ExplorerBar";
		public const string PlacesBar = "Placesbar";
		public const string Start = "Start";
		public const string StartMenu = "StartMenu";
		public const string TaskBand = "TaskBand";
		public const string TaskBandGroupMenu = "TaskBandGroupMenu";
		public const string TaskBandVert = "TaskBandVert";
		public const string TaskBar = "TaskBar";
		public const string TaskBarVert = "TaskBarVert";
		public const string TrayNotify = "TrayNotify";
		public const string TrayNotifyHoriz = "TrayNotifyHoriz";
		public const string TrayNotifyHorizOpen = "TrayNotifyHorizOpen";
		public const string TrayNotifyVert = "TrayNotifyVert";
		public const string TrayNotifyVertOpen = "TrayNotifyVertOpen";
		public const string Shell = "Shell";
	}
	#endregion
	
	#region Theme Enums
	
	#region enum ThemeDrawFlags
	/// <summary>
	/// Specifies internal theme part drawing options for the <see cref="ThemeRenderer.Draw"/> method.
	/// </summary>
	enum ThemeDrawFlags
	{
		DrawSolid = 0x00000002,
		OmitBorder = 0x00000004,
		OmitContent = 0x00000008,
		MirrorDC = 0x00000020,
		UseDCOrigin = 0x00000040
	}
	#endregion
	
	#region enum ThemeColor
#if THEME_ALL || THEME_COLOR
	/// <summary>
	/// Specifies a color property that can be retrieved from a Windows theme.
	/// </summary>
	enum ThemeColor
	{
		/// <summary>
		/// The color of the border associated with this part and state.
		/// </summary>
		Border = 3801,
		/// <summary>
		/// The color of the background fill associated with this part and state.
		/// </summary>
		Fill = 3802,
		/// <summary>
		/// The color of the text associated with this part and state.
		/// </summary>
		Text = 3803,
		/// <summary>
		/// The light color of the edge associated with this part and state.
		/// </summary>
		EdgeLight = 3804,
		/// <summary>
		/// The highlight color of the edge associated with this part and state.
		/// </summary>
		EdgeHighlight = 3805,
		/// <summary>
		/// The shadow color of the edge associated with this part and state.
		/// </summary>
		EdgeShadow = 3806,
		/// <summary>
		/// The dark shadow color of the edge associated with this part and state.
		/// </summary>
		EdgeDarkShadow = 3807,
		/// <summary>
		/// The fill color of the edge associated with this part and state.
		/// </summary>
		EdgeFill = 3808,
#if THEME_COLOR_RARE
		/// <summary>
		/// The transparent color associated with this part and state. If the
		/// <see cref="ThemeBooleanProperty.Transparent">ThemePropertyBool.Transparent</see> value for this part and state is
		/// true (see <see cref="ThemeProperties.GetBoolean"/>), parts of the graphic that use this color will not be drawn.
		/// </summary>
		Transparent = 3809,
		/// <summary>
		/// The first color of the gradient associated with this part and state.
		/// </summary>
		Gradient1 = 3810,
		/// <summary>
		/// The second color of the gradient associated with this part and state.
		/// </summary>
		Gradient2 = 3811,
		/// <summary>
		/// The third color of the gradient associated with this part and state.
		/// </summary>
		Gradient3 = 3812,
		/// <summary>
		/// The fourth color of the gradient associated with this part and state.
		/// </summary>
		Gradient4 = 3813,
		/// <summary>
		/// The fifth color of the gradient associated with this part and state.
		/// </summary>
		Gradient5 = 3814,
#endif
		/// <summary>
		/// The color of the shadow drawn underneath text associated with this part and state.
		/// </summary>
		Shadow = 3815,
		/// <summary>
		/// The color of the glow produced by calling DrawThemeIcon using this part and state.
		/// </summary>
		Glow = 3816,
		/// <summary>
		/// The color of the text border associated with this part and state.
		/// </summary>
		TextBorder = 3817,
		/// <summary>
		/// The color of the text shadow associated with this part and state.
		/// </summary>
		TextShadow = 3818,
		/// <summary>
		/// The color that the font-based glyph associated with this part and state will use.
		/// </summary>
		GlyphText = 3819,
#if THEME_COLOR_RARE
		/// <summary>
		/// The transparent glyph color associated with this part and state. If the
		/// <see cref="ThemeBooleanProperty.Transparent">ThemePropertyBool.GlyphTransparent</see> value for this part and
		/// state is true (see <see cref="ThemeProperties.GetBoolean"/>), parts of the glyph that use this color will not be drawn.
		/// </summary>
		GlyphTransparent = 3820,
#endif
		/// <summary>
		/// The color used as a fill color hint for custom controls.
		/// </summary>
		FillHint = 3821,
		/// <summary>
		/// The color used as a border color hint for custom controls.
		/// </summary>
		BorderHint = 3822,
		/// <summary>
		/// The color used as an accent color hint for custom controls.
		/// </summary>
		AccentHint = 3823,
		/// <summary>
		/// The color used as a blend color.
		/// </summary>
		Blend = 3824
	}
	#endif
	#endregion
	
	#region enum ThemeMetric
	#if THEME_ALL || THEME_METRIC
	/// <summary>
	/// Specifies a type of metric property.
	/// </summary>
	enum ThemeMetric
	{
#if THEME_METRIC_RARE
		/// <summary>
		/// The number of state images present in an image file.
		/// </summary>
		ImageCount = 2401,
		/// <summary>
		/// The alpha value (0-255) used for DrawThemeIcon.
		/// </summary>
		AlphaLevel = 2402,
#endif
		/// <summary>
		/// The thickness of the border drawn if this part uses a border fill.
		/// </summary>
		BorderSize = 2403,
#if THEME_METRIC_RARE
		/// <summary>
		/// The roundness (0-100%) of the part's corners.
		/// </summary>
		RoundCornerWidth = 2404,
		/// <summary>
		/// The roundness (0-100%) of the part's corners.
		/// </summary>
		RoundCornerHeight = 2405,
		/// <summary>
		/// The amount of the first gradient color to use in drawing the part. This value can be from 0 to 255, but this value plus the values of each of the GradientRatio values must add up to 255. See the TMT_GRADIENTCOLOR1 value of GetThemeColor.
		/// </summary>
		GradientRatio1 = 2406,
		/// <summary>
		/// The amount of the second gradient color to use in drawing the part.
		/// </summary>
		GradientRatio2 = 2407,
		/// <summary>
		/// The amount of the third gradient color to use in drawing the part.
		/// </summary>
		GradientRatio3 = 2408,
		/// <summary>
		/// The amount of the fourth gradient color to use in drawing the part.
		/// </summary>
		GradientRatio4 = 2409,
		/// <summary>
		/// The amount of the fifth gradient color to use in drawing the part.
		/// </summary>
		GradientRatio5 = 2410,
#endif
		/// <summary>
		/// The size of the progress control "chunk" shapes that define how far an operation has progressed.
		/// </summary>
		ProgressChunkSize = 2411,
		/// <summary>
		/// The total size of all of the progress control "chunks".
		/// </summary>
		ProgressSpaceSize = 2412,
#if THEME_METRIC_RARE
		/// <summary>
		/// The amount of saturation (0-255) to apply to an icon drawn using DrawThemeIcon.
		/// </summary>
		Saturation = 2413,
#endif
		/// <summary>
		/// The thickness of the border drawn around text characters.
		/// </summary>
		TextBorderSize = 2414,
		/// <summary>
		/// The minimum alpha value (0-255) that a pixel must be to be considered opaque.
		/// </summary>
		AlphaThreshold = 2415,
		/// <summary>
		/// The width of the part.
		/// </summary>
		Width = 2416,
		/// <summary>
		/// The height of the part.
		/// </summary>
		Height = 2417
#if THEME_METRIC_RARE
		,
		/// <summary>
		/// The character index into the selected font that will be used for the glyph, if the part uses a font-based glyph.
		/// </summary>
		GlyphIndex = 2418,
		/// <summary>
		/// The percentage of a true-size image's original size at which the image will be stretched.
		/// </summary>
		TrueSizeStretchMark = 2419,
		/// <summary>
		/// The minimum dpi that the first image file was designed for.
		/// </summary>
		MinDpi1 = 2420,
		/// <summary>
		/// The minimum dpi that the second image file was designed for.
		/// </summary>
		MinDpi2 = 2421,
		/// <summary>
		/// The minimum dpi that the third image file was designed for.
		/// </summary>
		MinDpi3 = 2422,
		/// <summary>
		/// The minimum dpi that the fourth image file was designed for.
		/// </summary>
		MinDpi4 = 2423,
		/// <summary>
		/// The minimum dpi that the fifth image file was designed for.
		/// </summary>
		MinDpi5 = 2424
#endif
	}
	#endif
	#endregion
	
	#region enum ThemeFileName
	#if THEME_ALL || THEME_FILENAME
	/// <summary>
	/// Identifies the names of the image files that are used to draw a theme part.
	/// </summary>
	enum ThemeFileName
	{
		ImageFile = 3001,
		ImageFile1 = 3002,
		ImageFile2 = 3003,
		ImageFile3 = 3004,
		ImageFile4 = 3005,
		ImageFile5 = 3006,
		StockImageFile = 3007,
		GlyphImageFile = 3008
	}
	#endif
	#endregion
	
	#region enum ThemeMargin
	#if THEME_ALL || THEME_MARGIN
	/// <summary>
	/// Identifies the margin properties of a theme part.
	/// </summary>
	enum ThemeMargin
	{
		/// <summary>Specifies a margin used for nine-grid sizing.</summary>
		Sizing = 3601,
		/// <summary>Specifies where content can be placed.</summary>
		Content = 3602,
		/// <summary>Specifies where caption text can be placed.</summary>
		Caption = 3603
	}
	#endif
	#endregion
	
	#region enum ThemeTextFormatFlags
	#if THEME_ALL || THEME_TEXT
	/// <summary>
	/// Provides a set of flags which determine how text is rendered by a <see cref="ThemeRenderer"/>.
	/// </summary>
	enum ThemeTextFormatFlags
	{
		/// <summary>Aligns the text on the bottom of the bounding rectangle. Applied only when the text is a single line.</summary>
		Bottom = 8,
		/// <summary>Applies the default formatting, which is left-aligned.</summary>
		Default = 0,
		/// <summary>Removes the end of trimmed lines, and replaces them with an ellipsis.</summary>
		EndEllipsis = 0x8000,
		/// <summary>Expands tab characters. The default number of characters per tab is eight. The WordEllipsis, PathEllipsis, and EndEllipsis values cannot be used with ExpandTabs.</summary>
		ExpandTabs = 0x40,
		/// <summary>Includes the font external leading in line height. Typically, external leading is not included in the height of a line of text.</summary>
		ExternalLeading = 0x200,
		/// <summary>Adds padding to the bounding rectangle to accommodate overhanging glyphs.</summary>
		GlyphOverhangPadding = 0,
		/// <summary>
		/// Applies to Windows 2000 and Windows XP only: Ignores the ampersand (&amp;) prefix character in the text. The letter that follows will not be underlined,
		/// but other mnemonic-prefix characters are still processed.
		/// </summary>
		HidePrefix = 0x100000,
		/// <summary>Centers the text horizontally within the bounding rectangle.</summary>
		HorizontalCenter = 1,
		/// <summary>Uses the system font to calculate text metrics.</summary>
		Internal = 0x1000,
		/// <summary>Aligns the text on the left side of the clipping area.</summary>
		Left = 0,
		/// <summary>Adds padding to both sides of the bounding rectangle.</summary>
		LeftAndRightPadding = 0x20000000,
		/// <summary>Modifies the specified string to match the displayed text. This value has no effect unless EndEllipsis or PathEllipsis is also specified.</summary>
		ModifyString = 0x10000,
		/// <summary>Allows the overhanging parts of glyphs and unwrapped text reaching outside the formatting rectangle to show.</summary>
		NoClipping = 0x100,
		/// <summary>
		/// Applies to Windows 98, Windows Me, Windows 2000, or Windows XP only: Prevents a line break at a double-wide character string, so that the line-breaking
		/// rule is equivalent to single-wide character strings. This value has no effect unless WordBreak is also specified.
		/// </summary>
		NoFullWidthCharacterBreak = 0x80000,
		/// <summary>Does not add padding to the bounding rectangle.</summary>
		NoPadding = 0x10000000,
		/// <summary>
		/// Turns off processing of prefix characters. Typically, the ampersand (&&) mnemonic-prefix character is interpreted as a directive to underscore the
		/// character that follows, and the double-ampersand (&&) mnemonic-prefix characters as a directive to print a single ampersand. By specifying NoPrefix,
		/// this processing is turned off.
		/// </summary>
		NoPrefix = 0x800,
		/// <summary>Removes the center of trimmed lines and replaces it with an ellipsis.</summary>
		PathEllipsis = 0x4000,
		/// <summary>
		/// Applies to Windows 2000 or Windows XP only: Draws only an underline at the position of the character following the ampersand (&&) prefix character. Does
		/// not draw any other characters in the string.
		/// </summary>
		PrefixOnly = 0x200000,
//		PreserveGraphicsClipping = 0x1000000,
//		PreserveGraphicsTranslateTransform = 0x2000000,
		/// <summary>Aligns the text on the right side of the clipping area.</summary>
		Right = 2,
		/// <summary>Displays the text from right to left.</summary>
		RightToLeft = 0x20000,
		/// <summary>Displays the text in a single line.</summary>
		SingleLine = 0x20,
		/// <summary>Specifies the text should be formatted for display on a TextBox control.</summary>
		TextBoxControl = 0x2000,
		/// <summary>Aligns the text on the top of the bounding rectangle.</summary>
		Top = 0,
		/// <summary>Centers the text vertically, within the bounding rectangle.</summary>
		VerticalCenter = 4,
		/// <summary>Breaks the text at the end of a word.</summary>
		WordBreak = 0x10,
		/// <summary>Trims the line to the nearest word and an ellipsis is placed at the end of a trimmed line.</summary>
		WordEllipsis = 0x40000
	}
	#endif
	#endregion
	
	#endregion
}
