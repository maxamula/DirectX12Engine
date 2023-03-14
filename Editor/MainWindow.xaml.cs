using Editor.Content;
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

namespace Editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Closing += OnMainWindowClosing;
            //ShowProjectDialog();

            // =================== TEST ==================='
            var scene = new Project.Scene();
            var obj = scene.CreateObject();
            var obj2 = obj.CreateObject();
            var obj3 = obj.CreateObject();
            obj.Destroy();
            obj2.Destroy();
            scene.Destroy();
            

            // =================== TEST ==================='
        }

        private void OnMainWindowClosing(object sender, EventArgs e)
        {
            Project.Project.Current?.Unload();
        }

        async private void ShowProjectDialog()
        {
            var projectDialog = new ProjectDialog();    // create project dialog instance
            if (projectDialog.ShowDialog() == false || projectDialog.DataContext == null)
                Application.Current.Shutdown();         // Shutdown application if user clicked exit btn or error 
            else
            {
                Project.Project.Current?.Unload();          // if loading other project - unload old one
                DataContext = projectDialog.DataContext;    // Pass project as context to main window
                // TODO BUILD & LOAD GCDLL async
            }
        }

        private void Window_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.F1)
            {
                Project.ProjectCoreDialog dialog = new Project.ProjectCoreDialog();
                dialog.ShowDialog();
            }
        }
    }
}
