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
    internal interface IKeyboardHookClient
    {
        bool OnKeyboardHooked(int message, KeyEventArgs args);
    }

    internal class KeyboardHook
    {
        // Fields
        private IKeyboardHookClient _client;
        private Control _control;
        private bool _hookDisable;
        private IntPtr _keyboardHookHandle = IntPtr.Zero;
        private GCHandle _keyboardHookRoot;
        private bool _processing;
//        private int _thisProcessID;

        // Methods
        public KeyboardHook(Control control, IKeyboardHookClient client)
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
                if (_keyboardHookHandle != IntPtr.Zero)
                    return;

//                if (_thisProcessID == 0)
  //                  Win32Calls.GetWindowThreadProcessId(new HandleRef(_control, _control.Handle), out _thisProcessID);

                KeyboardHookObject obj = new KeyboardHookObject(this);
                Win32Calls.HookProc keyboardHookProc = new Win32Calls.HookProc(obj.Callback);
                _keyboardHookRoot = GCHandle.Alloc(keyboardHookProc);

                _keyboardHookHandle = Win32Calls.SetWindowsHookEx(Win32Calls.WH_KEYBOARD, keyboardHookProc, (IntPtr)0, Win32Calls.GetCurrentThreadId());
            }
        }

        private IntPtr KeyboardHookProc(int nCode, IntPtr wParam, IntPtr lParam)
        {
            GC.KeepAlive(this);
            if (nCode == 0) // HC_ACTION
            {
                if (ProcessMessage(wParam, lParam))
                    return (IntPtr)1;
            }

            return Win32Calls.CallNextHookEx(new HandleRef(this, _keyboardHookHandle), nCode, wParam, lParam);
        }

        private bool ProcessMessage(IntPtr wParam, IntPtr lParam)
        {
            if (!_processing)
            {
/*                int processId;
                Win32Calls.GetWindowThreadProcessId(new HandleRef(null, hWnd), out processId);
                if (processId != _thisProcessID)
                {
                    HookMessages = false;
                    return false;
                }
                */
                try
                {
                    _processing = true;
                    int message = (((int)lParam & (1 << 31)) != 0 ? Win32Calls.WM_KEYUP : Win32Calls.WM_KEYDOWN);
                    if (_client.OnKeyboardHooked(message, new KeyEventArgs(((Keys)((int)((long)wParam))) | Control.ModifierKeys)))
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
                if (_keyboardHookHandle != IntPtr.Zero)
                {
                    Win32Calls.UnhookWindowsHookEx(new HandleRef(this, _keyboardHookHandle));
                    _keyboardHookRoot.Free();
                    _keyboardHookHandle = IntPtr.Zero;
                }
            }
        }

        // Properties
        public bool DisableKeyboardHook
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
                return (_keyboardHookHandle != IntPtr.Zero);
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
        private class KeyboardHookObject
        {
            // Fields
            internal WeakReference _reference;

            // Methods
            public KeyboardHookObject(KeyboardHook parent)
            {
                _reference = new WeakReference(parent, false);
            }

            public virtual IntPtr Callback(int nCode, IntPtr wparam, IntPtr lparam)
            {
                IntPtr hookRet = IntPtr.Zero;
                try
                {
                    KeyboardHook hook = (KeyboardHook)_reference.Target;
                    if (hook != null)
                        hookRet = hook.KeyboardHookProc(nCode, wparam, lparam);
                }
                catch
                {
                }

                return hookRet;
            }
        }
    }
}
