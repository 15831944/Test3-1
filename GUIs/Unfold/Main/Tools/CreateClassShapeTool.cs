using System;
using System.Collections.Generic;
using System.Text;
using Orbifold.Unfold.ShapeLibrary;
using System.Windows.Media;
using System.Windows;
namespace Orbifold.Unfold.Main
{
    class CreateClassShapeTool : ToolBase
    {
        public override void OnExecute(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
        {           
            ShapeFactory.AddShape(new Rect(new Point(10, 10), new Size(200, 55)), ShapeTypes.ClassShape);
        }

        public CreateClassShapeTool()
            : base("Create ClassShape")
        { }
    }
}
