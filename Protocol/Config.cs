
using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.Remoting.Lifetime;
using System.Runtime.Remoting.Channels;
using System.Collections;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Remoting.Channels.Tcp;

//using System.Security.Permissions;

namespace SysCAD.Protocol
{

  public class ConfigData : MarshalByRefObject
  {

    private Dictionary<String, GraphicStencil> graphicStencils = new Dictionary<String, GraphicStencil>();
    private Dictionary<String, ModelStencil> modelStencils = new Dictionary<String, ModelStencil>();

    private StringCollection projectList = new StringCollection();

    public delegate bool AddProjectHandler(string name, string path);
    public delegate void AddProjectAnywayHandler(string name, string path);

    public AddProjectHandler addProject;
    public AddProjectAnywayHandler addProjectAnyway;

    public void Setup(string stencilPath, Config.AddProjectHandler addProject, Config.AddProjectAnywayHandler addProjectAnyway)
    {
      this.addProject = addProject;
      this.addProjectAnyway = addProjectAnyway;

      {
        //int iStencil = 0;
        //LogNote("Srvr : 0 : ModelStencils:");
        String[] files = Directory.GetFiles(stencilPath, "*.modelstencil");
        for (int i = 0; i < files.GetLength(0); i++)
        {
          String fullpath = files[i];

          //try
          //{
          ModelStencil modelStencil = ModelStencil.Deserialize(fullpath);

          if (ConfirmModelStencil(modelStencil))
          {
            TrimAnchorPoints(modelStencil);
            ModelStencils.Add(System.IO.Path.GetFileNameWithoutExtension(fullpath), modelStencil);
          }

          //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
          //LogNote("Srvr : 0 : " + iStencil++ + " : " + System.IO.Path.GetFileNameWithoutExtension(fullpath));
          //}
          //catch (Exception)
          //{
          //  int asdf = 0;
          //  //Message("Error '" + e.Message + "' loading ModelStencil: " + fullpath, MessageType.Error);
          //}
        }

        Console.WriteLine("\n");
      }

      {
        //int iStencil = 0;
        //LogNote("Srvr : 0 : GraphicStencils:");
        String[] files = Directory.GetFiles(stencilPath, "*.graphicstencil");
        for (int i = 0; i < files.GetLength(0); i++)
        {
          String fullpath = files[i];

          //try
          //{
          GraphicStencil graphicStencil = GraphicStencil.Deserialize(fullpath);

          foreach (String tag in graphicStencil.Tags)
          {
            GraphicStencils.Add(tag, graphicStencil);
          }
          //}
          //catch
          //{
          //  int adsf = 0;
          //  //logNote("Config : Error loading GraphicStencil " + fullpath);
          //}
        }
      }
    }

    bool ConfirmModelStencil(ModelStencil modelstencil)
    {
      // TODO: check whether this stencil is to be included in the project.
      return true;
    }

    void TrimAnchorPoints(ModelStencil modelStencil)
    {
      ArrayList anchors = modelStencil.Anchors;
      ArrayList validAnchors = new ArrayList();
      for (int i = 0; i < anchors.Count; i++)
      {
        Anchor anchor = (Anchor)(anchors[i]);
        if (ValidAnchor(anchor))
        {
          validAnchors.Add(anchor);
        }
      }

      modelStencil.Anchors = validAnchors;
    }

    bool ValidAnchor(Anchor anchor)
    {
      // TODO: Check here if anchor.Tag is to be included. (and possibly in future anchor.Type)
      return true;
    }

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public Dictionary<String, GraphicStencil> GraphicStencils
    {
      get { return graphicStencils; }
      set { this.graphicStencils = value; }
    }

    public Dictionary<String, ModelStencil> ModelStencils
    {
      get { return modelStencils; }
      set { this.modelStencils = value; }
    }

    public StringCollection ProjectList
    {
      get { return projectList; }
      set { projectList = value; }
    }
  }

  public class Config : ConfigData
  {
    public String connectionError = "";

    private ConfigData remoteConfig;

    public void GetProjectList()
    {
      ProjectList = remoteConfig.ProjectList;
    }

    public bool RemoteAddProject(string name, string path)
    {
      return remoteConfig.addProject(name, path);
    }

    public void RemoteAddProjectAnyway(string name, string path)
    {
      remoteConfig.addProjectAnyway(name, path);
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

      ProjectList = remoteConfig.ProjectList;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ModelStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ModelStencils = bf.Deserialize(memoryStream) as Dictionary<String, ModelStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.GraphicStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      GraphicStencils = bf.Deserialize(memoryStream) as Dictionary<String, GraphicStencil>;

      //addProject += remoteConfig.addProject;
    }

    public bool TestUrl(Uri url)
    {
      try
      {
        remoteConfig = Activator.GetObject(typeof(ConfigData), url.ToString()) as ConfigData;
        StringCollection test = remoteConfig.ProjectList; // Check that the connection is up.
        return (remoteConfig != null);
      }
      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        String connectionError = remotingException.Message;
        return false;
      }
    }
  }
}
