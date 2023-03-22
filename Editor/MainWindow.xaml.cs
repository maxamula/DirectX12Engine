using Editor.Project;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
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
using System.Diagnostics;
using System.CodeDom.Compiler;
using Editor.GameProject;
using EnvDTE;

namespace Editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : HandyControl.Controls.Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Closing += OnMainWindowClosing;
            ShowProjectDialog();

            // =================== TEST ===================

            // =================== TEST ===================
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
                Project.Project project = (Project.Project)projectDialog.DataContext;
                DataContext = project;    // Pass project as context to main window
                // BUILD & LOAD GCDLL async
                await Task.Run(() =>
                {
                    GameProject.SolutionManager.Build(project, Project.Project.GetConfigName(project.DllConfig), new Action(() =>
                    {
                        // Launch Render thread
                    }));
                });
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
