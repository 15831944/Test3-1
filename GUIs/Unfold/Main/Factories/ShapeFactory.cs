using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Shapes;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using Orbifold.Unfold.ShapeLibrary;
using System.Collections.ObjectModel;
using Orbifold.Unfold.Core;

namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Simple shape management class. No reflection mechanism or layers or scene-graph here. 
    /// A lot can be improved in terms of features but that will be done outside this demo application.
    /// </summary>
    static class ShapeFactory
    {
        #region Events

        public static event EventHandler<ShapeEventArgs> OnShapeAdded;
        #endregion

        #region Fields
        private static List<UIElement> elements = new List<UIElement>();
        private static Canvas mCanvas;
        private static Dictionary<ShapeTypes, Type> creators = new Dictionary<ShapeTypes, Type>();
        #endregion

        #region Properties
        public static Canvas Canvas
        {
            get { return mCanvas; }
            set { mCanvas = value; }
        }
        internal static List<UIElement> Shapes
        {
            get { return elements; }
        }
        #endregion


        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        static  ShapeFactory()
        {
            creators.Add(ShapeTypes.Rectangle, typeof(RectangleCreator));
            creators.Add(ShapeTypes.Ellipse, typeof(EllipseCreator));
            creators.Add(ShapeTypes.ClassShape, typeof(ClassShapeCreator));
            
        }
        #endregion
  
       
        public  static void AddShape(Rect rect, ShapeTypes type)
        {
            Connectable shape = null;
           if (creators.ContainsKey(type))
           {
               Type creatingType = creators[type];
               IShapeCreator creator = Activator.CreateInstance(creatingType) as IShapeCreator;

               shape = creator.Create(rect) as Connectable;

               AddShape(shape, type); 
           }
            
        }
        public static void AddShape(Connectable shape, ShapeTypes type)
        {
            if (shape == null)
                throw new ArgumentNullException("A null cannot be added to the shape collection.");
            //the undo trick
            AddShapeCommand cmd = new AddShapeCommand(shape, type, Canvas);
            UndoManager.Execute(cmd);
            RaiseOnShapeAdded(shape);
        }
        private static void RaiseOnShapeAdded(Connectable shape)
        {
            if (OnShapeAdded != null)
                OnShapeAdded(null, new ShapeEventArgs(shape));
        }
        public static bool Exists(UIElement shape)
        {
            return elements.Contains(shape);
        }

        #region Creators

        class RectangleCreator : IShapeCreator
        {

            public UIElement Create(Rect rect)
            {
                SimpleRectangle shape = new SimpleRectangle();
                shape.Width = rect.Width;
                shape.Height = rect.Height;
                shape.Background = Brushes.LightYellow;               
                Canvas.SetLeft(shape, rect.X);
                Canvas.SetTop(shape, rect.Y);
                shape.Text = "The shadow effect is a killer and a software rendering process unless you are running Vista. You can use the font formatting toolbar buttons to change the appearance of this text. There are so many possibilities, this box is just too small to highlight it all.";
                return shape; 
            }
        }
        class EllipseCreator : IShapeCreator
        {

            public UIElement Create(Rect rect)
            {
                SimpleEllipse shape = new SimpleEllipse();
                shape.Width = rect.Width;
                shape.Height = rect.Height;
                shape.Background = Brushes.LemonChiffon;
                Canvas.SetLeft(shape, rect.X);
                Canvas.SetTop(shape, rect.Y);
                shape.Text = "The shadow effect is a killer and a software rendering process unless you are running Vista. You can use the font formatting toolbar buttons to change the appearance of this text. There are so many possibilities, this box is just too small to highlight it all.";
                return shape;
            }
        }
        class ClassShapeCreator : IShapeCreator
        {

            private Random rnd = new Random();

            public UIElement Create(Rect rect)
            {
                ClassShape shape = new ClassShape();
                //shape.Width = rect.Width;
                //shape.Height = rect.Height;
                shape.ShapeColor = RandomLowSaturationColor;
                
               
                Canvas.SetLeft(shape, rect.X);
                Canvas.SetTop(shape, rect.Y);
                return shape; 
            }
            /// <summary>
            /// Gets a random color from the whole available color spectrum.
            /// </summary>
            /// <value>The random color.</value>
            ///<example file="ArtPallet.RandomColor.xml">          
            ///</example>
            public  Color RandomColor
            {
                get
                {
                    byte[] b = new byte[3];
                    rnd.NextBytes(b);
                    return Color.FromArgb(255, b[0],b[1],b[2]);
                }
            }
            /// <summary>
            /// Gets a random blue color.
            /// <remarks>You can generate any variation of a certain color range by specifying the HSV range and a utility function will convert it to 
            /// an RGB value (<see cref="Utils.HSL2RGB"/>).
            /// </remarks>
            /// </summary>
            /// <value>The random blues.</value>
            public  Color RandomBlues
            {
                get
                {
                    return (Color)Utils.HSL2RGB((rnd.NextDouble() * 20D + 150D) / 255D, (rnd.NextDouble() * 150D + 100D) / 255D, (rnd.NextDouble() * 50D + 150D) / 255D);
                }
            }
            /// <summary>
            /// Gets a random low saturation color.
            /// </summary>
            /// <value>The random color.</value>
            public  Color RandomLowSaturationColor
            {
                get
                {
                    return (Color)Utils.HSL2RGB((rnd.NextDouble() * 255D) / 255D, (rnd.NextDouble() * 20D + 30D) / 255D, (rnd.NextDouble() * 20D + 130D) / 255D);
                }
            }

        }
       


        #endregion

        #region Commands
        class AddShapeCommand : ICommand
        {
            private Canvas mCanvas;

            public Canvas Canvas
            {
                get { return mCanvas; }
                set { mCanvas = value; }
            }

            private ShapeTypes mShapeType;

	        public ShapeTypes ShapeType
	        {
		        get { return mShapeType;}
		        set { mShapeType = value;}
	        }
            private UIElement mShape;

            public UIElement Shape
	        {
		        get { return mShape;}
		        set { mShape = value;}
	        }


            #region Constructor
            public AddShapeCommand(UIElement shape, ShapeTypes type, Canvas canvas)
            {
                mShapeType = type;
                mShape = shape;
                mCanvas = canvas;
            }
            #endregion


            public void Undo()
            {
                Canvas.Children.Remove(mShape);
                   ShapeFactory.Shapes.Remove(mShape);
                   SelectionManager.UnselectAll();
            }

            public void Redo()
            {
                if (mShape != null)
               {
                   Canvas.Children.Add(mShape);
                    
                   ShapeFactory.Shapes.Add(mShape);
                   SelectionManager.SelectShape(mShape);
               }
            }

            public string Text
            {
                get
                {
                    return "Shape added [" + Enum.GetName(typeof(ShapeTypes), this.ShapeType) + "]";
                }
                set
                {
                    //?necessary
                }
            }

        }

        #endregion
    }


}
