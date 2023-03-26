using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
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

namespace Editor.Project
{
    /// <summary>
    /// Логика взаимодействия для CreateProjectControl.xaml
    /// </summary>
    public partial class CreateProjectControl : UserControl
    {
        public CreateProjectControl()
        {
            InitializeComponent();
        }

        private void OnCreateBtnClick(object sendr, EventArgs e)
        {
            var context = DataContext as NewProject;
            var projectPath = context.CreateProject(templateList.SelectedItem as ProjectTemplate);
            bool dialog = false;
            var wnd = Window.GetWindow(this);
            if (!string.IsNullOrEmpty(projectPath))
            {
                dialog = true;
                var project = OpenProject.Open(new ProjectData() { ProjectName = context.ProjectName, ProjectPath = $@"{context.ProjectPath}{context.ProjectName}\" });

                // Create initial source files for project
                project.UpdateGPFiles();

                var launcherMainCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\Launcher\\main.cpp"));
                var assemblyMainH = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\GameAssembly\\assemblymain.h"));
                var assemblyMainCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\GameAssembly\\assemblymain.cpp"));
                GameProject.SolutionManager.AddFiles(System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"{project.Name}.sln")), project.Name, new string[] { launcherMainCpp });
                GameProject.SolutionManager.AddFiles(System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"{project.Name}.sln")), "GameAssembly", new string[] { assemblyMainCpp, assemblyMainH });

                wnd.DataContext = project;
            }
            wnd.DialogResult = dialog;
            wnd.Close();
        }
    }
}
