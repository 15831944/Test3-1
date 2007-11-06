using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Collections;
using System.Resources;
using System.Reflection;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Ipc;
using System.Threading;
using System.Runtime.Remoting;
using System.IO;
using System.Runtime.Remoting.Channels.Tcp;
using System.Runtime.Serialization.Formatters;

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
            startupMutexRequiresRelease = startupMutex.WaitOne(20000, false);
            Args = args;
            if (!SetupInterop())
            {
                if (startupMutexRequiresRelease)
                    startupMutex.ReleaseMutex();
                return;
            }
         
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

            Images.Images.Add("Warning", Reaction_Editor.Properties.Resources.Warning1);
            Images.Images.Add("Error", Reaction_Editor.Properties.Resources.Critical);
            Images.Images.Add("Info", Reaction_Editor.Properties.Resources.Information);

            foreach (string s in Args)
                if (s.ToLowerInvariant() == "/dyn" || s.ToLowerInvariant() == "/dynamic")
                {
                    Dynamic = true;
                    break;
                }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FrmMain());

            //The application responsible for the communication channel must not shut down:
            if (ResponsibleForChannel)
                interopMessenger.DoKeepAliveLoop();
        }

        public static ILog Log;
        public static ImageList Images = new ImageList();
        public static Random Rnd = new Random();
        public static FrmAutobalanceExtraComps FrmAutobalanceExtraComps;
        public static bool Dynamic = true;
        public static String[] Args;
        public static Dictionary<object, int> AutocompleteHitCounts = new Dictionary<object, int>();
        
        public static InteropMessenger interopMessenger;
        public static Mutex startupMutex = new Mutex(false, "KWA_ReactionEditor_Startup");
        public static bool startupMutexRequiresRelease;
        public static bool ResponsibleForChannel = false;
        
        private static bool SetupInterop()
        {
            IpcChannel ipcChannel = null;
            System.Runtime.Remoting.Channels.BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
            serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;
            BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

            try
            {
                Hashtable ipcProps = new Hashtable();
                ipcProps["portName"] = "SysCAD.ReactionEditor";
                ipcChannel = new IpcChannel(ipcProps, null, serverProv);
                ChannelServices.RegisterChannel(ipcChannel, false);
                interopMessenger = new InteropMessenger();
                RemotingServices.Marshal(interopMessenger, "interopMessenger");
                ResponsibleForChannel = true;
            }
            catch
            {
                try
                {
#if true           //Tests where the client would also register a channel...
                    Hashtable props = new Hashtable();
                    props["portName"] = "Remoting" + new Random().Next().ToString();
                    ipcChannel = new IpcChannel(props, null, serverProv);
                    ChannelServices.RegisterChannel(ipcChannel, false);
#endif

                    interopMessenger = Activator.GetObject(typeof(InteropMessenger), "ipc://SysCAD.ReactionEditor/interopMessenger") as InteropMessenger;
                    string directory = "";
                    foreach (string s in Args)
                        try  //Because GetDirectoryName has a few exceptions it likes to throw.
                        {
                            if (Directory.Exists(s) || File.Exists(s))
                            {
                                directory = Directory.Exists(s) ? s : 
                                    Path.GetDirectoryName(s);
                                if (directory == "")
                                    directory = Path.GetPathRoot(Path.GetDirectoryName(s));
                                break;
                            }
                        }
                        catch { }
                    bool alreadyOpen = Program.interopMessenger.AlreadyOpen(directory);
                    if (alreadyOpen)
                    {
                        interopMessenger.SendStrings(directory, Args);
                        return false;
                    }
                }
                catch (Exception ex) { Console.WriteLine(ex.ToString()); }
            }
            return true;
        }
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

        /*[DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern uint RegisterWindowMessage(string lpString);*/
    }

}