using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Editor.Project
{
    /// <summary>
    /// Логика взаимодействия для WindowDialog.xaml
    /// </summary>
    public partial class WindowDialog : Window
    {
        public WindowDialog()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty SelectedWindowProperty = DependencyProperty.Register("SelectedWindow", typeof(EngineWindow), typeof(WindowDialog), new FrameworkPropertyMetadata(null));
        public EngineWindow SelectedWindow
        {
            get { return (EngineWindow)GetValue(SelectedWindowProperty); }
            set { SetValue(SelectedWindowProperty, value); }
        }

        private void Border_MouseDown(object sender, MouseButtonEventArgs e)
        {
            SelectedWindow = (EngineWindow)((sender as Border).DataContext);
        }
    }
}
