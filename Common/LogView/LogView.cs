using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Resources;
using System.Reflection;
using System.Collections;
using System.IO;

namespace SysCAD.Log
{
  public partial class LogView : ListView, ILog
  {
    protected Stack<MessageSource> sourceStack = new Stack<MessageSource>();

    private int maxEntries = 1000;
    private string logFile = String.Empty;

    public string LogFile
    {
      get { return logFile; }
      set { logFile = value; }
    }
    private StreamWriter logStreamWriter;

    public LogView()
    {
      Initialize();
    }

    public LogView(int maxEntries)
    {
      this.maxEntries = maxEntries;

      if (Path.GetTempPath().Length > 0)
        logFile = Path.GetTempPath();
      else
        logFile = "C:\\SysCAD\\";

      logFile += "SysCAD10.log";

      Initialize();
    }

    public LogView(string logFile)
    {
      this.logFile = logFile;
      Initialize();
    }

    public LogView(int maxEntries, string logFile)
    {
      this.maxEntries = maxEntries;
      this.logFile = logFile;
      Initialize();
    }

    void Initialize()
    {
      InitializeComponent();

      ResourceManager resources = new ResourceManager(typeof(LogView));
      SmallImageList = new ImageList();
      SmallImageList.Images.Add("Note", (Icon)resources.GetObject("Note"));
      SmallImageList.Images.Add("Warning", (Icon)resources.GetObject("Warning"));
      SmallImageList.Images.Add("Error", (Icon)resources.GetObject("Error"));

      if (logFile.Length > 0)
        logStreamWriter = new StreamWriter(logFile, true);
    }

    private delegate void AddMessageDelegate(string msg, MessageType msgType, MessageSource src);

    private void AddMessage(string msg, MessageType msgType, MessageSource src)
    {
      if (InvokeRequired)
      {
        BeginInvoke(new AddMessageDelegate(AddMessage), new object[] { msg, msgType, src });
      }
      else
      {
        string source = "Global";
        if (src != null)
          source = src.Source;

        if (msgType != MessageType.Note)
        {
          ListViewItem lvi = new ListViewItem(new string[] { source, msg }, msgType.ToString());

          lvi.Tag = src;
          Items.Add(lvi);
          while (Items.Count > maxEntries)
            Items.RemoveAt(0);
          EnsureVisible(lvi.Index);
        }

        if (logStreamWriter != null)
        {
          logStreamWriter.WriteLine(DateTime.Now.ToString() + ": " + source + "; " + msgType.ToString() + "; " + msg);
          logStreamWriter.Flush();
        }
      }
    }

    public void Message(string msg, MessageType msgType, MessageSource src)
    {
      AddMessage(msg, msgType, src);
    }

    public void Message(string msg, MessageType msgType)
    {
      MessageSource src = null;
      if (sourceStack.Count != 0)
        src = sourceStack.Peek();
      AddMessage(msg, msgType, src);
    }

    public void SetSource(MessageSource src)
    {
      sourceStack.Push(src);
    }

    public void RemoveSource()
    {
      sourceStack.Pop();
    }

    public bool Active
    {
      get { return this.Enabled; }
      set { this.Enabled = value; }
    }

  }
}