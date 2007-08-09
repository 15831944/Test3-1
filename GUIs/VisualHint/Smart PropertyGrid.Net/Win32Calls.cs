// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <exclude />
    public class Win32Calls
    {
        #region Window messages
        public const int WM_ACTIVATE = 0x0006;
        public const int WM_SETFOCUS = 0x0007;
        public const int WM_KILLFOCUS = 0x0008;
        public const int WM_SETREDRAW = 0x000B;
        public const int WM_PAINT = 0x000F;
        public const int WM_MOUSEACTIVATE = 0x0021;
        public const int WM_NCCALCSIZE = 0x0083;
        public const int WM_NCPAINT = 0x0085;
        public const int WM_ERASEBKGND = 0x0014;
        public const int WM_NOTIFY = 0x004E;
        public const int WM_NCHITTEST = 0x0084;
        public const int WM_NCLBUTTONDOWN = 0x00A1;
        public const int EM_SETMARGINS = 0x00D3;
        public const int EM_GETMARGINS = 0x00D4;
        public const int WM_KEYDOWN = 0x0100;
        public const int WM_SYSKEYDOWN = 0x0104;
        public const int WM_KEYUP = 0x0101;
        public const int WM_CHAR = 0x0102;
        public const int WM_VSCROLL = 0x115;
        public const int WM_MOUSEMOVE = 0x0200;
        public const int WM_LBUTTONDOWN = 0x0201;
        public const int WM_LBUTTONDBLCLK = 0x0203;
        public const int WM_LBUTTONUP = 0x0202;
        public const int WM_RBUTTONDOWN = 0x0204;
        public const int WM_MBUTTONDOWN = 0x0207;
        public const int WM_MOUSEWHEEL = 0x20A;
        public const int WM_CUT = 0x300;
        public const int WM_COPY = 0x301;
        public const int WM_PASTE = 0x302;
        public const int WM_USER = 0x0400;
        public const int TTM_SETMAXTIPWIDTH = WM_USER + 24;
        public const int TTM_SETDELAYTIME = WM_USER + 3;
        public const int TTM_ADDTOOL = WM_USER + 50;
        public const int TTM_UPDATETIPTEXT = WM_USER + 57;
        public const int TTM_TRACKACTIVATE = WM_USER + 17;
        public const int TTM_TRACKPOSITION = WM_USER + 18;
        public const int TTM_SETMARGIN = WM_USER + 26;
        public const int TTM_SETTITLE = WM_USER + 32;
        #endregion

        public const int TTDT_AUTOMATIC = 0;
        public const int TTDT_RESHOW = 1;
        public const int TTDT_AUTOPOP = 2;
        public const int TTDT_INITIAL = 3;

        public const int TTF_IDISHWND = 0x0001;
        public const int TTF_SUBCLASS = 0x0010;
        public const int TTF_TRACK = 0x0020;
        public const int TTF_ABSOLUTE = 0x0080;
        public const int TTF_TRANSPARENT = 0x0100;

        public const int TTN_SHOW = -521;

        public const int EC_LEFTMARGIN = 0x0001;
        public const int EC_RIGHTMARGIN = 0x0002;
        public const int EC_USEFONTINFO = 0xffff;

        public const int GWL_EXSTYLE = -20;

        public const int WS_POPUP = unchecked((int)0x80000000);
        public const int WS_BORDER = 0x00800000;
        public const int WS_VSCROLL = 0x00200000;
        public const int WS_HSCROLL = 0x00100000;
        public const int WS_CHILD = 0x40000000;

        public const int WS_EX_TOOLWINDOW = 0x00000080;
        public const int WS_EX_TOPMOST = 0x00000008;
        public const int WS_EX_CLIENTEDGE = 0x00000200;

        public const int SW_SHOWNA = 8;

        public const int CS_DROPSHADOW = 0x00020000;

        [StructLayout(LayoutKind.Sequential)]
        public class NMHDR
        {
            public IntPtr hwndFrom;
            public IntPtr idFrom;
            public int code;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;

            public RECT(int left, int top, int right, int bottom)
            {
                Left = left; Top = top; Right = right; Bottom = bottom;
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SIZE
        {
            public int Width;
            public int Height;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct TEXTMETRIC
        {
            public int tmHeight;
            public int tmAscent;
            public int tmDescent;
            public int tmInternalLeading;
            public int tmExternalLeading;
            public int tmAveCharWidth;
            public int tmMaxCharWidth;
            public int tmWeight;
            public int tmOverhang;
            public int tmDigitizedAspectX;
            public int tmDigitizedAspectY;
            public byte tmFirstChar;
            public byte tmLastChar;
            public byte tmDefaultChar;
            public byte tmBreakChar;
            public byte tmItalic;
            public byte tmUnderlined;
            public byte tmStruckOut;
            public byte tmPitchAndFamily;
            public byte tmCharSet;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct NCCALCSIZE_PARAMS
        {
            public RECT rgrc0, rgrc1, rgrc2;
            public IntPtr lppos;
        }

        [DllImport("user32.dll")]
        public static extern bool GetUpdateRect(IntPtr hWnd, out RECT rect, bool bErase);

        [DllImport("user32.dll")]
        public static extern int GetWindowThreadProcessId(HandleRef hWnd, out int lpdwProcessId);

        [DllImport("user32.dll")]
        public static extern int MsgWaitForMultipleObjects(int nCount, IntPtr pHandles, bool fWaitAll, int dwMilliseconds, int dwWakeMask);

        [DllImport("user32.dll")]
        public static extern bool ShowWindow(HandleRef hWnd, int nCmdShow);

        public const int RDW_INVALIDATE = 0x0001;
        public const int RDW_INTERNALPAINT = 0x0002;
        public const int RDW_ERASE = 0x0004;
        public const int RDW_VALIDATE = 0x0008;
        public const int RDW_NOINTERNALPAINT = 0x0010;
        public const int RDW_NOERASE = 0x0020;
        public const int RDW_NOCHILDREN = 0x0040;
        public const int RDW_ALLCHILDREN = 0x0080;
        public const int RDW_UPDATENOW = 0x0100;
        public const int RDW_ERASENOW = 0x0200;
        public const int RDW_FRAME = 0x0400;
        public const int RDW_NOFRAME = 0x0800;

        [DllImport("user32.dll")]
        public static extern int RedrawWindow(IntPtr hwnd, IntPtr rect, IntPtr hrgnUpdate, int fuRedraw);

        [DllImport("user32.dll")]
        public extern static IntPtr GetWindowDC(IntPtr hWnd);

        [DllImport("user32.dll")]
        public extern static IntPtr WindowFromPoint(POINT Point);

        [DllImport("user32.dll")]
        public static extern IntPtr SetFocus(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern IntPtr GetFocus();

        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll")]
        public static extern int SetForegroundWindow(IntPtr handle);

        [DllImport("user32.dll")]
        public extern static int ReleaseDC(IntPtr hWnd, IntPtr hDC);

        [DllImport("user32.dll")]
        public static extern int GetWindowRect(IntPtr hwnd, ref RECT rc);

        public const int GWL_STYLE = (-16);

        [DllImport("user32.dll")]
        public static extern int GetWindowLong(IntPtr hWnd, int nItem);

        [DllImport("user32.dll")]
        public static extern IntPtr SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        [DllImport("user32.dll")]
        public static extern IntPtr GetWindow(IntPtr hWnd, int uCmd);

        [DllImport("user32.dll")]
        public static extern bool IsChild(IntPtr hWndParent, IntPtr hwnd);

        [DllImport("kernel32.dll")]
        public static extern int GetCurrentThreadId();
        
        [DllImport("user32.dll")]
        [CLSCompliant(false)]
        public static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint dwData, IntPtr dwExtraInfo);

        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_MOVE = 0x00000001;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_LEFTDOWN = 0x00000002;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_LEFTUP = 0x00000004;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_RIGHTDOWN = 0x00000008;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_RIGHTUP = 0x00000010;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_MIDDLEDOWN = 0x00000020;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_MIDDLEUP = 0x00000040;
        [CLSCompliant(false)]
        public const UInt32 MOUSEEVENTF_ABSOLUTE = 0x00008000;

        [DllImport("user32.dll")]
        [CLSCompliant(false)]
        public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);

        public const byte VK_RETURN = 0x0D;
        public const byte VK_MENU = 0x12;
        public const byte VK_UP = 0x26;
        public const byte VK_DOWN = 0x28;
        public const byte VK_DELETE = 0x2E;
        public const byte VK_F4 = 0x73;

        [DllImport("user32.dll", CharSet = CharSet.Auto, EntryPoint = "DrawText")]
        public static extern int DrawTextA(IntPtr hdc, string lpStr, int nCount, ref RECT lpRect, int wFormat);

        public const int DT_LEFT            = 0x00000000;
        public const int DT_TOP             = 0x00000000;
        public const int DT_CENTER          = 0x00000001;
        public const int DT_RIGHT           = 0x00000002;
        public const int DT_VCENTER         = 0x00000004;
        public const int DT_WORDBREAK       = 0x00000010;
        public const int DT_SINGLELINE      = 0x00000020;
        public const int DT_CALCRECT        = 0x00000400;
        public const int DT_NOPREFIX        = 0x00000800;
        public const int DT_EDITCONTROL     = 0x00002000;
        public const int DT_END_ELLIPSIS    = 0x00008000;
        public const int DT_HIDEPREFIX      = 0x00100000;

        private Hashtable _fonts = new Hashtable();
//        private Dictionary<Font, IntPtr> _fonts = new Dictionary<Font, IntPtr>();

        private static Win32Calls _instance = null;

        private bool disposed = false;
        protected virtual void Dispose(bool disposing) 
        {
            if (!disposed) 
            {
                // Dispose of resources held by this instance.
                foreach(IntPtr hFont in _fonts.Values)
                    DeleteObject(hFont);

                // Set the sentinel.
                disposed = true;
   
                // Suppress finalization of this disposed instance.
                if (disposing)
                {
                    GC.SuppressFinalize(this);
                }
            }
        }

        public void Dispose()
        {
            Dispose(true);
        }

        // Disposable types implement a finalizer.
        ~Win32Calls()
        {
            Dispose(false);
        }

        // DrawText wrapper
        public static int DrawText(Graphics graphics, string text, ref Rectangle rect, Font font, Color color, int format)
        {
            if (_instance == null)
                _instance = new Win32Calls();

            IntPtr hFont;
            object value = _instance._fonts[font];
            if (value == null)
            {
                value = font.ToHfont();
                _instance._fonts.Add(font, value);
            }
            hFont = (IntPtr)value;

            RECT rc = new RECT();
            rc.Left = rect.Left; rc.Right = rect.Right; rc.Top = rect.Top; rc.Bottom = rect.Bottom;
            IntPtr hDC = graphics.GetHdc();
            IntPtr oldHFont = SelectObject(hDC, hFont);
            ColorRef oldColor = SetTextColor(hDC, new ColorRef(color));
            SetBkMode(hDC, TRANSPARENT);
            int result = DrawTextA(hDC, text, text.Length, ref rc, format);
            if ((format & DT_CALCRECT) != 0)
            {
                rect.X = rc.Left;
                rect.Y = rc.Top;
                rect.Width = rc.Right - rc.Left;
                rect.Height = rc.Bottom - rc.Top;
            }
            SetTextColor(hDC, oldColor);
            SelectObject(hDC, oldHFont);
            graphics.ReleaseHdc(hDC);

            return result;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ColorRef
        {
            public byte red, green, blue;
            private byte unused;

            public ColorRef(Color color)
            {
                red = color.R;
                green = color.G;
                blue = color.B;
                unused = 0;
            }
        }

        [DllImport("gdi32.dll")]
        public static extern ColorRef SetTextColor(IntPtr hDC, ColorRef color);

        [DllImport("gdi32.dll")]
        public static extern int SetBkMode(IntPtr hDC, int mode);

        public const int TRANSPARENT = 1;

        [DllImport("gdi32.dll")]
        public static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiObj);

        [DllImport("gdi32.dll", EntryPoint = "GetTextExtentPoint32", CharSet = CharSet.Auto)]
        public static extern IntPtr GetTextExtentPoint32A(IntPtr hdc, string lpStr, int nCount, ref SIZE lpSize);

        // GetTextExtent wrapper
        public static Size GetTextExtent(Graphics graphics, string text, Font font)
        {
            IntPtr hDC = graphics.GetHdc();
            IntPtr hFont = font.ToHfont();
            IntPtr oldHFont = SelectObject(hDC, hFont);
            SIZE size = new SIZE();
            GetTextExtentPoint32A(hDC, text, text.Length, ref size);
            int width = size.Width;

            // Add the overhang for italic and bold fonts
            if ((font.Bold || font.Italic) && (text.Length > 0))
            {
                Win32Calls.ABCFLOAT[] WidthsABC = new Win32Calls.ABCFLOAT[1];
                uint code = text[text.Length - 1];
                Win32Calls.GetCharABCWidthsFloat(hDC, code, code, WidthsABC);
                double dOverhangTrailing = WidthsABC[0].abcfC;
                if (dOverhangTrailing < 0)
                    width -= (int)dOverhangTrailing;
            }

            SelectObject(hDC, oldHFont);
            DeleteObject(hFont);
            graphics.ReleaseHdc(hDC);

            return new Size(width, size.Height);
        }

        [DllImport("gdi32.dll")]
        static extern bool GetCharABCWidthsFloat(IntPtr hdc, uint iFirstChar, uint iLastChar, [Out] ABCFLOAT[] lpABCF);

        [StructLayout(LayoutKind.Sequential)]
        struct ABCFLOAT
        {
            /// <summary>Specifies the A spacing of the character. The A spacing is the distance to add to the current
            /// position before drawing the character glyph.</summary>
            public float abcfA;
            /// <summary>Specifies the B spacing of the character. The B spacing is the width of the drawn portion of
            /// the character glyph.</summary>
            public float abcfB;
            /// <summary>Specifies the C spacing of the character. The C spacing is the distance to add to the current
            /// position to provide white space to the right of the character glyph.</summary>
            public float abcfC;
        }

        [DllImport("gdi32.dll")]
        public static extern int DeleteObject(IntPtr hObject);

        [DllImport("gdi32.dll", EntryPoint = "GetTextMetrics")]
        public static extern int GetTextMetricsA(IntPtr hDC, ref TEXTMETRIC tm);

        // GetTextMetrics wrapper
        public static void GetTextMetrics(Graphics graphics, Font font, ref TEXTMETRIC tm)
        {
            IntPtr hDC = graphics.GetHdc();
            IntPtr hFont = font.ToHfont();
            IntPtr oldHFont = SelectObject(hDC, hFont);
            GetTextMetricsA(hDC, ref tm);
            SelectObject(hDC, oldHFont);
            DeleteObject(hFont);
            graphics.ReleaseHdc(hDC);
        }

        [DllImport("gdi32.dll")]
        public static extern IntPtr CreateRectRgnIndirect(ref RECT lprc);

        [DllImport("gdi32.dll")]
        public static extern int SelectClipRgn(IntPtr hdc, IntPtr hrgn);

        static private IntPtr _currentClipRegion;

        public static void SetClippingRect(Graphics graphics, Rectangle rect)
        {
            RECT clipRect = new RECT(rect.Left, rect.Top, rect.Right, rect.Bottom);
            _currentClipRegion = Win32Calls.CreateRectRgnIndirect(ref clipRect);
            IntPtr hDC = graphics.GetHdc();
            Win32Calls.SelectClipRgn(hDC, _currentClipRegion);
            graphics.ReleaseHdc(hDC);

            graphics.SetClip(rect);
        }

        public static void ResetClippingRect(Graphics graphics)
        {
            IntPtr hDC = graphics.GetHdc();
            Win32Calls.SelectClipRgn(hDC, IntPtr.Zero);
            DeleteObject(_currentClipRegion);
            graphics.ReleaseHdc(hDC);

            graphics.ResetClip();
        }

        public delegate IntPtr HookProc(int nCode, IntPtr wParam, IntPtr lParam);

        public const int WH_KEYBOARD = 2;
        public const int WH_CALLWNDPROC = 4;
        public const int WH_MOUSEPROC = 7;

        [StructLayout(LayoutKind.Sequential)]
        [CLSCompliant(false)]
        public struct CWPSTRUCT
        {
            public IntPtr lParam;
            public IntPtr wParam;
            public uint message;
            public IntPtr hwnd;
        }

        [StructLayout(LayoutKind.Sequential)]
        [CLSCompliant(false)]
        public struct CMHSTRUCT
        {
            public Point pt;
            public IntPtr hwnd;
            public uint wHitTestCode;
            public uint dwExtraInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct POINT
        {
            public int x;
            public int y;
        }

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern int MapWindowPoints(IntPtr hwndFrom, IntPtr hwndTo, ref POINT lpPoints, int cPoints);

        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr SetWindowsHookEx(int idHook, HookProc lpfn, IntPtr hInstance, int threadId);

        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern bool UnhookWindowsHookEx(HandleRef hhook);

        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr CallNextHookEx(HandleRef hhook, int code, IntPtr wparam, IntPtr lparam);

        #region Scrollbar functions
        public const int SB_BOTH = 3;
        public const int SB_BOTTOM = 7;
        public const int SB_CTL = 2;
        public const int SB_ENDSCROLL = 8;
        public const int SB_HORZ = 0;
        public const int SB_LEFT = 6;
        public const int SB_LINEDOWN = 1;
        public const int SB_LINELEFT = 0;
        public const int SB_LINERIGHT = 1;
        public const int SB_LINEUP = 0;
        public const int SB_PAGEDOWN = 3;
        public const int SB_PAGELEFT = 2;
        public const int SB_PAGERIGHT = 3;
        public const int SB_PAGEUP = 2;
        public const int SB_RIGHT = 7;
        public const int SB_THUMBPOSITION = 4;
        public const int SB_THUMBTRACK = 5;
        public const int SB_TOP = 6;
        public const int SB_VERT = 1;

        public const int SIF_RANGE = 0x1;
        public const int SIF_PAGE = 0x2;
        public const int SIF_POS = 0x4;
        public const int SIF_DISABLENOSCROLL = 0x8;
        public const int SIF_TRACKPOS = 0x10;
        public const int SIF_ALL = (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS);

        public const int ESB_ENABLE_BOTH = 0x0;
        public const int ESB_DISABLE_BOTH = 0x3;
        public const int ESB_DISABLE_DOWN = 0x2;
        public const int ESB_DISABLE_UP = 0x1;

        public const int SBS_SIZEGRIP = 0x10;

        // Mouse wheel stuff:
        public const int WHEEL_DELTA = 120;
        public const int WHEEL_PAGESCROLL = -1;
        public const int SPI_GETWHEELSCROLLLINES = 0x68;

        [StructLayout(LayoutKind.Sequential)]
        public struct SCROLLINFO
        {
            public int cbSize;
            public int fMask;
            public int nMin;
            public int nMax;
            public int nPage;
            public int nPos;
            public int nTrackPos;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public class TOOLINFO
        {
            public int cbSize;
            public int uFlags;
            public IntPtr hwnd;
            public IntPtr uId;
            public RECT rect;
            public IntPtr hinst;
            public string lpszText;
            public IntPtr lParam;
            public TOOLINFO()
            {
                cbSize = Marshal.SizeOf(typeof(TOOLINFO));
                uFlags = 0;
                hinst = IntPtr.Zero;
                lParam = IntPtr.Zero;
            }
        }

        [DllImport("user32.dll")]
        public static extern int ShowScrollBar(IntPtr hWnd, int wBar, bool bShow);

        [DllImport("user32.dll")]
        public static extern int EnableScrollBar(IntPtr hWnd, int wBar, int wArrows);

        [DllImport("user32.dll")]
        public static extern int IsScrollbarVisible(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern int SetScrollInfo(IntPtr hWnd, int wBar, ref SCROLLINFO lpsi, bool fRedraw);

        [DllImport("user32.dll")]
        public static extern int GetScrollInfo(IntPtr hwnd, int n, ref SCROLLINFO lpScrollInfo);

        [DllImport("user32.dll")]
        public static extern int GetScrollPos(IntPtr hWnd, int nBar);

        [DllImport("user32.dll")]
        public static extern int GetScrollRange(IntPtr hWnd, int nBar, ref int lpMinPos, ref int lpMaxPos);

        [DllImport("user32.dll")]
        public static extern int SetScrollPos(IntPtr hWnd, int nBar, int nPos, bool bRedraw);

        [DllImport("user32.dll")]
        public static extern int SetScrollRange(IntPtr hWnd, int nBar, int nMinPos, int nMaxPos, int bRedraw);
        #endregion

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, int lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, TOOLINFO lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, ref RECT lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, string lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern int PostMessage(IntPtr hWnd, int wMsg, int wParam, int lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern bool PeekMessage(out MSG msg, HandleRef hwnd, int msgMin, int msgMax, int remove);

        [Serializable, StructLayout(LayoutKind.Sequential)]
        public struct MSG
        {
            public IntPtr hwnd;
            public int message;
            public IntPtr wParam;
            public IntPtr lParam;
            public int time;
            public int pt_x;
            public int pt_y;
        }
        
        public static void RemovePendingMessages(HandleRef handle, int msgMin, int msgMax)
        {
            MSG msg = new MSG();
            while (PeekMessage(out msg, handle, msgMin, msgMax, 1))
            {
            }
        }

        public static bool ModifyStyleEx(IntPtr hWnd, int remove, int add, int flags)
        {
            int style = GetWindowLong(hWnd, GWL_EXSTYLE);
            int newStyle = (style & ~remove) | add;
	        if (style == newStyle)
		        return false;

	        SetWindowLong(hWnd, GWL_EXSTYLE, newStyle);
	        if (flags != 0)
	        {
		        SetWindowPos(hWnd, IntPtr.Zero, 0, 0, 0, 0,
			        SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | flags);
	        }

            return true;
        }

        public static int MakeLong(int LoWord, int HiWord)
        { 
            return (HiWord << 16) | (LoWord & 0xffff); 
        } 
 
        public static IntPtr MakeLParam(int LoWord, int HiWord) 
        { 
            return (IntPtr) ((HiWord << 16) | (LoWord & 0xffff)); 
        } 
         
        public static int HiWord(int doubleword) 
        { 
            return (short)((doubleword >> 16) & 0xffff);
        } 
         
        public static int LoWord(int doubleword) 
        { 
            return (short)(doubleword & 0xffff);
        }

        [DllImport("user32.dll")]
        public static extern short GetKeyState(int virtualKeyCode);

        public const int SWP_NOSIZE = 0x0001;
        public const int SWP_NOMOVE = 0x0002;
        public const int SWP_NOZORDER = 0x0004;
        public const int SWP_NOREDRAW = 0x0008;
        public const int SWP_NOACTIVATE = 0x0010;
        public const int SWP_FRAMECHANGED = 0x0020;
        public const int SWP_SHOWWINDOW = 0x0040;
        public const int SWP_HIDEWINDOW = 0x0080;
        public const int SWP_NOCOPYBITS = 0x0100;
        public const int SWP_NOOWNERZORDER = 0x0200;
        public const int SWP_NOSENDCHANGING = 0x0400;
        public const int SWP_DRAWFRAME = SWP_FRAMECHANGED;
        public const int SWP_NOREPOSITION = SWP_NOOWNERZORDER;

        [DllImport("user32.dll")]
        public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int cx, int cy, int flags);

#if !_DOTNET2
        [DllImport("comctl32.dll")]
        [CLSCompliant(false)]
        public static extern bool ImageList_DrawEx(IntPtr himl, int index, IntPtr hDestDC, int x, int y, int dx, int dy, uint rgbBk, uint rgbFg, int fStyle);
#endif

        public static void ImageListDraw(ImageList il, int index, Graphics graphics, int x, int y, int dx, int dy)
        {
#if _DOTNET2
            il.Draw(graphics, x, y, dx, dy, index);
#else
            // In .Net 1.1, ImageList.Draw does not draw PNG images correctly
            IntPtr hdc = graphics.GetHdc();
            ImageList_DrawEx(il.Handle, index, hdc, x, y, dx, dy, 0xFFFFFFFF, 0xFF000000, 0);
            graphics.ReleaseHdc(hdc);
#endif
        }
    }
}
