
using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Collections;

namespace SysCAD.Protocol
{

  [Serializable]
  internal class Animation
  {

    private String actions = null; // can't use XmlDocument because it's not serializable.

    [NonSerialized]
    private XmlDocument actionsXmlDoc = null;
    private ArrayList decorations = null;
    private ArrayList elements = null;

    public Animation()
    {
      Actions = String.Empty;
      Elements = new ArrayList();
      Decorations = new ArrayList();
    }

    public Animation(String actions, ArrayList elements, ArrayList decorations)
    {
      Actions = actions;
      Elements = elements;
      Decorations = decorations;
    }

    public String Actions
    {
      get { return actions; }
      set
      {
        actions = value;
        actionsXmlDoc.LoadXml(actions);
      }
    }

    public ArrayList Decorations
    {
      get { return decorations; }
      set { decorations = value; }
    }

    public ArrayList Elements
    {
      get { return elements; }
      set { elements = value; }
    }
  }
}
