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
    [TemplatePart(Name = "PART_textblock", Type = typeof(TextBlock))]
    [TemplatePart(Name = "PART_textbox", Type = typeof(TextBox))]
    public class EditBox : Control
    {
        private DispatcherTimer _timer;
        public string Value
        {
            get => (string)GetValue(ValueProperty);
            set => SetValue(ValueProperty, value);
        }
        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register(nameof(Value), typeof(string), typeof(EditBox), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            if (GetTemplateChild("PART_textblock") is TextBlock textBlock)
            {
                textBlock.MouseLeftButtonDown += OnTextBlock_Mouse_LBD;
            }
        }

        private void OnTextBlock_Mouse_LBD(object sender, MouseButtonEventArgs e)
        {
            if (_timer == null)
            {
                _timer = new DispatcherTimer();
                _timer.Interval = TimeSpan.FromMilliseconds(200);
                _timer.Tick += Timer_Tick;
                _timer.Start();
            }
            else
            {
                e.Handled = true;
                _timer.Stop();
                _timer = null;
                if (GetTemplateChild("PART_textbox") is TextBox textBox)
                {
                    textBox.Visibility = Visibility.Visible;
                    textBox.Focus();
                    textBox.SelectAll();
                }
            }
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            _timer.Stop();
            _timer = null;
        }
        static EditBox()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(EditBox), new System.Windows.FrameworkPropertyMetadata(typeof(EditBox)));
        }
    }
}
