using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Collections;
using System.Resources;
using System.Reflection;
using System.Drawing;

namespace Reaction_Editor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Assembly asm = Assembly.GetExecutingAssembly();
            ResourceReader rr = new ResourceReader(asm.GetManifestResourceStream(typeof(FrmMain), "Icons.resources"));

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
    }
}