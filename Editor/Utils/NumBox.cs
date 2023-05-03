using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Markup;

namespace Editor.Utils
{
    [TemplatePart(Name = "PART_textblock", Type = typeof(TextBlock))]
    [TemplatePart(Name = "PART_textbox", Type = typeof(TextBox))]
    public class NumBox : Control
    {
        private double _origValue;
        private double _newValue;
        private bool _captured = false;
        private bool _valueChanged = false;
        private double _initialX;
        private double _step;

        public NumBox()
        {
            Loaded += NumBox_Loaded;
        }

        public static readonly RoutedEvent ValueChangedEvent = EventManager.RegisterRoutedEvent(nameof(ValueChanged), RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(NumBox));
        public event RoutedEventHandler ValueChanged
        {
            add => AddHandler(ValueChangedEvent, value);
            remove => RemoveHandler(ValueChangedEvent, value);
        }

        private void NumBox_Loaded(object sender, RoutedEventArgs e)
        {
            double.TryParse(Value, NumberStyles.Float, CultureInfo.InvariantCulture, out _origValue);
            double.TryParse(Step, out _step);
        }

        public string Value
        {
            get => (string)GetValue(ValueProperty);
            set => SetValue(ValueProperty, value);
        }
        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register(nameof(Value), typeof(string), typeof(NumBox), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, new PropertyChangedCallback(OnValueChanged)));

        private static void OnValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            (d as NumBox).RaiseEvent(new RoutedEventArgs(ValueChangedEvent));
        }

        public string Step
        {
            get => (string)GetValue(StepProperty);
            set => SetValue(StepProperty, value);
        }
        public static readonly DependencyProperty StepProperty = DependencyProperty.Register(nameof(Step), typeof(string), typeof(NumBox), new FrameworkPropertyMetadata("1,0", FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            if (GetTemplateChild("PART_textblock") is TextBlock textBlock)
            {
                textBlock.MouseLeftButtonDown += OnTextBlock_Mouse_LBD;
                textBlock.MouseLeftButtonUp += OnTextBlock_Mouse_LBU;
                textBlock.MouseMove += OnTextBlock_Mouse_Move;
            }   
        }
        private void OnTextBlock_Mouse_Move(object sender, MouseEventArgs e)
        {
            if (_captured)
            {
                bool altPressed = Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt);
                double stepFactor = altPressed ? 5.0 : 1.0;

                var currentX = e.GetPosition(this).X;
                var deltaX = currentX - _initialX;
                var deltaValue = Math.Round(deltaX / 1.0) * _step * stepFactor;
                var newDragValue = _origValue + deltaValue;
                if (Math.Abs(newDragValue - _newValue) >= _step)
                {
                    _newValue = newDragValue;
                    Value = _newValue.ToString("0.###",CultureInfo.InvariantCulture);
                    _valueChanged = true;
                }
            }
        }

        private void OnTextBlock_Mouse_LBU(object sender, MouseButtonEventArgs e)
        {
            if(_captured)
            {
                Mouse.Capture(null);
                _captured = false;
                _origValue = _newValue;
                e.Handled = true;
                if(!_valueChanged && GetTemplateChild("PART_textbox") is TextBox textBox)
                {
                    textBox.Visibility = Visibility.Visible;
                    textBox.Focus();
                    textBox.SelectAll();
                }
            }
        }

        private void OnTextBlock_Mouse_LBD(object sender, MouseButtonEventArgs e)
        {
            double.TryParse(Value, NumberStyles.Float, CultureInfo.InvariantCulture, out _origValue);
            Mouse.Capture(sender as UIElement);
            _captured = true;
            _valueChanged = false;
            e.Handled = true;
            _initialX = e.GetPosition(this).X;
        }

        static NumBox()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(NumBox), new System.Windows.FrameworkPropertyMetadata(typeof(NumBox)));
        }
    }
}
