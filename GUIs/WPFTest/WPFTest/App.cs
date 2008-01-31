using System;
using System.Diagnostics;
using System.Windows;

namespace WPFApplication1
{
    public class App : Application
    {
        [DebuggerNonUserCodeAttribute()]
        [STAThreadAttribute()]
        public static void Main()
        {
        Window1 w = new Window1();
        w.ShowDialog();
        }
    }

}