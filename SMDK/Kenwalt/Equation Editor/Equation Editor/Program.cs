using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Collections;
using System.Resources;
using System.Reflection;
using System.Drawing;
using System.Runtime.InteropServices;

namespace Reaction_Editor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Args = args;
            Assembly asm = Assembly.GetExecutingAssembly();
            ResourceReader rr = new ResourceReader(asm.GetManifestResourceStream(typeof(FrmMain), "Images.Icons.resources"));

            foreach (DictionaryEntry de in rr)
            {
                if (de.Value is Icon)
                    Images.Images.Add((string)de.Key, (Icon)de.Value);
                else if (de.Value is Image)
                    Images.Images.Add((string)de.Key, (Image)de.Value);
                else
                    Console.WriteLine("Image " + de.Key + " cannot be properly cast");
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FrmMain());
        }

        public static ImageList Images = new ImageList();
        public static Random Rnd = new Random();
        public static String[] Args;
    }

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

        [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern uint RegisterWindowMessage(string lpString);
    }
}