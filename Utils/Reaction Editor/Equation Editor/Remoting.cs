using System;
using System.Collections.Generic;
using System.Threading;

namespace Reaction_Editor
{
    class InteropMessenger : MarshalByRefObject
    {
        private Dictionary<String, StringHandler> OpenDirectories = new Dictionary<string,StringHandler>();

        public void RegisterDirectory(string dir, StringHandler callBack)
        {
            OpenDirectories.Add(dir, callBack);
            StringSent += OpenDirectories[dir];
        }

        public void UnRegisterDirectory(string dir)
        {
            if (dir != null && OpenDirectories.ContainsKey(dir))
            {
                StringSent -= OpenDirectories[dir];
                OpenDirectories.Remove(dir);
            }
        }

        public bool AlreadyOpen(string dir)
        {
            string d = dir.ToLower();
            foreach (string s in OpenDirectories.Keys)
                if (d == s.ToLower())
                    return true;
            return false;
        }

        public override object InitializeLifetimeService()
        {
            return null;
        }

        public void SendStrings(string dir, string[] s)
        {
            if (StringSent != null)
                StringSent(dir, s);
        }

        private event StringHandler StringSent;

        public void DoKeepAliveLoop()
        {
            while (OpenDirectories.Count > 0)
                Thread.Sleep(1000);
        }
    }

    public delegate void StringHandler(string dir, string[] args);

    public class StringShim : MarshalByRefObject
    {
        private event StringHandler m_target;

        private StringShim(StringHandler callback)
        {
            m_target += callback;
        }

        public void DoInvoke(string dir, string[] args)
        {
            m_target(dir, args);
        }

        public static StringHandler Create(StringHandler target)
        {
            StringShim shim = new StringShim(target);
            return new StringHandler(shim.DoInvoke);
        }
    }
}