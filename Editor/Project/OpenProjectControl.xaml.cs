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
    /// Логика взаимодействия для OpenProjectControl.xaml
    /// </summary>
    public partial class OpenProjectControl : UserControl
    {
        public OpenProjectControl()
        {
            InitializeComponent();
        }

        private void OnOpenBtnClick(object sender, EventArgs e)
        {
            OpenSelectedProject();
        }

        private void OpenSelectedProject()
        {
            var project = OpenProject.Open(projectsListBox.SelectedItem as ProjectData);
            bool dialog = false;
            var wnd = Window.GetWindow(this);
            if (project != null)
            {
                dialog = true;
                wnd.DataContext = project;
            }
            wnd.DialogResult = dialog;
            wnd.Close();
        }
    }
}
