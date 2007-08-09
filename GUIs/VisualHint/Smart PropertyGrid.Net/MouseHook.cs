// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal interface IMouseHookClient
    {
        bool OnMouseHooked(IntPtr hWnd, uint message, Point pt, uint hitTest);
    }

    internal class MouseHook
    {
        // Fields
        private IMouseHookClient _client;
        private Control _control;
        private bool _hookDisable;
        private IntPtr _mouseHookHandle = IntPtr.Zero;
        private GCHandle _mouseHookRoot;
        private bool _processing;
        private int _thisProcessID;

        // Methods
        public MouseHook(Control control, IMouseHookClient client)
        {
            _control = control;
            _client = client;
        }

        public void Dispose()
        {
            StopHookMessages();
        }

        private void StartHookMessages()
        {
            GC.KeepAlive(this);
            lock(this)
            {
                if (_mouseHookHandle != IntPtr.Zero)
                    return;

                if (_thisProcessID == 0)
                    Win32Calls.GetWindowThreadProcessId(new HandleRef(_control, _control.Handle), out _thisProcessID);

                MouseHookObject obj = new MouseHookObject(this);
                Win32Calls.HookProc mouseHookProc = new Win32Calls.HookProc(obj.Callback);
                _mouseHookRoot = GCHandle.Alloc(mouseHookProc);

                _mouseHookHandle = Win32Calls.SetWindowsHookEx(Win32Calls.WH_MOUSEPROC, mouseHookProc, (IntPtr)0, Win32Calls.GetCurrentThreadId());
            }
        }

        private IntPtr MouseHookProc(int nCode, IntPtr wParam, IntPtr lParam)
        {
            GC.KeepAlive(this);
            if (nCode == 0) // HC_ACTION
            {
                Win32Calls.CMHSTRUCT mouseHookStruct = (Win32Calls.CMHSTRUCT)Marshal.PtrToStructure(lParam, typeof(Win32Calls.CMHSTRUCT));
                if (ProcessMessage(mouseHookStruct.hwnd, (uint)wParam, mouseHookStruct.pt, mouseHookStruct.wHitTestCode))
                    return (IntPtr)1;
            }

            return Win32Calls.CallNextHookEx(new HandleRef(this, _mouseHookHandle), nCode, wParam, lParam);
        }

        private bool ProcessMessage(IntPtr hWnd, uint message, Point pt, uint hitTest)
        {
            if (!_processing)
            {
                int processId;
                Win32Calls.GetWindowThreadProcessId(new HandleRef(null, hWnd), out processId);
                if (processId != _thisProcessID)
                {
                    HookMessages = false;
                    return false;
                }

                try
                {
                    _processing = true;
                    if (_client.OnMouseHooked(hWnd, message, pt, hitTest))
                        return true;
                }
                finally
                {
                    _processing = false;
                }
            }

            return false;
        }

        private void StopHookMessages()
        {
            GC.KeepAlive(this);
            lock (this)
            {
                if (_mouseHookHandle != IntPtr.Zero)
                {
                    Win32Calls.UnhookWindowsHookEx(new HandleRef(this, _mouseHookHandle));
                    _mouseHookRoot.Free();
                    _mouseHookHandle = IntPtr.Zero;
                }
            }
        }

        // Properties
        public bool DisableMouseHook
        {
            get { return _hookDisable; }
            set
            {
                _hookDisable = value;
                if (value)
                    StopHookMessages();
            }
        }

        public virtual bool HookMessages
        {
            get
            {
                GC.KeepAlive(this);
                return (_mouseHookHandle != IntPtr.Zero);
            }
            set
            {
                if (value && !_hookDisable)
                    StartHookMessages();
                else
                    StopHookMessages();
            }
        }

        // Nested Types
        private class MouseHookObject
        {
            // Fields
            internal WeakReference _reference;

            // Methods
            public MouseHookObject(MouseHook parent)
            {
                _reference = new WeakReference(parent, false);
            }

            public virtual IntPtr Callback(int nCode, IntPtr wparam, IntPtr lparam)
            {
                IntPtr hookRet = IntPtr.Zero;
                try
                {
                    MouseHook hook = (MouseHook)_reference.Target;
                    if (hook != null)
                        hookRet = hook.MouseHookProc(nCode, wparam, lparam);
                }
                catch
                {
                }

                return hookRet;
            }
        }
    }
}
