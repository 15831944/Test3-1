
using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Internal representation of a link in the model.
  /// </summary>
  [Serializable]
  public class ModelLink : ModelItem
  {
    private LinkClass linkClass;

    private Guid origin;
    private Guid destination;
    private String originPort;
    private String destinationPort;

    public ModelLink(Guid guid, String tag, LinkClass linkClass, Guid origin, Guid destination, String originPort, String destinationPort)
    {
      this.Guid = guid;
      this.Tag = tag;

      this.linkClass = linkClass;

      this.origin = origin;
      this.destination = destination;

      this.originPort = originPort;
      this.destinationPort = destinationPort;
    }

    public LinkClass LinkClass
    {
      get { return linkClass; }
      set { linkClass = value; }
    }

    public Guid Origin
    {
      get { return origin; }
      set { origin = value; }
    }

    public Guid Destination
    {
      get { return destination; }
      set { destination = value; }
    }

    public String OriginPort
    {
      get { return originPort; }
      set { originPort = value; }
    }

    public String DestinationPort
    {
      get { return destinationPort; }
      set { destinationPort = value; }
    }
  }
}
