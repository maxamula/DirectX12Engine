using Editor.Project;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Editor.Controls
{
    /// <summary>
    /// Логика взаимодействия для WindowManagerPage.xaml
    /// </summary>
    public partial class WindowManagerPage : EditorPage
    {
        public WindowManagerPage()
        {
            InitializeComponent();
            this.Unloaded += OnUnloaded;
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            (this.DataContext as Project.Project).UpdateGPFiles();
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var index = (sender as ComboBox).SelectedIndex;
            var project = this.DataContext as Project.Project;
            SelectedWindow = project.Windows[index];
            OnPropertyChanged(new DependencyPropertyChangedEventArgs(SelectedWindowProperty, null, SelectedWindow));
        }

        private void OnAddWndClick(object sender, RoutedEventArgs e)
        {
            (this.DataContext as Project.Project).AddWindow("Window");
        }

        private void OnAddWndProcClick(object sender, RoutedEventArgs e)
        {
            var project = this.DataContext as Project.Project;
            string userInput = Microsoft.VisualBasic.Interaction.InputBox("Enter window procedure name:", "New string", "DefaultWndProc");
            if (string.IsNullOrEmpty(userInput))
                return;
            project.AddWndProc(userInput);
        }

        public static readonly DependencyProperty SelectedWindowProperty = DependencyProperty.Register("SelectedWindow", typeof(EngineWindow), typeof(WindowManagerPage), new FrameworkPropertyMetadata(null));
        public EngineWindow SelectedWindow
        {
            get { return (EngineWindow)GetValue(SelectedWindowProperty); }
            set { SetValue(SelectedWindowProperty, value); }
        }

        private void LostFocus(object sender, RoutedEventArgs e)
        {
            (this.DataContext as Project.Project).UpdateGPFiles();
        }
    }
}
