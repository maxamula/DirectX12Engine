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
                wnd.DataContext = project;
            }
            wnd.DialogResult = dialog;
            wnd.Close();
        }
    }
}
