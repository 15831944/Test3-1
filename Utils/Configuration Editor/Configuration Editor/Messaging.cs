using System.Runtime.InteropServices;
using System;
namespace Messaging
{
    public class Messaging
    {
        public const int WM_MDIACTIVATE = 0x0222;
        public const int WM_NCACTIVATE = 0x0086;
        public const int WM_SETREDRAW = 0X000B;
        public const int WM_KEYDOWN = 0x0100;
        public const int WM_CHAR = 0x0102;
        public const int WM_KILLFOCUS = 0x0008;
        public const int WM_ACTIVATE = 0x0006;
        public const int WM_ACTIVATEAPP = 0x001C;
        public const int WM_PAINT = 0x000F;
        public const int WM_LBUTTONDOWN = 0x0201;

        public const int WA_INACTIVE = 0;
        /*[DllImport("user32.dll")]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);*/

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr
        wParam, IntPtr lParam);

        /*[DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern uint RegisterWindowMessage(string lpString);*/
    }
}