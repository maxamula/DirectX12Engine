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
                var _collection = new ObservableCollection<EngineWindow>() { new EngineWindow() { Name = "g_mainWindow", Title = "My Window" } }; // TEMP
                var parameters = new Dictionary<string, object>()
                {
                    {"windows", new ReadOnlyObservableCollection<EngineWindow>(_collection) }
                };

                var launcherMainCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\Launcher\\main.cpp"));
                GameProject.LauncherMainTemplate launcherTemplate = new GameProject.LauncherMainTemplate();
                launcherTemplate.Session = parameters;
                launcherTemplate.Initialize();
                using (var sw = File.CreateText(launcherMainCpp))
                    sw.Write(launcherTemplate.TransformText());
                GameProject.SolutionManager.AddFiles(System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"{project.Name}.sln")), project.Name, new string[] { launcherMainCpp });

                var assemblyMainH = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\GameAssembly\\assemblymain.h"));
                GameProject.AssemblyMainHTemplate assemblyMainHTemplate = new GameProject.AssemblyMainHTemplate();
                assemblyMainHTemplate.Session = parameters;
                assemblyMainHTemplate.Initialize();
                using (var sw = File.CreateText(assemblyMainH))
                    sw.Write(assemblyMainHTemplate.TransformText());

                var assemblyMainCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\GameAssembly\\assemblymain.cpp"));
                GameProject.AssemblyMainCPPTemplate assemblyMainCppTemplate = new GameProject.AssemblyMainCPPTemplate();
                assemblyMainCppTemplate.Session = parameters;
                assemblyMainCppTemplate.Initialize();
                using (var sw = File.CreateText(assemblyMainCpp))
                    sw.Write(assemblyMainCppTemplate.TransformText());
                GameProject.SolutionManager.AddFiles(System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"{project.Name}.sln")), "GameAssembly", new string[] { assemblyMainCpp, assemblyMainH });

                wnd.DataContext = project;
            }
            wnd.DialogResult = dialog;
            wnd.Close();
        }
    }
}
