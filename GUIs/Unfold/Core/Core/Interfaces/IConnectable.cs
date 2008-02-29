using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Orbifold.Unfold.Core
{
    /// <summary>
    /// Interface to connectable diagram entities.
    /// </summary>
    public interface IConnectable
    {
        void BindConnection(Connection con, DependencyProperty property, ConnectorLocation location);
        bool AnimationEnabled { get;set;}
    }
}
