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

    public LogView()
    {
      InitializeComponent();

      ResourceManager resources = new ResourceManager(typeof(LogView));
      SmallImageList = new ImageList();
      SmallImageList.Images.Add("Note", (Icon)resources.GetObject("Note"));
      SmallImageList.Images.Add("Warning", (Icon)resources.GetObject("Warning"));
      SmallImageList.Images.Add("Error", (Icon)resources.GetObject("Error"));
    }

    private void AddMessage(string msg, MessageType msgType, MessageSource src)
    {
      string source = "Global";
      if (src != null)
        source = src.Source;

      ListViewItem lvi = new ListViewItem(new string[] { source, msg }, msgType.ToString());

      lvi.Tag = src;
      Items.Add(lvi);
      EnsureVisible(lvi.Index);
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