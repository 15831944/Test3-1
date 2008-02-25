
using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;

using System.Drawing;
using System.Collections.Generic;
using System.Drawing.Drawing2D;
using System.IO;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for ThingStencil.
  /// </summary>
  [Serializable]
  public class ThingStencil
  {

    private String tag;
    private Size defaultSize = new Size(10.0, 10.0);
    private String xaml;

    public ThingStencil()
    {
    }

    public override string ToString()
    {
      return tag;
    }

    public Size DefaultSize
    {
      get { return defaultSize; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public String Xaml
    {
      get { return xaml; }
      set { xaml = value; }
    }

    public static void Serialize(string path, ThingStencil thingStencil)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamWriter streamWriter = new StreamWriter(path);
      Stream stream = streamWriter.BaseStream;

      SurrogateSelector ss = new SurrogateSelector();
      ss.AddSurrogate(typeof(ThingStencil), new StreamingContext(StreamingContextStates.All), new GraphicStencilSerializationSurrogate());
      sf.SurrogateSelector = ss;

      sf.Serialize(stream, thingStencil);
      stream.Close();
    }

    public static ThingStencil Deserialize(String path)
    {
      ThingStencil thingStencil;
      Stream stream = null;

      //try
      {
        SoapFormatter sf = new SoapFormatter();
        StreamReader streamReader = new StreamReader(path);
        stream = (streamReader).BaseStream;

        SurrogateSelector ss = new SurrogateSelector();
        ss.AddSurrogate(typeof(ThingStencil), new StreamingContext(StreamingContextStates.All), new GraphicStencilSerializationSurrogate());
        sf.SurrogateSelector = ss;

        thingStencil = (ThingStencil)sf.Deserialize(stream);
        stream.Close();
        Serialize(path, thingStencil);
      }
      //catch
      //{
      //  stream.Close();

      //  SoapFormatter sf = new SoapFormatter();
      //  StreamReader streamReader = new StreamReader(path);
      //  stream = (streamReader).BaseStream;

      //  graphicStencil = (GraphicStencil)sf.Deserialize(stream);
      //  stream.Close();
      //  graphicStencil.tag = Path.GetFileNameWithoutExtension(path);
      //  Serialize(path, graphicStencil);
      //}

      return thingStencil;
    }
  }
}
