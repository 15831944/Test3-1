using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Color picker control (unfinished).
    /// </summary>
    public class ColorPicker : Control
    {
        private static List<Brush> mColors = new List<Brush>();
        private static Brush[] predefinedBrushes = new Brush[]{
                Brushes.Black, Brushes.Brown, Brushes.OliveDrab, Brushes.DarkGreen, 
                Brushes.DarkTurquoise, Brushes.DarkBlue, Brushes.Indigo, Brushes.DimGray, 
            
                Brushes.DarkRed, Brushes.Orange, Brushes.Gold, Brushes.Green,
                Brushes.Teal, Brushes.Blue, Brushes.SteelBlue, Brushes.Silver,

                Brushes.Red, Brushes.Orange, Brushes.Lime, Brushes.SeaGreen, 
                Brushes.Aqua, Brushes.LightBlue, Brushes.Violet, Brushes.Gray,

                Brushes.Pink, Brushes.Gold, Brushes.Yellow, Brushes.Green, 
                Brushes.Turquoise, Brushes.SkyBlue, Brushes.Plum, Brushes.Gray,

                Brushes.Pink, Brushes.Tan, Brushes.LightYellow, Brushes.LightGreen, 
                Brushes.Turquoise, Brushes.LightBlue, Brushes.Lavender, Brushes.White,
            };
        public static List<Brush> Colors
        {
            get { return ColorPicker.mColors; }
           
        }
        #region Fields
        Popup colorPanelPopup;
        Button chooseButton, moreButton;
        #endregion

        /// <summary>
        /// the ChooseCommand field
        /// </summary>
        private static RoutedCommand mChooseCommand = new RoutedCommand();
        /// <summary>
        /// Gets or sets the ChooseCommand
        /// </summary>
        public static RoutedCommand ChooseCommand
        {
            get { return mChooseCommand; }
            set { mChooseCommand = value; }
        }
        /// <summary>
        /// the MoreCommand field
        /// </summary>
        private static RoutedCommand mMoreCommand = new RoutedCommand();
        /// <summary>
        /// Gets or sets the MoreCommand
        /// </summary>
        public static RoutedCommand MoreCommand
        {
            get { return mMoreCommand; }
            set { mMoreCommand = value; }
        }

        
        static ColorPicker()
        {
            //This OverrideMetadata call tells the system that this element wants to provide a style that is different than its base class.
            //This style is defined in themes\generic.xaml
            DefaultStyleKeyProperty.OverrideMetadata(typeof(ColorPicker), new FrameworkPropertyMetadata(typeof(ColorPicker)));
            for (int i = 0; i < predefinedBrushes.Length; i++)
            {
                mColors.Add(predefinedBrushes[i]);
            }
        }
        public ColorPicker()
            : base()
        {

           
        }
        
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            colorPanelPopup = this.GetTemplateChild("PART_Popup") as Popup;
            chooseButton = this.GetTemplateChild("PART_ChooseButton") as Button;
            moreButton = this.GetTemplateChild("PART_MoreButton") as Button;
            if (chooseButton != null)
                chooseButton.CommandBindings.Add(new CommandBinding(ChooseCommand, new ExecutedRoutedEventHandler(OnChooseClick)));
            if (moreButton != null)
                moreButton.CommandBindings.Add(new CommandBinding(MoreCommand, new ExecutedRoutedEventHandler(OnMoreClick)));
            //if(colorPanelPopup!=null)
            //    colorPanelPopup.BitmapEffect = new 

        }
        public void OnChooseClick(object sender, RoutedEventArgs args)
        {
            //myPopup.CustomPopupPlacementCallback =
            //    new CustomPopupPlacementCallback(placePopup);
            if(colorPanelPopup!=null)
                colorPanelPopup.IsOpen = true;

        }
        public void OnMoreClick(object sender, RoutedEventArgs args)
        {
            MessageBox.Show("An extended color picker will appear here.");
            colorPanelPopup.IsOpen = false;
        }


        public void CloseIt(object sender, RoutedEventArgs e)
        {
            if (colorPanelPopup != null)
                colorPanelPopup.IsOpen = false;
        }
    }
}
