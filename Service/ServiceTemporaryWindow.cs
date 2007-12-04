using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Protocol;
using System.Collections;
using System.IO;
using System.Runtime.Remoting.Channels;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting;
using SysCAD.Log;
using System.Runtime.Serialization.Formatters.Binary;   
using MindFusion.FlowChartX;
using System.Reflection;

namespace SysCAD.Service
{
  public partial class ServiceTemporaryWindow : Form, ILog
  {
    private ConfigData configData;
    private Dictionary<string, Project> projects = new Dictionary<string,Project>();

    public ServiceTemporaryWindow(string stencilPath, Dictionary<string, Project> projects)
    {
      InitializeComponent();

      addProjectButton.Focus();

      this.Text = "SysCAD.Service (" + Assembly.Load("SysCAD.Service").GetName().Version + ") -- SVN Revision: " + SysCAD.SVNVersion.SVNVersion.version;

      Config config = new Config();
      if (!ServiceConnectCheck(config)) // If service not already there.
      {
        System.Runtime.Remoting.Channels.BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
        serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

        BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

        Hashtable ipcProps = new Hashtable();
        ipcProps["portName"] = "SysCAD.Service";
        ipcProps["exclusiveAddressUse"] = false;
        //ipcProps["typeFilterLevel"] = TypeFilterLevel.Full;
        IpcChannel ipcChannel = new IpcChannel(ipcProps, clientProv, serverProv);
        ChannelServices.RegisterChannel(ipcChannel, false);

        logView.LogFile = System.IO.Path.GetTempPath() + "test.log";

        Config.AddProjectHandler addProjectHandler = new Config.AddProjectHandler(AddProject);
        Config.AddProjectAnywayHandler addProjectAnywayHandler = new Config.AddProjectAnywayHandler(AddProjectAnyway);
        configData = new ConfigData();
        configData.Setup(stencilPath, addProjectHandler, addProjectAnywayHandler);

        RemotingServices.Marshal(configData, "Global");

        Dictionary<string, Project> tempProjects = projects;
        
        foreach (Project project in tempProjects.Values)
        {
          AddProject(project);
        }

        this.projects = projects;
      }
    }

    //~ServiceTemporaryWindow()
    //{
    //  Dispose(false);
    //}

    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        foreach (System.Runtime.Remoting.Channels.IChannel channel in System.Runtime.Remoting.Channels.ChannelServices.RegisteredChannels)
          System.Runtime.Remoting.Channels.ChannelServices.UnregisterChannel(channel);

        if (components != null)
        {
          components.Dispose();
        }
      }

      base.Dispose(disposing);
    }

    private delegate bool TestProjectDelegate(Project project);

    public bool TestProject(Project project)
    {
      if (InvokeRequired)
      {
        return (bool)Invoke(new TestProjectDelegate(TestProject), new object[] { project });
      }
      else
      {
        if (project != null)
        {
          Int64 requestId;
          return project.LoadGraphics(out requestId);
        }
        else return false;
      }
    }

    public bool AddProject(string name, string path)
    {
      return AddProject(new Project(name, path));
    }

    private delegate bool AddProjectDelegate(Project project);

    public bool AddProject(Project project)
    {
      if (InvokeRequired)
      {
        return (bool)Invoke(new AddProjectDelegate(AddProject), new object[] { project });
      }
      else
      {
        if (TestProject(project))
        {
          AddProjectAnyway(project);
          return true;
        }
        else
        {
          return false;
        }
      }
    }

    public void AddProjectAnyway(string name, string path)
    {
      AddProjectAnyway(new Project(name, path));
    }

    private delegate void AddProjectAnywayDelegate(Project project);

    // Add project even though there aren't any 10 files -- for use in syscad9's export.
    // Should really be re-thought once we've sorted how 9 does it's thing.
    public void AddProjectAnyway(Project project)
    {
      if (InvokeRequired)
      {
        Invoke(new AddProjectAnywayDelegate(AddProjectAnyway), new object[] { project });
      }
      else
      {
        configData.ProjectList.Add(project.Name);
        ListViewItem listItem = new ListViewItem(project.Name);
        listItem.ToolTipText = project.Name;
        listItem.SubItems.Add(project.Path);
        projectListView.Items.Add(listItem);
        projects.Add(project.Path, project);

        project.FlowChart = flowChart;
        project.LogView = logView;

        project.ConfigData = configData;

        project.ConnectClientServiceProtocol();
      }
    }

    private static bool ServiceConnectCheck(Config config)
    {
      return (config.TestUrl(new Uri("ipc://SysCAD.Service/Global")));
    }

    //public ServiceTemporaryWindow(String projectPath, String configPath, String stencilPath)
    //{
    //  Project project = new Project(projectPath, System.IO.Path.GetFileNameWithoutExtension(System.IO.Path.GetDirectoryName(projectPath)));

    //  InitializeComponent();

    //  System.Runtime.Remoting.Channels.BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
    //  serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

    //  BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

    //  Hashtable ipcProps = new Hashtable();
    //  ipcProps["portName"] = "SysCAD.Service";
    //  //ipcProps["typeFilterLevel"] = TypeFilterLevel.Full;
    //  IpcChannel ipcChannel = new IpcChannel(ipcProps, clientProv, serverProv);
    //  ChannelServices.RegisterChannel(ipcChannel, false);

    //  logView.LogFile = projectPath + "test.log";

    //  configData = new Config();
    //  configData.GetStencils(stencilPath);
    //  configData.ProjectList.Add(project.Name);
    //  RemotingServices.Marshal(configData, "Global");

    //  project.Graphic = new Graphic();
    //  project.Model = new Model();


    //  Int64 requestId;
    //  project.LoadGraphics(out requestId);

    //  project.ConnectClientServiceProtocol();
    //}




    public void LogMessage(String message, MessageType messageType)
    {
      logView.Message(message, messageType);
    }

    private void LogNote(string message)
    {
      Message(message, MessageType.Note);
      Console.WriteLine(message);
    }

    public void Message(string msg, MessageType msgType, MessageSource src)
    {
      logView.Message(msg, msgType, src);
    }

    public void Message(string msg, MessageType msgType)
    {
      logView.Message(msg, msgType);
    }

    public void SetSource(MessageSource src)
    {
      logView.SetSource(src);
    }

    public void RemoveSource()
    {
      logView.RemoveSource();
    }

    public bool Active
    {
      get { return logView.Enabled; }
      set { logView.Enabled = value; }
    }

    private void projectListView_SelectedIndexChanged(object sender, EventArgs e)
    {
      ListView.SelectedListViewItemCollection selectedItems = projectListView.SelectedItems;

      removeProjectButton.Enabled = (selectedItems.Count > 0) && false; // We're not ready for removing yet.
    }

    private void removeProjectButton_Click(object sender, EventArgs e)
    {

    }

    private void addProjectButton_Click(object sender, EventArgs e)
    {
      AddProjectForm addProjectForm = new AddProjectForm();
      if (addProjectForm.ShowDialog(this) == DialogResult.OK)
      {
        AddProject(new Project(addProjectForm.projectName, addProjectForm.projectPath));
      }
    }
  }
}
