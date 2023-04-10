using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Windows;
using System.Windows.Controls;

namespace Editor.Utils
{
    public class DragBox : TextBox
    {
        private bool isDragging = false;
        private double previousX = 0;

        public static readonly DependencyProperty ValueProperty =
            DependencyProperty.Register("Value", typeof(double), typeof(DragBox), new PropertyMetadata(0.0, OnValueChanged));

        public double Value
        {
            get { return (double)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        public DragBox()
        {
            PreviewMouseLeftButtonDown += OnMouseDown;
            PreviewMouseMove += OnMouseMove;
            PreviewMouseLeftButtonUp += OnMouseUp;
            Text = Value.ToString();
            IsReadOnly = true;
        }

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            if (isDragging) // prevent TextBox from entering edit mode while dragging
            {
                e.Handled = true;
                return;
            }
            base.OnMouseLeftButtonDown(e);
        }

        private void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            isDragging = true;
            previousX = e.GetPosition(this).X;
            CaptureMouse();
            IsReadOnly = false;
        }

        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                IsReadOnly = true;
            }
            base.OnPreviewKeyDown(e);
        }

        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            if (isDragging)
            {
                double deltaX = e.GetPosition(this).X - previousX;
                Value += deltaX;
                previousX = e.GetPosition(this).X;
            }
        }

        private void OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            isDragging = false;
            ReleaseMouseCapture();
        }

        private static void OnValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var dragBox = (DragBox)d;
            dragBox.Text = e.NewValue.ToString();
        }
    }
}
