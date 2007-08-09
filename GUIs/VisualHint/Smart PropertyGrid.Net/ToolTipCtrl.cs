// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;

namespace VisualHint.SmartPropertyGrid
{
    [ToolboxItem(false)]
    internal class ToolTipCtrl : Control
    {
        [StructLayout(LayoutKind.Sequential)]
        internal class INITCOMMONCONTROLSEX
        {
            public int dwSize;
            public int dwICC;
            public INITCOMMONCONTROLSEX()
            {
                this.dwSize = 8;
            }
        }

        [DllImport("comctl32.dll")]
        public static extern bool InitCommonControlsEx(INITCOMMONCONTROLSEX icc);

        private Control _parent;

        private string _title = "";

        private Win32Calls.TOOLINFO _toolinfo;

        private PropertyEnumerator _enumerator;

        protected override CreateParams CreateParams
        {
            [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            get
            {
                INITCOMMONCONTROLSEX initcommoncontrolsex = new INITCOMMONCONTROLSEX();
                initcommoncontrolsex.dwICC = 8;
                InitCommonControlsEx(initcommoncontrolsex);
                CreateParams cp = new CreateParams();
                cp.Parent = IntPtr.Zero;
                cp.ClassName = "tooltips_class32";
                cp.Style |= 3;
                cp.ExStyle = 0;
                cp.Caption = null;
                return cp;
            }
        }

        public void SetText(string text, PropertyEnumerator enumerator)
        {
            if ((_title != text) || (_enumerator != enumerator))
            {
                _title = "";
                Win32Calls.SendMessage(Handle, Win32Calls.TTM_UPDATETIPTEXT, 0, GetTOOLINFO());

                _title = text;
                Win32Calls.SendMessage(Handle, Win32Calls.TTM_UPDATETIPTEXT, 0, GetTOOLINFO());
            }

            _enumerator = enumerator;
        }

        public ToolTipCtrl(Control parent)
        {
            _parent = parent;
            SetStyle(ControlStyles.UserPaint, false);
            Font = _parent.Font;

            if (_parent.IsHandleCreated)
            {
                Win32Calls.SetWindowPos(Handle, /*HWND_TOPMOST*/(IntPtr)(-1), 0, 0, 0, 0, 0x13);
                Win32Calls.SendMessage(Handle, Win32Calls.TTM_ADDTOOL, 0, GetTOOLINFO());
                Win32Calls.SendMessage(Handle, Win32Calls.TTM_SETMAXTIPWIDTH, 0, SystemInformation.MaxWindowTrackSize.Width);
  //              Win32Calls.SendMessage(Handle, Win32Calls.TTM_SETDELAYTIME, Win32Calls.TTDT_AUTOPOP, short.MaxValue);
//              Win32Calls.SendMessage(Handle, Win32Calls.TTM_SETDELAYTIME, Win32Calls.TTDT_INITIAL, 1000);
            }
        }

        public Win32Calls.TOOLINFO GetTOOLINFO()
        {
            if (_toolinfo == null)
            {
                _toolinfo = new Win32Calls.TOOLINFO();
                _toolinfo.uFlags = Win32Calls.TTF_IDISHWND | Win32Calls.TTF_SUBCLASS | Win32Calls.TTF_TRANSPARENT;
            }
            _toolinfo.lpszText = _title;
            _toolinfo.hwnd = _parent.Handle;
            _toolinfo.uId = _parent.Handle;
            return _toolinfo;
        }

        public void SetMargins(int left, int top, int right, int bottom)
        {
            Win32Calls.RECT rect = new Win32Calls.RECT(left, top, right, bottom);
            Win32Calls.SendMessage(Handle, Win32Calls.TTM_SETMARGIN, 0, ref rect);
        }

        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message msg)
        {
            if (msg.Msg == Win32Calls.WM_NCHITTEST)
            {
                msg.Result = (IntPtr)(-1);
                return;
            }

            base.WndProc(ref msg);
        }
    }
}
