using System;
using System.Collections.Generic;
using System.Text;

[assembly: CLSCompliant(true)]
namespace SysCAD.Log
{
  public enum MessageType { Error, Warning, Note };

  public class MessageSource
  {
    public MessageSource(string _Source)
    { Source = _Source; }
    public string Source;
    public virtual void DoCallback()
    { }
  }

  /// <summary>
  /// A Double threaded log. If used on multiple threads, each thread should have its own ILog object.
  /// </summary>
  public interface ILog
  {
    /// <summary>
    /// Notes a message in the log
    /// </summary>
    /// <param name="msg">The main information of the message</param>
    /// <param name="msgType">The message type</param>
    /// <param name="src">The source of the message. If <value>null</value>, message uses last source from SetSource</param>
    void Message(string msg, MessageType msgType, MessageSource src);

    /// <summary>
    /// Notes a message in the log
    /// </summary>
    /// <param name="msg">The main information of the message</param>
    /// <param name="msgType">The message type</param>
    void Message(string msg, MessageType msgType);

    /// <summary>
    /// Sets the default message source. This should be implemented as a stack.
    /// </summary>
    /// <param name="src"></param>
    void SetSource(MessageSource src);

    /// <summary>
    /// Sets the default message source to be the new source on the stack.
    /// </summary>
    void RemoveSource();

    bool Active
    {
      get;
      set;
    }
  }
}
