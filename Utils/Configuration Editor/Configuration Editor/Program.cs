using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Configuration_Editor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            EquationFragment.Setup();

            Application.Run(new FrmMain());
        }


        public static Random rnd = new Random();
        //public static Database SpecieDatabase;
        public const double ZeroK = -273.15;
    }
}