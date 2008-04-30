using System;
using System.Diagnostics;
using System.Windows;
using WPFApplication1;

namespace WPFTest
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