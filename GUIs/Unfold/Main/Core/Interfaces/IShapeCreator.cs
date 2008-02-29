using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Shapes;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
namespace Orbifold.Unfold.Main
{
    interface IShapeCreator
    {       
        UIElement Create(Rect rec);
    }
}
