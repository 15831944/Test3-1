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
using System.Windows.Shapes;
using System.Diagnostics;
using System.Globalization;
using System.Windows.Threading;
using System.Windows.Media.Animation;
using System.ComponentModel;
using System.Windows.Media.Effects;
namespace Orbifold.Unfold.Core
{
    /// <summary>
    /// Utility class with diverse WPF effects
    /// See http://www.codeplex.com/AnimationBehaviors for up-to-date versions of this code.
    /// </summary>
    public static class Effects
    {
        public static BitmapEffect ShadowEffect
        {
            get {
                DropShadowBitmapEffect shadow = new DropShadowBitmapEffect();
                shadow.Color = Colors.Black;
                shadow.Direction = -45;
                shadow.ShadowDepth = 5;
                shadow.Softness = 0.3;
                shadow.Opacity = 0.5;
                return shadow;
            }
        }
        public static void ApplyZoomInRotate(FrameworkElement fe, Duration duration, Duration delay)
        {
            if (delay.TimeSpan == TimeSpan.Zero)
            {
                DoubleAnimation da1 = new DoubleAnimation(0.0, 1.0, duration);
                da1.AccelerationRatio = da1.DecelerationRatio = 0.2;

                DoubleAnimation da2 = new DoubleAnimation(0.0, 360.0, duration);
                da2.AccelerationRatio = da2.DecelerationRatio = 0.2;

                TransformGroup tg = new TransformGroup();
                tg.Children.Add(new ScaleTransform(1, 1));
                tg.Children.Add(new RotateTransform(0));

                tg.Children[0].BeginAnimation(ScaleTransform.ScaleXProperty, da1);
                tg.Children[0].BeginAnimation(ScaleTransform.ScaleYProperty, da1);

                tg.Children[1].BeginAnimation(RotateTransform.AngleProperty, da2);

                fe.RenderTransformOrigin = new Point(0.5, 0.5);
                fe.RenderTransform = tg;
            }
            else
            {
                DoubleAnimationUsingKeyFrames da1 = new DoubleAnimationUsingKeyFrames();
                da1.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(TimeSpan.Zero)));
                da1.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(delay.TimeSpan)));
                da1.KeyFrames.Add(new LinearDoubleKeyFrame(1.0, KeyTime.FromTimeSpan(delay.TimeSpan + duration.TimeSpan)));
                da1.Duration = delay + duration;
                da1.AccelerationRatio = da1.DecelerationRatio = 0.2;

                DoubleAnimationUsingKeyFrames da2 = new DoubleAnimationUsingKeyFrames();
                da2.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(TimeSpan.Zero)));
                da2.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(delay.TimeSpan)));
                da2.KeyFrames.Add(new LinearDoubleKeyFrame(360.0, KeyTime.FromTimeSpan(delay.TimeSpan + duration.TimeSpan)));
                da2.Duration = delay + duration;
                da2.AccelerationRatio = da2.DecelerationRatio = 0.2;

                TransformGroup tg = new TransformGroup();
                tg.Children.Add(new ScaleTransform(1, 1));
                tg.Children.Add(new RotateTransform(0));

                tg.Children[0].BeginAnimation(ScaleTransform.ScaleXProperty, da1);
                tg.Children[0].BeginAnimation(ScaleTransform.ScaleYProperty, da1);

                tg.Children[1].BeginAnimation(RotateTransform.AngleProperty, da2);

                fe.RenderTransformOrigin = new Point(0.5, 0.5);
                fe.RenderTransform = tg;
            }
        }
        public static void Explode(FrameworkElement fe, Duration duration, double centerX, double centerY)
        {
            Storyboard sb = new Storyboard();
            DoubleAnimation ao = new DoubleAnimation(0.0, 1.0, new Duration(new TimeSpan(0, 0, 4)));
            //Storyboard.SetTargetName(ao, fe.Name);
            Storyboard.SetTargetProperty(ao, new PropertyPath(FrameworkElement.OpacityProperty));
            sb.Children.Add(ao);

            DoubleAnimation al = new DoubleAnimation(centerX, Canvas.GetLeft(fe), new Duration(new TimeSpan(0, 0, 4)));
            //Storyboard.SetTargetName(ao, fe.Name);
            Storyboard.SetTargetProperty(al, new PropertyPath(Canvas.LeftProperty));
            sb.Children.Add(al);

            DoubleAnimation at = new DoubleAnimation(centerY, Canvas.GetTop(fe), new Duration(new TimeSpan(0, 0, 4)));
            //Storyboard.SetTargetName(ao, fe.Name);
            Storyboard.SetTargetProperty(at, new PropertyPath(Canvas.TopProperty));
            sb.Children.Add(at);

            sb.Begin(fe);

        }
        public static void ApplyFadeIn(FrameworkElement fe, Duration duration, Duration delay)
        {
            if (delay.TimeSpan == TimeSpan.Zero)
            {
                DoubleAnimation da = new DoubleAnimation(0.0, 1.0, duration);
                fe.BeginAnimation(FrameworkElement.OpacityProperty, da);
            }
            else
            {
                DoubleAnimationUsingKeyFrames da = new DoubleAnimationUsingKeyFrames();
                da.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(TimeSpan.Zero)));
                da.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(delay.TimeSpan)));
                da.KeyFrames.Add(new LinearDoubleKeyFrame(1.0, KeyTime.FromTimeSpan(delay.TimeSpan + duration.TimeSpan)));
                da.Duration = delay + duration;
                fe.BeginAnimation(FrameworkElement.OpacityProperty, da);
            }
        }

        public static void ApplyFadeOut(FrameworkElement fe, Duration duration, Duration delay)
        {
            if (delay.TimeSpan == TimeSpan.Zero)
            {
                DoubleAnimation da = new DoubleAnimation(1.0, 0.0, duration);
                fe.BeginAnimation(FrameworkElement.OpacityProperty, da);
            }
            else
            {
                DoubleAnimationUsingKeyFrames da = new DoubleAnimationUsingKeyFrames();
                da.KeyFrames.Add(new LinearDoubleKeyFrame(1.0, KeyTime.FromTimeSpan(TimeSpan.Zero)));
                da.KeyFrames.Add(new LinearDoubleKeyFrame(1.0, KeyTime.FromTimeSpan(delay.TimeSpan)));
                da.KeyFrames.Add(new LinearDoubleKeyFrame(0.0, KeyTime.FromTimeSpan(delay.TimeSpan + duration.TimeSpan)));
                da.Duration = delay + duration;
                fe.BeginAnimation(FrameworkElement.OpacityProperty, da);
            }
        }

    }
}
