using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows;
using System.Windows.Threading;

namespace Editor.Utils
{
    public class EditableTextBlock : TextBlock
    {
        private TextBox _editBox;
        private DispatcherTimer _timer;

        public EditableTextBlock()
        {
            this.MouseLeftButtonDown += EditableTextBlock_MouseLeftButtonDown;
        }

        private void EditableTextBlock_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (_timer == null)
            {
                _timer = new DispatcherTimer();
                _timer.Interval = TimeSpan.FromMilliseconds(300);
                _timer.Tick += Timer_Tick;
                _timer.Start();
            }
            else
            {
                _timer.Stop();
                _timer = null;
                EnterEditMode();
            }
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            _timer.Stop();
            _timer = null;
        }

        private void EnterEditMode()
        {
            if (_editBox == null)
            {
                _editBox = new TextBox();
                _editBox.Text = this.Text;
                _editBox.HorizontalAlignment = this.HorizontalAlignment;
                _editBox.VerticalAlignment = this.VerticalAlignment;
                _editBox.FontFamily = this.FontFamily;
                _editBox.FontSize = this.FontSize;
                _editBox.FontStyle = this.FontStyle;
                _editBox.FontWeight = this.FontWeight;
                _editBox.BorderThickness = new Thickness(1);
                _editBox.BorderBrush = Brushes.Black;
                _editBox.Focusable = true;
                _editBox.Foreground = Brushes.Black;
                _editBox.Background = Brushes.White;
                //_editBox.Width = this.ActualWidth;
                _editBox.Padding = new Thickness(0);
                _editBox.PreviewKeyDown += EditBox_PreviewKeyDown;
                this.Visibility = Visibility.Collapsed;
                _editBox.Focus();
                var parent = VisualTreeHelper.GetParent(this) as Panel;
                if (parent != null)
                {
                    parent.Children.Add(_editBox);
                    _editBox.Focus();
                }
            }
        }

        private void ExitEditMode()
        {
            this.Text = _editBox.Text;
            this.Visibility = Visibility.Visible;
            var parent = VisualTreeHelper.GetParent(_editBox) as Panel;
            if (parent != null)
            {
                parent.Children.Remove(_editBox);
                _editBox = null;
            }
        }

        private void EditBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                ExitEditMode();
            }
        }

    }
}
