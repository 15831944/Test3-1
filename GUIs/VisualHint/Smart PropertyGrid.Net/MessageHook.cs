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

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal interface IMessageHookClient
    {
        bool OnMessageHooked(Control control, uint message, IntPtr wParam, IntPtr lParam);
    }

	[CLSCompliant(false)]
    public interface IMessageHookListener
    {
        bool OnListenMessageHook(Control control, uint message, IntPtr wParam, IntPtr lParam);
    }

    internal class MessageHook
    {
        // Fields
        private IMessageHookClient _client;
        private Control _control;
        private bool _hookDisable;
        private IntPtr _messageHookHandle = IntPtr.Zero;
        private GCHandle _messageHookRoot;
        private bool _processing;
        internal int thisProcessID;

        // Methods
        public MessageHook(Control control, IMessageHookClient client)
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
                if (_messageHookHandle != IntPtr.Zero)
                    return;

                if (thisProcessID == 0)
                    Win32Calls.GetWindowThreadProcessId(new HandleRef(_control, _control.Handle), out thisProcessID);

                MessageHookObject obj = new MessageHookObject(this);
                Win32Calls.HookProc msgHookProc = new Win32Calls.HookProc(obj.Callback);
                _messageHookRoot = GCHandle.Alloc(msgHookProc);

                _messageHookHandle = Win32Calls.SetWindowsHookEx(Win32Calls.WH_CALLWNDPROC, msgHookProc, (IntPtr)0, Win32Calls.GetCurrentThreadId());
            }
        }

        private IntPtr MessageHookProc(int nCode, IntPtr wParam, IntPtr lParam)
        {
            GC.KeepAlive(this);
            if (nCode >= 0)
            {
                Win32Calls.CWPSTRUCT msgHookStruct = (Win32Calls.CWPSTRUCT)Marshal.PtrToStructure(lParam, typeof(Win32Calls.CWPSTRUCT));
                if (ProcessMessage(msgHookStruct.hwnd, msgHookStruct.message, msgHookStruct.wParam, msgHookStruct.lParam))
                    return (IntPtr)1;
            }

            return Win32Calls.CallNextHookEx(new HandleRef(this, _messageHookHandle), nCode, wParam, lParam);
        }

        private bool ProcessMessage(IntPtr hWnd, uint message, IntPtr wParam, IntPtr lParam)
        {
            if (!_processing)
            {
                int processId;
                Win32Calls.GetWindowThreadProcessId(new HandleRef(null, hWnd), out processId);
                if (processId != thisProcessID)
                {
                    HookMessages = false;
                    return false;
                }

                try
                {
                    _processing = true;
                    if (_client.OnMessageHooked(Control.FromHandle(hWnd), message, wParam, lParam))
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
                if (_messageHookHandle != IntPtr.Zero)
                {
                    Win32Calls.UnhookWindowsHookEx(new HandleRef(this, _messageHookHandle));
                    _messageHookRoot.Free();
                    _messageHookHandle = IntPtr.Zero;
                }
            }
        }

        // Properties
        public bool DisableMessageHook
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
                return (_messageHookHandle != IntPtr.Zero);
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
        private class MessageHookObject
        {
            // Fields
            internal WeakReference _reference;

            // Methods
            public MessageHookObject(MessageHook parent)
            {
                _reference = new WeakReference(parent, false);
            }

            public virtual IntPtr Callback(int nCode, IntPtr wparam, IntPtr lparam)
            {
                IntPtr hookRet = IntPtr.Zero;
                try
                {
                    MessageHook hook = (MessageHook)_reference.Target;
                    if (hook != null)
                        hookRet = hook.MessageHookProc(nCode, wparam, lparam);
                }
                catch
                {
                }

                return hookRet;
            }
        }
    }
}
